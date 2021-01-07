#ifndef _GET_OPT_
#define _GET_OPT_

#include "DefineMarco.h"

#ifndef _NEED_GETOPT_
struct option
{
    const char* pszName;
    int         nVal;
};
#endif

class KGetOpt
{
public:
    KGetOpt();
    ~KGetOpt();

    int GetOptLong(
        int argc, char* argv[], const struct option* pLongOptions
    );
};
#endif

