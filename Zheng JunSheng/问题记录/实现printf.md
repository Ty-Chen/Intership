# 简单实现`printf`

## 1.简介

在学习后`printf`后，想自己实现一个简单的`printf`。

## 2.实现步骤

我们常用的可变参数列表有这几个：

### 2.1`va_list`

```c
typedef char* va_list; 
```

`va_list`为char\*类型重定义，所以`va_list`为一个指向char类型的指针(`va_list p`就等同于 char \*p)。

### 2.2`va_start(ap,v)`

```c
#define va_start _crt_va_start
#define _crt_va_start(ap,v) (ap=(va_list)_ADDRESSOF(v)+ _INTSIZEOF(V))
```

把`v`的地址强转为`va_list`类型即char\* ,把其移动`_INTSIZEOF(V)`个字节后的地址赋值给`ap`，其实就是让`ap`跳过第一个参数，指向"..."里的第一个可变参数。

### 2.3`va_arg(ap,t)`

```c
#define va_arg _crt_va_arg
#define _crt_va_arg(ap,t) ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
```

将`ap`移动`_INTSIZEOF(t)`个字节后，然后再取出`ap`未移动前所指向位置对应的数据。

下面假使t为一个`int`型变量，如下图分析：

![img](./实现printf.assets/1272978-20171122173538774-669180204.png)

### 2.4`va_end(ap)`

```c
#define va_end _crt_va_end
#define _crt_va_end(ap) ( ap = (va_list)0 ) 
```

将0强转为`va_list`类型,并赋值给`ap`,使其置空

## 3.实现代码

```c
#include<stdio.h>
#include<assert.h>  
#include<stdarg.h>

void putInt(int n) {
    if (n > 9) {
        putInt(n / 10);
    }
    putchar(n % 10 + '0');
}

int MyPrintf(const char *pszPattern, ...)
{
    assert(pszPattern);
    va_list arg;
    va_start(arg, pszPattern);
    const char *pStart = pszPattern;
    while (*pStart != '\0')
    {
        if (*pStart == '%') {
            pStart++;
            switch (*pStart)
            {
            case 'd':
                putInt(va_arg(arg, int));
                break;
            case 'c':
                putchar(va_arg(arg, int));
                break;
            case 's':
            {
                char* pzChar = va_arg(arg, char*);
                while (*pzChar)
                {
                    putchar(*pzChar);
                    pzChar++;
                }
            }
                break;
            case 'f':
            {
                float a = (float)va_arg(arg, double);
                printf("%f", a);
            }
                break;
            default:
                break;
            }
        }
        else {
            putchar(*pStart);
        }
        pStart++;
    }
    va_end(arg);
    return 0;
}

int main()
{
    char szStr[] = "hello world!";
    MyPrintf("Output:>%f  %c%c %d %s", 3.14, 't', 'p', 1234, szStr);
    return 0;
}
/*
输出结果：
Output:>3.140000  tp 1234 hello world!
*/
```

## 4.总结

`printf`的实现很复杂，这里只是简单的实现了一下。