1. Consider matrix addition. Can one use shared memory to reduce global memory bandwidth consumption?
    
    __Answer:__
        
      Matrix addition will not get benefits with shared memory. When use shared memory, we want to reduce duplicate loading work. In other words, data stored in shared memory will be used in more than once in the thread block. In matrix addition, each cell of input matrices will be used only once. Thus, there is no need to use shared memory.

2. Draw the equivalent of Fig 4.14 for an 8 x 8 matrix multiplication with 2 x 2 tiling and 4 x 4 tiling. Verify that the reduction in global memory bandwidth is indeed proportional to the dimensions of the tiles.
    
   __Answer:__
        
      The figure above is the visualization of the first thread block in 8 x 8 matrix multiplication with 2 x 2 tiling. The lighter the color is, the earlier the iteration will be. In the first thread block, there are 4 (2 x 2) threads,and each thread will load one element from the first input matrix and one elements from the second input matrix. In the outer loop, the first iteration will load first 2 x 2 matrix from the first matrix and first 2 x 2 matrix from the second matrix (marked with lightest shadow) and perform dot products for partial row and col. By completing the loop, each row from the first matrix will be used to calculate 2 elements but loaded once, and each col from the second matrix is also used to calculate 2 elements but loaded once. Based on the figure, row 1 is used for 1A and 1B and col A is used for 1A and 2A. Without using the tile matrix, we need to load row 1 or col A twice to calculate 2 elements. It is same for all thread blocks, and we can conclude that 2 x 2 tile will reduce global memory bandwidth by 2 times.
    
    Another figure here is the visualization of the first thread block in 8 x 8 matrix multiplication with 4 x 4 tiling. Similarly, each iteration of the outer loop will use loaded elements 4 times to perform dot product for sub-array for 4 different elements. It will reduce the global memory bandwidth by 4 times.
    
    Statements is verified by our observations. If we use N x N tiling in our algorithm, input data will be reused for N elements of the final result. As a result, the total global memory bandwidth will be reduced by N times.
    
3. What types of incorrect execution behavior can happen if one or both \_\_syncthreads() are omitted in the kernel of Fig. 4.16?
   
   __Answer:__
        
     If the first barrier was removed, the tiled matrices data will not be fully loaded to shared memory. As a result, the nest loop will not access to correct numbers to calculate the dot product. If the second barrier was removed, threads who complete the nested loop faster will load data from next tiled matrix to shared memory. As a result, slower threads may access data from next tile and generate a wrong dot product.

4. Assuming the capacity is not an issue for registers or shared memory, give one important reason why it would be valuable to use shared memory instead of registers to hold values fetched from global memory? Explain your answer. 
   
   __Answer:__
        There is no data sharing on registers because registers are private to each thread.  Furthermore, the size of shared memory is much larger than registers, using shared memory can store more data.

5. For our tiled matrix-matrix multiplication kernel, if we use a 32 x 32 tile, what is the reduction of memory bandwidth usage of input matrices M and N?
    
    __Answer:__
    
      1/32 of the original usages.
        
    __Explanation:__
    
      See Problem 2 for proof.

6. Assume that a CUDA kernel is launched with 1000 thread blocks, with each having 512 threads. If a variable is declared as a local variable in the kernel, how many version of the variable will be created through the lifetime of the execution of the kernel?
   
   __Answer:__
       
      512 * 1000 = 512000 versions
    
    __Explanation:__
       
      If a variable is defined in kernel, it will be placed into registers and its scope will be limited to one individual thread. There will be one copy of this variable created for every threads that execute the kernel function. In this problem, there is 512 * 1000 threads and then 512 * 1000 versions through the lifetime of the execution of the kernel.

7. In the previous question, if a variable is declared as a shared memory variable, how many version of the variable will be created throughout the lifetime of the execution of the kernel?
    
    __Answer:__
       
      1000 versions
    
    __Explanation:__
      
      Unlike automatic variable in kernel, variables in shared memory is shared by an individual thread block. In other words, it only has one copy for one block. In previous problem, there are 1000 thread blocks, so it will have 1000 versions in this problem.

