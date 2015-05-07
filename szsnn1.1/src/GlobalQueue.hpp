/**
 * \file GlobalQueue.hpp
 *
 * \date 21-12-2012
 * \author pawel
 * \description
 */

#ifndef GLOBALQUEUE_HPP_
#define GLOBALQUEUE_HPP_


#include <vector>

#include "opencl_header.h"

#include <boost/shared_ptr.hpp>

#include "types.h"
#include "MemoryBuffer.hpp"
#include "Context.hpp"
#include "DataBuffer.hpp"


namespace szsnn {

/*
 *
 */
class GlobalQueue :
	public MemoryBuffer {
private:

	cl_ulong pitch;

	DataBuffer<unsigned>		bundleCounter;
	DataBuffer<bundleIdx_t>		bundleQueue;

	virtual void specUploadToDevice();
	virtual void specDownloadFromDevice();
	virtual void specFreeHostData();

public:
	GlobalQueue(
			boost::shared_ptr<Context> 	_context):
		MemoryBuffer(_context) {};

	void initialize(
			unsigned neuronPartitionCount,
			unsigned maxIncomingBundles);


	cl_ulong getPitch() const { return pitch; }


	DataBuffer<unsigned>& getBundleCounterRef() { return bundleCounter; };
	DataBuffer<bundleIdx_t>& getBundleQueueRef() { return bundleQueue; };
};

} /* namespace szsnn */
#endif /* GLOBALQUEUE_HPP_ */
