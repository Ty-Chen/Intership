# S_ISDIR和S_ISREG的使用确认

## 1.简介

在Linux系统获取目录及子目录下所有文件时，先使用了`struct stat`来判断是不是目录。

## 2.详细分析

struct stat的成员变量：

```c
struct stat
{
	__mode_t st_mode;          /* File mode.  */
	__ino_t st_ino;            /* File serial number.  */
	__dev_t st_dev;            /* Device containing the file. */
	__nlink_t st_nlink;        /* Link count.*/
	__uid_t st_uid;            /* User ID of the file's owner. */
	__gid_t st_gid;            /* Group ID of the file's group.*/
	__off_t st_size;           /* Size of file, in bytes. */
	__off64_t st_size;         /* Size of file, in bytes. */
	__time_t st_atime;         /* Time of last access.  */
	__time_t st_mtime;         /* Time of last modification. */
	__time_t st_ctime;         /* Time of last status change.  */
};
```

st_mode

```c
/* Encoding of the file mode.  These are the standard Unix values,
   but POSIX.1 does not specify what values should be used.  */

#define __S_IFMT        0170000 /* These bits determine file type.  */

/* File types.  */
#define __S_IFDIR       0040000 /* Directory.  */
#define __S_IFCHR       0020000 /* Character device.  */
#define __S_IFBLK       0060000 /* Block device.  */
#define __S_IFREG       0100000 /* Regular file.  */
#define __S_IFIFO       0010000 /* FIFO.  */
```

S_ISDIR、S_ISREG等定义

```c
#define __S_ISTYPE(mode, mask)  (((mode) & __S_IFMT) == (mask))
 
#define S_ISDIR(mode)    __S_ISTYPE((mode), __S_IFDIR)
#define S_ISCHR(mode)    __S_ISTYPE((mode), __S_IFCHR)
#define S_ISBLK(mode)    __S_ISTYPE((mode), __S_IFBLK)
#define S_ISREG(mode)    __S_ISTYPE((mode), __S_IFREG)
```

简单使用：

```c
char*       pszFilePath;
struct stat statBuff;
if(stat(pszFilePath, &statBuff) != -1)
{
	if (S_ISREG(statBuff.st_mode) != 0)
    
    	printf( "%s is a file", file_path ) ;
    }
    else
    {
        printf( "%s is not a file", file_path ) ;
    }
}
```

## 3.参考资料

1. https://stackoverflow.com/questions/40163270/what-is-s-isreg-and-what-does-it-do
2. [sourceware.org/git/?p=glibc.git;a=blob;f=io/sys/…](https://sourceware.org/git/?p=glibc.git;a=blob;f=io/sys/stat.h;h=762c8538baad367c5e1917d98173166d2d8ed251;hb=HEAD#l128) 
3. [sourceware.org/git/?p=glibc.git;a=blob;f=bits/…](https://sourceware.org/git/?p=glibc.git;a=blob;f=bits/stat.h;h=0330f9ad27b56daccc052e0d37991dea471b71f0;hb=HEAD#l69) 

## 4.总结

使用S_ISREG等是先调用该`stat`函数以`struct stat`用有关文件的信息填充对象。然后`st_mode`，可以使用此宏测试此结构的成员的值（整数类型）。