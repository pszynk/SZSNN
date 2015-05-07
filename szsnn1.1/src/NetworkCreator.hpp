#ifndef NETWORKCREATOR_HPP_
#define NETWORKCREATOR_HPP_

#include <vector>
#include <string>
#include <sstream>

#include "utils.h"
#include "models/neuron_model_traits.hpp"

#include "synapse_model.h"
#include "Population.hpp"
#include "Connection.hpp"
#include "NetworkGraph.hpp"

namespace szsnn {

	namespace construction {


template<typename NeuronModelT, typename InputModelT>
class NetworkCreator {
private:
	typedef typename neuron_model_traits<NeuronModelT>::float_type float_type;
	struct popComp {
	  bool operator() (
			  const Population<NeuronModelT>&leftP,
			  const Population<NeuronModelT>&rightP)
	  { return (leftP.getFirstId()<leftP.getFirstId());}
	};

	struct inPopComp {
	  bool operator() (
			  const Population<InputModelT>&leftP,
			  const Population<InputModelT>&rightP)
	  { return (leftP.getFirstId()<leftP.getFirstId());}
	};

public:
	NetworkCreator():networkSize(0) {}
	void addPopulation(const Population<NeuronModelT>& population)
	{
		if (population.getFirstId() > networkSize)
		{
			THROW_SZSNNEX("SZSNN::CONSTRUCTION::NetworkCreator::addPopulation error:"
					<< " adding population `"<<population.getLabel()<<"` neurons must"
					<< " be consistance: networkSize="<<networkSize<<" firstId="
					<< population.getFirstId());
		}

		networkSize = std::max(networkSize, population.getLastId() + 1);
		populations.push_back(population);
		std::sort(populations.begin(), populations.end(), popComp());
	}

	void addInputPopulation(const Population<InputModelT>& inPopulation)
	{
		if (inPopulation.getFirstId() > inputSize)
		{
			THROW_SZSNNEX("SZSNN::CONSTRUCTION::NetworkCreator::addInputPopulation error:"
					<< " adding input population `"<<inPopulation.getLabel()<<"` inputs must"
					<< " be consistance: inputSize="<<inputSize<<" firstId="
					<< inPopulation.getFirstId());
		}

		inputSize = std::max(inputSize, inPopulation.getLastId() + 1);
		inputPopulations.push_back(inPopulation);
		std::sort(inputPopulations.begin(), inputPopulations.end(), inPopComp());
	}


	size_t addConnection(const Connection<float_type>& connection)
	{
		connections.push_back(connection);
		return connections.size()-1;
	}

	size_t addInputConnection(const Connection<float_type>& connection)
	{
		inputConnections.push_back(connection);
		return inputConnections.size()-1;
	}

	void saveAllConnections()
	{
		std::stringstream ss;
		for(unsigned i = 0; i < connections.size(); ++i)
		{
			ss.str("");
			ss << "connection_"<<i<<".conn";
			connections.at(i).saveToFile(ss.str());
		}
	}

	void fillNetworkGraph(NetworkGraph<NeuronModelT, InputModelT>& graph); //TODO const



private:

	size_t networkSize;
	size_t inputSize;
	std::vector<Population<NeuronModelT> > 	populations;
	std::vector<Population<InputModelT> > 	inputPopulations;
	std::vector<Connection<float_type> > 	connections;
	std::vector<Connection<float_type> > 	inputConnections;

	NetworkCreator(const NetworkCreator<NeuronModelT, InputModelT>& net);
	NetworkCreator<NeuronModelT, InputModelT>& operator=(const NetworkCreator<NeuronModelT, InputModelT>&);
};


template<typename NeuronModelT, typename InputModelT>
void NetworkCreator<NeuronModelT, InputModelT>::
fillNetworkGraph(NetworkGraph<NeuronModelT, InputModelT>& graph)
{
	// ADDING NEURONS
	for(typename std::vector<Population<NeuronModelT> >::iterator popIt = populations.begin();
			popIt != populations.end(); ++popIt)
	{
		size_t startID = popIt->getFirstId();
		for(typename Population<NeuronModelT>::iterator neuronIt = popIt->begin();
				neuronIt != popIt->end(); ++neuronIt)
		{
			size_t neuronID = startID + (neuronIt - popIt->begin());
			graph.addNeuron(neuronID, (*neuronIt));
		}
	}

	// ADDING INPUTS
	for(typename std::vector<Population<InputModelT> >::iterator inPopIt = inputPopulations.begin();
			inPopIt != inputPopulations.end(); ++inPopIt)
	{
		size_t startID = inPopIt->getFirstId();
		for(typename Population<InputModelT>::iterator inputIt = inPopIt->begin();
				inputIt != inPopIt->end(); ++inputIt)
		{
			size_t inputID = startID + (inputIt - inPopIt->begin());
			graph.addInput(inputID, (*inputIt));
		}
	}

	// ADD NEURON SYNAPSES
	for(typename std::vector<Connection<float_type> >::iterator conIt = connections.begin();
			conIt != connections.end(); ++conIt)
	{
		size_t fromStartID = conIt->getFromStartID();
		size_t toStartID = conIt->getToStartID();

		for(typename Connection<float_type>::iterator synapseIt = conIt->begin();
				synapseIt != conIt->end(); ++synapseIt)
		{
			size_t sourceNeuronID = fromStartID + synapseIt->first.first;
			size_t targetNeuronID = toStartID + synapseIt->first.second;
			float_type weight = synapseIt->second.first;
			unsigned deley = synapseIt->second.second;
			Synapse<float_type> sy;
			sy.sourceNeuronIdx = sourceNeuronID;
			sy.targetNeuronIdx = targetNeuronID;
			sy.weight = weight;
			sy.delay = deley;
			graph.addSynapse(sy);
		}
	}

	// ADD INPUT SYNAPSES
	for(typename std::vector<Connection<float_type> >::iterator inConIt = connections.begin();
			inConIt != connections.end(); ++inConIt)
	{
		size_t fromStartID = inConIt->getFromStartID();
		size_t toStartID = inConIt->getToStartID();

		for(typename Connection<float_type>::iterator inSynapseIt = inConIt->begin();
				inSynapseIt != inConIt->end(); ++inSynapseIt)
		{
			size_t sourceNeuronID = fromStartID + inSynapseIt->first.first;
			size_t targetNeuronID = toStartID + inSynapseIt->first.second;
			float_type weight = inSynapseIt->second.first;
			unsigned deley = inSynapseIt->second.second;
			Synapse<float_type> sy;
			sy.sourceNeuronIdx = sourceNeuronID;
			sy.targetNeuronIdx = targetNeuronID;
			sy.weight = weight;
			sy.delay = deley;
			graph.addInputSynapse(sy);
		}
	}
}


	} /* consturction */
} /* szsnn */

#endif /* NETWORKCREATOR_H_ */
