#include "Boost.h"
#include "BusinessImpl.h"
#include "StatisticsCallReceiveAlarm.h"
#include "StatisticsReceiveAlarmRate.h"
#include "StatisticsValidReceiveAlarm.h"
#include "StatisticsRealAlarmRate.h"
#include "StatisticsTotalProcessAlarm.h"
#include "StatisticsTotalFeedBack.h"
#include "StatisticsAlarmByLevel.h"
#include "StatisticsProcessResult.h"
#include "StatisticsProcessAndFeedBackRate.h"
#include "StatisticsAlarmByArea.h"
#include "StatisticsCommonTime.h"
#include "StatisticsTotalCallIn.h"
#include "StatisticsCountByCallInNumber.h"
#include "StatisticsCountByCallNature.h"
#include "StatisticsWaitingCallCount.h"
#include "StatisticsCallSynthesizeByCallCount.h"
#include "StatisticsCallSynthesizeByCallTime.h"
#include "StatisticsCallSynthesizeBySeatno.h"
#include "StatisticsProcessAndFeedbackByArea.h"
#include "StatisticsAuxiliaryReceiveAlarm.h"


#define TIMER_CMD_NAME "alarm_statistics"
#define ALARMSTATISTICS_OBSERVER_NAME ALARM_OBSERVER_CENTER

const unsigned long TIMERID_STATISTICS = 20000;
const unsigned long TIMERINTERVAL_STATISTICS = 60 * 1000;

void CBusinessImpl::OnInit()
{
	printf("OnInit enter! plugin = %s\n", MODULE_NAME);

	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(ALARMSTATISTICS_OBSERVER_NAME);
	m_pIConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pILog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pIDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pJsonFty = ICCGetIJsonFactory();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);

	//_InitAlarmStatisticsProcessers();

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{	
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);

	std::string strStatisticsTime = m_pIConfig->GetValue("ICC/Plugin/StatisticsAlarm/StatisticsStartTime", "1:00");	

	try
	{
		if (strStatisticsTime.empty())
		{
			throw "config is null!";
		}

		ICC_LOG_DEBUG(m_pILog, "alarm statistics hour = %s!", strStatisticsTime.c_str());

		std::vector<std::string> vecTime;
		m_pString->Split(strStatisticsTime, ":", vecTime, false);

		if (vecTime.size() != 2)
		{
			throw "config format is error!";
		}

		std::string strStatisticsHour = vecTime[0];
		std::string strStatisticsMinute = vecTime[1];
		if (strStatisticsHour.empty() || strStatisticsMinute.empty())
		{
			throw "conifg param is error";
		}

		m_iStatisticsHour = std::stoi(strStatisticsHour);
		m_iStatisticsMinute = std::stoi(strStatisticsMinute);

		if ((m_iStatisticsHour < 0 || m_iStatisticsHour > 23) || (m_iStatisticsMinute < 0 || m_iStatisticsMinute > 59))
		{
			throw "conifg param is expired";
		}	

	}	
	catch (const char* ex)
	{
		ICC_LOG_WARNING(m_pILog, "alarm statistics config error, msg = %s", ex);

		m_iStatisticsHour = 1;
		m_iStatisticsMinute = 0;
	}	

	ICC_LOG_DEBUG(m_pILog, "alarm statistics hour = %d, minute = %d!", m_iStatisticsHour, m_iStatisticsMinute);

	//m_statisticsTimer.AddTimer(this, TIMERID_STATISTICS, TIMERINTERVAL_STATISTICS);
	

	ICC_LOG_DEBUG(m_pILog, "alarm statistics start success!");
	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	m_statisticsTimer.RemoveTimer(TIMERID_STATISTICS);

	ICC_LOG_DEBUG(m_pILog, "alarm statistics stop success");
}

void CBusinessImpl::OnDestroy()
{

}

static bool bExec = false;
//定时器的响应
void CBusinessImpl::OnStatisticsTimer(unsigned long uMsgId)
{	
	ICC_LOG_DEBUG(m_pILog, "alarm statistics OnTimer enter");
	//if (!bExec)
	{
		_StatisticsProcess();
		bExec = true;//test code
	}	
}

