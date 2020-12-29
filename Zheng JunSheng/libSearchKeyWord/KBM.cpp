#include "KBM.h"

#include <time.h>
#include <stdio.h>
#include <string.h>

inline int MaxNum(int nNum1, int nNum2)
{
    if (nNum1 > nNum2)
    {
        return nNum1;
    }
    else
    {
        return nNum2;
    }
}

KBM::KBM()
{
    m_nKeyLen       = 0;
    m_nTextLen      = 0;
    m_nCntResult    = 0;
    m_pszText       = NULL;
    m_pnSuffix      = NULL;
    m_pszKeyWord    = NULL;
    m_pnGoodSuffix  = NULL;
    m_pszResultPath = NULL;
}

KBM::~KBM()
{
    UnInit();
}

bool KBM::Init(
    char* pszKeyWord, const int nKeyLen,
    char* pszText, const int nTextLen, char* pszResultPath
)
{
    bool bResult         = false;
    int  nRetCode        = 0;
    int  nSuffixSize     = 0;
    int  nBadCharSize    = 0;
    int  nGoodSuffixSize = 0;

    KGLOG_PROCESS_ERROR(pszKeyWord);
    KGLOG_PROCESS_ERROR(nKeyLen > 0);

    KGLOG_PROCESS_ERROR(pszText);
    KGLOG_PROCESS_ERROR(nTextLen > 0);

    KGLOG_PROCESS_ERROR(pszResultPath);

    m_nKeyLen       = nKeyLen;
    m_nTextLen      = nTextLen;
    m_pszText       = pszText;
    m_pszKeyWord    = pszKeyWord;
    m_pszResultPath = pszResultPath;

    m_pnSuffix = new int[m_nKeyLen + 1];
    KGLOG_PROCESS_ERROR(m_pnSuffix);
    nSuffixSize = m_nKeyLen + 1;

    m_pnGoodSuffix = new int[m_nKeyLen + 1];
    KGLOG_PROCESS_ERROR(m_pnGoodSuffix);
    nGoodSuffixSize = m_nKeyLen + 1;

    nRetCode = PrepareGoodSuffix(
        m_pnGoodSuffix, nGoodSuffixSize,
        m_pnSuffix, nSuffixSize
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    memset(m_nBadChar, 0, sizeof(m_nBadChar));

    nBadCharSize = sizeof(m_nBadChar) / sizeof(m_nBadChar[0]);
    nRetCode = PrepareBadChar(m_nBadChar, nBadCharSize);
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    return bResult;
}

void KBM::UnInit()
{
    if (m_pnSuffix)
    {
        delete []m_pnSuffix;
        m_pnSuffix = NULL;
    }

    if (m_pnGoodSuffix)
    {
        delete []m_pnGoodSuffix;
        m_pnGoodSuffix = NULL;
    }

    m_pszText       = NULL;
    m_nKeyLen       = 0;
    m_nTextLen      = 0;
    m_nCntResult    = 0;
    m_pszKeyWord    = NULL;
    m_pszResultPath = NULL;
}

bool KBM::PrepareGoodSuffix(
    int* pnGoodSuffix, const int nGoodSuffixSize,
    int* pnSuffix, const int nSuffixSize
)
{
    bool bResult = false;

    KGLOG_PROCESS_ERROR(pnGoodSuffix);
    KGLOG_PROCESS_ERROR(nGoodSuffixSize > 0);

    KGLOG_PROCESS_ERROR(pnSuffix);
    KGLOG_PROCESS_ERROR(nSuffixSize > 0);

    for (int i = 0; i < nGoodSuffixSize; ++i)
    {
        pnGoodSuffix[i] = 0;
    }

    pnSuffix[m_nKeyLen] = m_nKeyLen + 1;

    for (int i = m_nKeyLen, j = m_nKeyLen + 1; i > 0; --i, --j)
    {
        while (j <= m_nKeyLen && m_pszKeyWord[i - 1] != m_pszKeyWord[j - 1])
        {
            if (pnGoodSuffix[j] == 0)
            {
                pnGoodSuffix[j] = j - i;
            }

            j = pnSuffix[j];
        }

        pnSuffix[i - 1] = j - 1;
    }

    for (int i = 0, j = pnSuffix[0]; i <= m_nKeyLen; ++i)
    {
        if (pnGoodSuffix[i] == 0)
        {
            pnGoodSuffix[i] = j;
        }

        if (i == j)
        {
            j = pnSuffix[j];
        }
    }

    bResult = true;
Exit0:
    return bResult;
}

bool KBM::PrepareBadChar(
    int nBadChar[], const int nBadCharSize
)
{
    bool bResult = false;

    KGLOG_PROCESS_ERROR(nBadChar);
    KGLOG_PROCESS_ERROR(nBadCharSize > 0);

    for (int i = 0; i < nBadCharSize; ++i)
    {
        nBadChar[i] = m_nKeyLen;
    }

    for (int i = 0; i < m_nKeyLen - 1; ++i)
    {
        nBadChar[m_pszKeyWord[i]] = m_nKeyLen - 1 - i;
    }

    bResult = true;
Exit0:
    return bResult;
}

bool KBM::Run()
{
    bool  bResult    = false;
    int   nRetCode   = 0;
    int   nBadMove   = 0;
    int   nStartPos  = 0;
    int   nResultPos = 0;
    int   nCntResult = 0;
    FILE* fpFile     = NULL;
    char* pszResult  = NULL;

    fpFile = fopen(m_pszResultPath, "ab");
    KGLOG_PROCESS_ERROR(fpFile);

    pszResult = new char[m_nTextLen + 1];
    KGLOG_PROCESS_ERROR(pszResult);

    for (int i = 0, j = 0; i <= m_nTextLen - m_nKeyLen; )
    {
        for (j = m_nKeyLen - 1; j >= 0 && m_pszKeyWord[j] == m_pszText[i + j]; --j);

        if (j < 0)
        {
            ++nCntResult;

            for (nStartPos = i; nStartPos >= 0; --nStartPos)
            {
                if (m_pszText[nStartPos] == '\n')
                {
                    break;
                }
            }

            for (int k = nStartPos + 1; k < m_nTextLen; ++k)
            {
                pszResult[nResultPos++] = m_pszText[k];

                if (m_pszText[k] == '\n')
                {
                    i = k + 1;
                    break;
                }
            }

        }
        else
        {
            if (m_pszText[i + j] < 0)
            {
                nBadMove = 0;
            }
            else
            {
                nBadMove = m_nBadChar[m_pszText[i + j]] - m_nKeyLen + 1 + j;
            }

            i += MaxNum(nBadMove, m_pnGoodSuffix[j + 1]);
        }
    }
    pszResult[nResultPos++] = '\0';

    m_nCntResult += nCntResult;

    nRetCode = fwrite(pszResult, nResultPos, 1, fpFile);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    bResult = true;
Exit0:
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }

    if (pszResult)
    {
        delete []pszResult;
        pszResult = NULL;
    }

    return bResult;
}


