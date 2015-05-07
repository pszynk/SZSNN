#ifndef OTHERTEST_NEURON_H_
#define OTHERTEST_NEURON_H_

/**
 * \file othertest_neuron.h
 *
 * \date 17-11-2012
 * \author pawel
 * \description
 */


#include "locales.h"



#undef		NEURON_SIZE
#define		NEURON_SIZE		3


typedef struct
{
	CPU_DATA_TYPE		param1;
	CPU_DATA_TYPE		param2;
	CPU_DATA_TYPE		var1;
} neuron_othertest;

typedef neuron_othertest CPU_neuronType;

/* liczba parametrów itp? TODO */

#endif /* OTHERTEST_NEURON_H_ */



