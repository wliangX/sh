#pragma once

#include "ManagerTransportQueue.h"
#include "TransportAlarmInfo.h"

namespace ICC
{
	class CBusinessImpl :
		public CBusinessBase
	{
		enum eTransportResult
		{
			TransportSuccess = 0,
			TransportFail,
			TransportAcceptTimeOut,
			TransportRefuse,
			TransportHangUp = 12
		};
		enum eTransferCallResult
		{
			TransferCallSuccess=0,
			TransferCall_RingStae,
			TransferCall_NoCallrefID = 6,
			TransferCallFail_Busy = 12,
			TransferCallFail_Timeout,
			TransferCallFail_HangUp,
			TransferCallFail_Cancel,
			TransferCallFail_HangUpByOwner,
			TransferCallFail_UnIdentify = 255
		};
	public:
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();

	public:
		void OnCNotifiTransportAlarmRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnCNotifiTransportCacheRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnCNotifiTransportAcceptAlarmRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnCNotifiTransportRefuseAlarmRequest(ObserverPattern::INotificationPtr p_pNotify);

	public:
		void SetBindMember();
		std::string GetFreeSeatNo(std::string p_strDeptCode, std::string p_strSrcSeatNo);
		bool IsFreeSeatNo(std::string p_strDeptCode,std::string p_strSeatNo);
		void OnTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);
		unsigned int GetRandIndex(unsigned int p_nSize);
		std::string GenAlarmLogContent(const std::vector<std::string>& p_vecParamList);
		std::string GenAlarmLogAttach(std::string& type, std::string& id);

	protected:
		void SendTransferCallRequest(std::string p_strAlarmID);										// ����ת�绰�����첽�ȴ���� [4/28/2018 w26326]
		void SendTransNewAlarmIDRequest(std::string p_strAlarmID);									// ���������µľ���ID�����첽�ȴ���� [4/27/2018 w26326]
		void SendTopicTransportInfo(std::string p_strAlarmID,std::string p_strTargetClient, const std::string& p_strTransportType = "");		// ����ת���㲥��Ϣ [4/24/2018 w26326]
		void SendTopicTransResultSync(std::string p_strAlarmID, eTransportResult p_estate,std::string p_strNewAlarmID, std::string p_strTransportType = "");
		void SendReCallRequest(std::string p_strAlarmID);										    // ����绰���� [6/29/2018 w26326]

		void OnTransferCallSync(ObserverPattern::INotificationPtr p_oTransferCallResp);				// ת�绰���ͬ�� [6/29/2018 w26326]
		void OnTransNewAlarmIDResponse(ObserverPattern::INotificationPtr p_oNewAlarmIDRespond);

	protected:
		bool IsValidForTransPortRequest(const PROTOCOL::CTransPortAlarmRequest &p_oTransAlarmRequest, std::string &out_strErrorMsg);						// У����ת������ [4/24/2018 w26326]
		void SendTransportAlarmRespond(eTransportResult p_eState, ObserverPattern::INotificationPtr p_pNotifiRequest, std::string p_strRelateID, const std::string &p_strMsg);	  // �ظ�ת������ [5/3/2018 w26326]
		void SendTransportAcceptAlarmRespond(eTransportResult p_eState, ObserverPattern::INotificationPtr p_pNotifiRequest, std::string p_strRelateID);// �ظ�����ת������ [5/3/2018 w26326]

