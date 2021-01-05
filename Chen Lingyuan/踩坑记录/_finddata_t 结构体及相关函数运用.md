# _finddata_t 结构体及相关函数运用

## 简介

前几天在代码里面加入了搜索文件夹下面的文件的功能，相关的函数没来得及看，在这里做个记录：

## 问题分析

_finddata结构体，_findfirst函数,_findnext函数及_findclose函数全部定义在#include<io.h>头文件里。

_finddata结构体专门用来储存文件的各种信息，具体的参数有

![img](file:///C:\Users\admin\AppData\Local\Temp\ksohtml\wps_clip_image-13849.png)

其中arrrib表示文件的属性，这些属性包含：

![img](file:///C:\Users\admin\AppData\Local\Temp\ksohtml\wps_clip_image-13728.png)



NORMAL表示正常，READONLY表示只读，HIDDEN表示因此，SYSTEM表示系统文件，SUBDIR表示文件夹，ARCH表示存档文件

Time_create表示文件的创建时间

Time_access表示文件最后一次被访问的时间

Time_write表示文件最后一次被修改的时间

size表示文件的大小

Name代表文件的文件名

下面是关于findfirst函数的参数与定义：

![img](file:///C:\Users\admin\AppData\Local\Temp\ksohtml\wps_clip_image-12377.png)

该函数的作用是查找与filespec相符合的文件，并更改fileinfo的信息。

第一个参数是标明文件的字符串，可以包含通配符，第二个参数在这里就是finddata的结构体指针。如果执行成功，fileinfo的相关数据也会发生改变并返回唯一的搜索句柄以标识与filespec规范匹配的文件或文件组，可在随后的findnext与findclose函数中调用该句柄，否则返回-1.

findnext函数与findfirst函数类似的作用是查找对上次findfirst调用中与filespec相符合的文件，然后更改fileinfo中的信息。

它的参数与findfirst有点不同：

![img](file:///C:\Users\admin\AppData\Local\Temp\ksohtml\wps_clip_image-5812.png)

第一个参数是findfirst返回的句柄。如果执行成功返回0，否则返回-1.

 注意在findfirst函数与findnext函数执行完之后，fileinfo.name可能会变成"."与".."，这分别表示当前目录与上级目录，如果不需要可以舍弃。

 Findclose是一个资源释放函数，它的作用是关闭指定的搜索句柄，并释放相关资源，它的参数只有一个：就是findfirst函数返回的句柄，如果执行成功就返回0，否则返回-1.

## 问题总结

关于finddata的相关简介就写到这，如果有更加深入的问题会重新开篇文章继续写