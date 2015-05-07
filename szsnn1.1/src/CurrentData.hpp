/**
 * \file CurrentData.hpp
 *
 * \date 01-01-2013
 * \author pawel
 * \description
 */

#ifndef CURRENTDATA_HPP_
#define CURRENTDATA_HPP_

#include "locales.h"
#include "MemoryBuffer.hpp"
#include "DataBuffer.hpp"


namespace szsnn  {

/*
 *
 */
template<typename floatT>
class CurrentData : public MemoryBuffer
{
private:

	DataBuffer<floatT>	inputCurrents;
	DataBuffer<floatT>	currents;

	virtual void specUploadToDevice();
	virtual void specDownloadFromDevice();
	virtual void specFreeHostData();

public:
	CurrentData(boost::shared_ptr<Context> _context):
		MemoryBuffer(_context)
	{};

	void initialize(size_t totalPartitionCount, std::string update_kernel_name,
			std::string inputs_kernel_name);

	DataBuffer<floatT>& getInputCurrentsRef() {return inputCurrents; };
	DataBuffer<floatT>& getCurrentsRef() {return currents; };

};

} /* namespace szsnn */

#include "CurrentData.ipp"
#endif /* CURRENTDATA_HPP_ */
