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
    char     szResultPath[MAX_PATH_LEN];

    KGLOG_PROCESS_ERROR(argc);

    if (argc == 1)
    {
        printf("用法: SearchKeyWord KeyWord LogPath ResultPath\n");
        printf("样例: SearchKeyWord KGLOG ..\\Debug\\testLog\\NormalFile.log ..\\Debug\\SearchKeyWordResult.log\n");
        printf("\n说明：\n");
        printf("三个参数，第一个为查询关键字，第二个为查询文本的路径 第三个是结果存放的路径\n");
    }
    else
    {
        KGLOG_PROCESS_ERROR(argc == 4);
        KGLOG_PROCESS_ERROR(argv);

        KGLOG_PROCESS_ERROR(argv[1][0]);
        KGLOG_PROCESS_ERROR(argv[2][0]);
        KGLOG_PROCESS_ERROR(argv[3][0]);

        strncpy(szKeyWord, argv[1], sizeof(szKeyWord));
        szKeyWord[sizeof(szKeyWord) - 1] = '\0';

        nKeyLen = strnlen(szKeyWord, sizeof(szKeyWord));

        strncpy(szFilePath, argv[2], sizeof(szFilePath));
        szFilePath[sizeof(szFilePath) - 1] = '\0';

        strncpy(szResultPath, argv[3], sizeof(szResultPath));
        szResultPath[sizeof(szResultPath) - 1] = '\0';

        pSearch = new KSearch();
        KGLOG_PROCESS_ERROR(pSearch);

        nRetCode = pSearch->Init(
            nKeyLen, szKeyWord, szFilePath, szResultPath
        );
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = pSearch->RunBigFile();
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