#ifndef TYPES_HPP_
#define TYPES_HPP_

/**
 * \file types.hpp
 *
 * \date 16-12-2012
 * \author pawel
 * \description
 */

#include "opencl_header.h"

#include "locales.h"

typedef cl_uint partitionIdx_t;
typedef cl_uint neuronIdx_t;
typedef cl_ulong synapseIdx_t;
typedef cl_uint delay_t;

typedef cl_uint bundleIdx_t;


struct synapse_addr_struct {
	cl_ulong 		row;
	synapseIdx_t 	synapse;

	synapse_addr_struct(cl_ulong _row,
			synapseIdx_t _synapse):
				row(_row), synapse(_synapse){};
};

typedef synapse_addr_struct synapse_addr_t;

typedef cl_uint2 group_t;


typedef cl_uint2 group_addr_t;

typedef cl_short2 lq_entry_t; //TODO zmienic nazwe

//TODO funkcja albo cos
#define GROUP_ADDR_OFFSET(partition, neuron, delay) \
	(((partition) * MAX_PARTITION_SIZE + (neuron) ) * MAX_SYNAPTIC_DELAY + (delay))


#endif /* TYPES_HPP_ */
