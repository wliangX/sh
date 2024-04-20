#pragma once

#include "CommonWorkThread.h"
#include "Department.h"
#include "Staff.h"
#include "Dict.h"
#include "User.h"
#include "Bind.h"
#include "Auth.h"
#include "Language.h"
#include "Role.h"

#define SYSTEMID				"icc_server"
#define SUBSYSTEMID				"icc_server_smp"
#define CMD_SMP_TIMER           "smp_timer"

namespace ICC
{
	class CBusinessImpl;
	typedef int (CBusinessImpl::*SmpDataAnalyze)(const std::string& strMessage, bool bIsLast);

	struct DataNotify
	{
		std::string strAction;
		std::string strCallback;
		std::string strCmd;
		std::string strDataGuid;
	};

	struct DataVersion
	{
		DataVersion()
		{
			m_strDeptVersion = "null";
			m_strStaffVersion = "null";
			m_strDictVersion = "null";
			m_strAuthVersion = "null";
			m_strUserVersion = "null";
			m_strLanguageVersion = "null";
			m_strUserStaffBindVersion = "null";
			m_strUserRoleBindVersion = "null";
			m_strRoleAuthorityBindVersion = "null";
		}


		std::string m_strDeptVersion;
		std::string m_strStaffVersion;
		std::string m_strDictVersion;
		std::string m_strAuthVersion;
		std::string m_strUserVersion;
		std::string m_strLanguageVersion;
		std::string m_strUserStaffBindVersion;
		std::string m_strUserRoleBindVersion;
		std::string m_strRoleAuthorityBindVersion;
	};

