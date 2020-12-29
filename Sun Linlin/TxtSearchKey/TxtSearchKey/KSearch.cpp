#include "KSearch.h"

inline int Min(int nNum1, int nNum2)
{
    if (nNum1 < nNum2)
    {
        return nNum1;
    }
    else
    {
        return nNum2;
    }
}

KSearch::KSearch()
{
    m_pszFilePath          = NULL;
    m_pszPattern           = NULL;
    m_pszFirstReadBuffer   = NULL;
    m_pszSecondReadBuffer  = NULL;
    m_nPatternLen          = 0;
}

KSearch::~KSearch()
{
    UnInit();
}

bool KSearch::Init(char* pszPattern)
{
    bool  bResult  = false;

    m_pszPattern = pszPattern;
    KGLOG_PROCESS_ERROR(m_pszPattern);

    m_nPatternLen = strlen(m_pszPattern);
    KGLOG_PROCESS_ERROR(m_nPatternLen);

    bResult = true;
Exit0:
    return bResult;
}

void KSearch::UnInit()
{
    m_nPatternLen       = 0;
    m_pszFilePath       = NULL;
    m_pszPattern        = NULL;

    if (m_pszFirstReadBuffer)
    {
        delete[] m_pszFirstReadBuffer;
        m_pszFirstReadBuffer = NULL;
    }

    if (m_pszSecondReadBuffer)
    {
        delete[] m_pszSecondReadBuffer;
        m_pszSecondReadBuffer = NULL;
    }
}


