# `grep`源代码解析

## 一、简介

对于`grep`具体实现，不是很清楚，只是基本了解`grep`的原理。在此，简单解析一下`grep`源码。

启动`gdb`调试`grep`源码，将参数设置为：`DEBUG *.txt`。

我们看一下`*.txt`都有什么文件

```shell
zjs@ubuntu:~/Desktop/grep-3.6/src$ ls *.txt
a.txt  b.txt  c.txt

abc.txt:
abc.txt
```

## 二、调试过程

首先我们在`main`打一个断点(`b main`),再运行 `r DEBUG *.txt`。

我们可以`info`命令来看一下`main`函数的参数，如下：

```shell
(gdb) info frame
Stack level 0, frame at 0x7fffffffdf20:
 rip = 0x555555557ccd in main (grep.c:2471); saved rip = 0x7ffff7deb0b3
 source language c.
 Arglist at 0x7fffffffdd18, args: argc=6, argv=0x7fffffffe008
 Locals at 0x7fffffffdd18, Previous frame's sp is 0x7fffffffdf20
 Saved registers:
  rbx at 0x7fffffffdee8, rbp at 0x7fffffffdef0, r12 at 0x7fffffffdef8, r13 at 0x7fffffffdf00,
  r14 at 0x7fffffffdf08, r15 at 0x7fffffffdf10, rip at 0x7fffffffdf18
(gdb) p *argv@6
$1 = {0x7fffffffe352 "/home/zjs/Desktop/grep-3.6/src/grep", 0x7fffffffe376 "DEBUG", 
  0x7fffffffe37c "abc.txt", 0x7fffffffe384 "a.txt", 0x7fffffffe38a "b.txt", 0x7fffffffe390 "c.txt"}
```

我们可以看到参数一共有六个，分别是`grep`代码的路径，模式串，三个文件名和一个目录名。在传参的时候，`*.txt`就已经变成当前目录中所有`.txt`，但是这里面并不会判断是不是目录，也没有获取到下级目录的`.txt`。

在经过一系列初始化后，我们可以看到：

```c++
 while (prev_optind = optind,
         (opt = get_nondigit_option (argc, argv, &default_context)) != -1)
```

`grep`在初始化结束之后，会进行参数分析。让我们进入`get_nondigit_option`看看。

```c++
static int
get_nondigit_option (int argc, char *const *argv, intmax_t *default_context)
{
  static int prev_digit_optind = -1;
  int this_digit_optind;
  bool was_digit;
  char buf[INT_BUFSIZE_BOUND (intmax_t) + 4];
  char *p = buf;
  int opt;

  was_digit = false;
  this_digit_optind = optind;
  while (true)
    {
      opt = getopt_long (argc, (char **) argv, short_options,
                         long_options, NULL);
      if (! c_isdigit (opt))
        break;

      if (prev_digit_optind != this_digit_optind || !was_digit)
        {
          p = buf;
        }
      else
        {
          p -= buf[0] == '0';
        }

      if (p == buf + sizeof buf - 4)
        {
          strcpy (p, "...");
          p += 3;
          break;
        }
      *p++ = opt;

      was_digit = true;
      prev_digit_optind = this_digit_optind;
      this_digit_optind = optind;
    }
  if (p != buf)
    {
      *p = '\0';
      context_length_arg (buf, default_context);
    }

  return opt;
}
```

可以看到会调用`getopt_long`来解析`argv`。因为我们调试的参数没有其他参数，所以`getopt_long`会返回`-1`。

```c++
...
 else if (optind < argc)
   {
      /* Make a copy so that it can be reallocated or freed later.  */
      pattern_array = keys = xstrdup (argv[optind++]);
      ptrdiff_t patlen = strlen (keys);
      keys[patlen] = '\n';
      keycc = update_patterns (keys, 0, patlen + 1, "");
    }
...
```

此时`opotind`等于1， `argc`等于6。这里也就会对模式串进行处理，例如:拷贝，求模式串长度，添加`\n`。

解析完毕，我们得到了模式串和模式串长度，之后通过`matchers[].compile`和`matcher`处理模式串。

