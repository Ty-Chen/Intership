#include "KSunday.h"

KSunday::KSunday()
{
}

KSunday::~KSunday()
{
    UnInit();
}

bool KSunday::Init(char* pszPattern, int nPatternLen)
{
    bool bResult  = false;
    bool bRetCode = false;

    KGLOG_PROCESS_ERROR(pszPattern);
    KGLOG_PROCESS_ERROR(nPatternLen >= 0);

    bRetCode = GetNext(pszPattern, nPatternLen);
    KGLOG_PROCESS_ERROR(bRetCode);

    m_nSumLine = 0;

    bResult = true;
Exit0:
    return bResult;
}

void KSunday::UnInit()
{
    m_nSumLine = 0;
}

bool KSunday::SearchSunday(char* pszPattern, int nPatternLen, char* pszText, int nTextLen)
{
    bool bResult  = false;
    bool bRetCode = 0;

    KGLOG_PROCESS_ERROR(pszPattern);
    KGLOG_PROCESS_ERROR(pszText);

    bRetCode = Sunday(pszPattern, nPatternLen, pszText, nTextLen);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:

    return bResult;
}

bool KSunday::GetNext(char* pszPattern, int nPatternLen)
{
    int  nNextLen    = sizeof(m_nNext) / sizeof(int);


    for (int i = 0; i < nNextLen; i++)
    {
        m_nNext[i] = nPatternLen;
    }

    for (int i = 0; i < nPatternLen; i++)
    {
        m_nNext[pszPattern[i]] = nPatternLen - i - 1;
    }

    return true;
}

bool KSunday::Sunday(char* pszPattern, int nPatternLen, char* pszText, int nTextLen)
{
    bool  bResult        = false;
    int   nRetCode       = 0;
    int   i      = 0;
    int   nTextPos       = 0;
    int   nResultPos     = 0;
    int   nSumLine       = 0;
    char* pszResult      = NULL;
    char* pszResultPath  = NULL;
    FILE* fpFile         = NULL;

    pszResult = new char[nTextLen + 1];
    KGLOG_PROCESS_ERROR(pszResult);

    for (int pos = 1; pos < nTextLen - nPatternLen;)
    {
        for (i = pos - 1; i - pos + 1 < nPatternLen; i++)
        {
            if (pszText[i] != pszPattern[i - pos + 1])
            {
                break;
            }
        }

        if ((i - pos + 1) == nPatternLen)
        {
            for (nTextPos = pos; nTextPos >= 0; nTextPos--)
            {
                if (pszText[nTextPos] == '\n')
                {
                    break;
                }
            }

            for (nTextPos = nTextPos + 1; nTextPos < nTextLen; nTextPos++)
            {
                if (pszText[nTextPos] == '\n')
                {
                    pos = nTextPos;
                    break;
                }
                pszResult[nResultPos++] = pszText[nTextPos];
            }
            nSumLine++;
        }
        else
        {
            if (pszText[pos + nPatternLen - 1] < 0)
            {
                pos += 1;
                continue;
            }
            pos += m_nNext[pszText[pos + nPatternLen - 1]] + 1;
        }
    }

    pszResultPath = new char[MAX_PATHLEN + 1];
    KGLOG_PROCESS_ERROR(pszResultPath);

    nRetCode = sprintf(pszResultPath, "%s%s.txt", RESULTPATH, pszPattern);
    KGLOG_PROCESS_ERROR(nRetCode != -1);

    fpFile = fopen(pszResultPath, "ab");
    KGLOG_PROCESS_ERROR(fpFile);

    nRetCode = fwrite(pszResult, sizeof(char), nResultPos, fpFile);
    KGLOG_PROCESS_ERROR(nRetCode == nResultPos);

    m_nSumLine += nSumLine;
    std::cout << nSumLine << "  " << m_nSumLine <<std::endl;

    bResult = true;
Exit0:
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }
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
    return bResult;
}
