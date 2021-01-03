#include "KProcessState.h"

#include <windows.h>
#include <psapi.h>
#include <assert.h>

#pragma   comment(lib,"psapi.lib")

static unsigned long long FileTimeToUTC(FILETIME* ftime)
{
    LARGE_INTEGER li;

    assert(ftime);
    li.LowPart = ftime->dwLowDateTime;
    li.HighPart = ftime->dwHighDateTime;
    return li.QuadPart;
}

KProcessState::KProcessState()
{

}

KProcessState::~KProcessState()
{

}

int KProcessState::GetCpuUsage()
{
    int       nCpu             = -1;
    int       nProcessorCount  = -1;
    long long nTime            = 0;
    long long nLastTime        = 0;
    long long nTimeDelta       = 0;
    long long nSystemTime      = 0;
    long long nLastSystemTime  = 0;
    long long nSystemTimeDelta = 0;
    FILETIME  now;
    FILETIME  creationTime;
    FILETIME  exitTime;
    FILETIME  kernelTime;
    FILETIME  userTime;

    if (nProcessorCount == -1)
    {
        nProcessorCount = GetProcessorNumber();
    }

    GetSystemTimeAsFileTime(&now);

    if (!GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime))
    {
        return -1;
    }

    nSystemTime = (FileTimeToUTC(&kernelTime) + FileTimeToUTC(&userTime)) / nProcessorCount;
    nTime = FileTimeToUTC(&now);

    if ((nLastSystemTime == 0) || (nLastTime == 0))
    {
        nLastSystemTime = nSystemTime;
        nLastTime = nTime;
        return -1;
    }

    nTimeDelta = nTime - nLastTime;
    nSystemTimeDelta = nSystemTime - nLastSystemTime;

    assert(nTimeDelta != 0);

    if (nTimeDelta == 0)
    {
        return -1;
    }

    nCpu = (int)((nSystemTimeDelta * 100 + nTimeDelta / 2) / nTimeDelta);
    nLastTime = nTime;
    nLastSystemTime = nSystemTime;

    return nCpu;
}

bool KProcessState::GetMemoryUsage(
    unsigned long long* nWorkingSetSize,
    unsigned long long* nPeakWorkingSetSize,
    unsigned long long* nPagefileUsage
)
{
    bool bResult = false;
    PROCESS_MEMORY_COUNTERS PMC;

    KGLOG_PROCESS_ERROR(nWorkingSetSize);
    KGLOG_PROCESS_ERROR(nPagefileUsage);

    if (GetProcessMemoryInfo(GetCurrentProcess(), &PMC, sizeof(PMC)))
    {
        if (nWorkingSetSize)
        {
            *nWorkingSetSize = PMC.WorkingSetSize;
        }

        if (nPeakWorkingSetSize)
        {
            *nPeakWorkingSetSize = PMC.PeakWorkingSetSize;
        }

        if (nPagefileUsage)
        {
            *nPagefileUsage = PMC.PagefileUsage;
        }

        bResult = true;
    }

Exit0:
    return bResult;
}

int KProcessState::GetProcessorNumber()
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return (int)info.dwNumberOfProcessors;
}