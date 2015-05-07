

namespace szsnn {


template<typename floatT>
template<typename NeuronModelT, typename InputModleT>
void SynapseData<floatT>::initialize(const NetworkGraph<NeuronModelT, InputModleT>& graph)
{
	totalPartitionCount = graph.getTotalPartitionCount();
	maxDelay = graph.getMaxDelay()-1;


	std::vector<std::string> targetsKernelNames;
	targetsKernelNames.push_back("gather");

	std::vector<std::string> weightsKernelNames;
	weightsKernelNames.push_back("gather");

	std::vector<neuronIdx_t> 	tempTargets;
	std::vector<floatT>			tempWeights;

	SynapseIndex index;
	size_t nextFreeBundle = 1;
	for(typename NetworkGraph<NeuronModelT, InputModleT>::neuron_synapse_iterator ins = graph.neuron_synapse_begin();
			ins != graph.neuron_synapse_end(); ++ins)
	{
		for(typename NetworkGraph<NeuronModelT, InputModleT>::synapse_iterator is = ins->begin();
				is != ins->end(); ++is)
		{
			const Synapse<floatT>& s = *is;
#if SZSNN_DEBUG
			std::cout << "SZSNN::SynapseData: adding synapse: " << is->sourceNeuronIdx << " -> "
				<< is->targetNeuronIdx << " ("<<is->delay<<", "<<is->weight<<")" << std::endl;
#endif
			addSynapse(s, nextFreeBundle, index, tempTargets, tempWeights);

		}
	}

	targets = DataBuffer<neuronIdx_t>(tempTargets, CL_MEM_READ_WRITE, "synapseTargets",
			targetsKernelNames);

	weights = DataBuffer<floatT>(tempWeights, CL_MEM_READ_WRITE, "synapseWeights",
			weightsKernelNames);

	getContext()->createBuffer(targets);
	getContext()->createBuffer(weights);




	size_t delaysBitmapSize = graph.getNetworkSize();
	std::vector<cl_ulong>	tempDelaysBitmap(delaysBitmapSize, 0);
	std::vector<std::string> delayBitmapKernelNames;
	delayBitmapKernelNames.push_back("scatter");

	setDelaysBitmap(index, tempDelaysBitmap);

	delaysBitmap = DataBuffer<cl_ulong>(tempDelaysBitmap, CL_MEM_READ_WRITE, "delaysBitmap",
			delayBitmapKernelNames);
	getContext()->createBuffer(delaysBitmap);

	groups.initialize(totalPartitionCount, index);
}



template<typename floatT>
void SynapseData<floatT>::setDelaysBitmap(
		const SynapseIndex& index,  std::vector<cl_ulong>& _delaysBitmap)
{
	using namespace boost::tuples;

	for(SynapseIndex::iterator it = index.begin(); it != index.end(); ++it) {
		const SynapseIndex::source_key key = it->first;
		partitionIdx_t p = get<0>(key);
		neuronIdx_t n = get<1>(key);
		delay_t delay1 = get<2>(key);
		cl_ulong bits = _delaysBitmap[LOCAL_NEURON_IDX_TO_GLOBAL(p, n)];
		bits |= (cl_ulong(0x1) << cl_ulong(delay1-1));
		_delaysBitmap[LOCAL_NEURON_IDX_TO_GLOBAL(p, n)] = bits;
	}
}

template<typename floatT>
void SynapseData<floatT>::addSynapse(
		const Synapse<floatT>& 		synapse,
		size_t& 					nextFreeBundle,
		SynapseIndex& 				index,
		std::vector<neuronIdx_t>&	_targets,
		std::vector<floatT>&		_weights)
{
	partitionIdx_t p = GLOBAL_NEURON_IDX_TO_PARTITION(synapse.sourceNeuronIdx);
	neuronIdx_t n = GLOBAL_NEURON_IDX_TO_LOCAL(synapse.sourceNeuronIdx);
	partitionIdx_t tp = GLOBAL_NEURON_IDX_TO_PARTITION(synapse.targetNeuronIdx);
	neuronIdx_t tn = GLOBAL_NEURON_IDX_TO_LOCAL(synapse.targetNeuronIdx);
	synapse_addr_t addr = index.addSynapse(p, n, tp, synapse.delay, nextFreeBundle);

	if(addr.synapse == 0 && addr.row == nextFreeBundle)
	{
		nextFreeBundle += 1;
		_targets.resize(nextFreeBundle * BUNDLE_SIZE, 0);
		_weights.resize(nextFreeBundle * BUNDLE_SIZE, 0);
	}

	size_t idx = addr.row * BUNDLE_SIZE + addr.synapse;
	//assert(tn != 0);
	_targets[idx] = tn;
	_weights[idx] = synapse.weight;
}

template<typename floatT>
void SynapseData<floatT>::specUploadToDevice()
{
	groups.uploadToDevice();
	getContext()->writeBuffer(delaysBitmap, CL_TRUE);
	getContext()->writeBuffer(weights, CL_TRUE);
	getContext()->writeBuffer(targets, CL_TRUE);
}

template<typename floatT>
void SynapseData<floatT>::specDownloadFromDevice()
{
	groups.downloadFromDevice();
	getContext()->readBuffer(delaysBitmap, CL_TRUE);
	getContext()->readBuffer(weights, CL_TRUE);
	getContext()->readBuffer(targets, CL_TRUE);
}
template<typename floatT>
void SynapseData<floatT>::specFreeHostData()
{
	groups.freeHostData();
	delaysBitmap.freeHostData();
	weights.freeHostData();
	targets.freeHostData();
}




/****************************
 * prints
 */

template<typename floatT>
std::string SynapseData<floatT>::printDelaysBitmap()
{
	std::stringstream ss;
	ss << std::endl << "DELAY_BITS >>>>" << std::endl;

	ss << "     ";
	for(int j = 8*sizeof(cl_ulong) - 1; j !=-1; --j)
	{
		if( j > 9)
		{ ss << j << " "; }
		else
		{ ss << j << "  "; }
	}
	ss << std::endl;

	unsigned idx = 0;
	for(DataBuffer<cl_ulong>::iterator i = delaysBitmap.begin();
			i != delaysBitmap.end(); ++i)
	{

		std::string nb = boost::lexical_cast<std::string>(idx);
		std::string sp(5 - nb.length(), ' ');
		//=  " " * (5 - nb.length());
		ss << nb << sp;
		cl_ulong bits = (*i);
		for(int j = 8*sizeof(cl_ulong) - 1; j !=-1; --j)
		{
			bool b = ((bits >> j) & 0x1);
			ss << b << "  ";
		}
		ss << std::endl;
		idx++;
	}

	ss << "     ";
	for(int j = 8*sizeof(cl_ulong) - 1; j !=-1; --j)
	{
		if( j > 9)
		{ ss << j << " "; }
		else
		{ ss << j << "  "; }
	}

	ss << std::endl;
	return ss.str();
}

template<typename floatT>
std::string SynapseData<floatT>::printTargets()
{
	std::stringstream ss;
	ss << std::endl << "PRINT TARGETS BUNDLED >>>";
	for(unsigned i = 0; i < targets.getSize(); ++i)
	{
		if((i % BUNDLE_SIZE) == 0) ss << std::endl << "[" << (i / BUNDLE_SIZE) << "]" ;
		ss << targets.data[i] << " ";
	}
	return ss.str();
}

template<typename floatT>
std::string SynapseData<floatT>::printWeights()
{
	std::stringstream ss;
	ss << std::endl << "PRINT WEIGHTS BUNDLED >>>";
	for(unsigned i = 0; i < weights.getSize(); ++i)
	{
		if((i % BUNDLE_SIZE) == 0) ss << std::endl << "[" << (i / BUNDLE_SIZE) << "]" ;
		ss << weights.data[i] << " ";
	}
	return ss.str();
}

} /* namespace szsnn */
