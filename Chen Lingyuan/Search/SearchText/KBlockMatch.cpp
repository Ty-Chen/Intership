#include "KBlockMatch.h"

#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#include<string>
#include<future>
#include<vector>

KBlockMatch::KBlockMatch()
{
    for (int i = 0; i < THREAD_NUM; i++)
    {
        m_pszReadText[i]    = NULL;
        m_pszPerformText[i] = NULL;
        m_pnTempMatch[i]    = NULL;
    }
}

KBlockMatch::~KBlockMatch()
{
    UnInit();
}

bool KBlockMatch::Init()
{
    bool bResult  = false;

    for (int i = 0; i < THREAD_NUM; i++)
    {
        m_pszReadText[i] = new unsigned char[BUFFER_SIZE + MAX_LINE];
        KGLOG_PROCESS_ERROR(m_pszReadText[i]);

        m_pszPerformText[i] = new unsigned char[BUFFER_SIZE + MAX_LINE];
        KGLOG_PROCESS_ERROR(m_pszPerformText[i]);

        m_pnTempMatch[i] = new int[BUFFER_SIZE + MAX_LINE];
        KGLOG_PROCESS_ERROR(m_pnTempMatch[i]);
    }

    m_pszOutputText = new unsigned char[BUFFER_SIZE * 3];
    KGLOG_PROCESS_ERROR(m_pszOutputText);

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (m_pszOutputText)
        {
            delete[]m_pszOutputText;
            m_pszOutputText = NULL;
        }

        for (int i = 0; i < THREAD_NUM; i++)
        {
            if (m_pnTempMatch[i])
            {
                delete []m_pnTempMatch[i];
                m_pnTempMatch[i] = NULL;
            }

            if (m_pszPerformText[i])
            {
                delete []m_pszPerformText[i];
                m_pszPerformText[i] = NULL;
            }

            if (m_pszReadText[i])
            {
                delete []m_pszReadText[i];
                m_pszReadText[i] = NULL;
            }
        }
    }
    return bResult;
}

void KBlockMatch::UnInit()
{
    for (int i = 0; i < THREAD_NUM; i++)
    {
        if (m_pszReadText[i])
        {
            delete []m_pszReadText[i];
            m_pszReadText[i] = NULL;
        }

        if (m_pszPerformText[i])
        {
            delete []m_pszPerformText[i];
            m_pszPerformText[i] = NULL;
        }

        if (m_pnTempMatch[i])
        {
            delete []m_pnTempMatch[i];
            m_pnTempMatch[i] = NULL;
        }
    }
    if (m_pszOutputText)
    {
         delete[]m_pszOutputText;
         m_pszOutputText = NULL;
    }
}

int KBlockMatch::MatchWords(unsigned char* pszPattern, std::string szTestPath)
{
    bool      bResult          = false;
    bool      bRetCode         = false;
    bool      bReadComplete    = false;
    int       nMatchCount      = 0;
    int       nPatternLen      = 0;
    int       nTestPathLen     = 0;
    char*     pszTestPath      = NULL;
    KSunday*  pSundayTest      = NULL;
    FILE*     fpFile           = NULL;

	KGLOG_PROCESS_ERROR(pszPattern);

	fpFile = fopen(szTestPath.c_str(), "rb");
	KGLOG_PROCESS_ERROR(fpFile);

	szTestPath += ':';
	nTestPathLen = (int)szTestPath.length();

	pszTestPath = (char*)szTestPath.c_str();
	KGLOG_PROCESS_ERROR(pszTestPath);

	pSundayTest = new KSunday();
	KGLOG_PROCESS_ERROR(pSundayTest);

	nPatternLen = (int)strlen((char*)pszPattern);
	KGLOG_PROCESS_ERROR(nPatternLen <= PATTERN_SIZE);

	bRetCode = pSundayTest->Init(pszPattern, nPatternLen);
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = ReadFileBlock(fpFile);
	KGLOG_PROCESS_ERROR(bRetCode);

	bReadComplete = IsReadComplete();

    for (int i = 0; !bReadComplete; i++)
    {
        std::future<int> fMatch[THREAD_NUM];

        for (int j = 0; j < THREAD_NUM; j++)
        {
            fMatch[j] = std::async(
                &KSunday::SundayCount, pSundayTest, m_pszPerformText[j], m_nPerformTextLen[j],
                pszPattern, nPatternLen, m_pnTempMatch[j], BUFFER_SIZE + MAX_LINE
            );
        }

        bRetCode = ReadFileBlock(fpFile);
        KGLOG_PROCESS_ERROR(bRetCode);

        for (int j = 0; j < THREAD_NUM; j++)
        {
            int nCount = 0;
            nCount = fMatch[j].get();
            nMatchCount += nCount;

            KGLOG_PROCESS_ERROR(nCount >= 0);

            bRetCode = OutPut(
                m_pnTempMatch[j], nCount,
                m_pszPerformText[j], m_nPerformTextLen[j] - 1,
                pszTestPath, nTestPathLen
            );
            KGLOG_PROCESS_ERROR(bRetCode);

            fMatch[j].~future();
        }

        bReadComplete = IsReadComplete();
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        nMatchCount = -1;
    }

    if (pSundayTest)
    {
        delete pSundayTest;
        pSundayTest = NULL;
    }

    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }

    return nMatchCount;
}

