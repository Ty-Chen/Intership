#ifndef _KSUNDAY_H
#define _KSUNDAY_H

#include "MacroDefinition.h"

class KSunday
{
public:
    KSunday();
    ~KSunday();

    bool Init(char* pszPattern, int nPatternLen);
    void UnInit();

    bool SearchSunday(char* pszPattern, int nPatternLen, char* pszText, int nTextLen);

private:
    bool GetNext(char* pszPattern, int nPatternLen);
    bool Sunday(char* pszPattern, int nPatternLen, char* pszText, int nTextLen);

private:
    int m_nNext[MAX_CHAR];
    int m_nSumLine;

};
#endif // !_KSUNDAY_H
