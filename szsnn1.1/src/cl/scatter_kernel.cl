//#include "locales.h"
/* załaduj neurony strzelające

argumenty:
 * IN:
 * g -> licznik strzałów na partycję
 * g -> rzadka tablica strzałów (indeksy neuronów) na partycję
 *
 * OUT:
 * l -> ile strzałów w tej partycji
 * l -> indeksy strzelajacych w tej partycji
 *
 * 1) pierwszy wątek w bloku ładuje ile strzeliło w partycji do lokalnej
 * 2) synchronizacja lokalnej
 * 3) pętla for i = 0 -> ile strzeliło co ilość threadów na blok
 * 4)   index = i + thread w bloku
 * 5)   zapisz w lokalnej strałów pod index z globalnej z pod blok * WIELKOSC PARTYCJI + i
 * 6) synchornizuj po pamięci lokalnej
 */



/* scatter lokalne

argumenty:
 * IN:
 * cykl, ile strzaliło w partycji
 *
 * g -> bitmapa opóźnień neuronów
 * g -> zapełnienie kolejki globalnej
 * g -> kolejka globalna
 *
 * OUT:
 * g -> zapełnienie kolejki globalnej
 * g -> kolejka globalna
 *
 * 1) lokalne zapełnienia partycji (wielkość -> MAX_DELAY)
 * 2) synchronizacja lokalnej
 * 3) pętla for i = 0 -> ile strzeliło co ilość threadów na blok
 * 4)   index = i + thread w bloku
 * 5)   zapisz w lokalnej strałów pod index z globalnej z pod blok * WIELKOSC PARTYCJI + i
 * 6) synchornizuj po pamięci lokalnej
 */
#define DIV_CEIL(a, b) ((((a) + (b)) - 1) / (b))
#define ALIGN(a, b) (b) * DIV_CEIL((a), (b))

#define CURRENT_PARTITION get_group_id(0) 

#ifdef SZSNN_KERNEL_DEBUG
#pragma OPENCL EXTENSION cl_amd_printf : enable
#endif

__constant uint C_entryQueueWidth = 
    ALIGN(MAX_PARTITION_SIZE * MAX_SYNAPTIC_DELAY, 32); // w LocalQueue.cpp initialize


uint getDBreadSlot(uint cycle)
{
    return (cycle & 0x1) ^ 0x1;
}


uint getDBwriteSlot(uint cycle)
{
    return (cycle & 0x1);
}

