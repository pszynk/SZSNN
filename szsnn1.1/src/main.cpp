#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <malloc.h>


#include <stdio.h>


#include <sstream>

#include "opencl_header.h"

//using namespace std;

#include <fstream>
#include <algorithm>
#include <map>

#include <boost/shared_ptr.hpp>


#include "utils.h"
#include "locales.h"
#include "types.h"


#include "debug.hpp"
#include "models/neuron_model_traits.hpp"
#include "locales.h"
#include "NetworkGraph.hpp"
#include "SzsnnException.hpp"
#include "Context.hpp"
#include "Connection.hpp"
#include "Simulation.hpp"
#include "DeviceManager.hpp"
#include "SynapseData.hpp"
#include "NeuronData.hpp"
#include "SpikeBuffer.hpp"
#include "synapse_model.h"
#include "SzsnnTimer.hpp"
#include "models/IF_curr_exp_NeuronModel.hpp"
#include "Population.hpp"
#include "NetworkCreator.hpp"
#include "SpikeRecorder.hpp"
#include "models/Izhikevich_NeuronModel.hpp"
#include "models/TestNeuronModel.hpp"

int neurony2()
{
//	typedef float floatT;
//	szsnn::SzsnnTimer t;
//	size_t n = 2;
//	//size_t n08 = (size_t) n * 0.8;
//	//size_t n02 = (size_t) n * 0.2;
//	szsnn::construction::Population<szsnn::IF_curr_exp_NeuronModel<floatT> > p1(1, 0, "excictatory");
//	szsnn::construction::Population<szsnn::IF_curr_exp_NeuronModel<floatT> >  p2(1, 1, "inhibitory");
//	szsnn::construction::Connection<floatT> con1(0u, 0u);
//	szsnn::construction::Connection<floatT> con2(0u, 0u);
//	con2.loadFromFile("2neurony.conn");
//	//con1.loadFromFile("excit.conn");
//	szsnn::construction::NetworkCreator<szsnn::IF_curr_exp_NeuronModel<floatT> > netC;
//	//netC.addConnection(con1);
//	netC.addConnection(con2);
//	typedef szsnn::IF_curr_exp_NeuronModel<floatT> neuronT;
//	p1.setNeuronParameter(neuronT::p_V_REST, -49.0);
//	p2.setNeuronParameter(neuronT::p_V_REST, -49.0);
//	p1.setNeuronParameter(neuronT::p_TAU_SYN_I, 10.0);
//	p2.setNeuronParameter(neuronT::p_TAU_SYN_I, 10.0);
//	p1.setNeuronParameter(neuronT::p_TAU_SYN_E, 5.0);
//	p2.setNeuronParameter(neuronT::p_TAU_SYN_E, 5.0);
//	p1.setNeuronParameter(neuronT::p_V_RESET, -200.0);
//	p2.setNeuronParameter(neuronT::p_V_RESET, -200.0);
//	p1.setNeuronParameter(neuronT::p_V_THRESH, -50.0);
//	p2.setNeuronParameter(neuronT::p_V_THRESH, -50.0);
//	p1.setNeuronVariable(neuronT::v_V, -70.0);
//	p2.setNeuronVariable(neuronT::v_V, -70.0);
//	//p1.setNeuronVariable(neuronT::v_IE, 10.0);
//	//p2.setNeuronVariable(neuronT::v_IE, 10.0);
//
//	netC.addPopulation(p1);
//	netC.addPopulation(p2);
//	szsnn::NetworkGraph<szsnn::IF_curr_exp_NeuronModel<floatT> > ngg;
//	netC.fillNetworkGraph(ngg);
//	szsnn::Context cc;
//	cc.setup();
//	szsnn::Simulation<floatT> sim(cc, ngg);
//
//
////	std::cout << ngg.neuronsToString() << std::endl;
////	std::cout << ngg.synapsesToString() << std::endl;
//
////	std::cout << sim.neurons.neuronParamsToString() << std::endl;
////	std::cout << sim.neurons.neuronVarsToString() << std::endl;
////	std::cin.sync();std::cin.get();
//
//	//return 0;
//	szsnn::Simulation<floatT>::firing_output foutput;
////#if SZSNN_DEBUG
//	freopen ("2neurony.log","w",stdout);
////#endif
//	szsnn::record::SpikeRecorder r(0, 1000);
//	for(int c = 0; c < 80; c++)
//	{
//
//		foutput = sim.stepAndFired();
//		r.recordCycleSpikes(foutput);
//		std::cout<< "runda " << c<< std::endl;
//		//std::cin.sync();std::cin.get();
//		if(!foutput.empty()){
//			std::cout<< "strzalilo " << foutput.size();
//		}
//		else {continue;}
//		for(unsigned i = 0; i < foutput.size(); i ++)
//		{
//			std::cout<< " " << foutput[i] << std::endl;
//		}
//		//std::cin.sync();std::cin.get();
//	}
//	//netC.saveAllConnections();
////#if SZSNN_DEBUG
//	fclose (stdout);
////#endif
//	r.saveToFileByNeurons("my_spikes_2neurony.txt");
//	std::cout << double(sim.getTime()) / 1000 << "s" << std::endl;
	return 0;
}

