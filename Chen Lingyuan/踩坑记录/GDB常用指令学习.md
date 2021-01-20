#                                                 GDB常用指令学习

GDB在Linux下是非常优秀的代码调试工具，它的用法很多，在这里记录一下。

## 创建GDB

g++ -g main.cpp -o main

## 启动GDB

一般来说只要在GDB后面输入待执行程序的名字即可启动GDB

例: gdb main.out

## List 指令

List指令能够列出源代码，方便随时随地对源码的查询，指令简写为 l

1.(gdb) list line1  列出行line1之后的源码

2.(gdb)list line1,line2  列出l行ine1,line2之间的源码

3.(gdb)list func 列出函数func的源码

## Run指令

Run指令能够运行准备调试的程序，在它后面可以跟随发给该程序的各种参数,简写为r

1.（gdb）run 后面不加参数

2.(gdb) run argv[1]  argv[2] ......   后面跟随命令行参数

## Printf指令

Printf可以显示各个变量的值，简写为p

1.（gdb）printf (变量名)  查看变量在当前的值

2.(gdb)    print array@len 查看数组之后len个元素 

例: p array[0]@5 

## Breakpoint断点

Break命令可以用来在调试程序过程中设置断点,简写为b

1.(gdb) break line - number 使程序刚好在给定行之前停止

2.(gdb) break function  使程序刚好在指定的函数之前停止

3.(gdb) break line-or-function if condtion 如果条件是真，程序到达指定行或函数时停止

例如： b 78 if argc==3

4.(gdb) info break 查看所有断点

5.（gdb）delete 删除所有断点

6.（gdb）delete breakpoint num 删除第num号的断点 

例：delete breakpoint 1 删除第一个断点

## 单步执行

next 不进入的单步执行

step 进入的单步执行如果已经进入了某函数，而是想退出该函数返回到它的调用函数中，可使用命令finish

## 输出格式

一般来说，GDB会根据变量的类型输出变量的值，但是也可以自定义GDB的输出的格式。以下是GDB的数据显示格式：

x 按十六进制格式显示变量。
d 按十进制格式显示变量。
u 按十六进制格式显示无符号整型。
o 按八进制格式显示变量。
t 按二进制格式显示变量。
a 按十六进制格式显示变量。
c 按字符格式显示变量。
f 按浮点数格式显示变量。

1.(gdb) p i
$21 = 101

2.(gdb) p/a i 以16进制格式输出变量
$22 = 0x65

内存查看

可以使用examine命令（简写是x）来查看内存地址中的值，x命令的语法如下所示

x/

n、f、u是可选的参数。

n 是一个正整数，表示显示内存的长度，也就是说从当前地址向后显示几个地址的内容。
f 表示显示的格式，参见上面。如果地址所指的是字符串，那么格式可以是s，如果地十是指令地址，那么格式可以是i。
u 表示从当前地址往后请求的字节数，如果不指定的话，GDB默认是4个bytes。
u参数可以用下面的字符来代替，b表示单字节，h表示双字节，w表示四字节，g表示八字节。
当我们指定了字节长度后，GDB会从指内存定的内存地址开始，读写指定字节，并把其当作一个值取出来。

n/f/u三个参数可以一起使用,例如：

命令:x/3cb array[0] 表示从array[0]地址处读取内容，b表示以单字节作为一个单位，3表示往后读三个，c表示按字符格式显示。

## 其他

打印当前函数调用栈的所有信息

(gdb)bt

(gdb) disassemble func 查看函数func的汇编码

