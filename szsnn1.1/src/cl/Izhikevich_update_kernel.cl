//#include <locales.h>
//#define __kernel
//#define __global
//#define __local

#define CURRENT_EXC_IDX 0
#define CURRENT_INH_IDX 1

#define NULL 0

#define	p_A			0
#define	p_B			1
#define	p_C			2
#define	p_D			3


#define	v_V			0
#define	v_U			1
//#define	v_FIRED		3

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

__kernel void Izhikevich_update(
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
	__global const FLOAT_TYPE* gp_a		= g_neuronParams_inPartition + p_A * neuronParamStep;
	__global const FLOAT_TYPE* gp_b		= g_neuronParams_inPartition + p_B * neuronParamStep;
	__global const FLOAT_TYPE* gp_c		= g_neuronParams_inPartition + p_C * neuronParamStep;
	__global const FLOAT_TYPE* gp_d		= g_neuronParams_inPartition + p_D * neuronParamStep;


	__global FLOAT_TYPE* gv_v		 = g_neuronVars_inPartition + v_V * neuronVarsStep;
	__global FLOAT_TYPE* gv_u		 = g_neuronVars_inPartition + v_U * neuronVarsStep;
	//__global FLOAT_TYPE* gv_fired	 = g_neuronVars_inPartition + v_FIRED * neuronVarsStep;


	for(unsigned i=0; i < l_partitionSize; i += THREADS_PER_GROUP)
	{
		unsigned neuron = i + get_local_id(0);
		
		if (neuron <  l_partitionSize)
		{

			FLOAT_TYPE I = g_currentExc[neuron] + g_currentInh[neuron] + l_inputCurrent[neuron];

			FLOAT_TYPE v = gv_v[neuron];
			FLOAT_TYPE u = gv_u[neuron];
			
			FLOAT_TYPE a = gp_a[neuron];
			FLOAT_TYPE b = gp_b[neuron];
			//FLOAT_TYPE c = gp_c[neuron];
			//FLOAT_TYPE d = gp_d[neuron];

#ifdef SZSNN_KERNEL_DEBUG
			if(CURRENT_PARTITION == 0 && neuron == 3)
			{
				printf("\t>>>%d>>>I %f Ie%f Ii%f\n",
				cycle,  I, g_currentExc[neuron], g_currentInh[neuron]);
				
				printf("\t>>>%d>>>V %f U%f a%f b%f c%f d%f\n",
				cycle,  v, u, a, b, gp_c[neuron], gp_d[neuron]);
				
			}
#endif
		
				




			bool fired = false;

//			for(uint step = 0; step < 4; ++step)
//			{
//				if(!fired)
//				{
//					v += 0.25 * ((0.04 * v + 5.0) * v + 140.0 - u + I);
//					u += 0.25 * (a * (b * v - u));
//					fired = (v >= 30.0);
//				}
//			}
FLOAT_TYPE dv = ((0.04 * v + 5.0) * v + 140.0 - u + I);
FLOAT_TYPE du = (a * (b * v - u));
v += dv;
u += du;
fired = (v >= 30.0);

			bool firedExternal = isBitSet(neuron, l_inputFired);

#ifdef SZSNN_KERNEL_DEBUG			
				if(CURRENT_PARTITION == 0 && neuron == 3)
				{
					printf("\t>>>%d>>>newV%f dV%f newU%f dU%f fired%d extFired %d\n"
					,cycle, v, dv, u,du, fired,  firedExternal);
				}
#endif

			if (fired || firedExternal)
			{
				v =  gp_c[neuron];
				u += gp_d[neuron];

				unsigned idx = atomic_add(&l_firedCount, 1);
				l_fired[idx] = neuron;
#ifdef SZSNN_KERNEL_DEBUG
				printf("c%d[izh_update] p%dn%d fired (forced spike%d, current%d)\n",
					cycle, l_partitionIdx, neuron, firedExternal, l_inputCurrent[neuron]);
#endif
			}

			gv_v[neuron] = v;
			gv_u[neuron] = u;
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
