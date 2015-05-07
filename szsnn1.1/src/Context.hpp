/**
 * \file Context.hpp
 *
 * \date 07-12-2012
 * \author pawel
 * \description
 */

#ifndef CONTEXT_HPP_
#define CONTEXT_HPP_




#include <map>
#include <set>

#include <malloc.h>
#include <fstream>
#include <sstream>
#include <algorithm>


#ifdef _WIN32
//#define  _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define NOMINMAX
#include <windows.h>
#include <winbase.h>
#else
#include <sys/time.h>
#include <linux/limits.h>
#endif
//#include <linux/limits.h> // do getPAtsh
#include "opencl_header.h"

#include "utils.h"
#include "locales.h"
#include "DeviceManager.hpp"
#include "DataBuffer.hpp"
#include "KernelWrapper.hpp"






#define FIND_TARGET_DEVICE(platformDevices, targetDevices, deviceIterator, found)\
  {\
    const char *tD = targetDevices;\
    found = false;\
    char *str = (char *) calloc(0xFFFF, sizeof(char));\
    strcpy(str, tD);\
    char *pch;\
    pch = strtok (str, ";");\
    while (pch != NULL)\
    {\
      for(deviceIterator = platformDevices.begin(); deviceIterator != platformDevices.end();\
      ++deviceIterator)\
      {\
        std::string deviceName = (*deviceIterator).getInfo<CL_DEVICE_NAME>();\
        std::cout << deviceName << std::endl;\
        if(strcmp(deviceName.c_str(), pch) == 0)\
        {\
          found = true; break;\
        }\
      }\
      if(found){break;}\
      pch = strtok (NULL, ";");\
    }\
    free(str);\
  }
namespace szsnn {

/*
 *
 */
class Context {
private:
	DeviceManager deviceMgr;
	std::vector<cl::Platform> platforms;
	std::vector<cl::Device> devices;

	cl::Platform platform;
	cl::Context context;
	cl::Device device;
	cl::CommandQueue commandQueue;

	bool registerON;

	cl::Device findTargetDevice(
			std::vector<cl::Device>	platformDevices,
			std::vector<std::string>	targetDevices);



public:
	Context(bool _registerON = true):
		platforms(),
		devices(),
		platform(),
		context(),
		device(),
		commandQueue(),
		registerON(_registerON)
	{}

	const cl::Device& getDevice() const {return device; };
//	~Context();


	void setup();

	void createKernel(
		cl::Kernel&   		kernel,
		std::string         kernelFileName,
		std::string         kernelName,
		std::string         flagsStr,
		size_t              blockSizeX,
		size_t              blockSizeY);

	void createKernel(
		KernelWrapper&		wrap);

	void enqueueKernel(
		const cl::Kernel&				kernel,
		std::string 					kernelName,
		const cl::NDRange& 				global,
		const cl::NDRange& 				local,
		const std::vector<cl::Event>* 	events = NULL,
		cl::Event* 						event = NULL);

	void enqueueKernel(
		const KernelWrapper&			wrap,
		const std::vector<cl::Event>* 	events = NULL,
		cl::Event* 						event = NULL);

	void createBuffer(
		cl::Buffer&					buffer,
		std::string 				bufferName,
		std::vector<std::string>	kernelNames,
		cl_mem_flags 				flags,
		size_t						size);

	template <typename T>
	void createBuffer(
		cl::Buffer&					buffer,
		std::string 				bufferName,
		std::vector<std::string>	kernelNames,
		cl_mem_flags 				flags,
		const std::vector<T>&		vec)
	{
		cl_int err = CL_SUCCESS;
		cl_uint size = vec.capacity() * sizeof(T);
		createBuffer(buffer, bufferName, kernelNames, flags, size);
	 }

