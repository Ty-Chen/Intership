# Wiindows与Linux下的命令合计（不定期更新）

最近在书写代码的时候，用到了很多关于DOS以及Linux下的命令，但很多都是用过了就忘，所以在这边记录一下。



## Windows/

help -帮助命令，查看相关指令说明,如果想查询有关具体命令，在命令后面/?

例如:dir/?

### dir 查找目录下的相关文件

dir /? dir相关命令与参数

dir Path 当前目录下的文件及详细信息

dir Path  /s  /b  输出文件目录下所有文件及文件夹的信息（如果想显示信息，去掉/b）

dir Path\\*.??? /s /b 输出文件目录下所有指定后缀的文件信息。

dir Path  /A??? 输出所有符合要求属性的文件。

注意：dir Path\\*.???  Path不可以是文件，否则无法搜索。



### Dos 命令混合

‘|’符号 ，将第一条指令的结果作为参数给第二条指令执行

例如

dir E:\Log /s /b | findstr log 查询Log下的所有文件，并输出名字中带有log的文件

'&'符号，并列命令，前一条命令的失败不影响后面的命令

例如

Not Command & help

前面是不存在的命令，但并不影响后面help的执行。



'&&'符号，同为并列命令，但前一条命令执行错误之后立即结束

例如

Not Command && help

执行失败之后立即终止。



‘||’符号，同样也是并别命令，与‘&&'符号完全相反，在执行完一条正确命令之后立即结束

例如

not || not || not || help || dir E:\Log

前三个是错误命令，后两个是正确命令，但在执行完help之后就结束了，没有执行最后一条命令。

### For 命令

关于Dos下的for命令非常复杂

先更新一种

for / r 

基本格式如下：

FOR /R [[drive:]path] %variable IN (set) DO command [command-parameters]

 %variable  指定一个单一字母可替换的参数。
  (set)      指定一个或一组文件。可以使用通配符。
  command    指定对每个文件执行的命令。
  command-parameters 为特定命令指定参数或命令行开关

%i 在 for 语句中显式声明，%j 和 %k 是通过tokens= 选项隐式声明的。

例如：for /r  E:\Log  %i in (*.log) do @echo %i

## Linux/

### help 

-帮助命令，查看相关指令说明

如果想查询某个具体命令，在命令后面加 --help

例如：find --help

### find命令：

**find命令用于查找指定目录下的文件，同时也可以调用其它命令执行相应的操作**

-name 按照文件名查找文件。
-perm 按照文件权限来查找文件。
-prune 使用这一选项可以使find命令不在当前指定的目录中查找，如果同时使用-depth选项，那么-prune将被find命令忽略。
-user 按照文件属主来查找文件。
-group 按照文件所属的组来查找文件。
-mtime -n +n 按照文件的更改时间来查找文件， - n表示文件更改时间距现在n天以内，+ n表示文件更改时间距现在n天以前。find命令还有-atime和-ctime 选项，但它们都和-m time选项。
-nogroup 查找无有效所属组的文件，即该文件所属的组在/etc/groups中不存在。
-nouser 查找无有效属主的文件，即该文件的属主在/etc/passwd中不存在。
-newer file1 ! file2 查找更改时间比文件file1新但比文件file2旧的文件。
-type 查找某一类型的文件，诸如：
b - 块设备文件。
d - 目录。
c - 字符设备文件。
p - 管道文件。
l - 符号链接文件。
f - 普通文件。
-size n：[c] 查找文件长度为n块的文件，带有c时表示文件长度以字节计。-depth：在查找文件时，首先查找当前目录中的文件，然后再在其子目录中查找。
-fstype：查找位于某一类型文件系统中的文件，这些文件系统类型通常可以在配置文件/etc/fstab中找到，该配置文件中包含了本系统中有关文件系统的信息。
-mount：在查找文件时不跨越文件系统mount点。
-follow：如果find命令遇到符号链接文件，就跟踪至链接所指向的文件。
-cpio：对匹配的文件使用cpio命令，将这些文件备份到磁带设备中。

另外,下面三个的区别:

