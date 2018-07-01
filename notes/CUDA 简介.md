# CUDA 学习笔记  第一节
##  简介
CUDA（Compute Unified Device Architecture) 是由著名核弹厂商英伟达（NVIDIA）并行运算平台。类似的并行运算框架还有由 Khronos Group 开发的OpenCL（Open Computing Language）。在设计高性能的并行运算框架时，工程师们需要充分了解GPU架构，然后做出针对性的优化。CUDA不可避免的成为了一个闭源项目并且属于英伟达GPU独占，在一定程度上促进了英伟达的商业发展。目前为止，CUDA已经开发到9.1版本支持从G80之后包括G80的所有CPU，所支持的显卡架构包括Tesla, Fermi, Kelper, Maxwell, Pascal 和最新的Volta架构。

相比于CPU，GPU拥有着完全不同的设计理念。CPU在设计的过程中更加注重于顺序计算（Sequential Code Performance)因此CPU拥有十分强大的逻辑控制单元，而且CPU拥有相对较大的缓存空间（Cache）来缩短获取指令或数据延迟。而GPU在设计过程时更注重浮点运算（Floating Point Calculation）的质量和速度，只有这样才能更加准确，更加快速地去渲染场景或模拟物理现象。简而言之，在渲染过程种并没有大量的分支跳转（branch）更多的是巨量的基于输入的运算，因此GPU并不需要太过强大的逻辑运算单元。GPU所需要的是更强大更稳定的带宽（bandwith）来实现大量数据的I/O和成规模的运算核心对数据进行运算。在2007年，核弹厂推出了CUDA这一并行运行平台并对其GPU进行重新设计以适应CUDA的特性。

在学习使用CUDA的时候，我会穿插关于GPU架构的知识来帮助大家了解CUDA API的原理。

## 配置开发环境
在开始学习CUDA之前，第一件事就是配置开发环境。根据核弹厂官网，你首先需要~~一枚核弹~~一块英伟达的显卡，之后才需要去核弹厂的[CUDA分区](https://developer.nvidia.com/cuda-downloads)根据网站提示去下载CUDA SDK（滑稽的Mac OSX）。

![CUDA下载界面][CUDA_DOWNLOAD]

以安装Windows版本为例，下载完成之后只需按指示点点点即可，安装过程中会出现屏幕变黑几秒的状况。坐和放宽，显卡驱动的正常操作而已。安装完成之后，可以通过在命令行中输入 `nvcc --version` 来验证是否安装成功。

![CUDA安装验证][Cuda_Install_Verify]

安装包中包含很多实用的小程序，可以用来获取当前显卡的硬件信息以及检视CUDA的运行。文件夹地址为 `C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v9.1\extras\demo_suite`。下图为`DeviceQuery`的运行结果，这个程序会打印出当前显卡的具体信息。注意一定要在命令行中运行程序要不然会闪退。

![DeviceQuery运行结果][Cuda_Device_Query]

安装包中还包含一些CUDA的实例项目存在 `C:\ProgramData\NVIDIA Corporation\CUDA Samples\v9.1` 这个文件夹中。在打开这个文件夹之前，一定要把`显示隐藏文件`这个选项勾上，不然`ProgramData`不会显示出来。每个实例项目都支持不同版本的`Visual Studio`。这个实例项目对CUDA的学习会有很大的帮助。当然，我们现在也可以在`Visual Studio` 里面新建CUDA项目了。

![新建Visual Studio CUDA 项目][CUDA_VISUAL_STUDIO_NEW]



 [CUDA_DOWNLOAD]: images/Cuda_Download.png
 [CUDA_INSTALL_VERIFY]: images/Cuda_Install_Verify.png
 [CUDA_DEVICE_QUERY]:  images/Cuda_Device_Query.png
 [CUDA_VISUAL_STUDIO_NEW]: images/Cuda_Visual_Studio_New.png
