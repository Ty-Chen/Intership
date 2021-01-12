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

    nRestultChar = Getopt(argc, argv, "hp:f:", nIndex);

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
            "usage: Search  KeyWord LogPath\n"
            "sample: Search -pKGLOG  -fE:/TestText/Log\n"
            "\nexplain:\n"
            "Two parameters, the first for the query keyword, the second for the query text path\n"
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

int KOperation:: Getopt(int argc, char* argv[], const char* pszOpstrintg, int nIndex)
{
    bool bResult              = false;
    int  nCommandChar         = 0;
    char* pParameterStart     = NULL;

    KGLOG_PROCESS_ERROR(argv);
    KGLOG_PROCESS_ERROR(pszOpstrintg);

    if (nIndex >= argc || argv[nIndex][0] != '-' || argv[nIndex][1] == '\0' || argv[nIndex][1] == ':')
    {
        goto Exit0;
    }

    nCommandChar = argv[nIndex][1];

    pParameterStart = (char*)strchr(pszOpstrintg, (char)nCommandChar);

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

    nFileCount = m_szFileList.size();
    if (nFileCount == 0)
    {
        printf("File does Exist\n");
    }

    KGLOG_PROCESS_ERROR(nFileCount > 0);

    for (int i = 0; i < nFileCount; i++)
    {
        printf("%s\n", m_szFileList[i].c_str());
    }

    /*for (int i = 0; i < nFileCount; i++)
    {
        int nMatchCount = 0;
        nMatchCount = pBlockMatch->MatchWords(
            pszPattern, m_szFileList[i]
            );
        KGLOG_PROCESS_ERROR(nMatchCount >= 0);
    }*/

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
    bool bResult              = false;
    int  nRetCode             = 0;
    std::string szFullCommand = "";
    FILE* fpFile              = NULL;
    char szFilePath[MAX_LINE];
    char TerminationChar;

#if WIN32
    szFullCommand = "dir /s /b ";
    szFullCommand.append(szPath + " | findstr / log > File.txt");
    TerminationChar = '\r';
#else
    szFullCommand = "find ";
    szFullCommand.append(szPath + "  | grep  \"\.log\" > File.txt");
    TerminationChar = '\n';
#endif

    nRetCode = std::system(szFullCommand.c_str());
    KGLOG_PROCESS_ERROR(nRetCode >= 0);

    fpFile = fopen("File.txt", "rb");
    KGLOG_PROCESS_ERROR(fpFile);
    while (fgets(szFilePath, MAX_LINE + 1, fpFile))
    {
        int         nFileNameLen   = strlen(szFilePath);
        std::string szFullFilePath = "";

        for (int i = nFileNameLen - 1; i >= 0; i--)
        {
            if (szFilePath[i] == TerminationChar)
            {
                szFilePath[i] = '\0';
            }
        }

        szFullFilePath.append(szFilePath);
        m_szFileList.push_back(szFullFilePath);
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
