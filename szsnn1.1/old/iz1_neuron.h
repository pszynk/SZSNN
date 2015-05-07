#ifndef IZ1_NEURON_H_
#define IZ1_NEURON_H_

/**
 * \file iz1_neuron.h
 *
 * \date 19-11-2012
 * \author pawel
 * \description
 */

#include "locales.h"


typedef struct
{
	CPU_DATA_TYPE E; 											/*Electrical elastance 1/C*/
	CPU_DATA_TYPE vr;											/*Resting mebrane potential*/
	CPU_DATA_TYPE k;												/*Scaling constant*/
	CPU_DATA_TYPE l;												/*-k*vt*/
	CPU_DATA_TYPE a;												/*Recovery variable rate constant*/
	CPU_DATA_TYPE b;												/*Scaling constant*/
	CPU_DATA_TYPE v_peak;									/*Peak voltage during spike*/
	CPU_DATA_TYPE v_reset;									/*Post-spike reset potential*/
	CPU_DATA_TYPE u_step;									/*Post-spike recovery variable step*/
	CPU_DATA_TYPE I;												/*Input current*/
	CPU_DATA_TYPE g_ampa;									/*AMPA conductance*/
	CPU_DATA_TYPE g_gaba;									/*GABA_A conductance*/
	CPU_DATA_TYPE E_ampa;									/*AMPA reversal potential*/
	CPU_DATA_TYPE E_gaba;									/*GABA reversal potential*/
	CPU_DATA_TYPE v;												/*Membrane voltage*/
	CPU_DATA_TYPE u;												/*Recovery variable*/
	//int n_in;												/*Number of synaptic events in input buffers*/
	//double in_t[REFERENCE_EVENT_QUEUE_SIZE];						/*Time input buffer*/
	//float in_w[REFERENCE_EVENT_QUEUE_SIZE];							/*Weight input buffer*/
} neuron_iz;


typedef neuron_iz CPU_neuronType;

#endif /* IZ1_NEURON_H_ */
