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
	class CAvayaEvent
	{
	public:
		CAvayaEvent(void);
		virtual ~CAvayaEvent(void);

		static boost::shared_ptr<CAvayaEvent> Instance();
		void ExitInstance();

		void OnInit();

		void SetLogPtr(Log::ILogPtr p_pLog){ m_pLog = p_pLog; }
		void SetDateTimePtr(DateTime::IDateTimePtr p_pDateTime){ m_pDateTime = p_pDateTime; }
		void SetStringUtilPtr(StringUtil::IStringUtilPtr p_pStringUtil){ m_pStrUtil = p_pStringUtil; }

	private:
		static boost::shared_ptr<CAvayaEvent> m_pEventInstance;

		Log::ILogPtr				m_pLog;
		StringUtil::IStringUtilPtr	m_pStrUtil;
		DateTime::IDateTimePtr		m_pDateTime;

	public:
		void OnEvent(DWORD p_dwDevice, DWORD p_dwMsg, DWORD p_dwParam1, DWORD p_dwParam2, DWORD p_dwParam3);

		void LineReply(LONG p_lExcutedId, HRESULT p_hResult, DWORD p_dwParam);
		void LineClose(HLINE p_hLine);
		void LineAddressState(HLINE p_hLine, DWORD p_dwAddressID, DWORD p_dwAddressState);
		void LineCallInfo(HCALL p_hCall, DWORD p_dwCallInfoState);
//		bool LineGetCallInfo(HCALL p_hCall, DWORD p_dwCallInfoState, LINECALLINFO*& p_pCallInfo);
		void LineCallState(HCALL p_hCall, DWORD p_dwCallState, DWORD p_dwCallStateDetail, DWORD p_dwCallPrivilege);
		void LineNewCall(HLINE p_hLine, DWORD p_dwAddressID, HCALL p_hCall, DWORD p_dwCallPrivilege);
		void LineDevState(HLINE p_hLine, DWORD p_dwDeviceState, DWORD p_dwDeviceStateDetail1, DWORD p_dwDeviceStateDetail2);

		//	Agent ״̬�ı��¼�
		void AgentStateEvent(const std::string& p_strDeviceNum, DWORD p_dwDeviceState);
		// ���塢���塢���롢����
		//void DeliveredEvent(HLINE p_hLine, HCALL p_hCall, DWORD p_dwCallState, long p_lCSTACallRefId, long p_lRelatedCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID);
		bool DeliveredEvent(HLINE p_hLine, HCALL p_hCall, DWORD p_dwCallState, long p_lCSTACallRefId, long p_lRelatedCSTACallRefId, bool p_bIsTransferCall, const std::string& p_strCallerID, const std::string& p_strCalledID);
		// ���������γ�ͨ����
		//void EstablishedEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID);
		void EstablishedEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId, long p_lRelatedCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID);
		
		void EstablishedTransferEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID);
		void HoldTransferEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId, long p_lRelatedCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID);
		void ConferenceConnectEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId, long p_lRelatedCSTACallRefId);
		//	ժ��
		void ServiceInitiatedEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId, long p_lRelatedCSTACallRefId, const std::string& p_strOffHookDevice);
		//	�����¼�
		void HeldEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId); 
		//	�����¼�
		void ConferencedEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId, const std::string& p_strConnectedId);
		//	ת���¼�
		void TransferredEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId, long p_lRelatedCSTACallRefId);
		// �Ҷ��¼�
		void ConnectClearedEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId);

		// ����
		void CallIncoming(long p_lCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID, const std::string& p_strAlteringDevice);
		//	�Ŷ�
		void CallWaiting(long p_lCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID, const std::string& p_strAlteringDevice);
		//	����
		void CallAssign(long p_lCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID, const std::string& p_strAlteringDevice);
		// ժ��
		void DeviceOffHook(long p_lCSTACallRefId, long p_lRelatedCSTACallRefId, const std::string& p_strOffHookDevice);
		//	����
		void CallAlerting(long p_lCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID, const std::string& p_strAlteringDevice);
		//	����
		void CallAlteringBack(long p_lCSTACallRefId, long p_lRelatedCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID, const std::string& p_strAlteringDevice);
		//	ͨ��
		void CallTalking(long p_lCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID, const std::string& p_strTalkingDevice);
		//	����
		void CallConference(long p_lCSTACallRefId, const std::string& p_strConferenceDevice);
		//	����
		void CallSilentMonitor(long p_lCSTACallRefId, const std::string& p_strMonitorDevice);
		//	����
		void CallHold(long p_lCSTACallRefId, const std::string& p_strHoldDevice);
		void OnHoldPendConf(long p_lCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID);
		//	��ʰ
		void CallRetrieve(long p_lCSTACallRefId, const std::string& p_strRetrieveDevice);
		//	�һ�
		void DeviceHangup(long p_lCSTACallRefId, const std::string& p_strHangupDevice, const std::string& p_strMonitorDevice = "", const std::string& p_strHangupType = "");
		//	����
		void DeviceFree(long p_lCSTACallRefId, const std::string& p_strFreeDevice);
		//	�����Ա�һ�
		void ConferenceHangup(long p_lCSTACallRefId, const std::string& p_strCTICallRefId, const std::string& p_strHangupDevice);

		void ForcePopCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor);
	//	void CheckCallOver(long p_lCSTACallRefId, const std::string& p_strCTICallRefId);
		void CheckCallOver(long p_lCSTACallRefId);

		void SetAgentStateConf(long p_lRequestId, bool p_bResullt = true, int p_nErrorCode = 0, const std::string& p_strErrorMsg = "");
		void MakeCallConf(long p_lRequestId, bool p_bResullt, HCALL p_hCall, int p_nErrorCode = 0, const std::string& p_strErrorMsg = "");
		void ConsultationCallConf(long p_lRequestId, bool p_bResullt, HCALL p_hCall, int p_nErrorCode = 0, const std::string& p_strErrorMsg = "");
		void TransferCallConf(long p_lRequestId, bool p_bResullt = true, int p_nErrorCode = 0, const std::string& p_strErrorMsg = "");

		void PostDeviceState(long p_lCSTACallRefId, const std::string& p_strCTICallRefId, const std::string& p_strDeviceNum, const std::string& p_strState);
		void PostDeviceStateEx(long p_lCSTACallRefId, long p_lCSTARelatedCallRefId, const std::string& p_strDeviceNum, const std::string& p_strState);
		void PostSwitchConf(long p_lInvokeID, E_TASK_NAME p_nTaskName, bool p_bResullt = true, int p_nErrorCode = 0, const std::string& p_strErrorMsg = "");
	};// end class CAvayaEvent

}// end namespace ICC

