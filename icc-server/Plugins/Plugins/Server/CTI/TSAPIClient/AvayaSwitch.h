#pragma once

#if defined(WIN32) || defined(WIN64)
#define WM_OPEN_STREAM	(WM_USER + 701)
#define WM_CLOSE_STREAM	(WM_USER + 702)
#define WM_TSAPI_MSG	(WM_USER + 703) 
#endif	//WIN32

namespace ICC
{
	class CAvayaSwitch
	{
	public:
		CAvayaSwitch(void);
		virtual ~CAvayaSwitch(void);

		static boost::shared_ptr<CAvayaSwitch> Instance();
		void ExitInstance();

		void OnInit(IResourceManagerPtr pResourceManager);
		void OnStart();
		void OnStop();

	public:
#if defined(WIN32) || defined(WIN64)
		bool CreateMyWindows();
		void CloseMyWindows();
		void DestroyMyWindows();
#endif // WIN32
		
		bool GetEventPoll();
		bool SetEventNotify();
		void ResetHandleEvent();
		static void HandleEvent(int p_nSig);
		static bool EventIsPending(){ return CAvayaSwitch::m_nEventIsPending > 0 ? true : false; }
		int/*LRESULT*/ OnTSAPIEvent(/*WPARAM wParam, LPARAM lParam*/);
		int/*LRESULT*/ OnOpenStreamEvent(/*WPARAM wParam, LPARAM lParam*/);
		int/*LRESULT*/ OnCloseStreamEvent(/*WPARAM wParam, LPARAM lParam*/);

	public:
		void SetTSAPIConfigFile(bool p_bMasterFlag = true);
		bool Initialize();
		bool InitializeSlave();
		void PostOpenStreamMessage();
		void PostCloseStreamMessage();

		int GetInitFailedMaxCount();

		void SetConnectSlaveAvayaFlag(bool l_bFlag);
		bool GetConnectSlaveAvayaFlag();
		//////////////////////////////////////////////////////////////////////////
		/* TSAPI Server */
		bool FindServerNameFromNet();
		bool AddServerName(const std::string& strServerName);
		std::string GetServerNameFromList();
	private:
		/* AES */
		bool OpenAvayaStream(bool p_bMasterFlag=true);
		bool CloseAvayaStream();
	public:
		static bool AbortStream(ACSHandle_t acsHandle,
			PrivateData_t	*priv);
		static bool FlushEventQueue(ACSHandle_t acsHandle);
		static int GetEventBlock(ACSHandle_t acsHandle,
			void			*eventBuf,
			unsigned short	*eventBufSize,
			PrivateData_t	*privData,
			unsigned short	*numEvents);

		/* Basic Call Control Services */
		bool AlternateCall(const std::string& strDeviceNum, long lActiveCallRefId, 
			const long lHoldCallRefId, InvokeID_t invokeId);
		bool AnswerCall(const std::string& strDeviceNum, long lCallRefId, InvokeID_t invokeId);
		static bool CallCompletion(ACSHandle_t acsHandle,
			InvokeID_t invokeId,
			Feature_t feature,
			ConnectionID_t	*call);
		static bool ClearCall(ACSHandle_t acsHandle,
			InvokeID_t invokeId,
			ConnectionID_t	*call);
		bool ClearCall(const std::string& strDeviceNum, long lCallRefId, InvokeID_t invokeId);
		bool ClearConnection(const std::string& strDeviceNum, long lCallRefId, InvokeID_t invokeId,bool isDynamicId = false);
		static bool ConferenceCall(ACSHandle_t acsHandle,
			InvokeID_t invokeId,
			ConnectionID_t* heldCall,
			ConnectionID_t* activeCall);
		bool ConferenceCall(const std::string& strDeviceNum, long lHeldCallRefId, long lActiveCallRefId, InvokeID_t invokeId);
		bool ConsultationCall(const std::string& strDeviceNum, const std::string& strCalledId, 
			long lCallRefId, InvokeID_t invokeId);
		bool DeflectCall(const std::string& strDeviceNum, const std::string& strCalledId,
			long lCallRefId, InvokeID_t invokeId);
		bool GroupPickupCall(const std::string& strDeviceNum, const std::string& strPickupId,
			long lCallRefId, InvokeID_t invokeId);
		bool HoldCall(const std::string& strDeviceNum, long lCallRefId, InvokeID_t invokeId);
		bool MakeCall(const std::string& strCallerId, const std::string& strCalledId,InvokeID_t invokeId);
		static bool MakePredictiveCall(ACSHandle_t acsHandle,
			InvokeID_t invokeId,
			const std::string& strCallerId,
			const std::string& strCalledId);
		bool PickupCall(const std::string& strDeviceNum, const std::string& strPickupId,
			long lCallRefId, InvokeID_t invokeId);

