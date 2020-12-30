#ifndef _KLARGESEARCH_H
#define _KLARGESEARCH_H

#include <time.h>
#include "KBm.h"
#include "KSunday.h"

class KSearch
{
public:
    KSearch();
    ~KSearch();

    bool Init(char* pszPattern);
    void UnInit();

    bool SearchBm(char* pszFilePath);
    bool SearchBmTwo(char* pszFilepath);

private:
    char* m_pszPattern;
    int   m_nPatternLen;

    char* m_pszFilePath;

    char* m_pszFirstReadBuffer;

    char* m_pszSecondReadBuffer;
};

#endif // !_KLARGESEARCH_H
