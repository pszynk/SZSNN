/**
 * \file SpikeBuffer.cpp
 *
 * \date 29-12-2012
 * \author pawel
 * \description
 */

#include "SpikeBuffer.hpp"

namespace szsnn {

void SpikeBuffer::specUploadToDevice()
{
	getContext()->writeBuffer(firedNeurons, CL_TRUE);
	getContext()->writeBuffer(firedCounts, CL_TRUE);
	getContext()->writeBuffer(firedOutput, CL_TRUE);
}
void SpikeBuffer::specDownloadFromDevice()
{
	getContext()->readBuffer(firedNeurons, CL_TRUE);
	getContext()->readBuffer(firedCounts, CL_TRUE);
	getContext()->readBuffer(firedOutput, CL_TRUE);


}
void SpikeBuffer::specFreeHostData()
{
	firedNeurons.freeHostData();
	firedCounts.freeHostData();
	firedOutput.freeHostData();
}

void SpikeBuffer::initialize(std::string update_kernel_name, std::string inputs_kernel_name)
//TODO  update_kernel_name jest głupim argumentem, najlepiej brać te nazwy z Context
{

	size_t firedNeuronsSize =	totalPartitionCount *
								MAX_PARTITION_SIZE;

	std::vector<std::string> firedNeuronsKernelNames;
	firedNeuronsKernelNames.push_back("scatter");
	firedNeuronsKernelNames.push_back(update_kernel_name);
	if(inputs_kernel_name != "")
	{
		firedNeuronsKernelNames.push_back(inputs_kernel_name);
	}

	firedNeurons = DataBuffer<neuronIdx_t>(firedNeuronsSize, CL_MEM_READ_WRITE,
			"firedNeurons", firedNeuronsKernelNames);

	getContext()->createBuffer(firedNeurons);

	//***********
	size_t firedCountsSize = totalPartitionCount;

	std::vector<std::string>  firedCountsNames;
	firedCountsNames.push_back("scatter");
	firedCountsNames.push_back(update_kernel_name);
	if(inputs_kernel_name != "")
	{
		firedCountsNames.push_back(inputs_kernel_name);
	}

	firedCounts = DataBuffer<neuronIdx_t>(firedCountsSize, CL_MEM_READ_WRITE,
			"firedCounts", firedCountsNames);

	getContext()->createBuffer(firedCounts);
	//***********

	size_t firedInputSize = totalPartitionCount * BIT_VECTOR_WORD_PITCH;

	std::vector<std::string>  firedInputNames;
	firedInputNames.push_back(update_kernel_name);
	if(inputs_kernel_name != "")
	{
		firedInputNames.push_back(inputs_kernel_name);
	}

	firedInput = DataBuffer<cl_uint>(firedInputSize, CL_MEM_READ_WRITE,
			"firedInput", firedInputNames);

	getContext()->createBuffer(firedInput);

	//***********
	size_t firedOutputSize = BIT_VECTOR_WORD_PITCH * outputPartitionCount;

	std::vector<std::string> firedOutputKernelNames;
	firedOutputKernelNames.push_back(update_kernel_name);

	firedOutput = DataBuffer<cl_uint>(firedOutputSize, CL_MEM_READ_WRITE,
			"firedOutputSize", firedOutputKernelNames);

	getContext()->createBuffer(firedOutput);
}

void SpikeBuffer::setInput(const std::vector<neuronIdx_t>& fInput, cl::Event& event)
{
	// TODO not blocking
	if(! fInput.empty())
	{
		firedInput.clearHostData();
		std::vector<cl_uint>& vec = firedInput.getDataRef();

		for(std::vector<cl_uint>::const_iterator it = fInput.begin();
				it != fInput.end(); ++it)
		{
			partitionIdx_t partition = GLOBAL_NEURON_IDX_TO_PARTITION((*it));

			if(partition >= totalPartitionCount)
			{		
				THROW_SZSNNEX("SZSNN::SpikeBuffer::setInput error: neuron with idx "
					<< (*it) << " not in the network");
			}

			neuronIdx_t	neuron = GLOBAL_NEURON_IDX_TO_LOCAL((*it));

			//std::cout << "GLOBAL " << (*it) << " LOCAL P " <<partition << " LOCAL N " <<neuron << std::endl;
			size_t word = partition * BIT_VECTOR_WORD_PITCH + neuron / BIT_VECTOR_WORD_SIZE;
			size_t bit = neuron % BIT_VECTOR_WORD_SIZE;
			//std::cout << "WORD " << word << " BIT " << bit << std::endl;
			vec[word] |= 1 << bit;
		}

		getContext()->writeBuffer(firedInput, CL_TRUE);

		isInputSet = 1;
	}
}

void SpikeBuffer::setInput()
{
	isInputSet = 0;
}

const std::vector<neuronIdx_t>& SpikeBuffer::getOutput( cl::Event& event )
{
	getContext()->readBuffer(firedOutput, CL_TRUE);
	firingBuffer.clear();
	bitvectorToSparse(firedOutput.getDataRef(), firingBuffer);
	return firingBuffer;
}

void SpikeBuffer::bitvectorToSparse( const std::vector<cl_uint>& bitvec, std::vector<neuronIdx_t>& sparseVec )
{
	for(size_t partition = 0; partition < outputPartitionCount; ++partition)
	{
		size_t pitch = bitvec.size() / outputPartitionCount;
		size_t partitionOffset = partition * pitch;
		for(size_t nword = 0; nword < pitch; ++nword)
		{
			cl_uint word = bitvec.at(nword + partitionOffset);
			if (word == 0) continue;

			for(size_t nbit=0; nbit < BIT_VECTOR_WORD_SIZE; ++ nbit)
			{
				bool fired = (word & (1<<nbit)) != 0;
				if (fired)
				{
					sparseVec.push_back(LOCAL_NEURON_IDX_TO_GLOBAL(partition, nword*32+nbit));
				}
			}
		}
	}
}


std::string SpikeBuffer::spikesToString()
{
	const std::vector<cl_uint>& counts = firedCounts.getDataRef();
	const std::vector<cl_uint>& spikes = firedNeurons.getDataRef();
	std::stringstream ss;
	for(unsigned i = 0; i < spikes.size(); i++)
	{
		if (!(i % MAX_PARTITION_SIZE))
		{
			ss <<std::endl<< "PARTITION nr. " << i/MAX_PARTITION_SIZE <<" has " << counts.at(i/MAX_PARTITION_SIZE) << " spikes"  << std::endl;
		}
		ss << spikes.at(i)<< " ";
	}
	ss<<std::endl;
	return ss.str();
}

std::string SpikeBuffer::bitvectorSpikesToString()
{
	const std::vector<cl_uint>& spikes = firedOutput.getDataRef();
	std::stringstream ss;
	//assert(spikes.size() == firedOutput.getSize());
	for(size_t s = 0; s <  spikes.size(); ++s){
		ss<< spikes.at(s) << " ";
	}
	for(size_t partition = 0; partition < outputPartitionCount; ++partition)
	{
		ss <<std::endl<< "PARTITION nr. " << partition <<std::endl;
		size_t pitch = spikes.size() / outputPartitionCount;
		size_t partitionOffset = partition * pitch;
		//ss<<partitionOffset;
		for(size_t nword = 0; nword < pitch; ++nword)
		{

			cl_uint word = spikes.at(nword + partitionOffset);
			if (word == 0) ss<<"none ";

			for(size_t nbit=0; nbit < BIT_VECTOR_WORD_SIZE; ++ nbit)
			{
				bool fired = (word & (1<<nbit)) != 0;
				if (fired)
				{
					ss << nword*32+nbit << " ";
				}
			}
		}
	}
	ss<<std::endl;
	return ss.str();
}





} /* namespace szsnn */
