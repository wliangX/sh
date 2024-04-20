#pragma once


#include "CommonWorkThread.h"
#include "INotifyProcesser.h"
#include "SeatStorager.h"
#include "CommonDef.h"
#include "CommonTimer.h"

namespace ICC
{
	

	class CBusinessImpl : public CBusinessBase, public CCommonThreadCore, public CCommonTimerCore
	{	
	public:
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();

		void         SendMessage(const std::string& strMsg, int iType);

	public:
		CBusinessImpl();
		~CBusinessImpl();

	private:
		virtual void OnTimer(unsigned long uMsgId);

		void _OnCNotifySeatStatusChanged(ObserverPattern::INotificationPtr p_pNotify);
		void _OnCNotifyCTIStateChanged(ObserverPattern::INotificationPtr p_pNotify);
		void _OnCNotifyAlarmInfoEnter(ObserverPattern::INotificationPtr p_pNotify);
		void _OnCNotifySeatInfoChanged(ObserverPattern::INotificationPtr p_pNotify);		
		void _OnCNotifyAgentListResponse(ObserverPattern::INotificationPtr p_pNotify);

		virtual void ProcMessage(CommonThread_data msg_data);

		void _OnReceiveMessage(MsgType type, ObserverPattern::INotificationPtr p_pNotify);

		void _DispatchMessage(MsgType type, const std::string& strMsg);
		void _NotifyMessage(const std::string& strMsg, msgSendType iType);

		void _InitProcessers();
		void _UninitProcessers();

		bool _InitStatisticsTimer();
	
	private:
		Log::ILogPtr m_pLog;
		Config::IConfigPtr m_pConfig;
		DataBase::IDBConnPtr m_pDBConn;
		//Redis::IRedisClientPtr	m_pRedisClient;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
		DateTime::IDateTimePtr m_pDateTime;
		StringUtil::IStringUtilPtr m_pStrUtil;
		HelpTool::IHelpToolPtr m_pHelpTool;

		Timer::ITimerManagerPtr m_timerMgrPtr;

		CCommonWorkThread m_receiveThread;
		CCommonWorkThread m_sendThread;

		typedef std::map<MsgType, std::shared_ptr<INotifyProcesser>> ProcessersMap;
		ProcessersMap m_mapProcessers;

		std::shared_ptr<CSeatStorager> m_pSeatStorager;

		CCommonTimer                    m_statisticsTimer;
	};
}