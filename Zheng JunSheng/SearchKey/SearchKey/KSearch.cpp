#include "KSearch.h"

#include <time.h>
#include <stdio.h>
#include <string.h>

#include "KSunday.h"

KSearch::KSearch()
{

}

KSearch::~KSearch()
{
    UnInit();
}

bool KSearch::Init(
    int   nKeyLen, char* pszKeyWord,
    char* pszFilePath
)
{
    bool  bResult = false;

    KGLOG_PROCESS_ERROR(nKeyLen);
    KGLOG_PROCESS_ERROR(pszKeyWord);
    KGLOG_PROCESS_ERROR(pszFilePath);

    m_nKeyLen     = nKeyLen;
    m_nCntResult  = 0;
    m_pszKeyWord  = pszKeyWord;
    m_pszFilePath = pszFilePath;

    bResult = true;
Exit0:
    return bResult;
}

void KSearch::UnInit()
{
    m_nKeyLen     = NULL;
    m_nCntResult  = 0;
    m_pszKeyWord  = NULL;
    m_pszFilePath = NULL;
}

bool KSearch::RunSunday()
{
    bool     bResult       = false;
    int      nRetCode      = 0;
    int      nOffset       = 0;
    int      nReadBuffLen  = 0;
    bool     bIsFinishFile = false;
    char*    pszReadBuff   = NULL;
    FILE*    fpFile        = NULL;
    KSunday* pSunday       = NULL;

    fpFile = fopen(m_pszFilePath, "rb");
    KGLOG_PROCESS_ERROR(fpFile);

    pSunday = new KSunday();
    KGLOG_PROCESS_ERROR(pSunday);

    nRetCode = pSunday->Init((unsigned char*)m_pszKeyWord, m_nKeyLen);
    KGLOG_PROCESS_ERROR(nRetCode);

    pszReadBuff = new char[MAX_BUFFER + 1];
    KGLOG_PROCESS_ERROR(pszReadBuff);

    while (true)
    {
        nReadBuffLen = fread(pszReadBuff, 1, MAX_BUFFER, fpFile);
        KGLOG_PROCESS_ERROR(nReadBuffLen >= 0);
        pszReadBuff[nReadBuffLen] = '\0';

        if (nReadBuffLen == 0)
        {
            break;
        }

        bIsFinishFile = feof(fpFile);
        if (pszReadBuff[nReadBuffLen - 1] != '\n' && !bIsFinishFile)
        {
            for (int i = nReadBuffLen - 1; i >= 0; --i)
            {
                if (pszReadBuff[i] == '\n')
                {
                    nOffset = nReadBuffLen - i;
                    nReadBuffLen = i;
                    break;
                }
            }

            nRetCode = fseek(fpFile, -nOffset, SEEK_CUR);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }

        nRetCode = pSunday->Run((unsigned char*)pszReadBuff, nReadBuffLen);
        KGLOG_PROCESS_ERROR(nRetCode > 0);

        if (bIsFinishFile)
        {
            break;
        }
    }

    m_nCntResult = pSunday->m_nCntResult;

    bResult = true;
Exit0:
    if (pszReadBuff)
    {
        delete []pszReadBuff;
        pszReadBuff = NULL;
    }

    if (pSunday)
    {
        delete pSunday;
        pSunday = NULL;
    }

    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }

    return bResult;
}