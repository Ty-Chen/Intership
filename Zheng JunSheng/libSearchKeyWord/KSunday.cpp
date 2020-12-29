#include "KSunday.h"

#include <stdio.h>

KSunday::KSunday()
{
    m_nKeyLen       = 0;
    m_pszKeyWord    = NULL;
    m_pszResultPath = NULL;
}

KSunday::~KSunday()
{
    UnInit();
}

bool KSunday::Init(
    char* pszKeyWord, const int nKeyLen,
    char* pszResultPath
)
{
    bool bResult = false;

    KGLOG_PROCESS_ERROR(pszKeyWord);
    KGLOG_PROCESS_ERROR(nKeyLen > 0);
    KGLOG_PROCESS_ERROR(pszResultPath);

    m_nKeyLen       = nKeyLen;
    m_pszKeyWord    = pszKeyWord;
    m_pszResultPath = pszResultPath;

    for (int i = 0; i < ASCII_SIZE; ++i)
    {
        m_nSunday[i] = nKeyLen + 1;
    }

    for (int i = 0; i < nKeyLen; i++) {
        m_nSunday[pszKeyWord[i]] = nKeyLen - i;
    }

    bResult = true;
Exit0:
    return bResult;
}


void KSunday::UnInit()
{
    m_nKeyLen       = 0;
    m_pszKeyWord    = NULL;
    m_pszResultPath = NULL;
}

bool KSunday::Run(char* pszText, int nTextLen)
{
    bool bResult    = false;
    int  nTextPos   = 0;
    int  nCntResult = 0;

    KGLOG_PROCESS_ERROR(pszText);
    KGLOG_PROCESS_ERROR(nTextLen > 0);

    while (nTextPos <= nTextLen - m_nKeyLen)
    {
        int nKeyPos = 0;
        while (nKeyPos < m_nKeyLen && pszText[nTextPos + nKeyPos] == m_pszKeyWord[nKeyPos])
        {
            ++nKeyPos;
        }

        if (nKeyPos == m_nKeyLen)
        {
            ++nCntResult;

            while (nTextPos < nTextLen)
            {
                ++nTextPos;
                if (pszText[nTextPos] == '\n')
                {
                    break;
                }
            }
        }

        if (nTextPos + m_nKeyLen >= nTextLen)
        {
            break;
        }

        nTextPos += m_nSunday[pszText[nTextPos + m_nKeyLen]];
    }

    m_nCntResult += nCntResult;

    bResult = true;
Exit0:
    return bResult;
}