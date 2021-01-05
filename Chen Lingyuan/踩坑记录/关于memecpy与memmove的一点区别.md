# 关于memecpy与memmove的一点区别

## 简介

  最近在代码里面使用了memmove的函数，但是没怎么挖掘其特性，一直觉得它与memcpy有点区别。今天查阅了资料及源码，发现了其中的区别。

## 问题分析

  首先是关于memcpy的MSDN上的定义及其源码：

![img](file:///C:\Users\admin\AppData\Local\Temp\ksohtml\wps_clip_image-10410.png)



![img](file:///C:\Users\admin\AppData\Local\Temp\ksohtml\wps_clip_image-616.png)



可以看到memecpy如果两块内存没有重叠，这样处理没有问题，但是两块内存一旦出现重叠的情况，这样的处理会有问题。复制的数据会出现异常。

 再看memmove的MSDN上的描述及源码

![img](file:///C:\Users\admin\AppData\Local\Temp\ksohtml\wps_clip_image-10236.png)

 ![img](file:///C:\Users\admin\AppData\Local\Temp\ksohtml\wps_clip_image-29574.png)

可以看到memmove多了判断，处理了内存重叠情况下的复制问题，但是因为多了判断，所以在处理较多内存块复制的时候效率不如memcpy，所以要根据情况使用这两个函数.

## 问题解决方案

最后是本人对于这两个函数的测试代码：

![img](file:///C:\Users\admin\AppData\Local\Temp\ksohtml\wps_clip_image-7681.png)

按照源码描述这样写复制内存时数据会出问题，但是实际情况是两次输出结果相同，翻阅 memcpy的MSDN有这样描述：

![img](file:///C:\Users\admin\AppData\Local\Temp\ksohtml\wps_clip_image-29046.png)

可能是制约函数对特殊情况做了处理才会使得两者结果相同。

## 问题总结

虽然测试结果相同，但是为了保险起见，以后的工作中如果可能会出现内存重叠的现象还是使用memmove函数，否则就使用memecpy,关于这个制约函数，以后在继续深究了。



