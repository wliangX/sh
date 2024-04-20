#pragma once
//STL
#include <string>
#include <vector>

#include <boost/cstdint.hpp>

//Project
#include <IResource.h>
#include <TypeDef.h>


namespace ICC
{
	using boost::int16_t;
	using boost::uint16_t;
	using boost::int32_t;
	using boost::uint32_t;
	using boost::int64_t;
	using boost::uint64_t;

	namespace StringUtil
	{
		/*
		* class   字符串接口类
		* author  w16314
		* purpose
		* note
		*/
		class IStringUtil : public IResource
		{
		public:

			//************************************
			// Method:    AnsiToUtf8
			// FullName:  ACS::StringUtil::IStringUtil::AnsiToUtf8
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: Ansi编码转换为Utf8编码
			// Parameter: std::string p_strAnsi
			//************************************
			virtual std::string AnsiToUtf8(std::string p_strAnsi) = 0;
			//************************************
			// Method:    Utf8ToAnsi
			// FullName:  ACS::StringUtil::IStringUtil::Utf8ToAnsi
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: Utf8编码转换为Ansi编码
			// Parameter: std::string p_strUtf8
			//************************************
			virtual std::string Utf8ToAnsi(std::string p_strUtf8) = 0;

			//************************************
			// Method:    CreateGuid
			// FullName:  ACS::StringUtil::IStringUtil::CreateGuid
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: 生成GUID
			//************************************
			virtual std::string CreateGuid() = 0;

			//************************************
			// Method:    CreateRandom
			// FullName:  ACS::StringUtil::IStringUtil::CreateRandom
			// Access:    virtual public 
			// Returns:   int32_t
			// Qualifier: 按p_iMin到p_iMax生成随机数
			// Parameter: int32_t p_iMin
			// Parameter: int32_t p_iMax
			//************************************
			virtual int32_t CreateRandom(int32_t p_iMin = 0, int32_t p_iMax = 9) = 0;

			//************************************
			// Method:    CreateSerial
			// FullName:  ACS::StringUtil::IStringUtil::CreateSerial
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: 按固定长度生成流水串号
			// Parameter: int32_t p_iLen
			//************************************
			virtual std::string CreateSerial(int32_t p_iLen = 1) = 0;

			//************************************
			// Method:    Format
			// FullName:  ACS::StringUtil::IStringUtil::Format
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: 字符串格式化
			// Parameter: const char * p_pformat
			// Parameter: ...
			//************************************
			virtual std::string Format(const char* p_pformat, ...) = 0;

			//************************************
			// Method:    Find
			// FullName:  ACS::StringUtil::IStringUtil::Find
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier: 在字符串查找p_strFind是否存在，存在返回true，不存在返回 false。
			// Parameter: std::string p_str
			// Parameter: std::string p_strFind
			//************************************
			virtual bool Find(std::string p_str, std::string p_strFind) = 0;

			//************************************
			// Method:    Find
			// FullName:  ACS::StringUtil::IStringUtil::FindIndex
			// Access:    virtual public 
			// Returns:   ACS::int32_t
			// Qualifier: 从p_iIndex开始，找到字符串p_strFind第n次出现的位置，n从0开始，存在返回位置，不存在返回-1
			// Parameter: std::string p_str
			// Parameter: std::string p_str
			// Parameter: std::string p_strFind
			// Parameter: uint32_t p_iIndex
			//************************************
			virtual int32_t FindIndex(std::string p_str, std::string p_strFind, uint32_t p_iIndex) = 0;

			//************************************
			// Method:    FindFirst
			// FullName:  ACS::StringUtil::IStringUtil::FindFirst
			// Access:    virtual public 
			// Returns:   int32_t
			// Qualifier: 找到字符串p_strFind第一次出现的位置, 返回p_strFind第一次出现的位置，如果p_strFind没有被找到，返回 -1
			// Parameter: std::string p_str
			// Parameter: std::string p_strFind
			//************************************
			virtual int32_t FindFirst(std::string p_str, std::string p_strFind) = 0;

			//************************************
			// Method:    FindLast
			// FullName:  ACS::StringUtil::IStringUtil::FindLast
			// Access:    virtual public 
			// Returns:   int32_t
			// Qualifier: 找到字符串p_strFind最后一次出现的位置, 返回p_strFind最后一次出现的位置，如果p_strFind没有被找到，返回 -1
			// Parameter: std::string p_str 
			// Parameter: std::string p_strFind
			//************************************
			virtual int32_t FindLast(std::string p_str, std::string p_strFind) = 0;

