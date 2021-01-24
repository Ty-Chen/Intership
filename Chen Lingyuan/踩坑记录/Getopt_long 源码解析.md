#  Getopt_long 源码解析

##  简介

在学习完getopt之后，我又将getopt_long的源码看了一下，因为getopt_long的用法相对比与getopt简单一点，所以代码解析也相对容易。

## 用法：

关于长命令选项的用法与短命令选项差不多，基本上有三种：

1.在选项后面不加任何参数。例如 --exec

2.在选项后面必须跟一个参数，参数跟在选项之后，或用空格或 ‘=’ 分开。例如： --exec Test.exe，--exec=Test.exe

3.在选项后面可以跟参数也可以不跟，但是参数必须紧跟在选项之后。例如: --execTest.exe  , -exec

## 函数解析

首先分析函数的参数

```c
int
_getopt_internal (argc, argv, optstring, longopts, longind, long_only)
     int argc;
     char *const *argv;
     const char *optstring;
     const struct option *longopts;
     int *longind;
     int long_only;
```

前面的三个参数在getopt讲过，不再分析，直接看后面三个参数，longopts是指向数组的指针，这个指针是option结构体数组。longind是寻找到的长选项在longopts中的索引值，可用于错误诊断。而最后的long_only表示是否只有长命令选项。

关于option的结构体如下：

```c
struct option
{
# if (defined __STDC__ && __STDC__) || defined __cplusplus
  const char *name;
# else
  char *name;
# endif
  /* has_arg can't be an enum because some compilers complain about
     type mismatches in all the code that assumes it is an int.  */
  int has_arg;
  int *flag;
  int val;
};

/* Names for the values of the `has_arg' field of `struct option'.  */

# define no_argument		0
# define required_argument	1
# define optional_argument	2
#endif	/* need getopt */
```

name：指长命令选项的名字，比如"help" ,"verbose"。

has_arg:描述长选项是否有参数，如果有参数，是哪种类型的参数：

no_argument      0      选项没有参数
required_argument   1      选项需要参数
optional_argument   2      选项参数是可选的

flag：如果该指针为NULL，那么getopt_long返回val字段的值。如果不为NULL，那么会使得它所指向的结构填入val字段的值，同时getopt_long返回0。

val:val通常是个字符常量，它代表长命令选项的简写，如果短选项和长选项一致，那么该字符与opstring中出现的这个选项的参数相同。

```c

      nextchar = (argv[optind] + 1
		  + (longopts != NULL && argv[optind][1] == '-'));
