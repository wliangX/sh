#pragma once

#include "TaskManager.h"
#include "WorkThread.h"
#include "FSAesHttpHelper.h"
#include "CTIFSAesNotifaction.h"
#include "RequestResultEvent.h"
#include "FSAesEventProtocol.h"
#include "CommonTimer.h"

#include "CallManager.h"
#include "CTICallbackFunHelper.h"
namespace ICC
{
	////////////////////////////////////////////////////////////////////////////////
	class CFSAesSwitchManager :public CCommonTimerCore
	{
	public:
		CFSAesSwitchManager();
		virtual ~CFSAesSwitchManager();

		static boost::shared_ptr<CFSAesSwitchManager> Instance();
		void ExitInstance();

		
	public:
		void OnInit(IResourceManagerPtr p_pResourceManager, ISwitchEventCallbackPtr p_pCallback);

		void OnStart();
		void OnStop();
		void OnDestroy();

		virtual IResourceManagerPtr GetResourceManager()
		{
			return m_pResourceManager;
		}

		void OnRecviceFSAesMessage(const std::string& p_strSrcIP, const std::string& p_strGuid, const std::string& p_strTarget, const std::string& p_strBody);
		void OnCheckCallTimeOut(const std::string& p_strCallId, const std::string& p_strCallState, const std::string& p_strCaller, const std::string& p_strCalled);
		std::string GetRequestCmdString(int p_nRequestType);

		void PostDeviceState(const std::string& p_strCTICallRefId, long p_lCallIndex, const CHcpDeviceCallEvent& p_HcpDeviceStateEvent, const std::string& p_strDirection="",
			const std::string& p_strOrgCaller = "");
		int PostInitateDeviceState(const std::string& p_strDeviceNum, const std::string& p_strCTICallRefId, const std::string& p_strState, const std::string& p_strCaller ="", const std::string& p_strCalled = "");

		void PostCallState(const std::string& p_strCTICallRefId, long p_lCallIndex, CHcpDeviceCallEvent& p_HcpAcdCallStateEvent);
		void PostCallOver(const std::string& p_strHcpCallRefId, ICTICallPtr p_pCTICallPtr,const std::string &p_strHangupType = "");
		void PostConferenceState(const std::string& p_strCTIConferenceId, const std::string& p_strTarget, const std::string& p_strCallState, const CHcpDeviceCallEvent& p_HcpAcdCallStateEvent);
		
		typedef void (CFSAesSwitchManager::* PHCPAesEvent)(const std::string&,ICHCPNotifyEeventPtr p_pHcpNotifyEvent);
	public:
		virtual void OnTimer(unsigned long uMsgId);

		void _StartWorkThreads();
		void _StopWorkThreads();

		void _ThreadFunCmdTask();
		void _ThreadFunCheckExcusedTask();
		void _ThreadFuncEventTask();

		void _DispatchRequestResult(const std::string& p_strCallbackFunName, ISwitchResultNotifPtr p_pResultNotify, const std::string& p_strCmdName = "");
		void DispatchCTIEvent(const std::string& p_strCallbackFunName, ISwitchNotifPtr m_pSwitchNotify, const std::string& p_strCmdName = "");
	private:
		void _AesLoginRequest();
		void _AesLogoutRequest();
		void _AesHeartBeatRequest();
		void _AesGetFreeAgentListRequest();
		void _AesGetExtensionListRequest();
		void _AesMonitorRequest(int l_nDeviceType);
		void _AesMemberMuteRequest(const std::string& p_strConferenceId, const std::string& p_strConferenceName, const std::string& p_strCallId, const std::string& p_strTarget);
		void _AesReleaseConferenceRequest(const std::string& p_strConferenceId, const std::string& p_strConferenceName, const std::string& p_strCallId);

		void _AesAutoHangupCallRequest(const std::string& p_strCallId,const std::string &p_strDeviceNum);
		void _AesAutoAnswserCallRequest(const std::string& p_strCallId, const std::string& p_strDeviceNum);
		bool _isRequestResultMsg(const std::string& p_strEventName);

