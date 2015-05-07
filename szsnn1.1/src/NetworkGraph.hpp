/**
 * \file NetworkGraph.hpp
 *
 * \date 22-11-2012
 * \author pawel
 * \description
 */

#ifndef NETWORKGRAPH_HPP_
#define NETWORKGRAPH_HPP_

#ifdef _WIN32
#define NOMINMAX
#endif
#include "opencl_header.h"
#include <sstream>
#include <string>
#include <set>
#include <cstdlib>
#include <ctime>
#include <map>

#include <iostream>

#include "locales.h"
#include "types.h"
#include "utils.h"
#include "models/NeuronModel.hpp"
#include "models/neuron_model_traits.hpp"
#include "synapse_model.h"

namespace szsnn {

/*
 *
 */
template <typename NeuronModelT, typename InputModelT>
class NetworkGraph {
public:
	typedef typename neuron_model_traits<NeuronModelT>::float_type float_type;
private:
	void fillRandomSet(
		std::set<unsigned>& _set,
		unsigned size,
		unsigned lowest,
		unsigned highest);

	std::vector<cl_uint> partitionSizes; //TODO głupie, potrzebny mapper

	bool inputConnected;

	void updateNeuronPartitionSizes();

	void transferInputSynapse(const Synapse<float_type>& synapse, cl_uint inputOffset);

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

	//cl_uint getNeuronCount() const {return neuronCount;};
	cl_uint getNetworkSize() const
	{
		if(getInputPartitionCount() == 0)
		{
			return neurons.size();
		}
		return MAX_PARTITION_SIZE * getNeuronPartitionCount() + getInputPartitionSize();
	}

	delay_t getMaxDelay() const {return maxDelay;};
	delay_t getMinDelay() const {return minDelay;};

	std::vector<NeuronModelT> neurons;
	std::vector<InputModelT> inputs;

	std::vector< std::vector<Synapse<float_type> > > synapses;

	std::vector< std::vector<Synapse<float_type> > > inputSynapses;

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

	unsigned addNeuron(NeuronModelT neuron);
	unsigned addNeuron(size_t neuronId, NeuronModelT neuron);

	unsigned addInput(InputModelT input);
	unsigned addInput(size_t inputId, InputModelT input);


	void addSynapse(Synapse<float_type> synapse);
	unsigned addSynapse(unsigned neuronId, Synapse<float_type> synapse);

	void addInputSynapse(Synapse<float_type> synapse);
	//unsigned addInputSynapse(unsigned neuronId, Synapse<float_type> synapse);

	void _addPhonySynapse()
	{
		Synapse<float_type> synapse;
		synapse.targetNeuronIdx = 0;
		synapse.sourceNeuronIdx = 0;
		synapse.weight = 0;
		synapse.delay = 1;
		addSynapse(synapse);
	}

	typedef typename std::vector<NeuronModelT>::const_iterator neuron_iterator;
	neuron_iterator neuron_begin() const {return neurons.begin(); };
	neuron_iterator neuron_end() const {return neurons.end(); };

	typedef typename std::vector<InputModelT>::const_iterator input_iterator;
	input_iterator input_begin() const {return inputs.begin(); };
	input_iterator input_end() const {return inputs.end(); };

	typedef typename std::vector< std::vector<Synapse<float_type> > >::const_iterator neuron_synapse_iterator;
	neuron_synapse_iterator neuron_synapse_begin() const {return synapses.begin();};
	neuron_synapse_iterator neuron_synapse_end() const {return synapses.end();};


	typedef typename std::vector<Synapse<float_type> >::const_iterator synapse_iterator;


	cl_uint getNeuronPartitionCount() const
	{
		return GLOBAL_NEURON_IDX_TO_PARTITION(neurons.size() - 1) + 1;
	};

	cl_uint getInputPartitionCount() const
	{
		return getInputPartitionSize() ? 1 : 0;
	}

	cl_uint getTotalPartitionCount() const
	{
		return getNeuronPartitionCount() + getInputPartitionCount();
	};

	const std::vector<cl_uint>& getNeuronPartitionSizes() const
	{
		return partitionSizes;
	};

	std::vector<cl_uint> getInputPartitionSizes() const
	{
		std::vector<cl_uint> partSizes;
		partSizes.push_back(inputs.size());
		return partSizes;
	}

	cl_uint getInputPartitionSize() const
	{
		return inputs.size();
	}

	cl_uint getInputsIDOffset() const
	{
		return getNeuronPartitionCount() * MAX_PARTITION_SIZE;
	}

	const std::vector<NeuronModelT>& getNeurons() const {return neurons; };

	const std::vector<InputModelT>& getInputs() const {return inputs; };


	void connectInput();

	std::string neuronsToString();

	std::string synapsesToString();
};

} /* namespace szsnn */

#include "NetworkGraph.ipp"
#endif /* NETWORKGRAPH_HPP_ */