int izihovich()
{
//	typedef float floatT;
//	szsnn::SzsnnTimer t;
//	size_t n = 1000;
//	size_t n08 = (size_t) n * 0.8;
//	size_t n02 = (size_t) n * 0.2;
//	szsnn::construction::Population<szsnn::Izhikevich_NeuronModel<floatT> > p1(n08, 0, "excictatory");
//	szsnn::construction::Population<szsnn::Izhikevich_NeuronModel<floatT> >  p2(n02, n08, "inhibitory");
//	szsnn::construction::Connection<floatT> con1(0u, 0u);
//	szsnn::construction::Connection<floatT> con2(0u, 0u);
//	con1.loadFromFile("izi1.conn");
//	con2.loadFromFile("izi2.conn");
//	//con1.loadFromFile("excit.conn");
//	szsnn::construction::NetworkCreator<szsnn::Izhikevich_NeuronModel<floatT> > netC;
//	netC.addConnection(con1);
//	netC.addConnection(con2);
//	typedef szsnn::Izhikevich_NeuronModel<floatT> neuronT;
//	p1.setNeuronParameter(neuronT::p_A, 0.02);
//	p2.setNeuronParameter(neuronT::p_A, 0.02);
//	p1.setNeuronParameter(neuronT::p_B, 0.2);
//	p2.setNeuronParameter(neuronT::p_B, 0.2);
//	p1.setNeuronParameter(neuronT::p_C, -65);
//	p2.setNeuronParameter(neuronT::p_C, -65);
//	p1.setNeuronParameter(neuronT::p_D, 2);
//	p2.setNeuronParameter(neuronT::p_D, 2);
//
//
//	netC.addPopulation(p1);
//	netC.addPopulation(p2);
//	szsnn::NetworkGraph<szsnn::Izhikevich_NeuronModel<floatT> > ngg;
//	netC.fillNetworkGraph(ngg);
//	szsnn::Context cc;
//	cc.setup();
//	szsnn::Simulation<floatT> sim(cc, ngg);
//
//
////	std::cout << ngg.neuronsToString() << std::endl;
////	std::cout << ngg.synapsesToString() << std::endl;
//
////	std::cout << sim.neurons.neuronParamsToString() << std::endl;
////	std::cout << sim.neurons.neuronVarsToString() << std::endl;
////	std::cin.sync();std::cin.get();
//
//	//return 0;
//	szsnn::Simulation<floatT>::firing_output foutput;
////#if SZSNN_DEBUG
//	freopen ("IZIONE3.log","w",stdout);
////#endif
//	szsnn::record::SpikeRecorder r(0, 341);
//	szsnn::Simulation<floatT>::firing_output finput;
//	for(unsigned i=0; i<1000; i++)
//	{
//		finput.push_back(i);
//	}
//	for(int c = 0; c < 341; c++)
//	{
//
//
//
//		//std::cout<< "runda " << c<< std::endl;
//		//std::cin.sync();std::cin.get();
//		if (c == 1 || c == 41 || c == 100)
//		{
//			foutput = sim.stepAndFired(finput);
//		}
//		else
//		{
//			foutput = sim.stepAndFired();
//		}
//		r.recordCycleSpikes(foutput);
//		if(!foutput.empty()){
//			//std::cout<< "strzalilo " << foutput.size();
//		}
//		else {continue;}
//		for(unsigned i = 0; i < foutput.size(); i ++)
//		{
//			//std::cout<< " " << foutput[i] << std::endl;
//		}
//		//std::cin.sync();std::cin.get();
//	}
//	//netC.saveAllConnections();
////#if SZSNN_DEBUG
//	fclose (stdout);
////#endif
//	r.saveToFileByNeurons("my_spikes_izi3.txt");
//	std::cout << double(sim.getTime()) / 1000 << "s" << std::endl;
	return 0;
}
#include "models/NONE_NeuronModel.hpp"
#include "models/Const_rate_InputModel.hpp"

