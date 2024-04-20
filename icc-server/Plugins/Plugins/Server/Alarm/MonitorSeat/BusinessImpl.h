#pragma once
#include <algorithm> 
#include <Timer/ITimerFactory.h> 

using namespace std;

namespace ICC
{
	class CBusinessImpl : public CBusinessBase
	{
	public:
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();
	public:
		/*请求班长席审批及审批结果*/
		void OnCNotifiSetMonitorSeatApply(ObserverPattern::INotificationPtr p_pNotify);

		/*获取班长席审批信息*/
		void OnCNotifiGetMonitorSeatApply(ObserverPattern::INotificationPtr p_pNotify);
	
		/*获取班长席单个审批信息*/
		void OnCNotifiGetSingleMonitorSeatApply(ObserverPattern::INotificationPtr p_pNotify);

		void OnReceiveSynNacosParams(ObserverPattern::INotificationPtr p_pNotifiRequest);
	private:

		//获取审批信息
		void _GetMonitorSeatApply(ObserverPattern::INotificationPtr p_pNotify, bool in_bSingel);

		//获取单个审批信息(2023.4.3添加，新增接口以免对原接口进行破坏）
		void _GetMonitorSeatSingleApply(ObserverPattern::INotificationPtr p_pNotify);
		
		//检验请求参数
		bool _ValidSetMonitorSeatApply(PROTOCOL::CMonitorSeatApplyRequest& in_oReq, std::string& out_strErrInfo);

		//检验获取审批的记录
		bool _ValidGetMonitorSeatApply(PROTOCOL::CMonitorSeatApplyQueryRequest& in_oReq, std::string& out_strErrInfo);

		//更新表数据
		bool _AddOrUpdateApproveRecord(const PROTOCOL::CMonitorSeatApplyRequest &in_oReq, std::string & out_strErrInfo, bool in_bInsert = true);

		//获取数据
		bool _GetApproveRecords(const PROTOCOL::CMonitorSeatApplyQueryRequest&in_oReq, PROTOCOL::CMonitorSeatApplyQueryRespond &out_oRes);
		//获取单个审批数据
		bool _GetSingleApproveRecords(const PROTOCOL::CMonitorSeatSingleApplyQueryRequest& in_oReq, PROTOCOL::CMonitorSeatSingleApplyQueryRespond& out_oRes);
		std::string _GetDeptGuid(std::string l_staffCode, std::string l_staffName);

		void SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName);
		std::string BuildApprovalMsg(int l_curType);

		//坐席登录状态主动告警
		void InitiativeAlarm(ObserverPattern::INotificationPtr p_pNotify);
		//获取icc_t_param里面的通知信息
		void BuildManualAcceptConfig(std::string l_manualAcceptType, std::string& l_value);

	private:
		Log::ILogPtr						m_pLog;
		Config::IConfigPtr					m_pConfig;
		DataBase::IDBConnPtr				m_pDBConn;
		StringUtil::IStringUtilPtr			m_pString;
		JsonParser::IJsonFactoryPtr			m_pJsonFty;
		DateTime::IDateTimePtr				m_pDateTime;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		MsgCenter::IMessageCenterPtr		m_pMsgCenter;
		Redis::IRedisClientPtr				m_pRedisClient;		//redis客户端
		Timer::ITimerManagerPtr				m_pTimerMgr;

		std::string							m_strNacosServerIp;
		std::string							m_strNacosServerPort;
		std::string							m_strNacosServerNamespace;
		std::string							m_strNacosServerGroupName;
		boost::mutex						m_mutexNacosParams;
		//用来记录上一次发送的时间
		std::string							m_strSendTime;
		//配置项，定时器延时时间，以秒为单位
		std::string							m_strTiming;
		//发送消息中心延时时间，单位为分钟
		std::string							m_strSendDelayTime;
	};
}