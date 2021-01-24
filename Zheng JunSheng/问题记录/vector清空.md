# vector清空

## 1.简介

在读取目录下的所有文件路径时，运用到了`vector`存文件路径，而在`vector.clear()`时发现`vector`并没有清空内存。

## 2.问题分析

首先我们要明确`vector`中`size`和`capacity`的区别。

`size` 是当前 `vector` 容器真实占用的大小，也就是容器当前拥有多少个元素。

`capacity` 是指在发生 `realloc` 前能允许的最大元素数，即预分配的内存空间。

### 2.1`std::vector::clear()`

英文描述：Removes all elements from the vector (which are destroyed), leaving the container with a size of 0. A reallocation is not guaranteed to happen, and the vector capacity is not guaranteed to change due to calling this function. A typical alternative that forces a reallocation is to use swap。

### 2.2`std::vector::swap()`

英文描述：Exchanges the content of the container by the content of x, which is another vector object of the same type. Sizes may differ.

After the call to this member function, the elements in this container are those which were in x before the call, and the elements of x are those which were in this. All iterators, references and pointers remain valid for the swapped objects.

Notice that a non-member function exists with the same name, swap, overloading that algorithm with an optimization that behaves like this member function.

## 3.问题解决方案

测试`std::vector::clear()`

```c
#include<iostream>
#include<vector>

int main()
{
    std::vector<int> TestVector;
    TestVector.push_back(1);
    TestVector.push_back(2);
    TestVector.push_back(3);
    TestVector.push_back(4);
    TestVector.push_back(5);
    printf("size: %d\n"
        "capacity: %d\n",
        TestVector.size(), TestVector.capacity()
    );

    TestVector.clear();

    printf("after clear size: %d\n"
        "after clear capacity: %d\n",
        TestVector.size(), TestVector.capacity()
    );

    return 0;
}
/*
输出：
size: 5
capacity: 6
after clear size: 0
after clear capacity: 6
*/
```
clear后，size变为了0，capacity没有变化。

测试`std::vector::swap()`

```c
#include <iostream>
#include <vector>

int main()
{
    std::vector<int> FirstVector;
    FirstVector.push_back(1);
    FirstVector.push_back(2);
    FirstVector.push_back(3);
    FirstVector.push_back(4);
    FirstVector.push_back(5);

    std::vector<int> SecondVector;
    SecondVector.push_back(1);
    SecondVector.push_back(2);

    printf("FirstVector size: %d\n"
        "FirstVector capacity: %d\n"
        "SecondVector size: %d\n"
        "SecondVector capacity: %d\n",
        FirstVector.size(), FirstVector.capacity(),
        SecondVector.size(), SecondVector.capacity()
    );

    FirstVector.swap(SecondVector);

    printf("after swap FirstVector size: %d\n"
        "after swap FirstVector capacity: %d\n"
        "after swap SecondVector size: %d\n"
        "after swap SecondVector capacity: %d\n",
        FirstVector.size(), FirstVector.capacity(),
        SecondVector.size(), SecondVector.capacity()
    );

    return 0;
}
/*
输出：
FirstVector size: 5
FirstVector capacity: 6
SecondVector size: 2
SecondVector capacity: 2
after swap FirstVector size: 2
after swap FirstVector capacity: 2
after swap SecondVector size: 5
after swap SecondVector capacity: 6
*/
```

swap之后，不仅仅是size变化了，capacity也是变化了。

使用swap来替代clear()。

```c
#include<iostream>
#include<vector>

int main()
{
    std::vector<int> TestVector;
    TestVector.push_back(1);
    TestVector.push_back(2);
    TestVector.push_back(3);
    TestVector.push_back(4);
    TestVector.push_back(5);
    printf("size: %d\n"
        "capacity: %d\n",
        TestVector.size(), TestVector.capacity()
    );

    std::vector<int>().swap(TestVector);

    printf("after clear size: %d\n"
        "after clear capacity: %d\n",
        TestVector.size(), TestVector.capacity()
    );

    return 0;
}
/*
输出：
size: 5
capacity: 6
after clear size: 0
after clear capacity: 0
*/
```

## 4.问题总结

如果想要真正清空vector的内存可以使用swap。交换技巧实现内存释放思想：vector()使用vector的默认构造函数建立临时vector对象，再在该临时对象上调用swap成员，swap调用 之后对象vector占用的空间就等于一个默认构造的对象的大小，临时对象就具有原来对象vector的大小，而该临时对象随即就会被析构，从而其占用的空间也被释放。当然，在程序结束时，vector也会自动析构。

如果只想清空vector的元素使用clear即可。