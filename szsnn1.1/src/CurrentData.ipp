/**
 * \file currentsData.cpp
 *
 * \date 01-01-2013
 * \author pawel
 * \description
 */



namespace szsnn {


template<typename floatT>
void CurrentData<floatT>::initialize(size_t totalPartitionCount, std::string update_kernel_name,
		std::string inputs_kernel_name)
{

	size_t inputCurrentsSize = 	1 *
								totalPartitionCount *
								MAX_PARTITION_SIZE;

	std::vector<std::string> inputCurrentsKernelNames;
	inputCurrentsKernelNames.push_back("gather");
	inputCurrentsKernelNames.push_back(update_kernel_name);
	inputCurrentsKernelNames.push_back(inputs_kernel_name);


	inputCurrents = DataBuffer<floatT>(inputCurrentsSize, CL_MEM_READ_WRITE,
			"currentsData:inputCurrents", inputCurrentsKernelNames);

	getContext()->createBuffer(inputCurrents);

	//***********

	size_t currentsSize = 	2 *
							totalPartitionCount *
							MAX_PARTITION_SIZE;
	std::vector<std::string> currentsKernelNames;
	currentsKernelNames.push_back("gather");
	currentsKernelNames.push_back(update_kernel_name);
	currentsKernelNames.push_back(inputs_kernel_name);

	currents = DataBuffer<floatT>(currentsSize, CL_MEM_READ_WRITE,
			"currentsData:currents", currentsKernelNames);

	getContext()->createBuffer(currents);


}

template<typename floatT>
void CurrentData<floatT>::specUploadToDevice()
{
	getContext()->writeBuffer(inputCurrents, CL_TRUE);
	getContext()->writeBuffer(currents, CL_TRUE);
}

template<typename floatT>
void CurrentData<floatT>::specDownloadFromDevice()
{
	getContext()->readBuffer(inputCurrents, CL_TRUE);
	getContext()->readBuffer(currents, CL_TRUE);
}
template<typename floatT>
void CurrentData<floatT>::specFreeHostData()
{
	inputCurrents.freeHostData();
	currents.freeHostData();
}


} /* namespace szsnn */
