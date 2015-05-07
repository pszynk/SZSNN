/**
 * \file DeviceManager.cpp
 *
 * \date 09-12-2012
 * \author pawel
 * \description
 */

#ifdef _WIN32
#pragma warning( push )
#pragma warning( disable : 4503 )
#endif

#include "DeviceManager.hpp"

namespace szsnn {


DeviceManager::DeviceManager():
		minDataTypeAlignSize(0),
		maxMemAllocSize(0)
{
	for(int i = 0; i < __phony; i++) {
		GpuMemType memType = (GpuMemType) i;
		totalMemory.insert(std::pair<GpuMemType, size_t>(memType, 0));
		maxMemory.insert(std::pair<GpuMemType, size_t>(memType, 0));
		memoryTypeName.insert(std::pair<GpuMemType, std::string>(memType, ""));
	}

	memoryUnits[MEM_CONSTANT] = std::pair<float, std::string>(1024.0, "KB");
	memoryUnits[MEM_GLOBAL] = std::pair<float, std::string>(1024.0*1024.0, "MB");
	memoryUnits[MEM_LOCAL] = std::pair<float, std::string>(1024.0, "KB");

}

void DeviceManager::setup(cl::Device _device)
{
	device = _device;
	minDataTypeAlignSize = device.getInfo<CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE>();
	maxMemAllocSize = device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>();

	maxMemory[MEM_CONSTANT] = device.getInfo<CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE>();
	memoryTypeName[MEM_CONSTANT] = "CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE";

	maxMemory[MEM_GLOBAL] = device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>();
	memoryTypeName[MEM_GLOBAL] = "CL_DEVICE_GLOBAL_MEM_SIZE";

	maxMemory[MEM_LOCAL] = device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>();
	memoryTypeName[MEM_LOCAL] = "CL_DEVICE_LOCAL_MEM_SIZE";
}

size_t DeviceManager::adjustSize(cl_ulong _size)
{
	return (_size / minDataTypeAlignSize + 1) * minDataTypeAlignSize;
}


bool DeviceManager::addKernel(std::string kernelName)
{
	bool insert = kernels.insert(kernelName).second;

	if (!insert) return false;

	for(int i = 0; i < __phony; i++) {
		GpuMemType memType = (GpuMemType) i;
		kernelMemory[kernelName].insert(std::pair<GpuMemType,
				std::map<std::string, size_t> >(memType, std::map<std::string, size_t>()));
		totalKernelMemory[kernelName][memType] = 0;
	}

	return true;
}

bool DeviceManager::addMemory(
		std::string 	kernelName,
		std::string 	memName,
		GpuMemType		memType,
		size_t			size)
{
	if(kernelName.compare(_SZSNN_NONE_KERNEL) == 0) {return false; };
	if(kernels.find(kernelName) == kernels.end())
	{
		THROW_SZSNNEX("DeviceManager: register memory " << memName
				<< " -> ["<< memoryTypeName[memType] << "] failed"
				<< " there is no kernel named '" << kernelName << "'");
		return false;
	}

	bool insert =  kernelMemory[kernelName][memType].insert(
			std::pair<std::string, size_t>(memName, size)).second;

	if (!insert) return false;

	totalKernelMemory[kernelName][memType] += size;

	totalMemory[memType] += size;
	return true;
}

bool DeviceManager::addGlobalMemory(
		std::string 	memName,
		GpuMemType		memType,
		size_t			size)
{
	if (memType == MEM_GLOBAL)
	{
		if (!globalMemory.insert(memName).second)
		{
			THROW_SZSNNEX("DeviceManager: register memory '" << memName
					<< "' -> ["<< memoryTypeName[memType] << "] failed"
					<< " there already is global memory named '" << memName << "'");
			return false;
		}
		return true;
	}
	return false;
}

void DeviceManager::registerMemory(
	std::vector<std::string> 	kernelNames,
	std::string 				memName,
	GpuMemType					memType,
	size_t						_size)
{
	size_t size = adjustSize(_size);

	std::stringstream names;
	for(std::vector<std::string>::iterator ikn = kernelNames.begin(); ikn != kernelNames.end(); ++ikn)
	{
		std::string kernelName = (*ikn);
		names << kernelName << " ";
		if (addMemory(kernelName, memName, memType, size))
		{
			if(totalKernelMemory[kernelName][memType] > maxMemory[memType])
			{
				THROW_SZSNNEX("Total allocation for local memory in kernel " << kernelName
						<< ", " << ((float)totalKernelMemory[kernelName][memType]) /
							memoryUnits[memType].first << " " << memoryUnits[memType].second
						<< " , exceeds " << memoryTypeName[memType] << ", "
						<< ((float)maxMemory[memType])/(memoryUnits[memType].first) << ", "
						<< memoryUnits[memType].second);
			}
		}
	}
	if (addGlobalMemory(memName,memType,_size))
	{
		if (totalMemory[MEM_GLOBAL] > maxMemory[MEM_GLOBAL])
		{
			THROW_SZSNNEX("Total allocation of global memory exceeded in kernels " << names.str()
					<< ", " << ((float)totalMemory[memType]) /
						memoryUnits[memType].first << " " << memoryUnits[memType].second
					<< " , exceeds " << memoryTypeName[memType] << ", "
					<< ((float)maxMemory[memType])/(memoryUnits[memType].first) << ", "
					<< memoryUnits[memType].second);
		}
	}
}

void DeviceManager::registerKernel(
	std::string		kernelName)
{

	addKernel(kernelName);
}

std::string DeviceManager::getMemoryRaport() const
{
	std::stringstream stream;
	stream << " MEMORY RAPORT:" << std::endl;
	stream << "  " << "total memory usage:";
	for(int i = 0; i < __phony; i++)
	{
		const GpuMemType memType = (GpuMemType) i;
		stream << "   ";
		stream << memoryTypeName.at(memType) << "         ";
		stream << (float)totalMemory.at(memType) / memoryUnits.at(memType).first << " ";
		stream << "/ ";
		stream << (float)maxMemory.at(memType) / memoryUnits.at(memType).first << " ";
		stream << "[" << memoryUnits.at(memType).second << "]";
		stream << std::endl;
	}


	return stream.str();
}

#ifdef _WIN32
#pragma warning( pop )
#endif

} /* namespace szsnn */