void CBusinessImpl::_InitAlarmStatisticsProcessers()
{
	std::string strStatisticsCurrentDayFlag = m_pIConfig->GetValue("ICC/Plugin/StatisticsAlarm/NeedStatisticsCurrentDay", "0");

	std::shared_ptr<IAlarmStatistics> pAlarmStatistics;	
	
	pAlarmStatistics = std::make_shared<CStatisticsCallReceiveAlarm>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}
	
	pAlarmStatistics = std::make_shared<CStatisticsReceiveAlarmRate>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}

	pAlarmStatistics = std::make_shared<CStatisticsAuxiliaryReceiveAlarm>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}
	
	pAlarmStatistics = std::make_shared<CStatisticsValidReceiveAlarm>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}

	pAlarmStatistics = std::make_shared<CStatisticsRealAlarmRate>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}

	pAlarmStatistics = std::make_shared<CStatisticsTotalProcessAlarm>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}
	
	pAlarmStatistics = std::make_shared<CStatisticsTotalFeedBack>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}

	pAlarmStatistics = std::make_shared<CStatisticsAlarmByFirstType>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}

	pAlarmStatistics = std::make_shared<CStatisticsAlarmBySecondType>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}

	pAlarmStatistics = std::make_shared<CStatisticsAlarmByThirdType>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}

	pAlarmStatistics = std::make_shared<CStatisticsAlarmByFourthType>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}

	pAlarmStatistics = std::make_shared<CStatisticsAlarmResultByProcessed>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}

	pAlarmStatistics = std::make_shared<CStatisticsAlarmResultByFeedBack>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}

	pAlarmStatistics = std::make_shared<CStatisticsProcessAndFeedBackRate>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}

	pAlarmStatistics = std::make_shared<CStatisticsAlarmCountByArea>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}

	pAlarmStatistics = std::make_shared<CStatisticsTotalCallIn>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}

	pAlarmStatistics = std::make_shared<CStatisticsCountBy110>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}

	pAlarmStatistics = std::make_shared<CStatisticsCountBy119>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}

	pAlarmStatistics = std::make_shared<CStatisticsCountBy122>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}
	
	pAlarmStatistics = std::make_shared<CStatisticsCallNatureByAccept>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}

	pAlarmStatistics = std::make_shared<CStatisticsCallNatureByRelease>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}

	pAlarmStatistics = std::make_shared<CStatisticsCallNatureByDisturbance>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}

	pAlarmStatistics = std::make_shared<CStatisticsCallNatureByInside>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}

	pAlarmStatistics = std::make_shared<CStatisticsWaitingCallCount>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}

	pAlarmStatistics = std::make_shared<CStatisticsCallSynthesizeByCallCount>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}

	pAlarmStatistics = std::make_shared<CStatisticsCallSynthesizeByCallTime>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}

    pAlarmStatistics = std::make_shared<CStatisticsCallSynthesizeBySeatno>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}

	pAlarmStatistics = std::make_shared<CStatisticsProcessAndFeedbackByArea>(m_pILog, m_pDBConn, m_pString, m_pIDateTime);
	if (pAlarmStatistics)
	{
		m_vecAlarmStatisticsProcessers.push_back(pAlarmStatistics);
		pAlarmStatistics->SetStatisticsCurrentDayFlag(strStatisticsCurrentDayFlag);
	}

}

void CBusinessImpl::_StatisticsProcess()
{
	std::string strCurrentHour = CStatisticsCommonTime::CurrentHours(m_pIDateTime);
	std::string strCurrentMinute = CStatisticsCommonTime::CurrentMinute(m_pIDateTime);
	if (!strCurrentHour.empty() && !strCurrentMinute.empty())
	{
		int iCurrentHour = std::stoi(strCurrentHour);
		int iCurrentMinute = std::stoi(strCurrentMinute);
		if (iCurrentHour == m_iStatisticsHour && iCurrentMinute == m_iStatisticsMinute)
		{
			int iProcesserCount = m_vecAlarmStatisticsProcessers.size();
			for (int i = 0; i < iProcesserCount; ++i)
			{
				m_vecAlarmStatisticsProcessers[i]->Statistics();
			}
		}
		
	}	
}



