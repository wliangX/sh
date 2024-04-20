#ifndef __StringCommon_H__
#define __StringCommon_H__

#include <string>
#include <vector>

class CStringCommon
{
public:
	static void Split(const std::string& strData, std::vector<std::string>& vecBlocks, const std::string& strSplit);
	static std::vector<std::string> Split(const std::string& strData, const std::string& strSplit);
	static std::vector<std::wstring> Split(const std::wstring& wstrData, const std::wstring& wstrSplit);

	static void Replace(const std::string& strData, std::string& strResult, const std::string& strSrc, const std::string& strDest);
	static std::string Replace(const std::string& strData, const std::string& strSrc, const std::string& strDest);

	static std::string ToLower(const std::string& strInput);
	static std::string ToUpper(const std::string& strInput);

	static std::string TrimLeft(const std::string& strInput, char cErase);
	static std::string TrimRight(const std::string& strInput, char cErase);	

	static void SafeStringCopy(char* pszDest, const char* pszSrc, int iDestBuffSize);
};

#endif