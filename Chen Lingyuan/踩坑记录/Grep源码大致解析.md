#                                                            Grep源码大致解析

## 简介  

之前写了很久文本字符串搜索工具，只是简单借鉴了一下grep的基本原理，但是对于grep的具体实现一直都没怎么明白，所以这次就简单解析一下grep源码，看看自己写的与grep有什么不同。

我设置了最简单的模式串与文本作为测试用例

Pattern： DEBUG

Text： Test/test.txt

下面是text的内容：

```
DEBUG DEBUG
DEBUG AAA
DEBUG CCC
DEBUG
AAAAAAA
CCCCCCC
```

   首先还是从main函数开始

```c
int
main(int argc, char *argv[])
{
    ......
 while ((opt = get_nondigit_option (argc, argv, &default_context)) != -1)
    switch (opt)
  {
      ......
  }
```

​    先是用getopt函数对传进来的参数做解析，这个函数以前做过解析，详见请看[Getopt_Long源码解析](Getopt_long 源码解析.md)与[Getopt源码解析](Getopt源码解析.md)

```c
 if (optind < argc)
      {
	/* A copy must be made in case of an xrealloc() or free() later.  */
	keycc = strlen(argv[optind]);
	keys = xmalloc(keycc + 1);
	strcpy(keys, argv[optind++]);
      }
 ......
     compile(keys, keycc);

```

解析完毕之后我们得到keys与keycc两个变量，其中keys是模式串，keycc是模式串的长度，之后就调用complie对模式串做解析，由于要对模式串做正则判断，需要涉及很多函数，这里就不展开了，直接看比较核心的部分：

```c
const char *
kwsprep (kwset_t kws)
{
  register struct kwset *kwset;
  register int i;
  register struct trie *curr;
  register char const *trans;
  unsigned char delta[NCHAR];

  kwset = (struct kwset *) kws;
......

  memset(delta, kwset->mind < UCHAR_MAX ? kwset->mind : UCHAR_MAX, NCHAR);
    ......
 for (i = 0; i < kwset->mind; ++i)
    delta[(unsigned char) kwset->target[i]] = kwset->mind - (i + 1);
    ......
  if ((trans = kwset->trans) != NULL)
    for (i = 0; i < NCHAR; ++i)
      kwset->delta[i] = delta[U(trans[i])];
  else
    memcpy(kwset->delta, delta, NCHAR);
......
```

这个就是对模式串的预处理，看着比较抽象，delta应该为BM算法中的跳转数组。

```c
struct kwset
{
  struct obstack obstack;   /* Obstack for node allocation. */
  int words;            /* Number of words in the trie. */
  struct trie *trie;        /* The trie itself. */
  int mind;         /* Minimum depth of an accepting node. */
  int maxd;         /* Maximum depth of any node. */
  unsigned char delta[NCHAR];   /* Delta table for rapid search. */
  struct trie *next[NCHAR]; /* Table of children of the root. */
  char *target;         /* Target string if there's only one. */
  int mind2;            /* Used in Boyer-Moore search for one string. */
  char *trans;          /* Character translation table. */
};
```

这是关于kwset的结构体，里面的属性囊括模式串的各种信息，包括长度，跳转表，以及模式串的内容等。

```c
	  do
	{
	  char *file = argv[optind];
	  if ((included_patterns || excluded_patterns)
	      && !isdir (file))
	    {
          ......
               status &= grepfile (strcmp (file, "-") == 0 ? (char *) NULL : file,
			      &stats_base);
	}
```

