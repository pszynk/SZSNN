#ifndef TEST_NEURON_H_
#define TEST_NEURON_H_

/**
 * \file test_neuron.h
 *
 * \date 17-11-2012
 * \author pawel
 * \description
 */


#include "locales.h"

#undef		NEURON_SIZE
#define		NEURON_SIZE		5


typedef struct
{
	CPU_DATA_TYPE		param1;
	CPU_DATA_TYPE		param2;
	CPU_DATA_TYPE		param3;
	CPU_DATA_TYPE		var1;
	CPU_DATA_TYPE		var2;
} neuron_test;

typedef neuron_test CPU_neuronType;


//neuron_test genRandom(unsigned int seed, int type) {
//
//}

extern neuron_test createRandomNeuron(bool excitatory);


/* liczba parametrów itp? TODO */

#endif /* TEST_NEURON_H_ */
