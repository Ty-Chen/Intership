#include "KParameterAnalysis.h"

#include <stdio.h>
#include <string>

#include "KSearchTest.h"

KParameterAnalysis::KParameterAnalysis()
{

}

KParameterAnalysis::~KParameterAnalysis()
{

}

bool KParameterAnalysis::CmdOptionExists(int argc, char* argv[])
{
    bool         bResult = false;
    int          nRetCode = 0;
    KSearchTest* pSearchTest = NULL;

    KGLOG_PROCESS_ERROR(argc);

    if (argc == 1)
    {
        printf("用法: Test TestKeyWordPath TestLogPath\n");
        printf("样例: Test ..\\Debug\\testKey.txt ..\\Debug\\testLogPath.txt\n");
        printf("\n说明：\n");
        printf("两个参数，第一个为测试关键字所在文本的路径，第二个为测试文本路径所在文本的路径\n");
        printf("可以在..\\Debug\\testKey.txt中修改关键字， 可以在..\\Debug\\testLogPath.txt中修改测试文本路径\n");
    }
    else
    {
        KGLOG_PROCESS_ERROR(argc == 3);
        KGLOG_PROCESS_ERROR(argv);

        KGLOG_PROCESS_ERROR(argv[1]);
        KGLOG_PROCESS_ERROR(argv[2]);

        pSearchTest = new KSearchTest();
        KGLOG_PROCESS_ERROR(pSearchTest);

        nRetCode = pSearchTest->Run(argv[1], argv[2]);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    bResult = true;
Exit0:
    if (pSearchTest)
    {
        delete pSearchTest;
        pSearchTest = NULL;
    }

    return bResult;
}