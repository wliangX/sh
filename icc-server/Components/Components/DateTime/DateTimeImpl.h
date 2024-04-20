#pragma once

namespace ICC
{
	namespace DateTime
	{
		/*
		* class   时间实现类
		* author  w16314
		* purpose
		* note
		*/
		class CDateTimeImpl : public IDateTime
		{
		public:
			CDateTimeImpl();
			virtual ~CDateTimeImpl();
		public:
			// 从字符串解析日期时间
			virtual CDateTime FromString(std::string p_strDateTime);
			// 将日期时间转化为字符串
			virtual std::string ToString(CDateTime p_DateTime, std::string p_strFormat = DATETIME_STRING_FORMAT_NO_MICROSECOND);

			virtual std::string ToStringEx(std::time_t p_DateTime, std::string p_strFormat = DATETIME_STRING_FORMAT_NO_MICROSECOND);

			virtual CDateTime FromTime_T(std::time_t p_DateTime);
			virtual std::time_t ToTime_T(CDateTime p_DateTime);

			virtual std::string FromUTCToLocal(const std::string& strUTCDateTime, int iDefaultOffset = 8, std::string p_strFormat = DATETIME_STRING_FORMAT_NO_MICROSECOND);
			virtual std::string FromLocalToUTC(const std::string& strLocalDateTime, int iDefaultOffset = -8, std::string p_strFormat = DATETIME_STRING_FORMAT_NO_MICROSECOND);

			//创建流水号
			virtual std::string CreateSerial();
			virtual std::string CreateSerial2();
			virtual std::string CreateSerial(const int& iZoneSecond); //资阳使用
			// 获取当前时间
			virtual CDateTime CurrentDateTime();
			virtual CDateTime CurrentLocalDateTime();

			// 获取当前时间
			virtual std::string CurrentDateTimeStr();
			virtual std::string CurrentLocalDateTimeStr();

			//添加秒数
			virtual CDateTime AddSeconds(CDateTime p_DateTime, int p_iSeconds);
			//添加分钟数
			virtual CDateTime AddMinutes(CDateTime p_DateTime, int p_iMinutes);
			//添加小时数
			virtual CDateTime AddHours(CDateTime p_DateTime, int p_iHours);
			//添加天数
			virtual CDateTime AddDays(CDateTime p_DateTime, int p_iDays);
			//添加月数
			virtual CDateTime AddMonths(CDateTime p_DateTime, int p_iMonths);
			//添加年数
			virtual CDateTime AddYears(CDateTime p_DateTime, int p_iYears);
			//计算时间差（秒）
			virtual unsigned int SecondsDifference(CDateTime p_st, CDateTime p_et);
			//计算时间差（年）
			virtual unsigned int DaysDifference(CDateTime p_st, CDateTime p_et);

			//获取指定时间的前n秒(默认一天)
			virtual std::string GetFrontTime(std::string strTargetTime, long long l_secs = 86400);
			//获取指定时间的后n秒(默认一天)
			virtual std::string GetAfterTime(std::string strTargetTime, long long l_secs = 86400);
			//获取接警单ID时间
			virtual std::string GetAlarmIdTime(std::string strAlarmId);
			//获取话务ID时间
			virtual std::string GetCallRefIdTime(std::string strCallRefId);
			//获取派警单ID时间
			virtual std::string GetDispatchIdTime(std::string strDispatchId);

		private:
			static boost::mt19937 m_engine;//随机数种子
			std::string _ConvertLocalUTC(const std::string& strDateTime, int iOffset, std::string p_strFormat = DATETIME_STRING_FORMAT_NO_MICROSECOND);
		};
	}
}