/**
 * \file LocalQueue.hpp
 *
 * \date 29-12-2012
 * \author pawel
 * \description
 */

#ifndef LOCALQUEUE_HPP_
#define LOCALQUEUE_HPP_

#include "types.h"
#include "MemoryBuffer.hpp"
#include "DataBuffer.hpp"

namespace szsnn {

/*
 *
 */
class LocalQueue : public MemoryBuffer {
private:

	DataBuffer<lq_entry_t>	entryQueue;
	DataBuffer<cl_uint>		entryCounter;



	virtual void specUploadToDevice();
	virtual void specDownloadFromDevice();
	virtual void specFreeHostData();

public:
	LocalQueue(
			boost::shared_ptr<Context> 	_context):
		MemoryBuffer(_context) {};
	//virtual ~LocalQueue();

	void initialize(size_t totalPartitionCount);

	DataBuffer<lq_entry_t>& getEntryQueueRef() { return entryQueue; };
	DataBuffer<cl_uint>& getEntryCounterRef() { return entryCounter; };
};

} /* namespace szsnn */
#endif /* LOCALQUEUE_HPP_ */
