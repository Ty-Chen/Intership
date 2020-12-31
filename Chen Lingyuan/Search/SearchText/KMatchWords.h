#pragma once
#ifndef KMATCHWORDS_H_
#define KMATCHWORDS_H_

#include<stdio.h>

#include<string>

#include"KKmp.h"
#include"KSunday.h"
#include"KMacro.h"

class KMatchWords
{
public :
    KMatchWords();
    ~KMatchWords();

    bool Init();
    void UnInit();

    bool MatchWords(unsigned char* pszPattern, char* pszResultPath, char* pszTestPath);

private:
    bool Output(int* pnMatchArray,int nMatchArrayLen,
         unsigned char* pszText, int nTextLen,
         char* pszResultPath
    );

private :
    unsigned char*        m_pszText;
    int                   m_nTextLen;

    int*                  m_pnMatchArray;
    int                   m_nMatchArrayLen;
};
#endif //
