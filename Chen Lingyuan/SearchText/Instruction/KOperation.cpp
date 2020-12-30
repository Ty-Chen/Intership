﻿#include "KOperation.h"

#include<time.h>
#include<io.h>

#include<iostream>
#include<sstream>
#include<vector>
#include<string>

KOperation::KOperation()
{

}

KOperation::~KOperation()
{

}

void KOperation::Init()
{
    m_szFileList.clear();
}

void KOperation::CommandInput()
{
    bool bRetCode         = false;
    std::string szCommand = "";

    while (true)
    {
        std::getline(std::cin, szCommand);

        if (szCommand == "End")
        {
            break;
        }

        bRetCode = CommandParse(szCommand);
        if (!bRetCode)
        {
            printf("Command cannot be resolved\n");
        }

        printf("\n");
    }
}

bool KOperation::CommandParse(std::string szCommand)
{
    bool                      bResult             = false;
    bool                      bRetCode            = false;
    int                       nCommandLen         = 0;
    int                       nBeforeIndex        = -1;
    int                       nAfterIndex         = -1;
    std::string               szTempResult        = "";
    std::string               szFrontPartCommand  = "";
    std::string               szLatterPartCommand = "";
    std::string               szPattern           = "";
    std::stringstream         sStream;
    std::vector <std::string> szCommandResult;

    nCommandLen = szCommand.length();

    for (int i = 0; i < nCommandLen; i++)
    {
        if (szCommand[i] == '\'')
        {
            nBeforeIndex = i;
            break;
        }
    }

    for (int i = nCommandLen - 1; i >= 0; i--)
    {
        if (szCommand[i] == '\'')
        {
            nAfterIndex = i;
            break;
        }
    }

    if (nBeforeIndex >= nAfterIndex)
    {
        goto Exit0;
    }

    for (int i = nBeforeIndex + 1; i <= nAfterIndex - 1; i++)
    {
        szPattern += szCommand[i];
    }

    szFrontPartCommand = szCommand.substr(0, nBeforeIndex);
    szLatterPartCommand = szCommand.substr(nAfterIndex + 1, nCommandLen - nAfterIndex - 1);

    sStream = std::stringstream(szFrontPartCommand);
    while (sStream >> szTempResult)
    {
        szCommandResult.push_back(szTempResult);
    }

    sStream = std::stringstream(szLatterPartCommand);
    while (sStream >> szTempResult)
    {
        szCommandResult.push_back(szTempResult);
    }

    if (szCommandResult.size() != 4)
    {
        goto Exit0;
    }

    if (szCommandResult[0] == "search" && szCommandResult[2] == ">")
    {
        bRetCode = OutputToDoucment(
            (unsigned char*)szPattern.c_str(),
            (char*)szCommandResult[1].c_str(),
            (char*)szCommandResult[3].c_str()
        );
        if (!bRetCode)
        {
            goto Exit0;
        }
    }

    else if(szCommandResult[0]=="time" && szCommandResult[1] == "search" && szCommandResult[2] == "-c")
    {
        bRetCode = OutputMatchResult((unsigned char*)szPattern.c_str(), szCommandResult[3]);
        if (!bRetCode)
        {
            goto Exit0;
        }
    }

    else
    {
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

bool KOperation::OutputMatchResult(unsigned char* pszPattern, std::string szSourcePath)
{
    bool    bResult          = false;
    bool    bRetCode         = false;
    int     nSumMatchCount   = 0;
    int     nFileCount       = 0;
    clock_t nStart           = 0;
    clock_t nEnd             = 0;
    KBlockMatch* pBlockMatch = NULL;

    KGLOG_PROCESS_ERROR(pszPattern);

    pBlockMatch = new KBlockMatch();
    KGLOG_PROCESS_ERROR(pBlockMatch);

    bRetCode = pBlockMatch->Init();
    KGLOG_PROCESS_ERROR(bRetCode);

    nStart = clock();

    GetFiles(szSourcePath);
    nFileCount = m_szFileList.size();
    if (nFileCount == 0)
    {
        printf("The File does not Exist\n");
        goto Exit0;
    }
    for (int i = 0; i < nFileCount; i++)
    {
        int nMatchCount = 0;
        nMatchCount = pBlockMatch->MatchWords(pszPattern, (char*)m_szFileList[i].c_str());
        KGLOG_PROCESS_ERROR(nMatchCount >= 0);

        nSumMatchCount += nMatchCount;
        printf("%s : %d\n", (char*)m_szFileList[i].c_str(), nMatchCount);
    }

    nEnd = clock();

    KGLOG_PROCESS_ERROR(nSumMatchCount >= 0);

    printf("Match : %d\n", nSumMatchCount);
    printf("Time: %d ms\n", nEnd - nStart);

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

bool KOperation::OutputToDoucment(unsigned char* pszPattern, std::string szSourcePath, std::string szTargetPath)
{
    bool bResult             = false;
    bool bRetCode            = false;
    int  nFileCount          = 0;
    KBlockMatch* pBlockMatch = NULL;

    KGLOG_PROCESS_ERROR(pszPattern);

    pBlockMatch = new KBlockMatch();
    KGLOG_PROCESS_ERROR(pBlockMatch);

    bRetCode = pBlockMatch->Init();
    KGLOG_PROCESS_ERROR(bRetCode);

    GetFiles(szSourcePath);
    nFileCount = m_szFileList.size();

    if (nFileCount == 0)
    {
        printf("The File does not Exist\n");
        goto Exit0;
    }

    for (int i = 0; i < nFileCount; i++)
    {
        int nMatchCount      = 0;
        long long llFileSize = 0;

        nMatchCount = pBlockMatch->MatchWords(pszPattern, (char*)m_szFileList[i].c_str());
        KGLOG_PROCESS_ERROR(nMatchCount >= 0);

        llFileSize = pBlockMatch->TraversBlock((char*)m_szFileList[i].c_str(), (char*)szTargetPath.c_str());
        KGLOG_PROCESS_ERROR(llFileSize >= 0);
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

void KOperation::GetFiles(std::string szPath)
{
    long               hFile      = 0;
    struct _finddata_t FileInfo;
    std::string        szFullPath = "";
    FILE*              fpFile     =  NULL;

    fpFile = fopen((char*)szPath.c_str(), "rb");
    if (fpFile)
    {
        m_szFileList.push_back(szPath);
        fclose(fpFile);
        fpFile = NULL;
    }
    else
    {
        if ((hFile = _findfirst(szFullPath.assign(szPath).append("\\*").c_str(), &FileInfo)) != -1)
        {
            do
            {
                if ((FileInfo.attrib &  _A_SUBDIR))
                {
                    if (strcmp(FileInfo.name, ".") != 0 && strcmp(FileInfo.name, "..") != 0)
                        GetFiles(szFullPath.assign(szPath).append("\\").append(FileInfo.name));
                }
                else
                {
                    m_szFileList.push_back(szFullPath.assign(szPath).append("\\").append(FileInfo.name));
                }
            } while (_findnext(hFile, &FileInfo) == 0);
            _findclose(hFile);
        }
    }
}
