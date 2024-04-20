#include "StringCommon.h"

#include <regex>

/************************************************************************/
/*                                                                      */
/************************************************************************/
void CStringCommon::Split(const std::string& strData, std::vector<std::string>& vecBlocks, const std::string& strSplit)
{
	try
	{
		std::regex re{ strSplit };
		vecBlocks = std::vector<std::string> {
			std::sregex_token_iterator(strData.begin(), strData.end(), re, -1),
				std::sregex_token_iterator()
		};
	}	
	catch (...)
	{

	}
	

	/*std::string strTmp = strData;
	int pos;
	std::string subStr;
	int iSepLen = strSplit.length();
	if (strTmp.length() == 0)
	{
		return;
	}
	if (iSepLen == 0)
	{
		return;
	}

	pos = strTmp.find(strSplit);
	while (pos >= 0)
	{
		if (pos == 0)
		{
			subStr = "";
			vecBlocks.push_back(subStr);
		}
		else
		{
			subStr = strTmp.substr(0, pos);
			vecBlocks.push_back(subStr);
		}
		strTmp = strTmp.substr(pos + iSepLen, strTmp.length() - pos + iSepLen - 1);
		pos = strTmp.find(strSplit);
	}
	if (strTmp.length() > 0)
	{
		vecBlocks.push_back(strTmp);
	}*/
}

std::vector<std::string> CStringCommon::Split(const std::string& strData, const std::string& strSplit)
{
	try
	{
		std::regex re{ strSplit };
		return std::vector<std::string> {
			std::sregex_token_iterator(strData.begin(), strData.end(), re, -1),
				std::sregex_token_iterator()
		};
	}	
	catch (...)
	{
		return std::vector<std::string>();
	}
	
}


// std::wstring°æ±¾
std::vector<std::wstring> CStringCommon::Split(const std::wstring& in, const std::wstring& delim) 
{
	std::wregex re{ delim };
	return std::vector<std::wstring> {
		std::wsregex_token_iterator(in.begin(), in.end(), re, -1),
			std::wsregex_token_iterator()
	};
}

void CStringCommon::Replace(const std::string& strData, std::string& strResult, const std::string& strSrc, const std::string& strDest)
{
	strResult = strData;

	int iPos = -1;
	int iSrcLen = strSrc.length();
	int iTagLen = strDest.length();

	iPos = strResult.find(strSrc, 0);
	while (iPos >= 0)
	{
		strResult.replace(iPos, iSrcLen, strDest);
		iPos = strResult.find(strSrc, iPos + iTagLen);
	}
}

std::string CStringCommon::Replace(const std::string& strData, const std::string& strSrc, const std::string& strDest)
{
	std::string strTmp = strData;

	int iPos = -1;
	int iSrcLen = strSrc.length();
	int iTagLen = strDest.length();

	iPos = strTmp.find(strSrc, 0);
	while (iPos >= 0)
	{
		strTmp.replace(iPos, iSrcLen, strDest);
		iPos = strTmp.find(strSrc, iPos + iTagLen);
	}

	return strTmp;
}

std::string CStringCommon::ToLower(const std::string& strInput)
{
	std::string strTmp = strInput;
	int iLen = strTmp.length();
	for (int i = 0; i < iLen; i++)
	{
		strTmp[i] = tolower(strTmp[i]);
	}
	return strTmp;
}

std::string CStringCommon::ToUpper(const std::string& strInput)
{
	std::string strTmp = strInput;
	int iLen = strTmp.length();
	for (int i = 0; i < iLen; i++)
	{
		strTmp[i] = toupper(strTmp[i]);
	}
	return strTmp;
}

std::string CStringCommon::TrimLeft(const std::string& strInput, char cErase)
{
	std::string strTmp = strInput;
	int iLen = strTmp.length();
	for (int i = 0; i < iLen; ++i)
	{
		if (strTmp[0] == cErase)
		{
			strTmp.erase(0, 1);
		}
		else
		{
			break;
		}
	}

	return strTmp;
}

std::string CStringCommon::TrimRight(const std::string& strInput, char cErase)
{
	std::string strTmp = strInput;
	int iLen = strTmp.length();
	for (int i = iLen - 1; i >= 0; i--)
	{
		if (strTmp[i] == cErase)
		{
			strTmp.erase(i, 1);
		}
		else
		{
			break;
		}
	}
	return strTmp;
}

void CStringCommon::SafeStringCopy(char* pszDest, const char* pszSrc, int iDestBuffSize)
{

	//strncpy(pszDest, pszSrc, iDestBuffSize - 1);
	//pszDest[iDestBuffSize - 1] = 0;
}

