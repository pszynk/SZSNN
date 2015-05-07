/**
 * \file KernelWrapper.hpp
 *
 * \date 30-12-2012
 * \author pawel
 * \description
 */

#ifndef KERNELWRAPPER_HPP_
#define KERNELWRAPPER_HPP_

#include <string>

#include "opencl_header.h"

#include <boost/tuple/tuple.hpp>

namespace szsnn {

/*
 *
 */
class KernelWrapper {
	friend class Context;

private:

	cl::Kernel		kernel;

	const std::string		kernelName;
	const std::string 		kernelFileName;
	std::string     		flags;

	const size_t			blockSizeX;
	const size_t			blockSizeY;

	const cl::NDRange		globalRange;
	const cl::NDRange		localRange;

public:
	KernelWrapper(
		std::string		_kernelName,
		std::string 	_kernelFileName,
		std::string     _flags,
		size_t			_blockSizeX,
		size_t			_blockSizeY,
		const cl::NDRange&		_globalSizeRange,
		const cl::NDRange&		_localSizeRange);
};

} /* namespace szsnn */
#endif /* KERNELWRAPPER_HPP_ */
