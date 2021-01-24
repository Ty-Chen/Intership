# GDB学习

## 1.GDB是什么东西？

GDB调试器 可以运行你在程序运行的时候检查里面到底发生了什么？

GDB  可以做以下四件事情

- Start your program, specifying anything that might affect its behavior. 
- Make your program stop on specified conditions.
- Examine what has happened, when your program has stopped. 
- Change things in your program, so you can experiment with correcting the effects of one bug and go on to learn about another.

## 2.GDB支持的语言

- Ada
- Assembly
- C
- C++
- D
- Fortran
- Go
- Objective-C
- OpenCL
- Modula-2
- Pascal
- Rust

## 3.GDB的quickstart

### 前期准备

以hello.c源文件为例，正常情况下，使用gcc编译该源代码的指令如下：

```
zjs@ubuntu:~/Desktop/GDB_Study$ ls
hello.c
zjs@ubuntu:~/Desktop/GDB_Study$ gcc hello.c -o hello.exe
zjs@ubuntu:~/Desktop/GDB_Study$ ls
hello.c  hello.exe
```

可以看到，这里已经生成了 hello.c 对应的执行文件 hello.exe，但值得一提的是，此文件不支持使用 GDB 进行调试。原因很简单，使用 GDB 调试某个可执行文件，该文件中必须包含必要的调试信息（比如各行代码所在的行号、包含程序中所有变量名称的列表（又称为符号表）等），而上面生成的 hello.exe 则没有。

那如何生成符合GDB调试要求的可执行文件呢？
只需要使用 **gcc -g** 选项来编译源文件，即可生成满足GDB要求的可执行文件。

### 启动GDB调试器

在生成包含调试信息的 hello.exe 可执行文件的基础上，启动 GDB 调试器的指令如下：

```
zjs@ubuntu:~/Desktop/GDB_Study$ gdb hello.exe
GNU gdb (Ubuntu 9.2-0ubuntu1~20.04) 9.2
Copyright (C) 2020 Free Software Foundation, Inc.
...
(gdb)
```

该指令在启动GDB时，会打印出一堆免责条款。我们可以通过--silent选项，可以将一部分信息屏蔽掉：

```
zjs@ubuntu:~/Desktop/GDB_Study$ gdb hello.exe --silent
Reading symbols from hello.exe...
(No debugging symbols found in hello.exe)
(gdb) 
```

### 常用指令

#### **运行程序**

r 或者 run 执行被调试的程序，其会自动在第一个断点处暂停执行。

```
(gdb) run
Starting program: /home/zjs/Desktop/GDB_Study/hello.exe 
hello world
[Inferior 1 (process 16963) exited normally]
```

#### **设置断点**

break xxx 或者 b xxx 在源代码指定的某一行设置断点，其中 xxx 用于指定具体打断点的位置。

```
(gdb) b main
Breakpoint 1 at 0x555555555149: file hello.c, line 4.
```

#### **查看断点**

我们可以用info b来查看断点的情况。

```
(gdb) info b
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x0000555555555149 in main at hello.c:4
```

#### **删除断点**

delete 删除所有断点

```
(gdb) i b
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x0000555555555149 in main at hello.c:4
2       breakpoint     keep y   0x000055555555515d in main at hello.c:6
(gdb) delete
Delete all breakpoints? (y or n) y
(gdb) i b
No breakpoints or watchpoints.
```

delete breakpoints Num 删除某一个断点

```
(gdb) i b
Num     Type           Disp Enb Address            What
3       breakpoint     keep y   0x0000555555555149 in main at hello.c:4
4       breakpoint     keep y   0x000055555555515d in main at hello.c:6
(gdb) delete breakpoints 3
(gdb) i b
Num     Type           Disp Enb Address            What
4       breakpoint     keep y   0x000055555555515d in main at hello.c:6
```

#### **遇到断点后继续执行**

当我们需要到断点时，想要继续执行时，可以使用 continue 或者 c， 继续执行被调试程序，直至下一个断点或程序结束。

```
(gdb) r
Starting program: /home/zjs/Desktop/GDB_Study/hello.exe 

Breakpoint 1, main () at hello.c:4
4	{
(gdb) continue
Continuing.
hello world
[Inferior 1 (process 16978) exited normally]
```

或者使用next 或者 step 进行单步执行。

```
(gdb) r
Starting program: /home/zjs/Desktop/GDB_Study/hello.exe 

Breakpoint 5, main () at hello.c:4
4	{
(gdb) n
5	    printf("hello world\n");
(gdb) n
hello world
6	    return 0;
(gdb) 
7	}
```

next和step区别就像是vs调试里面的F10和F11，next是逐过程的，step是逐语句的，会进入函数，
如果进入了想要退出，可以使finish。

#### **查看代码**

我们可以使用list 或者 l 来查看代码。

```
(gdb) l
1	#include <stdio.h>
2	
3	int main()
4	{
5	    printf("hello world\n");
6	    return 0;
7	}
```

#### **查看变量**

我们可以通过printf 或者 p 来查看变量。

```
(gdb) l
1	#include <stdio.h>
2	
3	int main()
4	{
5	    int arr[4] = {1, 2, 3, 4};
6	    int i = 0;
7	    for (int i = 0; i < 4; ++i)
8	    {
9	    	printf("%d\n",arr[i]);
10	    }
(gdb) b 6
Breakpoint 1 at 0x11a0: file GDB_Test.c, line 6.
(gdb) r
Starting program: /home/zjs/Desktop/GDB_Study/a.out 

Breakpoint 1, main () at GDB_Test.c:6
6	    int i = 0;
(gdb) p arr[1]
$1 = 2
(gdb) p arr[2]
$2 = 3
(gdb) p &arr[1]
$3 = (int *) 0x7fffffffdf44
(gdb) p &arr[2]
$4 = (int *) 0x7fffffffdf48
(gdb) 
```

#### **退出GDB**

当我们想要退出GDB调试时，可以使用quit 或者 q

```
(gdb) q
zjs@ubuntu:~/Desktop/GDB_Study$ 
```

## 4.GDB的小技巧

### 1.调用终端命令

通过shell 去调用我们终端命令

```
(gdb) shell ls
a.out  GDB_Test.c  hello  hello.exe
(gdb) shell cat GDB_Test.c
#include <stdio.h>

int main()
{
    int arr[4] = {1, 2, 3, 4};
    int i = 0;
    for (int i = 0; i < 4; ++i)
    {
    	printf("%d\n",arr[i]);
    }
    return 0;
}
```

### 2.日志功能

启动日志功能

```
(gdb) set logging on
Copying output to gdb.txt.
Copying debug output to gdb.txt.
```

之后进行一系列调试操作，例如list源码，打断点，run等。
退出GDB，就会发现多了一个gdb.txt

```
(gdb) q
zjs@ubuntu:~/Desktop/GDB_Study$ ls
a.out  GDB_Test.c  gdb.txt  hello  hello.exe
zjs@ubuntu:~/Desktop/GDB_Study$ cat gdb.txt
```

通过cat查看gdb.txt, 我们就可以查看我们进行了哪些调试操作。

## 参考资料

https://www.gnu.org/software/gdb/