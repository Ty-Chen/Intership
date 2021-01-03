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
    bool         bResult     = false;
    int          nRetCode    = 0;
    KSearchTest* pSearchTest = NULL;

    KGLOG_PROCESS_ERROR(argc);

    if (argc == 1)
    {
        printf("usage: Test TestKeyWordPath TestLogPath\n");
        printf("sample: Test ..\\Debug\\testKey.txt ..\\Debug\\testLogPath.txt\n");
        printf("\nexplain:\n");
        printf("Two parameters, the first is the path of the text in which the test keyword is located,"
            " the second is the path of the text in which the test text path is located\n"
        );
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