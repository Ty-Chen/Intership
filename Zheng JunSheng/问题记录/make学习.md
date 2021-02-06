# `make`学习

## 一、简介

`make`可以自动确定大型程序的哪些部分需要重新编译，并发出重新编译它们的命令。

要准备使用`make`，必须要编写一个名为Makefile的文件，该文件描述程序中文件之间的关系，并说明更新每个文件的命令。

## 二、`make`工作步骤

1. 读入所有的Makefile。
2. 读入被`include`的其它Makefile。
3. 初始化文件中的变量。
4. 推导隐晦规则，并分析所有规则。
5. 为所有的目标文件创建依赖关系链。
6. 根据依赖关系，决定哪些目标要重新生成。
7. 执行生成命令。

1-5步为第一个阶段，6-7为第二个阶段。第一个阶段中，如果定义的变量被使用了，那么，`make`会把其展开在使用的位置。但`make`并不会完全马上展开，`make`使用的是拖延战术，如果变量出现在依赖关系的规则中，那么仅当这条依赖被决定要使用了，变量才会在其内部展开。

## 三、`make`常用参数

`-f` 指定makefile的文件的名称。如果不用该选项，那么`make`程序首先在当前目录查找名为makefile的文件，如果没有找到，它就会转而查找名为Makefile的文件。如果在Linux下使用`GNU Make`的话，它会首先查找GNUmakefile，之后再搜索makefile和Makefile。推荐使用Makefile，因为它明显地出现在目录列表的开头，就在其他重要文件(如README)的旁边。

`-k` 如果使用该选项，即使make程序遇到错误也会继续向下运行，如果生成一个目标失败了，那么依赖于其上的目标就不会被执行了。如果没有该选项，在遇到第一个错误时`make`程序马上就会停止，那么后面的错误情况就不得而知了。我们可以利用这个选项来查出所有有编译问题的源文件。

`-n` 打印要执行的命令，但不执行它们。

`-q` 不运行命令，也不输出。仅仅是检查所指定的目标是否需要更新。如果是0则说明要更新，如果是2则说明有错误发生。

## 四、`make`的退出码

`make`命令执行后有三个退出码：

| 0     | **表示成功执行**                                             |
| ----- | ------------------------------------------------------------ |
| **1** | **如果`make`运行时出现任何错误，其返回1**                    |
| **2** | **如果你使用了`make`的`-q`选项，并且`make`使得一些目标不需要更新，那么返回2** |

## 五、检查makefile规则

有时候，我们不想让我们的makefile中的规则执行起来，我们只想检查一下我们的命令，或是执行的序列。于是我们可以使用make命令的下述参数：

`-n`  打印要执行的命令，但不执行它们。

`-t`  这个参数的意思就是把目标文件的时间更新，但不更改目标文件。也就是说，make假装编译目标，但不是真正的编译目标，只是把目标变成已编译过的状态。

`-q`  检查目标存不存在。如果目标存在，那什么也不会输出或者编译；如果目标不存在，会打印一条出错信息。

`-w`  这个参数需要指定一个文件。一般是是源文件（或依赖文件），Make会根据规则推导来运行依赖于这个文件的命令，一般来说，可以和“-n”参数一同使用，来查看这个依赖文件所发生的规则命令。

## 六、简单尝试`make`

1.创建一个`hello.c`。

```shell
zjs@ubuntu:~/Desktop/hello$ touch hello.c
zjs@ubuntu:~/Desktop/hello$ vim hello.c

#include <stdio.h>

int main(int argc, char *argv[])
{       
        printf("hello GUN!");
        return 0;
} 
```

2.用 `autoscan` 产生一个 `configure.in` 的雏型，执行 `autoscan` 后会产生一个`configure.scan` 的档案，我们可以用它做为`configure.in`档的蓝本。

```shell
zjs@ubuntu:~/Desktop/hello$ autoscan
zjs@ubuntu:~/Desktop/hello$ 
zjs@ubuntu:~/Desktop/hello$ ls
autoscan.log  configure.scan  hello.c
```

3.把它的改成`configure.in`，并且编辑 `configure.in` 文档。

