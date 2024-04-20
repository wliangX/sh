#include "Boost.h"

#include "StringUtilImpl.h"

#define BAD_LEXICAL_CAST 0

boost::mt19937 CStringImpl::m_engine;
boost::atomic<int32_t> CStringImpl::m_siSerial;

CStringImpl::CStringImpl()
{

}
CStringImpl::~CStringImpl()
{

}

std::string CStringImpl::AnsiToUtf8(std::string p_strAnsi)
{
	//return boost::locale::conv::to_utf<char>(p_strAnsi, "GBK");
	try
	{
		std::string l_strCodePage = boost::locale::util::get_system_locale();
		return boost::locale::conv::to_utf<char>(p_strAnsi, boost::locale::generator().generate(l_strCodePage));
	}
	catch (const std::exception& exp)
	{
		return "";
	}
}

std::string CStringImpl::Utf8ToAnsi(std::string p_strUtf8)
{
	//return boost::locale::conv::between(p_strUtf8, "GBK", "utf8");
	try
	{
		std::string l_strCodePage = boost::locale::util::get_system_locale();
		return boost::locale::conv::from_utf(p_strUtf8, boost::locale::generator().generate(l_strCodePage));
	}
	catch (const std::exception& exp)
	{
		return "";
	}
}

std::string CStringImpl::CreateGuid()
{
	boost::uuids::random_generator l_rgen;//随机生成器  
	boost::uuids::uuid l_uuid = l_rgen();//生成一个随机的UUID
	return boost::uuids::to_string(l_uuid);
}

//生成随机数
int32_t CStringImpl::CreateRandom(int32_t p_iMin /* = 0 */, int32_t p_iMax /* = 9 */)
{
	boost::random::uniform_int_distribution<> l_distribution(p_iMin, p_iMax);
	return l_distribution(m_engine);
}

//按固定长度生成流水串号
std::string CStringImpl::CreateSerial(int32_t p_iLen /* = 1 */)
{
	if (Number(m_siSerial).length() > (uint32_t)p_iLen)
	{
		m_siSerial = 0;
	}
	std::string l_strformat = "%" + Format("0%dd", p_iLen);
	return Format(l_strformat.c_str(), m_siSerial++);
}

//格式化
std::string CStringImpl::Format(const char* p_pformat, ...)
{
	va_list l_argList, l_argCopyList;
	va_start(l_argList, p_pformat);
	va_copy(l_argCopyList, l_argList);
	size_t num_of_chars;
#if defined(WIN32) || defined(WIN64)
	num_of_chars = _vscprintf(p_pformat, l_argCopyList);
#else
	num_of_chars = vsnprintf(nullptr, 0, p_pformat, l_argCopyList);
#endif
	va_end(l_argCopyList);

	std::string l_strFormat(num_of_chars, 0);
	vsprintf((char*)l_strFormat.data(), p_pformat, l_argList);
	va_end(l_argList);

	return l_strFormat;
}

bool CStringImpl::Find(std::string p_str, std::string p_strFind)
{
	try
	{
		boost::iterator_range<std::string::iterator> l_range = boost::find_first(p_str, p_strFind);
		return !l_range.empty();
	}
	catch (const std::exception& exp)
	{
		return false;
	}
}

int32_t CStringImpl::FindIndex(std::string p_str, std::string p_strFind, uint32_t p_iIndex)
{
	int32_t l_iPos = -1;
	try
	{
		boost::iterator_range<std::string::iterator> l_range = boost::find_nth(p_str, p_strFind, p_iIndex);
		if (!l_range.empty())
		{
			l_iPos = l_range.begin() - p_str.begin();
		}
		return l_iPos;
	}
	catch (const std::exception& exp)
	{
		return l_iPos;
	}
}

int32_t CStringImpl::FindFirst(std::string p_str, std::string p_strFind)
{
	int32_t l_iPos = -1;
	try
	{
		boost::iterator_range<std::string::iterator> l_range = boost::find_first(p_str, p_strFind);
		int32_t l_iPos = -1;
		if (!l_range.empty())
		{
			l_iPos = l_range.begin() - p_str.begin();
		}
		return l_iPos;
	}
	catch (const std::exception& exp)
	{
		return l_iPos;
	}
}

