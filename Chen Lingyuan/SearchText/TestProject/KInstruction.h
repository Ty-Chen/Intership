#pragma once
#ifndef KINSTRUCTION_H_
#define KINSTRUCTION_H_

#include"KMatchTest.h"
#include"KMacro.h"

class KInstruction
{
public:
    KInstruction();
    ~KInstruction();

    bool Init(int argc, char* argv[]);

    bool ShowInstruction();

private:
    bool MatchWordsTest();
    bool ParameterDescription();

private:
    int    m_nArgNum;
    char** m_pszArgv;

};
#endif // !KINSTRUCTION_H_
