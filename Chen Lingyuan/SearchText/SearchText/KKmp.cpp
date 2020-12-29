#include "KKmp.h"

#include<Windows.h>
#include<time.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>

KKmp::KKmp()
{

}

KKmp::~KKmp()
{

}

bool KKmp::Init(const char* pcszPattern, int nPatternLen)
{
    bool bResult  = false;
    bool bRetCode = false;

    memset(m_nNext, 0, sizeof(m_nNext));

    KGLOG_PROCESS_ERROR(pcszPattern);
    KGLOG_PROCESS_ERROR(m_nNext);
    m_nNextLen = sizeof(m_nNext) / sizeof(int);

    KGLOG_PROCESS_ERROR(nPatternLen > 0 && m_nNextLen > 0 && nPatternLen <= m_nNextLen);

    bRetCode = GetNext(pcszPattern, nPatternLen, m_nNext, m_nNextLen);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

bool KKmp::GetNext(
    const char* pcszPattern, int nPatternLen,
    int* pnNext, int nNextLen
)
{
    bool bResult = false;

    KGLOG_PROCESS_ERROR(pnNext);
    KGLOG_PROCESS_ERROR(pcszPattern);
    KGLOG_PROCESS_ERROR(nPatternLen > 0 && nNextLen > 0 && nPatternLen <= nNextLen);

    pnNext[0] = 0;

    for (int i = 1, j = 0; i < nPatternLen; ++i)
    {
        for (; j > 0 && pcszPattern[j] != pcszPattern[i]; )
        {
            j = pnNext[j - 1];
        }
        if (pcszPattern[j] == pcszPattern[i])
        {
            ++j;
        }
        pnNext[i] = j;
    }

    bResult = true;
Exit0:
    return bResult;
}

int KKmp::KmpCount(
    const char* pcszText, int nTextLen,
    const char* pcszPattern, int nPatternLen,
    int* pnMatchArray, int nMatchArrayLen
)
{
    bool bResult    = false;
    int  MatchCount = 0;

    KGLOG_PROCESS_ERROR(pcszText);
    KGLOG_PROCESS_ERROR(pcszPattern);
    KGLOG_PROCESS_ERROR(nPatternLen > 0 && m_nNextLen > 0 && nPatternLen <= m_nNextLen);
    KGLOG_PROCESS_ERROR(nMatchArrayLen > 0 && nTextLen > 0);
    KGLOG_PROCESS_ERROR(nMatchArrayLen >= nTextLen);

    for (int i = 0, j = 0; i < nTextLen; ++i)
    {
        for (; j > 0 && (pcszPattern[j] != pcszText[i]); )
        {
            j = m_nNext[j - 1];
        }

        if (pcszPattern[j] == pcszText[i])
        {
            j = j + 1;
        }

        if (j == nPatternLen)
        {
            pnMatchArray[MatchCount++] = i;

            j = m_nNext[j - 1];

            for (int k = i + 1; k < nTextLen; k += 4)
            {
                if (pcszText[k] == '\n')
                {
                    i = k + 1;
                    break;
                }

                if (pcszText[k + 1] == '\n')
                {
                    i = k + 2;
                    break;
                }

                if (pcszText[k + 2] == '\n')
                {
                    i = k + 3;
                    break;
                }

                if (pcszText[k + 3] == '\n')
                {
                    i = k + 4;
                    break;
                }
            }
        }
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        MatchCount = -1;
    }

    return MatchCount;
}
