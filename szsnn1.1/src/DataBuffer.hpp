/**
 * \file DataBuffer.hpp
 *
 * \date 28-12-2012
 * \author pawel
 * \description
 */

#ifndef DATABUFFER_HPP_
#define DATABUFFER_HPP_

#include "opencl_header.h"

//#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>

#include "utils.h"
namespace szsnn {

/*
 *
 */
template<typename T>
class DataBuffer {
private: //TODO consty

public:
	size_t					size;
	size_t					sizeBytes;
	std::string				bufferName;
	std::vector<std::string>	kernelNames;

	//boost::shared_ptr<T>		data;
	std::vector<T>					data;

	cl::Buffer						buffer;

	cl_mem_flags				flags;

	T							zero;


	typedef typename std::vector<T>::const_iterator	iterator;
	iterator begin() const {return data.begin();};
	iterator end() const {return data.end();};

	DataBuffer():size(0), sizeBytes(0), flags(0) {};
	DataBuffer(size_t _size, cl_mem_flags _flags, std::string _bufferName,
		std::vector<std::string> _kernelNames, T _zero = 0):
			size(_size), sizeBytes(_size * sizeof(T)),
			bufferName(_bufferName), kernelNames(_kernelNames),
			data(_size, _zero), flags(_flags), zero(_zero)
	{};

	DataBuffer(size_t _size, const std::vector<T>& _data,
			cl_mem_flags _flags, std::string _bufferName,
			std::vector<std::string> _kernelNames,  T _zero = 0):
			size(_size), sizeBytes(_size * sizeof(T)),
			bufferName(_bufferName), kernelNames(_kernelNames),
			data(_data), flags(_flags), zero(_zero)
	{
		if(_size != _data.size())
		{
			THROW_SZSNNEX("SZSNN::DataBuffer::DataBuffer: _size=[" <<
				_size <<"] != _data.size() =[" <<
				_data.size() <<"]");
		}
	};

	DataBuffer(const std::vector<T>& _data,
			cl_mem_flags _flags, std::string _bufferName,
			std::vector<std::string> _kernelNames,  T _zero = 0):
			size(_data.size()), sizeBytes(_data.size() * sizeof(T)),
			bufferName(_bufferName), kernelNames(_kernelNames),
			data(_data), flags(_flags), zero(_zero)
	{};


	//GETTERS
	size_t getSize() const {return size; };
	size_t getSizeBytes() const {return sizeBytes; };
	std::string getBufferName() const {return bufferName; };
	std::vector<std::string> getKernelNames() const {return kernelNames; };
	cl_map_flags getFlags() const {return flags; };

	const std::vector<T>& getDataRef() const {return data; };
	std::vector<T>&	getDataRef() {return data; };

	const cl::Buffer& getBufferRef() const {return buffer; };
	cl::Buffer& getBufferRef() {return buffer; };

	//SETTERS
	//virtual ~DataBuffer();

	void putIntoData(size_t idx, T val)
	{
		if(idx < data.size())
		{
			data[idx] = val;
		}
		else
		{
			THROW_SZSNNEX("SZSNN::DataBuffer::putIntoData: idx=[" <<
				idx <<"] >= buffer size of this DataBuffer =[" <<
				data.size() <<"]");
		}
	}

	bool isHostData() const
	{
		return size == data.size();
	}

	void allocateHostData()
	{
		data.resize(size, zero);
	}

	void freeHostData()
	{
		std::vector<T>().swap(data);
	}

	void clearHostData()
	{
		std::fill(data.begin(), data.end(), zero);
	}
};

} /* namespace szsnn */
#endif /* DATABUFFER_HPP_ */
