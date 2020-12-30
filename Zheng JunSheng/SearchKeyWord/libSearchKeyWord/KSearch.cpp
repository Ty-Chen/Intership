#include "KSearch.h"

#include <time.h>
#include <stdio.h>
#include <string.h>

#include "KBM.h"
#include "KSunday.h"

KSearch::KSearch()
{

}

KSearch::~KSearch()
{
    UnInit();
}

bool KSearch::Init(
    int   nKeyLen, char* pszKeyWord,
    char* pszFilePath, char* pszResultPath
)
{
    bool  bResult = false;
    FILE* fpFile  = NULL;

    KGLOG_PROCESS_ERROR(nKeyLen);
    KGLOG_PROCESS_ERROR(pszKeyWord);
    KGLOG_PROCESS_ERROR(pszFilePath);
    KGLOG_PROCESS_ERROR(pszResultPath);

    m_nKeyLen       = nKeyLen;
    m_nCntResult    = 0;
    m_pszKeyWord    = pszKeyWord;
    m_pszFilePath   = pszFilePath;
    m_pszResultPath = pszResultPath;

    fpFile = fopen(m_pszResultPath, "wb");
    KGLOG_PROCESS_ERROR(fpFile);

    bResult = true;
Exit0:
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }

    return bResult;
}

void KSearch::UnInit()
{
    m_nKeyLen       = NULL;
    m_nCntResult    = 0;
    m_pszKeyWord    = NULL;
    m_pszFilePath   = NULL;
    m_pszResultPath = NULL;
}

bool KSearch::RunSunday()
{
    bool     bResult            = false;
    int      nRetCode           = 0;
    int      nFreadResult       = 0;
    int      nIncompleteLineLen = 0;
    bool     bIsFinishFile      = false;
    FILE*    fpFile             = NULL;
    KSunday* pSunday            = NULL;

    char*   pszText            = NULL;
    char*   pszIncompleteLine  = NULL;

    fpFile = fopen(m_pszFilePath, "rb");
    KGLOG_PROCESS_ERROR(fpFile);

    pSunday = new KSunday();
    KGLOG_PROCESS_ERROR(pSunday);

    nRetCode = pSunday->Init((unsigned char*)m_pszKeyWord, m_nKeyLen, m_pszResultPath);
    KGLOG_PROCESS_ERROR(nRetCode);

    pszText = new char[MAX_BUFFER + MAX_LINE_LEN + 1];
    KGLOG_PROCESS_ERROR(pszText);

    pszIncompleteLine = new char[MAX_LINE_LEN + 1];
    KGLOG_PROCESS_ERROR(pszIncompleteLine);

    while (true)
    {
        nFreadResult = fread(pszText, 1, MAX_BUFFER, fpFile);
        KGLOG_PROCESS_ERROR(nFreadResult > 0);
        pszText[nFreadResult] = '\0';

        bIsFinishFile = feof(fpFile);

        if (nFreadResult == MAX_BUFFER && pszText[nFreadResult - 1] != '\n' && !bIsFinishFile)
        {
            fgets(pszIncompleteLine, MAX_LINE_LEN + 1, fpFile);
            nIncompleteLineLen = strlen(pszIncompleteLine);

            bIsFinishFile = feof(fpFile);

            for (int i = 0; i < nIncompleteLineLen; ++i)
            {
                pszText[nFreadResult++] = pszIncompleteLine[i];
            }
        }
        pszText[nFreadResult] = '\0';

        nRetCode = pSunday->Run((unsigned char*)pszText, nFreadResult);
        KGLOG_PROCESS_ERROR(nRetCode);

        if (bIsFinishFile)
        {
            break;
        }
    }

    m_nCntResult = pSunday->m_nCntResult;

    bResult = true;
Exit0:
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }

    if (pszIncompleteLine)
    {
        delete []pszIncompleteLine;
        pszIncompleteLine = NULL;
    }

    if (pszText)
    {
        delete []pszText;
        pszText = NULL;
    }

    if (pSunday)
    {
        delete pSunday;
        pSunday = NULL;
    }

    return bResult;
}

