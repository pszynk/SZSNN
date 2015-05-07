/**
 * \file Simulation.hpp
 *
 * \date 22-11-2012
 * \author pawel
 * \description
 */

#ifndef SIMULATION_HPP_
#define SIMULATION_HPP_

#include <iostream>

#include "opencl_header.h"

#include <boost/shared_ptr.hpp>


#include "locales.h"
#include "SzsnnException.hpp"
#include "Context.hpp"
#include "NetworkGraph.hpp"
#include "MemoryBuffer.hpp"
#include "KernelWrapper.hpp"

#include "models/neuron_model_traits.hpp"

#include "NeuronData.hpp"
#include "SynapseData.hpp"
#include "GlobalQueue.hpp"
#include "LocalQueue.hpp"
#include "SpikeBuffer.hpp"
#include "CurrentData.hpp"
#include "SzsnnTimer.hpp"

namespace szsnn {

template<typename floatT>
class Simulation {
public:
	typedef std::vector<unsigned> firing_output;

	typedef std::vector<unsigned> firing_input;

private:


	const static  std::string kernelFlags;
	const static  std::string scatterKernelFlags;
	const static  std::string updateKernelFlags;
	const static  std::string inputKernelFlats;
	const static  std::string gatherKernelFlags;


	cl_uint		neuronPartitionCount;
	cl_uint		inputPartitionCount;
	cl_uint		totalPartitionCount;	//z input

	SzsnnTimer	timer;

	bool hasInputs;

	void prepKernels();

	void setKernelsCycle(unsigned long _cycle); 
	void setUpdateKernelInput();

	void prepUpdateKernel();
	void prepInputKernel();
	void prepScatterKernel();
	void prepGatherKernel();

	void prepareGPUMemory();

	void prefire();
	void fire();
	void postfire();

	void setFiringInput(const firing_input& firirng);
	void setFiringInput();

	void pauseTimeCount();
	void continueTimeCount();

	//void setCurrentInput(const std::vector<floatT>& current);



public:
	boost::shared_ptr<Context>  		context;

	// kernels
	KernelWrapper		scatterKernel;
	KernelWrapper		updateKernel;
	KernelWrapper		inputKernel;
	KernelWrapper		gatherKernel;


	// data structures
	NeuronData<floatT>		neurons;
	SynapseData<floatT>		synapses;
	GlobalQueue				globalQueue;
	LocalQueue				localQueue;

	SpikeBuffer				spikes;
	CurrentData<floatT>		currents;


	template<typename NeuronModelT,typename InputModel>
	Simulation(
		const Context& 						_context,
		/*const*/ NetworkGraph<NeuronModelT, InputModel>&	network); //TODO phony synapse

	void testUpdate();
	void testScatter();
	void testGather();
//	virtual ~Simulation();

	void prepareSimulation();

	void step();
	void step(const firing_input& fInput);

	const firing_output& stepAndFired();
	const firing_output& stepAndFired(const firing_input& fInput);

	size_t getTime()
	{
		return timer.getRealTime();
	}

};

} /* namespace szsnn */

#include "Simulation.ipp"
#endif /* SIMULATION_HPP_ */
