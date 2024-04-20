#pragma once

#include "TaskManager.h"

namespace ICC
{
	////////////////////////////////////////////////////////////////////////////////
	class CAvayaSwitch;
	class CTestCallInfo
	{
	public:
		long			m_lCSTACallRefId;
		std::string		m_strCTICallRefId;
		std::string		m_strCalled;
		std::string		m_strCaller;
		std::string		m_strAgentNo;
	};

	class CCTITestUdpHelper : public Udp::IUdpCallBack
	{
	public:
		CCTITestUdpHelper(Log::ILogPtr	pLogPtr, HelpTool::IHelpToolPtr pHelpTool, Udp::IUdpPtr pUdpServer);
		~CCTITestUdpHelper();

		void OnStart();
		void OnStop();

		void SetStringUtilPtr(StringUtil::IStringUtilPtr p_stringUtilPtr) { m_pStringUtilPtr = p_stringUtilPtr; }
		/********************************************************
		*  @function :  CUdpHelper::OnReceived
		*  @brief    :  UDP服务回调函数,处理所有收到的消息
		*  @input    :  p_pData, p_iLength, p_strRemoteIP, p_iPort
		*  @output   :
		*  @return   :
		*  @author   :  z13060  2018/05/30 17:08
		********************************************************/
		virtual void OnReceived(const char* p_pData, unsigned int p_iLength, std::string p_strRemoteIP, size_t p_iPort);

		void SetTestCallInfo(const string& strCaller, const string& strCalled, const string& strAgentNo, long nCallRefId);
		bool GetTestCallInfoByCaller(const string& strCaller, CTestCallInfo& pCallInfo);
		bool GetTestCallInfoByRefId(long nCallRefId, CTestCallInfo& pCallInfo);

		bool CTITestCallOut(long p_lRequestId,const string& strCaller, const string& strCalled, string& strCallRefId);
	private:
		Log::ILogPtr                        m_pLog;
		HelpTool::IHelpToolPtr				m_pHelpTool;
		Udp::IUdpPtr						m_pUdp;

		StringUtil::IStringUtilPtr			m_pStringUtilPtr;

		std::map<std::string, CTestCallInfo> m_mapCallInfo;

		int									m_nIntCallRefId;
	};
	typedef boost::shared_ptr<CCTITestUdpHelper> CTITestUdpHelperPtr;
	////////////////////////////////////////////////////////////////////////////////

	class CAvayaSwitchManager
	{
	public:
		CAvayaSwitchManager();
		virtual ~CAvayaSwitchManager();

		static boost::shared_ptr<CAvayaSwitchManager> Instance();
		void ExitInstance();

		bool StartTestSwitchService();

		void _OpenStreamThreadFun();
	public:
		void OnInit(IResourceManagerPtr p_pResourceManager, ISwitchEventCallbackPtr p_pCallback);

		void OnStart();
		void OnStop();

		virtual IResourceManagerPtr GetResourceManager()
		{
			return m_pResourceManager;
		}

		typedef int (CAvayaSwitchManager::*PCmdSwitch)(ITaskPtr p_pTask);
		typedef void (CAvayaSwitchManager::*PSwitchEvent)(ITaskPtr p_pTask);

	public:
		void SetSwitchEventCallbackPtr(ISwitchEventCallbackPtr p_pCallback){ m_pSwitchEventCallback = p_pCallback; }
	
		void SetSwitchConnect(bool p_bConnect)	{ m_bSwitchConnect = p_bConnect; }
		bool GetSwitchConnect()					{ return m_bSwitchConnect; }


		void OpenStreamProcess(bool p_bIsReOpen);
		//////////////////////////////////////////////////////////////////////////
		void DoWindowsMsg();
		void StartWindowsMsgThread();
		void StopWindowsMsgThread();

		/* TSAPI Server */
		//创建一个流
		bool OpenStream();

		//////////////////////////////////////////////////////////////////////////
		// 应用层请求任务
		/*static */void DoCmdTask();
		void ExeCommand(E_TASK_NAME p_nTaskName, long p_lTaskId, ITaskPtr p_pTask);
		void StartCmdTaskThread();
		void StopCmdTaskThread();

