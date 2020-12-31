
#ifndef KMATCHTEST_H_
#define KMATCHTEST_H_

#include"KSunday.h"
#include"KKmp.h"
#include"KMatchWords.h"
#include"KMacro.h"
#include"KBlockMatch.h"

#include<vector>

class KMatchTest
{
public:
    KMatchTest();
    ~KMatchTest();

   bool Init(char* pszKeyWordsDirectory, char* pszTestDirectory,
        char* pszMatchResultDirectory, char* pszContrastDirectory
   );
   void UnInit();

   bool Match();

private:
    bool LoadDirectory(char* pszDirectory, std::vector<std::string>& szList);

    bool Compare(char* pszResultFilePath, char* pszContrastFilePath, FILE* fpResultStatistic);

    bool GetFullPath(char* pszPattern, char* pszResultFilePath, char* pszContrastFilePath);

private:

    std::string m_szFullResultPath;
    std::string m_szFullContrastPath;

    std::vector<std::string>m_szTestFileList;
    std::vector<std::string>m_szKeyWordsList;
    std::vector<std::string>m_szContrastFileList;
    std::vector<std::string>m_szMatchResultFileList;
};
#endif // !KTEST_H_
