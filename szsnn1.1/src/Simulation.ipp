/**
 * \file Simulation.cpp
 *
 * \date 22-11-2012
 * \author pawel
 * \description
 */


namespace szsnn {

template<class floatT>
const std::string Simulation<floatT>::kernelFlags = std::string("")
		+ " -cl-denorms-are-zero"
//		+ " -cl-fp32-correctly-rounded-divide-sqrt"
#if SZSNN_DEBUG
		+ " -g"
		+ " -cl-opt-disable"
		+ " -DSZSNN_KERNEL_DEBUG"
#endif
;

template<class floatT>
const std::string Simulation<floatT>::scatterKernelFlags = kernelFlags+" ";

template<class floatT>
const std::string Simulation<floatT>::updateKernelFlags = kernelFlags+" "
		+ " -DSZSNN_KERNEL_OUTPUT_SPIKES";

template<class floatT>
const std::string Simulation<floatT>::inputKernelFlats = kernelFlags+" "
#if (SZSNN_NO_INPUT_TO_OUTPUT == 0)
		+ " -DSZSNN_KERNEL_OUTPUT_SPIKES"
#endif
		;

template<class floatT>
const std::string Simulation<floatT>::gatherKernelFlags = kernelFlags+" ";

template<typename floatT>
template<typename NeuronModelT, typename InputModelT>
Simulation<floatT>::Simulation(
	const Context& 						_context,
	/*const*/ NetworkGraph<NeuronModelT, InputModelT>&	network):
		context(new Context(_context)),
		neuronPartitionCount(network.getNeuronPartitionCount()),
		inputPartitionCount(network.getInputPartitionCount()),
		totalPartitionCount(network.getTotalPartitionCount()),
		hasInputs(network.getInputPartitionCount()),

