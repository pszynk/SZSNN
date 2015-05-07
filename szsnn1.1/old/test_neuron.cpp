/**
 * \file test_neuron.c
 *
 * \date 16-12-2012
 * \author pawel
 * \description
 */

#include "test_neuron.h"

neuron_test createRandomNeuron(bool excitatory)
{
	neuron_test neuron;
	if(excitatory)
	{
		neuron.param1 = 1;
		neuron.param2 = 1;
		neuron.param3 = 1;
	}
	else
	{
		neuron.param1 = 0;
		neuron.param2 = 0;
		neuron.param3 = 0;
	}

	neuron.var1 = 0;
	neuron.var1 = 0;

	return neuron;
}


