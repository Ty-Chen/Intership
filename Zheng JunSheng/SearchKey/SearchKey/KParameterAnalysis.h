#ifndef _PARAMETER_ANALYSIS_H_
#define _PARAMETER_ANALYSIS_H_

#include "DefineMarco.h"

class KParameterAnalysis
{
public:
    KParameterAnalysis();
    ~KParameterAnalysis();

    bool GetOption(int argc, char* argv[]);
private:
    void OutHelp();
    bool Sunday(char* pszKeyWord, char* pszFilePath);
};
#endif

