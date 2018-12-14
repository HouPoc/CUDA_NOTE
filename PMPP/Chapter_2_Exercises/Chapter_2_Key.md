1. If we want to use each thread to calculate one output element of a vector addition, what whould be the expression for mapping the thread/block index?

    * __Answer:__ 
      i = blockIdx.x * blockDim.x + threadIdx.x

    * __Explanation:__ 
     In this problem, we want to map each element in the vector to exact one thread. The globalID of a thread should be the gloabl index of the vector: 
globalID = # of threads in previous blocks + # of thread before it in the current block. 
Recall that, we organize threads in to the block and organize blocks into grid:
number of threads in previous block = blockIdx.x * blockDim.x
number of threads before the target = threadIdx.x
Finally, we reach to the final answer. (section 2.5)

 
2. Assume that we want to use each thread to calculate two (adjacent) elements of a vector addition. What would be the expression for mapping the thread/block indices to i, the data index of the first element to tbe processed by a thread?

    * __Answer:__ 
      i = (blockIdx.x * blockDim.x + threadIdx.x) * 2

    * __Explanation:__
       In this problem, we want to map each thread to two elements of the result vector. The mapping pattern is: T[0] -> (V[0], V[1]), T[1] -> (V[2], V[3]).....T[n] -> (V[2n], V[2n+1]). From problem 1, we know how to do one to one mapping. With given mapping pattern, we cna easily generate the equation to calculate the vector index. (section 2.5)

3. We want to use each thread to calculate two elements of a vector addition. Each thread block process 2 * blockDim.x consecutive elements that form two sections. All threads in each block will first process a section first, each processing one element. They will then all move to the next section, each processing one element. Assume the variable i should be the index for the first element to be processed by a thread. What would be the index for mapping the thread/block indices to data index of the first element?

   * __Answer:__ 
     i = blockIdx.x * blockDim.x *2 + threadIdx.x

   * __Explanation:__
     In this problem, we want each block process (2*blockDim.x) consecutive elements. For a block with m threads, it processes (2*m) elements. Since the block process m elements at a time, two elements assigned to each thread will have a gap m. Thus, the in-block mapping relation is T[i] -> (V[i], V[i+m]). For Nth block, it will process (2 * m * N) to (2 * m * (N + 1 ) ) element. The Kth thread in Nth block will process two elements whose index are: (2 * m * N + K) and (2 * m  * N + K + m). By replacing m, N, and K will built-in variables, we will get our answer.(section 2.5)

4. For a vector addition, assume that the vector length is 8000, each thread calculate one output element, and the thread block size is 1024 threads. The programmer configure the kernel launch to have a minimal number of thread thread blocks to cover all output elements. How many threads will be in the grid?
   * __Answer:__ 
    8192

   * __Explanation:__ 
    In this programming, we need 8000 threads in total to perform our task. Since each block has 1024 threads, we need (8000/1024) 7.8125 blocks to provide enough threads. However, CUDA does not support the partial block assignment. As a result, we round up to the greater integer to prepare enough threads. In this case, we need 8 blocks, totally 8192 threads. (section 2.6)

5. If we want to allocate an array of v integer elements in CUDA device global memory, what would be an appropriate expression for the second argument of the cudaMalloc call?

   * __Answer:__
     v * sizeof(int)

   * __Explanation:__
     cudaMalloc(void** devPtr, size_t size). The first argument in this function is the device pointer to allocated memory, and the second argument in this function is the total size in bit of data allocated. v * sizeof(int) is the propery way to calculate total bits needed for a array with v integers. Figure 2.15 provides a coding sample for cudaMalloc(). (section 2.6)

6. If we want to allocate an array of n floating-point element and have a floating-poin pointer variable d_A to point to the allocate memory, what would be an appropriate expression for the first argument of the cudaMalloc() function call? 

   * __Answer:__
     (void **) &d_A

   * __Explanation:__
     Same as problem 5.  Figure 2.15 provides a coding sample for cudaMalloc(). (section 2.6)


7. If we want to copy 3000 bytes of data from host array h_A (h_A is the pointer to element 0 of the source array) to device array d_A (d_A is the pointer to element 0 of the destination array),what would be an appropriate API call for this data copy in CUDA?

   * __Answer:__
     cudaMemcpy(d_A, h_A, 3000, cudaMemcpyHostToDevice)
   * __Explanation:__
     cudaMemcpy(void* dst, const void* src, size_t count, cudaMemcpyKind kind). This is the function used to copy memory among host and device. dst is the pointer to the destination, src is the pointer to the source, count is the data size in bytes, kind specifies the direction of the copy: cudaMemcpyHostToHost, cudaMemcpyHostToDevice, cudaMemcpyDeviceToHost, cudaMemcpyDeviceToDevice. By filling this API calls, we can ge our answer. Figure 2.10 provides a coding sample for cudaMemcpy(). (section 2.4)


8. How would one declare a varaible err that can appropriately reveive returned vale of a CUDA API call?
   * __Answer:__
     cudaError_t err

   * __Explanation:__ 
     cudaError_t is the return type of CUDA runtime API. (section 2.4)

9. A new summer intern was frustrated with CUDA. He has been complaining taht CUDA is very tedious: he hard to declare many functions that he plans to execute on both the host and the device twice,once as a host function and once as a device function. What is your response?

   * __Answer:__
     CUDA allows programmer to generate functions for both device and host at one time. It provides three different keywords "\_\_device\_\_", "\_\_host\_\_", and "\_\_global\_\_". A function labeled with "\_\_device\_\_" is a device function can ony be called from device, A functio labeled with "\_\_host\_\_" is a host function can only be called from host. A function labeled with "\_\_global\_\_" is a device function can only be called from host. Programmers can label a function with both "\_\_device\_\_" and "\_\_host\_\_" for a function to tell the compiler to generate two versions of the function. Note that a function without any above label is treated as host function, and "\_\_global\_\_" label cannot be used with other label. figure 2.13 provides a detail explanation to this problem. (section 2.5)