```c
grepfile (char const *file, struct stats *stats)
{
  int desc;
  int count;
  int status;
......
     else
    {
      if (stat (file, &stats->stat) != 0)
        {
          suppressible_error (file, errno);
          return 1;
        }
      if (directories == SKIP_DIRECTORIES && S_ISDIR (stats->stat.st_mode))
        return 1;
         ......
    while ((desc = open (file, O_RDONLY)) < 0 && errno == EINTR)
	continue;
     if (desc < 0)
	{
	  int e = errno;

	  if (is_EISDIR (e, file) && directories == RECURSE_DIRECTORIES)
	    {
	      if (stat (file, &stats->stat) != 0)
		{
		  error (0, errno, "%s", file);
		  return 1;
		}

	      return grepdir (file, stats);
	    }
         ......
 count = grep (desc, file, stats);
         ......
while (close (desc) != 0)
	  if (errno != EINTR)
	    {
	      error (0, errno, "%s", file);
	      break;
	    }
         ......
```

```c
grepdir (const char *dir, struct stats const *stats)
{
  struct stats const *ancestor;
  char *name_space;
  int status = 1;
    ......
         name_space = savedir (dir, stats->stat.st_size, included_patterns,
			excluded_patterns, excluded_directory_patterns);
    ......
 while (*namep)
	{
	  size_t namelen = strlen (namep);
	  file = xrealloc (file, dirlen + 1 + namelen + 1);
	  strcpy (file, dir);
	  file[dirlen] = '/';
	  strcpy (file + dirlen + needs_slash, namep);
	  namep += namelen + 1;
	  status &= grepfile (file, &child);
	}
    ......
```

```c
savedir (const char *dir, off_t name_size, struct exclude *included_patterns,
	 struct exclude *excluded_patterns, struct exclude *excluded_directory_patterns )
{
  DIR *dirp;
  struct dirent *dp;
  char *name_space;
  char *namep;
 dirp = opendir (dir);
  if (dirp == NULL)
    return NULL;
  while ((dp = readdir (dirp)) != NULL)
    {
      ......
           if ((included_patterns || excluded_patterns)
	      && !isdir1 (dir, dp->d_name))
               ......
               if ( excluded_directory_patterns
	      && isdir1 (dir, dp->d_name) )
	    {
	      if (excluded_directory_patterns
		  && excluded_filename (excluded_directory_patterns, dp->d_name, 0))
		continue;
	    }
      ......
               
```

```c
grep (int fd, char const *file, struct stats *stats)
{
  int nlines, i;
  int not_text;
  size_t residue, save;
  char oldc;
  char *beg;
  char *lim;
  char eol = eolbyte;

  if (!reset (fd, file, stats))
    return 0;

  if (file && directories == RECURSE_DIRECTORIES
      && S_ISDIR (stats->stat.st_mode))
    {
      /* Close fd now, so that we don't open a lot of file descriptors
	 when we recurse deeply.  */
      if (close (fd) != 0)
	error (0, errno, "%s", file);
      return grepdir (file, stats) - 2;
    }
```

```c
int
isdir (const char *path)
{
  struct stat stats;

  return stat (path, &stats) == 0 && S_ISDIR (stats.st_mode);
}

```

之后开始对输入的文件名做解析，其实就是几个函数嵌套而成，直接进入grepfile函数,首先因为file不为空，所以直接跳过第一层判断。进入else之后，对file进行解析，执行stat函数，更新stats的值。接着执行open系统函数，以只读的方式打开file，因为在Linux下文本的读写 二进制模式与文本模式没有差异，所以效率上也是一样的。然后返回文件句柄，因为文件句柄大于0，所以直接转到grep函数，将此前更新的stats也传入到grep函数，在grep函数里面再做判断，执行S_ISDIR，判断出是文件夹，不符合要求，所以继续转进到grepdir函数,执行savedir函数，在savedir函数中执行opendir与readdir函数，逐次读取目录下的文件，将目录下的所有文件名拼接成一个长字符串，关闭目录流，释放资源，并返回。并在grepdir函数中，拆分字符串。重新定义stats，将stats与分离的file当成参数传入grepfile函数。之后继续循环，直到搜索完所有的文件。

