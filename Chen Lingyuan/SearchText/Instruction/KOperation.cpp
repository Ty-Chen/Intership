#include "KOperation.h"

#include<sstream>
#include<vector>

KOperation::KOperation()
{

}

KOperation::~KOperation()
{

}

bool KOperation::CommandParse(std::string szCommand)
{
    bool                     bResult      = false;
    int                      nCommandLen  = 0;
    std::string              szTempResult = "";
    std::vector<std::string> szRes;

    std::string szResult = "";
    std::stringstream input(szCommand);

    while (input>>szResult)
    {

    }


    bResult = true;
Exit0:
    return bResult;
}