		bool ReconnectCall(const std::string& strDeviceNum, long lActiveCallRefId, long lHeldCallRefId, InvokeID_t invokeId);
		bool RetrieveCall(const std::string& strDeviceNum, long lCallRefId, InvokeID_t invokeId);
		bool TransferCall(const std::string& strDeviceNum, long lActiveCallRefId, long lHeldCallRefId, InvokeID_t invokeId);

		/* Telephony Supplementary Services */
		static bool SetMsgWaitingInd(ACSHandle_t acsHandle,
			InvokeID_t invokeId,
			const std::string& strDeviceNum, bool bOn);
		static bool SetDoNotDisturb(ACSHandle_t acsHandle,
			InvokeID_t invokeId,
			const std::string& strDeviceNum,
			bool bOn);
		static bool SetForwarding(ACSHandle_t acsHandle,
			InvokeID_t invokeId,
			const std::string& strDeviceNum,
			ForwardingType_t forwardingType,
			bool bOn,
			const std::string& strDest);
		bool SetAgentState(
			const std::string& strDeviceNum,
			const std::string& strGrp,
			const std::string& strAgent,
			const std::string& strPsw,
			int	iAgentMode,
			InvokeID_t invokeId);

		static bool QueryMsgWaitingInd(ACSHandle_t acsHandle,
			InvokeID_t invokeId,
			const std::string& strDeviceNum);
		static bool QueryDoNotDisturb(ACSHandle_t acsHandle,
			InvokeID_t invokeId,
			const std::string& strDeviceNum);
		static bool QueryForwarding(ACSHandle_t acsHandle,
			InvokeID_t invokeId,
			const std::string& strDeviceNum);
		static bool QueryAgentState(ACSHandle_t acsHandle,
			InvokeID_t invokeId,
			const std::string& strDeviceNum);
		bool QueryAgentState(const std::string& strDeviceNum, InvokeID_t invokeId);
		static bool QueryLastNumber(ACSHandle_t acsHandle,
			InvokeID_t invokeId,
			const std::string& strDeviceNum);
		bool QueryDeviceInfo(const std::string& strDeviceNum, InvokeID_t invokeId);
		static bool QueryDeviceInfo(ACSHandle_t acsHandle,
			InvokeID_t invokeId,
			const std::string& strDeviceNum);


		/* Monitor Services */
		bool MonitorDevice(const std::string& strDeviceNum, InvokeID_t invokeId);
		static bool MonitorCall(ACSHandle_t acsHandle,
			InvokeID_t invokeID,
			ConnectionID_t* call);
		bool MonitorCallsViaDevice(const std::string& strDeviceNum, InvokeID_t invokeId);
		static bool ChangeMonitorFilter(ACSHandle_t acsHandle,
			InvokeID_t invokeID,
			CSTAMonitorCrossRefID_t monitorCrossRefID,
			CSTAMonitorFilter_t *filterlist);
		static bool MonitorStop(ACSHandle_t acsHandle,
			InvokeID_t invokeId,
			CSTAMonitorCrossRefID_t monitorCrossRefID);

		/* Snapshot Services */
		static bool SnapshotCallReq(ACSHandle_t acsHandle,
			InvokeID_t invokeID,
			ConnectionID_t* pSnapshotObj,
			PrivateData_t* pPrivateData);
		static bool SnapshotDeviceReq(ACSHandle_t acsHandle,
			InvokeID_t invokeId,
			const std::string& strDeviceNum,
			PrivateData_t* pPrivateData);

