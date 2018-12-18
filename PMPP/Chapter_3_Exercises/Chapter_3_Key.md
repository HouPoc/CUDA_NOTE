3. If the SM of a CUDA device can take up to 1536 threads and up to 4 thread blocks. Which of the following block configuration would result in the largest number of threads in the SM?
    
    * __Answer:__
        
        256 threads per block
    
    * __Explanation:__ 
        
        According to the problem, the theoretical maximum number of thread in a block is 384 (1536/4) threads. Thus, the actual number of threads in a block should be less than 384, otherwise, it will exceed the SM limitation. In given options, 256 threads per block is a reasonable configuration that can achieve largest number of threads in a SM. 

4. For a vector addition, assume that the vector length is 2000, each thread calculates one output element, and the thread block size is 512 threads, how may threads will be in the grid?
    * __Answer:__
        
        2048 will be in the grid
    
    * __Explanation:__
        
        In previous chapter, we learned that to provide enough threads needs to round up to a greater integer. In this case, we have ceil(2000/512) = 4 blocks. The total threads in 4 block is 512 * 4 = 2048 threads.

5. With reference to the previous question, how many warps do you expect to have divergence due to the boundary check on vector length?
    * __Answer:__
        
        Only 1 warp will have branch divergence.
    
    * __Explanation:__
        
        Branch divergence happens when threads in a warp execuate both branchs of a conditional statement. In this problem, there is no doubt the first 1983 (62 * 32) threads only execute vector addition. Threads (2016 to 2047) in last warp do not perform and vector addition. Threads (1984 to 1999) will do the vector addition, while threads (2000 to 2015)do nothing. They are all in the same warp. As a result, there only 1 warp will have branch divergence.

6. You need to write a kernel that operates on an image of size 400 x 900 pixels. You would like to assignone thread to each pixel. You would like your thread blocks to be square and to use maximum number of threads per block possible on device (your device has compute capability 3.0). How would you select the grid dimentions and the block dimentions?
   
    * __Answer:__
        
        Device with compute capability 3.0 allows up to 1024 threads per block. Also, we want to have a squre thread block, and then final block dimention will be [32, 32 ,1] ( 32^2 = 1024). The cooresponding grid dimention will be [13, 29, 1] (400/32, 900/32, 1).

7. With reference to the previous question, how may idle threads do you expect to have?
    
    * __Answer:__
        
        number of idle thread = number of total threads - number of pixels
            = 13 * 32 * 29 * 32 - 400 * 900 
            = 26048 

8. Consider a hypothetical blocks with 8 threads executing a section of code before reaching a barier. The threads require the following amount of time (in micrisecinds) to execute the sections: 2.0, 2.3, 3.0, 2.8, 2.4, 1.9, 2.6, and 2.9 and to spend the rest of their tiem waiting for the barrier. What percentage of the total execution time of the thread is spent waiting for the barrier?
    
    * __Answer:__
        
        As long as all threads reach to the barrier, they can go to next stage. In other words, the total execution time is determined by the slowest thread that needs 3.0 ms. Since all threads execute parallelly, the final execution time of each thread is 3.0 ms. Then, we can calculate the waiting time of each block: 1.0 ms, 0.7 ms, 0.0 ms, 0.2 ms, 0.6 ms, 1.1 ms, 0.4 ms, and 0.1 ms. The corresponding waiting perentages: 33.3%, 23.3%, 0.6%, 6.6%, 20%, 36.6%, 13.3%, 3.3%. 

9. Indicate which of the following assignments per multiprocessor is possible:
    
    * __Answer:__
        1. 8 blocks with 128 threads each on a device with compute capability 1.2. 
        2. 8 blocks with 128 threads each on a device with compute capability 3.0.
        3. 16 blocks with 64 threads each on a device with compute capability 3.0.
    
    * __Explanation:__
        
        To answer this question, we need to figure the SM limitation of each compute capability. Specifications can be easily found on [Wikipedia](https://en.wikipedia.org/wiki/CUDA#Version_features_and_specifications):
        * Compute Capability 1.0: 8 blocks, 768 threads
        * Compute Capability 1.2: 8 blokcs, 1024 threads
        * Compute Capability 3.0: 16 blocks, 2048 threads 
        
        Then, we can tell which one is correct.  

10. A CUDA programmer says that if they launch a kernel with only 32 threads in each block, they can leave out the \_\_syncthreads() instruction where ever barrier synchronization is needed. Do you think this is a good idea? Explain.
    
    * __Answer:__
        
        It is not good because the warp size may not be 32. If the warp size is 32, he can ignore \_\_synchreads() becuase all threads in a warp will be SIMD-synchronous. They will ececute the same instruction at a insant. All threads will be in the same state. If the warp size has been changed to 16 or less, it will have two warps. In this case, they could access to wrong data without \_\_syncthreads().    

11. A student mentioned that he was able to multiply two 1024 x 1024 matrices by using a tiled matrix multiplication code with 32 x 32 thread blocks. He is using a CUDA device that allows up to 512 threads per block and up to 8 blocks per SM. He further mentuoned that each thread in a thread block calculates one element of the result matrix. What would be your reaction and why?  
    
    * __Answer:__  
        
        His statement is clearly not valid respecting to his device contrains. Starting with simple caculations, to multiplay two 1024 x 1024 matrices using 32 x 32 thread blocks requires each block to calcuate 1024 elements (1024 / 32 x 1024 /32). Also, he mentioned that each tread only calculate one element of the result matrix. Thus, he needs 1024 threads in each thread block, but his device only support 512 threads per block. As a result, his statement is not valid.  