bool KBlockMatch::ReadFileBlock(FILE* fpFile)
{
    bool bResult   = false;
    int  nReadSize = 0;
    int  nOffset   = 0;
    int  nRetCode  = 0;
    KGLOG_PROCESS_ERROR(fpFile);

    for (int i = 0; i < THREAD_NUM; i++)
    {
        nReadSize = (int)fread(m_pszReadText[i], 1, BUFFER_SIZE, fpFile);
        KGLOG_PROCESS_ERROR(nReadSize >= 0);

        if (nReadSize == BUFFER_SIZE)
        {
            for (int j = nReadSize - 1; j >= 0; j--)
            {
                if (m_pszReadText[i][j] == '\n' || m_pszReadText[i][j] == '\r')
                {
                    nOffset = nReadSize - j;

                    nReadSize = j;
                    break;
                }
            }
            nRetCode = fseek(fpFile, -nOffset, SEEK_CUR);
            KGLOG_PROCESS_ERROR(nRetCode >= 0);
        }

        m_pszReadText[i][nReadSize] = '\n';
        nReadSize++;

        m_nReadTextLen[i] = nReadSize;
    }

    bResult = true;
Exit0:
    return bResult;
}

bool KBlockMatch::OutPut(
    int* pnMatchArray, int nMatchArrayLen,
    unsigned char* pszText, int nTextLen,
    char* pszTestPath, int nTestPathLen
)
{
    bool  bResult      = false;
    int   nRetCode     = 0;
    int   nOutLen      = 0;

    KGLOG_PROCESS_ERROR(pnMatchArray);
    KGLOG_PROCESS_ERROR(pszText);
    KGLOG_PROCESS_ERROR(pszTestPath);

    for (int i = 0; i < nMatchArrayLen; ++i)
    {
        int  nIndex       = 0;
        int  nLeft        = 0;
        nIndex = pnMatchArray[i];

        for (int j = 0; j < nTestPathLen; j++)
        {
            m_pszOutputText[nOutLen++] = pszTestPath[j];
        }

        for (int j = nIndex; j >= 0; j--)
        {
            if (pszText[j] == '\n')
            {
                nLeft = j + 1;
                break;
            }
        }
        for (int j = nLeft; j <= nIndex; j++)
        {
            m_pszOutputText[nOutLen++] = pszText[j];
        }

        for (int j = nIndex + 1; j < nTextLen; j++)
        {
            if (pszText[j] == '\n')
            {
                break;
            }
            m_pszOutputText[nOutLen++] = pszText[j];
        }

        m_pszOutputText[nOutLen++] = '\n';
    }

    nRetCode = (int)fwrite(m_pszOutputText, 1, nOutLen, stdout);
    KGLOG_PROCESS_ERROR(nRetCode == nOutLen);

    bResult = true;
Exit0:

    return bResult;
}

bool KBlockMatch::IsReadComplete()
{
    bool bReadComplete = true;

    for (int j = 0; j < THREAD_NUM; j++)
    {
        std::swap(m_pszReadText[j], m_pszPerformText[j]);
        m_nPerformTextLen[j] = m_nReadTextLen[j];

        if (m_nReadTextLen[j] > 1)
        {
            bReadComplete = false;
        }
    }
    return bReadComplete;
}
