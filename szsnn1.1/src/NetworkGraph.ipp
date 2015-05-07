/**
 * \file NetworkGraph.cpp
 *
 * \date 22-11-2012
 * \author pawel
 * \description
 */
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif


namespace szsnn {

template<typename NeuronModelT, typename InputModelT>
unsigned NetworkGraph<NeuronModelT, InputModelT>::getRandomFromRage(unsigned lowest, unsigned highest)
{
	//std::cout << "RANDOM FORM " << lowest << " TO " << highest << std::endl;
	if(lowest > highest)
	{
		THROW_SZSNNEX("NetworkGraph<NeuronModelT, InputModelT>::getRandomFromRage: lowest=["<< lowest
				<< "] > highest=[" << highest << "]");
	}

	if (lowest == highest)
	{
		return lowest;
	}

	unsigned range = (highest - lowest) + 1;
	//return lowest + unsigned(range*rand()/(RAND_MAX + 1.0));
	unsigned retrn = lowest + (unsigned) ((double)rand() / (RAND_MAX + 1.0) * range);
	//std::cout << "RANDOM RETURNED " << retrn << std::endl;
	return retrn;

}

template<typename NeuronModelT, typename InputModelT>
void NetworkGraph<NeuronModelT, InputModelT>::fillRandomSet(
	std::set<unsigned>& _set,
	unsigned size,
	unsigned lowest,
	unsigned highest)
{
	if(lowest >= highest)
	{
		THROW_SZSNNEX("NetworkGraph<NeuronModelT, InputModelT>::getRandomFromRage: lowest=["<< lowest
				<< "] >= highest=[" << highest << "]");
	}

	unsigned range = (highest - lowest) + 1;

	if(size > range)
	{
		THROW_SZSNNEX("NetworkGraph<NeuronModelT, InputModelT>::getRandomFromRage: size=["<< size
				<< "] > (highest - lowest) + 1=[" << range << "]");
	}

	unsigned rest = size - range;

	if (rest >= range)
	{
		while(_set.size() < size)
		{
			_set.insert(getRandomFromRage(lowest, highest));
		}
	}
	else
	{
		for (unsigned i = lowest; i <= highest; ++i)
		{
			_set.insert(i);
		}

		while(_set.size() > size)
		{
			_set.erase(getRandomFromRage(lowest, highest));
		}
	}
}

template<typename NeuronModelT, typename InputModelT>
void NetworkGraph<NeuronModelT, InputModelT>::updateNeuronPartitionSizes()
{
	partitionSizes.clear();
	partitionSizes.resize(getNeuronPartitionCount(), 0);
	std::fill(partitionSizes.begin(), partitionSizes.end(), MAX_PARTITION_SIZE);
	partitionSizes.back() = (GLOBAL_NEURON_IDX_TO_LOCAL(neurons.size() - 1) + 1);
}

template<typename NeuronModelT, typename InputModelT>
NetworkGraph<NeuronModelT, InputModelT>::NetworkGraph():
		inputConnected(false),
		neuronCount(0),
		maxConnectionsPerNeuron(0),
		minConnectionsPerNeuron(0),
		density(0),
		synapsePerNeuronAvg(0),
		gabaSynapseCount(0),
		ampaSynapseCount(0),
		gabaPrt(0.0),
		minDelay(0),
		maxDelay(0),
		neurons(),
		synapses(),
		inputs()
			{};

//NetworkGraph<NeuronModelT, InputModelT>::~NetworkGraph() {
//}
template<typename NeuronModelT, typename InputModelT>
void NetworkGraph<NeuronModelT, InputModelT>::createRandomNetwork(
		unsigned _neuronCount,
		unsigned _inhibNeuronCount,
		unsigned _maxConnectionsPerNeuron,
		unsigned _minConnectionsPerNeuron,
		double _synapsePerNeuronAvg,
		double _density,
		//double _gabaPt,
		delay_t	_minDelay,
		delay_t	_maxDelay,
		unsigned seed)
{
	srand(seed);

	if (_neuronCount > MAX_NETWORK_SIZE)
	{
		THROW_SZSNNEX("NetworkGraph<NeuronModelT, InputModelT>::createRandomNetwork: _neuronCount=["
				<< _neuronCount << "] > MAX_NETWORK_SIZE=[" << MAX_NETWORK_SIZE
				<< "]");
	}

	if (_neuronCount < _inhibNeuronCount)
	{
		THROW_SZSNNEX("NetworkGraph<NeuronModelT, InputModelT>::createRandomNetwork: _neuronCount=["
				<< _neuronCount << "] < _inhibNeuronCount=[" << _inhibNeuronCount
				<< "]");
	}

	neuronCount	 		= _neuronCount;
	inhibNeuronCount	= _inhibNeuronCount;

	if (_maxConnectionsPerNeuron > MAX_CONNECTIONS_PER_NEURON)
	{
		THROW_SZSNNEX("NetworkGraph<NeuronModelT, InputModelT>::createRandomNetwork: _maxConnectionsPerNeuront=["
				<< _maxConnectionsPerNeuron << "] > MAX_CONNECTIONS_PER_NEURON=["
				<< MAX_CONNECTIONS_PER_NEURON << "]");
	}

	maxConnectionsPerNeuron = _maxConnectionsPerNeuron;
	minConnectionsPerNeuron = _minConnectionsPerNeuron;

	synapsePerNeuronAvg = _synapsePerNeuronAvg;

	if ((_density <= 0) || (_density > 1))
	{
		THROW_SZSNNEX("NetworkGraph<NeuronModelT, InputModelT>::createRandomNetwork: _density=["
				<< _density << "] not in (0, 1]");
	}
	density = _density;

	if (_maxDelay > MAX_SYNAPTIC_DELAY)
	{
		THROW_SZSNNEX("NetworkGraph<NeuronModelT, InputModelT>::createRandomNetwork: _maxDelay=["
				<< _maxDelay << "] >  MAX_SYNAPTIC_DELAY=["
				<< MAX_SYNAPTIC_DELAY << "]");
	}

	if (_minDelay < MIN_SYNAPTIC_DELAY)
	{
		THROW_SZSNNEX("NetworkGraph<NeuronModelT, InputModelT>::createRandomNetwork: _minDelay=["
				<< _minDelay << "] <  MIN_SYNAPTIC_DELAY=["
				<< MIN_SYNAPTIC_DELAY << "]");
	}

	minDelay = _minDelay;
	maxDelay = _maxDelay;

	neurons = std::vector<NeuronModelT>(neuronCount, NeuronModelT());
	synapses = std::vector< std::vector<Synapse<float_type> > >(
			neuronCount, std::vector<Synapse<float_type> >());


	unsigned long long maxCon = neuronCount * (neuronCount - 1);
	unsigned long long targetConCount = (unsigned long long)(density * maxCon);
	//unsigned long long currConCount = 0;

	std::set<std::pair<unsigned, unsigned> > connections =
			std::set<std::pair<unsigned, unsigned> >();

	std::vector<std::pair<unsigned, unsigned> > connCounters =
			std::vector<std::pair<unsigned, unsigned> >(
					neuronCount, std::pair<unsigned, unsigned>(0, 0));

	unsigned max_draws = 10000;

	/* minimalna ilość połączeń */
	for(unsigned i = 0; i < neuronCount; ++i)
	{
		//std::cout << neuronCount << " neuronCount "<< std::endl;

		unsigned target = i;
		unsigned draws = 0;
		std::pair<unsigned, unsigned> con;
		Synapse<float_type> sData;
		con.first = i;

		for(unsigned j = 0; j < minConnectionsPerNeuron; ++j)
		{
			target = i;
			draws = 0;
			//std::cout << minConnectionsPerNeuron << " minConnectionsPerNeuron "<< std::endl;

			while (draws < max_draws)
			{
				target = getRandomFromRage(0, neuronCount-1);
				//std::cout << i << " -> " << target << std::endl;


				if (target != i)
				{
					con.second = target;
					if (connections.insert(con).second)
					{
						//std::cout << "dodaje" <<std::endl;

						connCounters[i].first++;
						connCounters[target].second++;
						sData.delay = getRandomFromRage(minDelay, maxDelay);
						sData.sourceNeuronIdx = i;
						sData.targetNeuronIdx = target;
						sData.synapseIdx = connCounters[i].first - 1;
						sData.weight = 0;
						synapses[i].push_back(sData);
						//std::cout << "dodalem" <<std::endl;

						break;
					}
				}
				draws++;
			}

			//std::cout << "skonczyl" <<std::endl;
			if (draws == max_draws)
			{
				THROW_SZSNNEX("NetworkGraph<NeuronModelT, InputModelT>::createRandomNetwork: couldn't draw"
						<< " connection from neuron " << i << " connection number "
						<< j);
			}
		}
	}

	max_draws = 100000;
	unsigned draw = 0;

	while( (connections.size() < targetConCount) && (draw < max_draws) )
	{
		draw = 0;
		unsigned source = 0, target = 0;
		std::pair<unsigned, unsigned> con;
		Synapse<float_type> sData;

		while(draw < max_draws)
		{
			source = getRandomFromRage(0, neuronCount-1);
			target = getRandomFromRage(0, neuronCount-1);

			if(source == target)
			{
				++draw;
				continue;
			}

			if( (connCounters[source].first >=  maxConnectionsPerNeuron) ||
					(connCounters[target].second >=  maxConnectionsPerNeuron) )
			{
				++draw;
				continue;
			}
			con.first = source;
			con.second = target;
			if(connections.insert(con).second)
			{
				connCounters[source].first++;
				connCounters[target].second++;
				sData.delay = getRandomFromRage(minDelay, maxDelay);
				sData.sourceNeuronIdx = source;
				sData.targetNeuronIdx = target;
				sData.synapseIdx = connCounters[source].first - 1;
				sData.weight = 0;
				synapses[source].push_back(sData);
				break;
			}
			++draw;
		}
		if (draw == max_draws)
		{
			THROW_SZSNNEX("NetworkGraph<NeuronModelT, InputModelT>::createRandomNetwork: couldn't draw"
					<< " connections to create " << targetConCount - connections.size()
					<< " out of " << targetConCount);
		}
	}

	std::set<unsigned> inhibitory = std::set<unsigned>();
	fillRandomSet(inhibitory, inhibNeuronCount, 0, neuronCount-1);


	for(unsigned i = 0; i < neuronCount; ++i)
	{
		bool isInhib = inhibitory.find(i) != inhibitory.end();
		NeuronModelT neuron;
		neuron.randomizeMe(isInhib);
//		NeuronModelT::base_neuron_type type = (isInhib ?
//				NeuronModelT::base_type::INHIBITORY :
//				NeuronModelT::base_type::EXCITATORY);
		neurons[i] = neuron;
		for(typename std::vector<Synapse<float_type> >::iterator it = synapses[i].begin();
				it != synapses[i].end(); ++it)
		{
			if(isInhib)
			{
				(*it).weight = -1;
			}
			else
			{
				(*it).weight = 1;
			}
		}


	}
	updateNeuronPartitionSizes();
}


template<typename NeuronModelT, typename InputModelT>
std::string NetworkGraph<NeuronModelT, InputModelT>::neuronsToString()
{
	std::stringstream ss;
	for(unsigned i = 0; i < neurons.size(); ++i)
	{
		ss << "Idx [ " << i << " ], param1 = " << neurons[i].getParam(0) << std::endl;
	}
	return ss.str();
}
template<typename NeuronModelT, typename InputModelT>
std::string NetworkGraph<NeuronModelT, InputModelT>::synapsesToString()
{
	std::stringstream ss;
	for(unsigned i = 0; i < synapses.size(); ++i)
	{
		ss << "Idx [ " << i << " ] ";
		for(unsigned j = 0; j < synapses[i].size(); ++j)
		{
			ss << j << "[ " << synapses[i][j].sourceNeuronIdx << " -("
					<< synapses[i][j].weight << ", " << synapses[i][j].delay
					<< ")-> " << synapses[i][j].targetNeuronIdx << "]   ";
		}
		ss << std::endl;
	}
	return ss.str();
}

template<typename NeuronModelT, typename InputModelT>
unsigned NetworkGraph<NeuronModelT, InputModelT>::addNeuron(NeuronModelT neuron)
{
	if(inputConnected)
	{
		THROW_SZSNNEX("SZSNN::NetworkGraph::addNeuron(neuron) error: "
				<< "cant execute, after input connection");
	}
	neurons.push_back(neuron);
	synapses.push_back(std::vector<Synapse<float_type> >());
	neuronCount = neurons.size();
	updateNeuronPartitionSizes();
	return neuronCount-1;
}

template<typename NeuronModelT, typename InputModelT>
unsigned NetworkGraph<NeuronModelT, InputModelT>::addNeuron(size_t neuronIdx, NeuronModelT neuron)
{
	if(inputConnected)
	{
		THROW_SZSNNEX("SZSNN::NetworkGraph::addNeuron(idx, neuron) error: "
				<< "cant execute, after input connection");
	}

	if(neurons.size() > neuronIdx)
	{
		neurons[neuronIdx] = neuron;
		return neuronCount-1;
	}

	if(neurons.size() < neuronIdx)
	{
		THROW_SZSNNEX("SZSNN::NetworkGraph::addNeuron(idx, neuron) error: "
				<< "cant add neuron with ID=`"<<neuronIdx<<"`, ID's must"
				<< " be consistance, last neuron id =`"<<neuronCount-1<<"`");
	}
	neurons.push_back(neuron);
	synapses.push_back(std::vector<Synapse<float_type> >());
	neuronCount = neurons.size();
	updateNeuronPartitionSizes();
	return neuronCount-1;
}


//TODO neuronID potrzeben? synapse.synapseIdx ??
template<typename NeuronModelT, typename InputModelT>
unsigned NetworkGraph<NeuronModelT, InputModelT>::addSynapse(unsigned neuronId, Synapse<float_type> synapse)
{
	synapse.delay += 1;
	synapses[synapse.sourceNeuronIdx].push_back(synapse);
	minDelay = std::min(minDelay, synapse.delay);
	maxDelay = std::max(maxDelay, synapse.delay);
	return synapses[neuronId].size();
}

template<typename NeuronModelT, typename InputModelT>
void NetworkGraph<NeuronModelT, InputModelT>::addSynapse(Synapse<float_type> synapse)
{
// min max

	if(neurons.size() > synapse.sourceNeuronIdx &&
			neurons.size() > synapse.targetNeuronIdx)
	{
		synapse.delay += 1;
		synapses[synapse.sourceNeuronIdx].push_back(synapse);
		minDelay = std::min(minDelay, synapse.delay);
		maxDelay = std::max(maxDelay, synapse.delay);
	}
	else {
		THROW_SZSNNEX("SZSNN::NetworkGraph::addSynapse(syn) error:"
				<< " cant add synapse connecting nonexisting neurons"
				<< " sourceID=`"<<synapse.sourceNeuronIdx<< "` targetID=`"
				<< synapse.targetNeuronIdx <<"`, last neuron in network ID=`"
				<< neurons.size() - 1<<"`");
	}

}
template<typename NeuronModelT, typename InputModelT>
unsigned NetworkGraph<NeuronModelT, InputModelT>::addInput(InputModelT input)
{
	if(inputConnected)
	{
		THROW_SZSNNEX("SZSNN::NetworkGraph::addInput(input) error: "
				<< "cant execute, after input connection");
	}

	if (inputs.size() == MAX_INPUTS_SIZE)
	{
		THROW_SZSNNEX("SZSNN::NetworkGraph::addInput(input) error:"
						<< " cant add input, max input size is " << MAX_INPUTS_SIZE);
	}

	inputs.push_back(input);
	inputSynapses.push_back(std::vector<Synapse<float_type> >());
	return inputs.size()-1;
}


template<typename NeuronModelT, typename InputModelT>
unsigned NetworkGraph<NeuronModelT, InputModelT>::
addInput(size_t inputId, InputModelT input)
{
	if(inputs.size() > inputId)
	{
		inputs[inputId] = input;
		return inputs.size()-1;
	}

	if(neurons.size() < inputId)
	{
		THROW_SZSNNEX("SZSNN::NetworkGraph::addInput(idx, input) error: "
				<< "cant add input with ID=`"<<inputId<<"`, ID's must"
				<< " be consistance, last inputId id =`"<<inputs.size()-1<<"`");
	}
	return addInput(input);
}


template<typename NeuronModelT, typename InputModelT>
void NetworkGraph<NeuronModelT, InputModelT>::
addInputSynapse(Synapse<float_type> synapse)
{

	if(inputs.size() > synapse.sourceNeuronIdx &&
			neurons.size() > synapse.targetNeuronIdx)
	{
		std::cout << "ADDING INPUT SYNAPSE" << std::endl;
		synapse.delay += 1;
		inputSynapses[synapse.sourceNeuronIdx].push_back(synapse);
		minDelay = std::min(minDelay, synapse.delay);
		maxDelay = std::max(maxDelay, synapse.delay);
	}
	else {
		THROW_SZSNNEX("SZSNN::NetworkGraph::addInputSynapse(syn) error:"
				<< " cant add synapse connecting nonexisting neurons"
				<< " sourceID=`"<<synapse.sourceNeuronIdx<< "` targetID=`"
				<< synapse.targetNeuronIdx <<"`, last input in network ID=`"
				<< inputs.size() - 1 << ", last neuron in network ID=`"
				<< neurons.size() - 1<<"`");
	}
}

template<typename NeuronModelT, typename InputModelT>
void NetworkGraph<NeuronModelT, InputModelT>::transferInputSynapse(const Synapse<float_type>& synapse, cl_uint inputOffset)
{
	//synapse.sourceNeuronIdx += inputOffset;
	synapses.back().push_back(synapse);
	synapses.back().back().sourceNeuronIdx += inputOffset;
}


template<typename NeuronModelT, typename InputModelT>
void NetworkGraph<NeuronModelT, InputModelT>::connectInput()
{

	cl_uint inputOffset = getInputsIDOffset();

	for(neuron_synapse_iterator isIt = inputSynapses.begin(); isIt != inputSynapses.end();
			++isIt)
	{
		synapses.push_back(std::vector<Synapse<float_type> >());
		for(synapse_iterator sIt = isIt->begin(); sIt != isIt->end(); ++sIt)
		{
			transferInputSynapse(*sIt, inputOffset);
		}
	}

	inputConnected = true;
}

} /* namespace szsnn */
