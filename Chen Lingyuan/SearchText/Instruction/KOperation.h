#pragma once
#ifndef KOPERATION_H_
#define KOPERATION_H_

#include<string>

#include<KBlockMatch.h>
#include"KMacro.h"

class KOperation
{
public:
    KOperation();
    ~KOperation();

    bool CommandParse(std::string szCommand);

private:
    bool OutputMatchResult();

    bool OutputToDoucment();

};
#endif