int32_t CStringImpl::FindLast(std::string p_str, std::string p_strFind)
{
	int32_t l_iPos = -1;
	try
	{
		boost::iterator_range<std::string::iterator> l_range = boost::find_last(p_str, p_strFind);
		int32_t l_iPos = -1;
		if (!l_range.empty())
		{
			l_iPos = l_range.begin() - p_str.begin();
		}
		return l_iPos;
	}
	catch (const std::exception& exp)
	{
		return l_iPos;
	}
}

int32_t CStringImpl::Contains(std::string p_str, std::string p_strFind)
{
	int32_t count = -1;
	try
	{
		std::string l_str = p_str;
		std::string l_strFind = p_strFind;
		boost::iterator_range<std::string::iterator> range;
		
		do
		{
			++count;
			if (l_str == l_strFind && l_str.length() == l_strFind.length())
			{
				l_str = "";
				break;
			}

			range = boost::find_first(l_str, l_strFind);
			l_str = std::string(range.end(), l_str.end());
		} while (!range.empty());

		return count;
	}
	catch (const std::exception& exp)
	{
		return count;
	}
}

bool CStringImpl::RegexMatch(std::string p_str, std::string p_strRegex)
{
	try
	{
		boost::regex l_expr(p_strRegex);		
		return boost::regex_match(p_str, l_expr);
	}
	catch (const std::exception& exp)
	{
		return false;
	}
}

bool CStringImpl::RegexMatch(std::string p_str, std::string p_strRegex, std::vector<std::string>& p_vecWhat)
{
	try
	{
		boost::xpressive::sregex l_xReg = boost::xpressive::sregex::compile(p_strRegex);
		boost::xpressive::smatch l_what;
		if (boost::xpressive::regex_match(p_str, l_what, l_xReg))
		{
			for (int32_t i = 0; i < l_what.length(); ++i)
			{
				if (!l_what[i].str().empty())
				{
					p_vecWhat.push_back(l_what[i]);
				}				
			}
		}
	}
	catch (const std::exception& exp)
	{
		return false;
	}
	return true;
}

int32_t CStringImpl::Split(std::string p_str, std::string p_strSplit, std::vector<std::string>& p_SplitVec, bool p_bMultiSymbols)
{
	try
	{
		if (p_bMultiSymbols)
		{
			boost::split(p_SplitVec, p_str, boost::is_any_of(p_strSplit));
		}
		else
		{
			boost::split_regex(p_SplitVec, p_str, boost::regex(p_strSplit));
		}
		return p_SplitVec.size();
	}
	catch (const std::exception& exp)
	{
		return 0;
	}	
}

std::string CStringImpl::Left(std::string p_str, int32_t p_iLen)
{
	return p_str.substr(0, p_iLen);
}

std::string CStringImpl::Right(std::string p_str, int32_t p_iLen)
{
	if (p_iLen > p_str.length())
	{
		return "";
	}
	return p_str.substr(p_str.length() - p_iLen, p_iLen);
}

std::string CStringImpl::Mid(std::string p_str, uint32_t p_iIndex, int32_t p_iLen)
{
	return p_str.substr(p_iIndex, p_iLen);
}

bool CStringImpl::StartsWith(std::string p_str, std::string p_strTag)
{
	try
	{
		return boost::algorithm::starts_with(p_str, p_strTag);
	}
	catch (const std::exception& exp)
	{
		return false;
	}
}

bool CStringImpl::EndsWith(std::string p_str, std::string p_strTag)
{	
	try
	{
		return boost::algorithm::ends_with(p_str, p_strTag);
	}
	catch (const std::exception& exp)
	{
		return false;
	}
}

std::string CStringImpl::Lower(std::string p_str)
{	
	try
	{
		boost::algorithm::to_lower(p_str);
		return p_str;
	}
	catch (const std::exception& exp)
	{
		return p_str;
	}	
}

std::string CStringImpl::Upper(std::string p_str)
{
	try
	{
		boost::algorithm::to_upper(p_str);
		return p_str;
	}
	catch (const std::exception& exp)
	{
		return p_str;
	}
}


std::string CStringImpl::ReplaceAll(std::string p_str, std::string p_strSearch, std::string p_strReplace)
{
	try
	{
		boost::replace_all(p_str, p_strSearch, p_strReplace);
		return p_str;
	}
	catch (const std::exception& exp)
	{
		return p_str;
	}
}

