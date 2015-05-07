/*

Update neuron kernel

*/

#undef GLOBAL_WF_ID
#define GLOBAL_WF_ID(wg_id, wi_id)\
  (wg_id*UPDATE_NEURONS_WF_IN_WG + wi_id/UPDATE_NEURONS_WI_IN_WF)
  
#undef LOCAL_WF_ID
#define LOCAL_WF_ID(wi_id)          (wi_id/UPDATE_NEURONS_WI_IN_WF)

#undef GLOBAL_WI_ID
#define GLOBAL_WI_ID(wi_id)          (wi_id*UPDATE_NEURONS_WI_IN_WG)

#undef WI_ID_WF_SCOPE
#define WI_ID_WF_SCOPE(wi_id)       (wi_id%UPDATE_NEURONS_WI_IN_WF)

#undef ELEMENTS_PER_WF
#define ELEMENTS_PER_WF             (UPDATE_NEURONS_WI_IN_WF*\
                                     UPDATE_NEURONS_ELEMENTS_PER_WI)
                                     
#undef GLOBAL_TOTAL_WFs
#define GLOBAL_TOTAL_WFs            (UPDATE_NEURONS_GRID_SIZE_WG*UPDATE_NEURONS_WF_IN_WF)



__kernel 
void update_neurons(
      //__constant    DATA_TYPE       *c_coefficients,
      __global      GPU_DATA_TYPE       *g_model_parameters,
      __global      GPU_DATA_TYPE       *g_model_variables,
      __global      uint            *g_spike_counts,
      __global      uint            *g_spikes,
      __global      uint            *g_event_ptr,
      __global      DATA_TYPE       *g_events,
                    uint            step
                    uint            neuron_packets_total
                    uint            neuron_packets_per_wf)
 {
    uint wi_id = get_local_id(0);
    uint wg_id = get_group_id(0);
    
    /* 1 WF -> 1 SPIKE_PACKET*/
  __local uint l_spikePacketCounts[UPDATE_NEURONS_WF_IN_WG];
  __local uint l_spikePackets[UPDATE_NEURONS_WF_IN_WF * 
                    UPDATE_NEURONS_DATA_SPIKE_PACKET_SIZE_IN_WORDS];

  /* pierwszy WI w WF zeruje licznik*/
  if(WI_ID_WF_SCOPE(wi_id) == 0)
  {
    l_spikePacketCounts[LOCAL_WF_ID(wi_id)] = 0;
  }
 }
 
 /* neurony przerabiane w paczkach */
 /* wylicz ile jest wszystkich paczek */
 // uint packets = UPDATE_NEURONS_DATA_NEURONS_COUNT / ELEMENTS_PER_WF;
 // 
  //if(packets*ELEMENTS_PER_WF < UPDATE_NEURONS_DATA_NEURONS_COUNT)
  //{
 //   // istnieje niepełna paczka
 //   packets++;
 // }
  
//  /* wylicz ile paczeka na WF */
//  uint packetsPerWf = packets/GLOBAL_TOTAL_WFs;
//  if(packetsPerWf*GLOBAL_TOTAL_WFs  < packets)
//  {
//    packetsPerWf++;
//  }
  
  /* wylicz startową i końcową paczkę */
  uint startPacketWf = GLOBAL_WF_ID(wg_id, wi_id) * neuron_packets_per_wf;
  uint endPacketWf = startPacketWf + neuron_packets_per_wf;
  
  /* w razie przekroczenia zakresu danych */
  if(startPacketWf > totalChunks){startPacketWf = neuron_packets_total;}
  if(endPacketWf > totalChunks){endPacketWf = neuron_packets_total;}
  
  
  /* WF obsługuje paczkę neuronów pobraną z pamięci globalnej */
  /*TODO: instead of predetermined do first come first served using an atimic -> better balancing*/
  
  for(uint j = 0; j < (endPacketWf-startPacketWf); j++) // TODO inne for od start do end
  {
    uint neuronId = (startPacketWf + j)*ELEMENTS_PER_WF + WI_ID_WF_SCOPE(wi_id);
    
    /*WI: load event pointer and count, reset the count*/
    uint neuronEventPtrStart = *(gm_event_ptr + neuronId*UPDATE_NEURONS_STRUCT_ELEMENT_SIZE);
    int ev_count = *(gm_event_ptr + neuronId*UPDATE_NEURONS_STRUCT_ELEMENT_SIZE + 1);
    ev_count++;
    
#if (UPDATE_NEURONS_ERROR_TRACK_ENABLE)
    if(ev_count >= 0xFFFF)
    {
      atomic_or(gm_error_code,UPDATE_NEURONS_ERROR_CODE_8);
      ev_count = 0;
    }
#endif

    /*Load model variables*/