#include "KSearch.h"

#include <time.h>
#include <stdio.h>
#include <string.h>

#include "KBM.h"

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

bool KSearch::RunBigFile()
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

    pszText = new char[MAX_BUFFER + 1];
    KGLOG_PROCESS_ERROR(pszText);

    pszIncompleteLine = new char[MAX_LINE_LEN + 1];
    KGLOG_PROCESS_ERROR(pszIncompleteLine);

    pBM = new KBM();
    KGLOG_PROCESS_ERROR(pBM);

    nRetCode = pBM->Init(m_pszKeyWord, m_nKeyLen, pszText, MAX_BUFFER + 1, m_pszResultPath);
    KGLOG_PROCESS_ERROR(nRetCode);

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
        delete []pszIncompleteLine;
        pszIncompleteLine = NULL;
    }

    if (pszText)
    {
        delete []pszText;
        pszText = NULL;
    }

    if (pBM)
    {
        delete pBM;
        pBM = NULL;
    }

    return bResult;
}