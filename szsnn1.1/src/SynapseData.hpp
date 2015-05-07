/**
 * \file SynapseData.hpp
 *
 * \date 21-11-2012
 * \author pawel
 * \description
 */

#ifndef SYNAPSEDATA_HPP_
#define SYNAPSEDATA_HPP_

#include <string>
#include <sstream>

#include "opencl_header.h"

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include "locales.h"
#include "types.h"
#include "synapse_model.h"
#include "NetworkGraph.hpp"
#include "SynapseIndex.hpp"
#include "GroupData.hpp"
#include "MemoryBuffer.hpp"
#include "DataBuffer.hpp"

namespace szsnn {



/*
 *
 */
template<typename floatT>
class SynapseData : public MemoryBuffer {
private:

	virtual void specUploadToDevice();
	virtual void specDownloadFromDevice();
	virtual void specFreeHostData();


public:
	delay_t maxDelay;
	size_t totalPartitionCount;

	DataBuffer<cl_ulong> 		delaysBitmap;

	DataBuffer<neuronIdx_t> 	targets;
	DataBuffer<floatT> 			weights;

	GroupData	groups;

	std::string printDelaysBitmap();
	std::string printTargets();
	std::string printWeights();

	void addSynapse(
			const Synapse<floatT>& 		synapse,
			size_t& 					nextFreeBundle,
			SynapseIndex& 				index,
			std::vector<neuronIdx_t>& 	_targets,
			std::vector<floatT>&		_weights);

	void setDelaysBitmap(const SynapseIndex& index, std::vector<cl_ulong>& _delaysBitmap);

public:

	SynapseData(
		boost::shared_ptr<Context> 	_context):
		MemoryBuffer(_context), groups(_context), maxDelay(0), totalPartitionCount(0) {};


	template<typename NeuronModelT, typename InputModleT>
	void initialize(const NetworkGraph<NeuronModelT,InputModleT>& graph);

	inline size_t getMaxIncomingBundles() const {return groups.getMaxIncomingBundles(); };


	DataBuffer<floatT>& getWeightsRef() { return weights; };
	DataBuffer<neuronIdx_t>& getTargetsRef() { return targets; };
	DataBuffer<cl_ulong>& getDelaysBitmapRef() { return delaysBitmap; };

	
	cl_uint getPitch() const { return groups.getPitch(); };
	cl_uint getStep() const { return groups.getStep(); };

	DataBuffer<group_addr_t>& getGroupPtrsRef() { return groups.getGroupPtrsRef(); };
	DataBuffer<group_t>& getGroupMatrixRef() { return groups.getGroupMatrixRef(); };


};

} /* namespace szsnn */

#include "SynapseData.ipp"
#endif /* SYNAPSEDATA_HPP_ */