bool KBM::RunNewText(char* pszText, int nTextLen)
{
    bool bResult  = false;
    int  nRetCode = 0;

    KGLOG_PROCESS_ERROR(pszText);
    KGLOG_PROCESS_ERROR(nTextLen > 0);

    m_pszText  = pszText;
    m_nTextLen = nTextLen;

    if (m_nKeyLen == 1)
    {
        nRetCode = RunSingleChar();
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = Run();
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    bResult = true;
Exit0:
    return bResult;
}

bool KBM::RunSingleChar()
{
    bool  bResult    = false;
    int   nRetCode   = 0;
    int   nStartPos  = 0;
    int   nResultPos = 0;
    int   nCntResult = 0;
    FILE* fpFile     = NULL;
    char* pszResult  = NULL;

    pszResult = new char[m_nTextLen + 1];
    KGLOG_PROCESS_ERROR(pszResult);

    for (int i = 0; i < m_nTextLen; ++i)
    {
        if (m_pszText[i] == '\n')
        {
            nStartPos = i + 1;
        }

        if (m_pszText[i] == m_pszKeyWord[0])
        {
            ++nCntResult;

            for (int j = nStartPos; j < m_nTextLen; ++j)
            {
                pszResult[nResultPos++] = m_pszText[j];

                if (m_pszText[j] == '\n')
                {
                    i         = j + 1;
                    nStartPos = j + 1;
                    break;
                }

                if (j == m_nTextLen - 1)
                {
                    i = j;
                    break;
                }
            }
        }
    }
    pszResult[nResultPos++] = '\0';

    m_nCntResult += nCntResult;

    fpFile = fopen(m_pszResultPath, "ab");
    KGLOG_PROCESS_ERROR(fpFile);

    nRetCode = fwrite(pszResult, nResultPos - 1, 1, fpFile);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    bResult = true;
Exit0:
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }

    if (pszResult)
    {
        delete[]pszResult;
        pszResult = NULL;
    }

    return bResult;
}