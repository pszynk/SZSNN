/**
 * \file GroupData.cpp
 *
 * \date 16-12-2012
 * \author pawel
 * \description
 */

#include "GroupData.hpp"

namespace szsnn {

bool compare_bundleCounts(
		const std::pair<partitionIdx_t, size_t>& a,
		const std::pair<partitionIdx_t, size_t>& b)
{
	return a.second < b.second;
}

std::string GroupData::printGroupPtrs()
{
	std::stringstream ss;
    ss << "BEGIN GROUP PTRS >>>>" << std::endl;
	for(unsigned p = 0; p < totalpartitionCount; ++p)
	{
		ss << "[" << p << "]  " << std::endl;
		for(unsigned n = 0; n < MAX_PARTITION_SIZE; ++n)
		{
			ss << "  " << "<<" << n << ">>";
			for(unsigned d = 0; d< MAX_SYNAPTIC_DELAY; ++d)
			{
				ss << "[" << d << "]->" << "("
						<< groupPtrs.data[GROUP_ADDR_OFFSET(p,n,d)].s[0] << " | "
						<< groupPtrs.data[GROUP_ADDR_OFFSET(p,n,d)].s[1] << ")"
						<< " ";
			}
			ss << std::endl;
		}
		ss << std::endl;
	}
	return ss.str();
}
std::string GroupData::printGroupMatrix()
{
	std::stringstream ss;
	unsigned row_size = WARP_SIZE;
    ss << "BEGIN GROUP MATRIX >>>>" << std::endl;
    for(unsigned i = 0; i < groupMatrix.getSize(); ++i)
    {
    	if( (i % row_size) == 0)
    	{
    		ss << std::endl;
    	}
    	ss << "("<<groupMatrix.data[i].s[0]<< " | " << groupMatrix.data[i].s[1] << ") ";
    }
	return ss.str();
}
unsigned celiPowerOfTwo(unsigned val)
{
	val--;
	val |= val >> 1;
	val |= val >> 2;
	val |= val >> 4;
	val |= val >> 8;
	val |= val >> 16;
	val++;
	return val;
}

void GroupData::initialize(size_t _totalPartitionCount, const SynapseIndex& index)
{
#ifdef max
#undef max
#endif
	using namespace boost::tuples;

	totalpartitionCount = _totalPartitionCount;

	/* Każda grupa synaps [group] ma ten sam:
	 * - neuron presynaptyczny [source neuron] (czyli partycja i indeks neuronu
	 * w partycji)
	 * - opóźnienie [delay]
	 * - partycję docelową [target partition]
	 *
	 * w pesymistycznym przyadku (kazdy neuron połączony z każdym na wszystkich
	 * opóźnieniach) rozmiar wynosi:
	 *
	 * 			partitionCount * MAX_PARTITION_SIZE * MAX_SYNAPTIC_DELAY
	 *
	 * 	(bez wykorzystania macierzy rzadkiej)
	 * */


	size_t groupPtrsSize = totalpartitionCount * MAX_PARTITION_SIZE * MAX_SYNAPTIC_DELAY;
	std::vector<std::string> groupPtrsKernelNames;
	groupPtrsKernelNames.push_back("scatter");

	std::vector<std::string> groupMatrixKernelNames;
	groupMatrixKernelNames.push_back("scatter");

//	groupPtrs = DataBuffer<group_addr_t>(groupPtrsSize, CL_MEM_READ_WRITE, "groupPointers",
//			groupPtrsKernelNames);

	cl_uint2 zz; zz.s[0] = 0; zz.s[1] = 0;
	std::vector<group_addr_t> tempGroupPtrsVec(groupPtrsSize, zz);
	std::vector<group_t> tempGroupMatrixVec;

	std::map<partitionIdx_t, unsigned> incomingBundles;

	size_t wordsAllocated = 0;
	unsigned maxBundlesPerNeuronDelay = 0;

	/* przeiteruj po synapsach mających ten sam:
	 * - neuron presynaptyczny (partycja | neuron)
	 * - opóźnienie
	 *
	 * map{(s_part, s_neur, delay) -> map{(t_part) -> vector}}
	 */
	for(SynapseIndex::iterator it = index.begin(); it != index.end(); ++it)
	{
		const SynapseIndex::source_key& key = it->first;

		partitionIdx_t sourcePartition = get<0>(key);
		neuronIdx_t	sourceNeuron = get<1>(key);
		delay_t delay = get<2>(key);

		//std::cout << "ROZPATRUJE: Sp-> " << sourcePartition
		//		<< " Sn-> "<<sourceNeuron << " D-> " << delay << std::endl;
		unsigned sourceCount = index.getSourceCount(key);
		unsigned wordsCount = ALIGN(sourceCount, BUNDLE_SIZE);
	
		maxBundlesPerNeuronDelay = std::max(maxBundlesPerNeuronDelay, wordsCount);
		//std::cout << "SOURCE-> " << sourceCount << " WORDS " << wordsCount << " MAX " << maxBundlesPerNeuronDelay <<std::endl;
		//std::cout << "   PRZEZNACZE: " << sourceCount <<" ALIG " << wordsCount << std::endl;
		// invalid group -> (0,0);
		tempGroupMatrixVec.resize(wordsAllocated + wordsCount, zz);


		unsigned groupRowBegin = wordsAllocated;
		unsigned groupColumn = 0;

		const SynapseIndex::groups& groups = it->second;

		for(SynapseIndex::group_iterator ig = groups.begin(); ig != groups.end(); ++ig)
		{
			partitionIdx_t targetPartition = ig->first;
//			std::cout << "          PARTYCJA DOCELOWA: " << targetPartition << std::endl;

			const std::vector<size_t>& bundles = ig->second;
			size_t bundlesCount = bundles.size();
//			std::cout << "               BUNDLES SIZE: " << bundlesCount << std::endl;

			incomingBundles[targetPartition] += bundlesCount;
			group_t* bundlePtr = &tempGroupMatrixVec[groupRowBegin + groupColumn];
//			std::cout << "               zapisujemy od: " << groupRowBegin + groupColumn <<
//					" do " << groupRowBegin + groupColumn + bundlesCount<< std::endl;

			groupColumn += bundlesCount;
			//assert(groupColumn <= sourceCount);
			if(groupColumn > sourceCount)
			{
				THROW_SZSNNEX("SZSNN::GroupData::initialize: error: groupColumn > sourceCount: "
					<< groupColumn << " > " << sourceCount);
			}

			for(std::vector<size_t>::const_iterator ib = bundles.begin();
					ib != bundles.end(); ++ib, ++bundlePtr)
			{
				group_t temp;
				temp.s[0] = targetPartition;
				temp.s[1] = (*ib);
				*bundlePtr = temp;
			}
		}

		size_t addr = GROUP_ADDR_OFFSET(sourcePartition, sourceNeuron, delay-1);
		//std::cout << "WPISUJE dla " << sourcePartition << " " << sourceNeuron << " " << delay-1 << std::endl;
		group_addr_t addrVal;
		addrVal.s[0] = groupRowBegin;
		addrVal.s[1] = groupColumn;
		//std::cout << "WPISUJE dla " << groupRowBegin << " " << groupColumn << std::endl;

		tempGroupPtrsVec[addr] = addrVal;
		wordsAllocated += wordsCount;
	}

	zz.s[0] = 0; zz.s[1] = 0;
	groupPtrs = DataBuffer<group_addr_t>(groupPtrsSize, tempGroupPtrsVec, CL_MEM_READ_WRITE, "groupPointers",
				groupPtrsKernelNames, zz);
	groupMatrix = DataBuffer<group_addr_t>(wordsAllocated, tempGroupMatrixVec, CL_MEM_READ_WRITE, "groupMatrix",
				groupMatrixKernelNames, zz);

	getContext()->createBuffer(groupPtrs);
	getContext()->createBuffer(groupMatrix);

	maxIncomingBundles = incomingBundles.size() ?
			std::max_element(incomingBundles.begin(), incomingBundles.end(), compare_bundleCounts)->second :0;

	// TODO do kernela scatter
	/* maxBundlesPerNeuronDelay -> to maksymalna liczba paczek wpisanych w
	 * groupMatrix dla jednej pary <neuron (partycja:indeks w partycji), opóźnienie>
	 */
	this->pitch = std::max(1U, cl_uint(celiPowerOfTwo(maxBundlesPerNeuronDelay)));
	/* dopełniamy tą wielkość do potęgi 2 */
	//assert(this->pitch <= THREADS_PER_GROUP);
	/* nie może być większe od liczby threadów w jednej grupie, czyli
	 * pitch <= 256, czyli ograniczenie na synapsy o tym samym opóźnieniu
	 * dla jedngo neuronu to 256 * 32(64) = 8192 (16384)
	 *
	 */
	if(this->pitch > THREADS_PER_GROUP)
	{
		THROW_SZSNNEX("SZSNN::GroupData::initialize error: pitch > THREADS_PER_GROUP "
			<< this->pitch << " > " << THREADS_PER_GROUP);
	}


	this->step = THREADS_PER_GROUP / this->pitch;
	//assert(this->step * this->pitch == THREADS_PER_GROUP);
	if(this->step * this->pitch != THREADS_PER_GROUP)
	{
		THROW_SZSNNEX("SZSNN::GroupData::initialize error: step * pitch != THREADS_PER_GROUP "
			<< this->step * this->pitch << " != " << THREADS_PER_GROUP);
	}
	//std::cout << maxBundlesPerNeuronDelay << " " << pitch << " " << step << std::endl;
	//system("pause");

}

void GroupData::specUploadToDevice()
{
	getContext()->writeBuffer(groupPtrs, CL_TRUE);
	getContext()->writeBuffer(groupMatrix, CL_TRUE);


}
void GroupData::specDownloadFromDevice()
{
	getContext()->readBuffer(groupPtrs, CL_TRUE);
	getContext()->readBuffer(groupMatrix, CL_TRUE);
}
void GroupData::specFreeHostData()
{
	groupPtrs.freeHostData();
	groupMatrix.freeHostData();
}

} /* namespace szsnn */