	template <typename T>
	void createBuffer(
		DataBuffer<T>&			dataBuffer)
	{
		createBuffer(
				//dataBuffer.buffer,
				dataBuffer.getBufferRef(),
				dataBuffer.getBufferName(),
				dataBuffer.getKernelNames(),
				dataBuffer.getFlags(),
				dataBuffer.getSizeBytes());
	}

	void writeBuffer(
		const cl::Buffer&	buffer,
		std::string 		bufferName,
		cl_bool				block,
		cl_uint				offset,
		cl_uint				size,
		const void*			ptr);

	template <typename T>
	void writeBuffer(
		const cl::Buffer&		buffer,
		std::string 			bufferName,
		cl_bool					block,
		const std::vector<T>&	vec)
	{
		cl_uint size = vec.size() * sizeof(T);

		writeBuffer(buffer, bufferName, block, 0, size, &vec[0]);
	}

	template<typename T>
	void writeBuffer(
		const DataBuffer<T>&	dataBuffer,
		cl_bool					block)
	{
		if(!dataBuffer.isHostData())
		{
			THROW_SZSNNEX("Context::writeBuffer: data in dataBuffer " <<
				dataBuffer.getBufferName() << " is not allocated on host");
		}
		writeBuffer(
			dataBuffer.getBufferRef(),
			dataBuffer.getBufferName(),
			block,
			0,
			dataBuffer.getSizeBytes(),
			&(dataBuffer.getDataRef()[0]));
	}

	void readBuffer(
		const cl::Buffer&	buffer,
		std::string 		bufferName,
		cl_bool				block,
		cl_uint				offset,
		cl_uint				size,
		void*				ptr);


	template <typename T>
	void readBuffer(
		const cl::Buffer&		buffer,
		std::string 			bufferName,
		cl_bool					block,
		std::vector<T>&			vec)
	{
		cl_uint size = vec.size() * sizeof(T);

		readBuffer(buffer, bufferName, block, 0, size, &vec[0]);
	}

	template<typename T>
	void readBuffer(
		DataBuffer<T>&			dataBuffer,
		cl_bool					block)
	{
		if(!dataBuffer.isHostData())
		{
			dataBuffer.allocateHostData();
		}
		const cl::Buffer& b = dataBuffer.getBufferRef();
		readBuffer(
			b,
			dataBuffer.getBufferName(),
			block,
			0,
			dataBuffer.getSizeBytes(),
			&(dataBuffer.getDataRef()[0]));

	}

	template <typename T>
	void setKernelArg(
		cl::Kernel& 		kernel,
		std::string 		kernelName,
		cl_uint				argNum,
		T					arg,
		std::string 		argName)
	{
		cl_int status;
		status = kernel.setArg(argNum, arg);
		if(status != CL_SUCCESS)
		{
		  THROW_SZSNNEX("SZSNN::Context::setKernelArg() Failed to set kernel argument #" << argNum << ", `"
		  << argName << "` for kernel `" << kernelName << "` due to error code " << status);
		}
	}

	template <typename T>
	void setKernelArg(
		KernelWrapper&		kernelWrap,
		cl_uint				argNum,
		T					arg,
		std::string 		argName)
	{
		setKernelArg(kernelWrap.kernel, kernelWrap.kernelName, argNum, arg, argName);
	}

	template <typename T>
	void setKernelArg(
		KernelWrapper&		kernelWrap,
		cl_uint				argNum,
		DataBuffer<T>		buffer)
	{
		setKernelArg(kernelWrap.kernel, kernelWrap.kernelName, argNum, buffer.buffer, buffer.bufferName);
	}




	DeviceManager* getDeviceMgr()
	{
		return &deviceMgr;
	}


	cl::Platform getPlatform() {return this->platform;};
	cl::Device getDevice() {return this->device;};
	cl::Context getContext() {return this->context;};
	cl::CommandQueue getCommandQueue() {return this->commandQueue;};




};

} /* namespace szsnn */
#endif /* CONTEXT_HPP_ */
