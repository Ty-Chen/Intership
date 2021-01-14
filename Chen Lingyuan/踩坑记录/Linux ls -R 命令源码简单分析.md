# Linux ls -R 命令源码简单分析

最近学习了ls -R的源码，在这里记录一下：

ls -R的功能是输出文件夹下的所有文件名，与大多数C/C++ 程序一样，参数都是从main函数传进去。

## main函数的分析

```
main (int argc, char **argv)
{
  register int i;
  register struct pending *thispend;
  unsigned int n_files;
```

随后对参数进行分析：i = decode_switches (argc, argv)。分析参数时运用了Linux中比较常用的命令解析函数getopt()

```
while ((c = getopt_long (argc, argv,
			   "abcdfghiklmnopqrstuvw:xABCDFGHI:LNQRST:UX1",
			   long_options, NULL)) != -1)
    {
      switch (c)
	{

................

}

}
```

其中"abcdfghiklmnopqrstuvw:xABCDFGHI:LNQRST:UX1"是ls 后面跟的各种命令字符，每一种字符代表了不同的用法。getopt对传进来的命令字符进行分析，然后返回对应的数值。在main函数中根据返回的数值调用相应的函数。

这是在main函数中的while循环函数：

```
 while (pending_dirs)
    {
      thispend = pending_dirs;
      pending_dirs = pending_dirs->next;

..........

 print_dir (thispend->name, thispend->realname);
 free_pending_ent (thispend);
  print_dir_name = 1;
}
```

其中print_dir函数的功能是输出文件夹下的文件，pending_dirs是一个结构体链表，代表待执行的目录的记录表，等待被列出。

这是pending结构体的定义

```
struct pending
  {
    char *name;
    char *realname;
    struct pending *next;
  };
```

其中name与realname的具体含义不清楚，大概与文件名有关，next是指向下一个目录的指针。通过对pending_dirs的不断向下查找，得到一个个文件夹，并调用print_dir函数对name与realname做解析，并输出目录下的文件。待所有目录被查询完毕时，退出循环。

## print_dir函数的分析

这是截取print_dir函数的一个片段

```
print_dir (const char *name, const char *realname)
{
  register DIR *dirp;
  register struct dirent *next;

errno = 0;
 *dirp = opendir (name);

........

 while (1)
    {
      /* Set errno to zero so we can distinguish between a readdir failure
	 and when readdir simply finds that there are no more entries.  
      errno = 0;
      if ((next = readdir (dirp)) == NULL)
	{
	  if (errno)
	    {
	    
	      int e = errno;
	      closedir (dirp);
	      errno = e;

	    
	      dirp = NULL;
	    }
	  break;
	}
	
	  if (file_interesting (next))
	{
.........
	  total_blocks += gobble_file (next->d_name, type, 0, name);
	}
    }

.........

if (recursive)
       extract_dirs_from_files (name, command_line_arg);
......
}
```



可以看出来该函数使用了Linux下特有的DIR以及drient结构体，与我自己写的代码有点相似，先是前面执行opendir函数来打开文件，之后调用readdir对当前目录下的文件进行查找，如果readdir函数返回值为NULL，则意味着查找结束，循环退出，每次查找完成之后，调用gobble_file函数将文件加入当前文件表中并返回文件块数。循环执行完之后判断是否递归输出子目录，如果是递归输出，则调用 extract_dirs_from_files函数，将当前子目录从文件表删除，加入待执行目录列表，下面是截取extract_dirs_from_files函数的片段

## extract_dirs_from_files函数的分析

```
extract_dirs_from_files (const char *dirname, int ignore_dot_and_dot_dot)
{
  register int i, j;

........

for (i = files_index - 1; i >= 0; i--)
    if ((files[i].filetype == directory || files[i].filetype == arg_directory)
	&& (!ignore_dot_and_dot_dot
	    || !basename_is_dot_or_dotdot (files[i].name)))
      {
	if (files[i].name[0] == '/' || dirname[0] == 0)
	  {
	    queue_directory (files[i].name, files[i].linkname);
	  }
	else
	  {
	    char *path = path_concat (dirname, files[i].name, NULL);
	    queue_directory (path, files[i].linkname);
	    free (path);
	  }
	if (files[i].filetype == arg_directory)
	  free (files[i].name);
      }

........

}
```

可以看得出函数会对files的filetype属性进行判断，如果是目录，会把该项从文件表中删除，并调用queue_directory函数将目录名当作参数传进去。

## queue_directory函数的分析

```
queue_directory (const char *name, const char *realname)
{
  struct pending *new;

  new = XMALLOC (struct pending, 1);
  new->realname = realname ? xstrdup (realname) : NULL;
  new->name = name ? xstrdup (name) : NULL;
  new->next = pending_dirs;
  pending_dirs = new;
}
```

该函数的意思很简单，就是更新pending_dirs这个待执行目录链表。这就解释了为什么在main函数下的while循环中pending_dirs能不断更新的原因。

## 总结

其实关于ls -R这个命令的复杂程度远不止上述，现只是列出个大概，如果要详细解析ls，还要好好研究源码。