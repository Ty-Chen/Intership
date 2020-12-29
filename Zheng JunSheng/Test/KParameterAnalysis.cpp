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
        printf("�÷�: Test TestKeyWordPath TestLogPath\n");
        printf("����: Test ..\\Debug\\testKey.txt ..\\Debug\\testLogPath.txt\n");
        printf("\n˵����\n");
        printf("������������һ��Ϊ���Թؼ��������ı���·�����ڶ���Ϊ�����ı�·�������ı���·��\n");
        printf("������..\\Debug\\testKey.txt���޸Ĺؼ��֣� ������..\\Debug\\testLogPath.txt���޸Ĳ����ı�·��\n");
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