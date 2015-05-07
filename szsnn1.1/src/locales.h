#ifndef LOCALES_H_
#define LOCALES_H_

/**
 * \file locales.h
 *
 * \date 17-11-2012
 * \author pawel
 * \description Parametry i stałe programu
 */

/* typ danych obliczeń (precyzja) */




#define MAX_CONNECTIONS_PER_NEURON (1<<10)

#define MAX_PARTITION_SIZE 32//(1<<10)

#define MAX_PARTITION_COUNT (1<<10)

#define MAX_INPUTS_SIZE MAX_PARTITION_SIZE

#define MAX_NETWORK_SIZE MAX_PARTITION_COUNT * MAX_PARTITION_SIZE

#define BIT_VECTOR_WORD_SIZE 32

#define BIT_VECTOR_BYTE_PITCH ((MAX_PARTITION_SIZE)/8)

#define BIT_VECTOR_WORD_PITCH ((MAX_PARTITION_SIZE)/(BIT_VECTOR_WORD_SIZE))

//WORKFLOW_SIZE
#define WARP_SIZE 4//32 //32 64

#define BUNDLE_SIZE WARP_SIZE

#define NEURONS_PER_WI 4

#define THREADS_PER_GROUP ((MAX_PARTITION_SIZE)/(NEURONS_PER_WI))

#define WARPS_PER_GROUP (THREADS_PER_GROUP / WARP_SIZE)

#define MIN_SYNAPTIC_DELAY 1

#define MAX_SYNAPTIC_DELAY 4//64//((MAX_PARTITION_SIZE)/(NEURONS_PER_WI)) // < 64

#define SZSNN_COMMENT_CHAR	'#'

#define SZSNN_ROOT_PATH		"/home/pawel/Studia/PDI/SZSNN/szsnn1.0/"

#define SZSNN_SRC_PATH 		"/home/pawel/Studia/PDI/SZSNN/szsnn1.0/src/"

#define SZSNN_SRC_CL_PATH 	"/home/pawel/Studia/PDI/SZSNN/szsnn1.0/src/cl/"

#define SZSNN_CONFIG_PATH 	"/home/pawel/Studia/PDI/SZSNN/szsnn1.0/config/"

#define TARGET_DEVICE_NAME                                  "GeForce 8400M GS;Tahiti;Cayman;Juniper"

#define OCL_COMPILER_OPTIONS_FILE_NAME                        "oclCompilerOptions.txt"

#define _SZSNN_NONE_KERNEL		"__NONE"

#ifdef SZSNN_DEBUG
	#define SZSNN_NO_INPUT_TO_OUTPUT 1
#else
	#define SZSNN_NO_INPUT_TO_OUTPUT 1
#endif

#endif /* LOCALES_H_ */
