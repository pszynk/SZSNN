//#include <locales.h>
//#define __kernel
//#define __global
//#define __local

#define CURRENT_EXC_IDX 0
#define CURRENT_INH_IDX 1

#define NULL 0

#define	p_V_REST		0
#define	p_CM			1
#define	p_TAU_M			2
#define	p_TAU_REFRAC	3
#define	p_TAU_SYN_E		4
#define	p_TAU_SYN_I		5
#define	p_I_OFFSET		6
#define	p_V_RESET		7
#define	p_V_THRESH		8

#define	v_V				0
#define	v_IE			1
#define	v_II			2
#define	v_FIRED			3

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

#ifdef SZSNN_KERNEL_DEBUG
#pragma OPENCL EXTENSION cl_amd_printf : enable
#endif

__kernel void IF_curr_exp_update(
				const ulong 			cycle,				//0
				const uint 			localPartitionCount,	//1
	__global 	const unsigned* 	g_partitionSizes,		//2
	__global	const FLOAT_TYPE*	g_neuronParams,			//3
	__global		  FLOAT_TYPE*	g_neuronVars,			//4
	/* [p:n] MPS -> MAX_PARTITION_SIZE
	 * [0:0] [0:1] .............. [0:MPS]
	 * [1:0] ....
	 */
				const uint			isInputCurrent,			//5
	__global		  FLOAT_TYPE*	g_inputCurrent,			//6
	__global		  FLOAT_TYPE*	g_current,				//7
				const uint			isInputFiring,			//8
	__global		  unsigned*		g_inputFired,			//9
	__global		  unsigned*		g_firedCount,			//10
	__global		  unsigned*		g_fired,				//11
	__global		  unsigned*		g_firedOutput,			//12
				const uint			partitionOffset,		//13
				const uint			globalPartitionCount 	//14
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
		l_partitionIdx = CURRENT_PARTITION + partitionOffset;
		l_partitionSize = g_partitionSizes[l_partitionIdx];
	}

	barrier(CLK_LOCAL_MEM_FENCE);

	__global FLOAT_TYPE* g_currentExc =
			g_current + (CURRENT_EXC_IDX * globalPartitionCount + l_partitionIdx) * MAX_PARTITION_SIZE;

	__global FLOAT_TYPE* g_currentInh =
			g_current + (CURRENT_INH_IDX * globalPartitionCount + l_partitionIdx) * MAX_PARTITION_SIZE;


	/* �adowanie strza��w z zwen�trz */

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

	/* �adowanie napi�� zadanych z zwen�trz */
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
		g_neuronParams + CURRENT_PARTITION * MAX_PARTITION_SIZE;

	__global FLOAT_TYPE* g_neuronVars_inPartition = 
		g_neuronVars + CURRENT_PARTITION * MAX_PARTITION_SIZE;

	size_t neuronParamStep = localPartitionCount * MAX_PARTITION_SIZE;
	size_t neuronVarsStep = neuronParamStep;
	/* parametry neuronu */
	__global const FLOAT_TYPE* gp_v_rest		= g_neuronParams_inPartition + p_V_REST * neuronParamStep;
	__global const FLOAT_TYPE* gp_cm			= g_neuronParams_inPartition + p_CM * neuronParamStep;
	__global const FLOAT_TYPE* gp_tau_m			= g_neuronParams_inPartition + p_TAU_M * neuronParamStep;
	__global const FLOAT_TYPE* gp_tau_refrac	= g_neuronParams_inPartition + p_TAU_REFRAC * neuronParamStep;
	__global const FLOAT_TYPE* gp_tau_syn_e		= g_neuronParams_inPartition + p_TAU_SYN_E * neuronParamStep;
	__global const FLOAT_TYPE* gp_tau_syn_i		= g_neuronParams_inPartition + p_TAU_SYN_I * neuronParamStep;
	__global const FLOAT_TYPE* gp_i_offset		= g_neuronParams_inPartition + p_I_OFFSET * neuronParamStep;
	__global const FLOAT_TYPE* gp_v_reset		= g_neuronParams_inPartition + p_V_RESET * neuronParamStep;
	__global const FLOAT_TYPE* gp_v_thresh		= g_neuronParams_inPartition + p_V_THRESH * neuronParamStep;

	__global FLOAT_TYPE* gv_v		 = g_neuronVars_inPartition + v_V * neuronVarsStep;
	__global FLOAT_TYPE* gv_ie		 = g_neuronVars_inPartition + v_IE * neuronVarsStep;
	__global FLOAT_TYPE* gv_ii		 = g_neuronVars_inPartition + v_II * neuronVarsStep;
	__global FLOAT_TYPE* gv_fired	 = g_neuronVars_inPartition + v_FIRED * neuronVarsStep;


	for(unsigned i=0; i < l_partitionSize; i += THREADS_PER_GROUP)
	{
		unsigned neuron = i + get_local_id(0);
		
		if (neuron <  l_partitionSize)
		{
			FLOAT_TYPE Ie = ((1.0 - 1.0/gp_tau_syn_e[neuron]) * gv_ie[neuron]) + g_currentExc[neuron];
			FLOAT_TYPE Ii = ((1.0 - 1.0/gp_tau_syn_i[neuron]) * gv_ii[neuron]) + g_currentInh[neuron];

			FLOAT_TYPE I = Ie + Ii + l_inputCurrent[neuron] + gp_i_offset[neuron];

			gv_ie[neuron] = Ie;
			gv_ii[neuron] = Ii;

#ifdef SZSNN_KERNEL_DEBUG
			if(CURRENT_PARTITION == 0 && neuron == 1)
			{
				printf("\t>>>%d>>>IE %f last%f in%f tau%f\n", cycle,  Ie, gv_ie[neuron], g_currentExc[neuron], gp_tau_syn_e[neuron]);
				printf("\t>>>%d>>>II %f last%f in%f tau%f\n", cycle, Ii, gv_ii[neuron], g_currentInh[neuron], gp_tau_syn_i[neuron]);
			}
#endif
			FLOAT_TYPE v = gv_v[neuron];
			FLOAT_TYPE fired = gv_fired[neuron];

			bool in_refractory = fired <= gp_tau_refrac[neuron];
			//bool in_refractory = false;
			fired += 1;

			if(!in_refractory)
			{
				FLOAT_TYPE cm = gp_cm[neuron];
				FLOAT_TYPE v_rest = gp_v_rest[neuron];
				FLOAT_TYPE tau_m = gp_tau_m[neuron];
				//FLOAT_TYPE k1 = I / cm + (v_rest - v) / tau_m;
				//FLOAT_TYPE k2 = I / cm + (v_rest - (v+k1/2)) / tau_m;
				//FLOAT_TYPE k3 = I / cm + (v_rest - (v-k1 + 2*k2)) / tau_m;
				//FLOAT_TYPE delta = (k1 + 4*k2 + k3)/6.0;
				FLOAT_TYPE delta = I / cm + (v_rest - v) / tau_m;
				v += delta;
				
#ifdef SZSNN_KERNEL_DEBUG			
				if(CURRENT_PARTITION == 0 && neuron == 1)
				{
					printf("\t>>>%d>>>V %f last%f rest%f tau%f reset%f I%f, cm%f, delta%f\n"
					,cycle, v, gv_v[neuron], gp_v_rest[neuron], gp_tau_m[neuron], gp_v_reset[neuron],
					I, cm, delta);
				}
#endif
				
			}

#ifdef SZSNN_KERNEL_DEBUG		
			if(in_refractory)
				printf("!!!!!!!!!!!! neuron %d %d w refractory\n", l_partitionIdx, neuron);
#endif

			bool firedInternal = (!in_refractory) && (v >= gp_v_thresh[neuron]);
			bool firedExternal = isBitSet(neuron, l_inputFired);

			if (firedInternal || firedExternal)
			{
				v = gp_v_reset[neuron];
				fired = 1;

				unsigned idx = atomic_add(&l_firedCount, 1);
				l_fired[idx] = neuron;
#ifdef SZSNN_KERNEL_DEBUG
				printf("c%d[test_update] p%dn%d fired (forced spike%d, current%d)\n",
					cycle, l_partitionIdx, neuron, firedExternal, l_inputCurrent[neuron]);
#endif
			}

			gv_v[neuron] = v;
			gv_fired[neuron] = fired;
		}

		barrier(CLK_LOCAL_MEM_FENCE);
	}
	__local unsigned l_firedO[BIT_VECTOR_WORD_PITCH];
	barrier(CLK_LOCAL_MEM_FENCE);
	
	storeDenseSpikes(l_partitionIdx, l_firedCount, l_firedO, l_fired, g_firedOutput);
#ifdef SZSNN_KERNEL_OUTPUT_SPIKES	
	storeSparseSpikes(l_partitionIdx, l_firedCount, l_fired, g_firedCount, g_fired);
#endif
}
