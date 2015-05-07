/**
 * \file GroupData.hpp
 *
 * \date 16-12-2012
 * \author pawel
 * \description
 */

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <iostream>

#include <boost/shared_ptr.hpp>

#include "types.h"
#include "utils.h"
#include "Context.hpp"
#include "SynapseIndex.hpp"
#include "MemoryBuffer.hpp"
#include "DataBuffer.hpp"

#ifndef GROUPDATA_HPP_
#define GROUPDATA_HPP_

namespace szsnn {

/*
 *
 */
class GroupData : public MemoryBuffer
{

private:
	//boost::shared_ptr<Context> context;

	size_t totalpartitionCount;
	size_t maxIncomingBundles;

	cl_uint pitch;
	cl_uint step;


	DataBuffer<group_addr_t>	groupPtrs;

	DataBuffer<group_t>			groupMatrix;


//
//	std::vector<group_t> 		groupMatrix;
//
//	std::vector<group_addr_t>	groupPtrs;

	virtual void specUploadToDevice();
	virtual void specDownloadFromDevice();
	virtual void specFreeHostData();

public:
	std::string printGroupMatrix();
	std::string printGroupPtrs();

	GroupData(
			boost::shared_ptr<Context> 	_context):
				MemoryBuffer(_context), totalpartitionCount(0), maxIncomingBundles(0), 
				pitch(0), step(0)
		{};

	void initialize(size_t _partitionCount, const SynapseIndex& index);

	inline size_t getMaxIncomingBundles() const {return maxIncomingBundles; };


	cl_uint getPitch() const { return pitch; };
	cl_uint getStep() const { return step; };

	DataBuffer<group_addr_t>& getGroupPtrsRef() { return groupPtrs; };
	DataBuffer<group_t>& getGroupMatrixRef() { return groupMatrix; };
};

} /* namespace szsnn */
#endif /* GROUPDATA_HPP_ */
