// Instruction.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include"KOperation.h"
#include"KMacro.h"

int main()
{
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
