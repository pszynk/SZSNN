//#include "locales.h"

#define CURRENT_PARTITION get_group_id(0) 

#define CURRENT_EXC_IDX 0

#define CURRENT_INH_IDX 1

#ifdef SZSNN_KERNEL_DEBUG
#pragma OPENCL EXTENSION cl_amd_printf : enable
#endif


//#pragma OPENCL EXTENSION cl_khr_fp64 : enable 
// #pragma OPENCL EXTENSION cl_khr_int64_base_atomics : enable

//__constant uint sth = 1;

inline void AtomicFloatAdd(volatile __local float *source, const float operand) {
    union {
        uint intVal;
        float floatVal;
    } newVal;
    union {
        uint intVal;
        float floatVal;
    } prevVal;
    do {
        prevVal.floatVal = *source;
        newVal.floatVal = prevVal.floatVal + operand;
    } while (
            atomic_cmpxchg(
                    (volatile __local uint*)source, prevVal.intVal, newVal.intVal) 
            != prevVal.intVal);
}

//inline void AtomicDoubleTypeAdd(volatile __local double *source, const double operand) {
//    union {
//        ulong intVal;
//        double floatVal;
//    } newVal;
//    union {
//        ulong intVal;
//        double floatVal;
//    } prevVal;
//    do {
//        prevVal.floatVal = *source;
//        newVal.floatVal = prevVal.floatVal + operand;
//    } while (
//            atomic_cmpxchg(
//                    (volatile __local uint*)source, prevVal.intVal, newVal.intVal) 
//            != prevVal.intVal);
//}

uint getDBreadSlot(uint cycle)
{
    return (cycle & 0x1) ^ 0x1;
}


uint getDBwriteSlot(uint cycle)
{
    return (cycle & 0x1);
}


__kernel void gather(
				const	ulong 		cycle,				//0
				const	uint 		partitionCount,		//1
    __global 	const   uint* 	    g_partitionSizes,   //2
    __global		    FLOAT_TYPE*	g_current,			//3
				const   ulong		gq_pitch,			//4
	__global			uint*		g_gq_bundleCounter,	//5
	__global			uint*		g_gq_bundles,		//6
	__global	const	uint*		g_targets,			//7
	__global	const	FLOAT_TYPE*	g_weights			//8
	)
{
	if(get_local_id(0) == 0 && get_group_id(0) == 0)
	{
		//printf("\nTARGETS KERNEL\n");
		//for(uint i = 0; i < 6; ++i)
		//{
			//printf("[%d] %d %d\n", i, g_targets[i], g_targets[i+1]);
		//}
		
		//printf("\WEIGHTS KERNEL\n");
		//for(uint j = 0; j < 6; ++j)
		//{
			//printf("[%d] %f %f\n", j, g_weights[j], g_weights[j+1]);
		//}
		//printf("%d\n ", 
		//	g_targets[9]);
	}
    __local FLOAT_TYPE l_currentExc[MAX_PARTITION_SIZE];
    __local FLOAT_TYPE l_currentInh[MAX_PARTITION_SIZE];
    
    __local uint l_partitionSize;
    __local uint l_incomingBundlesCount;
    
    if(get_local_id(0) == 0)
    {
        l_partitionSize = g_partitionSizes[CURRENT_PARTITION];
        size_t addr = CURRENT_PARTITION * 2 + getDBreadSlot(cycle);
        l_incomingBundlesCount = g_gq_bundleCounter[addr];
        g_gq_bundleCounter[addr] = 0;
    }
    
    for(uint bNeuron = 0; bNeuron < MAX_PARTITION_SIZE; bNeuron += THREADS_PER_GROUP)
    {
        uint neuron = bNeuron + get_local_id(0);
        l_currentExc[neuron] = 0;
        l_currentInh[neuron] = 0;
    }
   
    barrier(CLK_LOCAL_MEM_FENCE);
    
    __local uint l_warpOffset[THREADS_PER_GROUP];
    
    for(uint bGroup = 0; bGroup < l_incomingBundlesCount; bGroup += THREADS_PER_GROUP)
    {
        uint group = bGroup + get_local_id(0);
        
        __local uint l_groupSize;
        
        if(get_local_id(0) == 0)
        {
            l_groupSize = 
                (bGroup + THREADS_PER_GROUP) > l_incomingBundlesCount
                ? l_incomingBundlesCount % THREADS_PER_GROUP
                : THREADS_PER_GROUP;
#ifdef SZSNN_KERNEL_DEBUG          
            printf("c%d|p%d[gather]: group size=%d, incoming=%d\n",
                cycle, CURRENT_PARTITION, l_groupSize, l_incomingBundlesCount);
#endif
        }
        barrier(CLK_LOCAL_MEM_FENCE);
        
        if(get_local_id(0) < l_groupSize)
        {
            uint sgin = g_gq_bundles[(CURRENT_PARTITION * 2 + getDBreadSlot(cycle)) * gq_pitch + group];
            l_warpOffset[get_local_id(0)] = sgin * BUNDLE_SIZE;
#ifdef SZSNN_KERNEL_DEBUG
            printf("c%d|p%d[gather]: w%d -> p%d\n", cycle, CURRENT_PARTITION, sgin, CURRENT_PARTITION);
#endif
        }
        
        barrier(CLK_LOCAL_MEM_FENCE);

        for(uint gwarp_base = 0; gwarp_base < l_groupSize; gwarp_base += WARPS_PER_GROUP)
        {
            uint bwarp = get_local_id(0) / WARP_SIZE;
            uint gwarp = gwarp_base + bwarp;
            
            uint postsynaptic;
            FLOAT_TYPE weight = 0;
            
            uint baseOffset = l_warpOffset[gwarp] + get_local_id(0) % WARP_SIZE;
            
            if(gwarp < l_groupSize)
            {
                postsynaptic = g_targets[baseOffset];
                if(postsynaptic >= MAX_PARTITION_SIZE) {
#ifdef SZSNN_KERNEL_DEBUG
                    printf("ASSERT [gather]: postsynaptic >= MAX_PARTITION_SIZE: %d >= %d"
                        , postsynaptic, MAX_PARTITION_SIZE);
#endif
                }
                weight = g_weights[baseOffset];
            }
            
            bool excitatory = (weight >= 0.0);
            __local FLOAT_TYPE* l_current = excitatory ? l_currentExc : l_currentInh;
            
           	
            if(weight != 0)
            {
                AtomicFloatAdd(l_current + postsynaptic, weight);
#ifdef SZSNN_KERNEL_DEBUG
                printf("c%d|p%d[gather]: p%dn%d(%d) %+f [warp %d]\n",
                    cycle, CURRENT_PARTITION, CURRENT_PARTITION, postsynaptic,baseOffset, weight, baseOffset / WARP_SIZE);
#endif
            }
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
    
	__global FLOAT_TYPE* g_currentExc =
			g_current + (CURRENT_EXC_IDX * partitionCount + CURRENT_PARTITION) * MAX_PARTITION_SIZE;

	__global FLOAT_TYPE* g_currentInh =
			g_current + (CURRENT_INH_IDX * partitionCount + CURRENT_PARTITION) * MAX_PARTITION_SIZE;

    for(uint bNeuron=0; bNeuron < l_partitionSize; bNeuron += THREADS_PER_GROUP)
    {
        uint neuron = bNeuron + get_local_id(0);
        g_currentExc[neuron] = l_currentExc[neuron];
        g_currentInh[neuron] = l_currentInh[neuron];
    }
}