		// 交换机主动推送的事件和应用层请求的结果反馈
		/*static */void DoSwitchEventTask();
		void ExeSwitchEvent(E_TASK_NAME p_nTaskName, long p_lTaskId, ITaskPtr p_pTask);
		void StartSwitchEventTaskThread();
		void StopSwitchEventTaskThread();

		//	任务超时检测
		void ResetCheckExcusedTime();
		bool CheckExcusedIsTimeout();
		/*static */void DoCheckExcusedTask();
		void StartCheckExcusedTaskThread();
		void StopCheckExcusedTaskThread();
		
		/* Switch Event */
		void FailedEvent(ITaskPtr p_pTask);
		void AgentStateEvent(ITaskPtr p_pTask);
		void DeviceStateEvent(ITaskPtr p_pTask);
		void ConferenceHangupEvent(ITaskPtr p_pTask);
		void CallStateEvent(ITaskPtr p_pTask);
		void CallOverEvent(ITaskPtr p_pTask);
		void RefuseCallEvent(ITaskPtr p_pTask);

		/* Failed Event*/
		void ProcessFailedCmd(E_TASK_NAME p_nTaskName, long p_lRequestId, int p_nErrorCode);
		void QueryDeviceInfoFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg = "");
		void SetAgentStateFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg = "");
		void MakeCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg = "");
		void ClearConnectionFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg = "");
		void ClearCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg = "");
		void AnswerCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg = "");
		void SingleStepConferenceFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg = "");
		void PickupCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg = "");
		void ConsultationCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg = "");
		void TransferCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg = "");
		void DeflectCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg = "");
		void HoldCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg = "");
		void RetrieveCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg = "");
		void ReconnectCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg = "");
		void ConferenceCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg = "");
		void MonitorDeviceFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg = "");
		void TakeOverCallFailed(long p_lRequestId, int p_nErrorCode, const std::string& p_strErrorMsg = "");

		/* Switch Conf */
		void UniversalFailRet(ITaskPtr p_pTask);
		void OpenStreamConf(ITaskPtr p_pTask);
		void QueryDeviceInfoRet(ITaskPtr p_pTask);
		void MonitorDeviceConf(ITaskPtr p_pTask);
		void RouteRegisterConf(ITaskPtr p_pTask);
		void SetAgentStateConf(ITaskPtr p_pTask);
		void QueryAgentStateRet(ITaskPtr p_pTask);
		void MakeCallConf(ITaskPtr p_pTask);
		void ClearConnectionConf(ITaskPtr p_pTask);
		void ClearCallConf(ITaskPtr p_pTask);
		void AnswerCallConf(ITaskPtr p_pTask);
	//	void RefuseAnswerConf(ITaskPtr p_pTask);
		void SingleStepConferenceConf(ITaskPtr p_pTask);
		void PickupCallConf(ITaskPtr p_pTask);
	//	void BargeInCallConf(ITaskPtr p_pTask);
		void ConsultationCallConf(ITaskPtr p_pTask);
		void TransferCallConf(ITaskPtr p_pTask);
		void DeflectCallConf(ITaskPtr p_pTask);
		void HoldCallConf(ITaskPtr p_pTask);
		void RetrieveCallConf(ITaskPtr p_pTask);
		void ReconnectCallConf(ITaskPtr p_pTask);
		void ConferenceCallConf(ITaskPtr p_pTask);
		/*void AddConferencePartyConf(ITaskPtr p_pTask);
		void DeleteConferencePartyConf(ITaskPtr p_pTask);*/
		void TakeOverCallConf(ITaskPtr p_pTask);

		void GetAESConnStateRet(ITaskPtr p_pTask);
		void GetDeviceListRet(ITaskPtr p_pTask);
		void GetACDListRet(ITaskPtr p_pTask);
		void GetAgentListRet(ITaskPtr p_pTask);
		void GetCallListRet(ITaskPtr p_pTask);
		void GetReadyAgentRet(ITaskPtr p_pTask);

		void GetFreeAgentListRet(ITaskPtr p_pTask);

		/* Switch cmd */
		int QueryDeviceInfo(ITaskPtr p_pTask);

		int MonitorDevice(ITaskPtr p_pTask);
		int MonitorCallsViaDevice(ITaskPtr p_pTask);
		int RouteRegisterReq(ITaskPtr p_pTask);
		int RouteRequest(ITaskPtr p_pTask);

		int AgentLogin(ITaskPtr p_pTask);
		int AgentLogout(ITaskPtr p_pTask);
		int SetAgentState(ITaskPtr p_pTask);
		int QueryAgentState(ITaskPtr p_pTask);
		
		int MakeCall(ITaskPtr p_pTask);
		int Hangup(ITaskPtr p_pTask);
		int AnswerCall(ITaskPtr p_pTask);
		int RefuseAnswer(ITaskPtr p_pTask);
		int RefuseBlackCall(ITaskPtr p_pTask);
		int ClearCall(ITaskPtr p_pTask);
		int ListenCall(ITaskPtr p_pTask);
		int PickupCall(ITaskPtr p_pTask);
		int BargeInCall(ITaskPtr p_pTask);
		int ForcePopCall(ITaskPtr p_pTask);
		int ConsultationCall(ITaskPtr p_pTask);
		int TransferCall(ITaskPtr p_pTask);
		int DeflectCall(ITaskPtr p_pTask);
		int HoldCall(ITaskPtr p_pTask);
		int RetrieveCall(ITaskPtr p_pTask);
		int ReconnectCall(ITaskPtr p_pTask);
		int ConferenceCall(ITaskPtr p_pTask);
	//	int AddConferenceParty(ITaskPtr p_pTask);
		int TakeOverCall(ITaskPtr p_pTask);

		int GetAESConnState(ITaskPtr p_pTask);
		int GetDeviceList(ITaskPtr p_pTask);
		int GetACDList(ITaskPtr p_pTask);
		int GetAgentList(ITaskPtr p_pTask);
		int GetCallList(ITaskPtr p_pTask);
		int GetReadyAgent(ITaskPtr p_pTask);

		int GetFreeAgent(ITaskPtr p_pTask);

		int SetBlackList(ITaskPtr p_pTask);
		int DeleteBlackList(ITaskPtr p_pTask);
		int DeleteAllBlackList(ITaskPtr p_pTask);
		
		bool isCTITestMode() { return m_bCTITestModeFlag; };
	public:
		bool m_bOpenStreamThreadAlive;
	private:
		static boost::shared_ptr<CAvayaSwitchManager> m_pInstance;

		bool m_bCmdTaskThreadAlive;
		bool m_bCheckExcusedTaskThreadAlive;
		bool m_bSwitchEventTaskThreadAlive;
		bool m_bWindowMsgThreadAlive;
		bool m_bSwitchConnect;
		static int m_nHeartBeatCount;		// CTI 与 AES 心跳检测次数

		Log::ILogPtr				m_pLog;
		StringUtil::IStringUtilPtr	m_pStrUtil;
		DateTime::IDateTimePtr		m_pDateTimePtr;
		DateTime::CDateTime			m_oLastCheckExcusedTime;
		ISwitchEventCallbackPtr		m_pSwitchEventCallback;

		std::map<int, PCmdSwitch>	m_mapCmdNameToSwitch;
		std::map<int, PSwitchEvent>	m_mapSwitchEvent;

		boost::shared_ptr<boost::thread>		m_pWindowsMsgThread;
		boost::shared_ptr<boost::thread>		m_pCmdTaskThread;
		boost::shared_ptr<boost::thread>		m_pSwitchEventTaskThread;
		boost::shared_ptr<boost::thread>		m_pCheckExcusedTaskThread;

		bool m_bCTITestModeFlag;
		HelpTool::IHelpToolPtr              m_pHelpTool;
		Udp::IUdpPtr						m_pUdp;
		CTITestUdpHelperPtr					m_CTITestUdpHelper;

		IResourceManagerPtr					m_pResourceManager;

		shared_ptr<boost::thread> m_threadOpenStream;
		
	};
} // end namespac