8. Consider performing a matrix multiplication of two input matrices with dimensions N x N. How many times is each element in the input matrices requested from global memory in the following situation?
    
    __Answer:__
        
      Case 1: No tiling
          
      A ROW of data from the first matrix will be used to calculate one ROW of data of the result matrix. A COL of data from the second matrix will be used to calculate one COL of the result matrix. Since the result matrix's dimension is N (# of rows) x N (# of cols). A ROW of data from the first matrix will be used N (# of cols) times because each row of result matrix has N (# of cols) elements. Thus, elements from the first matrix will be requested by N (# of cols) times. Similar, elements from the second matrix will be requested by N (# of rows) times. 
       
      Case 2: Tiles of size T x T
      
      For the first matrix, it will request (N/T) times where N is the # of rows inf the result matrix. For the second matrix, it will request (N/T) times where N is the # of cols in the result matrix.

9. A kernel performs 36 floating-point operations and 7 32-bit word global memory access per thread. For each of the following device properties, indicate whether is compute or memory-bound.

    __Answer:__
    
      * Case 1: 
      
           Peak FLOPS = 200 GFLOPS, Peak Memory Bandwidth = 100 GB/s
           
           200 GFLOPS = 200 * 10^9 floating-point operations/s
           
           Computation Cap: 200 * 10^9 / (36) = 5.55 * 10^9 threads/s
           
           100 GB/s = 100 * 10^9 bytes/s
           
           Memory Cap = 100 * 10^9 / (7 * 32 / 4) = 3.57 * 10^9 threads/s
           
           5.55 * 10^9 > 3.57 * 10^9
           
           For this device, kernel will be memory-bound.

      * Case 2: 
      
           Peak FLOPS = 300 GFLOPS, Peak Memoyr Bandwidth = 250 GB/s
           
           300 GFLOPS = 300 * 10^9 floating-point operations/s
           
           Computation Cap: 300 * 10^9 / (36) = 8.33 * 10^9 threads/s
           
           250 GB/s = 250 * 10^9 bytes/s
           
           Memory Cap = 250 * 10^9 / (7 * 32 / 4) = 8.92 * 10^9 threads/s    
           
           8.33 * 10^9 < 8.92 * 10^9
           
           For this device, kernel will be compute-bound.


10. To manipulate tiles, a new CUDA programmer has written the following device kernel, which will transpose each tile in a matrix. The tiles are of size BLOCK_WIDTH by BLOCK_WIDTH and each of the dimensions of matrix is known to be a multiple of BLOCK_WIDTH. The kernel invocation and code are shown below BLOCK_WIDTH is known at compile time, but could be set anywhere from 1 to 20. 
    ```cpp
        dim3 blockDim(BLOCK_WIDTH, BLOCK_WIDTH);
        dim3 gridDim(A_width/blockDim.x, A_height/blockDim.y);
        BlockTranspose<<<gridDim, blockDim>>>(A, A_width, A_height);

        __global__ void
        BlockTranspose(float* A_element, int A_width, int A_height)
        {
            __shared float blockA[BLOCK_WIDTH][BLOCK_WIDTH];
            int baseIdx = blockIdx.x * BLOCK_SIZE + threadIdx.x;
            baseIdx += (blockIdx.y * BLOCK_SIZE + threadIdx.y) * A_width;
            blockA[threadIdx.y][threadIdx.y] = A_elements[baseIdx];
            A_elements[baseIdx] = blockA[threadIdx.x][threadIdx.y];
        }
    ```
    * Out of the possible range of values for BLOCK_SIZE. for what values of BLOCK_SIZE will this kernel function execute correctly on the device?

        __Answer:__
        
         Before answering this problem, the first thing to do is to understand the given kernel. First, this kernel _only_ _find_ _transpose_ _for_ _the_ _tile_ _matrix_ rather than the whole matrix. Second, this kernel uses shared memory to store the original value and then each thread find the target element in shared memory. Then main problem here is that no barrier before accessing shared memory. 

         To make this kernel work well without barriers, we need to turn it into a synchronous kernel. All threads in a thread block should execute the same instruction at an instant. To do this, we need to limit the thread block's size under 32, which put all threads in a block into one warp. Then, they can be synchronous. Thus, BLOCK_SIZE < int(sqrt(32)) -> BLOCK_SIZE < 5.    
    * If the code does not execute correctly for all BLOCK_SIZE values, suggest a fix to the code to make it work for all BLOC_SIZE values.
        
        __Answer:__
        
         The only thing we need to do is to add two barriers in this kernel. The first barrier is placed before changing the original matrix so that all threads finish loading data to shared memory. The second barrier is placed after changing the origninal matrix so that the faster threads will not pollute current shared memory with data from next tile.

