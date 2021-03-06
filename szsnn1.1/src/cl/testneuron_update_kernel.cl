//#define __kernel
//#define __global
//#define __local

#define CURRENT_EXC_IDX 0
#define CURRENT_INH_IDX 1

#define NULL 0

//#define THREADS_PER_GROUP 256


#define CURRENT_PARTITION get_group_id(0)

bool isBitSet(unsigned neuron, __local unsigned* vec)
{
	return (vec[neuron/BIT_VECTOR_WORD_SIZE] >> (neuron % BIT_VECTOR_WORD_SIZE)) & 0x1;
}

void storeSparseSpikes(
	unsigned partitionIdx,
	unsigned l_firedC,
	__local unsigned* l_fired,
	__global unsigned* g_firedC,
	__global unsigned* g_fired)
{
	for(unsigned i=0; i < l_firedC; i += THREADS_PER_GROUP)
	{
		unsigned spike = i + get_local_id(0);
		if(spike < l_firedC)
		{
			g_fired[partitionIdx * MAX_PARTITION_SIZE + spike] = l_fired[spike];
		}
	}

	if(get_local_id(0) == 0)
	{
		g_firedC[partitionIdx] = l_firedC;	
	}
}

void storeDenseSpikes(
	unsigned partitionIdx,
	unsigned l_firedC,
	__local unsigned* l_firedO,
	__local unsigned* l_fired,
	__global unsigned* g_firedO)
{


	//czysc bitvector
	if(get_local_id(0) < BIT_VECTOR_WORD_PITCH)
	{
		l_firedO[get_local_id(0)] = 0;
	}
	barrier(CLK_LOCAL_MEM_FENCE);

	for(unsigned nbase= 0; nbase < l_firedC; nbase += THREADS_PER_GROUP)
	{
		unsigned i = nbase + get_local_id(0);
		unsigned neuron = l_fired[i];

		if(i < l_firedC)
		{
			unsigned word = neuron / 32;
			unsigned mask = 0x1 << (neuron % 32);
			atomic_or(l_firedO + word, mask);
			
		}
		barrier(CLK_LOCAL_MEM_FENCE);
	}

	if(get_local_id(0) < BIT_VECTOR_WORD_PITCH)
	{
		//printf("%d-t %d-m %d-i\n", get_local_id(0), l_firedO[get_local_id(0)], get_local_id(0) + partitionIdx * BIT_VECTOR_WORD_PITCH);
		g_firedO[get_local_id(0) + partitionIdx * BIT_VECTOR_WORD_PITCH] = l_firedO[get_local_id(0)];
	}
}

