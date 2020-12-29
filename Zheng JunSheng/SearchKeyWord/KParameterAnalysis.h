#ifndef _PARAMETER_ANALYSIS_H_
#define _PARAMETER_ANALYSIS_H_

#include "DefineMarco.h"

class KParameterAnalysis
{
public:
    KParameterAnalysis();
    ~KParameterAnalysis();

    bool CmdOptionExists(int argc, char* argv[]);
};
#endif