```c++
static struct
{
  char name[12];
  int syntax; /* used if compile == GEAcompile */
  compile_fp_t compile;
  execute_fp_t execute;
} const matchers[] = {
  { "grep", RE_SYNTAX_GREP, GEAcompile, EGexecute },
  { "egrep", RE_SYNTAX_EGREP, GEAcompile, EGexecute },
  { "fgrep", 0, Fcompile, Fexecute, },
  { "awk", RE_SYNTAX_AWK, GEAcompile, EGexecute },
  { "gawk", RE_SYNTAX_GNU_AWK, GEAcompile, EGexecute },
  { "posixawk", RE_SYNTAX_POSIX_AWK, GEAcompile, EGexecute },
#if HAVE_LIBPCRE
  { "perl", 0, Pcompile, Pexecute, },
#endif
};
```

接下来，就会分配缓冲区大小。

```c++
  long psize = getpagesize ();
  
  if (! (0 < psize && psize <= (SIZE_MAX - sizeof (uword)) / 2))
    abort ();
  pagesize = psize;
  bufalloc = ALIGN_TO (INITIAL_BUFSIZE, pagesize) + pagesize + sizeof (uword);
  buffer = xmalloc (bufalloc);
```

`ALIGN_TO`宏的定义如下：

```c++
#define ALIGN_TO(val, alignment) \
  ((size_t) (val) % (alignment) == 0 \
   ? (val) \
   : (val) + ((alignment) - (size_t) (val) % (alignment)))
```

通过`print`命令，我们可以查看`pagesize`等于4096， `bufalloc`等于102408，`sizeof(uword)`等于8。

接下来处理文件名，

```c++
  char *const *files;
  if (0 < num_operands)
    {
      files = argv + optind;
    }
  else if (directories == RECURSE_DIRECTORIES && 0 < last_recursive)
    {
      static char *const cwd_only[] = { (char *) ".", NULL };
      files = cwd_only;
      omit_dot_slash = true;
    }
  else
    {
      static char *const stdin_only[] = { (char *) "-", NULL };
      files = stdin_only;
    }
```

到这里`grep`的准备工作也就结束了，下面就开始在文件中匹配模式串。

```c++
  bool status = true;
  do
    status &= grep_command_line_arg (*files++);
  while (*files != NULL);
```

我们可以看到这是按照一个一个文件来的，直到文件全部匹配完才结束。当我们进入`grep_command_line_arg`可以看到该函数会先将参数与“-”进行比较。

```c++
static bool
grep_command_line_arg (char const *arg)
{
  if (STREQ (arg, "-"))
    {
      filename = label;
      if (binary)
        xset_binary_mode (STDIN_FILENO, O_BINARY);
      return grepdesc (STDIN_FILENO, true);
    }
  else
    {
      filename = arg;
      return grepfile (AT_FDCWD, arg, true, true);
    }
}
```

因为我们的参数是一个文件名，也就会进入`grepfile`，在`grepfile`的最后也是会去调用`grepdesc`。

```c++
static bool
grepfile (int dirdesc, char const *name, bool follow, bool command_line)
{
  int oflag = (O_RDONLY | O_NOCTTY
               | (IGNORE_DUPLICATE_BRANCH_WARNING
                  (binary ? O_BINARY : 0))
               | (follow ? 0 : O_NOFOLLOW)
               | (skip_devices (command_line) ? O_NONBLOCK : 0));
  int desc = openat_safer (dirdesc, name, oflag);
  if (desc < 0)
    {
      if (follow || ! open_symlink_nofollow_error (errno))
        suppressible_error (errno);
      return true;
    }
  return grepdesc (desc, command_line);
}
```

`grepfile`是以只读写方式打开文件的。

在`grepdesc`中我们可以看到`grep`也是通过`S_ISDIR`来判断`desc`是不是目录，如果是目录，就会通过`goto`进行跳转。之后，会调用`grep`函数。

```c++
...
count = grep (desc, &st, &ineof);
...
```

下面是`grep`函数中大致流程：

