/**
 * \file DeviceManager.hpp
 *
 * \date 09-12-2012
 * \author pawel
 * \description
 */

#ifndef DEVICEMANAGER_HPP_
#define DEVICEMANAGER_HPP_

#ifdef _WIN32
#pragma warning( push )
#pragma warning( disable : 4503 )
#endif

#include <string>
#include <map>
#include <set>
#include <list>

#include "opencl_header.h"
#include "locales.h"
#include "utils.h"
//#include "SzsnnException.hpp"



namespace szsnn {

/*
 *
 */
class DeviceManager {
public:
	enum GpuMemType {
		MEM_CONSTANT 	= 0,
		MEM_GLOBAL,
		MEM_LOCAL,
		__phony};

private:

	cl::Device	device;

	cl_uint minDataTypeAlignSize;
	cl_ulong maxMemAllocSize;

	std::map<GpuMemType, std::pair<float, std::string> >	memoryUnits;

	  /* map{kernel_name -> map{alloc_name -> alloc_size}} */
	std::map<GpuMemType, cl_ulong>	maxMemory;


	std::map<GpuMemType, size_t>	totalMemory;

	  /* map{kernel_name -> map{alloc_name -> alloc_size}} */
	std::map<GpuMemType, std::string>	memoryTypeName;

	  /* map{kernel_name -> map{mem_type -> map{mem_name -> alloc_size}}} */
	std::map<std::string,
		std::map<GpuMemType,
			std::map<std::string, size_t> > >  kernelMemory;

	std::map<std::string,
		std::map<GpuMemType, size_t > >  totalKernelMemory;

	std::set<std::string>	globalMemory;

	std::set<std::string> kernels;

	std::map<std::string, std::map<std::string, double> > execTime;

	std::set<std::string> kernelNamesExecTime;


	bool addKernel(
		std::string 	kernelName);

	bool addMemory(
		std::string 	kernelName,
		std::string 	memName,
		GpuMemType		memType,
		size_t			_size);

	bool addGlobalMemory(
		std::string 	memName,
		GpuMemType		memType,
		size_t			_size);


	size_t adjustSize(cl_ulong _size);

public:

	void registerMemory(
		std::vector<std::string> 	kernelNames,
		std::string 				memName,
		GpuMemType					memType,
		size_t						size);

	void registerKernel(
		std::string 	kernelName);

	void setup(cl::Device _device);

	size_t getKernelMemoryUsage(std::string kernelName)
	{
		size_t ssum = 0;
		for(int i = 0; i < __phony; i++)
		{
			GpuMemType memType = (GpuMemType) i;
			ssum += totalKernelMemory[kernelName][memType];
		}
		return ssum;
	}

	size_t getKernelMemoryUsage(std::string kernelName, GpuMemType memType)
	{
		return totalKernelMemory[kernelName][memType];
	}

	std::string getMemoryRaport() const;
	DeviceManager();
	//virtual ~DeviceManager();
};

} /* namespace szsnn */

#ifdef _WIN32
#pragma warning( pop )
#endif

#endif /* DEVICEMANAGER_HPP_ */