bool KSearch::SearchBm(char* pszFilePath)
{
    bool      bResult          = false;
    bool      bRetCode         = false;
    bool      bFirstBuffer     = true;
    int       nBufferLen       = 0;
    int       nFirstSize       = 0;
    int       nLastBufferPos   = 0;
    int       nSecondSize      = 0;
    int       nBufferPos       = 0;
    clock_t   lStart           = 0;
    clock_t   lEnd             = 0;
    FILE*     fpFile           = NULL;
    KBm*      pBmTest          = NULL;

    KGLOG_PROCESS_ERROR(pszFilePath);

    lStart = clock();

    pBmTest = new KBm();
    KGLOG_PROCESS_ERROR(pBmTest);


    fpFile = fopen(pszFilePath, "rb");
    KGLOG_PROCESS_ERROR(fpFile);

    m_pszFirstReadBuffer = new char[MAX_BUFFER_SIZE + MAX_PATTERN + 1];
    KGLOG_PROCESS_ERROR(m_pszFirstReadBuffer);
    KGLOG_PROCESS_ERROR(m_nPatternLen <= nFirstSize);

    nFirstSize = fread(m_pszFirstReadBuffer, sizeof(char),
        MAX_BUFFER_SIZE, fpFile);
    KGLOG_PROCESS_ERROR(nFirstSize >= 0);
    m_pszFirstReadBuffer[nFirstSize] = '\0';

    bRetCode = pBmTest->Init(m_pszPattern, m_nPatternLen);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (nFirstSize < MAX_BUFFER_SIZE)
    {
        bRetCode = pBmTest->Search(m_pszPattern, m_nPatternLen,
            m_pszFirstReadBuffer, nFirstSize);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        m_pszSecondReadBuffer = new char[MAX_BUFFER_SIZE + MAX_PATTERN + 1];
        KGLOG_PROCESS_ERROR(m_pszSecondReadBuffer);

        while (true)
        {
            if (nFirstSize < MAX_BUFFER_SIZE)
            {
                bRetCode = pBmTest->Search(m_pszPattern, m_nPatternLen, m_pszFirstReadBuffer, nFirstSize);
                KGLOG_PROCESS_ERROR(bRetCode);
                break;
            }
            nSecondSize = fread(m_pszSecondReadBuffer, sizeof(char), MAX_BUFFER_SIZE, fpFile);
            m_pszSecondReadBuffer[nSecondSize] = '\0';
            KGLOG_PROCESS_ERROR(nSecondSize >= 0);

            nBufferPos = nFirstSize;
            if (m_pszFirstReadBuffer[nFirstSize - 1] != '\n')
            {
                nBufferLen = nFirstSize + Min(MAX_PATTERN, nSecondSize);

                for (; nBufferPos < nBufferLen; nBufferPos++)
                {
                    m_pszFirstReadBuffer[nBufferPos] = m_pszSecondReadBuffer[nBufferPos - nFirstSize];
                    if (m_pszSecondReadBuffer[nBufferPos - nFirstSize] == '\n')
                    {
                        break;
                    }
                }
            }

            m_pszFirstReadBuffer[nBufferPos] = '\0';
            bRetCode = pBmTest->Search(m_pszPattern, m_nPatternLen, m_pszFirstReadBuffer + nLastBufferPos,
                nBufferPos + 2 - nLastBufferPos);
            KGLOG_PROCESS_ERROR(bRetCode);
            nLastBufferPos = nBufferPos - nFirstSize;
            /*if (bFirstBuffer)
            {
                if (nFirstSize < MAX_BUFFER_SIZE)
                {
                    bRetCode = pBmTest->Search(m_pszPattern, m_nPatternLen,
                        m_pszFirstReadBuffer, nFirstSize);
                    KGLOG_PROCESS_ERROR(bRetCode);
                    break;
                }
                nSecondSize = fread(m_pszSecondReadBuffer, sizeof(char),
                    MAX_BUFFER_SIZE, fpFile);
                KGLOG_PROCESS_ERROR(nSecondSize >= 0);
                m_pszSecondReadBuffer[nSecondSize] = '\0';
                bFirstBuffer = false;
                if (m_pszFirstReadBuffer[nFirstSize - 1] == '\n')
                {
                    bRetCode = pBmTest->Search(m_pszPattern, m_nPatternLen,
                        m_pszFirstReadBuffer, nFirstSize + 1);
                    KGLOG_PROCESS_ERROR(bRetCode);
                    nLastBufferPos = 0;
                    continue;
                }
                nBufferLen = nFirstSize + Min(MAX_PATTERN, nSecondSize);
                for (nBufferPos = nFirstSize; nBufferPos < nBufferLen;
                    nBufferPos++)
                {
                    m_pszFirstReadBuffer[nBufferPos] =
                        m_pszSecondReadBuffer[nBufferPos - nFirstSize];

                    if (m_pszSecondReadBuffer[nBufferPos - nFirstSize] == '\n')
                    {
                        break;
                    }
                }

                m_pszFirstReadBuffer[nBufferPos + 1] = '\0';
                bRetCode = pBmTest->Search(m_pszPattern, m_nPatternLen,
                    m_pszFirstReadBuffer + nLastBufferPos,
                    nBufferPos + 2 - nLastBufferPos);
                KGLOG_PROCESS_ERROR(bRetCode);

                nLastBufferPos = nBufferPos - nFirstSize + 1;
            }
            else
            {
                if (nSecondSize < MAX_BUFFER_SIZE)
                {
                    bRetCode = pBmTest->Search(m_pszPattern, m_nPatternLen,
                        m_pszSecondReadBuffer, nSecondSize);
                    KGLOG_PROCESS_ERROR(bRetCode);

                    break;
                }

                nFirstSize = fread(m_pszFirstReadBuffer, sizeof(char),
                    MAX_BUFFER_SIZE, fpFile);
                KGLOG_PROCESS_ERROR(nFirstSize);
                m_pszFirstReadBuffer[nFirstSize] = '\0';

                bFirstBuffer = true;

                if (m_pszSecondReadBuffer[nSecondSize - 1] == '\n')
                {
                    bRetCode = pBmTest->Search(m_pszPattern, m_nPatternLen,
                        m_pszSecondReadBuffer, nSecondSize + 1);
                    KGLOG_PROCESS_ERROR(bRetCode);

                    nLastBufferPos = 0;
                    continue;
                }

                nBufferLen = nSecondSize + Min(nFirstSize, MAX_PATTERN);
                for (nBufferPos = nSecondSize; nBufferPos < nBufferLen;
                    nBufferPos++)
                {
                    m_pszSecondReadBuffer[nBufferPos] =
                        m_pszFirstReadBuffer[nBufferPos - nSecondSize];

                    if (m_pszFirstReadBuffer[nBufferPos - nSecondSize] == '\n')
                    {
                        break;
                    }

                }

                m_pszSecondReadBuffer[nBufferPos + 1] = '\0';
                bRetCode = pBmTest->Search(m_pszPattern, m_nPatternLen,
                    m_pszSecondReadBuffer + nLastBufferPos,
                    nBufferPos + 2 - nLastBufferPos);
                KGLOG_PROCESS_ERROR(bRetCode);

                nLastBufferPos = nBufferPos - nFirstSize + 1;
            }*/
        }
    }

    lEnd = clock() - lStart;
    std::cout << "KeyWord :" << m_pszPattern << "  FilePath :" << pszFilePath << std::endl;
    std::cout << "All Use Time :" << lEnd << std::endl;
    std::cout << "Read Time :" << lEnd - pBmTest->GetMatchTime()<< std::endl;
    std::cout << "Match Time:" << pBmTest->GetMatchTime() << std::endl;

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (m_pszFirstReadBuffer)
        {
            delete[] m_pszFirstReadBuffer;
            m_pszFirstReadBuffer = NULL;
        }

        if (m_pszSecondReadBuffer)
        {
            delete[] m_pszSecondReadBuffer;
            m_pszSecondReadBuffer = NULL;
        }
    }

    if (pBmTest)
    {
        delete pBmTest;
        pBmTest = NULL;
    }

    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }

    return bResult;
}