			//************************************
			// Method:    Contains
			// FullName:  ACS::StringUtil::IStringUtil::Contains
			// Access:    virtual public 
			// Returns:   int32_t
			// Qualifier: 返回在这个字符串中p_strFind出现的次数
			// Parameter: std::string p_str
			// Parameter: std::string p_strFind
			//************************************
			virtual int32_t Contains(std::string p_str, std::string p_strFind) = 0;

			//************************************
			// Method:    RegexMatch
			// FullName:  ACS::StringUtil::IStringUtil::RegexMatch
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier: 正则表达式搜索
			// Parameter: std::string p_str
			// Parameter: std::string p_strRegex
			//************************************
			virtual bool RegexMatch(std::string p_str, std::string p_strRegex) = 0;

			//************************************
			// Method:    RegexMatch
			// FullName:  ACS::StringUtil::IStringUtil::RegexMatch
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier: 正则表达式搜索
			// Parameter: std::string p_str
			// Parameter: std::string p_strRegex
			// Parameter: std::vector<std::string> & p_vecWhat
			//************************************
			virtual bool RegexMatch(std::string p_str, std::string p_strRegex, std::vector<std::string>& p_vecWhat) = 0;

			//************************************
			// Method:    Split
			// FullName:  ACS::StringUtil::IStringUtil::Split
			// Access:    virtual public 
			// Returns:   int32_t
			// Qualifier: 字符串分割 p_bMultiSymbols 分割规则，支持同时多个符号匹配，
			// p_bMultiSymbols = true , 支持 传递",./;!" 匹配任意符号进行分割， 
			// p_bMultiSymbols = false , p_strSplit 作为一个整体进行分割
			// Parameter: std::string p_str
			// Parameter: std::string p_strSplit
			// Parameter: std::vector<std::string> & p_SplitVec
			// Parameter: bool p_bMultiSymbols
			//************************************
			virtual int32_t Split(std::string p_str, std::string p_strSplit, std::vector<std::string>& p_SplitVec, bool p_bMultiSymbols) = 0;

			//************************************
			// Method:    Left
			// FullName:  ACS::StringUtil::IStringUtil::Left
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: 返回包含字符串最左面的len个字符的子字符串, 如果len超过字符串的长度，则整个字符串被返回
			// Parameter: std::string p_str
			// Parameter: int32_t p_iLen
			//************************************
			virtual std::string Left(std::string p_str, int32_t p_iLen) = 0;

			//************************************
			// Method:    Right
			// FullName:  ACS::StringUtil::IStringUtil::Right
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: 返回包含字符串最右面的len个字符的子字符串, 如果len超过字符串的长度，则整个字符串被返回
			// Parameter: std::string p_str
			// Parameter: int32_t p_iLen
			//************************************
			virtual std::string Right(std::string p_str, int32_t p_iLen) = 0;


			//************************************
			// Method:    Mid
			// FullName:  ACS::StringUtil::IStringUtil::Mid
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: 返回包含这个字符串从p_iIndex位置开始p_iLen个字符的字符串。
			// 如果这个字符串为空或者index超出范围，返回空字符串。如果p_iIndex + p_iLen超过这个字符串的长度，返回整个字符串。
			// Parameter: std::string p_str
			// Parameter: uint32_t p_iIndex
			// Parameter: int32_t p_iLen
			//************************************
			virtual std::string Mid(std::string p_str, uint32_t p_iIndex, int32_t p_iLen) = 0;

			//************************************
			// Method:    StartsWith
			// FullName:  ACS::StringUtil::IStringUtil::StartsWith
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier: 如果字符串以s开始，返回真，否则返回假
			// Parameter: std::string p_str
			// Parameter: std::string p_strTag
			//************************************
			virtual bool StartsWith(std::string p_str, std::string p_strTag) = 0;

			//************************************
			// Method:    EndsWith
			// FullName:  ACS::StringUtil::IStringUtil::EndsWith
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier: 如果字符串以s结束，返回真，否则返回假。
			// Parameter: std::string p_str
			// Parameter: std::string p_strTag
			//************************************
			virtual bool EndsWith(std::string p_str, std::string p_strTag) = 0;

