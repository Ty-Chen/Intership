#pragma once
#ifndef KSUNDAY_H_
#define KSUNDAY_H_
#include"KMacro.h"

class KSunday
{
public :
    KSunday();
    ~KSunday();

    bool Init(const unsigned char* pcszPattern, int nPatternLen);

    int  SundayCount(
         const unsigned char* pcszText, int nTextLen,
         const unsigned char* pcszPattern, int nPatternLen,
         int *pnMatchArray, int nMatchArrayLen
    );

private :
    bool GetBadChar(
        const unsigned char* pcszPattern, int nPatternLen,
        int* pnBmbc, int nBmbcLen
    );

private:
    int m_nBadChar[ASCLL];
    int m_nBadCharLen;
};
#endif //
