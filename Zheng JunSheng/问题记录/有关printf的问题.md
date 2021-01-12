# `printf()`函数学习

## 一.简介

​		在将结果输出到文件中修改成输出到控制台(`fwrite`→`printf`)，速度上慢了很多。在使用`printf`之后使用重定向输入到文件(> file),速度也慢。

## 二.问题分析

​		1.`printf()`如何实现的？

​		2.为什么`printf()`函数慢？

​		3.有什么办法加快输出？

​		解决思路：通过查询`MSDN`，源码，stack overflow。

## 三.问题解决

```
int printf(const char *fmt, ...)
{
	char printf_buf[1024];
	va_list args;
	int printed;
	va_start(args, fmt);
	printed = vsprintf(printf_buf, fmt, args);
	va_end(args);

	puts(printf_buf);

	return printed;
}
```

`printf`是行缓冲函数，先写到缓冲区，满足条件后，才刷新缓冲区(将输出从缓冲区发送到屏幕或文件称为刷新缓冲区).

`printf`刷新缓冲区条件：

​	1.缓冲区写满(缓冲区大小为1024`byte`).

​	2.`fflush`手动刷新缓冲区

​	3.执行`printf`的进程或者线程结束时会主动调用`fflush`刷新缓冲区

​	4.遇到换行符\n \r(不一定正确，输出设备是非交互式(磁盘，管道等)， 则输出不一定是行缓冲).

​	5.当有即将到来的输入(不一定正确，取决于输入输出是否交互).

## 四.总结

​	在输出时，最好在每次将数据输出到文件时清空缓冲区，否则这样即使程序异常中断数据也不会丢失。