std::string CStringImpl::ReplaceFirst(std::string p_str, std::string p_strSearch, std::string p_strReplace)
{
	try
	{
		boost::replace_first(p_str, p_strSearch, p_strReplace);
		return p_str;
	}
	catch (const std::exception& exp)
	{
		return p_str;
	}
}

std::string CStringImpl::ReplaceLast(std::string p_str, std::string p_strSearch, std::string p_strReplace)
{
	try
	{
		boost::replace_last(p_str, p_strSearch, p_strReplace);
		return p_str;
	}
	catch (const std::exception& exp)
	{
		return p_str;
	}
}

std::string CStringImpl::Replace(std::string p_str, std::string p_strReplace, uint32_t p_iForwardIndex, uint32_t p_iReverseIndex)
{
	try
	{
		boost::replace_range(p_str, boost::make_iterator_range(p_str.begin() + p_iForwardIndex, p_str.end() - p_iForwardIndex), p_strReplace);
		return p_str;
	}
	catch (const std::exception& exp)
	{
		return p_str;
	}
}

boost::int16_t CStringImpl::ToShort(std::string p_str)
{
	boost::int16_t l_idigit = BAD_LEXICAL_CAST;
	if (p_str.empty())
	{
		return l_idigit;
	}	
	try
	{
		l_idigit = boost::lexical_cast<boost::int16_t>(p_str);
	}
	catch (boost::bad_lexical_cast& e)
	{
		return BAD_LEXICAL_CAST;
	}
	catch (...)
	{
		return BAD_LEXICAL_CAST;
	}
	return l_idigit;
}

boost::uint16_t CStringImpl::ToUShort(std::string p_str)
{
	boost::uint16_t l_idigit = BAD_LEXICAL_CAST;
	if (p_str.empty())
	{
		return l_idigit;
	}
	try
	{
		l_idigit = boost::lexical_cast<boost::uint16_t>(p_str);
	}
	catch (boost::bad_lexical_cast& e)
	{
		return BAD_LEXICAL_CAST;
	}
	catch (...)
	{
		return BAD_LEXICAL_CAST;
	}
	return l_idigit;
}

boost::int32_t CStringImpl::ToInt(std::string p_str)
{
	boost::int32_t l_idigit = BAD_LEXICAL_CAST;
	if (p_str.empty())
	{
		return l_idigit;
	}
	try
	{
		l_idigit = boost::lexical_cast<boost::int32_t>(p_str);
	}
	catch (boost::bad_lexical_cast& e)
	{
		return BAD_LEXICAL_CAST;
	}
	catch (...)
	{
		return BAD_LEXICAL_CAST;
	}
	return l_idigit;
}
boost::uint32_t CStringImpl::ToUInt(std::string p_str)
{
	boost::uint32_t l_idigit = BAD_LEXICAL_CAST;
	if (p_str.empty())
	{
		return l_idigit;
	}
	try
	{
		l_idigit = boost::lexical_cast<boost::uint32_t>(p_str);
	}
	catch (boost::bad_lexical_cast& e)
	{
		return BAD_LEXICAL_CAST;
	}
	catch (...)
	{
		return BAD_LEXICAL_CAST;
	}
	return l_idigit;
}
boost::int64_t CStringImpl::ToInt64(std::string p_str)
{
	boost::int64_t l_idigit = BAD_LEXICAL_CAST;
	if (p_str.empty())
	{
		return l_idigit;
	}
	try
	{
		l_idigit = boost::lexical_cast<boost::int64_t>(p_str);
	}
	catch (boost::bad_lexical_cast& e)
	{
		return BAD_LEXICAL_CAST;
	}
	catch (...)
	{
		return BAD_LEXICAL_CAST;
	}
	return l_idigit;
}

boost::uint64_t CStringImpl::ToUInt64(std::string p_str)
{
	boost::uint64_t l_idigit = BAD_LEXICAL_CAST;
	if (p_str.empty())
	{
		return l_idigit;
	}
	try
	{
		l_idigit = boost::lexical_cast<uint64_t>(p_str);
	}
	catch (...)
	{
		return BAD_LEXICAL_CAST;
	}
	return l_idigit;
}

