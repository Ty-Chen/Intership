#include "KSunday.h"

#include <stdio.h>

KSunday::KSunday()
{
    m_nKeyLen = 0;
    m_pszKeyWord = NULL;
}

KSunday::~KSunday()
{
    UnInit();
}

bool KSunday::Init(
    unsigned char* pszKeyWord, const int nKeyLen
)
{
    bool bResult = false;

    KGLOG_PROCESS_ERROR(pszKeyWord);
    KGLOG_PROCESS_ERROR(nKeyLen > 0);

    m_nKeyLen    = nKeyLen;
    m_pszKeyWord = pszKeyWord;

    for (int i = 0; i < ASCII_SIZE; ++i)
    {
        m_nSunday[i] = nKeyLen + 1;
    }

    for (int i = 0; i < nKeyLen; ++i)
    {
        m_nSunday[pszKeyWord[i]] = nKeyLen - i;
    }

    bResult = true;
Exit0:
    return bResult;
}


void KSunday::UnInit()
{
    m_nKeyLen    = 0;
    m_pszKeyWord = NULL;
}

bool KSunday::Run(unsigned char* pszText, int nTextLen)
{
    bool  bResult    = false;
    int   nTextPos   = 0;
    int   nCntResult = 0;
    int   nResultLen = 0;
    char* pszResult  = NULL;

    KGLOG_PROCESS_ERROR(pszText);
    KGLOG_PROCESS_ERROR(nTextLen > 0);

    pszResult = new char[nTextLen + 1];
    KGLOG_PROCESS_ERROR(pszResult);

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

            int nIndex = nTextPos;
            while (nIndex >= 0)
            {
                if (pszText[nIndex] == '\n')
                {
                    break;
                }
                --nIndex;
            }

            ++nIndex;
            while (nIndex < nTextLen)
            {
                pszResult[nResultLen++] = pszText[nIndex];

                if (pszText[nIndex] == '\r' || nIndex == nTextLen - 1)
                {
                    nTextPos = nIndex;
                    break;
                }

                ++nIndex;
            }
        }
        else
        {
            nTextPos += m_nSunday[pszText[nTextPos + m_nKeyLen]];
        }
    }
    pszResult[nResultLen] = '\0';

    m_nCntResult += nCntResult;

    fwrite(pszResult, 1, nResultLen, stdout);
    fflush(stdout);

    bResult = true;
Exit0:
    if (pszResult)
    {
        delete []pszResult;
        pszResult = NULL;
    }

    return bResult;
}