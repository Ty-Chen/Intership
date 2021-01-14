#ifndef _PARAMETER_ANALYSIS_H_
#define _PARAMETER_ANALYSIS_H_

#include "..\SearchKey\DefineMarco.h"

class KParameterAnalysis
{
public:
    KParameterAnalysis();
    ~KParameterAnalysis();

    bool GetOption(int argc, char* argv[]);
};
#endif

