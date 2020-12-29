#include "KMatchTest.h"

#include<Windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#include<string>

#include"Kmacro.h"

KMatchTest::KMatchTest()
{

}

KMatchTest::~KMatchTest()
{
    UnInit();
}

bool KMatchTest::Init(
    char* pszKeyWordsDirectory, char* pszTestDirectory,
     char* pszMatchResultDirectory, char* pszContrastDirectory
)
{
    bool bResult  = false;
    bool bRetCode = false;

    KGLOG_PROCESS_ERROR(pszKeyWordsDirectory);
    KGLOG_PROCESS_ERROR(pszTestDirectory);
    KGLOG_PROCESS_ERROR(pszMatchResultDirectory);
    KGLOG_PROCESS_ERROR(pszContrastDirectory);

    m_szTestFileList.clear();
    m_szKeyWordsList.clear();
    m_szMatchResultFileList.clear();
    m_szContrastFileList.clear();

    m_szFullContrastPath = "";
    m_szFullResultPath = "";

    bRetCode = LoadDirectory(pszTestDirectory, m_szTestFileList);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = LoadDirectory(pszKeyWordsDirectory, m_szKeyWordsList);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = LoadDirectory(pszMatchResultDirectory, m_szMatchResultFileList);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = LoadDirectory(pszContrastDirectory, m_szContrastFileList);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    if (!bResult)
    {
        m_szTestFileList.clear();
        m_szKeyWordsList.clear();
        m_szMatchResultFileList.clear();
        m_szContrastFileList.clear();
    }
    return bResult;
}

void KMatchTest::UnInit()
{
    m_szTestFileList.clear();
    m_szKeyWordsList.clear();
    m_szMatchResultFileList.clear();
    m_szContrastFileList.clear();
}

bool KMatchTest::Match()
{
    bool                    bResult           = false;
    bool                    bRetCode          = false;
    int                     nMatchCount       = 0;
    long long               llFileSize        = 0;
    clock_t                 nStart            = 0;
    clock_t                 nEnd              = 0;
    HANDLE                  hHandle;
    PROCESS_MEMORY_COUNTERS PMC;
    FILE*                   fpResultStatistic = NULL;
    KBlockMatch*            pBlockMatch       = NULL;

    pBlockMatch = new KBlockMatch();
    KGLOG_PROCESS_ERROR(pBlockMatch);

    bRetCode = pBlockMatch->Init();
    KGLOG_PROCESS_ERROR(bRetCode);

    fpResultStatistic = fopen(MATCH_RESULT, "a");
    KGLOG_PROCESS_ERROR(fpResultStatistic);

    for (unsigned int i = 0; i < m_szKeyWordsList.size(); i++)
    {
        fprintf(fpResultStatistic, "Pattern : %s\n", m_szKeyWordsList[i].c_str());
        if (m_szKeyWordsList[i].length() > PATTERN_SIZE)
        {
            fprintf(fpResultStatistic, "Pattern is too Large\n\n");
            continue;
        }

        for (unsigned int j = 0; j < m_szTestFileList.size(); j++)
        {
            nStart = clock();

            bRetCode = GetFullPath(
                (char*)m_szKeyWordsList[i].c_str(),
                (char*)m_szMatchResultFileList[j].c_str(),
                (char*)m_szContrastFileList[j].c_str()
            );
            KGLOG_PROCESS_ERROR(bRetCode);

            fprintf(fpResultStatistic, "File Name is %s\n", m_szTestFileList[j].c_str());

            nMatchCount = pBlockMatch->MatchWords(
                (unsigned char*)m_szKeyWordsList[i].c_str(),
                (char*)m_szFullResultPath.c_str(),
                (char*)m_szTestFileList[j].c_str()
            );
            if (nMatchCount < 0)
            {
                fprintf(fpResultStatistic, "Failed Search\n\n");
                continue;
            }

            hHandle = GetCurrentProcess();
            GetProcessMemoryInfo(hHandle, &PMC, sizeof(PMC));


            llFileSize = pBlockMatch->TraversBlock((char*)m_szTestFileList[j].c_str(), (char*)m_szFullResultPath.c_str());
            if (llFileSize < 0)
            {
                fprintf(fpResultStatistic, "\n");
                continue;
            }
            nEnd = clock();

            fprintf(fpResultStatistic, "File Size is %lld Bytes\n", llFileSize);
            fprintf(fpResultStatistic, "Match Count: %d, Match Time %d ms, Process Memory %d MB\n",
                nMatchCount, nEnd - nStart , PMC.WorkingSetSize / MB
            );

            bRetCode = Compare((char*)m_szFullResultPath.c_str(), (char*)m_szFullContrastPath.c_str(), fpResultStatistic);
            if (!bRetCode)
            {
                continue;
            }
        }
    }

    bResult = true;
Exit0:
    if (pBlockMatch)
    {
        delete pBlockMatch;
        pBlockMatch = NULL;
    }

    if (fpResultStatistic)
    {
        fclose(fpResultStatistic);
        fpResultStatistic = NULL;
    }

    return bResult;
}

