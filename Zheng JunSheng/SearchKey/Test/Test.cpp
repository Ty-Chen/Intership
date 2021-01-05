#include <stdio.h>
#include <string.h>

#include <string>

#include "KParameterAnalysis.h"

int main(int argc, char* argv[])
{
    bool                bResult            = false;
    int                 nRetCode           = 0;
    KParameterAnalysis* pParameterAnalysis = NULL;

    KGLOG_PROCESS_ERROR(argc);
    KGLOG_PROCESS_ERROR(argv);

    pParameterAnalysis = new KParameterAnalysis();
    KGLOG_PROCESS_ERROR(pParameterAnalysis);

    nRetCode = pParameterAnalysis->GetOption(argc, argv);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    if (pParameterAnalysis)
    {
        delete pParameterAnalysis;
        pParameterAnalysis = NULL;
    }

    return bResult;
}
