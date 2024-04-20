 #pragma once
#include "CRegisterInfo.h"
#include "CMQClientInfo.h"
#include "CClientNameInfo.h"
#include "CAlarmOnlineList.h"
#include "CDBOnlineList.h"
#include "UserInfo.h"
#include "CConfigInfo.h"
#include <HttpClient/IHttpClientFactory.h>


//ҵ�����ClientId                      
#define SERVER_MONITOR					"ICC.Server.Monitor"
#define SERVER_SMP						"ICC.Server.SMP"
#define SERVER_DBAGENT					"ICC.Server.DBAgent"
#define SERVER_CTI						"ICC.Server.CTI"
#define SERVER_CONFIG					"ICC.Server.Config"
#define SERVER_BASEDATA					"ICC.Server.Basedata"
#define SERVER_AUTH						"ICC.Server.Auth"
#define SERVER_ALARM					"ICC.Server.Alarm"
#define SERVER_MPRS						"ICC.Server.MPRS"
#define SERVER_VCS						"ICC.Server.VCS"
#define SERVER_WEB						"ICC.Server.Web"

//����״̬
#define  SERVER_STATUS_START_COMPLETE   "server_start_complete"

//�ͻ�������
#define CTYPE_MONITOR					"ctype_server_monitor"
#define CTYPE_SMP						"ctype_server_smp"
#define CTYPE_DBAGENT					"ctype_server_dbagent"
#define CTYPE_CTI						"ctype_server_cti"
#define CTYPE_CONFIG					"ctype_server_config"
#define CTYPE_BASEDATA					"ctype_server_basedata"
#define CTYPE_AUTH						"ctype_server_auth"
#define CTYPE_ALARM						"ctype_server_alarm"
#define CTYPE_MPRS						"ctype_server_mprs"
#define CTYPE_VCS						"ctype_server_vcs"
#define CTYPE_WEB						"ctype_server_web"
#define CTYPE_CLIENT                    "ctype_client"

#define CONVERT_SECOND                   (1000*1000)
//����ϯ
#define READY_STATE_BUSY					"busy"
#define READY_STATE_IDLE					"idle"
#define CTI_LOGIN_STATE						"login"

//�����û���ϵ
#define LOGIN_USER_DEPT						"login_user_dept"

#define SELECT_CONFIG_ID	"select_icc_t_config"
#define GET_ALL_CONFIG_RESPOND "get_all_config_respond"

