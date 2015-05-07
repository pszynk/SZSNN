/**
 * \file SpikeBuffer.hpp
 *
 * \date 29-12-2012
 * \author pawel
 * \description
 */

#ifndef SPIKEBUFFER_HPP_
#define SPIKEBUFFER_HPP_
#include <vector>
#include <boost/shared_ptr.hpp>

#include "types.h"
#include "utils.h"
#include "MemoryBuffer.hpp"
#include "DataBuffer.hpp"

namespace szsnn {

/*
 *
 */
class SpikeBuffer : public MemoryBuffer {

private:

	/**
	 * rzadka macierz strzałów (podział na partycje)
	 */
	DataBuffer<neuronIdx_t>		firedNeurons;
	/**
	 * licznik strałów dla macierzy rzadkiej
	 */
	DataBuffer<cl_uint>			firedCounts;


	/**
	 * bitowy wektor strzelających neuronów podany z zwenątrz
	 * słowo -> 32 neurony indeksowane [n+31, n+30, ... n+1, n] (bity)
	 * partycja -> słowa indeksowane [p, p+1, ..., p+k-1], gdzie
	 * 	k = maksymalna ilosć neuronów w partycji / bity w słowie
	 *
	 */
	cl_uint						isInputSet;
	DataBuffer<cl_uint>			firedInput;
	/**
	 * historia strzałów w bitmapie
	 */
	DataBuffer<cl_uint>			firedOutput;

	std::vector<neuronIdx_t>	firingBuffer;

	virtual void specUploadToDevice();
	virtual void specDownloadFromDevice();
	virtual void specFreeHostData();

	size_t totalPartitionCount;
	size_t outputPartitionCount;

	void bitvectorToSparse(const std::vector<cl_uint>& bitvec, std::vector<neuronIdx_t>& sparseVec);

public:
	SpikeBuffer(boost::shared_ptr<Context> _context,
			size_t _totalPartitionCount,
			size_t _outputPartitionCount):
		MemoryBuffer(_context), isInputSet(0), totalPartitionCount(_totalPartitionCount),
		outputPartitionCount(_outputPartitionCount)
	{};

	void initialize(std::string update_kernel_name, std::string inputs_kernel_name = "");

	void setInput(const std::vector<neuronIdx_t>& fInput, cl::Event& event); //TODO blocking nonoblocking
	void setInput(); 

	const std::vector<neuronIdx_t>& getOutput(cl::Event& event); //TODO blocking /nonoblocking

	std::string spikesToString();
	std::string bitvectorSpikesToString();

	DataBuffer<neuronIdx_t>&	getFiredNeuronsRef() {return firedNeurons; };

	DataBuffer<cl_uint>&		getFiredCountsRef() {return firedCounts; };

	DataBuffer<cl_uint>&		getFiredInputRef() {return firedInput; };

	DataBuffer<cl_uint>&		getFiredOutputRef() {return firedOutput; };

	cl_uint getIsInputSet() const {return isInputSet; };

};

} /* namespace szsnn */
#endif /* SPIKEBUFFER_HPP_ */
