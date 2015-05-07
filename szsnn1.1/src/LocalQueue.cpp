/**
 * \file LocalQueue.cpp
 *
 * \date 29-12-2012
 * \author pawel
 * \description
 */

#include "LocalQueue.hpp"

namespace szsnn {

void LocalQueue::specUploadToDevice()
{
	getContext()->writeBuffer(entryQueue, CL_TRUE);
	getContext()->writeBuffer(entryCounter, CL_TRUE);
}

void LocalQueue::specDownloadFromDevice()
{
	getContext()->readBuffer(entryQueue, CL_TRUE);
	getContext()->readBuffer(entryCounter, CL_TRUE);

}

void LocalQueue::specFreeHostData()
{
	entryQueue.freeHostData();
	entryCounter.freeHostData();
}

void LocalQueue::initialize(size_t totalPartitionCount)
{

	/*
	 * najgorszy przypadek:
	 * 	* wszystkie neurony mają połączenia na wszystkich opóźnieniach
	 * 	   +
	 * 	* wszystkie neurony ciągle strzelają
	 *
	 * 	'szerokość' musi pomieścić wszystkie pary <neuron strzelający, opóźnienie neuronu>
	 * 	'wysokość'  każda partycja ma bufor cykliczny o długości MAX_SYNAPTIC_DELAY,
	 * 					jedno jego wejście ma rozmiar 'szerokości'
	 */
	size_t entryQueueWidth = ALIGN(MAX_PARTITION_SIZE * MAX_SYNAPTIC_DELAY, 32);
	size_t entryQueueHigh = totalPartitionCount * MAX_SYNAPTIC_DELAY;
	size_t entryQueueSize = entryQueueWidth * entryQueueHigh;

	std::vector<std::string> entryQueueKernelNames;
	entryQueueKernelNames.push_back("scatter");

	cl_short2 zero; zero.s[0] = 0; zero.s[1] = 0;//zero.x = 0; zero.y = 0;
	entryQueue = DataBuffer<lq_entry_t>(entryQueueSize, CL_MEM_READ_WRITE, "localQueue",
		entryQueueKernelNames, zero);

	getContext()->createBuffer(entryQueue);

	//*************
	size_t entryCounterSize = entryQueueHigh;
	std::vector<std::string> entryCounterKernelNames;
	entryCounterKernelNames.push_back("scatter");

	entryCounter = DataBuffer<cl_uint>(entryCounterSize, CL_MEM_READ_WRITE, "localQueueCounter",
			entryCounterKernelNames);

	getContext()->createBuffer(entryCounter);



}


} /* namespace szsnn */
