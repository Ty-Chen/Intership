# `_finddata_t`结构体学习

## 1.简介

​	在获取文件类型和文件名时，我们可以使用`_finddata_t`来存文件信息。

## 2.结构体分析

```c
#define _finddata_t     _finddata64i32_t

typedef __int64  __time64_t;
typedef unsigned long _fsize_t;

struct _finddata64i32_t
{
    unsigned    attrib;
    __time64_t  time_create;    // -1 for FAT file systems
    __time64_t  time_access;    // -1 for FAT file systems
    __time64_t  time_write;
    _fsize_t    size;
    char        name[260];
};
```

下面介绍一下成员变量

| `attrib`        | 所查询的文件属性           |
| --------------- | -------------------------- |
| **time_create** | **创建文件的时间**         |
| **time_access** | **最后一次访问文件的时间** |
| **time_write**  | **文件最后被修改的时间**   |
| **size**        | **文件大小**               |
| **name**        | **文件名**                 |

`attrib` 是所查询的文件属性（`_A_NORMAL`，`_A_RDONLY`，`_A_HIDDEN`，`_A_SYSTEM`，_`A_SUBDIR`，`_A_ARCH`）。

```c
#define _A_NORMAL 0x00 // Normal file - No read/write restrictions
#define _A_RDONLY 0x01 // Read only file
#define _A_HIDDEN 0x02 // Hidden file
#define _A_SYSTEM 0x04 // System file
#define _A_SUBDIR 0x10 // Subdirectory
#define _A_ARCH   0x20 // Archive file
```

文件可能会有多种属性，那么`attrib`会按照位或的方式来获得文件的综合属性。例如：只读+隐藏+系统属性，应该为：`_A_HIDDEN | _A_RDONLY | _A_SYSTEM` 。所以在判断时，要仔细考虑使用`&`还是`==`。

## 3.相关函数（`_findfirst` ， `_findnext`， `_findclose`）

### 3.1`_findfirst` 

```c
intptr_t _findfirst(
	const char *filespec,
	struct _finddata_t *fileinfo
);
```

第一个参数为文件名，第二个参数是`_finddata_t`结构体指针。

作用：查找与`filespec`符合的文件，并更改`fileinfo`的信息。

返回值：若查找成功，返回文件句柄，若失败，返回-1。

### 3.2`_findnext`

```c
int _findnext(
   intptr_t handle,
   struct _finddata_t *fileinfo 
);
```

第一个参数为文件句柄，第二个参数是`_finddata_t`结构体指针。

作用：查找对`findfirst`调用中与`filespec`符合的文件，然后更改`fileinfo`中的信息。

返回值：若查找成功，返回0，若失败，返回-1。

### 3.3`_findclose`

```c
int _findclose( 
   intptr_t handle 
);
```

参数为`_findfirst`查找成功后返回的文件句柄。

作用：关闭文件句柄，释放资源。

返回值：若关闭成功，返回0，若失败，返回-1。

## 4.总结

**在使用`_findfirst`或`_findnext`函数(或任何变体)完成之后，必须调用`_findclose`，释放应用程序中这些函数所使用的资源。**

关于`_finddata_t`结构体及其相关函数就写到这，如果有更加深入的问题会重新开篇文章继续写。