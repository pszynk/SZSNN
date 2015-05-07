/**
 * \file NetworkGraph.hpp
 *
 * \date 22-11-2012
 * \author pawel
 * \description
 */

#ifndef NETWORKGRAPH_HPP_
#define NETWORKGRAPH_HPP_

#include <CL/opencl.h>
#include <CL/cl.hpp>
#include <sstream>
#include <string>
#include <set>
#include <cstdlib>
#include <ctime>

#include <iostream>

#include "locales.h"
#include "types.h"
#include "neuron_model.h"
#include "synapse_model.h"

namespace szsnn {

/*
 *
 */
class NetworkGraph {
private:
	void fillRandomSet(
		std::set<unsigned>& _set,
		unsigned size,
		unsigned lowest,
		unsigned highest);

//	void fillRandomSet(
//		std::set<std::pair<unsigned, unsigned> >& _set,
//		unsigned size,
//		unsigned lowest,
//		unsigned highest);

public:
	unsigned neuronCount;
	unsigned int maxConnectionsPerNeuron;
	unsigned int minConnectionsPerNeuron;


	unsigned inhibNeuronCount;
	double density;
	double synapsePerNeuronAvg;


	unsigned long long gabaSynapseCount;	//TODO nazwy
	unsigned long long ampaSynapseCount;	//

	double gabaPrt;

	delay_t	minDelay;
	delay_t	maxDelay;

	unsigned getNeuronCount() const {return neuronCount;};

	delay_t getMaxDelay() const {return maxDelay;};
	delay_t getMinDelay() const {return minDelay;};

	std::vector<CPU_neuronType> neurons;
	std::vector< std::vector<synapseData> > synapses;
	NetworkGraph();
	//virtual ~NetworkGraph();

	unsigned getRandomFromRage(unsigned lowest, unsigned highest);

	void createRandomNetwork(
			unsigned _neuronCount,
			unsigned _inhibNeuronCount,
			unsigned _maxConnectionsPerNeuron,
			unsigned _minConnectionsPerNeuron,
			double _synapsePerNeuronAvg,
			double density,
			//double _gabaPt,
			delay_t	_minDelay,
			delay_t	_maxDelay,
			unsigned seed);

	unsigned addNeuron(CPU_neuronType neuron);
	unsigned addSynapse(unsigned neuronId, synapseData synapse);

	typedef std::vector< std::vector<synapseData> >::const_iterator neuron_synapse_iterator;
	neuron_synapse_iterator neuron_synapse_begin() const {return synapses.begin();};
	neuron_synapse_iterator neuron_synapse_end() const {return synapses.end();};

	typedef std::vector<synapseData>::const_iterator synapse_iterator;

	std::pair<std::vector<synapseData>::iterator,
		std::vector<synapseData>::iterator> getNeuronSynapsesBeginEnd(unsigned int neuronIdx);

	std::string neuronsToString();

	std::string synapsesToString();
};

} /* namespace szsnn */
#endif /* NETWORKGRAPH_HPP_ */
