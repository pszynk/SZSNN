#ifndef SYNAPSE_MODEL_H_
#define SYNAPSE_MODEL_H_

/**
 * \file synapse_model.h
 *
 * \date 22-11-2012
 * \author pawel
 * \description
 */

#include "types.h"

namespace szsnn {
/**
 * struktura zawierająca dane o pojedynczej synapsie
 */
template<typename floatT>
class Synapse
{
public:
	floatT			weight;
	delay_t			delay;
	synapseIdx_t	synapseIdx;
	neuronIdx_t		sourceNeuronIdx;
	neuronIdx_t		targetNeuronIdx;
};

} /* namespace szsnn */
#endif /* SYNAPSE_MODEL_H_ */
