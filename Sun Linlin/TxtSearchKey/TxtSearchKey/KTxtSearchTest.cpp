#include "KTxtSearchTest.h"

KTxtSearchTest::KTxtSearchTest()
{
    m_pszPattern        = NULL;
    m_nPatternPathLen   = 0;
    m_nTestNum          = 0;
    m_nFilePathNum      = 0;
}

KTxtSearchTest::~KTxtSearchTest()
{
    UnInit();
}

bool KTxtSearchTest::Init(const char* pszPatternPath, const char* pszPathFile, int nTestNum)
{
    bool   bResult  = false;
    bool   bRetCode = false;

    KGLOG_PROCESS_ERROR(pszPatternPath);
    KGLOG_PROCESS_ERROR(pszPathFile);

    m_nTestNum = nTestNum;
    KGLOG_PROCESS_ERROR(m_nTestNum <= MAX_TEST_NUM);

    bRetCode = TxtFilePathInput(pszPathFile);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = PatternInput(pszPatternPath);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

void KTxtSearchTest::UnInit()
{
    m_nFilePathNum    = 0;
    m_nTestNum        = 0;
    m_nPatternPathLen = 0;

    if (m_pszPattern)
    {
        delete[] m_pszPattern;
        m_pszPattern = NULL;
    }
}

bool KTxtSearchTest::TestRun()
{
    bool          bResult         = false;
    bool          bRetCode        = false;
    int           nTestNum        = m_nTestNum;
    int           nPatternPos     = 0;
    KSearch*      pSearch         = NULL;
    clock_t       lStart          = 0;
    clock_t       lEnd            = 0;
    char          szPattern[MAX_PATTERN];

    lStart = clock();
    pSearch = new KSearch();
    KGLOG_PROCESS_ERROR(pSearch);

    for (int i = 0; i < m_nFilePathNum; i++)
    {
        std::cout << "FilePathName: " << m_szFilePath[i] << std::endl;

        nPatternPos = 0;
        nTestNum    = m_nTestNum;

        for (int j = 0; j <= m_nPatternPathLen; j++)
        {
            KGLOG_PROCESS_ERROR((j - nPatternPos) < MAX_PATTERN);
            szPattern[j - nPatternPos] = m_pszPattern[j];

            if (m_pszPattern[j] == '\r' || m_pszPattern[j] == '\0')
            {
                szPattern[j - nPatternPos] = '\0';

                while (nTestNum-- > 0)
                {
                    bRetCode = pSearch->Init(szPattern);
                    KGLOG_PROCESS_ERROR(bRetCode);

                    bRetCode = pSearch->SearchBmTwo(m_szFilePath[i]);
                    KGLOG_PROCESS_ERROR(bRetCode);
                }

                nPatternPos = j + 2;
                if (nPatternPos >= m_nPatternPathLen)
                {
                    break;
                }

                nTestNum = m_nTestNum;
                continue;
            }

        }
    }

    lEnd = clock() - lStart;

    std::cout << lEnd << std::endl;

    bResult = true;
Exit0:
    if (pSearch)
    {
        delete pSearch;
        pSearch = NULL;
    }

    return bResult;
}

bool KTxtSearchTest::TxtFilePathInput(const char* pszFilePath)
{
    bool   bResult        = false;
    int    nRetCode       = 0;
    int    nFilePos       = 0;
    int    nFilePathLen   = 0;
    FILE*  fpFile         = NULL;

    KGLOG_PROCESS_ERROR(pszFilePath);

    fpFile = fopen(pszFilePath, "rb");
    KGLOG_PROCESS_ERROR(fpFile);

    while (fgets(m_szFilePath[nFilePos], MAX_PATHLEN, fpFile))
    {
        nRetCode = ferror(fpFile);
        KGLOG_PROCESS_ERROR(nRetCode == 0);
        nFilePos++;
    }
    for (int i = 0; i < nFilePos; i++)
    {
        nFilePathLen = strlen(m_szFilePath[i]);
        KGLOG_PROCESS_ERROR(nFilePathLen >= 0);
        if (i == (nFilePos - 1))
        {
            break;
        }
        m_szFilePath[i][nFilePathLen - 2] = '\0';
    }
    m_nFilePathNum = nFilePos;

    bResult = true;
Exit0:
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }
    return bResult;
}

bool KTxtSearchTest::PatternInput(const char* pszPatternPath)
{
    bool  bResult     = false;
    int   nRetCode    = 0;
    int   nPatternPos = 0;
    FILE* fpFile      = NULL;

    KGLOG_PROCESS_ERROR(pszPatternPath);

    fpFile = fopen(pszPatternPath, "rb");
    KGLOG_PROCESS_ERROR(fpFile);

    nPatternPos = fseek(fpFile, 0L, SEEK_END);
    KGLOG_PROCESS_ERROR(nPatternPos == 0);

    m_nPatternPathLen = (int)ftell(fpFile);
    KGLOG_PROCESS_ERROR(m_nPatternPathLen >= 0);

    nPatternPos = fseek(fpFile, 0L, SEEK_SET);
    KGLOG_PROCESS_ERROR(nPatternPos == 0);

    m_pszPattern = new char[m_nPatternPathLen + 1];
    KGLOG_PROCESS_ERROR(m_pszPattern);

    nRetCode = fread(m_pszPattern, sizeof(char), m_nPatternPathLen, fpFile);
    KGLOG_PROCESS_ERROR(nRetCode == m_nPatternPathLen);

    m_pszPattern[m_nPatternPathLen] = '\0';

    bResult = true;
Exit0:
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }

    if (!bResult)
    {
        if (m_pszPattern)
        {
            delete[] m_pszPattern;
            m_pszPattern = NULL;
        }
    }
    return bResult;
}
