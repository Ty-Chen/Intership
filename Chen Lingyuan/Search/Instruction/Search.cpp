// Instruction.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include<string>
#include<time.h>
#include<vector>

#include"KOperation.h"
#include"KMacro.h"

int main(int argc, char* argv[])
{
    KOperation* pOperation = NULL;
    bool         bRetCode  = false;


    pOperation = new KOperation();
    KGLOG_PROCESS_ERROR(pOperation);

    pOperation->Init();

    bRetCode = pOperation->CommandInput(argc, argv);
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    if (pOperation)
    {
        delete pOperation;
        pOperation = NULL;
    }
    return 0;
}
