# 调试Makefile变量

## 一、简介

对于Makefile中的各种变量，我们要查看他们并不方便，需要修改makefile加入`echo`命令。
我们可以自己写一个专门用来输出变量的Makefile(假设名字是DebugMakefile)。

## 二、输出变量的Makefile代码

```shell
%:
        @echo '$*=$($*)'

d-%:
        @echo '$*=$($*)'
        @echo ' origin = $(origin $*)'
        @echo ' value  = $(value  $*)'
        @echo ' flavor = $(flavor $*)'
```

这样我们就可以使用`make`命令的`-f`参数来查看`makefile`中的相关变量（包括`make`的内建变量，比如：`COMPILE.c`或`MAKE_VERSION`之类的）。
**注意：第二个以`d-`为前缀的目标可以用来打印关于这个变量更为详细的东西**

## 三、如何使用

```shell
zjs@ubuntu:~/Desktop/SearchKey/SearchKey$ make -f Makefile -f DebugMakefile appname
appname=SearchKey
zjs@ubuntu:~/Desktop/SearchKey/SearchKey$ make -f Makefile -f DebugMakefile d-srcfiles
srcfiles=./SearchKey.cpp ./KParameterAnalysis.cpp ./KSunday.cpp ./KSearch.cpp
 origin = file
 value  = $(shell find . -maxdepth 1 -name "*.cpp")
 flavor = recursive
```

- `make`的第一个`-f`后是要测试的makefile，第二个`-f`是我们的debug makefile。

- 后面直接跟变量名，如果在变量名前加`d-`，则输出更为详细的东西。

`d-`调用的三个参数：

| **origin** | **告诉你这个变量来自哪里，例如：file表示文件，environment表示环境变量** |
| ---------- | ------------------------------------------------------------ |
| **value**  | **输出这个变量没被展开的样子**                               |
| **flavor** | **有两个值，simple表示是一般展开的变量，recursive表示递归展开的变量。** |

## 四、总结

专门写一个用来输出变量的Makefile，可以帮助我们调试我们自己写的Makefile。