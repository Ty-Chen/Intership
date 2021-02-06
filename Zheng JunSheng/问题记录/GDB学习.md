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

以`hello.c`源文件为例，正常情况下，使用`gcc`编译该源代码的指令如下：

```shell
zjs@ubuntu:~/Desktop/GDB_Study$ ls
hello.c
zjs@ubuntu:~/Desktop/GDB_Study$ gcc hello.c -o hello.exe
zjs@ubuntu:~/Desktop/GDB_Study$ ls
hello.c  hello.exe
```

可以看到，这里已经生成了 `hello.c` 对应的执行文件 `hello.exe`，但值得一提的是，此文件不支持使用 GDB 进行调试。原因很简单，使用 GDB 调试某个可执行文件，该文件中必须包含必要的调试信息（比如各行代码所在的行号、包含程序中所有变量名称的列表（又称为符号表）等），而上面生成的 `hello.exe` 则没有。

那如何生成符合GDB调试要求的可执行文件呢？
只需要使用 **gcc -g** 选项来编译源文件，即可生成满足GDB要求的可执行文件。

### 启动GDB调试器

在生成包含调试信息的 `hello.exe` 可执行文件的基础上，启动 GDB 调试器的指令如下：

```shell
zjs@ubuntu:~/Desktop/GDB_Study$ gdb hello.exe
GNU gdb (Ubuntu 9.2-0ubuntu1~20.04) 9.2
Copyright (C) 2020 Free Software Foundation, Inc.
...
(gdb)
```

该指令在启动GDB时，会打印出一堆免责条款。我们可以通过`--silent`选项，可以将一部分信息屏蔽掉：

```shell
zjs@ubuntu:~/Desktop/GDB_Study$ gdb hello.exe --silent
Reading symbols from hello.exe...
(No debugging symbols found in hello.exe)
(gdb) 
```

### 常用指令

#### **运行程序**

`r` 或者 `run` 执行被调试的程序，其会自动在第一个断点处暂停执行。

```shell
(gdb) run
Starting program: /home/zjs/Desktop/GDB_Study/hello.exe 
hello world
[Inferior 1 (process 16963) exited normally]
```

#### **暂停程序运行**

调试程序中，暂停程序运行是必须的，GDB可以方便地暂停程序的运行。你可以设置程序的在哪行停住，在什么条件下停住，在收到什么信号时停往等等。以便于你查看运行时的变量，以及运行时的流程。

当进程被gdb停住时，你可以使用info program 来查看程序的是否在运行，进程号，被暂停的原因。

在gdb中，我们可以有以下几种暂停方式：

- ​	断点（BreakPoint）
- ​	观察点（WatchPoint）
- ​	捕捉点（CatchPoint）
- ​	信号（Signals）
- ​	线程停止（Thread Stops）

当我们想要恢复程序时，可以使用 `continue` 或者 `c`， 继续执行被调试程序，直至下一个断点或程序结束。

```shell
(gdb) r
Starting program: /home/zjs/Desktop/GDB_Study/hello.exe 

Breakpoint 1, main () at hello.c:4
4	{
(gdb) continue
Continuing.
hello world
[Inferior 1 (process 16978) exited normally]
```

#### **设置断点**

| `break xxx`                    | 在源代码指定的某一行设置断点，其中 `xxx` 用于指定具体打断点的位置，`xxx`可以是行号或者是函数。 |
| :----------------------------- | ------------------------------------------------------------ |
| **`break +offset`**            | **在当前行的后面的offset行设置断点。**                       |
| **`break -offset`**            | **在当前行的前面的offset行设置断点。**                       |
| **`break filename::linenum`**  | **在源文件filename的linenum行设置断点。**                    |
| **`break filename::function`** | **在源文件filename的function函数入口行设置断点。**           |
| **`break`**                    | **break没有命令参数时，表示在下一行设置断点**                |
| **`break ... if <condition>`** | **`...`可以是上述的参数，`condition`表示条件，在条件成立时停住。** |