    class CBusinessImpl :  public CBusinessBase, public CCommonThreadCore
    {
    public:
        virtual void OnInit();
        virtual void OnStart();
		virtual void OnStop();
        virtual void OnDestroy();
    public:
		void OnRequestInfo(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void OnCheckLicense(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void LoadDeptMapInfo();
		std::string TransformDeptTypeCode(std::string p_strSMPDeptTypeCode);	
    public:
        //��SMP�����������
        void RequestInfoFunc();

		//////////////////////////////////////////////////////////////////////////
        //������֯������Ϣ
        void OnGetOrganInfor(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//��֯��������޸�
		void OnSmpOrgNotify(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//������֯����֪ͨ
		void SendOrganRequest(const std::vector<PROTOCOL::CGetDeptInfoRequest::CBody::CData>& p_DeptInfo);
		void SendOrganChangeRequest(const std::vector<PROTOCOL::CDeptInfoChangeRequest::CBody::CData>& p_DeptInfo, const std::string& p_strSyncType);

		void OnNotifiDoubleServerSync(ObserverPattern::INotificationPtr p_pNotifiRequest);
		

		//////////////////////////////////////////////////////////////////////////
        //���澯Ա��Ϣ
        void OnGetStaffInfor(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//��Ա����޸�
		void OnSmpStaffNotify(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//���;�Ա��Ϣ֪ͨ
		void SendStaffRequest(const std::vector<PROTOCOL::CGetStaffInfoRequest::CBody::CData>& p_StaffInfo);
		void SendStaffChangeRequest(const std::vector<PROTOCOL::CStaffInfoChangeRequest::CBody::CData>& p_StaffInfo, const std::string& p_strSyncType);
		

		//////////////////////////////////////////////////////////////////////////
        //�����ֵ���Ϣ
        void OnGetDictInfor(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//�ֵ����޸�
		void OnSmpDictNotify(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//�����ֵ���Ϣ֪ͨ
		void SendDictRequest(const std::vector<PROTOCOL::CGetDictInfoRequest::CBody::CData>& p_DictInfo);
		void SendDictChangeRequest(const std::vector<PROTOCOL::CDictInfoChangeRequest::CBody::CData>& p_DictInfo, const std::string& p_strSyncType);
		void SendDictDelRequest(const std::vector<PROTOCOL::CDictInfoChangeRequest::CBody::CData>& p_DictInfo);
		
		//////////////////////////////////////////////////////////////////////////
		//�����û���Ϣ
		void OnGetSmpUserInfo(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//�����û���Ϣ֪ͨ
		void SendUserRequest(const std::vector<PROTOCOL::CGetUserInfoRequest::CBody::CData>& p_UserInfo);
		
		//�û�����޸�
		void OnSmpUserNotify(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void UserDeleteNotify(const std::vector<PROTOCOL::CUserInfoChangeRequest::CBody::CData>& p_rvecData);
		void UserChangeNotify(const std::vector<PROTOCOL::CUserInfoChangeRequest::CBody::CData>& p_rvecData, const std::string& p_strSyncType);
		void UserBindChangeNotify(std::string p_strBindType, const std::vector<PROTOCOL::CUserInfoChangeRequest::CBody::CData>& p_rvecData);
		void UserUnBindChangeNotify(std::string p_strBindType, const std::vector<Data::CBindInfo>& p_rvecData);

		//////////////////////////////////////////////////////////////////////////
		//���湦����Ϣ func info
		void OnGetAuthInfor(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//���͹�����Ϣ֪ͨ
		void SendAuthRequest(const std::vector<PROTOCOL::CGetAuthInfoRequest::CBody::CData>& p_AuthInfo);
		
		//������Ϣ�������֪ͨ
		void OnSmpAuthNotify(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void FuncChangeNotify(const std::vector<PROTOCOL::CSMPFuncInfoChangeNotify::CBody::CData>& p_rvecData, const std::string& p_strSyncType);//SMP���������޸� >>> BaseData������
		void FuncDeleteNotify(const std::vector<PROTOCOL::CSMPFuncInfoChangeNotify::CBody::CData>& p_rvecData);

		//////////////////////////////////////////////////////////////////////////
        //����������Ϣ
        void OnGetDictLanguageInfo(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//����������Ϣ֪ͨ
		void SendLanguageRequest(const std::vector<PROTOCOL::CGetLanguageInfoRequest::CBody::CData>& p_LanguageInfo);

		//������Ϣ�������֪ͨ
		void OnSmpLanguageNotify(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void LanguageChangeNotify(const std::vector<PROTOCOL::CSMPLangInfoChangeNotify::CBody::CData>& p_rvecData, const std::string& p_strSyncType);//SMP���������޸� >>> BaseData������
		void LanguageDeleteNotify(const std::vector<PROTOCOL::CSMPLangInfoChangeNotify::CBody::CData>& p_rvecData);

		//��ɫ��Ϣ�������֪ͨ
		void OnSmpRoleNotify(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void RoleChangeNotify(const std::vector<PROTOCOL::CSMPRoleInfoChangeNotify::CBody::CData>& p_rvecData, const std::string& p_strSyncType);//SMP���������޸� >>> BaseData������
		void RoleDeleteNotify(const std::vector<PROTOCOL::CSMPRoleInfoChangeNotify::CBody::CData>& p_rvecData);
		void RoleBindFuncRequest(const std::vector<PROTOCOL::CSMPRoleInfoChangeNotify::CBody::CData>& p_rvecData);

		//�û�����Ա
        void OnGetSmpUserStaffBind(ObserverPattern::INotificationPtr p_pNotifiRequest);
		//�����û�����Ա��Ϣ֪ͨ
		void SendUserStaffBindRequest(const std::vector<PROTOCOL::CGetUserStaffInfoRequest::CBody::CData>& p_BindInfo);
		
		//�û��󶨽�ɫ
        void OnGetUserRoleInfo(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void SendUserRoleRequest(const std::vector<PROTOCOL::CGetUserRoleInfoRequest::CBody::CData>& p_rUserRole);
		void SendRoleRequest(const PROTOCOL::CGetUserRoleInfoRequest::CBody::CData::CRoleInfo& p_rRoleInfo);

		//ϵͳ��ɫ��Ϣ  PROTOCOL::CGetUserAuthorityInfoRequest::CBody::CData&
		void OnGetSysRoleInfo(ObserverPattern::INotificationPtr p_pNotifiRequest);
		void SendSysRoleInfoRequest(const std::vector<PROTOCOL::CGetRoleFuncBindRequest::CBody::CData>& p_rSysRoleInfo);

		void OnSmpSeatNotify(ObserverPattern::INotificationPtr p_pNotifiRequest);


		//�ͻ��˻�ȡlicense��Ϣ
		void OnGetExpiration(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void OnGetSynDataResult(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void GeneralHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader);

		void OnReceiveSynNacosParams(ObserverPattern::INotificationPtr p_pNotifiRequest);

	private:
		virtual void ProcMessage(CommonThread_data msg_data);

		void CheckLicense();

		bool _LoadBaseData();


		void _RequestLanguageInfo();
		void _RequestOrgnInfo();
		void _RequestStaffInfo();
		void _RequestDictInfo();
		void _RequestFuncInfo();
		void _RequestUserInfo();
		void _RequestUserStaffBindInfo();
		void _RequestSysRoleInfo();
		void _RequestUserRoleInfo();
		
		int _ProcReceiveLanguageInfo(const std::string& strMessage, bool bIsLast = false);
		int _ProcReceiveOrgnInfo(const std::string& strMessage, bool bIsLast = false);
		int _ProcReceiveStaffInfo(const std::string& strMessage, bool bIsLast = false);
		int _ProcReceiveDictInfo(const std::string& strMessage, bool bIsLast = false);
		int _ProcReceiveFuncInfo(const std::string& strMessage, bool bIsLast = false);
		int _ProcReceiveUserInfo(const std::string& strMessage, bool bIsLast = false);
		int _ProcReceiveUserStaffBindInfo(const std::string& strMessage, bool bIsLast = false);
		int _ProcReceiveSysRoleInfo(const std::string& strMessage, bool bIsLast = false);
		int _ProcReceiveUserRoleInfo(const std::string& strMessage, bool bIsLast = false);
		int _ProcReceiveSeatsInfo(const std::string& strMessage, bool bIsLast = false);

		bool _ProcReceiveLanguageInfoNotify(const std::string& strMessage);
		bool _ProcReceiveOrgnInfoNotify(const std::string& strMessage);
		bool _ProcReceiveStaffInfoNotify(const std::string& strMessage);
		bool _ProcReceiveDictInfoNotify(const std::string& strMessage);
		bool _ProcReceiveFuncInfoNotify(const std::string& strMessage);
		bool _ProcReceiveUserInfoNotify(const std::string& strMessage);		
		bool _ProcReceiveSysRoleInfoNotify(const std::string& strMessage);
		bool _ProcReceiveSeatNotify(const std::string& strMessage);

		void _TransSmpOrgToIcc(const PROTOCOL::SmpDataOrg& org, Data::CDeptInfo& deptInfo);
		void _TransSmpStaffToIcc(const PROTOCOL::SmpDataStaff& staff, Data::CStaffInfo& staffInfo);
		void _TransSmpDictToIcc(const PROTOCOL::SmpDataDict& dict, Data::CDictInfo& dictInfo);
		void _TransSmpFuncToIcc(const PROTOCOL::SmpDataFunc& func, Data::CAuthInfo& funcInfo);
		void _TransSmpUserToIcc(const PROTOCOL::SmpDataUser& user, Data::CUserInfo& userInfo);
		void _TransSmpRoleToIcc(const PROTOCOL::SmpDataRole& role, Data::CRoleInfo& roleInfo);
		

		void _SynData();
		int _ProcSynData();
		int _ProcSynDataEx();
		void _SendSynDataSuccess();	

		bool _AnalyzeNotify(const std::string& strMessage, std::vector<DataNotify>& notifyDatas);
		std::string _GetNotifyData(const DataNotify& notifyData);
		Data::IBasicDataManagerPtr _CreateIBasicDataManager(const std::string& strCmd);
		std::string                _PacketTarget(const std::string& strCmd, const std::string& strCallback, const std::string& strData);

		void _InitDataAnalyzers();

		bool _LoginSmp(std::string& strToken);
		bool _LogoutSmp(const std::string& strToken);

		bool _AnalyzeValidSynCmds(const std::string& strSynCmds);
		bool _AnalyzeSynPackages(const std::string& strMessage, std::string& strPackageCount, std::string& strVersion);
		bool _AnalyzeSmpDataChangedNotify(const std::string& strMessage);

		std::string _ReadTmpResults(const std::string& strFile);

		bool _QuerySmpService();
		
		void _SyncDictUpdateToClient();

		void _SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName);
		void _GetNacosParams(std::string& strNacosIp, std::string& strNacosPort, std::string& strNameSpace, std::string& strGroupName);

    private:
		Log::ILogPtr m_pLog;
		Config::IConfigPtr m_pConfig;
		Timer::ITimerManagerPtr	m_pTimerMgr;
		StringUtil::IStringUtilPtr m_pString;
		DateTime::IDateTimePtr m_pDateTime;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		Redis::IRedisClientPtr m_pRedisClient;
		License::ILicensePtr m_pLicense;
		DataBase::IDBConnPtr m_pDBConn;

		IHttpClientPtr m_pHttpClient;
	private:
		DataVersion m_dataVersion;

		std::string m_strSystemType;	//ͬ����ϵͳ�µĵ�λ��Ĭ��ΪICC
		std::string m_strRootDeptCode; // ICCϵͳ�������ڵ㵥λ�����־���������ڵ�ĸ��ڵ���Ҫ�ر���
		std::string m_strAutoSyncUserStaffBind;

		std::string m_strSyncTime;
		std::string m_strSyncHour;
		std::string m_strSyncMinute;
		std::string m_strCheckLicense;
		bool        m_bStartedTimerFlag;
		bool        m_bFirstSynDataFlag;	//����ʱ�Ƿ�� SMP ��ȡ����		
		std::atomic_bool m_bSynDataSuccess;

		std::string m_strNacosServerIp;
		std::string m_strNacosServerPort;
		std::string m_strNacosNamespace;
		std::string m_strSmpServiceName;
		std::string m_strNacosGroupName;		
		std::string m_strNacosQueryUrl;		
		std::string m_strNacosSuccessFlag;
		std::string m_strSmpServiceHealthyFlag;

		std::string m_strSyncProcessSeatFlag;

		boost::mutex     m_mutexNacosParams;

		std::atomic_bool m_bNeedSynData;

		boost::mutex     m_mutexSyning;
		std::atomic_bool m_bIsSyning;

		CCommonWorkThread m_threadProc;

		boost::shared_ptr<boost::thread>   m_threadSynData;

		std::map<std::string, std::string> m_mapValidSynCmds;

		std::string m_strSmpServerIp;
		std::string m_strSmpPort;
		std::string m_strUser;
		std::string m_strPassword;
		std::string m_strLoginType;
		std::string m_strSystemCode;
		std::string m_strPublicCode;
		std::string m_strGatewayServiceName;
		int         m_iPackageSize;
		std::string m_strIgnoreHttpError;
		std::vector<std::string> m_vecSmpSynCmds;
		int         m_nTimeOut;
		std::map<std::string, SmpDataAnalyze> m_mapSmpDataAnalyzers;

		std::vector<std::string> m_vecSmpSynCmdsEx;
		std::map<std::string, Data::IBasicDataManagerPtr> m_mapSmpDataAnalyzersEx;

		Data::IBaseDataPtr m_pLanguage;
		Data::IBaseDataPtr m_pOrg;
		Data::IBaseDataPtr m_pStaff;
		Data::IBaseDataPtr m_pDict;
		Data::IBaseDataPtr m_pFunc;
		Data::IBaseDataPtr m_pUser;
		Data::IBaseDataPtr m_pUserBindStaff;
		Data::IBaseDataPtr m_pUserBindRole;		
		Data::IBaseDataPtr m_pRoleBindFunc;
		Data::IBaseDataPtr m_pRoleBindData;
		Data::IBaseDataPtr m_pRole;		

    private:
		//SMP����������ICCϵͳ�Ĳ�������ӳ���ϵ��"BMLX00X"-"DIC01400X"
		std::map<std::string, std::string> m_mapDeptCode;


    };
}