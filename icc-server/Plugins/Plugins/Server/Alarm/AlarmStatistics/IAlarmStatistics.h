#ifndef __IAlarmStatistics_H__
#define __IAlarmStatistics_H__

namespace ICC
{
	class IAlarmStatistics
	{
	public:
		virtual int Statistics() = 0;
		void        SetStatisticsCurrentDayFlag(const std::string& strFlag){ m_strNeedStatisticsCurrentDayFlag = strFlag; }

	public:
		virtual ~IAlarmStatistics(){}
	protected:
		IAlarmStatistics(){}
	protected:
		std::string m_strNeedStatisticsCurrentDayFlag;
	};
}


#endif