__kernel void scatter(
				const	ulong 		cycle,				//0
				const	uint 		partitionCount,		//1
	__global			uint*		g_firedCount,		//2
	__global			uint*		g_fired,			//3
                const   uint        groupPitch,         //4
                const   uint        groupStep,          //5
	__global	const	uint2*		g_groupPtrs,		//6
	__global	const	uint2*		g_groupMatrix,		//7
				const   ulong		gq_pitch,			//8
	__global			uint*		g_gq_bundleCounter,	//9
	__global			uint*		g_gq_bundles,		//10
	__global			uint*		g_lq_entryCounter,	//11
	__global			short2*		g_lq_entries,		//12
	__global	const	ulong*		g_delayBits			//13
	)
{

    __local uint l_firedCount;
    __local uint l_fired[MAX_PARTITION_SIZE];
    
    // zaladuj strzaly do lokalnej
    
    if(get_local_id(0) == 0)
    {
        l_firedCount = g_firedCount[CURRENT_PARTITION];
    }
    barrier(CLK_LOCAL_MEM_FENCE);
    
    for(uint i = 0; i < l_firedCount; i += THREADS_PER_GROUP)
    {
        uint spike = i + get_local_id(0);
        if (spike < l_firedCount)
        {
            l_fired[spike] = g_fired[CURRENT_PARTITION * MAX_PARTITION_SIZE + spike];
        }
    }
    barrier(CLK_LOCAL_MEM_FENCE);
    
    // lokalne rozsiewanie
    __local uint l_lq_entryCounter[MAX_SYNAPTIC_DELAY];
    
    //zaladowanie lokalnej kolejki (licznik)
    if(get_local_id(0) < MAX_SYNAPTIC_DELAY)
    {
        l_lq_entryCounter[get_local_id(0)] = 
            g_lq_entryCounter[CURRENT_PARTITION * MAX_SYNAPTIC_DELAY + get_local_id(0)];
    }
    barrier(CLK_LOCAL_MEM_FENCE);

//	if(get_local_id(0) == 0)
//		printf("(0) partition %d\n", CURRENT_PARTITION);

    for(uint spike = 0; spike < l_firedCount; ++spike)
    {
    
//		if(get_local_id(0) == 0 && spike == 0)
//			printf("(1) partition %d\n", CURRENT_PARTITION);

        uint neuron = l_fired[spike];
//		if(get_local_id(0) == 0)
//			printf("spike %d, partition %d %d neuron\n", spike, CURRENT_PARTITION, neuron);
        __local ulong delayBits;
        if(get_local_id(0) == 0)
        {
            delayBits = g_delayBits[CURRENT_PARTITION * MAX_PARTITION_SIZE + neuron];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
        
        uint delay0 = get_local_id(0);

//		if(get_local_id(0) == 0 && spike == 0)
//			printf("(2) partition %d\n", CURRENT_PARTITION);

        if(delay0 < MAX_SYNAPTIC_DELAY)
        {
//			if(get_local_id(0) == 0 && spike == 0)
//				printf("(3) partition %d\n", CURRENT_PARTITION);
            bool hasDelay = (delayBits >> (ulong)(delay0)) & 0x1;
            if(hasDelay)
            {

//				if(get_local_id(0) == 0 && spike == 0)
//					printf("(4) partition %d\n", CURRENT_PARTITION);

                // zaladuj na lokalna kolejke
                uint delaySlot = (cycle + delay0) % MAX_SYNAPTIC_DELAY;
                uint next = atomic_inc(l_lq_entryCounter + delaySlot);

                //TODO USUNAC
                if (next >= C_entryQueueWidth)
                {
#ifdef SZSNN_KERNEL_DEBUG
                    printf("przepalniona kolejka\n");
#endif
                }
				else
				{
					uint offset = (CURRENT_PARTITION * MAX_SYNAPTIC_DELAY + delaySlot) *
						C_entryQueueWidth +
						next;
					g_lq_entries[offset] = (short2) (neuron, delay0);
					//printf("wsadzam %d:%d, d-%d \n",CURRENT_PARTITION, neuron, delay0 );
				}
			}
			else
			{
#ifdef SZSNN_KERNEL_DEBUG
				if(get_local_id(0) == 0)
					printf("partition %d:%d -> %d\n", CURRENT_PARTITION, neuron, delayBits);
#endif
			}
        }
    }
    barrier(CLK_LOCAL_MEM_FENCE);
    
    if(get_local_id(0) < MAX_SYNAPTIC_DELAY)
    {
        g_lq_entryCounter[CURRENT_PARTITION * MAX_SYNAPTIC_DELAY + get_local_id(0)] =
            l_lq_entryCounter[get_local_id(0)];
            
    }
	barrier(CLK_GLOBAL_MEM_FENCE); // POTRZEBNA??????????
	// lokalny scatter END
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////   
    // globalny scatter BEGIN
    
    __local uint l_gq_bundleCounter_TEMP[MAX_PARTITION_SIZE];
    
    /* ile wejsc w danej partycji dla danego cyklu */
    __local uint l_lq_partitionAndCycle;

//------------------------------------------------------------------------------------------------------
    /* l_lq_partitionAndCycle będzie zawierać liczbę wpisów w kolejce localQueue
     * [neuron strzelający, opóźnienie jego synaps] których strzały dotrą do celu
     * w następnym cyklu, czyli wpisy z `cycle % MAX_SYNAPTIC_DELAY` listy */
    if(get_local_id(0) == 0) 
    {
        uint offset = CURRENT_PARTITION * MAX_SYNAPTIC_DELAY + 
                        (cycle % MAX_SYNAPTIC_DELAY);
        l_lq_partitionAndCycle = atomic_xchg(g_lq_entryCounter + offset, 0);
#ifdef SZSNN_KERNEL_DEBUG
		printf("partition_cycle %d (part / cycle) (%d / %d)\n", l_lq_partitionAndCycle, cycle, CURRENT_PARTITION);
#endif
    }
    barrier(CLK_LOCAL_MEM_FENCE);

//------------------------------------------------------------------------------------------------------
    /* przerabiamy wszystkie wpisy z kolejki localQueue równolegle
     * jednocześnie wy*/
    for(uint bLqEntry = 0; bLqEntry < l_lq_partitionAndCycle; bLqEntry += THREADS_PER_GROUP)
    {
        
        uint iLqEntry = bLqEntry + get_local_id(0);
 
 #ifdef SZSNN_KERNEL_DEBUG       
		__local short2 s_lq[THREADS_PER_GROUP]; //DEBUG
#endif		
        __local uint l_offset[THREADS_PER_GROUP];
        __local uint l_len[THREADS_PER_GROUP];
        
        l_len[get_local_id(0)] = 0;
        
        
        if(iLqEntry < l_lq_partitionAndCycle)
        {
            uint offset = ((cycle % MAX_SYNAPTIC_DELAY) + 
                                MAX_SYNAPTIC_DELAY * CURRENT_PARTITION) *
                            C_entryQueueWidth;
            short2 entry = g_lq_entries[offset + iLqEntry];
            
            short neuron = entry.x;
            short delay0 = entry.y;

#ifdef SZSNN_KERNEL_DEBUG
			s_lq[get_local_id(0)] = entry;
			
			if(delay0 >= MAX_SYNAPTIC_DELAY) { 
                printf(">>ASSERT delay0 %d >= MAX_SYNAPTIC_DELAY %d >>>>>>>>>>>\n", delay0, MAX_SYNAPTIC_DELAY);
            }
			if(neuron >= MAX_PARTITION_SIZE) { 
                printf(">>ASSERT neuron %d >= MAX_PARTITION_SIZE %d >>>>>>>>>>>\n", neuron, MAX_PARTITION_SIZE);
            }
#endif
            size_t addr_offset = 
                (CURRENT_PARTITION * MAX_PARTITION_SIZE + neuron) *
                MAX_SYNAPTIC_DELAY + delay0;
            uint2 addr = g_groupPtrs[addr_offset];
            
            l_offset[get_local_id(0)] = addr.x;
            l_len[get_local_id(0)] = addr.y;
            
#ifdef SZSNN_KERNEL_DEBUG
			printf("c%d|p%d[global scatter]:\n\t dequeued n%d d%d from local queue (%d warps from %d)\n",
				cycle, CURRENT_PARTITION, neuron, delay0, l_len[get_local_id(0)], l_offset[get_local_id(0)]);
#endif
        }
        barrier(CLK_LOCAL_MEM_FENCE);

//------------------------------------------------------------------------------------------------------
        /* załadowaliśmy maksymalnie THREADS_PER_GROUP wpisów [początek grupy w GroupMatrix, ile paczek]
         * dochodzacych w następnym cyklu
         */
     
        /* przerabiam wczytane wejscia z kolejki lokalnej */
        /* ile ich wczytałem w tym obiegu ?*/
        uint jLqMax = min((uint)THREADS_PER_GROUP,  l_lq_partitionAndCycle - bLqEntry);
    
        /* zaczynam od wpisu nr. bLqEntry */
        for(uint jbLq = 0; jbLq < jLqMax; jbLq += groupStep)
        {
            uint jLq = jbLq + get_local_id(0) / groupPitch;
            
            /* groupPitch to największa liczba zapisanych paczek
             * w groupMatrix dla jednej pary [neuron, delay]
             */

#ifdef SZSNN_KERNEL_DEBUG
            if(jLq >= THREADS_PER_GROUP) { 
                printf(">>ASSERT jLq %d >= TPG %d >>>>>>>>>>>\n", jLq, THREADS_PER_GROUP);
            }
#endif
            uint nOut = l_len[jLq];
            if(get_local_id(0) < partitionCount)
            {
                l_gq_bundleCounter_TEMP[get_local_id(0)] = 0;
            }
            barrier(CLK_LOCAL_MEM_FENCE);
            
            uint iOut = get_local_id(0) % groupPitch;
            uint targetPartition = 0;
            uint bundleOffset = 0;
            uint localOffset = 0;
            bool valid = 
                (bLqEntry + jLq) < l_lq_partitionAndCycle && iOut < nOut;
            
            if(valid)
            {
                uint2 sout = g_groupMatrix[l_offset[jLq] + iOut];
                targetPartition = sout.x;
#ifdef SZSNN_KERNEL_DEBUG
                if(targetPartition >= partitionCount) { 
                    printf(">>ASSERT targetPartition %d >= partitionCount %d >>>>>>>>>>>\n", targetPartition, partitionCount);
                }
#endif
                bundleOffset = sout.y;
#ifdef SZSNN_KERNEL_DEBUG
                if(bundleOffset == 0) { 
                    printf(">>ASSERT bundleOffset %d == 0 >>>>>>>>>>>\n", bundleOffset);
                }
#endif
                localOffset = atomic_add(l_gq_bundleCounter_TEMP + targetPartition, 1);
            }
            barrier(CLK_LOCAL_MEM_FENCE);
            
            if(get_local_id(0) < partitionCount)
            {
                size_t gq_addr = get_local_id(0) * 2 + getDBwriteSlot(cycle);
                l_gq_bundleCounter_TEMP[get_local_id(0)] = 
                    atomic_add(g_gq_bundleCounter + gq_addr, l_gq_bundleCounter_TEMP[get_local_id(0)]);
            }
            
            barrier(CLK_LOCAL_MEM_FENCE);
            
            if(valid)
            {
                uint offset = l_gq_bundleCounter_TEMP[targetPartition] + localOffset;
                size_t base = (targetPartition * 2 + getDBwriteSlot(cycle)) * gq_pitch;
#ifdef SZSNN_KERNEL_DEBUG
                if(offset >= gq_pitch) { 
                    printf(">>ASSERT offset %d >= gq_pitch %d >>>>>>>>>>>\n",offset,  gq_pitch);
                }

                if(base >= 2 * partitionCount * gq_pitch) { 
                    printf(">>ASSERT base %d >= %d >>>>>>>>>>>\n",base, 2 * partitionCount * gq_pitch);
                }
#endif
                g_gq_bundles[base + offset] = bundleOffset;
                
#ifdef SZSNN_KERNEL_DEBUG
                printf("c%d|p%d[global scatter]:\n\t enqueued warp %d (p%dn%d -> p%d with d%d) \n\t to global queue (buffer entry %d/%lu)\n",
                cycle, CURRENT_PARTITION, bundleOffset, CURRENT_PARTITION, s_lq[jLq].x, targetPartition, s_lq[jLq].y, offset, gq_pitch);
#endif               
            }
            barrier(CLK_LOCAL_MEM_FENCE);
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
}
