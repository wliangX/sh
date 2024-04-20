#include "Boost.h"
#include "BusinessImpl.h"
//#include "CommonLogger.h"
#include "Protocol/CAlarmSync.h"
#include "Format.h"
#include "AlarmProcesser.h"
#include "SeatStatusChangedProcesser.h"
#include "AlarmStatisticsProcesser.h"
#include "SeatInfoChangedProcesser.h"
#include "SeatDeleteProcesser.h"
#include "AgentListResponseProcesser.h"
#include "SeatStorager.h"

const unsigned long THREADID_RECEIVEMSG = 10000;
const unsigned long THREADID_SENDMSG = 10001;

const unsigned long TIMERID_STATISTICS = 20000;
const unsigned long TIMERINTERVAL_STATISTICS = 5 * 60 * 1000;
#define INTERVAL_MIN_THREEMINUTE 3 * 60
#define INTERVAL_MAX_ONEDAY 24 * 3600

namespace ICC
{
	CBusinessImpl* g_pBusinessImpl = nullptr;

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	void CBusinessImpl::OnInit()
	{		
		m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(GATEWAY_IFDS_OBSERVER_CENTER);
		if (!m_pObserverCenter)
		{
			ICC_LOG_ERROR(m_pLog, "%s", "observer center is null!");
		}

		m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
		if (!m_pLog)
		{
			return;
		}

		ICC_LOG_DEBUG(m_pLog, "%s", "CBusinessImpl::OnInit() begin.");

		m_pConfig = ICCGetIConfigFactory()->CreateConfig();
		if (!m_pConfig)
		{					
			ICC_LOG_ERROR(m_pLog, "%s", "config is null!");
		}		

		m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
		if (!m_pDBConn)
		{
			ICC_LOG_ERROR(m_pLog, "%s", "dbcon is null!");
		}

		/*m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
		if (!m_pRedisClient)
		{
		WRITE_ERROR_LOG("redisclient is null!");
		}*/

		m_pJsonFty = ICCGetIJsonFactory();
		if (!m_pJsonFty)
		{			
			ICC_LOG_ERROR(m_pLog, "%s", "json factory is null!");
		}

		m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
		if (!m_pDateTime)
		{			
			ICC_LOG_ERROR(m_pLog, "%s", "datetime is null!");
		}

		m_pStrUtil = ICCGetIStringFactory()->CreateString();
		if (!m_pStrUtil)
		{			
			ICC_LOG_ERROR(m_pLog, "%s", "strutil is null!");
		}

		/*m_pHelpTool = ICCGetHelpToolFactory()->CreateHelpTool();
		if (!m_pHelpTool)
		{
			WRITE_ERROR_LOG("helptool is null!");
		}*/

		/*m_timerMgrPtr = ICCGetITimerFactory()->CreateTimerManager();
		if (!m_timerMgrPtr)
		{
			WRITE_ERROR_LOG("timermgr is null!");
		}*/

		_InitProcessers();
		
		ICC_LOG_DEBUG(m_pLog, "%s", "CBusinessImpl::OnInit() end.");
       
	}

