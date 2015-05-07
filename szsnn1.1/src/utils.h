#ifndef UTILS_H_
#define UTILS_H_

/**
 * \file utils.h
 *
 * \date 17-11-2012
 * \author pawel
 * \description Narzędzia pomocnicze
 */

#include <iostream>
#include <time.h>
#include <string.h>

#include <sstream>
#include <fstream>

#include "SzsnnException.hpp"


#define DO_PRAGMA(x)	_Pragma(#x)


/******************************************************************************
	>>> Operacje na stringach
******************************************************************************/

#define XSTRING(x)		#x
#define TOSTRING(x)     XSTRING(x)

/******************************************************************************
	<<< Operacje na stringach
******************************************************************************/


/*****************************************************************************
	>>> Operacje matematyczne
*****************************************************************************/

/* TODO potrzebne?
#define TESTPOW2(x)           (x && !( (x-1) & x ))
#define FLOAT_UNFLIP(f)       (f ^ (((f >> 31) - 1) | 0x80000000))
#define FLOAT_FLIP(f)         (f ^ (-int(f >> 31) | 0x80000000))
#define DIV_POW2(i, j)        (i >> j)
#define INT_CEIL(i,j)         (i/j + ((i%j) > 0))
#define ADD(i, j)             (i+j)
#define MUL(i, j)             (i*j)
#define MAD(i, j, k)          (i*j+k)
#define DIV(i, j)             (i/j)
#define MUL_UINT24(i, j)      (i*j)
*/

/*Konwersja 4-bitowych liczb binarnych na hex*/
#define BIN_0000 0
#define BIN_0001 1
#define BIN_0010 2
#define BIN_0011 3
#define BIN_0100 4
#define BIN_0101 5
#define BIN_0110 6
#define BIN_0111 7
#define BIN_1000 8
#define BIN_1001 9
#define BIN_1010 a
#define BIN_1011 b
#define BIN_1100 c
#define BIN_1101 d
#define BIN_1110 e
#define BIN_1111 f

/*Konwersja 8-bitowych liczb binarnych na hex*/
#define BIN_8_HEXIFY(b1,b2) (0x ## b1 ## b2)
#define BIN_8_RELAY(b1,b2) BIN_8_HEXIFY(b1, b2)
#define BIN_8(b1,b2) BIN_8_RELAY(BIN_ ## b1, BIN_ ## b2)

/*Konwersja 16-bitowych liczb binarnych na hex*/
#define BIN_16_HEXIFY(b1,b2,b3,b4) (0x ## b1 ## b2 ## b3 ## b4)
#define BIN_16_RELAY(b1,b2,b3,b4) BIN_16_HEXIFY(b1, b2, b3, b4)
#define BIN_16(b1,b2,b3,b4) BIN_16_RELAY(BIN_##b1, BIN_##b2, BIN_##b3, BIN_##b4)

/*Checksum*/
/*For finding primes refer to http://www.archimedes-lab.org/primOmatic.html*/
#define CHECKSUM01(checksum, data)\
  {\
    checksum += ((1705662821u + data) % 2147483659u);\
  }

/*****************************************************************************
	<<< Operacje matematyczne
*****************************************************************************/

/*****************************************************************************
	>>> Wypisywanie kontorlne
*****************************************************************************/

/* czas kompilacji */

#define PRINT_LOCATION(info, msg)	DO_PRAGMA(message info ": " \
		__FILE__ "[" TOSTRING(__LINE__) "]: " #msg)

#ifndef _WIN32
#define ERROR(msg)		PRINT_LOCATION("SZSNN compilation error!", msg)

#define WARNING(msg) 	PRINT_LOCATION("SZSNN compilation warning!", msg)

#define MESSAGE(msg) 	PRINT_LOCATION("SZSNN compilation message", msg)
#endif

/* czas wykonania */

#define PRINT               printf

#define PRINTERR(...)       fprintf(stderr,__VA_ARGS__)

#define PRINT_HEX(byteSize, data)\
  do {\
    long dataLong = *((long *)(&(data)));\
    unsigned int i;\
    for(i = (sizeof(byteSize)-1)*8; i > 0; i-=8)\
    {\
      PRINTF("%02X ",(unsigned char)(dataLong >> i));\
    }\
    PRINTF("%02X",(unsigned char)(dataLong >> i));\
  } while(0)

/*Logging macro*/
#define LOGGING_SZSNN_ON	1

#if LOGGING_SZSNN_ON
  #define LOG_SZSNN(message)\
    {\
      time_t t; time (&t); char *s=ctime(&t); s[strlen(s)-1]=0;\
      std::cout << ctime(&t) << " " << message << std::endl;\
    }
#else
  #define LOG_SZSNN(message)
#endif

/*****************************************************************************
	<<< Wypisywanie kontorlne
*****************************************************************************/

#define CHECK_CL_SUCCESS(var, msg)\
  if(var != CL_SUCCESS)\
  {\
    throw szsnn::SzsnnException(var, msg);\
  }

#define THROW_SZSNNEX(msg)\
  {\
    std::stringstream ss;\
    ss << msg << std::endl;\
    throw szsnn::SzsnnException(ss.str());\
  }

#define DIV_CEIL(a, b) ((((a) + (b)) - 1) / (b))
#define ALIGN(a, b) (b) * DIV_CEIL((a), (b))


#define GLOBAL_NEURON_IDX_TO_LOCAL(idx) ((idx) % (MAX_PARTITION_SIZE))
#define GLOBAL_NEURON_IDX_TO_PARTITION(idx) ((idx) / (MAX_PARTITION_SIZE))
#define LOCAL_NEURON_IDX_TO_GLOBAL(partition, idx) ((MAX_PARTITION_SIZE) * (partition) + (idx))


/************************************************
 *
 */
void skipCommentsData(std::ifstream infile, char commentChar);

#endif /* UTILS_H_ */
