#ifndef _PROCESS_STAT_H_
#define _PROCESS_STAT_H_

#include <stdio.h>

#include "DefineMarco.h"

class KProcessState
{
public:
    KProcessState();
    ~KProcessState();

    int GetCpuUsage();
    bool GetMemoryUsage(
        unsigned long long* nWorkingSetSize,
        unsigned long long* nPeakWorkingSetSize,
        unsigned long long* nPagefileUsage
    );
private:
    int GetProcessorNumber();
};
#endif
