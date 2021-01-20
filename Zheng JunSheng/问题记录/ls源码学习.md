# ls源码学习

## 1.简介

命令用于显示指定工作目录下之内容（列出目前工作目录所含之文件及子目录)。

语法：

```
 ls [-alrtAFR] [name...]
```

## 2.源码分析

```
int
main (int argc, char **argv)
{
  register int i;
  register struct pending *thispend;
  unsigned int n_files;
  ...
  i = decode_switches (argc, argv);
  ...
    while (pending_dirs)
    {
      thispend = pending_dirs;
      pending_dirs = pending_dirs->next;

      if (LOOP_DETECT)
      {
          if (thispend->name == NULL)
          {
          struct dev_ino di = dev_ino_pop ();
          struct dev_ino *found = hash_delete (active_dir_set, &di);
          assert (found);
          dev_ino_free (found);
          free_pending_ent (thispend);
          continue;
          }
      }

      print_dir (thispend->name, thispend->realname);

      free_pending_ent (thispend);
      print_dir_name = 1;
    }
    ...
 }
```

以上就是ls main中大致的流程。

在定义、初始化之后，先进行`decode_switches (argc, argv)`，这个函数会通过`getopt`来分析参数。

```
while ((c = getopt_long (argc, argv,
			   "abcdfghiklmnopqrstuvw:xABCDFGHI:LNQRST:UX1",
			   long_options, NULL)) != -1)
    {
      switch (c)
      {
      	...
      }
     }
     
```

其中`abcdfghiklmnopqrstuvw:xABCDFGHI:LNQRST:UX1`是ls命令后面可以跟的参数。`:`表示后面跟一个参数。在分析参数之后，会返回相对应的数值，而main也会根据这个数值来调用相对应的函数。

在下来，我们可以看到一个while循环。其中`pending_dirs`是一个结构体链表，代表待执行的目录的记录表，等待被列出。

```
struct pending
  {
    char *name;
    char *realname;
    struct pending *next;
  };
```

`print_dir`函数的功能是输出文件夹下的文件。

以下是`print_dir`代码中的一部分。

```
static void
print_dir (const char *name, const char *realname)
{
  register DIR *dirp;
  register struct dirent *next;
  ...
    while (1)
    {
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
	  	enum filetype type = unknown;

#if HAVE_STRUCT_DIRENT_D_TYPE
	  	if (next->d_type == DT_BLK
	      || next->d_type == DT_CHR
	      || next->d_type == DT_DIR
	      || next->d_type == DT_FIFO
	      || next->d_type == DT_LNK
	      || next->d_type == DT_REG
	      || next->d_type == DT_SOCK)
	    type = next->d_type;
#endif
	  	total_blocks += gobble_file (next->d_name, type, 0, name);
	 }
 	 ...
 }
```

我们可以看到该函数使用了DIR以及`dirent`结构体，先用`opendir`函数来打开文件，再用`readdir`对当前目录下的文件进行查找，如果`readdir`函数返回值为NULL，则意味着查找结束，循环退出。每次查找完成之后，调用gobble_file函数将文件加入当前文件表中并返回文件数。

## 3.总结

ls命令很复杂，这里只分析了一部分，还需要继续研究ls源码。