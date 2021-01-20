#include "KOperation.h"

#ifdef WIN32
#include<io.h>
#include<fcntl.h>
#else
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#endif

#include<string.h>
#include<stdlib.h>
#include<stdio.h>

#include<iostream>
#include<vector>
#include<queue>

KOperation::KOperation()
{

}

KOperation::~KOperation()
{

}

void KOperation::Init()
{
    m_pszOptarg = NULL;

    m_szFileList.clear();
}

void KOperation::UnInit()
{
    m_pszOptarg = NULL;

    m_szFileList.clear();
}

bool KOperation::CommandInput(int argc, char* argv[])
{
    bool bResult      = false;
    bool bRetCode     = false;
    bool bIsHelp      = false;
    int  nRestultChar = 0;
    int  nIndex       = 1;
    char* pszPattern  = NULL;
    char* pszTestPath = NULL;

    nRestultChar = Getopt(argc, argv, "hp:f:s:", nIndex);

    while (nRestultChar != -1)
    {
        switch (nRestultChar)
        {
            case 'h':
                bIsHelp = true;
                break;
            case 'p':
                pszPattern  = m_pszOptarg;
                break;
            case 'f':
                pszTestPath = m_pszOptarg;
                break;
            default:
                break;
        }
        nIndex++;
        nRestultChar = Getopt(argc, argv, "hp:f:", nIndex);
    }

    if (bIsHelp && !pszTestPath && !pszPattern)
    {
        printf(
            "usage: Search KeyWord LogPath\n"
            "sample: Search -pKGLOG -fE:\\TestText\\Log \n"
            "\nsample: Search -pKGLOG -fE:\\TestTextLog\\*.log\n"
            "\nsample: Search -pKGLOG -fE:\\TestText\\other\\Big_Size.txt\n"
            "\n If run it under Linux,replace '\' to '/'\n"
        );
    }
    else if (pszTestPath && pszPattern && !bIsHelp)
    {
        KGLOG_PROCESS_ERROR(strlen(pszPattern) <= PATTERN_SIZE);

#ifdef WIN32
        _setmode(fileno(stdout), O_BINARY);
#endif

        bRetCode = Search((unsigned char*)pszPattern, pszTestPath);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        printf("Command cannot be executed\n");
    }

    bResult = true;
Exit0:
    return bResult;
}

int KOperation::Getopt(int argc, char* argv[], const char* pszOpstring, int nIndex)
{
    bool bResult              = false;
    int  nCommandChar         = 0;
    char* pParameterStart     = NULL;

    KGLOG_PROCESS_ERROR(argv);
    KGLOG_PROCESS_ERROR(pszOpstring);

    if (nIndex >= argc || argv[nIndex][0] != '-' || argv[nIndex][1] == '\0' || argv[nIndex][1] == ':')
    {
        goto Exit0;
    }

    nCommandChar = argv[nIndex][1];

    pParameterStart = (char*)strchr(pszOpstring, (char)nCommandChar);

    if (pParameterStart == NULL)
    {
        goto Exit0;
    }
    if (*++pParameterStart == ':')
    {
        if (argv[nIndex][2] != '\0')
        {
            m_pszOptarg = &argv[nIndex][2];
        }
        else
        {
            goto Exit0;
        }
    }
    else
    {
        if (argv[nIndex][2] != '\0')
        {
            goto Exit0;
        }
        m_pszOptarg = NULL;
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        nCommandChar = -1;
    }

    return nCommandChar;
}

bool KOperation::Search(unsigned char* pszPattern, char* pszSourcePath)
{
    bool         bResult             = false;
    bool         bRetCode            = false;
    int          nFileCount          = 0;
    std::string  szSourcePath        = "";
    KBlockMatch* pBlockMatch         = NULL;

    KGLOG_PROCESS_ERROR(pszPattern);
    KGLOG_PROCESS_ERROR(pszSourcePath);

    szSourcePath.append(pszSourcePath);

    pBlockMatch = new KBlockMatch();
    KGLOG_PROCESS_ERROR(pBlockMatch);

    bRetCode = pBlockMatch->Init();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = GetFiles(szSourcePath);
    KGLOG_PROCESS_ERROR(bRetCode);

    nFileCount = (int)m_szFileList.size();
    if (nFileCount == 0)
    {
        printf("File does Exist\n");
    }

    for (int i = 0; i < nFileCount; i++)
    {
        printf("%s\n", m_szFileList[i].c_str());
    }

    KGLOG_PROCESS_ERROR(nFileCount > 0);

    for (int i = 0; i < nFileCount; i++)
    {
        int nMatchCount = 0;
        nMatchCount = pBlockMatch->MatchWords(
            pszPattern, m_szFileList[i]
            );
        KGLOG_PROCESS_ERROR(nMatchCount >= 0);
    }

    bResult = true;
Exit0:
    m_szFileList.clear();

    if (pBlockMatch)
    {
        delete pBlockMatch;
        pBlockMatch = NULL;
    }
    return bResult;
}

bool KOperation::GetFiles(std::string szPath)
{
    bool        bResult              = false;
    int         nRetCode             = 0;
    char        szFullCommand [MAX_LINE];
    std::string szSuffix             = "*";
    FILE*       fpFile               = NULL;
    char        szFilePath[MAX_LINE + 2];

#if WIN32
    snprintf(szFullCommand, MAX_LINE, "dir %s /a-d /s /b > File.txt", szPath.c_str());
#else
    int         nDotIndex      = 0;
    int         nDiagonalIndex = 0;

    nDotIndex      = (int)szPath.rfind(".");
    nDiagonalIndex = (int)szPath.rfind("/");

    if (nDotIndex >= 0 && nDiagonalIndex >= 0 && nDotIndex - nDiagonalIndex == 2 && szPath[nDiagonalIndex + 1] == '*')
    {
        szSuffix.assign(szPath.begin() + nDiagonalIndex + 1, szPath.end());
        szPath.assign(szPath.begin(), szPath.begin() + nDiagonalIndex);
    }

    snprintf(szFullCommand, MAX_LINE, "find %s -type f -name \"%s\" > File.txt", szPath.c_str(), szSuffix.c_str());
#endif

    nRetCode = std::system(szFullCommand);
    KGLOG_PROCESS_ERROR(nRetCode >= 0);

    fpFile = fopen("File.txt", "r");
    KGLOG_PROCESS_ERROR(fpFile);

    while (fgets(szFilePath, MAX_LINE + 2, fpFile))
    {
        int nFileNameLen = (int)strlen(szFilePath);

        if (szFilePath[nFileNameLen - 1] == '\n')
        {
            szFilePath[nFileNameLen - 1] = '\0';
        }

        m_szFileList.push_back(szFilePath);
    }

    bResult = true;
Exit0:
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }
    return bResult;
}