	void CBusinessImpl::OnStart()
	{		
	  
		ICC_LOG_DEBUG(m_pLog, "%s", "CBusinessImpl::OnStart() begin.");

		std::string l_strIsUsing = m_pConfig->GetValue("ICC/Plugin/IFDS/IsUsing", "0");
		if (l_strIsUsing != "1")
		{
			ICC_LOG_INFO(m_pLog, "there is no need to load the plugin");
			return;
		}


		for (ProcessersMap::const_iterator itr = m_mapProcessers.begin(); itr != m_mapProcessers.end(); ++itr)
		{
			if (itr->second)
			{
				itr->second->InitProcesser();
			}
		}

		if (m_pObserverCenter)
		{
			ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "acd_agent_state_sync", _OnCNotifySeatStatusChanged);
			ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "cti_connect_state_sync", _OnCNotifyCTIStateChanged);
			ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_sync", _OnCNotifyAlarmInfoEnter);
			ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "seat_sync", _OnCNotifySeatInfoChanged);			
			ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_agent_list_respond", _OnCNotifyAgentListResponse);
		}
		else
		{			
			ICC_LOG_DEBUG(m_pLog, "%s", "observercenter is null! not init!");
		}

		if (m_pSeatStorager)
		{
			m_pSeatStorager->InitStorager();
		}	
		
		_InitStatisticsTimer();

		ICC_LOG_DEBUG(m_pLog, "%s", "CBusinessImpl::OnStart() end.");	
        		
	}

	void CBusinessImpl::OnStop()
	{
		ICC_LOG_DEBUG(m_pLog, "%s", "CBusinessImpl::OnStop() begin.");		

		m_statisticsTimer.RemoveTimer(TIMERID_STATISTICS);

		for (ProcessersMap::const_iterator itr = m_mapProcessers.begin(); itr != m_mapProcessers.end(); ++itr)
		{
			if (itr->second)
			{
				itr->second->UninitProcesser();
			}
		}

		ICC_LOG_DEBUG(m_pLog, "%s", "CBusinessImpl::OnStop() end.");		
	}

	void CBusinessImpl::OnDestroy()
	{
		ICC_LOG_DEBUG(m_pLog, "%s", "CBusinessImpl::OnDestroy() begin.");		

		m_mapProcessers.clear();

		ICC_LOG_DEBUG(m_pLog, "%s", "CBusinessImpl::OnDestroy() end.");		
	}

	void CBusinessImpl::SendMessage(const std::string& strMsg, int iType)
	{
		if (strMsg.empty())
		{			
			ICC_LOG_ERROR(m_pLog, "%s", "msg is empty!");
			return;
		}
		m_sendThread.AddMessage(this, THREADID_SENDMSG, iType, 0, 0, nullptr, strMsg);
	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	CBusinessImpl::CBusinessImpl()
	{
		g_pBusinessImpl = this;
		
	}

	CBusinessImpl::~CBusinessImpl()
	{
	}

	void CBusinessImpl::_InitProcessers()
	{
		if (m_pConfig && m_pJsonFty && m_pDBConn && m_pStrUtil && m_pDateTime && m_pLog)
		{			
			m_pSeatStorager = std::make_shared<CSeatStorager>(m_pDBConn, m_pLog, m_pJsonFty, m_pStrUtil, m_pDateTime);
			if (!m_pSeatStorager)
			{
				ICC_LOG_ERROR(m_pLog, "%s", "alloc memory for create seatstorager failed!");
				return;
			}
			m_mapProcessers.insert(std::make_pair(MsgType_Alarm, std::make_shared<CAlarmProcesser>(m_pJsonFty, m_pStrUtil, m_pDateTime, m_pLog, m_pDBConn, m_pConfig)));
			m_mapProcessers.insert(std::make_pair(MsgType_SeatSatausChanged, std::make_shared<CSeatStatusChanngedProcesser>(m_pJsonFty, m_pStrUtil, m_pDateTime, m_pLog, m_pSeatStorager)));
			m_mapProcessers.insert(std::make_pair(MsgType_SeatInfoChanged, std::make_shared<CSeatInfoChangedProcesser>(m_pJsonFty, m_pStrUtil, m_pDateTime, m_pLog, m_pSeatStorager)));
			m_mapProcessers.insert(std::make_pair(MsgType_AgentListReponse, std::make_shared<CAgentListResponseProcesser>(m_pJsonFty, m_pStrUtil, m_pDateTime, m_pLog, m_pSeatStorager)));
			m_mapProcessers.insert(std::make_pair(MsgType_AlarmStatistics, std::make_shared<CAlarmStatisticsProcesser>(m_pJsonFty, m_pDBConn, m_pStrUtil, m_pDateTime, m_pLog, m_pConfig)));	

			
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "%s", "components not load success!");			
		}
	}

	bool CBusinessImpl::_InitStatisticsTimer()
	{
		printf("init statistics timer begin!\n");
		std::string strTmpLog;
		unsigned long long ulStatisticsInterval = 0;
		std::string strStatisticsInterval = m_pConfig->GetValue("ICC/Plugin/IFDS/StatisticsInterval", "");
		strTmpLog = StringCommon::format("read confid ICC/Plugin/IFDS/StatisticsInterval, value = %s", strStatisticsInterval);
		ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());
		if (strStatisticsInterval.empty())
		{
			ulStatisticsInterval = TIMERINTERVAL_STATISTICS;
			ICC_LOG_DEBUG(m_pLog, "%s", "config interval is empty! will use default value!");
		}
		else
		{
			unsigned long ulConfigInterval = std::stoul(strStatisticsInterval);
			if (ulConfigInterval >= INTERVAL_MIN_THREEMINUTE && ulConfigInterval <= INTERVAL_MAX_ONEDAY)
			{
				ulStatisticsInterval = ulConfigInterval;
				ulStatisticsInterval *= 1000;
			}
			else
			{
				ulStatisticsInterval = TIMERINTERVAL_STATISTICS;
				ICC_LOG_DEBUG(m_pLog, "%s", "config interval is not valid! will use default value!");
			}
		}

		strTmpLog = StringCommon::format("statistics real interval, value = %Lu", ulStatisticsInterval);
		ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());
		m_statisticsTimer.AddTimer(this, TIMERID_STATISTICS, ulStatisticsInterval);
		
		printf("init statistics timer end!\n");
		
		return true;
	}

	void CBusinessImpl::_OnReceiveMessage(MsgType type, ObserverPattern::INotificationPtr p_pNotify)
	{
		if (!p_pNotify)
		{
			ICC_LOG_ERROR(m_pLog, "%s", "notify is null!");
			return;
		}

		std::string l_strRecvMsg(p_pNotify->GetMessages());
		if (l_strRecvMsg.empty())
		{
			ICC_LOG_ERROR(m_pLog, "%s", "msg is empty!");
			return;
		}

		std::string strTmpLog = StringCommon::format("receive msg = %s", l_strRecvMsg);
		ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());

		m_receiveThread.AddMessage(this, THREADID_RECEIVEMSG, type, 0, 0, nullptr, l_strRecvMsg);
	}

	void CBusinessImpl::_OnCNotifyAlarmInfoEnter(ObserverPattern::INotificationPtr p_pNotify)
	{
		_OnReceiveMessage(MsgType_Alarm, p_pNotify);
	}

	void CBusinessImpl::_OnCNotifySeatInfoChanged(ObserverPattern::INotificationPtr p_pNotify)
	{
		_OnReceiveMessage(MsgType_SeatInfoChanged, p_pNotify);
	}

	void CBusinessImpl::_OnCNotifySeatStatusChanged(ObserverPattern::INotificationPtr p_pNotify)
	{
		_OnReceiveMessage(MsgType_SeatSatausChanged, p_pNotify);
	}

	void CBusinessImpl::_OnCNotifyCTIStateChanged(ObserverPattern::INotificationPtr p_pNotify)
	{
		//_OnReceiveMessage(MsgType_CtiStateChanged, p_pNotify);
		_OnReceiveMessage(MsgType_SeatSatausChanged, p_pNotify);
	}

	void CBusinessImpl::_OnCNotifyAgentListResponse(ObserverPattern::INotificationPtr p_pNotify)
	{
		_OnReceiveMessage(MsgType_AgentListReponse, p_pNotify);
	}

	void CBusinessImpl::_DispatchMessage(MsgType type, const std::string& strMsg)
	{
		ProcessersMap::const_iterator itr;
		itr = m_mapProcessers.find(type);
		if (itr != m_mapProcessers.end())
		{
			if (itr->second != nullptr)
			{
				std::string strResult = itr->second->ProcNotify(strMsg);
				if (!strResult.empty())
				{
					SendMessage(strResult, msgSendType_Post);
				}
			}
			else
			{
				std::string strTmpLog = StringCommon::format("processer is nullptr! type = %d msg = %s", (int)type, strMsg);
				ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
			}
		}
	}

	void CBusinessImpl::_NotifyMessage(const std::string& strMsg, msgSendType iType)
	{
		std::string strTmpLog;
		if (iType == msgSendType_Post)
		{
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(strMsg));
			strTmpLog = StringCommon::format("post msg complete! msg = %s", strMsg);
			ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());
		}
		else if (iType == msgSendType_RequestRespond)
		{
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(strMsg, ObserverPattern::ERequestMode::Request_Respond));
			strTmpLog = StringCommon::format("send msg complete! msg = %s", strMsg);
			ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());
		}
		else
		{
			strTmpLog = StringCommon::format("send msg error! not support send type,  msg = %s, type = %d", strMsg, int(iType));
		}
		
	}

	void CBusinessImpl::ProcMessage(CommonThread_data msg_data)
	{
		switch (msg_data.msg_id)
		{
		case THREADID_RECEIVEMSG:
		    {
			    try
			    {
					_DispatchMessage((MsgType)msg_data.lParam, msg_data.str_msg);
			    }			    
			    catch (...)
			    {
					ICC_LOG_ERROR(m_pLog, "%s", "unknown exception!");
			    }
		    }
		    break;
		case THREADID_SENDMSG:
		    {
				try
				{
					_NotifyMessage(msg_data.str_msg, (msgSendType)msg_data.lParam);
				}				
				catch (...)
				{
					ICC_LOG_ERROR(m_pLog, "%s", "unknown exception!");
				}
		    }
			break;
		default:
			break;
		}
	}

	void CBusinessImpl::OnTimer(unsigned long uMsgId)
	{		
		if (uMsgId == TIMERID_STATISTICS)
		{
			m_receiveThread.AddMessage(this, THREADID_RECEIVEMSG, MsgType_AlarmStatistics, 0, 0, nullptr, "");			
		}
	}
}

