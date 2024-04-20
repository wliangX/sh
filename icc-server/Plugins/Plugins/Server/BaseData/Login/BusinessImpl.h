 #pragma once
#include "CRegisterInfo.h"
#include "CMQClientInfo.h"
#include "CClientNameInfo.h"
#include "CAlarmOnlineList.h"
#include "CDBOnlineList.h"
#include "UserInfo.h"
#include "CConfigInfo.h"
#include <HttpClient/IHttpClientFactory.h>


//业务服务ClientId                      
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

//服务状态
#define  SERVER_STATUS_START_COMPLETE   "server_start_complete"

//客户端类型
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
//在离席
#define READY_STATE_BUSY					"busy"
#define READY_STATE_IDLE					"idle"
#define CTI_LOGIN_STATE						"login"

//部门用户关系
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

		// MQ客户端的连接与断开 [3/12/2018 w26326]
		void OnCNotifiConnect(ObserverPattern::INotificationPtr p_pNotifiConnect);
		void OnCNotifiDisConnect(ObserverPattern::INotificationPtr p_pNotifiDisConnect);
		void OnCNotifiClientRegisterRequest(ObserverPattern::INotificationPtr p_pNotifiRegisterRequest);
		void OnCNotifiGetClientInfoRequest(ObserverPattern::INotificationPtr p_pNotifiGetClientInfo);
		void OnCNotifiGetOnOffInfo(ObserverPattern::INotificationPtr p_pNotifiRequest);

		// IP、坐席绑定
		void OnCNotifiSetBindIpSeat(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void OnCNotifiDeleteBindIpSeat(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void OnCNotifiGetBindIpSeat(ObserverPattern::INotificationPtr p_pNotifiRequest);

		// 获取用户在离席信息 [4/25/2018 w26326]
		void OnCNotifiGetInOutInfo(ObserverPattern::INotificationPtr p_pNotifiGetInOutInfo);
		void OnCNotifiSetInOutInfo(ObserverPattern::INotificationPtr p_pNotifiSetInOutInfo);
		void OnCNotifiDeleteClientRegisterRequest(ObserverPattern::INotificationPtr p_pNotifiDeleteRegisterRequest);

		void SetUserDept(const CRegisterInfo& p_ClientRegisterInfo);
		void DelUserDept(std::string p_strSeatNo);
		void UpdateUserDept(std::string p_strSeatNo,std::string p_steState);
		
		void OnNotifiLoadConfigRequest(ObserverPattern::INotificationPtr p_pNotifiLoadConfigRequest);
		void OnNotifiUploadConfigRequest(ObserverPattern::INotificationPtr p_pNotifiUploadConfigRequest);
		//修改参数
		void OnNotifiUpdateParamRequest(ObserverPattern::INotificationPtr p_pNotifiUpdateParamRequest);
		//下载参数
		void OnNotifiGetAllParamRequest(ObserverPattern::INotificationPtr p_pNotifiGetAllParamRequest);
		void BuildRespondHeader(PROTOCOL::CHeader& p_oRespondHeader, const PROTOCOL::CHeader& p_oRequestHeader);
		// 获取所有配置文件 [5/25/2018 w26326]
		void OnGetAllConfigRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

	    //客户端心跳处理
		void OnNotifClientHeartBeat(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//超时检测
		void OnTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//查询空闲席位
		void OnQueryFreeSeatRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);
		
		//查询当前用户绑定的席位
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
		//查询用户对应的部门名称
		bool GetDeptCodeByUser(const std::string& p_strUserCode, std::string& p_strDeptCode, std::string& p_strDeptName);
		//写入上下线信息
		bool InsertUserOnOff(const std::string& p_strGuid, const std::string & p_strUserCode, const std::string & p_strClientID, const std::string& p_strDeptCode, const std::string& p_strCurrentTime, const PROTOCOL::CClientRegisterRequest::CBody& p_oRequestBody);
		//通过席位号获取席位类型
		unsigned int GetSeatType(const std::string& p_strUserCode);
		//通过获取单位类型
		std::string GetDeptType(const std::string& p_strDeptCode);

		//插件启动时，保持数据库与Redis的一致性
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
		Redis::IRedisClientPtr		m_pRedisClient;		//redis客户端
		HelpTool::IHelpToolPtr      m_pHelpTool;
		DataBase::IPGClientPtr		m_pIPGClient;		
		IHttpClientPtr              m_pHttpClient;		
		Timer::ITimerManagerPtr	m_pTimerMgr;

		bool m_bAlarmFirst;
		bool m_bDbFirst;
		bool m_bIpAuthFlag;
		std::string m_strClientName;

		bool m_bStartedTimerFlag;


		std::string m_strHeartBeatTime; //客户端登陆心跳间隔时间

		unsigned int m_uiTimeOutTime;  //客户端超时时间
	};
}