namespace ICC
{
	enum eClientStatus
	{
		ICC_CLIENT_LOGOUT = 0,
		ICC_CLIENT_LOGIN
	};
	class CBusinessImpl :
		public CBusinessBase
	{
	public:
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();

	public:
		void OnNotifiDoubleServerSync(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void OnCNotifiCheckUserRequest(ObserverPattern::INotificationPtr p_pNotifiCheckUserRequest);
		void OnCNotifiGetUserRequest(ObserverPattern::INotificationPtr p_pNotifiGetUserRequest);
		void OnCNotifiSetUserRequest(ObserverPattern::INotificationPtr p_pNotifiSetUserRequest);
		void OnCNotifiDeleteUserRequest(ObserverPattern::INotificationPtr p_pNotifiDeleteUserRequest);
		void OnCNotifiAcdAgentStateRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		// MQ�ͻ��˵�������Ͽ� [3/12/2018 w26326]
		void OnCNotifiConnect(ObserverPattern::INotificationPtr p_pNotifiConnect);
		void OnCNotifiDisConnect(ObserverPattern::INotificationPtr p_pNotifiDisConnect);
		void OnCNotifiClientRegisterRequest(ObserverPattern::INotificationPtr p_pNotifiRegisterRequest);
		void OnCNotifiGetClientInfoRequest(ObserverPattern::INotificationPtr p_pNotifiGetClientInfo);
		void OnCNotifiGetOnOffInfo(ObserverPattern::INotificationPtr p_pNotifiRequest);

		// IP����ϯ��
		void OnCNotifiSetBindIpSeat(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void OnCNotifiDeleteBindIpSeat(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void OnCNotifiGetBindIpSeat(ObserverPattern::INotificationPtr p_pNotifiRequest);

		// ��ȡ�û�����ϯ��Ϣ [4/25/2018 w26326]
		void OnCNotifiGetInOutInfo(ObserverPattern::INotificationPtr p_pNotifiGetInOutInfo);
		void OnCNotifiSetInOutInfo(ObserverPattern::INotificationPtr p_pNotifiSetInOutInfo);
		void OnCNotifiDeleteClientRegisterRequest(ObserverPattern::INotificationPtr p_pNotifiDeleteRegisterRequest);

		void SetUserDept(const CRegisterInfo& p_ClientRegisterInfo);
		void DelUserDept(std::string p_strSeatNo);
		void UpdateUserDept(std::string p_strSeatNo,std::string p_steState);
		
		void OnNotifiLoadConfigRequest(ObserverPattern::INotificationPtr p_pNotifiLoadConfigRequest);
		void OnNotifiUploadConfigRequest(ObserverPattern::INotificationPtr p_pNotifiUploadConfigRequest);
		//�޸Ĳ���
		void OnNotifiUpdateParamRequest(ObserverPattern::INotificationPtr p_pNotifiUpdateParamRequest);
		//���ز���
		void OnNotifiGetAllParamRequest(ObserverPattern::INotificationPtr p_pNotifiGetAllParamRequest);
		void BuildRespondHeader(PROTOCOL::CHeader& p_oRespondHeader, const PROTOCOL::CHeader& p_oRequestHeader);
		// ��ȡ���������ļ� [5/25/2018 w26326]
		void OnGetAllConfigRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

	    //�ͻ�����������
		void OnNotifClientHeartBeat(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//��ʱ���
		void OnTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//��ѯ����ϯλ
		void OnQueryFreeSeatRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		
		//��ѯ��ǰ�û��󶨵�ϯλ
		void OnQueryPreUserBindSeat(ObserverPattern::INotificationPtr p_pNotifiRequest);

	public:
		void SyncClient(CRegisterInfo p_ClientRegisterInfo, eClientStatus p_eState,int p_nLogoutCase);
	private:

		void _SyncAgentTimeOut(const CRegisterInfo &p_ClientRegisterInfo, const std::string &p_strSeatNo);
		bool InitConfigCache();
		bool LoadUserInfo();
		std::string _QueryGuidFromInOutByClientId(const std::string& strClientId);
		bool _UpdateClientInOutTime(const std::string& strGuid);

		std::string _QueryGuidFromOnOffByClientId(const std::string& strClientId);
		bool _UpdateClientOnOffTime(const std::string& strGuid);
				

		void _ClearAllICCClientRegistInfo();
		void _SynServerStatus(const std::string& strServerId);

		//void InitClientType();
		void InsertICCClientInOut(PROTOCOL::CSetInOutInfo::CBody l_InOutInfo);
		void UpdateICCClientInOut(PROTOCOL::CSetInOutInfo::CBody l_InOutInfo);
		//void BuildRespond(PROTOCOL::CClientRegisterRespond& p_oRespond, const PROTOCOL::CClientRegisterRequest& p_oRequest, std::string p_strResult);
		void BuildRespondBody(PROTOCOL::CClientRegisterRespond::CBody& p_oRespondBody, const PROTOCOL::CClientRegisterRequest::CBody& p_oRequestBody, std::string p_strResult);
		DataBase::SQLRequest ConstructSQLStruct(CUserInfo& p_oAccountInfo, std::string p_strOperator);
		void UpdateClientOffTime(const std::string& p_strClientId, const std::string& p_strClientName);

		bool _InsertStatisticRecord(const std::string& strGuid, const std::string& strClientName, const std::string& strClientId, const std::string& strDeptCode, const std::string& strOnTime);
		
		bool _UpdateStatistic(const std::string& strGuid);

		std::string _PacketUrl(const std::string& strServer);
		std::string _PacketAuthInfo(const std::string& strUser, const std::string& strPwd);
		bool _IsConnectionInfoEnable(const std::string& strAllConnectionInfo);
		void _GetAllActiveConnectionInfo(const std::string& strServer, std::string& strAllConnectionInfo);
		bool _ExistClientConnect(const std::string& strAllConnectionInfo, const std::string& strClientId);

		void _GetMqServers(std::vector<std::string>& vecServers);
		bool _IsClientActive(const std::string& strClientId);
		bool _ClearRegistInfo(const std::string& strClientId);

		bool GetUserInfo(const std::string & p_strUserCode, CUserInfo& p_oUserInfo);
		bool GetBindInfoByCode(const std::string & p_strUserCode, std::string & p_strStaffCode, std::string & p_strStaffName, std::string & p_strDeptCode, std::string & p_strDeptName);

		bool Auth(const std::string & p_strUserCode, const std::string & p_strClientID, const std::string& p_IPList);
		bool MultiClientAuth(const std::string & p_strUserCode, const std::string & p_strClientID, const std::string& p_IPList);
		//��ѯ�û���Ӧ�Ĳ�������
		bool GetDeptCodeByUser(const std::string& p_strUserCode, std::string& p_strDeptCode, std::string& p_strDeptName);
		//д����������Ϣ
		bool InsertUserOnOff(const std::string& p_strGuid, const std::string & p_strUserCode, const std::string & p_strClientID, const std::string& p_strDeptCode, const std::string& p_strCurrentTime, const PROTOCOL::CClientRegisterRequest::CBody& p_oRequestBody);
		//ͨ��ϯλ�Ż�ȡϯλ����
		unsigned int GetSeatType(const std::string& p_strUserCode);
		//ͨ����ȡ��λ����
		std::string GetDeptType(const std::string& p_strDeptCode);

		//�������ʱ���������ݿ���Redis��һ����
		bool GetParamInfo();
		bool InsertParamInfoToRedis(std::vector<std::map<std::string, std::string>> &vecParamInfo);
	private:
		DataBase::IDBConnPtr m_pDBConn;
		Config::IConfigPtr m_pConfig;
		Log::ILogPtr m_pLog;
		JsonParser::IJsonPtr			m_pJson;
		StringUtil::IStringUtilPtr      m_pString;
		JsonParser::IJsonFactoryPtr     m_pJsonFac;
		DateTime::IDateTimePtr			m_pDateTime;
		ObserverPattern::IObserverCenterPtr  m_pObserverCenter;
		Redis::IRedisClientPtr		m_pRedisClient;		//redis�ͻ���
		HelpTool::IHelpToolPtr      m_pHelpTool;
		DataBase::IPGClientPtr		m_pIPGClient;		
		IHttpClientPtr              m_pHttpClient;		
		Timer::ITimerManagerPtr	m_pTimerMgr;

		bool m_bAlarmFirst;
		bool m_bDbFirst;
		bool m_bIpAuthFlag;
		std::string m_strClientName;

		bool m_bStartedTimerFlag;


		std::string m_strHeartBeatTime; //�ͻ��˵�½�������ʱ��

		unsigned int m_uiTimeOutTime;  //�ͻ��˳�ʱʱ��
	};
}