#### **查看断点**

我们可以用`info`命令来查看断点的情况。例如，`info breakpoints [n]` ，`n`表示断点编号，即`Num`。

```shell
(gdb) info breakpoints
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x0000555555555a82 in KParameterAnalysis::GetFiles(char*) at KParameterAnalysis.cpp:166
	breakpoint already hit 1 time
2       breakpoint     keep y   0x0000555555555a82 KParameterAnalysis.cpp:166
3       breakpoint     keep y   0x0000555555555aaf KParameterAnalysis.cpp:167
(gdb) info breakpoints 3
Num     Type           Disp Enb Address            What
3       breakpoint     keep y   0x0000555555555aaf KParameterAnalysis.cpp:167
```

#### **删除断点**

`delete` 删除所有断点

```shell
(gdb) i b
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x0000555555555149 in main at hello.c:4
2       breakpoint     keep y   0x000055555555515d in main at hello.c:6
(gdb) delete
Delete all breakpoints? (y or n) y
(gdb) i b
No breakpoints or watchpoints.
```

`delete breakpoints Num` 删除某一个断点

```shell
(gdb) i b
Num     Type           Disp Enb Address            What
3       breakpoint     keep y   0x0000555555555149 in main at hello.c:4
4       breakpoint     keep y   0x000055555555515d in main at hello.c:6
(gdb) delete breakpoints 3
(gdb) i b
Num     Type           Disp Enb Address            What
4       breakpoint     keep y   0x000055555555515d in main at hello.c:6
```

#### **恢复程序运行和单步调试**

当程序被停住了，你可以用`continue`命令恢复程序的运行直到程序结束，或下一个断点到来。也可以使用`step`或`next`命令单步执行程序。

