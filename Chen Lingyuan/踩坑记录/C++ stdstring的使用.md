#                                         C++ std:string的使用

因为使用char类型的数组处理字符串非常麻烦，很多时候对于字符串的插入，删除，修改，查找都非常困难，同时还要担心是否越界，所以C++ 才有了string类来帮我们处理字符串。涉及到string的API非常多，而且功能强大。先列出几个比较常用的API。

## string的构造函数

string类有个默认的构造函数，这个函数的作用就是把string初始化为一个空字符串

例如：

```
string  s
```

同时也衍生开去好几种类型的构造函数

string(const char *s) ：将 string 对象初始化为 s 指向的字符串

例如： 

```
string s("hello")
```

basic_string(_CRT_GUARDOVERFLOW const size_type _Count, const _Elem _Ch)  ,创建一个包含-Count个元素的string对象，每个元素都被初始化字符_Ch。

例如 ：

```
string str(10,'a')
```

basic_string(basic_string&& _Right)  将一个 string 对象初始化为 string 对象 str

例如：

```
string str1("hello!");
string str2(str1);
```



 basic_string(_Iter _First, _Iter _Last, const _Alloc& _Al = _Alloc())，将string对象初始化为区间[begin,end)内的字符，其中begin和end为迭代器用于指定位置，范围包括begin，但是不包括end

例如：

```
char alls[] = "All's well that ends well";
string s(alls+6,alls+10);
cout<<s // well
```

## string的拼接

1.使用'+'运算重载符号拼接字符串

例如：

```
string s1("hello");
string s2("world!");
string s3 = str1 + str2;
```

2.使用 '+='重载运算符在字符串后面附加内容

```
string s1("hello");
string s2("world");

s1+=s2;
s1+='a';

```

3.可以使用 string.append() 函数来在一个 string 对象后面附加一个 string 对象或字符数组：

```
string s1="hello";
string s2="world";
s1.append(s1);
s1.append("world");
```

4.使用string.push_back()函数

可以使用 string.push_back() 函数来在一个 string 对象后面附加一个字符：

```
string str("Hello");
str.push_back('w');
```

String对象的比较

对于string对象的比较相比于C语言中的strcmp函数来说要简单不少，可以直接使用关系运算符 '==' ,'>=','<='等比较常见的运算比较符即可。

```
string s1("hello");
string s2("world");
if(s1==s2)
{
  printf("s1 == s2");
}
else if(s1>s2)
{
   printf("s1 > s2");
}
else
{
   printf("s1 < s2");
}
```

## 截取字符串

basic_string substr( size_type pos = 0, size_type count = npos )，pos是字符串的起始位置，len是要截取的字串的长度，这个函数的功能是从原来的string对象中的pos处开始的len个字符复制到新的string对象。

```
string s1("hello");
string s2=s1.substr(0,2);
```

## String对象的查找操作

size_type find( const basic_string& str, size_type pos = 0 )，从字符串的pos处查找string类型的子串，若不指定默认为0，如果找到，返回该子串首次出现时其首字符的索引，否则返回string::npos

```
string s("hello hello");
string p("he");
int n=s.find(p);
int m=s.find(p,2);
```

size_type find( const CharT* s, size_type pos = 0 )，从字符串的pos处查找指针s指向的字符串，若不指定默认为0，如果找到，返回该子串首次出现时其首字符的索引，否则返回string::pos。

```
string s("hello hello");
char p[]="he";
int n=s.find(p);
int m=s.find(p,2);
```

size_type find( const CharT* s, size_type pos, size_type count ),从字符串的 pos 位置开始查找 s 的前 count 个字符组成的子字符串。如果找到，则返回该子字符串首次出现时其首字符的索引；否则，返回 string::npos：

```
string s("helloworld");
char p[]="wo";
int n=s.find(p,0,5)
```

size_type find( CharT ch, size_type pos = 0 )，从字符串的pos处查找字符串ch，若不指定默认为0，如果找到，返回该字符首次出现时的所有，否则赶回string::npos

```
string s("hello world");
char p='o';
int n=s.find(p,5);
int m=s.find(p);
```

## string的插入操作

basic_string& insert( size_type index, const CharT* s );，在string对象的index处插入s指针指向的字符串。

```
string s1("helloworld");
chat s2={"he"};
s.insert(1,s2);
cout<<s; //hheelloworld
```

basic_string& insert( size_type index, const CharT* s, size_type count );在string对象的index处插入s的前count字符串。

```
string s1("helloworld");
chat s2={"he"};
s.insert(1,s2，1);
cout<<s;   //hhelloworld

```

basic_string& insert( size_type index, const basic_string& str );在string对象的index处插入str

```
string s1("hello");
string s2("world");
s2.insert(0,s1);
cout<<s2; //helloworld
```

basic_string& insert( size_type index, const basic_string& str,size_type index_str, size_type count );在原string的index处插入常量str的从 index_str开始的count个字符

```
string s1 = "~~world";
string s2 = "hello~";
s2.insert(6,s1,2,5);
cout<<s2<<endl;  //hello~world
```

## string字符串的元素删除

basic_string& erase( size_type index = 0, size_type count = npos );删除从pos开始的count个字符

```
    std::string s2 = "hello";
    s2.erase(0, 2);
    printf("%s", s2.c_str()); // llo
```

