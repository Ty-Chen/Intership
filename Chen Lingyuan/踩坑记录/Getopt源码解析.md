#                                            Getopt源码解析

最近在代码中需要对传进来的命令参数进行解析，很容易想到用Linux下的getopt函数，但是这个函数在Windows下又没有，所以，学习一下getopt的源码，顺便做个记录，由于getopt函数分成两部分，一部分是对长命令选项做解析，一部分是对短命令选项做解析，这里只涉及到短命令选项。

## 命令种类

关于短命令一共分为三种：

1.单个字符，表示选项，后面不能跟参数。例：-a  ，-abc

2.单个字符后面接一个冒号，表示该选项后面必须跟一个参数，参数紧跟在选项之后或者有空格分开，该参数的指针赋值给optarg。例:-a Test.exe ,  -aText.exe

3.单个字符后面接两个冒号，表示该选项后面可以跟参数也可以不跟，但是参数必须紧跟在选项之后不得分开，该参数的指针赋值给optarg。例：-a,  -aTest.exe

## 函数解析

首先是全局变量

```c
char *optarg;
int optind = 1;
static char *nextchar;
int opterr = 1;
int optopt = '?';
```

当getopt找到一个带参数的选项时，将参数的地址赋值给optarg。optind则是argv的索引。nextchar是选项元素中要扫描的下一个字符。opterr则是错误标识符，如果为0则代表扫描继续。optopt则是未识别的命令选项。

```c
_getopt_internal (argc, argv, optstring, longopts, longind, long_only)
     int argc;
     char *const *argv;
     const char *optstring;
     const struct option *longopts;
     int *longind;
     int long_only;
```

分析getopt的参数，argc代表传入的命令行参数数量，argv则是传入的命令行参数，optstring是具体规定的命令选项字符串，后面的参数都跟长命令选项有关，longopt是有关结构体，longind为长命令的索引，long_only应该是是否只有长命令选项。

```c
 if (argc < 1)
    return -1;

  optarg = NULL;

  if (optind == 0 || !__getopt_initialized)
    {
        if (optind == 0)
        {
             optind = 1;	
        }
        optstring = _getopt_initialize (argc, argv, optstring);
        __getopt_initialized = 1;
    }
......
# define NONOPTION_P (argv[optind][0] != '-' || argv[optind][1] == '\0')
```

先判断argc的数量，如果<1直接返回-1，表示解析结束，再将optarg,optind初始化，opstring则调用了_getopt_initialize函数，最后定义了宏定义。

这是_getopt_initialize函数的定义

```c
_getopt_initialize (argc, argv, optstring)
     int argc;
     char *const *argv;
     const char *optstring;
{
......

  nextchar = NULL;

  posixly_correct = getenv ("POSIXLY_CORRECT");

    if (optstring[0] == '-')
    {
        ordering = RETURN_IN_ORDER;
        ++optstring;
    }
    else if (optstring[0] == '+')
    {
        ordering = REQUIRE_ORDER;
        ++optstring;
    }
    else if (posixly_correct != NULL)
    {
        ordering = REQUIRE_ORDER;
    }
    else
    {
        ordering = PERMUTE;
    }
......
  return optstring;
}

```

如果opstring 的一个字符为'+'或设置了环境变量POSIXLY_CORRECT，则将ordering置为REQUIRE_ORDER,表示一旦遇到非命令选项也非选项参数的元素，就认为没有更多的选项存在了，直接停止扫描。

如果opstring的第一个字符为'-',将ordering置为RETURN_IN_ORDER，表示每个非命令选项也非选项参数的argv元素都被处理为ASCLL为1的选项字符的参数。

其余情况下为默认值。

```c
  if (nextchar == NULL || *nextchar == '\0')
  {
  ......

      if (ordering == PERMUTE)
	  {
	       ......
	       while (optind < argc && NONOPTION_P)
	       optind++;
	       ......
	  }


      if (optind != argc && !strcmp (argv[optind], "--"))
	  {
	       optind++;
            ......
	       optind = argc;
	  }  

      /* If we have done all the ARGV-elements, stop the scan
	 and back over any non-options that we skipped and permuted.  */

      if (optind == argc)
	  {
            ......
	       return -1;
	  }

      /* If we have come to a non-option and did not permute it,
	 either stop the scan or describe it to the caller and pass it by.  */

      if (NONOPTION_P)
	  {
          if (ordering == REQUIRE_ORDER)
          {
               return -1;
          }
          
	     optarg = argv[optind++];
          {
               return 1;
          }
	  }

      /* We have found another option-ARGV-element.
	 Skip the initial punctuation.  */

      nextchar = (argv[optind] + 1 + (longopts != NULL && argv[optind][1] == '-'));
 }
```

当nextchar为空时，进入if语句，如果为默认情况，利用while循环查找第一个初步判定不为非法命令项的argv元素（NONOPTION_P是上面的宏定义，表示参数的一个字符不为'-'或参数的第二个字符为空，这两者都是非法命令）。如果找到，退出循环，否则直到optind>=argc（参数最大数量为argc，如果索引>=argc表示已经越界）。

接着再判定argv元素是否是'--'，如果为真，表示扫描提前结束，optind直接变为argc。

再判定optind是否等于argc，如果为真，表示命令选项字符串扫描完毕，退出函数。

