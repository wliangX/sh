#include "Boost.h"

const CDateTime MICROSECONDS_PER_SECOND = 1000000;
const CDateTime SECONDS_PER_MINUTE = 60;
const CDateTime SECONDS_PER_HOUR = 60 * SECONDS_PER_MINUTE;
const CDateTime SECONDS_PER_DAY = 24 * SECONDS_PER_HOUR;
const std::string DATETIME_SERIAL_STRING_FORMAT = "%04d%02d%02d%02d%02d%02d%06d";

const char szCharacter[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

// 用于utc时间与本地时间的转换（东八区）
//typedef boost::date_time::local_adjustor<boost::posix_time::ptime, 8, boost::posix_time::no_dst> local_adj;

// 1. 相对起始时间"1970/1/1 00:00:00.000"
// 2. 不通过boost::posix_time::time_from_string("1582/10/15 00:00:00.000000")，
// 避免依赖于`boost::gregorian::greg_month::get_month_map_ptr()'，导致在linux下其他
// 程序使用该库时出现链接错误：
// undefined reference to boost::gregorian::greg_month::get_month_map_ptr()
static const boost::posix_time::ptime seedDateTime(boost::gregorian::date(1970, 1, 1),
	boost::posix_time::time_duration(0, 0, 0, 0));

// 日期时间信息
struct DateTimeInfo
{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	//int millisecond;
	int microsecond;
};

/*
* class   日期时间解析类
* author  w16314
* purpose 解析日期时间字符串
* note    格式形如: 2013-01-02 23:04:05.123456
*/
class DateTimeParser
{
public:
	// 从字符串中解析日期时间，格式为：2013-01-02 23:04:05.123456
	static bool ParseDateTime(const std::string& strDateTime, boost::posix_time::ptime& dateTime);
	// 将boost::posix_time::ptime解析成DateTimeInfo
	static void ToDateTimeInfo(const boost::posix_time::ptime& dateTime, DateTimeInfo& dateTimeInfo);
};

/////////////////////////////////////////////////////////////////////////
//功能: 从字符串中解析日期时间，格式为：2013-01-02 23:04:05.123456
bool DateTimeParser::ParseDateTime(const std::string& strDateTime, boost::posix_time::ptime& dateTime)
{
	try
	{
		std::string l_strReg = "(\\d{4})([-/ ]?)(\\d{1,2})([-/ ]?)(\\d{1,2})([ T]?)(\\d{1,2})([: ]?)(\\d{1,2})([: ]?)(\\d{1,2})([. ]?)(\\d{0,6})";
		boost::xpressive::sregex l_xReg = boost::xpressive::sregex::compile(l_strReg);
		boost::xpressive::smatch what;
		if (boost::xpressive::regex_match(strDateTime, what, l_xReg))
		{
			boost::gregorian::date date(
				boost::lexical_cast<unsigned short>(what[1]),
				boost::lexical_cast<unsigned short>(what[3]),
				boost::lexical_cast<unsigned short>(what[5]));

 			boost::posix_time::time_duration time(
 				boost::lexical_cast<boost::posix_time::time_duration::hour_type>(what[7]),
 				boost::lexical_cast<boost::posix_time::time_duration::min_type>(what[9]),
 				boost::lexical_cast<boost::posix_time::time_duration::sec_type>(what[11]),
				boost::lexical_cast<boost::posix_time::time_duration::fractional_seconds_type>(what[13].str().empty() ? "0" : what[13].str()));
 
 			dateTime = boost::posix_time::ptime(date, time);
			return true;
		}
	}
	catch (const std::exception& exp)
	{
		// nothing to do
	}
	return false;
}

// 将boost::posix_time::ptime解析成DateTimeInfo
void DateTimeParser::ToDateTimeInfo(const boost::posix_time::ptime& dateTime, DateTimeInfo& dateTimeInfo)
{
	const boost::posix_time::ptime::date_type::ymd_type date = dateTime.date().year_month_day();
	const boost::posix_time::ptime::time_duration_type time = dateTime.time_of_day();

	// 注意date.year，date.month和date.day是一个对象，而不是一个整数，
	dateTimeInfo.year = (short)date.year;
	dateTimeInfo.month = (short)date.month;
	dateTimeInfo.day = (short)date.day;
	dateTimeInfo.hour = (int)time.hours();
	dateTimeInfo.minute = (int)time.minutes();
	dateTimeInfo.second = (int)time.seconds();
	//dateTimeInfo.millisecond = (int)(time.fractional_seconds() / 1000);
	dateTimeInfo.microsecond = (int)(time.fractional_seconds());
}

boost::mt19937 CDateTimeImpl::m_engine;

CDateTimeImpl::CDateTimeImpl()
{

}

CDateTimeImpl::~CDateTimeImpl()
{

}

// 从字符串解析日期时间
CDateTime CDateTimeImpl::FromString(std::string p_strDateTime)
{
	boost::posix_time::ptime l_destDateTime;
	if (!DateTimeParser::ParseDateTime(p_strDateTime, l_destDateTime))
	{
		return ERROR_DATE_TIME;
	}

	try
	{
		boost::posix_time::time_duration l_offsetTime = l_destDateTime - seedDateTime;
		return l_offsetTime.total_microseconds();
	}
	catch (const std::exception& exp)
	{
		fprintf(stderr, "Wrong date time format: %s, %s\n", p_strDateTime.c_str(), exp.what());
		return ERROR_DATE_TIME;
	}

	return ERROR_DATE_TIME;
}

std::string CDateTimeImpl::_ConvertLocalUTC(const std::string& strDateTime, int iOffset, std::string p_strFormat /* = DATETIME_STRING_FORMAT_NO_MICROSECOND */)
{
	CDateTime curTime = FromString(strDateTime);
	if (curTime == ERROR_DATE_TIME)
	{
		return "";
	}

	CDateTime realTime = AddHours(curTime, iOffset);

	return ToString(realTime, p_strFormat);
}

std::string CDateTimeImpl::FromLocalToUTC(const std::string& strLocalDateTime, int iDefaultOffset /* = -8 */, std::string p_strFormat /* = DATETIME_STRING_FORMAT_NO_MICROSECOND */)
{
	return _ConvertLocalUTC(strLocalDateTime, iDefaultOffset, p_strFormat);
}

std::string CDateTimeImpl::FromUTCToLocal(const std::string& strUTCDateTime, int iDefaultOffset /* = 8 */, std::string p_strFormat /* = DATETIME_STRING_FORMAT_NO_MICROSECOND */)
{
	return _ConvertLocalUTC(strUTCDateTime, iDefaultOffset, p_strFormat);
}


// 将日期时间转化为字符串
std::string CDateTimeImpl::ToString(CDateTime p_DateTime, std::string p_strFormat /* = DATETIME_STRING_FORMAT_NO_MICROSECOND */)
{
	std::string l_strDateTime;
	if (p_strFormat.empty())
	{
		return l_strDateTime;
	}

	boost::posix_time::ptime l_localDateTime;
	try
	{
		l_localDateTime = seedDateTime + ICC::microseconds(p_DateTime);
	}
	catch (...)
	{
		//ICCExceptionDefaultHandle;
		return l_strDateTime;
	}

	DateTimeInfo l_dateTimeInfo;
	DateTimeParser::ToDateTimeInfo(l_localDateTime, l_dateTimeInfo);
	
	try
	{
		boost::format l_fmt(p_strFormat);
		int l_iLen = l_fmt.size();
		switch (l_iLen)
		{
		case 0:
		case 6:
		{
			l_fmt % l_dateTimeInfo.year % l_dateTimeInfo.month % l_dateTimeInfo.day
				% l_dateTimeInfo.hour % l_dateTimeInfo.minute % l_dateTimeInfo.second
				% l_dateTimeInfo.microsecond;
			l_strDateTime = l_fmt.str();
			break;
		}
		case 5:
		{
			l_fmt % l_dateTimeInfo.year % l_dateTimeInfo.month % l_dateTimeInfo.day
				% l_dateTimeInfo.hour % l_dateTimeInfo.minute % l_dateTimeInfo.second;
			l_strDateTime = l_fmt.str();
			break;
		}
		case 2:
		{
			l_fmt % l_dateTimeInfo.year % l_dateTimeInfo.month % l_dateTimeInfo.day;
			l_strDateTime = l_fmt.str();
			break;
		}
		default:		
			l_fmt % l_dateTimeInfo.year % l_dateTimeInfo.month % l_dateTimeInfo.day
				% l_dateTimeInfo.hour % l_dateTimeInfo.minute % l_dateTimeInfo.second
				% l_dateTimeInfo.microsecond;
			l_strDateTime = l_fmt.str();
			break;		
		}
	}
	catch (...)
	{
		//ICCExceptionDefaultHandle;
		return l_strDateTime;
	}

	return l_strDateTime;
}

std::string CDateTimeImpl::ToStringEx(std::time_t p_DateTime, std::string p_strFormat /* = DATETIME_STRING_FORMAT_NO_MICROSECOND */)
{
	std::string l_strDateTime;
	if (p_strFormat.empty())
	{
		return l_strDateTime;
	}

	boost::posix_time::ptime l_localDateTime;
	try
	{
		l_localDateTime = boost::posix_time::from_time_t(p_DateTime);		
	}
	catch (...)
	{
		//ICCExceptionDefaultHandle;
		return l_strDateTime;
	}

	DateTimeInfo l_dateTimeInfo;
	DateTimeParser::ToDateTimeInfo(l_localDateTime, l_dateTimeInfo);

	try
	{
		boost::format l_fmt(p_strFormat);
		int l_iLen = l_fmt.size();
		switch (l_iLen)
		{
		case 0:
		case 6:
		{
			l_fmt% l_dateTimeInfo.year% l_dateTimeInfo.month% l_dateTimeInfo.day
				% l_dateTimeInfo.hour% l_dateTimeInfo.minute% l_dateTimeInfo.second
				% l_dateTimeInfo.microsecond;
			l_strDateTime = l_fmt.str();
			break;
		}
		case 5:
		{
			l_fmt% l_dateTimeInfo.year% l_dateTimeInfo.month% l_dateTimeInfo.day
				% l_dateTimeInfo.hour% l_dateTimeInfo.minute% l_dateTimeInfo.second;
			l_strDateTime = l_fmt.str();
			break;
		}
		case 2:
		{
			l_fmt% l_dateTimeInfo.year% l_dateTimeInfo.month% l_dateTimeInfo.day;
			l_strDateTime = l_fmt.str();
			break;
		}
		default:
			l_fmt% l_dateTimeInfo.year% l_dateTimeInfo.month% l_dateTimeInfo.day
				% l_dateTimeInfo.hour% l_dateTimeInfo.minute% l_dateTimeInfo.second
				% l_dateTimeInfo.microsecond;
			l_strDateTime = l_fmt.str();
			break;
		}
	}
	catch (...)
	{
		//ICCExceptionDefaultHandle;
		return l_strDateTime;
	}

	return l_strDateTime;
}

std::string CDateTimeImpl::CreateSerial()
{
	CDateTime l_DateTime = CurrentDateTime();
	std::string l_strRandom;
	try
	{
		boost::random::uniform_int_distribution<> l_distribution(10, 99);
		int l_iRandom = l_distribution(m_engine);
		l_strRandom = boost::lexical_cast<std::string>(l_iRandom);
	}
	catch (...)
	{
		//ICCExceptionDefaultHandle;
		return "";
	}
	return ToString(l_DateTime, DATETIME_SERIAL_STRING_FORMAT) + l_strRandom;
}

std::string CDateTimeImpl::CreateSerial(const int& iZoneSecond)
{
	CDateTime l_DateTime =  AddSeconds(CurrentDateTime(), iZoneSecond);

	std::string tmp_strData(ToString(l_DateTime, DATETIME_SERIAL_STRING_FORMAT));
	//return ToString(l_DateTime, DATETIME_SERIAL3_STRING_FORMAT);

	return tmp_strData.substr(0, 8); //只取YYYYMMDD
}

std::string CDateTimeImpl::CreateSerial2()
{
	boost::this_thread::sleep(boost::posix_time::millisec(10));
	std::string l_strSerial;
	CDateTime l_DateTime = CurrentDateTime();

	boost::posix_time::ptime l_localDateTime;
	l_localDateTime = seedDateTime + ICC::microseconds(l_DateTime);
	DateTimeInfo l_dateTimeInfo;
	DateTimeParser::ToDateTimeInfo(l_localDateTime, l_dateTimeInfo);
	

	std::string l_strYear(1, szCharacter[l_dateTimeInfo.year % 62]);
	std::string l_strMonth(1, szCharacter[l_dateTimeInfo.month]);
	std::string l_strDay(1, szCharacter[l_dateTimeInfo.day]);
	std::string l_strHour(1, szCharacter[l_dateTimeInfo.hour]);

	std::string l_strMin(1, szCharacter[l_dateTimeInfo.minute]);
	std::string l_strSecond(1, szCharacter[l_dateTimeInfo.second]);

	boost::format l_fmt("%02X");
	l_fmt % (l_DateTime % 0xFF);

	l_strSerial = l_strYear + l_strMonth + l_strDay + l_strHour + l_strMin + l_strSecond + l_fmt.str();	
	return l_strSerial;
}

// 获取当前时间
CDateTime CDateTimeImpl::CurrentDateTime()
{
	try
	{
		//boost::posix_time::ptime localTime = boost::posix_time::microsec_clock::local_time();
		boost::posix_time::ptime localTime = boost::posix_time::microsec_clock::universal_time();
		boost::posix_time::time_duration offsetTime = localTime - seedDateTime;
		return offsetTime.total_microseconds();
	}	
	catch (...)
	{
		return 0;
	}	
}

CDateTime CDateTimeImpl::CurrentLocalDateTime()
{
	try
	{
		boost::posix_time::ptime localTime = boost::posix_time::microsec_clock::local_time();
		//boost::posix_time::ptime localTime = boost::posix_time::microsec_clock::universal_time();
		boost::posix_time::time_duration offsetTime = localTime - seedDateTime;
		return offsetTime.total_microseconds();
	}
	catch (...)
	{
		return 0;
	}
}

CDateTime CDateTimeImpl::FromTime_T(std::time_t p_DateTime)
{
	try
	{		
		boost::posix_time::ptime localTime = boost::posix_time::from_time_t(p_DateTime);
		boost::posix_time::time_duration offsetTime = localTime - seedDateTime;
		return offsetTime.total_microseconds();
	}
	catch (...)
	{
		return 0;
	}
}

std::time_t CDateTimeImpl::ToTime_T(CDateTime p_DateTime)
{
	boost::posix_time::ptime l_localDateTime;
	try
	{
		l_localDateTime = seedDateTime + ICC::microseconds(p_DateTime);
		return boost::posix_time::to_time_t(l_localDateTime);
	}
	catch (...)
	{
		//ICCExceptionDefaultHandle;
		return 0;
	}
}

// 获取当前时间
std::string CDateTimeImpl::CurrentDateTimeStr()
{
	return ToString(CurrentDateTime());
}

std::string CDateTimeImpl::CurrentLocalDateTimeStr()
{
	return ToString(CurrentLocalDateTime());
}

//添加秒数
CDateTime CDateTimeImpl::AddSeconds(CDateTime p_DateTime, int p_iSeconds)
{
	return p_DateTime + p_iSeconds * MICROSECONDS_PER_SECOND;
}

//添加分钟数
CDateTime CDateTimeImpl::AddMinutes(CDateTime p_DateTime, int p_iMinutes)
{
	return AddSeconds(p_DateTime, p_iMinutes * SECONDS_PER_MINUTE);
}

//添加小时数
CDateTime CDateTimeImpl::AddHours(CDateTime p_DateTime, int p_iHours)
{
	return AddSeconds(p_DateTime, p_iHours * SECONDS_PER_HOUR);
}

CDateTime CDateTimeImpl::AddDays(CDateTime p_DateTime, int p_iDays)
{
	return AddSeconds(p_DateTime, p_iDays * SECONDS_PER_DAY);
}

CDateTime CDateTimeImpl::AddMonths(CDateTime p_DateTime, int p_iMonths)
{
	boost::posix_time::ptime l_destDateTime;
	if (!DateTimeParser::ParseDateTime(ToString(p_DateTime), l_destDateTime))
	{
		return ERROR_DATE_TIME;
	}
	l_destDateTime += boost::gregorian::months(p_iMonths);	
	try
	{
		boost::posix_time::time_duration offsetTime = l_destDateTime - seedDateTime;
		return offsetTime.total_microseconds();
	}
	catch (const std::exception& exp)
	{
		fprintf(stderr, "Wrong date time, %s\n", exp.what());
		return ERROR_DATE_TIME;
	}
}

CDateTime CDateTimeImpl::AddYears(CDateTime p_DateTime, int p_iYears)
{
	boost::posix_time::ptime l_destDateTime;
	if (!DateTimeParser::ParseDateTime(ToString(p_DateTime), l_destDateTime))
	{
		return ERROR_DATE_TIME;
	}
	l_destDateTime += boost::gregorian::years(p_iYears);
	try
	{
		boost::posix_time::time_duration offsetTime = l_destDateTime - seedDateTime;
		return offsetTime.total_microseconds();
	}
	catch (const std::exception& exp)
	{
		fprintf(stderr, "Wrong date time, %s\n", exp.what());
		return ERROR_DATE_TIME;
	}
}

unsigned int CDateTimeImpl::SecondsDifference(CDateTime p_st, CDateTime p_et)
{
	uint64 l_tmp = (p_st >= p_et) ? (p_st - p_et) : (p_et - p_st);
	return l_tmp / MICROSECONDS_PER_SECOND;
//	uint64 l_tmp = 0;
//#if defined(_WIN32) || defined(_WIN64)
//	l_tmp = _abs64(p_et - p_st);
//#else
//	l_tmp = fabs(p_et - p_st);
//#endif
//	return l_tmp / MICROSECONDS_PER_SECOND;
}

unsigned int CDateTimeImpl::DaysDifference(CDateTime p_st, CDateTime p_et)
{
	return SecondsDifference(p_st, p_et) / SECONDS_PER_DAY;
}

std::string CDateTimeImpl::GetFrontTime(std::string strTargetTime, long long l_secs)
{
	CDateTime l_strCurTime = FromString(strTargetTime);
	CDateTime l_strBeginTime;

	l_strBeginTime = AddSeconds(l_strCurTime, -1 * l_secs);

	return ToString(l_strBeginTime);
}

std::string CDateTimeImpl::GetAfterTime(std::string strTargetTime, long long l_secs)
{
	CDateTime l_strCurTime = FromString(strTargetTime);
	CDateTime l_strEndTime;

	l_strEndTime = AddSeconds(l_strCurTime, l_secs);

	return ToString(l_strEndTime);
}

std::string CDateTimeImpl::GetAlarmIdTime(std::string strAlarmId)
{
	if (strAlarmId.empty() || strAlarmId.size() < 15)
	{
		return "";
	}

	std::string strYear;
	std::string strMonth;
	std::string strDay;
	strYear = strAlarmId.substr(6, 4);
	strMonth = strAlarmId.substr(10, 2);
	strDay = strAlarmId.substr(12, 2);

	char targetString[20] = {0};
	sprintf(targetString, "%s-%s-%s 12:00:00", strYear.c_str(), strMonth.c_str(), strDay.c_str());

	return targetString;
}

std::string CDateTimeImpl::GetCallRefIdTime(std::string strCallRefId)
{
	if (strCallRefId.size() != 20)
	{
		return "";
	}

	std::string strYear;
	std::string strMonth;
	std::string strDay;
	std::string strHour;
	std::string strMin;
	std::string strSec;
	strYear = strCallRefId.substr(0, 4);
	strMonth = strCallRefId.substr(4, 2);
	strDay = strCallRefId.substr(6, 2);
	strHour = strCallRefId.substr(8, 2);
	strMin = strCallRefId.substr(10, 2);
	strSec = strCallRefId.substr(12, 2);

	char targetString[20] = { 0 };
	sprintf(targetString, "%s-%s-%s %s:%s:%s", strYear.c_str(), strMonth.c_str(), strDay.c_str(), strHour.c_str(), strMin.c_str(), strSec.c_str());

	return targetString;
}

std::string CDateTimeImpl::GetDispatchIdTime(std::string strDispatchId)
{
	if (strDispatchId.empty())
	{
		return "";
	}

	std::string strYear;
	std::string strMonth;
	std::string strDay;
	strYear = strDispatchId.substr(6, 4);
	strMonth = strDispatchId.substr(10, 2);
	strDay = strDispatchId.substr(12, 2);

	char targetString[20] = { 0 };
	sprintf(targetString, "%s-%s-%s 12:00:00", strYear.c_str(), strMonth.c_str(), strDay.c_str());

	return targetString;
}