bool KSearch::RunBM()
{
    bool    bResult            = false;
    int     nRetCode           = 0;
    int     nTextStartPos      = 0;
    int     nTextEndPos        = 0;
    int     nFreadResult       = 0;
    int     nIncompleteLineLen = 0;
    KBM*    pBM                = NULL;
    FILE*   fpFile             = NULL;
    char*   pszText            = NULL;
    char*   pszIncompleteLine  = NULL;

    fpFile = fopen(m_pszFilePath, "rb");
    KGLOG_PROCESS_ERROR(fpFile);

    pBM = new KBM();
    KGLOG_PROCESS_ERROR(pBM);

    nRetCode = pBM->Init((char*)m_pszKeyWord, m_nKeyLen, m_pszResultPath);
    KGLOG_PROCESS_ERROR(nRetCode);

    pszText = new char[MAX_BUFFER + 1];
    KGLOG_PROCESS_ERROR(pszText);

    pszIncompleteLine = new char[MAX_LINE_LEN + 1];
    KGLOG_PROCESS_ERROR(pszIncompleteLine);

    while (true)
    {
        nFreadResult = fread(pszText, 1, MAX_BUFFER, fpFile);
        pszText[nFreadResult] = '\0';

        if (nFreadResult == 0)
        {
            if (nIncompleteLineLen != 0)
            {
                nRetCode = pBM->RunNewText(pszIncompleteLine, nIncompleteLineLen);
                KGLOG_PROCESS_ERROR(nRetCode);

                nIncompleteLineLen = 0;
            }

            break;
        }

        if (nIncompleteLineLen != 0)
        {
            for (int i = 0; i < nFreadResult; ++i)
            {
                pszIncompleteLine[nIncompleteLineLen++] = pszText[i];

                if (pszText[i] == '\n')
                {
                    nTextStartPos = i + 1;
                    break;
                }
            }
            pszIncompleteLine[nIncompleteLineLen++] = '\0';

            nRetCode = pBM->RunNewText(pszIncompleteLine, nIncompleteLineLen);
            KGLOG_PROCESS_ERROR(nRetCode);

            nIncompleteLineLen = 0;
        }
        else
        {
            nTextStartPos = 0;
        }

        if (nFreadResult == MAX_BUFFER && pszText[nFreadResult - 1] != '\n')
        {
            for (int i = nFreadResult - 1; i >= 0; --i)
            {
                if (pszText[i] == '\n')
                {
                    nTextEndPos = i;
                    break;
                }
            }

            for (int i = nTextEndPos + 1; i < nFreadResult; ++i)
            {
                pszIncompleteLine[nIncompleteLineLen++] = pszText[i];
            }
        }
        else
        {
            nTextEndPos = nFreadResult - 1;
        }

        nRetCode = pBM->RunNewText(pszText + nTextStartPos, nTextEndPos - nTextStartPos + 1);
        KGLOG_PROCESS_ERROR(nRetCode);
        /*nFreadResult = fread(pszText, 1, MAX_BUFFER, fpFile);
        KGLOG_PROCESS_ERROR(nFreadResult);

        if (nFreadResult == 0)
        {
            break;
        }

        if (nFreadResult == MAX_BUFFER && pszText[nFreadResult - 1] != '\n')
        {
            fgets(pszIncompleteLine, MAX_LINE_LEN + 1, fpFile);
            nIncompleteLineLen = strlen(pszIncompleteLine);

            for (int i = 0; i < nIncompleteLineLen; ++i)
            {
                pszText[nFreadResult++] = pszIncompleteLine[i];
            }
        }
        pszText[nFreadResult] = '\0';

        nRetCode = pBM->RunNewText(pszText, nFreadResult);
        KGLOG_PROCESS_ERROR(nRetCode);*/
    }

    m_nCntResult = pBM->m_nCntResult;

    bResult = true;
Exit0:
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }

    if (pszIncompleteLine)
    {
        delete[]pszIncompleteLine;
        pszIncompleteLine = NULL;
    }

    if (pszText)
    {
        delete[]pszText;
        pszText = NULL;
    }

    if (pBM)
    {
        delete pBM;
        pBM = NULL;
    }

    return bResult;
}