```shell
zjs@ubuntu:~/Desktop/hello$ mv configure.scan  configure.in
zjs@ubuntu:~/Desktop/hello$ 
zjs@ubuntu:~/Desktop/hello$ ls
autoscan.log  configure.in  hello.c
zjs@ubuntu:~/Desktop/hello$ 
zjs@ubuntu:~/Desktop/hello$ vim configure.in 

#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
AM_INIT_AUTOMAKE				
AC_CONFIG_SRCDIR([hello.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

```

4.执行`aclocal` 和 `autoheader`，分别会产生 `aclocal.m4` 和 `configure.h.in` 

```shell
zjs@ubuntu:~/Desktop/hello$ aclocal
aclocal: warning: autoconf input should be named 'configure.ac', not 'configure.in'
zjs@ubuntu:~/Desktop/hello$ 
zjs@ubuntu:~/Desktop/hello$ ls
aclocal.m4  autom4te.cache  autoscan.log  configure.in  hello.c
zjs@ubuntu:~/Desktop/hello$ 
zjs@ubuntu:~/Desktop/hello$ rm -rf autom4te.cache/ aclocal.m4 
zjs@ubuntu:~/Desktop/hello$ 
zjs@ubuntu:~/Desktop/hello$ mv configure.in  configure.ac
zjs@ubuntu:~/Desktop/hello$ 
zjs@ubuntu:~/Desktop/hello$ aclocal
zjs@ubuntu:~/Desktop/hello$ 
zjs@ubuntu:~/Desktop/hello$ ls
aclocal.m4  autom4te.cache  autoscan.log  configure.ac  hello.c
zjs@ubuntu:~/Desktop/hello$ 
zjs@ubuntu:~/Desktop/hello$ autoheader 
zjs@ubuntu:~/Desktop/hello$ 
zjs@ubuntu:~/Desktop/hello$ ls
aclocal.m4  autom4te.cache  autoscan.log  config.h.in  configure.ac  hello.c
```

 5.编辑`Makefile.am`

```shell
zjs@ubuntu:~/Desktop/hello$ touch Makefile.am
zjs@ubuntu:~/Desktop/hello$ 
zjs@ubuntu:~/Desktop/hello$ ls
aclocal.m4  autom4te.cache  autoscan.log  config.h.in  configure.ac  hello.c  Makefile.am
zjs@ubuntu:~/Desktop/hello$ 
zjs@ubuntu:~/Desktop/hello$ vim Makefile.am 

AUTOMAKE_OPTIONS = foreign
bin_PROGRAMS = hello
hello_SOURCES = hello.c
hello_CPPFLAGS = -I /usr/include/
```

6. 执行 `automake --add-missing`

`automake` 会根据 `Makefile.am` 产生一些文件，包含最重要的 `Makefile.in`。

```shell
zjs@ubuntu:~/Desktop/hello$ automake --add-missing
configure.ac:11: installing './compile'
configure.ac:6: installing './install-sh'
configure.ac:6: installing './missing'
Makefile.am: installing './depcomp'
zjs@ubuntu:~/Desktop/hello$ 
zjs@ubuntu:~/Desktop/hello$ ls
aclocal.m4      autoscan.log  config.h.in   depcomp  install-sh   Makefile.in
autom4te.cache  compile       configure.ac  hello.c  Makefile.am  missing
```

7.执行 `autoconf` 得到 `configure`可执行脚本文件

```shell
zjs@ubuntu:~/Desktop/hello$ autoconf
zjs@ubuntu:~/Desktop/hello$ 
zjs@ubuntu:~/Desktop/hello$ ls
aclocal.m4      autoscan.log  config.h.in  configure.ac  hello.c     Makefile.am  missing
autom4te.cache  compile       configure    depcomp       install-sh  Makefile.in
```

8.执行测试

1.执行`./configure` 

2.执行make （此时已生成可执行文件，可用ls查看）