至于为什么还要继续对非法命令选项做判定，那是因为上面在做判定时针对的是默认情况也就是PERMUTE，而在  if (NONOPTION_P)语句下做的是REQUIRE_ORDER以及RETURN_IN_ORDER模式下的判定，如果在REQUIRE_ORDER模式下，遇到非法参数直接返回-1，表示扫描结束。而在RETURN_IN_ORDER模式下，非法参数都是ascll为1的字符选项字符的参数，所以将optarg赋值为argv[optind]，并返回1。

如果参数argv[optind]的第一个字符为'-'同时第二个字符不为空，nextchar就指向argv[optind][1],也就是‘-’后面的一个字符，后面的括号是判断输入的命令选项是否是长命令选项（短命令选项开头是'-',而长命令选项则是'--'）,如果是长命令选项，指针再加1。

 跳过长命令选项的解析，直接看短命令。

```c
  char c = *nextchar++;
  char *temp = my_index (optstring, c);

    /* Increment `optind' when we start to process its last character.  */
    if (*nextchar == '\0')
    { 
         ++optind;
    }

    if (temp == NULL || c == ':')
    {
          ......
	     optopt = c;
	     return '?';
    }
```

```c
my_index (str, chr)
     const char *str;
     int chr;
{
  while (*str)
  {
      if (*str == chr)
      {
           return (char *) str;
      }
      str++;
  }
  return 0;
}
```

字符c为nextchar指针指向的第一个字符，也就是‘-’后面的那个字符，同时再将nextchar指针加1，然后在opstring（命令选项字符串中）中对字符c进行查找，这里调用了my_index这个函数，这个函数与C++库函数strchr很像，具体功能都是寻找 字符ch在字符串str中的首次出现位置。如果nextchar指向空字符，则表示后面没有紧跟参数,那将optind+1。接着再判断temp，如果temp为空，则表示在opstring找不到该命令字符。如果temp为':',则表示'-'符号后面跟的是':'符号。这两种情况都是非法的，需要对这种情况进行error处理（具体细节已省略），同时将字符c赋值给optopt并返回‘?’符号，表示这是无法识别的命令字符。

```c
 if (temp[1] == ':')
{
    if (temp[2] == ':')
    {
        /* This is an option that accepts an argument optionally.  */
        if (*nextchar != '\0')
        {
            optarg = nextchar;
            optind++;
        }
        else
        {
            optarg = NULL;
        }

        nextchar = NULL;
    }
    else
    {
        if (*nextchar != '\0')
        {
            optarg = nextchar;
            optind++;
        }
        else if (optind == argc)
        {
            if (print_errors)
            {
                ......
            }
            optopt = c;

            if (optstring[0] == ':')
            {
                c = ':';
            }
            else
            {
                c = '?';
            }
        }
        else
        {
            optarg = argv[optind++];
        }

        nextchar = NULL;
    }
}
return c;

```

上述情况如果都符合，则表示temp[0]在opstring中能够找得到，那对temp[1]与temp[2]进行判断。

   如果temp[1]为':'符号，而temp[2]不为':'符号则表示该命令选项后面必须跟一个参数，继续对nextchar进行判断，如果nextchar不为空，则表示参数紧跟在命令选项之后，将nextchar之后的内容赋值给optarg,同时将optind+1。否则nextchar就为空，然后对nextchar为空的情况再做出判断，由于之前在nextchar为空时已经对optind+1,optind实际上已经指向下一个参数，这个时候与argc进行比较，如果optind==argc表示已经越界，后面没有参数可以跟，之后就输出错误。如果没有越界，则optarg指向当前参数的下一个参数，同时对optind+1,将nextchar赋空。

如果temp[1]与temp[2]都为':',则是可选参数命令选项，如果nextchar不为空，则表示参数紧跟在命令选项之后，将nextchar之后的内容赋值给optarg，同时将optind+1,如果为空，表示命令选项之后不跟参数，将optarg与nextchar置空。

如果都不是，则为后面不跟参数的命令选项，这种情况比较复杂，因为无参命令选项涉及形如'-abc'与-a 这两种情况，所以解析命令需要执行好几次getopt函数，首先是形如'-a'的情况，这种比较简单，在上文的if语句中即可做出判断

```c
  if (*nextchar == '\0')
      ++optind;
```

因为'a'字符后面没有字符，所以*nextchar为'\0',同时optind+1,直接跳向下一个参数，由于后面的if语句都不满足，所以会直接返回。第二次进入函数，因为*nextchar为'\0',所以会直接进入上文的if语句，直接对'-a'的下一个参数做解析。

```c
if (nextchar == NULL || *nextchar == '\0')
```

 然后是形如‘-abc’这种情况，这种情况是几个无参命令选项的混合，由于不符合(*nextchar == '\0')的情况，所以optind也不会加1，后面的if语句也不会执行，当然nextchar也不会赋空，直接返回。第二次执行getopt时，由于nextchar不为空，因此不会进入if (nextchar == NULL || *nextchar == '\0')这个语句，也就是说nextchar不会改变，optind不会改变。所以在继续执行以下代码块时就是对‘a'后面的'b'字符做出解析，然后一直循环往复直到将'-abc'这个混合无参命令选项解析完毕。

```c
char c = *nextchar++;
char *temp = my_index (optstring, c);
```

## 总结：

getopt逻辑还是比较跳跃的，所以在写起来的时候显得有点乱，还有些细节方面没有做解析，至于getopt_long,请看下面的链接了。

[Getopt_Long源码解析](Getopt_long 源码解析.md)

