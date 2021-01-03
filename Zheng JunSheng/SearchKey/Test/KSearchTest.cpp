#include "KSearchTest.h"

#include <stdio.h>
#include <string.h>

#include <iostream>

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
    int   nCntPath    = 0;
    int   nCntKeyWord = 0;
    char  szResultPath[MAX_PATH_LEN];
    char  szSystemCommand[MAX_PATH_LEN];
    char  szTestFileType[TEST_MAX_NUM][MAX_PATH_LEN];
    char  szTestFilePath[TEST_MAX_NUM][MAX_PATH_LEN];
    char  szTestKeyWord[TEST_MAX_NUM][MAX_PATH_LEN];

    KGLOG_PROCESS_ERROR(pszTestKeyWordPath);
    KGLOG_PROCESS_ERROR(pszTestTxtPath);

    nRetCode = GetTestInfo(pszTestKeyWordPath, szTestKeyWord, nCntKeyWord);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = GetTestInfo(pszTestTxtPath, szTestFilePath, nCntPath);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = GetTestFileType(szTestFilePath, nCntPath, szTestFileType);
    KGLOG_PROCESS_ERROR(nRetCode);

    for (int i = 0; i < nCntKeyWord; ++i)
    {
        for (int j = 0; j < nCntPath; ++j)
        {
            snprintf(szResultPath, sizeof(szResultPath),
                "..\\Debug\\resultLog\\%s\\%s.log",
                szTestFileType[j], szTestKeyWord[i]
            );

           snprintf(szSystemCommand, sizeof(szSystemCommand),
                "..\\Debug\\SearchKey.exe %s %s > %s",
               szTestKeyWord[i], szTestFilePath[j], szResultPath
            );

           system(szSystemCommand);
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
    if (!bResult)
    {
        if (pszText)
        {
            delete[]pszText;
            pszText = NULL;
        }
    }

    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }

    return bResult;
}

bool KSearchTest::GetTestInfo(
    char* pszTestInfoPath, char szTestInfo[][MAX_PATH_LEN],
    int& nCntTest
)
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

bool KSearchTest::GetTestFileType(
    char szTestFilePath[][MAX_PATH_LEN], int nCntTest,
    char szTestType[][MAX_PATH_LEN]
)
{
    bool bResult   = false;
    int  nInfoLen  = 0;
    int  nStartPos = 0;

    KGLOG_PROCESS_ERROR(szTestFilePath);
    KGLOG_PROCESS_ERROR(nCntTest);
    KGLOG_PROCESS_ERROR(szTestType);

    for (int i = 0; i < nCntTest; ++i)
    {
        nInfoLen = strlen(szTestFilePath[i]);
        KGLOG_PROCESS_ERROR(nInfoLen);

        for (int j = nInfoLen - 1; j >= 0; --j)
        {
            if (szTestFilePath[i][j] == '\\')
            {
                nStartPos = j;
                break;
            }
        }

        memset(szTestType[i], '\0', sizeof(szTestType[i]));

        for (int j = nStartPos + 1, k = 0; j < nInfoLen; ++j, ++k)
        {
            if (szTestFilePath[i][j] == '.')
            {
                break;
            }

            szTestType[i][k] = szTestFilePath[i][j];
        }
    }

    bResult = true;
Exit0:
    return bResult;
}