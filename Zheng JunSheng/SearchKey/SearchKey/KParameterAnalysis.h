#ifndef _PARAMETER_ANALYSIS_H_
#define _PARAMETER_ANALYSIS_H_

#include "DefineMarco.h"

#include <string>
#include <vector>

class KParameterAnalysis
{
public:
    KParameterAnalysis();
    ~KParameterAnalysis();

    void Init();
    void UnInit();

    bool CommandInput(int argc, char* argv[]);
private:
    int  GetOption(int argc, char* argv[], struct option* pLongOptions, int nOptind);
    void Output();
    bool GetWindowsFiles(char* pszDirectoryPath);
    bool GetLinuxFiles(char* pszPath);
    bool GetFiles(char* pszPath);
    bool Sunday(char* pszKeyWord, const char* pszFilePath);
private:
    std::vector<std::string> m_szFileList;
};
#endif