			//************************************
			// Method:    Lower
			// FullName:  ACS::StringUtil::IStringUtil::Lower
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: 返回由这个字符串转换的小写字符串
			// Parameter: std::string p_str
			//************************************
			virtual std::string Lower(std::string p_str) = 0;

			//************************************
			// Method:    Upper
			// FullName:  ACS::StringUtil::IStringUtil::Upper
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: 返回由这个字符串转换的大写字符串
			// Parameter: std::string p_str
			//************************************
			virtual std::string Upper(std::string p_str) = 0;

			//************************************
			// Method:    ReplaceAll
			// FullName:  ACS::StringUtil::IStringUtil::ReplaceAll
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: 从字符串中使用p_strReplace全部替换p_strSearch
			// Parameter: std::string p_str
			// Parameter: std::string p_strSearch
			// Parameter: std::string p_strReplace
			//************************************
			virtual std::string ReplaceAll(std::string p_str, std::string p_strSearch, std::string p_strReplace) = 0;

			//************************************
			// Method:    ReplaceFirst
			// FullName:  ACS::StringUtil::IStringUtil::ReplaceFirst
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: 从字符串中使用p_strReplace替换第一次出现的p_strSearch
			// Parameter: std::string p_str
			// Parameter: std::string p_strSearch
			// Parameter: std::string p_strReplace
			//************************************
			virtual std::string ReplaceFirst(std::string p_str, std::string p_strSearch, std::string p_strReplace) = 0;

			//************************************
			// Method:    ReplaceLast
			// FullName:  ACS::StringUtil::IStringUtil::ReplaceLast
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: 从字符串中使用p_strReplace替换最后一次出现的p_strSearch
			// Parameter: std::string p_str
			// Parameter: std::string p_strSearch
			// Parameter: std::string p_strReplace
			//************************************
			virtual std::string ReplaceLast(std::string p_str, std::string p_strSearch, std::string p_strReplace) = 0;

			//************************************
			// Method:    Replace
			// FullName:  ACS::StringUtil::IStringUtil::Replace
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: 从字符串中的p_iForwardIndex位置开始到p_iReverseIndex，替换为p_strReplace
			//p_iForwardIndex从字符串左侧0开始计数，p_iReverseIndex从字符串右侧0开始计数
			//如果超出字符串的长度,保持原状。
			// Parameter: std::string p_str
			// Parameter: std::string p_strReplace
			// Parameter: uint32_t p_iForwardIndex
			// Parameter: uint32_t p_iReverseIndex
			//************************************
			virtual std::string Replace(std::string p_str, std::string p_strReplace, uint32_t p_iForwardIndex, uint32_t p_iReverseIndex) = 0;

			/************************************************************************/
			/* 字符串转数字，支持不同进制转换                                          */
			/************************************************************************/
			virtual boost::int16_t ToShort(std::string p_str) = 0;
			virtual boost::uint16_t ToUShort(std::string p_str) = 0;
			virtual boost::int32_t ToInt(std::string p_str) = 0;
			virtual boost::uint32_t ToUInt(std::string p_str) = 0;
			virtual boost::int64_t ToInt64(std::string p_str) = 0;
			virtual boost::uint64_t ToUInt64(std::string p_str) = 0;
			virtual float ToFloat(std::string p_str) = 0;
			virtual double ToDouble(std::string p_str) = 0;

			/************************************************************************/
			/* 数字转字符串                                                         */
			/************************************************************************/
			virtual std::string Number(boost::int16_t n) = 0;
			virtual std::string Number(boost::uint16_t n) = 0;
			virtual std::string Number(boost::int32_t n) = 0;
			virtual std::string Number(boost::uint32_t n) = 0;
			virtual std::string Number(boost::int64_t n) = 0;
			virtual std::string Number(boost::uint64_t n) = 0;
			virtual std::string Number(float n) = 0;
			virtual std::string Number(double n) = 0;
			virtual std::string Number(bool n) = 0;

			//去空格
			virtual std::string Trim(std::string p_str) = 0;
			virtual std::string TrimLeft(std::string p_str) = 0;
			virtual std::string TrimRight(std::string p_str) = 0;

			//合并字符串
			//strSplit 连接符
			virtual std::string Join(std::vector<std::string>& p_vecStr, std::string p_strSplit) = 0;
			virtual std::string Join(std::list<std::string>& p_lstStr, std::string p_strSplit) = 0;

		};

		typedef boost::shared_ptr<IStringUtil> IStringUtilPtr;
	}
}
