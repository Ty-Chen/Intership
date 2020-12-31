#include "KBlockMatch.h"

#include<Windows.h>
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

    m_pszOutputText = new unsigned char[BUFFER_SIZE];
    KGLOG_PROCESS_ERROR(m_pszOutputText);

    bResult = true;
Exit0:
    if (!bResult)
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

    for (unsigned int i = 0; i < m_FileBlock.size(); i++)
    {
        if (m_FileBlock[i].pnMatchArray)
        {
            delete []m_FileBlock[i].pnMatchArray;
            m_FileBlock[i].pnMatchArray = NULL;
        }
    }
    m_FileBlock.clear();
}

int KBlockMatch::MatchWords(unsigned char* pszPattern, char* pszTestPath)
{


    bool      bResult          = false;
    bool      bRetCode         = false;
    bool      bReadComplete    = false;
    int       nMatchCount      = 0;
    int       nPatternLen      = 0;
    KSunday*  pSundayTest      = NULL;
    FILE*     fpFile           = NULL;

    KGLOG_PROCESS_ERROR(pszPattern);
    KGLOG_PROCESS_ERROR(pszTestPath);

    pSundayTest = new KSunday();
    KGLOG_PROCESS_ERROR(pSundayTest);

    fpFile = fopen(pszTestPath, "rb");
    if (!fpFile)
    {
        goto Exit0;
    }

    nPatternLen = strlen((char*)pszPattern);
    KGLOG_PROCESS_ERROR(nPatternLen <= PATTERN_SIZE);

    bRetCode = pSundayTest->Init(pszPattern, nPatternLen);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = ReadFileBlock(fpFile);
    KGLOG_PROCESS_ERROR(bRetCode);

    bReadComplete = IsReadComplete();

    for (int i = 0; !bReadComplete; i++)
    {
        KPART_OF_FILE TempFileBlock[THREAD_NUM];
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

            KGLOG_PROCESS_ERROR(nCount >= 0);

            TempFileBlock[j].pnMatchArray = NULL;
            TempFileBlock[j].nTextLen = m_nPerformTextLen[j] - 1;

            TempFileBlock[j].nMatchArrayLen = nCount;
            nMatchCount += nCount;

            TempFileBlock[j].pnMatchArray = new int[nCount];
            KGLOG_PROCESS_ERROR(TempFileBlock[j].pnMatchArray);

            memcpy(TempFileBlock[j].pnMatchArray, m_pnTempMatch[j], nCount * sizeof(int));

            TempFileBlock[j].pText = pszTestPath;

            m_FileBlock.push_back(TempFileBlock[j]);

            fMatch[j].~future();
        }

        bReadComplete = IsReadComplete();
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        for (unsigned int i = 0; i < m_FileBlock.size(); i++)
        {
            if (m_FileBlock[i].pnMatchArray)
            {
                delete[]m_FileBlock[i].pnMatchArray;
                m_FileBlock[i].pnMatchArray = NULL;
            }
        }
        m_FileBlock.clear();

        nMatchCount = -1;
    }

    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }

    if (pSundayTest)
    {
        delete pSundayTest;
        pSundayTest = NULL;
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
        nReadSize = fread(m_pszReadText[i], 1, BUFFER_SIZE, fpFile);
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

bool KBlockMatch::CollectDate(
    int* pnMatchArray, int nMatchArrayLen,
    unsigned char* pszText, int nTextLen,
    FILE* fpResult, char* pszTestPath
)
{
    bool  bResult      = false;
    int   nRetCode     = 0;
    int   nOutLen      = 0;
    int   nTestPathLen = 0;
    char* pszMatchTest = NULL;

    KGLOG_PROCESS_ERROR(pnMatchArray);
    KGLOG_PROCESS_ERROR(pszText);
    KGLOG_PROCESS_ERROR(fpResult);
    KGLOG_PROCESS_ERROR(pszTestPath);

    pszMatchTest = new char[BUFFER_SIZE * 3];
    KGLOG_PROCESS_ERROR(pszMatchTest);

    nTestPathLen = strlen(pszTestPath);

    for (int i = 0; i < nMatchArrayLen; ++i)
    {
        int  nIndex       = 0;
        int  nLeft        = 0;
        nIndex = pnMatchArray[i];

        for (int j = 0; j < nTestPathLen; j++)
        {
            pszMatchTest[nOutLen++] = pszTestPath[j];
        }
        pszMatchTest[nOutLen++] = ':';

        for (int j = nIndex; j >= 0; j--)
        {
            if (pszText[j] == '\n' || pszText[j] == '\r')
            {
                nLeft = j + 1;
                break;
            }
        }
        for (int j = nLeft; j <= nIndex; j++)
        {
            pszMatchTest[nOutLen++] = pszText[j];
        }

        for (int j = nIndex + 1; j < nTextLen; j++)
        {
            if (pszText[j] == '\n' || pszText[j] == '\r')
            {
                break;
            }
            pszMatchTest[nOutLen++] = pszText[j];
        }

        pszMatchTest[nOutLen++] = '\n';
    }

    nRetCode = fwrite(pszMatchTest, 1, nOutLen, fpResult);
    KGLOG_PROCESS_ERROR(nRetCode == nOutLen);

    bResult = true;
Exit0:

    if (pszMatchTest)
    {
        delete []pszMatchTest;
        pszMatchTest = NULL;
    }
    return bResult;
}

long long KBlockMatch::TraversBlock(char* pszTestPath,char* pszResultPath)
{
    bool         bResult     = false;
    bool         bRetCode    = false;
    int          nRetCode    = 0;
    int          nBlockSize  = 0;
    long long    llFilesize  = 0;
    FILE*        fpFile      = NULL;
    FILE*        fpResult    = NULL;
    unsigned int nBlockCount = 0;

    KGLOG_PROCESS_ERROR(pszTestPath);
    KGLOG_PROCESS_ERROR(pszResultPath);

    fpFile = fopen(pszTestPath, "rb");
    KGLOG_PROCESS_ERROR(fpFile);

    fpResult = fopen(pszResultPath,"w");
    if (!fpResult)
    {
        goto Exit0;
    }

    nBlockCount = m_FileBlock.size();
    for (unsigned int i = 0; i < nBlockCount; i++)
    {
        KPART_OF_FILE FileBlock;
        FileBlock = m_FileBlock[i];

        llFilesize += FileBlock.nTextLen;

        if (FileBlock.pnMatchArray && FileBlock.nMatchArrayLen > 0)
        {
            nBlockSize = fread(m_pszOutputText, 1, FileBlock.nTextLen, fpFile);
            KGLOG_PROCESS_ERROR(nBlockSize == FileBlock.nTextLen);

            bRetCode = CollectDate(
                FileBlock.pnMatchArray, FileBlock.nMatchArrayLen,
                m_pszOutputText, nBlockSize, fpResult, pszTestPath
            );
        }
        else
        {
            nRetCode = fseek(fpFile, FileBlock.nTextLen, SEEK_CUR);
            KGLOG_PROCESS_ERROR(nRetCode >= 0);
        }
    }

    bResult = true;
Exit0:
    if (!bResult)
    {

        llFilesize = -1;
    }

    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }

    if (fpResult)
    {
        fclose(fpResult);
        fpResult = NULL;
    }

    for (unsigned int i = 0; i < m_FileBlock.size(); i++)
    {
        if (m_FileBlock[i].pnMatchArray)
        {
            delete []m_FileBlock[i].pnMatchArray;
            m_FileBlock[i].pnMatchArray = NULL;
        }
    }
    m_FileBlock.clear();

    return llFilesize;
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
