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
		/*����೤ϯ�������������*/
		void OnCNotifiSetMonitorSeatApply(ObserverPattern::INotificationPtr p_pNotify);

		/*��ȡ�೤ϯ������Ϣ*/
		void OnCNotifiGetMonitorSeatApply(ObserverPattern::INotificationPtr p_pNotify);
	
		/*��ȡ�೤ϯ����������Ϣ*/
		void OnCNotifiGetSingleMonitorSeatApply(ObserverPattern::INotificationPtr p_pNotify);

		void OnReceiveSynNacosParams(ObserverPattern::INotificationPtr p_pNotifiRequest);
	private:

		//��ȡ������Ϣ
		void _GetMonitorSeatApply(ObserverPattern::INotificationPtr p_pNotify, bool in_bSingel);

		//��ȡ����������Ϣ(2023.4.3��ӣ������ӿ������ԭ�ӿڽ����ƻ���
		void _GetMonitorSeatSingleApply(ObserverPattern::INotificationPtr p_pNotify);
		
		//�����������
		bool _ValidSetMonitorSeatApply(PROTOCOL::CMonitorSeatApplyRequest& in_oReq, std::string& out_strErrInfo);

		//�����ȡ�����ļ�¼
		bool _ValidGetMonitorSeatApply(PROTOCOL::CMonitorSeatApplyQueryRequest& in_oReq, std::string& out_strErrInfo);

		//���±�����
		bool _AddOrUpdateApproveRecord(const PROTOCOL::CMonitorSeatApplyRequest &in_oReq, std::string & out_strErrInfo, bool in_bInsert = true);

		//��ȡ����
		bool _GetApproveRecords(const PROTOCOL::CMonitorSeatApplyQueryRequest&in_oReq, PROTOCOL::CMonitorSeatApplyQueryRespond &out_oRes);
		//��ȡ������������
		bool _GetSingleApproveRecords(const PROTOCOL::CMonitorSeatSingleApplyQueryRequest& in_oReq, PROTOCOL::CMonitorSeatSingleApplyQueryRespond& out_oRes);
		std::string _GetDeptGuid(std::string l_staffCode, std::string l_staffName);

		void SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName);
		std::string BuildApprovalMsg(int l_curType);

		//��ϯ��¼״̬�����澯
		void InitiativeAlarm(ObserverPattern::INotificationPtr p_pNotify);
		//��ȡicc_t_param�����֪ͨ��Ϣ
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
		Redis::IRedisClientPtr				m_pRedisClient;		//redis�ͻ���
		Timer::ITimerManagerPtr				m_pTimerMgr;

		std::string							m_strNacosServerIp;
		std::string							m_strNacosServerPort;
		std::string							m_strNacosServerNamespace;
		std::string							m_strNacosServerGroupName;
		boost::mutex						m_mutexNacosParams;
		//������¼��һ�η��͵�ʱ��
		std::string							m_strSendTime;
		//�������ʱ����ʱʱ�䣬����Ϊ��λ
		std::string							m_strTiming;
		//������Ϣ������ʱʱ�䣬��λΪ����
		std::string							m_strSendDelayTime;
	};
}