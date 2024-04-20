#pragma once

#include <StringUtil/IStringUtil.h>


namespace ICC
{
	
	class CStringImpl :
		public IStringUtil
	{
	public:
		CStringImpl();
		virtual ~CStringImpl();
	public:
		virtual std::string AnsiToUtf8(std::string p_strAnsi);

		virtual std::string Utf8ToAnsi(std::string p_strUtf8);

		virtual std::string CreateGuid();

		virtual int32_t CreateRandom(int32_t p_iMin = 0, int32_t p_iMax = 9);

		virtual std::string CreateSerial(int32_t p_iLen = 1);
		
		virtual std::string Format(const char* p_pformat, ...);

		virtual bool Find(std::string p_str, std::string p_strFind);

		virtual int32_t FindIndex(std::string p_str, std::string p_strFind, uint32_t p_iIndex);

		virtual int32_t FindFirst(std::string p_str, std::string p_strFind);

		virtual int32_t FindLast(std::string p_str, std::string p_strFind);

		virtual int32_t Contains(std::string p_str, std::string p_strFind);

		virtual bool RegexMatch(std::string p_str, std::string p_strRegex);

		virtual bool RegexMatch(std::string p_str, std::string p_strRegex, std::vector<std::string>& p_vecWhat);

		virtual int32_t Split(std::string p_str, std::string p_strSplit, std::vector<std::string>& p_SplitVec, bool p_bMultiSymbols);

		virtual std::string Left(std::string p_str, int32_t p_iLen);

		virtual std::string Right(std::string p_str, int32_t p_iLen);

		virtual std::string Mid(std::string p_str, uint32_t p_iIndex, int32_t p_iLen);

		virtual bool StartsWith(std::string p_str, std::string p_strTag);

		virtual bool EndsWith(std::string p_str, std::string p_strTag);

		virtual std::string Lower(std::string p_str);

		virtual std::string Upper(std::string p_str);

		virtual std::string ReplaceAll(std::string p_str, std::string p_strSearch, std::string p_strReplace);

		virtual std::string ReplaceFirst(std::string p_str, std::string p_strSearch, std::string p_strReplace);

		virtual std::string ReplaceLast(std::string p_str, std::string p_strSearch, std::string p_strReplace);

		virtual std::string Replace(std::string p_str, std::string p_strReplace, uint32_t p_iForwardIndex, uint32_t p_iReverseIndex);

		virtual boost::int16_t ToShort(std::string p_str);
		virtual boost::uint16_t ToUShort(std::string p_str);
		virtual boost::int32_t ToInt(std::string p_str);
		virtual boost::uint32_t ToUInt(std::string p_str);
		virtual boost::int64_t ToInt64(std::string p_str);
		virtual boost::uint64_t ToUInt64(std::string p_str);
		virtual float ToFloat(std::string p_str);
		virtual double ToDouble(std::string p_str);

		virtual std::string Number(boost::int16_t n);
		virtual std::string Number(boost::uint16_t n);
		virtual std::string Number(boost::int32_t n);
		virtual std::string Number(boost::uint32_t n);
		virtual std::string Number(boost::int64_t n);
		virtual std::string Number(boost::uint64_t n);
		virtual std::string Number(float n);
		virtual std::string Number(double n);
		virtual std::string Number(bool n);


		virtual std::string Trim(std::string p_str);
		virtual std::string TrimLeft(std::string p_str);
		virtual std::string TrimRight(std::string p_str);

		virtual std::string Join(std::vector<std::string>& p_vecStr, std::string p_strSplit);
		virtual std::string Join(std::list<std::string>& p_lstStr, std::string p_strSplit);

	private:
		static boost::mt19937 m_engine;//随机数种子
		static boost::atomic<int32_t> m_siSerial;//序列号
	};
}