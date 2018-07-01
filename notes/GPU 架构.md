# CUDA 学习笔记 第二节

## GPU 架构

了解GPU的架构是学习CUDA编程的基础中的基础，只有这样我们才能理解CUDA是如何利用GPU资源来实现并行运算的。下图为GTX970所属的Maxwell架构:

![GPU maxwell architecture][MAXWELL_ARCHITECTURE]

我们先暂时忽略关于图形的部分，只考虑和并行编程有关的架构。 第一个概念: `CUDA Core`, GPU并行处理器。`CUDA Core`是并行计算的最小处理单位，每一个`CUDA Core`负责处理一个线程 `thread`。上图中的每一个绿色小方块就代表了一个`CUDA Core`。 第二个概念：`Streaming Multiprocessor (SM)`。`SM`是GPU程序的最小运行单位。  

[MAXWELL_ARCHITECTURE]: images/gtx970_architecture.png
