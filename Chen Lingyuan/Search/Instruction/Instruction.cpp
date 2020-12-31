// Instruction.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include<string>

#include"KOperation.h"
#include"KMacro.h"

int main()
{
    char s[8] = "abcdefg";
    char* p = s;
    p+=1;
    memmove(p, s, 4);
    printf("%s\n", p);

    KOperation* pOperation = NULL;

    pOperation = new KOperation();
    KGLOG_PROCESS_ERROR(pOperation);

    pOperation->Init();

    pOperation->CommandInput();

Exit0:
    if (pOperation)
    {
        delete pOperation;
        pOperation = NULL;
    }
    return 0;
}