```c
grep (int fd, char const *file, struct stats *stats)
{
  int nlines, i;
  int not_text;
  size_t residue, save;
  char oldc;
  char *beg;
  char *lim;
  char eol = eolbyte;

  if (!reset (fd, file, stats))
    return 0;

......
   totalcc = 0;
  lastout = 0;
  totalnl = 0;
  outleft = max_count;
  after_last_match = 0;
  pending = 0;

  nlines = 0;
  residue = 0;
  save = 0;

  if (! fillbuf (save, stats))
    {
      if (! is_EISDIR (errno, file))
	suppressible_error (filename, errno);
      return 0;
    }
```

之后调用reset函数来设置缓冲区大小，并初始化。

```c
 if (! pagesize)
    {
      pagesize = getpagesize ();
      if (pagesize == 0 || 2 * pagesize + 1 <= pagesize)
	abort ();
      bufalloc = ALIGN_TO (INITIAL_BUFSIZE, pagesize) + pagesize + 1;
      buffer = xmalloc (bufalloc);
    }

  bufbeg = buflim = ALIGN_TO (buffer + 1, pagesize);
  bufbeg[-1] = eolbyte;
  bufdesc = fd;

......
}
```

首先判断有没有将缓冲区初始化过，如果没有设置，就进入if语句，这样就避免了重复申请内存带来的时间与内存浪费。之后将分配的缓冲区保存区域大小（bufsalloc）重新设置，分配内存，buffer则是缓冲区，bufbeg为搜索的起始地址，buflim则是搜索的终止地址。

接着进入fillbuf来填充缓冲区

```c
fillbuf (size_t save, struct stats const *stats)
{
  size_t fillsize = 0;
  int cc = 1;
  char *readbuf;
  size_t readsize;

    sp = buflim - save;
    dp = nbuffer + bufsalloc - save;
    bufbeg = dp;
    ......
          cc = read(bufdesc, buffer + bufsalloc, bufalloc - bufsalloc);
#endif
if (! fillsize)
    {
      ssize_t bytesread;
      while ((bytesread = read (bufdesc, readbuf, readsize)) < 0
	     && errno == EINTR)
	continue;
      if (bytesread < 0)
	cc = 0;
      else
	fillsize = bytesread;
    }
......
  buflim = readbuf + fillsize;
  return cc;
}
        
```

调用read函数读取文件中的内容，并返回读取的字节数量，将数量赋值给bytesread，并重新移动buflim的值，将buflim移动到读取文件的末尾，最终返回是否读取成功，如果读取成功返回1，否则返回0。

 之后继续在grep进行解析

```c
......
    for (lim = buflim; lim[-1] != eol; lim--)
	continue;
        residue = buflim - lim;
         if (beg < lim)
	{
	  if (outleft)
	    nlines += grepbuf (beg, lim);
	 ......
	}
```

  首先第一个for循环的意思是从buflim这个位置往前搜索，直到搜索到第一个换行符停止，这里的eol为换行符，这个在main函数已经定义过，然后将新的终止地址赋值给lim,也就是将最后一行单独拿出来处理了，之后将起始地址与终止地址当成参数，调用grepbuf函数，匹配缓冲区内的字符串，并返回匹配的行数。

```c
grepbuf(char *beg,char *lim,char *NAME)
{

    int nlines, n;
    register char *p, *b;
    char *endp;

    nlines = 0;
    p = beg;


     while ((match_offset = execute(p, lim - p, &match_size,
				 NULL)) != (size_t) -1) {
        ......
```

之后就是进入execute函数进行字符串匹配了，将匹配起始地址与匹配字节数当成参数传进去。

