#include "KMatchWords.h"

#include<Windows.h>
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

KMatchWords::KMatchWords()
{
     m_pnMatchArray   = NULL;
     m_pszText        = NULL;
     m_nTextLen       = 0;
     m_nMatchArrayLen = 0;
}

KMatchWords::~KMatchWords()
{

    UnInit();
}

bool KMatchWords::Init()
{
    bool bResult  = false;

    m_pszText = new unsigned char[BUFFER_SIZE + 1];
    KGLOG_PROCESS_ERROR(m_pszText);

    m_pnMatchArray = new int[BUFFER_SIZE];
    KGLOG_PROCESS_ERROR(m_pnMatchArray);
    m_nMatchArrayLen = BUFFER_SIZE;

    bResult = true;
Exit0:
    return bResult;
}

void KMatchWords::UnInit()
{
    if (m_pszText)
    {
        delete []m_pszText;
        m_pszText = NULL;
    }

    if (m_pnMatchArray)
    {
        delete []m_pnMatchArray;
        m_nMatchArrayLen = 0;
    }

    m_nTextLen = 0;
    m_nMatchArrayLen = 0;
}

bool KMatchWords::MatchWords(unsigned char* pszPattern, char* pszResultPath, char* pszTestPath)
{
    bool     bResult      = false;
    //bool     bRetCode     = false;
    int      nMatchCount  = 0;
    int      nPatternLen  = 0;
    clock_t  nStart       = 0;
    clock_t  nEnd         = 0;
    KSunday* pSundayTest  = NULL;
    FILE*    fpResultFile = NULL;
    FILE*    fpFile       = NULL;
    // HANDLE    handle;
   // PROCESS_MEMORY_COUNTERS pmc;

    KGLOG_PROCESS_ERROR(pszPattern);
    KGLOG_PROCESS_ERROR(pszResultPath);

    nStart = clock();

    fpResultFile = fopen(MATCH_RESULT, "a");
    KGLOG_PROCESS_ERROR(fpResultFile);

      /*fprintf(
        fpResultFile, "File Name is %s, Pattern: %s \n",
        pszTestPath, pszPattern
    );*/

    nPatternLen = strlen((char*)pszPattern);
    if (nPatternLen > PATTERN_SIZE)
    {
        fprintf(fpResultFile, "Pattern is too Large\n\n");
        goto Exit0;
    }

    fpFile = fopen(pszTestPath, "rb");
    KGLOG_PROCESS_ERROR(fpFile);

    m_nTextLen = fread(m_pszText, 1, BUFFER_SIZE, fpFile);
    KGLOG_PROCESS_ERROR(m_nTextLen >= 0);

    m_pszText[m_nTextLen] = '\n';
    m_nTextLen++;

    pSundayTest = new KSunday();
    KGLOG_PROCESS_ERROR(pSundayTest);

    pSundayTest->Init((unsigned char*)pszPattern, nPatternLen);

    nMatchCount = pSundayTest->SundayCount(
        m_pszText, m_nTextLen, pszPattern,
        nPatternLen, m_pnMatchArray, m_nMatchArrayLen
    );
    KGLOG_PROCESS_ERROR(nMatchCount >= 0);

    //handle = GetCurrentProcess();
    //GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));

    nEnd = clock();

   /*fprintf(fpResultFile, "File Size is %d Bytes\n", m_nTextLen);
    fprintf(
        fpResultFile, "Match Count: %d, Match Time %d ms, Process Memory %d MB\n",
        nMatchCount, nEnd - nStart , pmc.WorkingSetSize / (1024 * 1024)
    );*/

    /*bRetCode = Output(m_pnMatchArray, nMatchCount, m_pszText, m_nTextLen, pszResultPath);
    KGLOG_PROCESS_ERROR(bRetCode);*/

    bResult = true;
Exit0:
    if (pSundayTest)
    {
        delete pSundayTest;
        pSundayTest = NULL;
    }

    if (fpResultFile)
    {
        fclose(fpResultFile);
        fpResultFile = NULL;
    }

    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }

    return bResult;
}

bool KMatchWords::Output(
    int* pnMatchArray,int nMatchArrayLen,
    unsigned char* pszText, int nTextLen,
    char* pszResultPath
)
{
    bool  bResult   = false;
    int   nRetCode  = 0;
    FILE* fpFile    = NULL;
    char* pszLine   = NULL;
    int   nLineLen  = 0;

    KGLOG_PROCESS_ERROR(pnMatchArray);
    KGLOG_PROCESS_ERROR(pszText);

    fpFile = fopen(pszResultPath, "a");
    KGLOG_PROCESS_ERROR(fpFile);

    for (int i = 0; i < nMatchArrayLen; ++i)
    {
        int  nIndex       = 0;
        int  nLeft        = 0;
        int  nRight       = 0;
        bool bSelectLeft  = false;
        bool bSelectRight = false;

        nIndex = pnMatchArray[i];

        for (int j = nIndex, k = nIndex; !bSelectLeft || !bSelectRight;)
        {
            if (j >= 0 && pszText[j] != '\n' && pszText[j] != 'r')
            {
                nLeft = j;
                --j;
            }
            else
            {
                bSelectLeft = true;
            }

            if (k < nTextLen && pszText[k] != '\n' && pszText[k] != '\r')
            {
                nRight = k;
                ++k;
            }
            else
            {
                bSelectRight = true;
            }
        }

        pszLine = new char[nRight - nLeft + 2];
        KGLOG_PROCESS_ERROR(pszLine);

        for (int j = nLeft; j <= nRight; ++j)
        {
            pszLine[nLineLen++] = pszText[j];
        }

        pszLine[nLineLen++] = '\n';

        nRetCode = fwrite(pszLine, nLineLen, 1, fpFile);
        KGLOG_PROCESS_ERROR(nRetCode == 1);

        if (pszLine)
        {
            delete []pszLine;
            pszLine = NULL;
        }

        nLineLen = 0;
    }

    bResult = true;
Exit0:
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }

    if (pszLine)
    {
        delete []pszLine;
        pszLine = NULL;
    }

    return bResult;
}
