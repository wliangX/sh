#pragma once

/**
 @Copyright Copyright (C), 2018
 @file		AvayaEvent.h
 @created	2018/1/1
 @brief		交换机事件接收类
 负责接收所有来自交换机的事件,根据事件类型解析,交给相应处理类
 该类与Avaya交换机相匹配,与AvayaSwitch类配合使用

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

		//	Agent 状态改变事件
		void AgentStateEvent(const std::string& p_strDeviceNum, DWORD p_dwDeviceState);
		// 振铃、回铃、呼入、分配
		//void DeliveredEvent(HLINE p_hLine, HCALL p_hCall, DWORD p_dwCallState, long p_lCSTACallRefId, long p_lRelatedCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID);
		bool DeliveredEvent(HLINE p_hLine, HCALL p_hCall, DWORD p_dwCallState, long p_lCSTACallRefId, long p_lRelatedCSTACallRefId, bool p_bIsTransferCall, const std::string& p_strCallerID, const std::string& p_strCalledID);
		// 话务建立（形成通话）
		//void EstablishedEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID);
		void EstablishedEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId, long p_lRelatedCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID);
		
		void EstablishedTransferEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID);
		void HoldTransferEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId, long p_lRelatedCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID);
		void ConferenceConnectEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId, long p_lRelatedCSTACallRefId);
		//	摘机
		void ServiceInitiatedEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId, long p_lRelatedCSTACallRefId, const std::string& p_strOffHookDevice);
		//	保留事件
		void HeldEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId); 
		//	会议事件
		void ConferencedEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId, const std::string& p_strConnectedId);
		//	转移事件
		void TransferredEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId, long p_lRelatedCSTACallRefId);
		// 挂断事件
		void ConnectClearedEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId);

		// 呼入
		void CallIncoming(long p_lCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID, const std::string& p_strAlteringDevice);
		//	排队
		void CallWaiting(long p_lCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID, const std::string& p_strAlteringDevice);
		//	分配
		void CallAssign(long p_lCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID, const std::string& p_strAlteringDevice);
		// 摘机
		void DeviceOffHook(long p_lCSTACallRefId, long p_lRelatedCSTACallRefId, const std::string& p_strOffHookDevice);
		//	振铃
		void CallAlerting(long p_lCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID, const std::string& p_strAlteringDevice);
		//	回铃
		void CallAlteringBack(long p_lCSTACallRefId, long p_lRelatedCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID, const std::string& p_strAlteringDevice);
		//	通话
		void CallTalking(long p_lCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID, const std::string& p_strTalkingDevice);
		//	会议
		void CallConference(long p_lCSTACallRefId, const std::string& p_strConferenceDevice);
		//	监听
		void CallSilentMonitor(long p_lCSTACallRefId, const std::string& p_strMonitorDevice);
		//	保留
		void CallHold(long p_lCSTACallRefId, const std::string& p_strHoldDevice);
		void OnHoldPendConf(long p_lCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID);
		//	重拾
		void CallRetrieve(long p_lCSTACallRefId, const std::string& p_strRetrieveDevice);
		//	挂机
		void DeviceHangup(long p_lCSTACallRefId, const std::string& p_strHangupDevice, const std::string& p_strMonitorDevice = "", const std::string& p_strHangupType = "");
		//	空闲
		void DeviceFree(long p_lCSTACallRefId, const std::string& p_strFreeDevice);
		//	会议成员挂机
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