#pragma OPENCL EXTENSION cl_amd_printf : enable
__kernel void testneuron_update(
				const uint 			cycle,				//0
				const uint 			partitionCount,		//1
	__global 	const unsigned* 	g_partitionSizes,	//2
	__global	const FLOAT_TYPE*	g_neuronParams,		//3
	__global		  FLOAT_TYPE*	g_neuronVars,		//4
	/* [p:n] MPS -> MAX_PARTITION_SIZE
	 * [0:0] [0:1] .............. [0:MPS]
	 * [1:0] ....
	 */
				const uint			isInputCurrent,		//5
	__global		  FLOAT_TYPE*	g_inputCurrent,		//6
	__global		  FLOAT_TYPE*	g_current,			//7
				const uint			isInputFiring,		//8
	__global		  unsigned*		g_inputFired,		//9
	__global		  unsigned*		g_firedCount,		//10
	__global		  unsigned*		g_fired,			//11
	__global		  unsigned*		g_firedOutput		//12
	)
{

	__local unsigned l_firedCount;
	__local unsigned l_fired[MAX_PARTITION_SIZE];


	__local unsigned l_partitionIdx;
	__local unsigned l_partitionSize;

	__local FLOAT_TYPE l_inputCurrent[MAX_PARTITION_SIZE];

	__local unsigned l_inputFired[BIT_VECTOR_WORD_PITCH];


//	return;
	if(get_local_id(0) == 0)
	{
		l_firedCount = 0;
		l_partitionIdx = CURRENT_PARTITION;
		l_partitionSize = g_partitionSizes[l_partitionIdx];
	}

	barrier(CLK_LOCAL_MEM_FENCE);

	__global FLOAT_TYPE* g_currentExc =
			g_current + (CURRENT_EXC_IDX * partitionCount + l_partitionIdx) * MAX_PARTITION_SIZE;

	__global FLOAT_TYPE* g_currentInh =
			g_current + (CURRENT_INH_IDX * partitionCount + l_partitionIdx) * MAX_PARTITION_SIZE;


	/* ładowanie strzałów z zwenątrz */

	if(isInputFiring == 1)
	{
		unsigned partitionStart = l_partitionIdx * BIT_VECTOR_WORD_PITCH;
		// ASSERT THREADS_PER_GROUP >= BIT_VECTOR_WORD_PITCH
		if(get_local_id(0) < BIT_VECTOR_WORD_PITCH)
		{
			l_inputFired[get_local_id(0)] = g_inputFired[partitionStart + get_local_id(0)];
		}
	}
	else
	{
		if(get_local_id(0) < BIT_VECTOR_WORD_PITCH)
		{
			l_inputFired[get_local_id(0)] = 0;
		}
	}
	//return;

	/* ładowanie napięć zadanych z zwenątrz */
	if(isInputCurrent == 1)
	{
		for(unsigned i=0; i < l_partitionSize; i += THREADS_PER_GROUP)
		{
			unsigned neuron = i + get_local_id(0);
			unsigned partitionStart = l_partitionIdx * MAX_PARTITION_SIZE;
			if(neuron < l_partitionSize) {
				l_inputCurrent[neuron] = g_inputCurrent[partitionStart + neuron];
			}
		}
	}
	else
	{
		for(unsigned i=0; i < l_partitionSize; i += THREADS_PER_GROUP)
		{
			unsigned neuron = i + get_local_id(0);
			if(neuron < l_partitionSize) {
				l_inputCurrent[neuron] = 0;
			}
		}
	}

	barrier(CLK_LOCAL_MEM_FENCE);

	

	__global const FLOAT_TYPE* g_neuronParams_inPartition = 
		g_neuronParams + l_partitionIdx * MAX_PARTITION_SIZE;

	__global const FLOAT_TYPE* g_neuronVars_inPartition = 
		g_neuronVars + l_partitionIdx * MAX_PARTITION_SIZE;

	size_t neuronParamStep = partitionCount * MAX_PARTITION_SIZE;

	__global const FLOAT_TYPE* param_0 = g_neuronParams_inPartition + 0 * neuronParamStep;
	__global const FLOAT_TYPE* param_1 = g_neuronParams_inPartition + 1 * neuronParamStep;

	__global FLOAT_TYPE* var_0 = g_neuronVars_inPartition + 0 * neuronParamStep;
	__global FLOAT_TYPE* var_1 = g_neuronVars_inPartition + 1 * neuronParamStep;


	for(unsigned i=0; i < l_partitionSize; i += THREADS_PER_GROUP)
	{
		unsigned neuron = i + get_local_id(0);
		
		if (neuron <  l_partitionSize)
		{
			FLOAT_TYPE Ie = g_currentExc[neuron];
			FLOAT_TYPE Ii = g_currentInh[neuron];

			FLOAT_TYPE I = Ie - Ii + l_inputCurrent[neuron];

			FLOAT_TYPE v = var_0[neuron] + I;

			unsigned iid = get_local_id(0);
			unsigned gid = get_group_id(0);
			//printf("%d-n %f-p0 (%d)\n", neuron, param_0[neuron], iid);
			//bool firedInternal = (param_0[neuron] == 1);
			bool firedInternal = (v >= 1);
			bool firedExternal = isBitSet(neuron, l_inputFired);
			//((l_inputFired[neuron/BIT_VECTOR_WORD_SIZE] >> (neuron % BIT_VECTOR_WORD_SIZE)) & 0x1);

			if (firedInternal || firedExternal)
			{
				//printf("%d-n ", neuron);
				v = var_1[neuron];
				//unsigned idx = neuron;
				//l_firedCount = MAX_PARTITION_SIZE; 
				unsigned idx = atomic_add(&l_firedCount, 1);
				l_fired[idx] = neuron;
				printf("c%d[test_update] p%dn%d fired (forced spike%d, current%d)\n",
					cycle, CURRENT_PARTITION, neuron, firedExternal, l_inputCurrent[neuron]);
			}

			var_0[neuron] = v;
		}

		barrier(CLK_LOCAL_MEM_FENCE);
	}
	__local unsigned l_firedO[BIT_VECTOR_WORD_PITCH];
	barrier(CLK_LOCAL_MEM_FENCE);
	
	storeDenseSpikes(l_partitionIdx, l_firedCount, l_firedO, l_fired, g_firedOutput);
	storeSparseSpikes(l_partitionIdx, l_firedCount, l_fired, g_firedCount, g_fired);
	if(get_local_id(0) == 0)
		printf("%d-p %d\n", l_partitionIdx, g_firedOutput[l_partitionIdx]);
}