		bool AddAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_pAlarmLogInfo);
		bool InsertDBAlarmLogInfo(PROTOCOL::CAlarmLogSync::CBody& p_AlarmLogInfo);
		void SyncAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_rAlarmLogToSync);
		void EditTransportAlarmLog(const std::vector<std::string>& p_vecParamList, std::string p_strAlarmID,std::string p_strType);
		void EditTakeOverCallLog(const std::vector<std::string>& p_vecParamList, const std::string&  p_strAlarmID, const std::string& p_strCTICallRefId);
		bool CopyAlarmLog(const std::string& p_strNewAlarmID, const std::string& p_strAlarmID);

	protected:
		bool InsertTransportTable(const PROTOCOL::CTransPortAlarmRequest& p_oTransAlarmRequest);
		bool UpdateTransportTable(const PROTOCOL::CAlarmTransportAcceptRequest::CBody& p_UpdateInfo);
		bool DeleteTransportTable(std::string p_strAlarmID);									// ʧ��ʱ����ת����¼�����ݿ���ɾ�� [4/28/2018 w26326]
		bool UpdateTransportTable(std::string p_strAlarmID,std::string p_strNewAlarmID, std::string& p_strNewCallrefID);		// ת�绰�ɹ��󣬸���ת�绰��� [5/3/2018 w26326]
		bool UpdateAlarm(const PROTOCOL::CAlarmInfo& p_oAlarmInfo);// ������� [4/26/2018 w26326]
		bool UpdateAlarm(std::string p_strAlarmID, std::string p_strNewAlarmID, const PROTOCOL::CTransPortAlarmRequest& p_oInfo, std::string strCallRefID, std::string strNewCallrefID);			// ������Ϻ����ԭ����Ϊ������� [4/27/2018 w26326]
		void SyncAlarmReceiptInfo(std::string p_strAlarmID, std::string p_strState, bool bIsDelete);

		void _AssignReceipterInfo(const std::string& strSeatNo, std::string& strStaffCode, std::string& strStaffName);

	public:
		void AddTransportQue(std::string p_strAlarmID, CTransportQueue& p_tTransportQueue);
		void DeleteTransportQue(std::string p_strAlarmID);
		void AddRelateID(std::string p_strRelateID, std::string p_strAlarmID);
		bool GetAlarmIDByRelateID(std::string p_strRelateID, std::string& p_strAlarmID);
		bool GetAlarmInfoByAlarmID(std::string p_strAlarmID, PROTOCOL::CTransPortAlarmRequest&p_oInfo);
		bool GetAlarmInfoByAlarmIDEx(std::string p_strAlarmID, PROTOCOL::CTransPortAlarmRequest&p_oInfo, std::string& p_strTransportType);
		void SetTransCallInfo(const PROTOCOL::CAlarmTransferCallRespond& p_oTransCallByDeptResp, std::string p_strAlarmID);
		void SetTraget(std::string p_strTarget, std::string p_strAlarmID);
		void SetUpdateTime(std::string p_strUpdateTime, std::string p_strAlarmID);
		void SetTraget(std::string p_strUserName,std::string p_strUserCode, std::string p_strAlarmID);
		bool GetTransCallInfo(std::string p_strAlarmID, PROTOCOL::CAlarmTransferCallRespond&p_oInfo);
		bool GetInfoByTimerName(std::string p_strTimerName, std::string &p_strAlarmID, PROTOCOL::CTransPortAlarmRequest&p_oInfo);
		bool UpdateCarInfo(const std::string& strSourceAlarmID, const std::string& strNewAlarmID);
		bool _GetStaffInfo(const std::string& strStaffCode, Data::CStaffInfo& l_oStaffInfo);
		std::string	_GetPoliceTypeName(const std::string& strStaffType, const std::string& strStaffName);
		std::string _GetStaffCodeBySeat(const::std::string& strSeatNo);
		bool _BuildDeptInfo(const std::string& strDeptCode, std::string& strDeptName, std::string& strDeptDistrictCode);
		std::string _IsExistAdminDept(const std::string& strAlarmID);
	private:
		DataBase::IDBConnPtr	m_pDBConn;
		HelpTool::IHelpToolPtr  m_pHelpTool;
		Lock::ILockPtr			m_pDBLock;
		Config::IConfigPtr		m_pConfig;
        Log::ILogPtr			m_pLog;
		Lock::ILockFactoryPtr	m_pLockFty;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
        StringUtil::IStringUtilPtr	m_pString;
		DateTime::IDateTimePtr		m_pDateTime;
		Timer::ITimerManagerPtr		m_pTimerMgr;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		Redis::IRedisClientPtr	m_pRedisClient;			//redis�ͻ���
		std::string m_strDefaultVDN;
		unsigned int m_iTimeOut;						//ת����ʱʱ�䣬Ĭ��20s

		//config
		std::string m_strAssistantPolice;	//�������Ƿ�Ϊ����
		std::string m_strPolice;			//�������Ƿ�Ϊ��
		std::string m_strCodeMode;			//��Ա����ģʽ��1����Ա���֤���� 2����Ա���
	};
}




