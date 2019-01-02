6.  For the simple matrix multiplication (P = M * N) based on row-major layout, which input matrix will have coalesced accesses?
    
    * __Answer:__ 

        Only M matrix has coalesced accesses.

    * __Explanation:__

        In the simple matrix multiplication kernel. Each thread calculates one element of the final matrix. Then, it needs to load one row from M and one col from N.
        Since our model has row-major layout, each row of M, N are placed adjacent. As a result, M has coalesced accesses.

7. For the tiled matrix multiplication based on row-major layout, which input matrix will have coallesced accesses?
    
    * __Answer:__ 

        M, N both have coalesced accesses.

    * __Explanation:__

        In the tiled matrix multiplication algorithm, we use shared memory to store common used elements from M, N in a block. For threads in a warp, they load elements
        from M and M both in row order. Thus, both M, N will have coalesced accesses. Here is the accessing pattern for each thread.

        ```CPP
            globalX = blockIdx.x * blockDim.x + thread.x
        
            globalY = blockIdx.y * blockDim.y + thread.y

            sharedMIndex = globalY * blockDim.x + TileWidt * i th of Tile + thread.x
        
            sharedNIndex = (TileHight * i th of Tile + thread.y) * blockDim.x + globalX 
        ```

8.  For the simple reduction kernel, if the block size is 1024 and warp size is 32, how many warps in a block will have divergence during the 5th iteration?
    
    * __Answer:__ 

        32 warps will have divergence.

    * __Explanation:__
        
        In Nth iteration, the stride will be 2^N. For the first 5 iterations, the strides are less than 32, which means that every warp has at least one working
        threads. In 5th iteration, the stride is 32. There is exactly one working threads in each warp, which leads to 32 divergence warps in total.  
        
        * 1th: 0 2 4 6      .... 1022 ---- 512 working threads (32 divergence warps) 
        * 2th: 0 4 8 12     .... 1020 ---- 256 working threads (32 divergence warps)
        * 3th: 0 8 16 24    .... 1016 ---- 128 working threads (32 divergence warps) 
        * 4th: 0 16 32 48   .... 1008 ---- 64  working threads (32 divergence warps)
        * 5th: 0 32 64 96   .... 992  ---- 32  working threads (32 divergence warps)
        * 6th: 0 64 128 192 .... 960  ---- 16  working threads (16 divergence warps)

9. For the improved reductrion kernel, if the block size is 1024 and warp size is 32, how many warps will have divergence during the 5th iteration?

    * __Answer:__

        0 warps will have divergence.

    * __Explanation:__

        In the improved kernel, we reduce elements to first halp in every iterations:

        * 1th: 0 1 2 3 4 ....511 ---- 512 working threads (0 divergence warps)
        * 2th: 0 1 2 3 4 ....255 ---- 256 working threads (0 divergence warps)
        * 3th: 0 1 2 3 4 ....127 ---- 128 working threads (0 divergence warps)
        * 4th: 0 1 2 3 4 ....63  ---- 64 working threads (0 divergence warps)
        * 5th: 0 1 2 3 4 ....31  ---- 32 working threads (0 divergence warps)

11. For tiled matrix multiplication out of possible range of values for BLOCK_SIZE, for what values of BLOCK_SIZE will the kernel completely avoid
    un-coalesced accesses to global memory? (in square matrix)

    * __Answer:__

        The BLOCK_SIZE's x dimention should be multiple by 32. CUDA assigns threads into warp based on its thread ID. Supposed BLOCK_SIZE is (35, N, 1), In the second warp, we will have 3 threads from the 1st row and 29 threads from the 2nd row. Obviously, they will generate un-coalesced accesses to global memory. 


12. In an attempt to improve performance, a bright young engineer changed the reduction kernel into the following. (A) Do you believe that the performance
    will imporved? Why or why not? (B) Should the engineer receive a reward or a lecture? Why?

    ```cpp
        __shared float partialSum[];
        unsigned int tid = threadIdx.x;
        for (unsigned int stride = n >> 1; stride >= 32, stride >>=1){
            __syncthreads();
            if (tid < stride){
                shared[tid] += shared[tid + stride]
            }
        }
        if (tid < 32){
            //unroll last 5 predicated steps 
            shared[tid] += shared[tid + 16];
            shared[tid] += shared[tid + 8];
            shared[tid] += shared[tid + 4];
            shared[tid] += shared[tid + 2];
            shared[tid] += shared[tid + 1];
        }
    ```
 
    * __Answer:__

        The kernel's performance will not get improved, and worsely it will generate a wrong answer. Intially, the modification want to take advantages of 
        syncrosynchronicity of threads in warp. In the last if statement, the first warp will execute one reduction at a time for all 32 threads. However, 
        he counted a wrong iteration numbers. The loop in above codes left 5 more reduction (16, 8, 4 ,2, 1), which requires 16 threads, 8 threads, 4 threads.
        2 threads, and 1 threads. Consequently, only threads with ID less than 16 should work after the loop. Obviously, he is wrong in this part.  
        
        




         