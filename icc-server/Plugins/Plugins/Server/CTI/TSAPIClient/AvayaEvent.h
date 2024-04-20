#pragma once

/**
 @Copyright Copyright (C), 2018
 @file		AvayaEvent.h
 @created	2018/1/1
 @brief		�������¼�������
 ��������������Խ��������¼�,�����¼����ͽ���,������Ӧ������
 ������Avaya��������ƥ��,��AvayaSwitch�����ʹ��

 @author	psy
 */

namespace ICC
{
	struct sDeviceInfo
	{
		std::string strCstaCallId;
		std::string strDevice;
		std::string strCovertNumber;
	};

	typedef std::vector<sDeviceInfo> DeviceList;

	class CAvayaEvent
	{
	public:
		CAvayaEvent(void);
		virtual ~CAvayaEvent(void);

		static boost::shared_ptr<CAvayaEvent> Instance();
		void ExitInstance();

		void Init(IResourceManagerPtr pResourceManager);

	private:
		static boost::shared_ptr<CAvayaEvent> m_pEventInstance;

		Log::ILogPtr				m_pLog;
		StringUtil::IStringUtilPtr	m_pStrUtil;
		DateTime::IDateTimePtr		m_pDateTimePtr;

		std::map<std::string, DeviceList> m_mapResonanceGroupsInfo;

	private:
		

	public:
		// DispatchEvent
		void DispatchEvent(CSTAEvent_t* pEventData, ATTEvent_t* pATTEvent);

		//ACS֤ʵ����Ϣ
		void HandleACSConfirmation(CSTAEvent_t* pEventData);
		//ACS�����㱨����Ϣ
		void HandACSUnsolicited(CSTAEvent_t* pEventData);
		//CSTA������Ϣ
		void RecCSTARequestEvent(CSTAEvent_t* pEventData);
		//CSTA֤ʵ��Ϣ
		void HandleCSTAConfirmation(CSTAEvent_t* pEventData, ATTEvent_t* pATTEvent);
		//�յ�CSTA�������͵���Ϣ
		void HandleCSTAUnsolicited(CSTAEvent_t* pEventData, ATTEvent_t* pATTEvent);
		//�յ�CSTA�㱨��Ϣ
		void RecCSTAReportEvent(CSTAEvent_t* pEventData);

		void OpenStreamConf(CSTAEvent_t* pEventData);
		void OpenStreamFail(ACSUniversalFailure_t error);
		void CloseStreamConf();
		void StreamException();

		// RecCSTAUnsoliEvent
		// ���塢���塢���롢����
		void DeliveredEvent(long lMonitorId, CSTADeliveredEvent_t& delivered);
		// ���������γ�ͨ����
		void EstablishedEvent(long lMonitorId, CSTAEstablishedEvent_t& established);
		// �Ҷ��¼�
		void ConnectClearedEvent(long lMonitorId, CSTAConnectionClearedEvent_t& connectionCleared);
		// ժ���¼�
		void ServiceInitiatedEvent(long lMonitorId, CSTAServiceInitiatedEvent_t& serviceInitiated);
		void OriginatedEvent(long lMonitorId, CSTAOriginatedEvent_t& originated);
		//	�����¼�
		void CallClearedEvent(long lMonitorId, CSTACallClearedEvent_t& callCleared);
		// �Ŷ��¼�
		void QueuedEvent(long lMonitorId, CSTAQueuedEvent_t& queued);
		// �����¼�
		void ConferencedEvent(long lMonitorId, CSTAConferencedEvent_t& conferenced);
		// ƫת�����𡢳�ʱת���¼�
		void DivertedEvent(long lMonitorId, CSTADivertedEvent_t& diverted);
		//	ʧ���¼�
		void FailedEvent(long lMonitorId, CSTAFailedEvent_t& failed);
		//	�����¼�
		void HeldEvent(long lMonitorId, CSTAHeldEvent_t& held);
		//	ת���¼�
		void TransferredEvent(long lMonitorId, CSTATransferredEvent_t& transferred);
		//	��ʰ�¼�
		void RetrievedEvent(long lMonitorId, CSTARetrievedEvent_t& retrieved);
		void MonitorEndEvent(long lMonitorId, CSTAMonitorEndedEvent_t& monitorEnded);
		//	����·��
		void RouteRequestExtEvent(long lnvokeID, CSTARouteRequestExtEvent_t& routeRequestExtEvent);
		// not use
		void NetworkReachedEvent(long lMonitorId, CSTANetworkReachedEvent_t& networkReached);
		void CallInformationEvent(long lMonitorId, CSTACallInformationEvent_t& callInformation);
		void DoNotDisturbEvent(long lMonitorId, CSTADoNotDisturbEvent_t& doNotDisturb);
		void ForwardingEvent(long lMonitorId, CSTAForwardingEvent_t&	forwarding);
		void MessageWaitingEvent(long lMonitorId, CSTAMessageWaitingEvent_t& messageWaiting);
		void LoggedOnEvent(long lMonitorId, CSTALoggedOnEvent_t& loggedOn);
		void LoggedOffEvent(long lMonitorId, CSTALoggedOffEvent_t& loggedOff);
		void NotReadyEvent(long lMonitorId, CSTANotReadyEvent_t& notReady);
		void ReadyEvent(long lMonitorId, CSTAReadyEvent_t& ready);
		void WorkNotReadyEvent(long lMonitorId, CSTAWorkNotReadyEvent_t& workNotReady);
		void WorkReadyEvent(long lMonitorId, CSTAWorkReadyEvent_t& workReady);
		void BackInServiceEvent(long lMonitorId, CSTABackInServiceEvent_t& backInService);
		void OutOfServiceEvent(long lMonitorId, CSTAOutOfServiceEvent_t& outOfService);
		void PrivateStatusEvent(long lMonitorId, CSTAPrivateStatusEvent_t& privateStatus);

