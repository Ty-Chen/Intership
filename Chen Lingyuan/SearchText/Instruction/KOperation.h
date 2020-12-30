#pragma once
#ifndef KOPERATION_H_
#define KOPERATION_H_

#include<string>
#include<vector>

#include<KBlockMatch.h>
#include"KMacro.h"

class KOperation
{
public:
    KOperation();
    ~KOperation();

    void Init();

    void CommandInput();

private:
    bool CommandParse(std::string szCommand);

    bool OutputMatchResult(unsigned char* pszPattern, std::string szSourcePath);

    bool OutputToDoucment(unsigned char* pszPattern, std::string szSourcePath, std::string szTargetPath);

    void GetFiles(std::string szPath);

private:
    std::vector <std::string> m_szFileList;
};
#endif
