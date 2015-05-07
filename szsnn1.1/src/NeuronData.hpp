/**
 * \file NeuronData.hpp
 *
 * \date 17-11-2012
 * \author pawel
 * \description
 */

#ifndef NEURONDATA_HPP_
#define NEURONDATA_HPP_

#include "opencl_header.h"

#include <boost/shared_ptr.hpp>
//#include <vector>

#include "locales.h"
#include "MemoryBuffer.hpp"
#include "DataBuffer.hpp"

namespace szsnn {

/*
 *
 */
template <typename floatT>
class NeuronData : public MemoryBuffer {
private:
	cl_uint neuronPartitionCount;
	cl_uint inputPartitionCount;
	bool hasInputs;


	DataBuffer<cl_uint> totalPartitionSizes;

	//DataBuffer<cl_uint>	neuronPartitionSizes;
	DataBuffer<floatT>	neuronParameters;
	DataBuffer<floatT>	neuronVariables;

	//DataBuffer<cl_uint>	inputPartitionSizes;
	DataBuffer<floatT>	inputParameters;
	DataBuffer<floatT>	inputVariables;

	virtual void specUploadToDevice();
	virtual void specDownloadFromDevice();
	virtual void specFreeHostData();
	size_t offset(
		size_t partitionCount,
		size_t subIdx,
		size_t partitionIdx,
		size_t neuronIdx);

	std::string neuronDataToString(DataBuffer<floatT>& dataB);

public:
	NeuronData(boost::shared_ptr<Context> 	_context, bool _hasInputs):
		MemoryBuffer(_context), hasInputs(_hasInputs) {};


	template<typename NeuronTypeT, typename InputModelT>
	void initialize(const NetworkGraph<NeuronTypeT, InputModelT>&	network);


	DataBuffer<cl_uint> getTotalPartitionSizesRef() {return totalPartitionSizes; };

	//DataBuffer<cl_uint>& getNeuronPartitionSizesRef() {return neuronPartitionSizes; };
	DataBuffer<floatT>& getNeuronParametersRef() {return neuronParameters; };
	DataBuffer<floatT>& getNeuronVariablesRef() {return neuronVariables; };

//	DataBuffer<cl_uint>& getInputPartitionSizesRef()
//	{
//		if(hasInputs)
//		{ return inputPartitionSizes; }
//		THROW_SZSNNEX("SZSNN::NeuronData::getInputPartitionSizesRef() error: "
//				<<"NeuronData initialized with hasInputs=`"<<hasInputs<<"` ");
//	};

	DataBuffer<floatT>& getInputParametersRef()	{
		if(hasInputs)
		{ return inputParameters; }
		THROW_SZSNNEX("SZSNN::NeuronData::getInputParametersRef() error: "
				<<"NeuronData initialized with hasInputs=`"<<hasInputs<<"` ");
	};
	DataBuffer<floatT>& getInputVariablesRef() 	{
		if(hasInputs)
		{ return inputVariables; }
		THROW_SZSNNEX("SZSNN::NeuronData::getInputVariablesRef() error: "
				<<"NeuronData initialized with hasInputs=`"<<hasInputs<<"` ");
	};

//	bool test();

	std::string neuronParamsToString();
	std::string neuronVarsToString();

};

template <typename floatT>
std::string szsnn::NeuronData<floatT>::neuronVarsToString()
{
	return neuronDataToString(neuronVariables);
}


template <typename floatT>
std::string szsnn::NeuronData<floatT>::neuronParamsToString()
{
	return neuronDataToString(neuronParameters);
}


template <typename floatT>
std::string szsnn::NeuronData<floatT>::neuronDataToString(DataBuffer<floatT>& dataB)
{
	std::stringstream ss;
	const std::vector<floatT>& dataVec = dataB.getDataRef();

	for(unsigned i = 0; i < dataVec.size(); i++)
	{
		if (!(i % MAX_PARTITION_SIZE))
		{
			ss <<std::endl<< "ROW nr. "<<i/MAX_PARTITION_SIZE << std::endl;
		}
		ss << dataVec.at(i)<< " ";
	}
	ss<<std::endl;
	return ss.str();
}

} /* namespace szsnn */

#include "NeuronData.ipp"
#endif /* NEURONDATA_HPP_ */
