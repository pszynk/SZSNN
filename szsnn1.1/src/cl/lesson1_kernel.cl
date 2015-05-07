#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable
__constant char hw[] = "Hello World\n";
__kernel void hello(__global char * out)
{
    size_t tid = get_global_id(0);
    out[tid] = hw[tid];
}

__kernel void DEFAULT(__global const float* src_a,
                     __global const float* src_b,
                     __global float* res,
		   const int num)
{
   /* get_global_id(0) returns the ID of the thread in execution.
   As many threads are launched at the same time, executing the same kernel,
   each one will receive a different ID, and consequently perform a different computation.*/
   const int idx = get_global_id(0);

   //printf("global id %d\n", idx);

   /* Now each work-item asks itself: "is my ID inside the vector's range?"
   If the answer is YES, the work-item performs the corresponding computation*/
   if (idx < num)
      res[idx] = src_a[idx] + src_b[idx];
}