		/* Routing Services */
		bool RouteRegisterReq(const std::string& strDeviceNum, InvokeID_t invokeId);
		static bool RouteRegisterReq(ACSHandle_t acsHandle,
			InvokeID_t invokeId,
			const std::string& strDeviceNum,
			PrivateData_t* pPrivateData);
		static bool RouteRegisterCancel(ACSHandle_t acsHandle,
			InvokeID_t invokeId,
			RouteRegisterReqID_t routeRegisterReqID,
			PrivateData_t* pPrivateData);
		/* Release 1 calls, w/o invokeID, for backward compatibility */
		static bool RouteSelect(ACSHandle_t acsHandle,
			RouteRegisterReqID_t routeRegisterReqID,
			RoutingCrossRefID_t	routingCrossRefID,
			const std::string& strRouteSelected,
			RetryValue_t	remainRetry,
			SetUpValues_t*	pSetupInformation,
			bool bRouteUsedReq,
			PrivateData_t* pPrivateData);
		static bool RouteEnd(ACSHandle_t acsHandle,
			RouteRegisterReqID_t routeRegisterReqID,
			RoutingCrossRefID_t routingCrossRefID,
			CSTAUniversalFailure_t errorValue,
			PrivateData_t* pPrivateData);
		/* Release 2 calls, with invokeID */
		static bool RouteSelectInv(ACSHandle_t acsHandle,
			InvokeID_t invokeId,
			RouteRegisterReqID_t routeRegisterReqID,
			RoutingCrossRefID_t	routingCrossRefID,
			const std::string& strRouteSelected,
			RetryValue_t remainRetry,
			SetUpValues_t* setupInformation,
			Boolean	routeUsedReq,
			PrivateData_t* pPrivateData);
		bool RouteSelectInv(
			RouteRegisterReqID_t routeRegisterReqID,
			RoutingCrossRefID_t	routingCrossRefID,
			const std::string& strRouteSelected,
			InvokeID_t invokeId);
		static bool RouteEndInv(ACSHandle_t acsHandle,
			InvokeID_t invokeId,
			RouteRegisterReqID_t routeRegisterReqID,
			RoutingCrossRefID_t routingCrossRefID,
			CSTAUniversalFailure_t errorValue,
			PrivateData_t* pPrivateData);
		bool RouteEndInv(
			RouteRegisterReqID_t routeRegisterReqID,
			RoutingCrossRefID_t routingCrossRefID,
			InvokeID_t invokeId);

		/* Escape Services */
		int EscapeService(InvokeID_t invokeID);
		static bool EscapeServiceConf(ACSHandle_t acsHandle,
			InvokeID_t invokeID,
			CSTAUniversalFailure_t error,
			PrivateData_t* pPrivateData);
		static bool SendPrivateEvent(ACSHandle_t acsHandle,
			PrivateData_t* pPrivateData);

		/* Maintenance Services */
		static bool SysStatReq(ACSHandle_t acsHandle,
			InvokeID_t invokeID,
			PrivateData_t* pPrivateData);
		static bool SysStatStart(ACSHandle_t acsHandle,
			InvokeID_t invokeID,
			SystemStatusFilter_t statusFilter,
			PrivateData_t* pPrivateData);
		static bool SysStatStop(ACSHandle_t acsHandle,
			InvokeID_t invokeID,
			PrivateData_t* pPrivateData);

		static bool GetAPICaps(ACSHandle_t acsHandle,
			InvokeID_t invokeID);
		static bool GetDeviceList(ACSHandle_t acsHandle,
			InvokeID_t	invokeID,
			long		index,
			CSTALevel_t	level);
		static bool QueryCallMonitor(ACSHandle_t acsHandle,
			InvokeID_t invokeID);

		/********* ATT Private Data Encoding Functions	*******/
		int SingleStepConferenceCall(const std::string& strDeviceNum, const std::string& strToBeJoinNum,
			long lCallRefId, int iMode);

		bool ListenCall(const std::string& strSponsor, const std::string& strTarget,
			long lCallRefId, InvokeID_t invokeId);
		bool BargeInCall(const std::string& strSponsor, const std::string& strTarget,
			long lCallRefId, InvokeID_t invokeId);
		bool ForcePopCall(const std::string& strTarget,long lCallRefId, InvokeID_t invokeId);

		private:

#if defined(WIN32) || defined(WIN64)
			HWND		m_hWnd;
			HINSTANCE	m_hInstance;
			static TCHAR m_szAppName[MAX_BUFFER];
#endif
			static int	m_nEventIsPending;
			bool		m_bStopOpenStream;
			bool		m_bOpeningStream;

			Log::ILogPtr							m_pLog;
			static boost::shared_ptr<CAvayaSwitch>	m_pInstance;
			std::list<std::string>					m_tsapiServerNameQueue;

			ACSHandle_t			m_acsHandle;
			CSTAEvent_t			m_cstaEvent;
			unsigned short		m_usEventSize;
			ATTPrivateData_t	m_privateData;

			bool m_bConnectSlaveAvayaFlag;
	};

}// end namespace
