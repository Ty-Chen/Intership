#ifndef _KHELP_H
#define _KHELP_H

#include "KSearch.h"

class KHelp
{
public:
    KHelp();
    ~KHelp();

    bool Init();
    void UnInit();

    bool TxtSearchKeyOption(int argc, char *argv[]);

private:
    bool PrintHelp(int nPrintType);

private:
    char* m_pszHelpText;
};

#endif // !_KHELP_H
