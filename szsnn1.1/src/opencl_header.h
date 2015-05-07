#ifndef OPENCL_HEADER_H_
#define OPENCL_HEADER_H_

#ifdef _WIN32

#pragma warning( push )
#pragma warning( disable : 4996 )

#include <CL/opencl.h>
#include <CL/cl.hpp>

#pragma warning( pop )
#else

//#include <CL/opencl.h>
#include <CL/cl.hpp>

#endif


#endif