```

继续分析代码，前面部分在getopt中介绍过，这边直接跳过，只有在上段代码中getopt与getopt_long有点区别，因为所有的长命令选项都是形如'--' + 命令选项的形式，所以一旦longopts数组不为空且argv[optind]的第二个字符为'-'表示默认为长命令选项，nextchar指针直接跳向第二个'-'字符的后面一个字符。

我们直接看长命令选项的解析

```c
if (longopts != NULL
      && (argv[optind][1] == '-'
	  || (long_only && (argv[optind][2] || !my_index (optstring, argv[optind][1])))))
    {
      char *nameend;
      const struct option *p;
      const struct option *pfound = NULL;
      int exact = 0;
      int ambig = 0;
      int indfound = -1;
      int option_index;

      for (nameend = nextchar; *nameend && *nameend != '='; nameend++)
	/* Do nothing.  */ ;

      /* Test all long options for either exact match
	 or abbreviated matches.  */
      for (p = longopts, option_index = 0; p->name; p++, option_index++)
	if (!strncmp (p->name, nextchar, nameend - nextchar))
	  {
	    if ((unsigned int) (nameend - nextchar)
		== (unsigned int) strlen (p->name))
	      {
		/* Exact match found.  */
		pfound = p;
		indfound = option_index;
		exact = 1;
		break;
	      }
	    else if (pfound == NULL)
	      {
		/* First nonexact match found.  */
		pfound = p;
		indfound = option_index;
	      }
	    else if (long_only
		     || pfound->has_arg != p->has_arg
		     || pfound->flag != p->flag
		     || pfound->val != p->val)
	      /* Second or later nonexact match found.  */
	      ambig = 1;
	  }

```

首先是条件的判断，如果longopts结构体数组不为空同时参数的第二个字符为'-'或参数的第三个字符不为空或参数的第二个字符在opstring无法找到就进入判断语句。

nameend是一个字符指针，指向参数扫描结束时的位置。

p是指向结构体数组的指针，表示遍历longopts时指向的位置。

pfound同为结构体数组指针，表示找到符合要求的结构体时，指针指向的位置。

exact为标记记号，表示命令选项是否解析成功。

ambig也是标记记号，表示该命令选项是否为模糊命令选项。

indfound则表示在结构体数组中找到符合要求的元素时，数组的索引。

option_index则为结构体数组longopts的索引。

在指针nextchar的起始位置遍历参数，如果找到‘=’符号或遍历完整个argv[optind]，表示扫描结束，nameend指向扫描结束时的位置。从nextchar到nameend的整段字符串默认为命令选项。

接下来对longopts结构体数组进行遍历，如果发现自定义的命令选项与在argv[optind]中截取到的命令选项，它们两者的字符串前缀相同，再进行第二层判断，如果截取到的命令与结构体中的name（命令选项）完全相同，那么这个命令就是没有争议，pfound就指向当前结构体数组的元素，indfound就是当前结构体数组的索引，exact设为1，跳出循环。

如果说只是前缀相同，但与结构体中的name不完全相同，那么就把当前结构体数组中的元素暂时赋值给pfound，索引也暂时赋值给indfound。但是不跳出循环，继续判断这个不完整的命令是否有争议。

第三个if就是在第二个if的基础上，判断有无第二个前缀与当前截取的命令相同的命令选项，但是其他属性完全不同的结构体元素。如果有，则ambig设置为1，表示该命令是模糊命令。

```c
 if (ambig && !exact)
	{
	  if (print_errors)
	    {
           ......
	    }
	  nextchar += strlen (nextchar);
	  optind++;
	  optopt = 0;
	  return '?';
	}
```

循环完成之后就该做解析，首先如果输入的命令是模糊命令，那么直接输出错误，nextchar指向的字符也变成'\0',optind直接跳向下一个参数，并返回‘？’，表示该命令无法解析。

```c


      if (pfound != NULL)
	{
	  option_index = indfound;
	  optind++;
	  if (*nameend)
	    {
	      /* Don't test has_arg with >, because some C compilers don't
		 allow it to be used on enums.  */
	      if (pfound->has_arg)
		optarg = nameend + 1;
	      else
		{
		  if (print_errors)
		    {
              .......
		    }

		  nextchar += strlen (nextchar);

		  optopt = pfound->val;
		  return '?';
		}
	    }
	  else if (pfound->has_arg == 1)
	    {
	      if (optind < argc)
		optarg = argv[optind++];
	      else
		{
		  if (print_errors)
		    {
                .......
		    }
		  nextchar += strlen (nextchar);
		  optopt = pfound->val;
		  return optstring[0] == ':' ? ':' : '?';
		}
	    }
	  nextchar += strlen (nextchar);
	  if (longind != NULL)
	    *longind = option_index;
	  if (pfound->flag)
	    {
	      *(pfound->flag) = pfound->val;
	      return 0;
	    }
	  return pfound->val;
	}
```

如果命令没有争议且合法，那么首先让optim_index的值变为当前匹配到的结构体元素的索引，optind也向后面跳一级。

如果说namend指向的字符不为空，则代表命令与参数是合在一起的，它们只隔了一个‘=’符号。再判断该命令选项后面是否能跟参数，如果可以跟，那么optarg指向nameend之后的字符串，表示命令选项的参数。否则，如果该命令选项后面不能跟任何参数，这么输入就是非法，输出错误，optopt指向该命令选项的简写，并将nextchar指向'\0'。

如果nameend指向的字符为空，则代表输入的只包含命令，不包含任何参数。在该条件的基础上再次对该命令选项的属性做判断。如果该命令后面一定要跟参数，先拿optind与argc做对比，如果optind<argc,则将当前argv的后面一个元素作为本次命令选项的参数，将optind往后再移动一位，如果optind==argc,表示argv数组已经遍历到头，后面已经没有元素了，那么就输出错误，返回'?',将nextchar指向空字符，optopt指向该命令选项的简写。

上述情况都是解析失败之后的情况，而解析成功之后，首先要做的就是将nextchar指向空字符，再将当前longopts的索引赋值给longind.下面那个if (pfound->flag)的情况在上文中flag的定义中有过解释（其实这个情况我也不是非常了解），最后将当前命令选项的简写返回。

```c
   if (!long_only || argv[optind][1] == '-'
	  || my_index (optstring, *nextchar) == NULL)
	{
	  if (print_errors)
	    {
             ......
	    }
	  nextchar = (char *) "";
	  optind++;
	  optopt = 0;
	  return '?';
	}
    }
```

在以上情况都不满足的情况下，对一些特殊情况做判断，如果只有长命令选项集合，那么该命令选项也绝无第二种可能。如果argv的第二个字符为'-'，表示默认为长命令选项，但是在longopts中无法找到。第三个则是代表‘-’后面的字符在短命令选项集合中无法找到。这些情况只要满足一种，则代表该命令非法，返回‘？’，nextchar置空，optind往后跳一级等。

```c
    if (temp[0] == 'W' && temp[1] == ';')
      {
	char *nameend;
	const struct option *p;
	const struct option *pfound = NULL;
	int exact = 0;
	int ambig = 0;
	int indfound = 0;
	int option_index;

	/* This is an option that requires an argument.  */
	if (*nextchar != '\0')
	  {
	    optarg = nextchar;
	    /* If we end this ARGV-element by taking the rest as an arg,
	       we must advance to the next element now.  */
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
	      c = ':';
	    else
	      c = '?';
	    return c;
	  }
	else
	  /* We already incremented `optind' once;
	     increment it again when taking next ARGV-elt as argument.  */
	  optarg = argv[optind++];
......
```

除此之外，还有更加特殊的用法，此行为是GNU才有的扩展功能，在glibc 2之前的库中不可使用。这是在解析短命令选项时做的判断，之前在解析getopt时没写，这里补上。

如果opstring中包含"W;" 举个例子，如果在opstring中为"hW;" ,那么-h  foo或-hfoo 被视为长选项 --foo。

前面的参数定义与之前解析长命令选项时一样，之后如果nextchar之后的字符不为空（nextchar指向'-'字符后面的地址），那么将nextchar后面的字符串赋值给optarg，将它当作长命令选项供后面使用。如果说nextchar后面指向空字符，那么还是先判断optind与argc的关系，如果optind==argc说明越界，后面已经没有元素，输出错误.如果没有越界，则后面还有元素，那么将后面的argv元素赋值给optarg,当作长命令选项，并将optind+1。之后的流程与之前解析长命令选项时一样，这里就不展开了。

## 总结

关于getopt的函数这里就解析完毕了，这个函数的构想还是比较巧妙的，以后在开发过程中还是得多借鉴Linux源码，以便写出更高效的代码。