```shell
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

#### **查看栈信息**

当程序被停住了，你需要做的第一件事就是查看程序是在哪里停住的。当你的程序调用了一个函数，函数的地址，函数参数，函数内的局部变量都会被压入“栈”（Stack）中。你可以用GDB命令来查看当前的栈中的信息。

| **`backtrace`** | **打印当前函数调用栈的所有信息。**                  |
| --------------- | --------------------------------------------------- |
| **`bt [n]`**    | **n是一个正整数，表示只打印栈顶上n层的栈信息。**    |
| **`bt [-n]`**   | **-n表示一个负整数，表示只打印栈底下n层的栈信息。** |

如果你要查看某一层的信息，你需要在切换当前的栈，一般来说，程序停止时，最顶层的栈就是当前栈，如果你要查看栈下面层的详细信息，首先要做的是切换当前栈。

| frame [n]      | n是一个从0开始的整数，是栈中的层编号。例如： 0表示栈顶，1表示栈的第二层。 |
| -------------- | ------------------------------------------------------------ |
| **`up [n]`**   | **表示向栈的上面移动n层。没有写 `n`，表示向上移动一层。**    |
| **`down [n]`** | **表示向栈的下面移动n层。没有写 `n`，表示向下移动一层。**    |

上面的命令，都会打印出移动到的栈层的信息。如果你不想让其打出信息，可以使用这三个命令：

- ​	`select-frame [n]` 对应于 `frame` 命令。
- ​	`up-silently [n]` 对应于 `up` 命令。
- ​	`down-silently [n]` 对应于 `down` 命令。

`frame` 或 `f` 会打印出这些信息：栈的层编号，当前的函数名，函数参数值，函数所在文件及行号，函数执行到的语句。
我们也可以使用`info`命令来查看当前栈层的信息。

| **`info frame`**  | 打印出更为详细的当前栈层的信息，大多数都是运行时的内存地址。比如：函数地址，调用函数的地址，被调用函数的地址，目前的函数是由什么样的程序语言写成的、函数参数地址及值、局部变量的地址等等 |
| ----------------- | ------------------------------------------------------------ |
| **`info args`**   | **打印出当前函数的参数名及其值。**                           |
| **`info locals`** | **打印出当前函数中所有局部变量及其值。**                     |
| **`info catch`**  | **打印出当前的函数中的异常处理信息。**                       |

#### **查看源代码**

 GDB 可以打印出所调试程序的源代码，当然，在程序编译时一定要加上`-g`的参数，把源程序信息编译到执行文件中。不然就看不到源程序了。当程序停下来以后，GDB会报告程序停在了那个文件的第几行上。你可以用`list`命令来打印程序的源代码。

| **`list [linenum]`**  | **显示程序第`linenum`行的周围的源程序。**  |
| --------------------- | ------------------------------------------ |
| **`list [function]`** | **显示函数名为`function`的函数的源程序。** |
| **`list [+]`**        | **显示当前行后面的程序。**                 |
| **`list -`**          | **显示当前行前面的程序。**                 |

一般是打印当前行的上5行和下5行，如果显示函数是是上2行下8行，默认是10行。当然也可以自定义显示行数。

| **`set listsize [count]`** | **设置一次显示源代码的行数。** |
| -------------------------- | ------------------------------ |
| **`show listsize`**        | **查看当前`listsize`的设置。** |

`list` 命令还有以下用法：

| **`list  <first>, <last>`** | **显示从`first`行到`last`行之间的源代码。** |
| --------------------------- | ------------------------------------------- |
| **`list  , <last>`**        | **显示从当前行到`last`行之间的源代码。**    |

 一般来说，`list`命令后面跟以下这些参数：

| **`linenum`**           | **行号。**                           |
| ----------------------- | ------------------------------------ |
| **`+offset`**           | **当前行号的正偏移量。**             |
| **`-offset`**           | **当前行号的负偏移量。**             |
| **`filename:linenum`**  | **`filename`文件的`linenum`行。**    |
| **`function`**          | **函数名。**                         |
| **`filename:function`** | **`filename`文件的`function`函数。** |
| **`*address`**          | **程序运行时的语句在内存中的地址。** |

#### **搜索源代码**

GDB也提供了搜索源代码的命令。

| **`search <regexp>`**         | **向后搜索。** |
| ----------------------------- | -------------- |
| **`forward-search <regexp>`** | **向后搜索。** |
| **`reverse-search <regexp>`** | **向前搜索。** |

其中， `<regexp>`是正则表达式。

#### **查看运行时数据**

在你调试程序时，当程序被停住时，你可以使用`print`命令（简写命令为p），或是同义命令`inspect`来查看当前程序的运行数据。

`print <expr>`
`print /<f> <expr>`
	`/<f>`是输出格式。`<expr>`是表达式，是你所调试的程序的语言的表达式（GDB可以调试多种编程语言）。

一般来说，GDB会根据变量的类型输出变量的值，但你也可以自定义GDB的输出的格式。GDB的数据显示格式：

| **`x`** | **按十六进制格式显示变量。**       |
| ------- | ---------------------------------- |
| **`d`** | **按十进制格式显示变量。**         |
| **`u`** | **按十六进制格式显示无符号整型。** |
| **`o`** | **按八进制格式显示变量。**         |
| **`t`** | **按二进制格式显示变量。**         |
| **`a`** | **按十六进制格式显示变量。**       |
| **`c`** | **按字符格式显示变量。**           |
| **`f`** | **按浮点数格式显示变量。**         |

在表达式中，有几种GDB所支持的操作符，它们可以用在任何一种语言中。

| **`@`**               | 是一个和数组有关的操作符。                         |
| --------------------- | -------------------------------------------------- |
| **`::`**              | **指定一个在文件或是一个函数中的变量。**           |
| **`{<type>} <addr>`** | 一个指向内存地址`<addr>`的类型为`type`的一个对象。 |

##### **变量**

在GDB中，我们可以随时查看三种变量：

1. 全局变量
2. 静态全局变量
3. 局部变量

如果你的局部变量和全局变量发生冲突（也就是重名），一般情况下是局部变量会隐藏全局变量，也就是说，如果一个全局变量和一个函数中的局部变量同名时，如果当前停止点在函数中，用print显示出的变量的值会是函数中的局部变量的值。如果此时你想查看全局变量的值时，你可以使用 `::` 操作符
       `function::variable`
可以用过这种形式指定想查看的变量，是在哪个文件中的或是哪个函数中的。

##### **数组**

有时候，你需要查看一段连续的内存空间的值。比如数组的一段，或是动态分配的数据的大小。你可以使用GDB的`@`操作符，`@`的左边是第一个内存的地址的值，`@`的右边则你你想查看内存的长度。例如:
	`p *array@len`
`@`的左边是数组的首地址的值，也就是变量`array`所指向的内容，右边则是数据的长度，其保存在变量`len`中。

如果是静态数组的话，可以直接用`print`数组名，就可以显示数组中所有数据的内容了。

#### **查看内存**

你可以使用examine命令（简写是x）来查看内存地址中的值。x命令的语法如下所示：
`x/[n/f/u] <addr>`

  `n` 是一个正整数，表示显示内存的长度，也就是说从当前地址向后显示几个地址的内容。
  `f` 表示显示的格式，参见上面。如果地址所指的是字符串，那么格式可以是`s`，如果指令地址，那么格式可以是`i`。
  `u` 表示从当前地址往后请求的字节数，如果不指定的话，GDB默认是4个bytes。u参数可以用下面的字符来代替，`b`表示单字节，`h`表示双字节，`w`表示四字节，`g`表示八字节。当我们指定了字节长度后，GDB会从指内存定的内存地址开始，读写指定字节，并把其当作一个值取出来。

`<addr>`表示一个内存地址。

`n/f/u`三个参数可以一起使用。例如：
命令：`x/3uh 0x54320` 表示，从内存地址`0x54320`读取内容，`h`表示以双字节为一个单位，`3`表示三个单位，`u`表示按十六进制显示。

**自动显示**

你可以通过`display`命令设置一些自动显示的变量，当程序停住时，或是在你单步跟踪时，这些变量会自动显示。例如：

  `display <expr>`
  `display/<fmt> <expr>`
  `display/<fmt> <addr>`

`expr`是一个表达式，`fmt`表示显示的格式，`addr`表示内存地址，当你用`display`设定好了一个或多个表达式后，只要你的程序被停下来，GDB会自动显示你所设置的这些表达式的值。

一些和display相关的GDB命令：
`undisplay <dnums...>`
`delete display <dnums...>`
删除自动显示，`dnums`意为所设置好了的自动显式的编号。如果要同时删除几个，编号可以用空格分隔，如果要删除一个范围内的编号，可以用减号表示（如：`2-5`）。

`disable display <dnums...>`
`enable display <dnums...>`
注意`disable`和`enable`不删除自动显示的设置，而是让其失效和恢复。

`info display`
 查看display设置的自动显示的信息。

#### **退出GDB**

当我们想要退出GDB调试时，可以使用`quit` 或者 `q`

```shell
(gdb) q
zjs@ubuntu:~/Desktop/GDB_Study$ 
```

## 4.GDB的小技巧

### 1.调用终端命令

通过shell 去调用我们终端命令

```shell
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

```shell
(gdb) set logging on
Copying output to gdb.txt.
Copying debug output to gdb.txt.
```

之后进行一系列调试操作，例如`list`源码，打断点，`run`等。
退出GDB，就会发现多了一个gdb.txt

```shell
(gdb) q
zjs@ubuntu:~/Desktop/GDB_Study$ ls
a.out  GDB_Test.c  gdb.txt  hello  hello.exe
zjs@ubuntu:~/Desktop/GDB_Study$ cat gdb.txt
```

通过`cat`查看gdb.txt, 我们就可以查看我们进行了哪些调试操作。

## 参考资料

https://www.gnu.org/software/gdb/