```c++
static intmax_t
grep (int fd, struct stat const *st, bool *ineof)
{
  ...
  if (! reset (fd, st))
    return 0;

  ...
  if (! fillbuf (save, st))
    {
      suppressible_error (errno);
      return 0;
    }

  offset_width = 0;
  if (align_tabs)
    {
      /* Width is log of maximum number.  Line numbers are origin-1.  */
      uintmax_t num = usable_st_size (st) ? st->st_size : UINTMAX_MAX;
      num += out_line && num < UINTMAX_MAX;
      do
        offset_width++;
      while ((num /= 10) != 0);
    }

  for (bool firsttime = true; ; firsttime = false)
    {
      if (nlines_first_null < 0 && eol && binary_files != TEXT_BINARY_FILES
          && (buf_has_nulls (bufbeg, buflim - bufbeg)
              || (firsttime && file_must_have_nulls (buflim - bufbeg, fd, st))))
        {
          if (binary_files == WITHOUT_MATCH_BINARY_FILES)
            return 0;
          if (!count_matches)
            done_on_match = out_quiet = true;
          nlines_first_null = nlines;
          nul_zapper = eol;
          skip_nuls = skip_empty_lines;
        }

      lastnl = bufbeg;
      if (lastout)
        lastout = bufbeg;

      beg = bufbeg + save;

      if (beg == buflim)
        {
          *ineof = true;
          break;
        }

      zap_nuls (beg, buflim, nul_zapper);

      oldc = beg[-1];
      beg[-1] = eol;
      
      lim = memrchr (beg - 1, eol, buflim - beg + 1);
      ++lim;
      beg[-1] = oldc;
      if (lim == beg)
        lim = beg - residue;
      beg -= residue;
      residue = buflim - lim;

      if (beg < lim)
        {
          if (outleft)
            nlines += grepbuf (beg, lim);
          if (pending)
            prpending (lim);
          if ((!outleft && !pending)
              || (done_on_match && MAX (0, nlines_first_null) < nlines))
            goto finish_grep;
        }

      i = 0;
      beg = lim;
      while (i < out_before && beg > bufbeg && beg != lastout)
        {
          ++i;
          do
            --beg;
          while (beg[-1] != eol);
        }

      if (beg != lastout)
        lastout = 0;

      save = residue + lim - beg;
      if (out_byte)
        totalcc = add_count (totalcc, buflim - bufbeg - save);
      if (out_line)
        nlscan (beg);
      if (! fillbuf (save, st))
        {
          suppressible_error (errno);
          goto finish_grep;
        }
    }
  if (residue)
    {
      *buflim++ = eol;
      if (outleft)
        nlines += grepbuf (bufbeg + save - residue, buflim);
      if (pending)
        prpending (buflim);
    }
  ...
  return nlines;
}
```

在`grep`中，会先调用`reset`来重置新文件的缓冲区，并在第一次调用时进行初始化。

再调用`fillbuf`将新的内容填入缓冲区。

```c++
static bool
fillbuf (size_t save, struct stat const *st)
{
  ...
  if (pagesize <= buffer + bufalloc - sizeof (uword) - buflim)
    {
      readbuf = buflim;
      bufbeg = buflim - save;
    }
  else
    {
      ...
      for (newsize = bufalloc - pagesize - sizeof (uword);
           newsize < minsize;
           newsize *= 2)
        if ((SIZE_MAX - pagesize - sizeof (uword)) / 2 < newsize)
          xalloc_die ();
      
      if (usable_st_size (st))
        {
          off_t to_be_read = st->st_size - bufoffset;
          off_t maxsize_off = save + to_be_read;
          if (0 <= to_be_read && to_be_read <= maxsize_off
              && maxsize_off == (size_t) maxsize_off
              && minsize <= (size_t) maxsize_off
              && (size_t) maxsize_off < newsize)
            newsize = maxsize_off;
        }
      
      newalloc = newsize + pagesize + sizeof (uword);

      newbuf = bufalloc < newalloc ? xmalloc (bufalloc = newalloc) : buffer;
      readbuf = ALIGN_TO (newbuf + 1 + save, pagesize);
      bufbeg = readbuf - save;
      memmove (bufbeg, buffer + saved_offset, save);
      bufbeg[-1] = eolbyte;
      if (newbuf != buffer)
        {
          free (buffer);
          buffer = newbuf;
        }
    }

  clear_asan_poison ();

  readsize = buffer + bufalloc - sizeof (uword) - readbuf;
  readsize -= readsize % pagesize;

  while (true)
    {
      fillsize = safe_read (bufdesc, readbuf, readsize);
      if (fillsize == SAFE_READ_ERROR)
        {
          fillsize = 0;
          cc = false;
        }
      bufoffset += fillsize;

      if (((fillsize == 0) | !skip_nuls) || !all_zeros (readbuf, fillsize))
        break;
      totalnl = add_count (totalnl, fillsize);
      ...
    }

  buflim = readbuf + fillsize;

  memset (buflim, 0, sizeof (uword));

  asan_poison (buflim + sizeof (uword),
               bufalloc - (buflim - buffer) - sizeof (uword));

  return cc;
}
```

我们可以看到是通过`safe_read`来读取文件信息的。完成后，`bufbeg`指向缓冲区内容的开头，而`buflim`则指向缓冲区内容的末尾。如果有错误返回false。

