#include "KSearchTest.h"

#include <time.h>
#include <stdio.h>
#include <string.h>

#include "KBM.h"
#include "KSearch.h"
#include "KProcessState.h"

KSearchTest::KSearchTest()
{

}

KSearchTest::~KSearchTest()
{

}

bool KSearchTest::Run(char* pszTestKeyWordPath, char* pszTestTxtPath)
{
    bool  bResult     = false;
    int   nRetCode    = 0;
    int   nCntTest    = 0;
    int   nCntPath    = 0;
    int   nCntKeyWord = 0;
    char* pszParameter[4];
    char  szResultPath[MAX_PATH_LEN];
    char  szLogType[TEST_MAX_NUM][MAX_PATH_LEN];
    char  szTestLogPath[TEST_MAX_NUM][MAX_PATH_LEN];
    char  szTestKeyWord[TEST_MAX_NUM][MAX_PATH_LEN];

    KGLOG_PROCESS_ERROR(pszTestKeyWordPath);
    KGLOG_PROCESS_ERROR(pszTestTxtPath);

    nRetCode = GetTestInfo(pszTestKeyWordPath, szTestKeyWord, nCntKeyWord);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = GetTestInfo(pszTestTxtPath, szTestLogPath, nCntPath);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = GetTestType(szTestLogPath, nCntPath, szLogType);
    KGLOG_PROCESS_ERROR(nRetCode);

    memset(pszParameter, NULL, sizeof(pszParameter));

    for (int i = 0; i < nCntKeyWord; ++i)
    {
        for (int j = 0; j < nCntPath; ++j)
        {
            ++nCntTest;

            snprintf(szResultPath, sizeof(szResultPath),
                "..\\Debug\\resultLog\\%s\\%s.log",
                szLogType[j], szTestKeyWord[i]
            );
            szResultPath[sizeof(szResultPath) - 1] = '\0';

            pszParameter[1] = szTestKeyWord[i];
            pszParameter[2] = szTestLogPath[j];
            pszParameter[3] = szResultPath;

            nRetCode = TestKSearch(4, pszParameter);
            if (nRetCode)
            {
                printf("test%d run result = true\n\n", nCntTest);
            }
            else
            {
                printf("test%d run result = fail\n\n", nCntTest);
            }
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

bool KSearchTest::GetFile(char* pszFilePath, char*& pszText, int& nTextLen)
{
    bool  bResult  = false;
    int   nRetCode = 0;
    FILE* fpFile   = NULL;

    KGLOG_PROCESS_ERROR(pszText == NULL);
    KGLOG_PROCESS_ERROR(nTextLen == 0);

    fpFile = fopen(pszFilePath, "rb");
    KGLOG_PROCESS_ERROR(fpFile);

    nRetCode = fseek(fpFile, 0, SEEK_END);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    nTextLen = ftell(fpFile);
    KGLOG_PROCESS_ERROR(nTextLen >= 0);

    nRetCode = fseek(fpFile, 0, SEEK_SET);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    pszText = new char[nTextLen + 1];
    KGLOG_PROCESS_ERROR(pszText);

    nRetCode = fread(pszText, nTextLen, 1, fpFile);
    KGLOG_PROCESS_ERROR(nRetCode == 1);
    pszText[nTextLen] = '\0';

    bResult = true;
Exit0:
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }

    if (!bResult)
    {
        if (pszText)
        {
            delete []pszText;
            pszText = NULL;
        }
    }

    return bResult;
}

bool KSearchTest::GetTestInfo(char* pszTestInfoPath, char szTestInfo[][MAX_PATH_LEN], int& nCntTest)
{
    bool  bResult  = false;
    int   nRetCode = 0;
    int   nTxtLen  = 0;
    char* pszTxt   = NULL;

    KGLOG_PROCESS_ERROR(pszTestInfoPath);
    KGLOG_PROCESS_ERROR(szTestInfo);
    KGLOG_PROCESS_ERROR(nCntTest == 0);

    nRetCode = GetFile(pszTestInfoPath, pszTxt, nTxtLen);
    KGLOG_PROCESS_ERROR(nRetCode);

    for (int i = 0, nPathpos = 0; i < nTxtLen; ++i)
    {
        if (pszTxt[i] == '\n')
        {
            szTestInfo[nCntTest][nPathpos++] = '\0';
            ++nCntTest;
            nPathpos = 0;
            continue;
        }

        if (pszTxt[i] == '\r')
        {
            continue;
        }

        szTestInfo[nCntTest][nPathpos++] = pszTxt[i];

        if (i == nTxtLen - 1)
        {
            szTestInfo[nCntTest][nPathpos++] = '\0';
            ++nCntTest;
            nPathpos = 0;
            continue;
        }
    }

    bResult = true;
Exit0:
    if (pszTxt)
    {
        delete []pszTxt;
        pszTxt = NULL;
    }

    return bResult;
}

bool KSearchTest::TestKSearch(int nParameter, char* pszParameter[])
{
    bool               bResult             = false;
    int                nRetCode            = 0;
    int                nKeyLen             = 0;
    float              fRunTime            = 0.0;
    FILE*              fpFile              = NULL;
    clock_t            nEndTime            = 0;
    clock_t            nBeginTime          = 0;
    KSearch*           pSearch             = NULL;
    KProcessState*     pProcessState       = NULL;
    unsigned long long nPagefileUsage      = 0;
    unsigned long long nWorkingSetSize     = 0;
    unsigned long long nPeakWorkingSetSize = 0;
    char               szKeyWord[MAX_PATH_LEN];
    char               szFilePath[MAX_PATH_LEN];
    char               szResultPath[MAX_PATH_LEN];

    nBeginTime = clock();

    KGLOG_PROCESS_ERROR(nParameter == 4);
    KGLOG_PROCESS_ERROR(pszParameter);

    KGLOG_PROCESS_ERROR(pszParameter[1][0]);
    KGLOG_PROCESS_ERROR(pszParameter[2][0]);
    KGLOG_PROCESS_ERROR(pszParameter[3][0]);

    strncpy(szKeyWord, pszParameter[1], sizeof(szKeyWord));
    szKeyWord[sizeof(szKeyWord) - 1] = '\0';

    nKeyLen = strnlen(szKeyWord, sizeof(szKeyWord));

    strncpy(szFilePath, pszParameter[2], sizeof(szFilePath));
    szFilePath[sizeof(szFilePath) - 1] = '\0';

    strncpy(szResultPath, pszParameter[3], sizeof(szResultPath));
    szResultPath[sizeof(szResultPath) - 1] = '\0';

    pSearch = new KSearch();
    KGLOG_PROCESS_ERROR(pSearch);

    nRetCode = pSearch->Init(
        nKeyLen, szKeyWord, szFilePath, szResultPath
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pSearch->RunSunday();
    KGLOG_PROCESS_ERROR(nRetCode);

    nEndTime = clock();
    fRunTime = (float)(nEndTime - nBeginTime) / CLOCKS_PER_SEC;

    pProcessState = new KProcessState();
    KGLOG_PROCESS_ERROR(pProcessState);

    nRetCode = pProcessState->GetMemoryUsage(&nWorkingSetSize, &nPeakWorkingSetSize, &nPagefileUsage);
    KGLOG_PROCESS_ERROR(nRetCode);

    fpFile = fopen(TEST_RESULT_PATH, "ab");
    KGLOG_PROCESS_ERROR(fpFile);

    nRetCode = fprintf(fpFile,
        "test:\nKeyWord = \"%s\" LogPath = \"%s\" ResultPath = \"%s\"\n"
        "spend time                  \t = %fs\n"
        "number of result statistics \t = %d\n"
        "nWorkingSetSize             \t = %lld byte\n"
        "nPeakWorkingSetSize         \t = %lld byte\n"
        "nVirtualMemory              \t = %lld byte\n",
        szKeyWord, szFilePath, szResultPath,
        fRunTime, pSearch->m_nCntResult,
        nWorkingSetSize, nPeakWorkingSetSize, nPagefileUsage
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }

    if (pSearch)
    {
        delete pSearch;
        pSearch = NULL;
    }

    return bResult;
}

bool KSearchTest::GetTestType(
    char szTestInfo[][MAX_PATH_LEN], int nCntTest, char szTestType[][MAX_PATH_LEN]
)
{
    bool bResult   = false;
    int  nInfoLen  = 0;
    int  nStartPos = 0;

    KGLOG_PROCESS_ERROR(szTestInfo);
    KGLOG_PROCESS_ERROR(nCntTest);
    KGLOG_PROCESS_ERROR(szTestType);

    for (int i = 0; i < nCntTest; ++i)
    {
        nInfoLen = strlen(szTestInfo[i]);
        KGLOG_PROCESS_ERROR(nInfoLen);

        for (int j = nInfoLen - 1; j >= 0; --j)
        {
            if (szTestInfo[i][j] == '\\')
            {
                nStartPos = j;
                break;
            }
        }

        memset(szTestType[i], '\0', sizeof(szTestType[i]));

        for (int j = nStartPos + 1, k = 0; j < nInfoLen; ++j, ++k)
        {
            if (szTestInfo[i][j] == '.')
            {
                break;
            }

            szTestType[i][k] = szTestInfo[i][j];
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

bool KSearchTest::CompareFile(char* pszResultPath, char* pszGrepResultPath)
{
    bool  bCompare     = true;
    int   nGrepLen     = 0;
    int   nResultLen   = 0;
    char* pszGrep      = NULL;
    char* pszResult    = NULL;
    FILE* fpGrepFile   = NULL;
    FILE* fpResultFile = NULL;

    KGLOG_PROCESS_ERROR(pszResultPath);
    KGLOG_PROCESS_ERROR(pszGrepResultPath);

    fpResultFile = fopen(pszResultPath, "rb");
    fpGrepFile = fopen(pszGrepResultPath, "rb");

    pszResult = new char[MAX_BUFFER];
    KGLOG_PROCESS_ERROR(pszResult);

    pszGrep = new char[MAX_BUFFER];
    KGLOG_PROCESS_ERROR(pszGrep);

    while ((feof(fpResultFile) || feof(fpGrepFile)) && bCompare)
    {
        nResultLen = fread(pszResult, 1, MAX_BUFFER, fpResultFile);
        KGLOG_PROCESS_ERROR(nResultLen >= 0);

        nGrepLen = fread(pszGrep, 1, MAX_BUFFER, fpGrepFile);
        KGLOG_PROCESS_ERROR(nGrepLen >= 0);

        if (nResultLen != nGrepLen)
        {
            bCompare = false;
        }

        for (int i = 0; i < nResultLen; ++i)
        {
            if (pszResult[i] != pszGrep[i])
            {
                bCompare = false;
                break;
            }
        }
    }

Exit0:
    if (pszGrep)
    {
        delete []pszGrep;
        pszGrep = NULL;
    }

    if (pszResult)
    {
        delete []pszResult;
        pszResult = NULL;
    }

    if (fpResultFile)
    {
        fclose(fpResultFile);
        fpResultFile = NULL;
    }

    if (fpGrepFile)
    {
        fclose(fpGrepFile);
        fpGrepFile = NULL;
    }

    return bCompare;
}