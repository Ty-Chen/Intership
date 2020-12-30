#include <time.h>

#include "KBm.h"

KBm::KBm()
{
    m_pszPattern    = NULL;
    m_nPatternLen   = 0;
    m_nSumLine      = 0;
    m_nMatchTime     = 0;
}

KBm::~KBm()
{
    UnInit();
}

bool KBm::Init(char* pszPattern, int nPatternLen)
{
    bool bResult  = false;
    bool bRetCode = false;

    m_pszPattern = pszPattern;
    KGLOG_PROCESS_ERROR(m_pszPattern);

    m_nPatternLen  = nPatternLen;

    bRetCode = PrepareBadChar();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = PrepareSuffix();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = PrepareGoodSuffix();
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

void KBm::UnInit()
{
    m_nPatternLen   = 0;
    m_nSumLine      = 0;
    m_nMatchTime     = 0;
    m_pszPattern    = NULL;
}

int KBm::GetMatchTime()
{
    return m_nMatchTime;
}

bool KBm::Search(char* pszPattern, int nPatternLen, char* pszText, int nTextLen)
{
    bool    bResult  = false;
    clock_t lStart   = 0;
    clock_t lEnd     = 0;

    KGLOG_PROCESS_ERROR(pszPattern);

    lStart = clock();
    bResult = BoyerMoore(pszPattern, nPatternLen, pszText, nTextLen);
    KGLOG_PROCESS_ERROR(bResult);
    lEnd = clock()-lStart;
    m_nMatchTime += lEnd;

    bResult = true;
Exit0:
    return bResult;
}

bool KBm::PrepareBadChar()
{
    int  nBadCharLen  = sizeof(m_nBadChar) / sizeof(int);
    int  nPatternLen  = 0;

    for (int i = 0; i < nBadCharLen; i++)
    {
        m_nBadChar[i] = m_nPatternLen;
    }

    nPatternLen = m_nPatternLen - 1;
    for (int i = 0; i < nPatternLen; i++)
    {
        m_nBadChar[m_pszPattern[i]] = nPatternLen - i;
    }

    return true;
}

bool KBm::BoyerMoore(char* pszPattern, int nPatternLen, char* pszText, int nTextLen)
{
    bool  bResult         = false;
    int   nRetCode        = 0;
    int   nMatchLen       = 0;
    int   nSumLine        = 0;
    int   nTextPos        = 0;
    int   nMatchPos       = 0;
    int   nBadCharMove    = 0;
    int   nGoodSuffixMove = 0;
    char* pszResult       = NULL;
    char* pszResultPath   = NULL;
    FILE* fpFile          = NULL;

    pszResult = new char[nTextLen + 1];
    KGLOG_PROCESS_ERROR(pszResult);

    KGLOG_PROCESS_ERROR(pszPattern);
    KGLOG_PROCESS_ERROR(pszText);

    for (int i = 0; i <= nTextLen - nPatternLen;)
    {
        nMatchPos = nPatternLen - 1;
        for (; nMatchPos >= 0; nMatchPos--)
        {
            if (pszPattern[nMatchPos] != pszText[nMatchPos + i])
            {
                break;
            }
        }

        if (nMatchPos < 0)
        {
            for (nMatchLen = i; nMatchLen >= 0; nMatchLen--)
            {
                if (pszText[nMatchLen] == '\n')
                {
                    break;
                }
            }

            for (int j = nMatchLen + 1; j < nTextLen; j++)
            {
                if (pszText[j] == '\n')
                {
                    i = j;
                    break;
                }
                pszResult[nTextPos++] = pszText[j];
            }

            nSumLine++;
        }
        else
        {
            nBadCharMove = (m_nBadChar[pszText[nMatchPos + i]] - nPatternLen + 1 + nMatchPos);
            nGoodSuffixMove = m_nGoodSuffix[nMatchPos];

            if (nBadCharMove >= nGoodSuffixMove)
            {
               i = i + nBadCharMove;
            }
            else
            {
                i = i + nGoodSuffixMove;
            }
        }
    }

    pszResultPath = new char[MAX_PATHLEN + 1];
    KGLOG_PROCESS_ERROR(pszResultPath);

    nRetCode = sprintf(pszResultPath, "%s%s.txt", RESULTPATH, pszPattern);
    KGLOG_PROCESS_ERROR(nRetCode != -1);

    fpFile = fopen(pszResultPath, "ab");
    KGLOG_PROCESS_ERROR(fpFile);

    nRetCode = fwrite(pszResult, sizeof(char), nTextPos, fpFile);
    KGLOG_PROCESS_ERROR(nRetCode == nTextPos);

    m_nSumLine += nSumLine;
    std::cout << "LineNum: " << nSumLine << " SumLine:" << m_nSumLine << std::endl;

    bResult = true;
Exit0:
    if (pszResult)
    {
        delete[] pszResult;
        pszResult = NULL;
    }

    if (pszResultPath)
    {
        delete[] pszResultPath;
        pszResultPath = NULL;
    }

    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }
    return bResult;
}

bool KBm::PrepareGoodSuffix()
{
    int  nPatternLen   = 0;

    for (int i = 0; i < m_nPatternLen; i++)
    {
        m_nGoodSuffix[i] = m_nPatternLen;
    }

    for (int i = m_nPatternLen - 1, j = 0; i >= 0; i--)
    {
        if (m_nSuff[i] != (i + 1))
        {
            continue;
        }

        nPatternLen = m_nPatternLen - 1 - i;
        for (; j < nPatternLen; j++)
        {
            if (m_nGoodSuffix[j] == m_nPatternLen)
            {
                m_nGoodSuffix[j] = nPatternLen;
            }
        }
    }

    nPatternLen = m_nPatternLen - 1;
    for (int i = 0; i < nPatternLen; i++)
    {
        m_nGoodSuffix[nPatternLen - m_nSuff[i]] = nPatternLen - i;
    }

    return true;
}

bool KBm::PrepareSuffix()
{
    int  nPatternPos   = 0;

    m_nSuff[m_nPatternLen - 1] = m_nPatternLen;
    for (int i = m_nPatternLen - 2; i >= 0; i--)
    {
        nPatternPos = i;
        while (nPatternPos >= 0 &&
            (m_pszPattern[nPatternPos] == m_pszPattern[nPatternPos + m_nPatternLen - 1 - i]))
        {
            --nPatternPos;
        }
        m_nSuff[i] = i - nPatternPos;
    }

    return true;
}
