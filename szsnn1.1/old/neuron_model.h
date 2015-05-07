#ifndef NEURON_MODEL_H_
#define NEURON_MODEL_H_

/**
 * \file neuron_model.h
 *
 * \date 17-11-2012
 * \author pawel
 * \description plik nagłówkowy importujący odpowiedni model
 * neuronu dla danej kompilacji
 */

#include "utils.h"
/*
 * Model domyślny -> idx = 0
 * 0 - test_neuron.h
 * 1 - othertest_neuron.h
 */

#ifndef SZSNN_NEURON_MODEL_IDX
	#define SZSNN_NEURON_MODEL 0
#endif

#if SZSNN_NEURON_MODEL_IDX == 0
	#include "test_neuron.h"
	MESSAGE("wybrany model to test_neuron.h")

#elif SZSNN_NEURON_MODEL_IDX == 1

	#include "othertest_neuron.h"
	MESSAGE("wybrany model to othertest_neuron.h")

#else
/* niepoprawny index */
	ERROR(Podano niepoprawny indeks modelu SZSNN_NEURON_MODEL_IDX: TOSTRING(SZSNN_NEURON_MODEL_IDX) nie istnieje)
	#error(SZSNN error in compilation)

#endif /* if SZSNN_NEURON_MODEL */

/*
 * Struktura modelu neuronu do obliczeń na CPU
 */
#if SZSNN_SIMULATIOR_TYPE == 0

typedef struct
{
	/* dane dpecyficzne modelu */
	CPU_neuronType		modelData;
	/* dane pomocnicze do symulacji */
	CPU_DATA_TYPE		cyclicBuffer[MAX_SYNAPTIC_DELAY - MIN_SYNAPTIC_DELAY + 1];
} CPU_Neuron;

#elif SZSNN_SIMULATIOR_TYPE == 1

typedef struct
{
	/* dane dpecyficzne modelu */
	CPU_neuronType		modelData;
	/* dane pomocnicze do symulacji */
	unsigned int		incEventsCount;		// liczba zdarzeń w kolejce do neuronu (dochodzące strzały)
	CPU_DATA_TYPE		incEventTimes[CPU_INC_EVENT_QUEUE_SIZE];
	CPU_DATA_TYPE		incEventWeights[CPU_INC_EVENT_QUEUE_SIZE];
} CPU_Neuron;

#endif





#endif /* NEURON_MODEL_H_ */