```shell
zjs@ubuntu:~/Desktop/hello$ ./configure 
checking for a BSD-compatible install... /usr/bin/install -c
checking whether build environment is sane... yes
checking for a thread-safe mkdir -p... /usr/bin/mkdir -p
checking for gawk... no
checking for mawk... mawk
checking whether make sets $(MAKE)... yes
checking whether make supports nested variables... yes
checking for gcc... gcc
checking whether the C compiler works... yes
checking for C compiler default output file name... a.out
checking for suffix of executables... 
checking whether we are cross compiling... no
checking for suffix of object files... o
checking whether we are using the GNU C compiler... yes
checking whether gcc accepts -g... yes
checking for gcc option to accept ISO C89... none needed
checking whether gcc understands -c and -o together... yes
checking whether make supports the include directive... yes (GNU style)
checking dependency style of gcc... gcc3
checking that generated files are newer than configure... done
configure: creating ./config.status
config.status: creating Makefile
config.status: creating config.h
config.status: executing depfiles commands
zjs@ubuntu:~/Desktop/hello$ ls
aclocal.m4      autoscan.log  config.h     config.log     configure     depcomp  install-sh  Makefile.am  missing
autom4te.cache  compile       config.h.in  config.status  configure.ac  hello.c  Makefile    Makefile.in  stamp-h1
zjs@ubuntu:~/Desktop/hello$ 
zjs@ubuntu:~/Desktop/hello$ make
make  all-am
make[1]: Entering directory '/home/zjs/Desktop/hello'
gcc -DHAVE_CONFIG_H -I.  -I /usr/include/   -g -O2 -MT hello-hello.o -MD -MP -MF .deps/hello-hello.Tpo -c -o hello-hello.o `test -f 'hello.c' || echo './'`hello.c
mv -f .deps/hello-hello.Tpo .deps/hello-hello.Po
gcc  -g -O2   -o hello hello-hello.o  
make[1]: Leaving directory '/home/zjs/Desktop/hello'
zjs@ubuntu:~/Desktop/hello$ 
zjs@ubuntu:~/Desktop/hello$ ls
aclocal.m4      compile      config.log     configure.ac  hello.c        Makefile     missing
autom4te.cache  config.h     config.status  depcomp       hello-hello.o  Makefile.am  stamp-h1
autoscan.log    config.h.in  configure      hello         install-sh     Makefile.in
```

9.运行可执行文件

因为没有`printf`没有换行，就出现了下面的样子。

```shell
zjs@ubuntu:~/Desktop/hello$ ./hello
hello GUN!zjs@ubuntu:~/Desktop/hello$ 
```

10.修改`hello.c`

```shell
zjs@ubuntu:~/Desktop/hello$ vim hello.c
#include <stdio.h>

int main(int argc, char *argv[])
{
        printf("hello GUN!\n add \\n \n");
        return 0;
}
```

这个时候我们不进行任何操作，再次运行`hello`，结果还是和上面一样

```shell
zjs@ubuntu:~/Desktop/hello$ ./hello
hello GUN!zjs@ubuntu:~/Desktop/hello$ 
```

进行`make`后，运行`hello`可执行文件

```shell
zjs@ubuntu:~/Desktop/hello$ make
make  all-am
make[1]: Entering directory '/home/zjs/Desktop/hello'
gcc -DHAVE_CONFIG_H -I.  -I /usr/include/   -g -O2 -MT hello-hello.o -MD -MP -MF .deps/hello-hello.Tpo -c -o hello-hello.o `test -f 'hello.c' || echo './'`hello.c
mv -f .deps/hello-hello.Tpo .deps/hello-hello.Po
gcc  -g -O2   -o hello hello-hello.o  
make[1]: Leaving directory '/home/zjs/Desktop/hello'
zjs@ubuntu:~/Desktop/hello$ ./hello
hello GUN!
 add \n 
zjs@ubuntu:~/Desktop/hello$ 
```

我们可以发现`make`直接帮助重新编译了。

## 七、总结

`make`和`makefile`可以帮助我们完成自动编译。
在大型程序编译时，如果我们只修改了一个源文件，`make`能确定那些受影响的文件，并保证所有受影响的文件都将重新编译，而不受影响的文件则不予编译。