```c
EXECUTE_FCT(Fexecute)
{
  register char const *beg, *try, *end;
  register size_t len;
  char eol = eolbyte;
  struct kwsmatch kwsmatch;
  size_t ret_val;
......
  buflim = buf + size;

  for (beg = start_ptr ? start_ptr : buf; beg <= buf + size; beg++)
    {
      size_t offset = kwsexec (kwset, beg, buf + size - beg, &kwsmatch);
      if (offset == (size_t) -1)
	goto failure;
#ifdef MBS_SUPPORT
      if (MB_CUR_MAX > 1 && mb_properties[offset+beg-buf] == 0)
	continue; /* It is a part of multibyte character.  */
#endif /* MBS_SUPPORT */
      beg += offset;
......
```

设置buflim，将它的值设置为buf偏移size个单位，同时将beg与end的初始值设置为buf，每次循环时执行kwsexec函数，将匹配起始地址与字节数传进去，kwsexec函数返回匹配完成时指针的偏移量，将beg指针加上这个偏移量就是下一次匹配时的起始地址。

```c
char *
kwsexec(kwset_t kws, char *text, size_t size, struct kwsmatch *kwsmatch)
{
  struct kwset *kwset;
  char *ret;

  kwset = (struct kwset *) kws;
  if (kwset->words == 1 && kwset->trans == 0)
    {
      ret = bmexec(kws, text, size);
      if (kwsmatch != 0 && ret != 0)
    {
      kwsmatch->index = 0;
      kwsmatch->beg[0] = ret;
      kwsmatch->size[0] = kwset->mind;
    }
      return ret;
    }
    ......
}
```

```c
static char *
bmexec(kwset_t kws, char *text, size_t size)
{
  struct kwset *kwset;
  register unsigned char *d1;
  register char *ep, *sp, *tp;
  register int d, gc, i, len, md2;

  kwset = (struct kwset *) kws;
  len = kwset->mind;

  if (len == 0)
    return text;
  if ((size_t)len > size)
    return 0;
  if (len == 1)
    return memchr(text, kwset->target[0], size);

  d1 = kwset->delta;
  sp = kwset->target + len;
  gc = U(sp[-2]);
  md2 = kwset->mind2;
  tp = text + len;

.......

  return 0;
}
```

之后就是执行BM匹配算法了，这也是整个命令的核心之处，执行完毕之后返回匹配到的地址，接着就将匹配到的行打印输出。这部分的函数过于抽象，也没怎么看懂。

```c
static void
prline(char *beg, char *lim, char sep, char * NAME)
{
    if ( out_file )
        printf("%s%c", NAME, sep);
    if ( out_line ) {
        nlscan(beg);
        printf("%d%c", ++totalnl, sep);
        lastnl = lim;
    }
    if ( out_byte )
        printf("%lu%c", totalcc + (beg - bufbeg), sep);
    fwrite(beg, 1, lim - beg, stdout);
    if ( ferror(stdout) )
        error("writing output", errno);
    lastout = lim;c
}

```

最后调用prline函数把匹配到的行进行输出，输出完之后继续调用上述的 kwsexec函数，直到所有的内容都搜索完毕。

## 总结：

  与我之前写的文本搜索工具相比，grep在处理方面与我的还是有很多不同，我的代码是先搜索文件，将文件放入待执行列表里面，再分配内存块，最后解析模式串进行文本搜索，并记录匹配节点，最后根据匹配节点输出匹配内容。但是grep的执行与我的有非常大的不同，它是先解析模式串，接着搜索文件，并分配一次内存块，每搜索到一个文件就进行文本匹配，在搜索文件时，也不记录匹配节点，而是匹配到之后，跳出BM，直接打印匹配的行，并移动搜索时的起始指针，继续下一次匹配。对照我的代码，这样写有几点好处：搜索到一个文件直接对文本进行搜索省去了二次遍历文件的时间，如果文件数量过多，这样做还是比较节省时间的。还有就是省去了申请储存匹配节点的内存块，节省了空间。同时，匹配到模式串直接输出也省去了二次遍历匹配节点所需要的时间，这样在时间与空间上都有所优化。

  grep命令非常复杂，同时代码也非常抽象，这边也就写了个大概，更具体的解析也只能之后再说了。

