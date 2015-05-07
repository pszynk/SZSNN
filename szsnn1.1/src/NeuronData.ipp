/**
 * \file NeuronData.cpp
 *
 * \date 17-11-2012
 * \author pawel
 * \description
 */

#include <vector>
#include <string>

#include "utils.h"
#include "models/neuron_model_traits.hpp"
#include "locales.h"

namespace szsnn {

template<typename floatT>
size_t NeuronData<floatT>::offset(size_t partitionCount, size_t subIdx, size_t partitionIdx, size_t neuronIdx)
{
	return (subIdx * partitionCount + partitionIdx) * MAX_PARTITION_SIZE + neuronIdx;
}

template<typename floatT>
template<typename NeuronModelT, typename InputModelT>
//TODO enable_if is_same
void NeuronData<floatT>::initialize(const NetworkGraph<NeuronModelT, InputModelT>& network)
{
	neuronPartitionCount = network.getNeuronPartitionCount();

	if(!(neuronPartitionCount > 0))
	{
		THROW_SZSNNEX("SZSNN::NeuronData::NeuronData() error: cant create NeuronData"
				<< " for 0 neurons")
	}

//	size_t neuronPartitionSizesSize = neuronPartitionCount;
//	std::vector<std::string> neuronPartitionSizesKernelNames;
//	neuronPartitionSizesKernelNames.push_back(neuron_model_traits<NeuronModelT>::update_kernel_name);
//
//	neuronPartitionSizes = DataBuffer<cl_uint>(neuronPartitionSizesSize, network.getNeuronPartitionSizes(),
//			CL_MEM_READ_ONLY, "NeuronData:neuronPartitionSizes", neuronPartitionSizesKernelNames);

	unsigned neuronParamsCount = neuron_model_traits<NeuronModelT>::param_count;
	unsigned neuronVarsCount = neuron_model_traits<NeuronModelT>::var_count;

	size_t neuronParmsSize = 	neuronParamsCount *
								neuronPartitionCount *
								MAX_PARTITION_SIZE;
	std::vector<std::string> neuronParamsKernelNames;
	neuronParamsKernelNames.push_back(neuron_model_traits<NeuronModelT>::update_kernel_name);

	neuronParameters = DataBuffer<floatT>(neuronParmsSize,
			CL_MEM_READ_WRITE, "NeuronData:neuronParameters", neuronParamsKernelNames);



	size_t neuronVarsSize = 	neuronVarsCount *
								neuronPartitionCount *
								MAX_PARTITION_SIZE;
	std::vector<std::string> neuronVarsKernelNames;
	neuronVarsKernelNames.push_back(neuron_model_traits<NeuronModelT>::update_kernel_name);
	neuronVariables = DataBuffer<floatT>(neuronVarsSize, CL_MEM_READ_WRITE,
			"NeuronData:neuronVariables", neuronVarsKernelNames);

	const std::vector<NeuronModelT>& neurons = network.getNeurons();
	for(unsigned idx = 0; idx < neurons.size(); ++idx)
	{
		unsigned neuronIdx = GLOBAL_NEURON_IDX_TO_LOCAL(idx);
		unsigned partitionIdx = GLOBAL_NEURON_IDX_TO_PARTITION(idx);
		for(unsigned i = 0; i < neuronParamsCount; ++i)
		{
			neuronParameters.putIntoData(offset(neuronPartitionCount, i, partitionIdx, neuronIdx),
					neurons.at(idx).getParam(i));
		}
		for(unsigned i = 0; i < neuronVarsCount; ++i)
		{
			neuronVariables.putIntoData(offset(neuronPartitionCount, i, partitionIdx, neuronIdx),
					neurons.at(idx).getVar(i));
		}
	}
	//getContext()->createBuffer(neuronPartitionSizes);
	getContext()->createBuffer(neuronParameters);
	getContext()->createBuffer(neuronVariables);

/********** INPUTS ********************/

	inputPartitionCount = network.getInputPartitionCount();
	if(hasInputs)
	{
		if(!(inputPartitionCount > 0))
		{
			THROW_SZSNNEX("SZSNN::NeuronData::NeuronData() error: cant create NeuronData"
							<< " for with `hasInputs`=True and 0 inputs")
		}


//		size_t inputPartitionSizesSize = inputPartitionCount;
//		std::vector<std::string> inputPartitionSizesKernelNames;
//		inputPartitionSizesKernelNames.push_back(neuron_model_traits<InputModelT>::update_kernel_name);
//
//		inputPartitionSizes = DataBuffer<cl_uint>(inputPartitionSizesSize, network.getInputPartitionSizes(),
//				CL_MEM_READ_ONLY, "NeuronData:inputPartitionSizes", inputPartitionSizesKernelNames);

		unsigned inputParamsCount = neuron_model_traits<InputModelT>::param_count;
		unsigned inputVarsCount = neuron_model_traits<InputModelT>::var_count;

		size_t inputParmsSize = 	inputParamsCount *
									inputPartitionCount *
									MAX_PARTITION_SIZE;
		std::vector<std::string> inputParamsKernelNames;
		inputParamsKernelNames.push_back(neuron_model_traits<InputModelT>::update_kernel_name);

		inputParameters = DataBuffer<floatT>(inputParmsSize,
				CL_MEM_READ_WRITE, "NeuronData:inputParameters", inputParamsKernelNames);



		size_t inputVarsSize = 		inputVarsCount *
									inputPartitionCount *
									MAX_PARTITION_SIZE;
		std::vector<std::string> inputVarsKernelNames;
		inputVarsKernelNames.push_back(neuron_model_traits<InputModelT>::update_kernel_name);
		inputVariables = DataBuffer<floatT>(inputVarsSize, CL_MEM_READ_WRITE,
				"NeuronData:inputVariables", inputVarsKernelNames);

		const std::vector<InputModelT>& inputs = network.getInputs();
		for(unsigned idx = 0; idx < inputs.size(); ++idx)
		{
			unsigned inputIdx = GLOBAL_NEURON_IDX_TO_LOCAL(idx);
			unsigned partitionIdx = GLOBAL_NEURON_IDX_TO_PARTITION(idx);
			for(unsigned i = 0; i < inputParamsCount; ++i)
			{
				inputParameters.putIntoData(offset(inputPartitionCount, i, partitionIdx, inputIdx),
						inputs.at(idx).getParam(i));
			}
			for(unsigned i = 0; i < inputVarsCount; ++i)
			{
				inputVariables.putIntoData(offset(inputPartitionCount, i, partitionIdx, inputIdx),
						inputs.at(idx).getVar(i));
			}
		}
		//getContext()->createBuffer(inputPartitionSizes);
		getContext()->createBuffer(inputParameters);
		getContext()->createBuffer(inputVariables);

	}

	/* COMMON */

	size_t totalPartitionSizesSize = neuronPartitionCount + inputPartitionCount;

	std::vector<std::string> totalPartitionSizesKernelNames;
		totalPartitionSizesKernelNames.push_back("gather");
		totalPartitionSizesKernelNames.push_back(neuron_model_traits<NeuronModelT>::update_kernel_name);
		totalPartitionSizesKernelNames.push_back(neuron_model_traits<InputModelT>::update_kernel_name);

	std::vector<cl_uint> totalPartSizes = network.getNeuronPartitionSizes();
	if (hasInputs)
	{
		totalPartSizes.push_back(network.getInputPartitionSize());
	}
	assert(totalPartSizes.size() == totalPartitionSizesSize);

	totalPartitionSizes = DataBuffer<cl_uint>(totalPartitionSizesSize, totalPartSizes,
			CL_MEM_READ_ONLY, "NeuronData:totalPartitionSizes", totalPartitionSizesKernelNames);

	getContext()->createBuffer(totalPartitionSizes);

}
template<typename floatT>
void NeuronData<floatT>::specUploadToDevice()
{
	//getContext()->writeBuffer(neuronPartitionSizes, CL_TRUE);
	getContext()->writeBuffer(totalPartitionSizes, CL_TRUE);
	getContext()->writeBuffer(neuronParameters, CL_TRUE);
	getContext()->writeBuffer(neuronVariables, CL_TRUE);

	if (hasInputs)
	{
		//getContext()->writeBuffer(inputPartitionSizes, CL_TRUE);
		getContext()->writeBuffer(inputParameters, CL_TRUE);
		getContext()->writeBuffer(inputVariables, CL_TRUE);
	}
};

template<typename floatT>
void NeuronData<floatT>::specDownloadFromDevice()
{
	//getContext()->readBuffer(neuronPartitionSizes, CL_TRUE);
	getContext()->readBuffer(totalPartitionSizes, CL_TRUE);
	getContext()->readBuffer(neuronParameters, CL_TRUE);
	getContext()->readBuffer(neuronVariables, CL_TRUE);

	if (hasInputs)
	{
		//getContext()->readBuffer(inputPartitionSizes, CL_TRUE);
		getContext()->readBuffer(inputParameters, CL_TRUE);
		getContext()->readBuffer(inputVariables, CL_TRUE);
	}
};


template<typename floatT>
void NeuronData<floatT>::specFreeHostData()
{
	//neuronPartitionSizes.freeHostData();
	totalPartitionSizes.freeHostData();
	neuronParameters.freeHostData(); //TODO
	neuronVariables.freeHostData();

	if (hasInputs)
	{
		//inputPartitionSizes.freeHostData();
		inputParameters.freeHostData(); //TODO
		inputVariables.freeHostData();
	}
};

//#include <algorithm>
//#include <utility>
//template<typename floatT>
//bool NeuronData<floatT>::test()
//{
//	std::vector<floatT> tempP(neuronParameters.data);
//	std::vector<floatT> tempV(neuronVariables.data);
//
//	this->uploadToDevice();
//	neuronParameters.freeHostData();
//	neuronVariables.freeHostData();
//	this->downloadFromDevice();
//
//	typedef typename std::vector<floatT>::iterator iter;
//
//	std::pair<iter,iter> mypair1;
//	std::pair<iter,iter> mypair2;
//
//	std::cout << std::endl;
//	for(iter itp1 = neuronParameters.data.begin(), itp2 = tempP.begin();
//			itp1 != neuronParameters.data.end(); ++itp1, ++itp2)
//	{
//		std::cout << (*itp1) << " " << (*itp2) << ", ";
//	}
//	mypair1 = std::mismatch(neuronParameters.data.begin(), neuronParameters.data.end(), tempP.begin());
//	mypair2 = std::mismatch(neuronVariables.data.begin(), neuronVariables.data.end(), tempV.begin());
//
//	return (mypair1.first == neuronParameters.data.end()) && (mypair1.second == tempP.end())
//			&&
//		(mypair2.first == neuronVariables.data.end()) && (mypair2.second == tempV.end());
//
//};

} /* namespace szsnn */
