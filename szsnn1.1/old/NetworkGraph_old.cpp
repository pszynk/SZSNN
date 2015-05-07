/**
 * \file NetworkGraph.cpp
 *
 * \date 22-11-2012
 * \author pawel
 * \description
 */

#include "NetworkGraph.hpp"

namespace szsnn {

unsigned NetworkGraph::getRandomFromRage(unsigned lowest, unsigned highest)
{
	//std::cout << "RANDOM FORM " << lowest << " TO " << highest << std::endl;
	if(lowest > highest)
	{
		THROW_SZSNNEX("NetworkGraph::getRandomFromRage: lowest=["<< lowest
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

void NetworkGraph::fillRandomSet(
	std::set<unsigned>& _set,
	unsigned size,
	unsigned lowest,
	unsigned highest)
{
	if(lowest >= highest)
	{
		THROW_SZSNNEX("NetworkGraph::getRandomFromRage: lowest=["<< lowest
				<< "] >= highest=[" << highest << "]");
	}

	unsigned range = (highest - lowest) + 1;

	if(size > range)
	{
		THROW_SZSNNEX("NetworkGraph::getRandomFromRage: size=["<< size
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


NetworkGraph::NetworkGraph():
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
		synapses()
			{};

//NetworkGraph::~NetworkGraph() {
//}
void NetworkGraph::createRandomNetwork(
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
		THROW_SZSNNEX("NetworkGraph::createRandomNetwork: _neuronCount=["
				<< _neuronCount << "] > MAX_NETWORK_SIZE=[" << MAX_NETWORK_SIZE
				<< "]");
	}

	if (_neuronCount < _inhibNeuronCount)
	{
		THROW_SZSNNEX("NetworkGraph::createRandomNetwork: _neuronCount=["
				<< _neuronCount << "] < _inhibNeuronCount=[" << _inhibNeuronCount
				<< "]");
	}

	neuronCount	 		= _neuronCount;
	inhibNeuronCount	= _inhibNeuronCount;

	if (_maxConnectionsPerNeuron > MAX_CONNECTIONS_PER_NEURON)
	{
		THROW_SZSNNEX("NetworkGraph::createRandomNetwork: _maxConnectionsPerNeuront=["
				<< _maxConnectionsPerNeuron << "] > MAX_CONNECTIONS_PER_NEURON=["
				<< MAX_CONNECTIONS_PER_NEURON << "]");
	}

	maxConnectionsPerNeuron = _maxConnectionsPerNeuron;
	minConnectionsPerNeuron = _minConnectionsPerNeuron;

	synapsePerNeuronAvg = _synapsePerNeuronAvg;

	if ((_density <= 0) || (_density > 1))
	{
		THROW_SZSNNEX("NetworkGraph::createRandomNetwork: _density=["
				<< _density << "] not in (0, 1]");
	}
	density = _density;

	if (_maxDelay > MAX_SYNAPTIC_DELAY)
	{
		THROW_SZSNNEX("NetworkGraph::createRandomNetwork: _maxDelay=["
				<< _maxDelay << "] >  MAX_SYNAPTIC_DELAY=["
				<< MAX_SYNAPTIC_DELAY << "]");
	}

	if (_minDelay < MIN_SYNAPTIC_DELAY)
	{
		THROW_SZSNNEX("NetworkGraph::createRandomNetwork: _minDelay=["
				<< _minDelay << "] <  MIN_SYNAPTIC_DELAY=["
				<< MIN_SYNAPTIC_DELAY << "]");
	}

	minDelay = _minDelay;
	maxDelay = _maxDelay;

	neurons = std::vector< CPU_neuronType>(neuronCount, CPU_neuronType());
	synapses = std::vector< std::vector<synapseData> >(
			neuronCount, std::vector<synapseData>());


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
		synapseData sData;
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
				THROW_SZSNNEX("NetworkGraph::createRandomNetwork: couldn't draw"
						<< "connection from neuron " << i << " connection number "
						<< j);
			}
		}
	}

	max_draws = 10000;
	unsigned draw = 0;

	while( (connections.size() < targetConCount) && (draw < max_draws) )
	{
		draw = 0;
		unsigned source = 0, target = 0;
		std::pair<unsigned, unsigned> con;
		synapseData sData;

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
			THROW_SZSNNEX("NetworkGraph::createRandomNetwork: couldn't draw"
					<< "connections to create " << targetConCount - connections.size());
		}
	}

	std::set<unsigned> inhibitory = std::set<unsigned>();
	fillRandomSet(inhibitory, inhibNeuronCount, 0, neuronCount-1);


	for(unsigned i = 0; i < neuronCount; ++i)
	{
		bool isInhib = inhibitory.find(i) != inhibitory.end();
		CPU_neuronType neuron = createRandomNeuron(!isInhib);
		neurons[i] = neuron;
		for(std::vector<synapseData>::iterator it = synapses[i].begin();
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
}



std::string NetworkGraph::neuronsToString()
{
	std::stringstream ss;
	for(unsigned i = 0; i < neurons.size(); ++i)
	{
		ss << "Idx [ " << i << " ], param1 = " << neurons[i].param1 << std::endl;
	}
	return ss.str();
}

std::string NetworkGraph::synapsesToString()
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


unsigned NetworkGraph::addNeuron(CPU_neuronType neuron)
{
	neurons.push_back(neuron);
	synapses.push_back(std::vector<synapseData>());
	neuronCount = neurons.size();
	return neuronCount-1;
}

//TODO neuronID potrzeben?

unsigned NetworkGraph::addSynapse(unsigned neuronId, synapseData synapse)
{
	synapses[synapse.sourceNeuronIdx].push_back(synapse);
	minDelay = std::min(minDelay, synapse.delay);
	maxDelay = std::max(maxDelay, synapse.delay);
	return synapses[neuronId].size();
}

} /* namespace szsnn */
