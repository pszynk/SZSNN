/**
 * \file MemoryBuffer.hpp
 *
 * \date 21-12-2012
 * \author pawel
 * \description
 */

#ifndef MEMORYBUFFER_HPP_
#define MEMORYBUFFER_HPP_

#include <boost/shared_ptr.hpp>
#include "Context.hpp"

namespace szsnn {

/*
 *
 */
class MemoryBuffer {
private:
	bool _isUpToDate;

	boost::shared_ptr<Context> context;
protected:
	MemoryBuffer(boost::shared_ptr<Context> _context):
		context(_context) {};

	Context* getContext() {return context.get();};

	void setIsUpToDate(bool state) { _isUpToDate = state; };

	virtual void specUploadToDevice() = 0;
	virtual void specDownloadFromDevice() = 0;
	virtual void specFreeHostData() = 0;
public:
	bool isUpToDate() {return _isUpToDate;};

	void uploadToDevice() {_isUpToDate = false; specUploadToDevice(); };
	void downloadFromDevice() {_isUpToDate = true; specDownloadFromDevice(); };
	void freeHostData() {_isUpToDate = false; specFreeHostData(); };

	virtual ~MemoryBuffer() {};
};

} /* namespace szsnn */
#endif /* MEMORYBUFFER_HPP_ */
