#include "KParameterAnalysis.h"

#include <stdio.h>
#include <string>

#include "KSearch.h"

KParameterAnalysis::KParameterAnalysis()
{

}

KParameterAnalysis::~KParameterAnalysis()
{

}

bool KParameterAnalysis::CmdOptionExists(int argc, char* argv[])
{
    bool     bResult  = false;
    int      nRetCode = 0;
    int      nKeyLen  = 0;
    KSearch* pSearch  = NULL;
    char     szKeyWord[MAX_PATH_LEN];
    char     szFilePath[MAX_PATH_LEN];

    KGLOG_PROCESS_ERROR(argc);

    if (argc == 1)
    {
        printf("usage: SearchKeyWord KeyWord LogPath\n");
        printf("sample: SearchKeyWord KGLOG ..\\Debug\\testLog\\NormalFile.log\n");
        printf("\nexplain:\n");
        printf("Two parameters, the first for the query keyword, the second for the query text path\n");
    }
    else if (argc == 3)
    {
        KGLOG_PROCESS_ERROR(argv);

        KGLOG_PROCESS_ERROR(argv[1][0]);
        KGLOG_PROCESS_ERROR(argv[2][0]);

        strncpy(szKeyWord, argv[1], sizeof(szKeyWord));
        szKeyWord[sizeof(szKeyWord) - 1] = '\0';

        nKeyLen = strnlen(szKeyWord, sizeof(szKeyWord));

        strncpy(szFilePath, argv[2], sizeof(szFilePath));
        szFilePath[sizeof(szFilePath) - 1] = '\0';

        pSearch = new KSearch();
        KGLOG_PROCESS_ERROR(pSearch);

        nRetCode = pSearch->Init(
            nKeyLen, szKeyWord, szFilePath
        );
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = pSearch->RunSunday();
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    bResult = true;
Exit0:
    if (pSearch)
    {
        delete pSearch;
        pSearch = NULL;
    }

    return bResult;
}