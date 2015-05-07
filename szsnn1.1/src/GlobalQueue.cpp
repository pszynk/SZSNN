/**
 * \file GlobalQueue.cpp
 *
 * \date 21-12-2012
 * \author pawel
 * \description
 */

#include "GlobalQueue.hpp"

namespace szsnn {

void GlobalQueue::initialize(unsigned neuronPartitionCount, unsigned maxIncomingBundles)
{
	if (neuronPartitionCount > MAX_PARTITION_COUNT)
	{
		THROW_SZSNNEX("GlobalQueue::initialize: neuronPartitionCount ["
				<< neuronPartitionCount << "] > MAX_PARTITION_COUNT ["
				<< MAX_PARTITION_COUNT << "]");
	}

	size_t bundleCounterSize = ALIGN(neuronPartitionCount * 2, 32); // TODO dlaczego 32?
	std::vector<std::string> counterKernelNames;
	counterKernelNames.push_back("scatter");
	counterKernelNames.push_back("gather");

	bundleCounter = DataBuffer<unsigned>(bundleCounterSize, CL_MEM_READ_WRITE, "GlobalQueue:bundleCounter",
			counterKernelNames);

	getContext()->createBuffer(bundleCounter);

	size_t bundleQueueHeight = neuronPartitionCount * 2;
	size_t bundleQueueWidth = maxIncomingBundles; //TODO multiplicator
	this->pitch = bundleQueueWidth;

	std::vector<std::string> queueKernelNames;
	queueKernelNames.push_back("scatter");
	queueKernelNames.push_back("gather");

	bundleQueue = DataBuffer<bundleIdx_t>(bundleQueueHeight * bundleQueueWidth, CL_MEM_READ_WRITE,
			"GlobalQueue:bundleQueue", queueKernelNames);


	getContext()->createBuffer(bundleQueue);
}


void GlobalQueue::specUploadToDevice()
{
	getContext()->writeBuffer(bundleCounter, CL_TRUE);
	getContext()->writeBuffer(bundleQueue,  CL_TRUE);
}

void GlobalQueue::specDownloadFromDevice()
{
	getContext()->readBuffer(bundleCounter,  CL_TRUE);
	getContext()->readBuffer(bundleQueue, CL_TRUE);
}

void GlobalQueue::specFreeHostData()
{
	bundleCounter.freeHostData();
	bundleQueue.freeHostData();
}

} /* namespace szsnn */