		// HandleCSTAConfirmation
		void UniversalFailRet(long invokeID, CSTAUniversalFailureConfEvent_t& universalFailure);
		void QueryDeviceInfoConf(long invokeID, CSTAQueryDeviceInfoConfEvent_t& queryDeviceInfo);
		void SysStatReqConf(long invokeID, CSTASysStatReqConfEvent_t& SysStatReq);
		void QueryAgentStateConf(long invokeID, CSTAQueryAgentStateConfEvent_t& queryAgentState);
		void SetAgentStateConf(long invokeID, CSTASetAgentStateConfEvent_t& setAgentState);
		void MonitorDeviceConf(long invokeID, CSTAMonitorConfEvent_t& monitorStart);
		void MakeCallConf(long invokeID, CSTAMakeCallConfEvent_t& makeCall);
		void AnswerCallConf(long invokeID, CSTAAnswerCallConfEvent_t& answerCall);
		void ClearCallConf(long invokeID, CSTAClearCallConfEvent_t& clearCall);
		void ClearConnectionConf(long invokeID, CSTAClearConnectionConfEvent_t& clearConnection);
		void DeflectCallConf(long invokeID, CSTADeflectCallConfEvent_t& deflectCall);
		void PickupCallConf(long invokeID, CSTAPickupCallConfEvent_t& pickupCall);
		void EscapeServiceConf(long invokeID, CSTAEscapeSvcConfEvent_t& escapeService, ATTEvent_t* pATTEvent);
		void ConsultationConf(long invokeID, CSTAConsultationCallConfEvent_t& consultationCall);
		void TransferCallConf(long invokeID, CSTATransferCallConfEvent_t& transferCall);
		void HoldCallConf(long invokeID, CSTAHoldCallConfEvent_t& holdCall);
		void RetrieveCallConf(long invokeID, CSTARetrieveCallConfEvent_t& retrieveCall);
		void ConferenceConf(long invokeID, CSTAConferenceCallConfEvent_t& conferenceCall);
		void SingleStepConferenceConf(long invokeID, ATTSingleStepConferenceCallConfEvent_t* ssconference);
		void RouteRegisterReqConf(long invokeID, CSTARouteRegisterReqConfEvent_t& routeRegister);
		void AlternateCallConf(long invokeID, CSTAAlternateCallConfEvent_t& alternateCall);
		// not use
		void CallCompletionRet(long invokeID, CSTACallCompletionConfEvent_t& callCompletion);
		void GroupPickupCallRet(long invokeID, CSTAGroupPickupCallConfEvent_t& groupPickupCall);
		void MakePredictiveCallRet(long invokeID, CSTAMakePredictiveCallConfEvent_t& makePredictiveCall);
		void QueryMwiRet(long invokeID, CSTAQueryMwiConfEvent_t& queryMwi);
		void QueryDndRet(long invokeID, CSTAQueryDndConfEvent_t& queryDnd);
		void QueryFwdRet(long invokeID, CSTAQueryFwdConfEvent_t& queryFwd);
		void QueryLastNumberRet(long invokeID, CSTAQueryLastNumberConfEvent_t& queryLastNumber);
		void ReconnectCallRet(long invokeID, CSTAReconnectCallConfEvent_t& reconnectCall);
		void SetMwiRet(long invokeID, CSTASetMwiConfEvent_t& setMwi);
		void SetDndRet(long invokeID, CSTASetDndConfEvent_t& setDnd);
		void SetFwdRet(long invokeID, CSTASetFwdConfEvent_t& setFwd);
		void RouteRegisterCancelRet(long invokeID, CSTARouteRegisterCancelConfEvent_t& routeCancel);
		void SysStatStartRet(long invokeID, CSTASysStatStartConfEvent_t& sysStatStart);
		void SysStatStopRet(long invokeID, CSTASysStatStopConfEvent_t& sysStatStop);
		void ChangeSysStatFilterRet(long invokeID, CSTAChangeSysStatFilterConfEvent_t& changeSysStatFilter);
		void ChangeMonitorFilterRet(long invokeID, CSTAChangeMonitorFilterConfEvent_t& changeMonitorFilter);
		void MonitorStopRet(long invokeID, CSTAMonitorStopConfEvent_t& monitorStop);
		void SnapshotCallRet(long invokeID, CSTASnapshotCallConfEvent_t& snapshotCall);
		void SnapshotDeviceRet(long invokeID, CSTASnapshotDeviceConfEvent_t& snapshotDevice);
		void GetAPICapsRet(long invokeID, CSTAGetAPICapsConfEvent_t& getAPICaps);
		void GetDeviceListRet(long invokeID, CSTAGetDeviceListConfEvent_t& getDeviceList);
		void QueryCallMonitorRet(long invokeID, CSTAQueryCallMonitorConfEvent_t& queryCallMonitor);

