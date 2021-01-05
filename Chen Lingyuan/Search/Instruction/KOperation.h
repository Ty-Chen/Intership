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

    bool CommandInput(int argc, char* argv[]);

private:
    bool Output(unsigned char* pszPattern, char* pszSourcePath);

    bool GetFiles(std::string szPath);
private:
    std::vector <std::string> m_szFileList;
};
#endif
