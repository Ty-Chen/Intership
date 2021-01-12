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
    void UnInit();

    bool CommandInput(int argc, char* argv[]);

private:
    int Getopt(int argc, char* argv[], const char* pszOpstring, int nIndex);

    bool Search(unsigned char* pszPattern, char* pszSourcePath);

    bool GetFiles(std::string szPath);
private:
    std::vector <std::string> m_szFileList;

    char* m_pszOptarg;
};
#endif
