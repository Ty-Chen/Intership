﻿#include <stdio.h>
#include <string.h>

#include "KParameterAnalysis.h"

int main(int argc, char* argv[])
{
    bool                bResult            = false;
    int                 nRetCode           = 0;
    KParameterAnalysis* pParameterAnalysis = NULL;

    pParameterAnalysis = new KParameterAnalysis();
    KGLOG_PROCESS_ERROR(pParameterAnalysis);

    nRetCode = pParameterAnalysis->CmdOptionExists(argc, argv);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:

    return bResult;
}