	//kernels
		scatterKernel(
				"scatter",
				"scatter_kernel.cl",
				scatterKernelFlags,
				THREADS_PER_GROUP, 1, cl::NDRange(
					THREADS_PER_GROUP*totalPartitionCount),
				cl::NDRange(THREADS_PER_GROUP,1)),
		updateKernel(
				neuron_model_traits<NeuronModelT>::update_kernel_name,
				neuron_model_traits<NeuronModelT>::update_kernel_filename,
				updateKernelFlags + " " +
					szsnn_supported_floating_point<floatT>::kernel_flags,
				THREADS_PER_GROUP, 1, cl::NDRange(
						THREADS_PER_GROUP*neuronPartitionCount),
				cl::NDRange(THREADS_PER_GROUP,1)),
		inputKernel(
				neuron_model_traits<InputModelT>::update_kernel_name,
				neuron_model_traits<InputModelT>::update_kernel_filename,
				inputKernelFlats + " " +
					szsnn_supported_floating_point<floatT>::kernel_flags,
				THREADS_PER_GROUP, 1, cl::NDRange(
						THREADS_PER_GROUP*inputPartitionCount),
				cl::NDRange(THREADS_PER_GROUP,1)),
		gatherKernel(
				"gather",
				"gather_kernel.cl",
				gatherKernelFlags + " " +
					szsnn_supported_floating_point<floatT>::kernel_flags,
				THREADS_PER_GROUP, 1, cl::NDRange(
					THREADS_PER_GROUP*totalPartitionCount),
				cl::NDRange(THREADS_PER_GROUP,1)),
	//data
		neurons(context, hasInputs),
		synapses(context),
		globalQueue(context),
		localQueue(context),
		spikes(context, totalPartitionCount,
#if (SZSNN_NO_INPUT_TO_OUTPUT == 1)
				neuronPartitionCount
#elif (SZSNN_NO_INPUT_TO_OUTPUT == 0)
				totalPartitionCount
#endif
		),
		currents(context)
{
	if(!network.getNetworkSize())
	{
		THROW_SZSNNEX("SZSNN::Simulation::Simulation() error: network has "
				<< "0 neurons")
	}
	//create kernels
	context.get()->createKernel(scatterKernel);
	context.get()->createKernel(updateKernel);
	if(hasInputs) {context.get()->createKernel(inputKernel);};
	context.get()->createKernel(gatherKernel);

	network._addPhonySynapse();

	neurons.initialize(network);
	synapses.initialize(network);
	globalQueue.initialize(network.getNeuronPartitionCount(), synapses.getMaxIncomingBundles());
	localQueue.initialize(network.getTotalPartitionCount());
	spikes.initialize(neuron_model_traits<NeuronModelT>::update_kernel_name,
					neuron_model_traits<InputModelT>::update_kernel_name);
	currents.initialize(network.getTotalPartitionCount(),
			neuron_model_traits<NeuronModelT>::update_kernel_name,
			neuron_model_traits<InputModelT>::update_kernel_name);

	std::cout << context.get()->getDeviceMgr()->getMemoryRaport() << std::endl;

	//assert(spikes.getFiredOutputRef().getSize() ==spikes.getFiredOutputRef().getDataRef().size() );
	prepareGPUMemory();
	prepareSimulation();
};



template<typename floatT>
void szsnn::Simulation<floatT>::prepareGPUMemory()
{
	neurons.uploadToDevice();
	synapses.uploadToDevice();
	globalQueue.uploadToDevice();
	localQueue.uploadToDevice();
	spikes.uploadToDevice();
	currents.uploadToDevice();
}


template<typename floatT>
void Simulation<floatT>::testUpdate()
{
	prepUpdateKernel();
	std::cout << "Simulation test -> UPDATE KERNEL" << std::endl;
	std::cout << neurons.neuronParamsToString() << std::endl;
	cl::Event ndrEvt;
	context.get()->setKernelArg(updateKernel, 0, 0, "cycle");
	context.get()->enqueueKernel(updateKernel, 0, &ndrEvt);
	cl_int eventStatus = CL_QUEUED;
	cl_int status;
	ndrEvt.wait();
	while(eventStatus != CL_COMPLETE)
	{
	    status = ndrEvt.getInfo<cl_int>(CL_EVENT_COMMAND_EXECUTION_STATUS, &eventStatus);
	    CHECK_CL_SUCCESS(status, "Neurosim::run: cl:Event.getInfo(CL_EVENT_COMMAND_EXECUTION_STATUS)"
	      " failed");
	}
	context->getCommandQueue().finish();
	context->getCommandQueue().flush();
	//	assert(spikes.getFiredOutputRef().getSize() ==spikes.getFiredOutputRef().getDataRef().size() );
	//spikes.downloadFromDevice();
	//	assert(spikes.getFiredOutputRef().getSize() ==spikes.getFiredOutputRef().getDataRef().size() );
	//std::cout << ">>>>>>>>>>>>>>>>>>" <<std::endl << spikes.spikesToString() << std::endl;
	//std::cout << ">>>>>>>>>>>>>>>>>>" <<std::endl << spikes.bitvectorSpikesToString() << std::endl;;
};

template<typename floatT>
void Simulation<floatT>::testScatter()
{
	prepScatterKernel();
	std::cout << "Simulation test -> SCATTER KERNEL" << std::endl;
	cl::Event ndrEvt;
	context.get()->setKernelArg(scatterKernel, 0, 0, "cycle");
	context.get()->enqueueKernel(scatterKernel, 0, &ndrEvt);
	cl_int eventStatus = CL_QUEUED;
	cl_int status;
	ndrEvt.wait();
	while(eventStatus != CL_COMPLETE)
	{
		status = ndrEvt.getInfo<cl_int>(CL_EVENT_COMMAND_EXECUTION_STATUS, &eventStatus);
		CHECK_CL_SUCCESS(status, "Neurosim::run: cl:Event.getInfo(CL_EVENT_COMMAND_EXECUTION_STATUS)"
			" failed");
	}
	context->getCommandQueue().finish();
	context->getCommandQueue().flush();
}

template<typename floatT>
void Simulation<floatT>::testGather()
{
	prepGatherKernel();
	std::cout << "Simulation test -> GATHER KERNEL" << std::endl;
	std::cout << synapses.printTargets() << std::endl;
	std::cout << synapses.printWeights() << std::endl;
	cl::Event ndrEvt;
	context.get()->setKernelArg(gatherKernel, 0, 1, "cycle");
	context.get()->enqueueKernel(gatherKernel, 0, &ndrEvt);
	cl_int eventStatus = CL_QUEUED;
	cl_int status;
	ndrEvt.wait();
	while(eventStatus != CL_COMPLETE)
	{
		status = ndrEvt.getInfo<cl_int>(CL_EVENT_COMMAND_EXECUTION_STATUS, &eventStatus);
		CHECK_CL_SUCCESS(status, "Neurosim::run: cl:Event.getInfo(CL_EVENT_COMMAND_EXECUTION_STATUS)"
			" failed");
	}
	context->getCommandQueue().finish();
	context->getCommandQueue().flush();
}


template<typename floatT>
void Simulation<floatT>::prepUpdateKernel()
{
	
	// ARG 0
		// cycle
	// ARG 1
	context.get()->setKernelArg(updateKernel, 1, neuronPartitionCount, "neuronPartitionCount");
	// ARG 2
	context.get()->setKernelArg(updateKernel, 2, neurons.getTotalPartitionSizesRef());
	// ARG 3
	context.get()->setKernelArg(updateKernel, 3, neurons.getNeuronParametersRef());
	// ARG 4
	context.get()->setKernelArg(updateKernel, 4, neurons.getNeuronVariablesRef());
	// ARG 5
		// isInputCurrentSet
	// ARG 6
	context.get()->setKernelArg(updateKernel, 6, currents.getInputCurrentsRef());
	// ARG 7
	context.get()->setKernelArg(updateKernel, 7, currents.getCurrentsRef());
	// ARG 8
		// isInputFirirngSet
	// ARG 9
	context.get()->setKernelArg(updateKernel, 9, spikes.getFiredInputRef());
	// ARG 10
	context.get()->setKernelArg(updateKernel, 10, spikes.getFiredCountsRef());
	// ARG 11
	context.get()->setKernelArg(updateKernel, 11, spikes.getFiredNeuronsRef());
	// ARG 12
	context->setKernelArg(updateKernel, 12, spikes.getFiredOutputRef());
	// ARG 13
	context->setKernelArg(updateKernel, 13, 0, "neuronPartitionOffset");
	// ARG 14
	context->setKernelArg(updateKernel, 14, totalPartitionCount, "totalPartitionCount");
}

template<typename floatT>
void Simulation<floatT>::prepInputKernel()
{
	// ARG 0
		// cycle
	// ARG 1
	context.get()->setKernelArg(inputKernel, 1, inputPartitionCount, "inputPartitionCount");
	std::cout << inputPartitionCount << std::endl;
	// ARG 2
	context.get()->setKernelArg(inputKernel, 2, neurons.getTotalPartitionSizesRef());
	std::cout << neurons.getTotalPartitionSizesRef().getDataRef()[0] << std::endl;
	// ARG 3
	context.get()->setKernelArg(inputKernel, 3, neurons.getInputParametersRef());
	std::cout << neurons.getInputParametersRef().getDataRef()[0] << std::endl;
	// ARG 4
	context.get()->setKernelArg(inputKernel, 4, neurons.getInputVariablesRef());
	std::cout << neurons.getInputVariablesRef().getDataRef()[0] << std::endl;
	// ARG 5
		// isInputCurrentSet
	// ARG 6
	context.get()->setKernelArg(inputKernel, 6, currents.getInputCurrentsRef());
	// ARG 7
	context.get()->setKernelArg(inputKernel, 7, currents.getCurrentsRef());
	// ARG 8
		// isInputFirirngSet
	// ARG 9
	context.get()->setKernelArg(inputKernel, 9, spikes.getFiredInputRef());
	// ARG 10
	context.get()->setKernelArg(inputKernel, 10, spikes.getFiredCountsRef());
	// ARG 11
	context.get()->setKernelArg(inputKernel, 11, spikes.getFiredNeuronsRef());
	// ARG 12
	context->setKernelArg(inputKernel, 12, spikes.getFiredOutputRef());
	// ARG 13
	context->setKernelArg(inputKernel, 13, neuronPartitionCount, "inputPartitionOffset");
	// ARG 14
	context->setKernelArg(inputKernel, 14, totalPartitionCount, "totalPartitionCount");
}

template<typename floatT>
void Simulation<floatT>::prepScatterKernel()
{
	// ARG 0  cycle
	// ARG 1 partitionCount
	context.get()->setKernelArg(scatterKernel, 1, totalPartitionCount, "totalPartitionCount");
	// ARG 2 firedCount
	context.get()->setKernelArg(scatterKernel, 2, spikes.getFiredCountsRef());
	// ARG 3 firedSparse
	context.get()->setKernelArg(scatterKernel, 3, spikes.getFiredNeuronsRef());
	// ARG 4 groupPitch
	context->setKernelArg(scatterKernel, 4, synapses.getPitch(), "groupPitch");
	// ARG 5 groupStep
	context->setKernelArg(scatterKernel, 5, synapses.getStep(), "groupStep");
	// ARG 6 group addr
	context->setKernelArg(scatterKernel, 6, synapses.getGroupPtrsRef());
	// ARG 7 groups
	context->setKernelArg(scatterKernel, 7, synapses.getGroupMatrixRef());
	// ARG 8 globalQueue pitch
	context->setKernelArg(scatterKernel, 8, globalQueue.getPitch(), "globalQeueuPitch");
	// ARG 9 globalQueue Counts
	context->setKernelArg(scatterKernel, 9, globalQueue.getBundleCounterRef());
	// ARG 10 globalQueue
	context->setKernelArg(scatterKernel, 10, globalQueue.getBundleQueueRef());
	// ARG 11 localQueueCounts
	context->setKernelArg(scatterKernel, 11, localQueue.getEntryCounterRef());
	// ARG 12 localQueue
	context->setKernelArg(scatterKernel, 12, localQueue.getEntryQueueRef());
	// ARG 13 delayBits
	context->setKernelArg(scatterKernel, 13, synapses.getDelaysBitmapRef());
}

template<typename floatT>
void Simulation<floatT>::prepGatherKernel()
{
	// ARG 0  cycle
	// ARG 1 partitionCount
	context.get()->setKernelArg(gatherKernel, 1, totalPartitionCount, "totalPartitionCount");
	// ARG 2 partitionSizes
	context.get()->setKernelArg(gatherKernel, 2, neurons.getTotalPartitionSizesRef());
	// ARG 3 current
	context.get()->setKernelArg(gatherKernel, 3, currents.getCurrentsRef());
	// ARG 4 globalQueue Pitch
	context->setKernelArg(gatherKernel, 4, globalQueue.getPitch(), "globalQeueuPitch");
	// ARG 5 globalQueue counter
	context->setKernelArg(gatherKernel, 5, globalQueue.getBundleCounterRef());
	// ARG 6 globalQueue
	context->setKernelArg(gatherKernel, 6, globalQueue.getBundleQueueRef());
	// ARG 7 targets
	context->setKernelArg(gatherKernel, 7, synapses.getTargetsRef());
	// ARG 8 weights
	context->setKernelArg(gatherKernel, 8, synapses.getWeightsRef());
}

template<typename floatT>
void Simulation<floatT>::prepKernels()
{
	prepGatherKernel();
	prepScatterKernel();
	prepUpdateKernel();
	if(hasInputs) {prepInputKernel();};
}

template<typename floatT>
void Simulation<floatT>::prepareSimulation()
{
	prepKernels();
	timer.resetTimer();

}
template<typename floatT>
void Simulation<floatT>::setKernelsCycle(unsigned long _cycle)
{
	context->setKernelArg(gatherKernel, 0, _cycle, "cycle");
	context->setKernelArg(updateKernel, 0, _cycle, "cycle");
	if(hasInputs) {context->setKernelArg(inputKernel, 0, _cycle, "cycle"); };
	context->setKernelArg(scatterKernel, 0, _cycle, "cycle");
} 

template<typename floatT>
void Simulation<floatT>::pauseTimeCount()
{
	if (timer.pauseTimer() == SzsnnTimer::SZSNN_TIMER_ERROR)
	{
		THROW_SZSNNEX("SZSNN::Simulation::pauseTimeCount() error: Timer worng state "
			<< timer.getState());
	}
}

template<typename floatT>
void Simulation<floatT>::continueTimeCount()
{
	if (timer.continueTimer() == SzsnnTimer::SZSNN_TIMER_ERROR)
	{
		THROW_SZSNNEX("SZSNN::Simulation::continueTimeCount() error: Timer worng state "
			<< timer.getState());
	}
}

template<typename floatT>
void Simulation<floatT>::setFiringInput(const firing_input& firing)
{
	cl::Event e;
	spikes.setInput(firing, e);
	//setUpdateKernelInput();
}

template<typename floatT>
void Simulation<floatT>::setFiringInput()
{
	spikes.setInput();
}

template<typename floatT>
void Simulation<floatT>::prefire()
{
	cl::Event ndrEvt;
	context.get()->enqueueKernel(gatherKernel, 0, &ndrEvt);

	cl_int eventStatus = CL_QUEUED;
	cl_int status;
	status = ndrEvt.wait();
	CHECK_CL_SUCCESS(status, "Neurosim::run: updateKernel wait() error status");
	/*while(eventStatus != CL_COMPLETE)
	{
		status = ndrEvt.getInfo<cl_int>(CL_EVENT_COMMAND_EXECUTION_STATUS, &eventStatus);
		std::cout << "CURRNET STATUS gather " << eventStatus << std::endl;
		CHECK_CL_SUCCESS(status, "Neurosim::run: cl:Event.getInfo(CL_EVENT_COMMAND_EXECUTION_STATUS)"
			" failed");
	}*/
	context->getCommandQueue().finish();
	context->getCommandQueue().flush();
}

template<typename floatT>
void Simulation<floatT>::fire()
{
	cl::Event ndrEvt;
	context.get()->enqueueKernel(updateKernel, 0, &ndrEvt);
	if(hasInputs) {context.get()->enqueueKernel(inputKernel, 0, &ndrEvt);};
	cl_int eventStatus = CL_QUEUED;
	cl_int status;
	status = ndrEvt.wait();
	CHECK_CL_SUCCESS(status, "Neurosim::run: updateKernel wait() error status");
	while(eventStatus != CL_COMPLETE)
	{
		status = ndrEvt.getInfo<cl_int>(CL_EVENT_COMMAND_EXECUTION_STATUS, &eventStatus);
		//std::cout << "CURRNET STATUS update " << eventStatus << std::endl;
		CHECK_CL_SUCCESS(status, "Neurosim::run: cl:Event.getInfo(CL_EVENT_COMMAND_EXECUTION_STATUS)"
			" failed");
	}
	context->getCommandQueue().finish();
	context->getCommandQueue().flush();
}

template<typename floatT>
void Simulation<floatT>::postfire()
{
	cl::Event ndrEvt;
	context.get()->enqueueKernel(scatterKernel, 0, &ndrEvt);
	cl_int eventStatus = CL_QUEUED;
	cl_int status;
	ndrEvt.wait();
	while(eventStatus != CL_COMPLETE)
	{
		status = ndrEvt.getInfo<cl_int>(CL_EVENT_COMMAND_EXECUTION_STATUS, &eventStatus);
		CHECK_CL_SUCCESS(status, "Neurosim::run: cl:Event.getInfo(CL_EVENT_COMMAND_EXECUTION_STATUS)"
			" failed");
	}
	context->getCommandQueue().finish();
	context->getCommandQueue().flush();
	timer.step();
}

template<typename floatT>
void  Simulation<floatT>::setUpdateKernelInput()
{
	context.get()->setKernelArg(updateKernel, 5, 0, "isCurrentInput");
	context.get()->setKernelArg(updateKernel, 8, spikes.getIsInputSet(), "isFiringInput");
	context.get()->setKernelArg(inputKernel, 5, 0, "isCurrentInput");
	context.get()->setKernelArg(inputKernel, 8, spikes.getIsInputSet(), "isFiringInput");
}
template<typename floatT>
void Simulation<floatT>::step()
{
	continueTimeCount();
	setKernelsCycle(timer.getSimualtionTime());
	prefire();

	
	setFiringInput();
	//TODO setCurrentInput
	setUpdateKernelInput();
	fire();
	postfire();
}

template<typename floatT>
void Simulation<floatT>:: step(const firing_input& fInput)
{
	continueTimeCount();
	setKernelsCycle(timer.getSimualtionTime());
	prefire();
	
	setFiringInput(fInput);
	setUpdateKernelInput();
	fire();
	postfire();
}


template<typename floatT>
const typename Simulation<floatT>::firing_output& Simulation<floatT>::stepAndFired()
{
	continueTimeCount();
	setKernelsCycle(timer.getSimualtionTime());
	prefire();

	
	setFiringInput();
	//TODO setCurrentInput
	setUpdateKernelInput();
	fire();
	postfire();
	cl::Event e;
	return spikes.getOutput(e);
}

template<typename floatT>
const typename Simulation<floatT>::firing_output& Simulation<floatT>::stepAndFired(const firing_input& fInput)
{
	continueTimeCount();
	setKernelsCycle(timer.getSimualtionTime());
	prefire();
	
	setFiringInput(fInput);
	setUpdateKernelInput();
	fire();
	postfire();
	cl::Event e;
	return spikes.getOutput(e);

}




} /* namespace szsnn */
