#ifndef GPULOCALES_H_
#define GPULOCALES_H_

/**
 * \file gpulocales.h
 *
 * \date 11-12-2012
 * \author pawel
 * \description
 */
#define EVENT_TIME_SLOTS							16

#define SPIKE_PACKETS                               512
#define SPIKE_PACKET_SIZE                           16
#define	NEURONS_COUNT								1<<12

#define WI_IN_WF								64

#define WF_IN_WG								1

#define WI_IN_WG

/* UPDATE NEORONS */


#define UPDATE_NEURONS_ELEMENTS_PER_WI                        1

#define UPDATE_NEURONS_WI_IN_WF 				WI_IN_WF

#define UPDATE_NEURONS_WF_IN_WG					WF_IN_WG

#define UPDATE_NEURONS_WI_IN_WG				(UPDATE_NEURONS_WF_IN_WG *\
														UPDATE_NEURONS_WI_IN_W)

#define UPDATE_NEURONS_GRID_SIZE_WG			(SPIKE_PACKETS/\
                                                 UPDATE_NEURONS_WF_IN_WG)


#define UPDATE_NEURONS_DATA_NEURONS_COUNT			NEURONS_COUNT

/*Spike data structure parameters*/
#define UPDATE_NEURONS_DATA_SPIKE_PACKET_SIZE                  SPIKE_PACKET_SIZE
#define UPDATE_NEURONS_DATA_SPIKE_SIZE_IN_WORDS               1
#define UPDATE_NEURONS_DATA_SPIKE_PACKET_SIZE_IN_WORDS                  (UPDATE_NEURONS_SPIKE_DATA_BUFFER_SIZE * \
                                                                  UPDATE_NEURONS_SPIKE_DATA_UNIT_SIZE_WORDS)


#endif /* GPULOCALES_H_ */
