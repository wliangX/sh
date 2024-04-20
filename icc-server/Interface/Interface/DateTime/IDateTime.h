#pragma once

#include <string>
#include <ctime>
#include <IObject.h>
#include <TypeDef.h>

namespace ICC
{
	namespace DateTime
	{
		typedef uint64 CDateTime; //单位 微秒
		const CDateTime ERROR_DATE_TIME = 0;
		const std::string DEFAULT_DATETIME_STRING_FORMAT = "%04d-%02d-%02d %02d:%02d:%02d.%06d";
		const std::string DATETIME_STRING_FORMAT_NO_MICROSECOND = "%04d-%02d-%02d %02d:%02d:%02d";


		
		/*
		* class   时间类
		* author  w16314
		* purpose
		* note    字符串格式形如: 2017-01-02 23:04:05.123456
		*/
		class IDateTime : public IObject
		{
		public:

			//************************************
			// Method:    FromString
			// FullName:  ICC::IDateTime::FromString
			// Access:    virtual public 
			// Returns:   ICC::CDateTime
			// Qualifier: 从字符串解析日期时间
			// Parameter: std::string p_strDateTime
			//************************************
			virtual CDateTime FromString(std::string p_strDateTime) = 0;


			//************************************
			// Method:    ToString
			// FullName:  ICC::DateTime::IDateTime::ToString
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier:
			// Parameter: CDateTime p_DateTime
			// Parameter: std::string p_strFormat 
			//************************************
			virtual std::string ToString(CDateTime p_DateTime, std::string p_strFormat = DATETIME_STRING_FORMAT_NO_MICROSECOND) = 0;

			virtual std::string ToStringEx(std::time_t p_DateTime, std::string p_strFormat = DATETIME_STRING_FORMAT_NO_MICROSECOND) = 0;

			virtual std::string FromUTCToLocal(const std::string& strUTCDateTime, int iDefaultOffset = 8, std::string p_strFormat = DATETIME_STRING_FORMAT_NO_MICROSECOND) = 0;
			virtual std::string FromLocalToUTC(const std::string& strLocalDateTime, int iDefaultOffset = -8, std::string p_strFormat = DATETIME_STRING_FORMAT_NO_MICROSECOND) = 0;

			virtual CDateTime FromTime_T(std::time_t p_DateTime) = 0;
			virtual std::time_t ToTime_T(CDateTime p_DateTime) = 0;

			//************************************
			// Method:    CreateSerial
			// FullName:  ICC::DateTime::IDateTime::CreateSerial
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: 固定格式 %04d%02d%02d%02d%02d%02d%06d%02d
			//************************************
			virtual std::string CreateSerial() = 0;
			virtual std::string CreateSerial2() = 0;

			//资阳使用，增加时区的秒进行转换
			virtual std::string CreateSerial(const int &iZoneSecond) = 0;
			//************************************
			// Method:    CurrentDateTime
			// FullName:  ICC::IDateTime::CurrentDateTime
			// Access:    virtual public 
			// Returns:   ACS::CDateTime
			// Qualifier: 获取当前时间
			//************************************
			virtual CDateTime CurrentDateTime() = 0;

			//************************************
			// Method:    CurrentDateTimeStr
			// FullName:  ICC::IDateTime::CurrentDateTimeStr
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: 获取当前时间
			//************************************
			virtual std::string CurrentDateTimeStr() = 0;
			virtual std::string CurrentLocalDateTimeStr() = 0;

			//************************************
			// Method:    AddSeconds
			// FullName:  ICC::DateTime::IDateTime::AddSeconds
			// Access:    virtual public 
			// Returns:   ICC::DateTime::CDateTime
			// Qualifier:
			// Parameter: CDateTime p_DateTime
			// Parameter: int p_iSeconds 负数代表减去
			//************************************
			virtual CDateTime AddSeconds(CDateTime p_DateTime, int p_iSeconds) = 0;
			
			//************************************
			// Method:    AddMinutes
			// FullName:  ICC::DateTime::IDateTime::AddMinutes
			// Access:    virtual public 
			// Returns:   ICC::DateTime::CDateTime
			// Qualifier:
			// Parameter: CDateTime p_DateTime
			// Parameter: int p_iMinutes 负数代表减去
			//************************************
			virtual CDateTime AddMinutes(CDateTime p_DateTime, int p_iMinutes) = 0;

			//************************************
			// Method:    AddHours
			// FullName:  ICC::DateTime::IDateTime::AddHours
			// Access:    virtual public 
			// Returns:   ICC::DateTime::CDateTime
			// Qualifier:
			// Parameter: CDateTime p_DateTime
			// Parameter: int p_iHours 负数代表减去
			//************************************
			virtual CDateTime AddHours(CDateTime p_DateTime, int p_iHours) = 0;

			//************************************
			// Method:    AddDays
			// FullName:  ICC::DateTime::IDateTime::AddDays
			// Access:    virtual public 
			// Returns:   ICC::DateTime::CDateTime
			// Qualifier:
			// Parameter: CDateTime p_DateTime
			// Parameter: int p_iDays 负数代表减去
			//************************************
			virtual CDateTime AddDays(CDateTime p_DateTime, int p_iDays) = 0;
			//************************************
			// Method:    AddMonths
			// FullName:  ICC::DateTime::IDateTime::AddMonths
			// Access:    virtual public 
			// Returns:   ICC::DateTime::CDateTime
			// Qualifier:
			// Parameter: CDateTime p_DateTime
			// Parameter: int p_iMonths 负数代表减去
			//************************************
			virtual CDateTime AddMonths(CDateTime p_DateTime, int p_iMonths) = 0;
			//************************************
			// Method:    AddYears
			// FullName:  ICC::DateTime::IDateTime::AddYears
			// Access:    virtual public 
			// Returns:   ICC::DateTime::CDateTime
			// Qualifier:
			// Parameter: CDateTime p_DateTime
			// Parameter: int p_iYears 负数代表减去
			//************************************
			virtual CDateTime AddYears(CDateTime p_DateTime, int p_iYears) = 0;

			//************************************
			// Method:    SecondsDifference
			// FullName:  ICC::DateTime::IDateTime::SecondsDifference
			// Access:    virtual public 
			// Returns:   unsigned int
			// Qualifier:
			// Parameter: CDateTime p_st
			// Parameter: CDateTime p_et
			//************************************
			virtual unsigned int SecondsDifference(CDateTime p_st, CDateTime p_et) = 0;

			//************************************
			// Method:    DaysDifference
			// FullName:  ICC::DateTime::IDateTime::DaysDifference
			// Access:    virtual public 
			// Returns:   unsigned int
			// Qualifier:
			// Parameter: CDateTime p_st
			// Parameter: CDateTime p_et
			//************************************
			virtual unsigned int DaysDifference(CDateTime p_st, CDateTime p_et) = 0;

			virtual std::string GetFrontTime(std::string strTargetTime, long long l_secs = 86400) = 0;
			virtual std::string GetAfterTime(std::string strTargetTime, long long l_secs = 86400) = 0;
			virtual std::string GetAlarmIdTime(std::string strAlarmId) = 0;
			virtual std::string GetCallRefIdTime(std::string strCallRefId) = 0;
			virtual std::string GetDispatchIdTime(std::string strDispatchId) = 0;
		};

		typedef boost::shared_ptr<IDateTime> IDateTimePtr;
	}
}