bool KMatchTest::LoadDirectory(char* pszDirectory, std::vector<std::string>& szList)
{
    bool        bResult         = false;
    FILE        *fpFile         = NULL;
    std::string szItem          = "";
    int         nChar           = 0;

    fpFile = fopen(pszDirectory, "rb");
    KGLOG_PROCESS_ERROR(fpFile);

    while ((nChar = fgetc(fpFile)) != EOF)
    {
        if ((char)nChar != '\r' && (char)nChar != '\n' && (char)nChar != '\0')
        {
            szItem += (char)nChar;
        }

        else if(szItem != "")
        {
            szList.push_back(szItem);
            szItem = "";
        }
    }
    if (szItem != "")
    {
        szList.push_back(szItem);
    }

    bResult = true;
Exit0:
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }

    return bResult;
}

bool KMatchTest::Compare(char* pszResultFilePath, char* pszContrastFilePath, FILE* fpResultStatistic)

{
    bool  bResult              = false;
    bool  bMatch               = true;
    int   nRetCode             = 0;
    char* pszResult            = NULL;
    char* pszContrast          = NULL;
    FILE* fpResultFile         = NULL;
    FILE* fpContrastFile       = NULL;

    KGLOG_PROCESS_ERROR(pszResultFilePath);
    KGLOG_PROCESS_ERROR(pszContrastFilePath);
    KGLOG_PROCESS_ERROR(fpResultStatistic);

    fpResultFile = fopen(pszResultFilePath, "rb");
    if (!fpResultFile)
    {
        fprintf(fpResultStatistic, "\n");
        goto Exit0;
    }

    fpContrastFile = fopen(pszContrastFilePath, "rb");
    if (!fpContrastFile)
    {
        fprintf(fpResultStatistic, "\n");
        goto Exit0;
    }

    pszResult = new char[BUFFER_SIZE];
    KGLOG_PROCESS_ERROR(pszResult);

    pszContrast = new char[BUFFER_SIZE];
    KGLOG_PROCESS_ERROR(pszContrast);

    while (feof(fpResultFile) || feof(fpContrastFile))
    {
        nRetCode = fread(pszContrast, 1, BUFFER_SIZE, fpContrastFile);
        KGLOG_PROCESS_ERROR(nRetCode >= 0);

        nRetCode = fread(pszResult, 1, BUFFER_SIZE, fpResultFile);
        KGLOG_PROCESS_ERROR(nRetCode >= 0);

        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            if (pszContrast[i] != pszResult[i])
            {
                bMatch = false;
            }
        }
    }

    if (bMatch)
    {
        fprintf(fpResultStatistic, "Successful Match\n\n");
    }
    else
    {
        fprintf(fpResultStatistic, "Failed Match\n\n");
    }

    bResult = true;
Exit0:
    if (fpResultFile)
    {
        fclose(fpResultFile);
        fpResultFile = NULL;
    }

    if (fpContrastFile)
    {
        fclose(fpContrastFile);
        fpContrastFile = NULL;
    }

    if (pszContrast)
    {
        delete []pszContrast;
        pszContrast = NULL;
    }

    if (pszResult)
    {
        delete []pszResult;
        pszResult = NULL;
    }

    return bResult;
 }

bool KMatchTest:: GetFullPath(char* pszPattern, char* pszResultPath, char* pszContrastPath)
{
    bool bResult = false;

    KGLOG_PROCESS_ERROR(pszPattern);
    KGLOG_PROCESS_ERROR(pszResultPath);
    KGLOG_PROCESS_ERROR(pszContrastPath);

    m_szFullResultPath = "";
    m_szFullResultPath += pszResultPath;
    m_szFullResultPath += (char*)pszPattern;
    m_szFullResultPath += ".txt";

    m_szFullContrastPath = "";
    m_szFullContrastPath += pszContrastPath;
    m_szFullContrastPath += (char*)pszPattern;
    m_szFullContrastPath += ".txt";

  bResult = true;
Exit0:
    return bResult;
 }