		void _AllCallOver();
	private:
		

		void _ExeCommand(int p_nTaskType, long p_lTaskId, ITaskPtr p_pTask);
		void _ExeLocalGetEvent(long p_lTaskId, ITaskPtr p_pTask);
		void _ExeHcpNotifyEvent(int p_nTaskName, long p_lTaskId, ITaskPtr p_pTask);

		void _DispAsyncRequestFailedCmd(int p_nErrorCode, IFSAesCmdRequestPtr p_pAesCmdRequest);
		int _DispLocalGetCmd(long p_lTaskId, IFSAesCmdRequestPtr p_pAesCmdRequest);
		int _DispSyncRquestResultMsg(int p_nRequestType, int p_nRequestId, const std::string& p_strResponse);
		void _DispRecvRequestResult(const std::string& p_strResultMsg);
		void _DispRecvHCPNotifyEvent(const std::string p_strEventName, const std::string& p_strNotifyEvent);
		void _ExeSyncResultEvent(long p_lTaskId, ITaskPtr p_pTask);
		void _ExeAsyncResultEvent(long p_lTaskId, ITaskPtr p_pTask);
		
		void _InitHcpAesEventFun();
		void _DoHcpAgentStateEvent(const std::string &p_strEventName,ICHCPNotifyEeventPtr p_pHcpNotifyEvent);
		void _DoHcpCallStateEvent(const std::string& p_strEventName,ICHCPNotifyEeventPtr p_pHcpNotifyEvent);
		void _DoHcpDeviceStateEvent(const std::string& p_strEventName, ICHCPNotifyEeventPtr p_pHcpNotifyEvent);

		void _DoHcpConferenceCreateEvent(const std::string& p_strEventName, ICHCPNotifyEeventPtr p_pHcpNotifyEvent);
		void _DoHcpConferenceReleaseEvent(const std::string& p_strEventName, ICHCPNotifyEeventPtr p_pHcpNotifyEvent);
		void _DoHcpConferenceMemberEvent(const std::string& p_strEventName, ICHCPNotifyEeventPtr p_pHcpNotifyEvent);

		std::string _GetConferenceHangupType(const std::string& p_strCallDirection, const std::string& p_strCompere, const std::string& p_strLastCallOverMember);

		void _ProcessHangupCheckCallOver(ICTICallPtr p_pCTICallPtr, const CHcpDeviceCallEvent& p_HcpDeviceStateEvent);
	private:
		static boost::shared_ptr<CFSAesSwitchManager> m_pInstance;
		ISwitchEventCallbackPtr		m_pSwitchEventCallback;
		IResourceManagerPtr			m_pResourceManager;
		Log::ILogPtr				m_pLog;
		DateTime::IDateTimePtr		m_pDateTimePtr;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
		StringUtil::IStringUtilPtr	m_pStringPtr;

		std::map<std::string, PHCPAesEvent>	m_mapHcpAesEvent;
	private:
		CCommonTimer			m_myTimer;
		IWorkThreadPtr			m_pEventTaskThread;
		IWorkThreadPtr			m_pCmdTaskThread;
		IWorkThreadPtr			m_pCheckExcusedTaskThread;
		DateTime::CDateTime		m_oLastCheckExcusedTime;

		CFSAesHttpHelperPtr		m_pFsAesHttpHelper;

		CCTICallbackFunHelper	m_CallbackInterface;
		bool					m_bStart;
		bool					m_bLoginAesSuccess;//登录AES服务是否成功
		bool					m_bLoadExtensionFlag;

		std::string				m_strCurrASEServerIp;
		std::string				m_strLocalServerIP;

		int	m_nAESLoginTick;
		int m_nSendLoginCount;

		int	m_nHeartBeatErrorCount;

		bool m_bPhoneTransferEnable;  //是否支持话机上操作转移适配
		
	};
} // end namespac