-amin n 查找系统中最后N分钟访问的文件
-atime n 查找系统中最后n24小时访问的文件
-cmin n 查找系统中最后N分钟被改变文件状态的文件
-ctime n 查找系统中最后n24小时被改变文件状态的文件
-mmin n 查找系统中最后N分钟被改变文件数据的文件
-mtime n 查找系统中最后n*24小时被改变文件数据的文件

例如：find Log -type f -name "*.log" 查找 Log 文件夹下后缀为log的文件

### Linux下创建C/C++

vim main.c/main.cpp,编写完成按下esc，并输入:wq

Linux下编译C/C++

gcc/g++ -o xxx  xxx.cpp/c

如果用到C++11的库，后面加 -std=c++11

如果要使用多线程，后面加 -lpthread

### Time指令

Linux下的Time指令具有强大的功能，不仅能计算进程运行时间，还能计算进程的各种属性，例如CPU利用率，进程平均内存使用量，最高内存使用量等。

 最基本的使用是 time + 进程

输出为real : xmx.xxxs

​           user:xmx.xxxs

​           sys:xm.xxxs

real 时间是挂钟时间，也就是命令开始执行到结束的时间，这个时间包括进程执行时间以及被阻塞的时间，一般来说在单线程程序下，real>=user+sys。

user时间是指进程花费在用户模式中的CPU时间

sys时间是指进程花费在内核模式中的CPU时间

shell内建也有一个time命令，当运行time时候是调用的系统内建命令，应为系统内建的功能有限，所以需要时间其他功能需要使用time命令可执行二进制文件`/usr/bin/time`。

-f, --format=FORMAT 

使用指定格式输出。如果没有指定输出格式，采用环境变量 TIME 指定的格式

 -p, --portability 

使用兼容 POSIX 的格式输出，real %e\nuser %U\nsys %S 

-o, --output=FILE 

结果输出到指定文件。如果文件已经存在，覆写其内容 

-a, --append 

与 -o 选项一起使用，使用追加模式将输出写入指定文件 

-v, --verbose 

使用冗余模式尽可能地输出统计信息

 --help 

显示帮助信息 

-V, --version

 显示版本信息 

--

 终止选项列表

使用 -o 选项将执行时间写入到文件中

例如:/usr/bin/time -o out.txt  ls Log

使用 -a选项追加信息:

例如：/usr/bin/time -a -o out.txt ls 将time信息追加输出到out.txt末尾。

使用 -f选项格式化时间输出

例如：/usr/bin/time -f "time: %U" ls Log

-f选项后的参数：

Time
%E：执行指令所花费的时间，格式[hours:]minutes:seconds
%e：执行指令所花费的时间，单位是秒
%S：指令执行时在内核模式（kernel mode）所花费的时间，单位是秒
%U：指令执行时在用户模式（user mode）所花费的时间，单位是秒
%P：执行指令时 CPU 的占用比例。其实这个数字就是内核模式加上用户模式的 CPU 时间除以总时间（(%S+%U)/%E）

Memory
%M：执行时所占用的内存的最大值。单位KB
%t：执行时所占用的内存的平均值，单位是 KB
%K：执行程序所占用的内存总量（stack+data+text）的平均大小，单位是 KB
%D：执行程序的自有数据区（unshared data area）的平均大小，单位是 KB
%p：执行程序的自有栈（unshared stack）的平均大小，单位是 KB
%X：执行程序是共享代码段（shared text）的平均值，单位是 KB
%Z：系统内存页的大小，单位是 byte。对同一个系统来说这是个常数
%F：内存页错误次数。内存页错误指需要从磁盘读取数据到内存
%R：次要或可恢复的页面错误数。这些是无效页面的错误，但其他虚拟页面尚未使用该内存页。因此，页面中的数据仍然有效，但必须更新系统表
%W：进程从内存中交换的次数
%c：进程上下文被切换的次数（因为时间片已过期）
%w：进程等待次数，指程序主动进行上下文切换的次数，例如等待I/O操作完成

I/O
%I：此程序所输入的档案数
%O：此程序所输出的档案数
%r：此程序所收到的 Socket Message
%s：此程序所送出的 Socket Message
%k：此程序所收到的信号 （Signal）数量

Command Info
%C：执行时的参数指令名称
%x：指令的结束代码 ( Exit Status )

