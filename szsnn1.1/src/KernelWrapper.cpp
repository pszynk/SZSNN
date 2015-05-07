/**
 * \file KernelWrapper.cpp
 *
 * \date 30-12-2012
 * \author pawel
 * \description
 */

#include "KernelWrapper.hpp"

namespace szsnn {

KernelWrapper::KernelWrapper(
	std::string		_kernelName,
	std::string 	_kernelFileName,
	std::string     _flags,
	size_t			_blockSizeX,
	size_t			_blockSizeY,
	const cl::NDRange&		_globalSizeRange,
	const cl::NDRange&		_localSizeRange):
		kernelName(_kernelName),
		kernelFileName(_kernelFileName),
		flags(_flags),
		blockSizeX(_blockSizeX),
		blockSizeY(_blockSizeY),
		globalRange(_globalSizeRange),
		localRange(_localSizeRange)
		{};


} /* namespace szsnn */