		// ժ��
		void DeviceOffHook(long lCSTACallRefId, const std::string& strOffHookDevice);
		// ����
		void CallIncoming(long lCSTACallRefId, const std::string& strCallerID, const std::string& strCalledID, const std::string& strAlteringDevice);
		//	�Ŷ�
		void CallWaiting(long lCSTACallRefId, const std::string& strCallerID, const std::string& strCalledID, const std::string& strAlteringDevice);
		//	����
		void CallAssign(long lCSTACallRefId, const std::string& strCallerID, const std::string& strCalledID, const std::string& strAlteringDevice);
		//	����
		void CallAlerting(long lCSTACallRefId, const std::string& strCallerID, const std::string& strCalledID, const std::string& strAlteringDevice);
		//	����
		void CallAlteringBack(long lCSTACallRefId, const std::string& strCallerID, const std::string& strCalledID, const std::string& strAlteringDevice);
		//	ͨ��
		void CallTalking(long p_lCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID, const std::string& p_strTalkingDevice);
		//	����
		void CallConference(long lNewCSTACallRefId, long lOldCSTACallRefId, const std::string& strMonitorDevice, const std::string& strConferenceDevice);
		//	����
		void CallSilentMonitor(long lCSTACallRefId, const std::string& strMonitorDevice);
		//	����
		void CallHold(long lCSTACallRefId, const std::string& strHoldDevice);
		//	��ʰ
		void CallRetrieve(long lCSTACallRefId, const std::string& strRetrieveDevice);
		//	�һ�
		void DeviceHangup(long lCSTACallRefId, const std::string& strHangupDevice, const std::string& strMonitorDevice = "", const std::string& strHangupType = "");
		//	�����Ա�һ�
		void ConferenceHangup(long lCSTACallRefId, const std::string& strCTICallRefId, const std::string& strHangupDevice);

		void ForcePopCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor);
		void ClearCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);
		void CheckCallOver(long lCSTACallRefId, bool bRemoveCall = false);

		void PostDeviceState(long p_lCSTACallRefId, const std::string& p_strCTICallRefId, const std::string& p_strDeviceNum);
		void PostSwitchConf(long invokeID, E_TASK_NAME nTaskName, bool bResullt = true, int nErrorCode = 0, const std::string& strErrorMsg = "");
	};// end class CAvayaEvent

}// end namespace ICC