float CStringImpl::ToFloat(std::string p_str)
{
	float l_fdigit = 0.0;
	if (p_str.empty())
	{
		return l_fdigit;
	}
	try
	{
		l_fdigit = boost::lexical_cast<float>(p_str);
	}
	catch (...)
	{
		return 0.0;
	}
	return l_fdigit;
}

double CStringImpl::ToDouble(std::string p_str)
{
	double l_fdigit = 0.0;
	if (p_str.empty())
	{
		return l_fdigit;
	}
	try
	{
		l_fdigit = boost::lexical_cast<double>(p_str);
	}
	catch (...)
	{
		return 0.0;
	}
	return l_fdigit;
}

std::string CStringImpl::Number(boost::int64_t n)
{
	std::string l_str;
	try
	{
		l_str = boost::lexical_cast<std::string>(n);
	}
	catch (...)
	{
		return l_str;
	}
	return l_str;
}

std::string CStringImpl::Number(boost::uint64_t n)
{
	std::string l_str;
	try
	{
		l_str = boost::lexical_cast<std::string>(n);
	}
	catch (...)
	{
		return l_str;
	}
	return l_str;
}

std::string CStringImpl::Number(boost::int32_t n)
{
	std::string l_str;
	try
	{
		l_str = boost::lexical_cast<std::string>(n);
	}
	catch (...)
	{
		return l_str;
	}
	return l_str;
}

std::string CStringImpl::Number(boost::uint32_t n)
{
	std::string l_str;
	try
	{
		l_str = boost::lexical_cast<std::string>(n);
	}
	catch (...)
	{
		return l_str;
	}
	return l_str;
}

std::string CStringImpl::Number(double n)
{
	std::string l_str;
	try
	{
		l_str = boost::lexical_cast<std::string>(n);
	}
	catch (...)
	{
		return l_str;
	}
	return l_str;
}
std::string CStringImpl::Number(boost::int16_t n)
{
	std::string l_str;
	try
	{
		l_str = boost::lexical_cast<std::string>(n);
	}
	catch (...)
	{
		return l_str;
	}
	return l_str;
}

std::string CStringImpl::Number(boost::uint16_t n)
{
	std::string l_str;
	try
	{
		l_str = boost::lexical_cast<std::string>(n);
	}
	catch (...)
	{
		return l_str;
	}
	return l_str;
}

std::string CStringImpl::Number(bool n)
{
	std::string l_str;
	try
	{
		l_str = boost::lexical_cast<std::string>(n);
	}
	catch (...)
	{
		return l_str;
	}
	return l_str;
}

std::string CStringImpl::Number(float n)
{
	std::string l_str;
	try
	{
		l_str = boost::lexical_cast<std::string>(n);
	}
	catch (boost::bad_lexical_cast& e)
	{
		fprintf(stderr, "%s\n", e.what());
		return l_str;
	}
	catch (...)
	{
		return l_str;
	}
	return l_str;
}

std::string CStringImpl::Trim(std::string p_str)
{
	try
	{
		boost::trim(p_str);
		return p_str;
	}
	catch (boost::bad_lexical_cast& e)
	{
		return p_str;
	}
	catch (...)
	{
		return p_str;
	}
}

std::string CStringImpl::TrimLeft(std::string p_str)
{
	try
	{
		boost::trim_left(p_str);
		return p_str;
	}
	catch (boost::bad_lexical_cast& e)
	{
		return p_str;
	}
	catch (...)
	{
		return p_str;
	}
}

std::string CStringImpl::TrimRight(std::string p_str)
{
	try
	{
		boost::trim_right(p_str);
		return p_str;
	}
	catch (boost::bad_lexical_cast& e)
	{
		return p_str;
	}
	catch (...)
	{
		return p_str;
	}
}

std::string CStringImpl::Join(std::vector<std::string>& p_vecStr, std::string p_strSplit)
{	
	try
	{
		return boost::join(p_vecStr, p_strSplit);
	}
	catch (boost::bad_lexical_cast& e)
	{
		return "";
	}
	catch (...)
	{
		return "";
	}
}

std::string CStringImpl::Join(std::list<std::string>& p_lstStr, std::string p_strSplit)
{	
	try
	{
		return boost::join(p_lstStr, p_strSplit);
	}
	catch (boost::bad_lexical_cast& e)
	{
		return "";
	}
	catch (...)
	{
		return "";
	}
}