回到`grep`函数代码中，通过`zap_nuls`来确定新的剩余部分(缓冲区末尾的不完整行长度，0表示没有不完整的最后一行)，再通过`memrchr`来获取新的终止符（`eol`）的位置。

```c++
... 
if (beg < lim)
 {
 	if (outleft)
 	nlines += grepbuf (beg, lim);
 	if (pending)
 		prpending (lim);
 	if ((!outleft && !pending)
 			|| (done_on_match && MAX (0, nlines_first_null) < nlines))
 		goto finish_grep;
 }
...
```

之后会调用`grepbuf`来匹配缓冲区的内容，并返回匹配到的行数。

```c++
static intmax_t
grepbuf (char *beg, char const *lim)
{
  intmax_t outleft0 = outleft;
  char *endp;

  for (char *p = beg; p < lim; p = endp)
    {
      size_t match_size;
      size_t match_offset = execute (compiled_pattern, p, lim - p,
                                     &match_size, NULL);
      if (match_offset == (size_t) -1)
        {
          if (!out_invert)
            break;
          match_offset = lim - p;
          match_size = 0;
        }
      char *b = p + match_offset;
      endp = b + match_size;
      /* Avoid matching the empty line at the end of the buffer. */
      if (!out_invert && b == lim)
        break;
      if (!out_invert || p < b)
        {
          char *prbeg = out_invert ? p : b;
          char *prend = out_invert ? b : endp;
          prtext (prbeg, prend);
          if (!outleft || done_on_match)
            {
              if (exit_on_match)
                exit (errseen ? exit_failure : EXIT_SUCCESS);
              break;
            }
        }
    }

  return outleft0 - outleft;
}
```

通过`prtext`来输出。

```c++
static void
prtext (char *beg, char *lim)
{
  static bool used;	/* Avoid printing SEP_STR_GROUP before any output.  */
  char eol = eolbyte;

  if (!out_quiet && pending > 0)
    prpending (beg);

  char *p = beg;

  if (!out_quiet)
    {
      /* Deal with leading context.  */
      char const *bp = lastout ? lastout : bufbeg;
      intmax_t i;
      for (i = 0; i < out_before; ++i)
        if (p > bp)
          do
            --p;
          while (p[-1] != eol);

      /* Print the group separator unless the output is adjacent to
         the previous output in the file.  */
      if ((0 <= out_before || 0 <= out_after) && used
          && p != lastout && group_separator)
        {
          pr_sgr_start_if (sep_color);
          fputs_errno (group_separator);
          pr_sgr_end_if (sep_color);
          putchar_errno ('\n');
        }

      while (p < beg)
        {
          char *nl = rawmemchr (p, eol);
          nl++;
          prline (p, nl, SEP_CHAR_REJECTED);
          p = nl;
        }
    }

  intmax_t n;
  if (out_invert)
    {
      /* One or more lines are output.  */
      for (n = 0; p < lim && n < outleft; n++)
        {
          char *nl = rawmemchr (p, eol);
          nl++;
          if (!out_quiet)
            prline (p, nl, SEP_CHAR_SELECTED);
          p = nl;
        }
    }
  else
    {
      /* Just one line is output.  */
      if (!out_quiet)
        prline (beg, lim, SEP_CHAR_SELECTED);
      n = 1;
      p = lim;
    }

  after_last_match = bufoffset - (buflim - p);
  pending = out_quiet ? 0 : MAX (0, out_after);
  used = true;
  outleft -= n;
}
```

最后调用`prline`函数输出匹配到的行，输出完之后返回到`grepbuf`的循环中继续执行，直到所有的内容都搜索完毕，返回到`grep`。

## 三、总结

在调试过程中，我们能够发现`grep`在传参时，就会把`*.txt`解析出来，而不是自己调用系统函数去寻找文件。这和我们自己写的文本搜索工具有很大区别，我们是自己去调用系统函数，将结果写入一个文档中，在读取这个文档，从而获得所有的文件路径。

其次，`grep`执行的过程和我们也有点不同。首先，都是进行`getopt`参数分析，之后`grep`是处理模式串，处理文件名，在进行匹配，输出。而我则是先搜索文件名，在调用匹配，匹配前处理模式串，在输出结果。输出的方式也有所不同，`grep`在成功匹配到一行后先输出一行，而我是将成功匹配的那一行先放到缓冲区后在输出。