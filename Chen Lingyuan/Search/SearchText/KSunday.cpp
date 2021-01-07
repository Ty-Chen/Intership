#include "KSunday.h"

#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

KSunday::KSunday()
{

}

KSunday::~KSunday()
{

}

bool KSunday::Init(const unsigned char* pcszPattern,  int nPatternLen)
{
    bool bResult  = false;
    bool bRetCode = false;

    memset(m_nBadChar, 0, sizeof(m_nBadChar));
    m_nBadCharLen = 0;

    KGLOG_PROCESS_ERROR(pcszPattern);
    KGLOG_PROCESS_ERROR(m_nBadChar);

    m_nBadCharLen = sizeof(m_nBadChar) / sizeof(int) ;
    KGLOG_PROCESS_ERROR(m_nBadCharLen > 0);


    bRetCode = GetBadChar(pcszPattern, nPatternLen, m_nBadChar, m_nBadCharLen);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

bool KSunday::GetBadChar(
    const unsigned char* pcszPattern, int nPatternLen,
    int* pnBadChar, int nBadCharLen
)
{
    bool bResult = false;

    KGLOG_PROCESS_ERROR(pcszPattern);
    KGLOG_PROCESS_ERROR(pnBadChar);
    KGLOG_PROCESS_ERROR(nBadCharLen == ASCLL);

    for (int i = 0; i < nBadCharLen; ++i)
    {
        pnBadChar[i] = -1;
    }

    for (int i = 0; i < nPatternLen; ++i)
    {
        pnBadChar[pcszPattern[i]] = i;
    }

    bResult = true;
Exit0:
    return bResult;
}

int  KSunday::SundayCount(
    const unsigned char* pcszText, int nTextLen,
    const unsigned char* pcszPattern, int nPatternLen,
    int *pnMatchArray, int nMatchArrayLen
)
{
    bool bResult       = false;
    int  nMatchCount   = 0;

    KGLOG_PROCESS_ERROR(pcszPattern);
    KGLOG_PROCESS_ERROR(pcszText);
    KGLOG_PROCESS_ERROR(m_nBadChar);
    KGLOG_PROCESS_ERROR(pnMatchArray);

    KGLOG_PROCESS_ERROR(m_nBadCharLen == ASCLL);
    KGLOG_PROCESS_ERROR(nMatchArrayLen > 0 && nTextLen >= 0);
    KGLOG_PROCESS_ERROR(nMatchArrayLen >= nTextLen);

    for(int i = 0; i <= nTextLen - nPatternLen;)
    {
        int nPatternIndex;
        for (nPatternIndex = 0; nPatternIndex < nPatternLen; ++nPatternIndex)
        {
            if (pcszPattern[nPatternIndex] != pcszText[nPatternIndex + i])
            {
                break;
            }
        }

        if (nPatternIndex >= nPatternLen)
        {
           pnMatchArray[nMatchCount++] = i;

            for (int j = i + nPatternLen; j < nTextLen; j += 4)
            {
                if (pcszText[j] == '\n')
                {
                    i = j + 1;
                    break;
                }

                if (pcszText[j + 1] == '\n')
                {
                    i = j + 2;
                    break;
                }

                if (pcszText[j + 2] == '\n')
                {
                    i = j + 3;
                    break;
                }

                if (pcszText[j + 3] == '\n')
                {
                    i = j + 4;
                    break;
                }
            }
        }
        else
        {
            i += nPatternLen - m_nBadChar[pcszText[i + nPatternLen]];
        }
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        nMatchCount = -1;
    }

    return nMatchCount;
}