void izih_train()
{
		typedef float floatT;


		szsnn::construction::Population<szsnn::Izhikevich_NeuronModel<floatT> >  p1(1, 0, "izih");
		szsnn::construction::Population<szsnn::Const_rate_InputModel<floatT> > p2(1, 0, "input");
		szsnn::construction::Connection<floatT> con1(1, 0);
		con1.addConnection(0, 0, 20, 1);
		szsnn::construction::NetworkCreator<szsnn::Izhikevich_NeuronModel<floatT>,
			szsnn::Const_rate_InputModel<floatT> > netC;
		netC.addInputConnection(con1);
		typedef szsnn::Izhikevich_NeuronModel<floatT> neuronT;
		typedef szsnn::Const_rate_InputModel<float> inputT;
		p1.setNeuronParameters(neuronT::p_A, 0.02);
		p1.setNeuronParameters(neuronT::p_B, 0.2);
		p1.setNeuronParameters(neuronT::p_C, -65.0);
		p1.setNeuronParameters(neuronT::p_D, 8.0);
		p1.setNeuronVariables(neuronT::v_V, -80);
		p2.setNeuronParameters(inputT::p_RATE, 2.0);
		p2.setNeuronVariables(inputT::v_LAST, 0.0);

		netC.addPopulation(p1);
		netC.addInputPopulation(p2);

		szsnn::NetworkGraph<neuronT, inputT> ngg;
		netC.fillNetworkGraph(ngg);
		ngg.connectInput();
		std::cout << ngg.neuronsToString() << std::endl;
		std::cout << ngg.synapsesToString() << std::endl;
		return;
		szsnn::Context cc;
		cc.setup();
		szsnn::Simulation<floatT> sim(cc, ngg);

		std::cout <<sim.synapses.printDelaysBitmap() << std::endl;
		std::cout <<sim.synapses.printTargets() << std::endl;
		std::cout <<sim.synapses.printWeights() << std::endl;
		return;
	//
	//
	////	std::cout << ngg.neuronsToString() << std::endl;
	////	std::cout << ngg.synapsesToString() << std::endl;
	//
	////	std::cout << sim.neurons.neuronParamsToString() << std::endl;
	////	std::cout << sim.neurons.neuronVarsToString() << std::endl;
	////	std::cin.sync();std::cin.get();
	//
	//	//return 0;
		szsnn::Simulation<floatT>::firing_output foutput;
	#if SZSNN_DEBUG
		freopen ("myfile.txt","w",stdout);
	#endif
		szsnn::record::SpikeRecorder r(0, 1000);
		for(int c = 0; c < 500; c++)
		{
	//
			//foutput = sim.stepAndFired();
			r.recordCycleSpikes(sim.stepAndFired());
	//		//std::cout<< "runda " << c<< std::endl;
	//		//std::cin.sync();std::cin.get();
	//		if(!foutput.empty()){
	//			//std::cout<< "strzalilo " << foutput.size();
	//		}
	//		else {continue;}
	//		for(unsigned i = 0; i < foutput.size(); i ++)
	//		{
	//			//std::cout<< " " << foutput[i] << std::endl;
	//		}
	//		//std::cin.sync();std::cin.get();
		}
	//	//netC.saveAllConnections();
	#if SZSNN_DEBUG
		fclose (stdout);
	#endif
	//	r.saveToFileByNeurons("my_spikes_4000.txt");
		std::cout << double(sim.getTime()) / 1000 << "s" << std::endl;
		r.saveToFileByNeurons("input_test.txt");
	//	return 0;
}
int main(int argc, char **argv) {

	izih_train();
	return 0;
//	izihovich();
//	return 0;
//	//neurony2();
//	//return 0;
//	//izih();
//	//return 0;
	typedef float floatT;
	szsnn::SzsnnTimer t;
	size_t n = 4000;
	size_t n08 = (size_t) n * 0.8;
	size_t n02 = (size_t) n * 0.2;
	szsnn::construction::Population<szsnn::IF_curr_exp_NeuronModel<floatT> > p1(n08, 0, "excictatory");
	szsnn::construction::Population<szsnn::IF_curr_exp_NeuronModel<floatT> >  p2(n02, n08, "inhibitory");
	szsnn::construction::Connection<floatT> con1(0u, 0u);
	szsnn::construction::Connection<floatT> con2(0u, 0u);
	con2.loadFromFile("inhib.conn");
	con1.loadFromFile("excit.conn");
	szsnn::construction::NetworkCreator<szsnn::IF_curr_exp_NeuronModel<floatT>,
	szsnn::NONE_NeuronModel<floatT> > netC;
	netC.addConnection(con1);
	netC.addConnection(con2);
	typedef szsnn::IF_curr_exp_NeuronModel<floatT> neuronT;
	typedef szsnn::NONE_NeuronModel<float> inputT;
	p1.setNeuronParameters(neuronT::p_V_REST, -49.0);
	p2.setNeuronParameters(neuronT::p_V_REST, -49.0);
	p1.setNeuronParameters(neuronT::p_TAU_M, 30.0);
	p2.setNeuronParameters(neuronT::p_TAU_M, 30.0);
	p1.setNeuronParameters(neuronT::p_TAU_SYN_I, 10.0);
	p2.setNeuronParameters(neuronT::p_TAU_SYN_I, 10.0);
	p1.setNeuronParameters(neuronT::p_TAU_SYN_E, 5.0);
	p2.setNeuronParameters(neuronT::p_TAU_SYN_E, 5.0);
	p1.setNeuronParameters(neuronT::p_V_RESET, -100.0);
	p2.setNeuronParameters(neuronT::p_V_RESET, -100.0);
	p1.setNeuronParameters(neuronT::p_V_THRESH, -50.0);
	p2.setNeuronParameters(neuronT::p_V_THRESH, -50.0);
	p1.setNeuronVariables(neuronT::v_V, -70.0);
	p2.setNeuronVariables(neuronT::v_V, -70.0);
	//p1.setNeuronVariable(neuronT::v_IE, 10.0);
	//p2.setNeuronVariable(neuronT::v_IE, 10.0);
//
	netC.addPopulation(p1);
	netC.addPopulation(p2);
	szsnn::NetworkGraph<neuronT, inputT> ngg;
	netC.fillNetworkGraph(ngg);
	szsnn::Context cc;
	cc.setup();
	szsnn::Simulation<floatT> sim(cc, ngg);
//
//
////	std::cout << ngg.neuronsToString() << std::endl;
////	std::cout << ngg.synapsesToString() << std::endl;
//
////	std::cout << sim.neurons.neuronParamsToString() << std::endl;
////	std::cout << sim.neurons.neuronVarsToString() << std::endl;
////	std::cin.sync();std::cin.get();
//
//	//return 0;
	szsnn::Simulation<floatT>::firing_output foutput;
////#if SZSNN_DEBUG
//	//freopen ("myfile.txt","w",stdout);
////#endif
	szsnn::record::SpikeRecorder r(0, 1000);
	for(int c = 0; c < 1000; c++)
	{
//
		//foutput = sim.stepAndFired();
		r.recordCycleSpikes(sim.stepAndFired());
//		//std::cout<< "runda " << c<< std::endl;
//		//std::cin.sync();std::cin.get();
//		if(!foutput.empty()){
//			//std::cout<< "strzalilo " << foutput.size();
//		}
//		else {continue;}
//		for(unsigned i = 0; i < foutput.size(); i ++)
//		{
//			//std::cout<< " " << foutput[i] << std::endl;
//		}
//		//std::cin.sync();std::cin.get();
	}
//	//netC.saveAllConnections();
////#if SZSNN_DEBUG
//	//fclose (stdout);
////#endif
//	r.saveToFileByNeurons("my_spikes_4000.txt");
	std::cout << double(sim.getTime()) / 1000 << "s" << std::endl;
	r.saveToFileByNeurons("my_spikes_1000.txt");
//	return 0;
//	//szsnn::SzsnnException ex("hahaha");
//	//throw ex;
//	try {
//	szsnn::Context c;
//	//boost::shared_ptr<szsnn::Context> cp(new szsnn::Context());
//
////
////	cl::Event ev;
////	cp.get()->setup();
////	szsnn::SpikeBuffer sb(cp);
////
////	sb.initialize(2, "byleco");
////	cl_uint vv[7] = {1, 2, 31, 20, 0, 61, 32};
////	std::vector<cl_uint> vec(&vv[0], &vv[0] + 7);
////	sb.setInput(vec, ev);
////	cp.get()->writeBuffer(sb.getFiredInput(), CL_TRUE);
////
////	std::vector<cl_uint>& bitvec = sb.getFiredInput().getDataRef();
////	std::fill(bitvec.begin(), bitvec.end(), 0);
////	//cp.get()->readBuffer(sb.getFiredInput(), CL_TRUE);
////	unsigned nidx = 0;
////	for(std::vector<cl_uint>::const_iterator it = bitvec.begin(); it != bitvec.end(); ++it)
////	{
////		for(int i=0; i<32; i++ )
////		{
////			bool bit = ((*it) & (1<<i));
////			std::cout << nidx << "  " <<  bit << std::endl;
////			++nidx;
////		}
////	}
//	szsnn::NetworkGraph<szsnn::IF_curr_exp_NeuronModel<float> > ng;
//	unsigned networkSize = 10;
//	for(int i =0 ; i <networkSize; i++)
//	{
//		szsnn::IF_curr_exp_NeuronModel<float> n;
//		ng.addNeuron(n);
//	}
//
//	for(int i =0 ; i <networkSize; i++)
//	{
//		unsigned j = (i + 3) % networkSize;
//		szsnn::Synapse<float> sy;
//		sy.sourceNeuronIdx = i;
//		sy.targetNeuronIdx = j;
//		sy.delay = 1;
//		sy.weight = 1.62;
//		ng.addSynapse(0, sy);
//	}
	//ng.createRandomNetwork(100, 4, 100, 0, 1, 0.01, 1, 4, unsigned(time(0)));
//
//
//	//************ TESTY *******************
//	unsigned partSize = 50;
//	for(int i =0 ; i <partSize; i++)
//	{
//		szsnn::TestNeuronModel<float> n;
//		float p[2] = {2,3};
//		float v[2] = {0,0};
//		//if (!((i+1)%33)) p[0] = 1;
//		if (i == 1) p[0] = 1;
//		if (i == 2) p[0] = 1;
//		if(i == MAX_PARTITION_SIZE) p[0] = 1;
//		//if (i != 1024) v[1] = 1;
//		//p[0] = 1;
//		n.setAll(p, v);
//		ng.addNeuron(n);
//	}
//	unsigned nn = 100000;
//	for(int i = 0; i < partSize ; i++)
//	{
//		for(int j = 0; j < partSize; j++)
//		{
//			if(i == (j - nn))
//			{
//				//for(int k = i, d = 0; k < j; k++, d++)
//				for(int k = i, d = nn-1; k < j; k++, d--)
//				{
//					szsnn::Synapse<float> sy;
//					sy.weight = 1.0/nn;
//					if (k == j-1) sy.weight += 1.0/(nn*2);
//					sy.delay = (d % MAX_SYNAPTIC_DELAY) + 1;
//					sy.sourceNeuronIdx = k;
//					sy.targetNeuronIdx = j;
//					sy.synapseIdx = 0;
//					//std::cout << "ADDING SYNAPSE " << k << " -> " << j << " delay "<<sy.delay<< " weight " << sy.weight << std::endl;
//					//system("pause");
//					ng.addSynapse(0, sy);
//					
//					
//				}
//	/*			szsnn::Synapse<float> sy;
//				sy.weight = 0.4;
//				sy.delay = 62;
//				sy.sourceNeuronIdx = i;
//				sy.targetNeuronIdx = j;
//				sy.synapseIdx = 0;
//				ng.addSynapse(0, sy);
//				sy.sourceNeuronIdx = i+1;
//				sy.delay = 63;
//				ng.addSynapse(0, sy);
//				sy.sourceNeuronIdx = i+2;
//				sy.delay = 64;
//				ng.addSynapse(0, sy);*/
//			}
//
//		}
//	}
//	//for(int i = 0; i < partSize-1; i++)
//	//{
//	//	szsnn::Synapse<float> sy;
//	//	sy.sourceNeuronIdx = i;
//	//	sy.targetNeuronIdx = 1024;
//	//	sy.weight = 1.0/1024;
//	//	sy.delay = 1;
//	//	sy.synapseIdx = 0;
//	//	ng.addSynapse(0, sy);
//	//}
//	for(int i = 0; i < partSize ; i++)
//	{
//		for(int j = 0; j < partSize; j++)
//		{
//			if (j == i) continue;
//			szsnn::Synapse<float> sy;
//			sy.weight = 1.0/(partSize - 1) + (1.0/((partSize - 1) * (partSize - 1)));
//			sy.delay =  1;
//			sy.sourceNeuronIdx = i;
//			sy.targetNeuronIdx = j;
//			sy.synapseIdx = 0;
//			ng.addSynapse(0, sy);
//		}
//	}
//	/*szsnn::Synapse<float> sy;
//	sy.weight = 0.5;
//	sy.delay = 4;
//	sy.sourceNeuronIdx = 0;
//	sy.targetNeuronIdx = 32;
//	sy.synapseIdx = 0;
//	ng.addSynapse(0, sy);
//	sy.targetNeuronIdx = 4;
//	ng.addSynapse(0, sy);
//	sy.sourceNeuronIdx = 1;
//	sy.delay = 6;
//	ng.addSynapse(0, sy);
//	sy.targetNeuronIdx = 32;
//	ng.addSynapse(0, sy);*/
//	//for(int n1 = 0; n1 < MAX_PARTITION_SIZE; ++n1)
//	//{
//	//	for(int n2 = 0; n2 < 2*8; ++n2)
//	//	{
//	//		if (n1 == n2) continue;
//	//		for(int i =1 ; i <= 8; i++)
//	//		{
//	//			szsnn::Synapse<float> sy;
//	//			sy.weight = 1;
//	//			sy.delay = i;
//	//			sy.sourceNeuronIdx = n1;
//	//			sy.targetNeuronIdx = (n1 + n2 + 1) % MAX_PARTITION_SIZE;
//	//			sy.synapseIdx = 0;
//
//	//			ng.addSynapse(0,sy);
//	//		}
//	//	}
//	//}
//	//szsnn::Synapse<float> sy;
//	//sy.weight = 1;
//	//sy.delay = 1;
//	//sy.sourceNeuronIdx = 1;
//	//sy.targetNeuronIdx = 2;
//	//sy.synapseIdx = 0;
//
//	//ng.addSynapse(0,sy);
//	//sy.targetNeuronIdx = 3;
//	//ng.addSynapse(0,sy);
//	//sy.targetNeuronIdx = 2;
//	//sy.delay = 2;
//	//ng.addSynapse(0,sy);
//	//sy.sourceNeuronIdx = 2;
//	//sy.targetNeuronIdx = 4;
//	//sy.weight = -2;
//	//sy.delay = 1;
//	//ng.addSynapse(0, sy);
//	//sy.sourceNeuronIdx = 1;
//	//sy.targetNeuronIdx = MAX_PARTITION_SIZE;
//	//sy.weight = 3;
//	//ng.addSynapse(0, sy);
//	//sy.delay = 2;
//	//ng.addSynapse(0, sy);
//	//sy.delay = 1;
//	//sy.sourceNeuronIdx = MAX_PARTITION_SIZE;
//	//sy.targetNeuronIdx = 1;
//	//sy.weight = -4;
//	//ng.addSynapse(0, sy);
////	for(std::vector<unsigned>::const_iterator it = ng.getPartitionSizes().begin();
////			it != ng.getPartitionSizes().end(); ++it)
////	{
////		std::cout << (*it) <<std::endl;
////	}


}
