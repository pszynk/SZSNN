/*
 * debug.hpp
 *
 *  Created on: 03-03-2012
 *      Author: pawel
 */


/*
@(#)File:           $RCSfile: debug.h,v $
@(#)Version:        $Revision: 3.7 $
@(#)Last changed:   $Date: 2011/04/17 22:01:21 $
@(#)Purpose:        Definitions for the debugging system
@(#)Author:         J Leffler
@(#)Copyright:      (C) JLSS 1990-93,1997-99,2003,2005,2008
@(#)Product:        IBM Informix Database Driver for Perl DBI Version 2011.0612 (2011-06-12)
*/

#ifndef DEBUG_HPP_
#define DEBUG_HPP_

#include <stdio.h>


#ifndef SZSNN_DEBUG
	#define SZSNN_DEBUG 0
#endif /* SZSNN_DEBUG */

#define SZSNN_PRINT_DEBUG_ERR(fmt, ...) \
		do { if (SZSNN_DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); } while (0)

#define SZSNN_PRINT_DEBUG_MSG(fmt, ...) \
		do { if (SZSNN_DEBUG) fprintf(stdout, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); } while (0)
#endif /* DEBUG_HPP_ */
