/**
 @Copyright Copyright (C), 2018
 @file		AvayaEvent.cpp
 @created	2018/01/18
 @brief		�������¼�������
 ��������������Խ��������¼�,�����¼����ͽ���,������Ӧ������
 ������Avaya��������ƥ��,��AvayaSwitch�����ʹ��

 @author	psy
 */

#include "Boost.h"

#include "AvayaEvent.h"
#include "AvayaResult.h"
#include "SysConfig.h"
#include "AgentManager.h"
#include "Device.h"
#include "DeviceManager.h"
#include "TaskManager.h"
#include "Property.h"
#include "Call.h"
#include "CallManager.h"
#include "BlackListManager.h"
#include "TransferCallManager.h"
#include "ConferenceManager.h"

boost::shared_ptr<CAvayaEvent> CAvayaEvent::m_pEventInstance = nullptr;
CAvayaEvent::CAvayaEvent(void)
{
	m_pLog = nullptr;
	m_pStrUtil = nullptr;
}

CAvayaEvent::~CAvayaEvent(void)
{
	//
}

boost::shared_ptr<CAvayaEvent> CAvayaEvent::Instance()
{
	if (m_pEventInstance == nullptr)
	{
		m_pEventInstance = boost::make_shared<CAvayaEvent>();
	}

	return m_pEventInstance;
}
void CAvayaEvent::ExitInstance()
{
	//
}

void CAvayaEvent::OnInit()
{
	//
}

//////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------
// Handle messages from TAPI
void CAvayaEvent::OnEvent(DWORD p_dwDevice, DWORD p_dwMsg, DWORD p_dwParam1, DWORD p_dwParam2, DWORD p_dwParam3)
{
	ICC_LOG_DEBUG(m_pLog, "TAPI Event, Device: [%u], Msg: [%u], Param1: [%u], Param2: [%u], Param3: [%u]", 
		p_dwDevice, p_dwMsg, p_dwParam1, p_dwParam2, p_dwParam3);
	
	switch (p_dwMsg)
	{
	case LINE_REPLY: // Reply to an asynchronous TAPI function
		this->LineReply(p_dwParam1, p_dwParam2, p_dwParam3);
		break;

	case LINE_CLOSE: // A line has closed
		this->LineClose((HLINE)p_dwDevice);
		break;

	case LINE_ADDRESSSTATE: // The address state has changed
		this->LineAddressState((HLINE)p_dwDevice, p_dwParam1, p_dwParam2);
		break;

	case LINE_CALLINFO: // The call info has changed
		this->LineCallInfo((HCALL)p_dwDevice, p_dwParam1);
		break;

	case LINE_CALLSTATE: // The call state has changed
		this->LineCallState((HCALL)p_dwDevice, p_dwParam1, p_dwParam2, p_dwParam3);
		break;

	case LINE_LINEDEVSTATE: // The device state has changed
		this->LineDevState((HLINE)p_dwDevice, p_dwParam1, p_dwParam2, p_dwParam3);
		break;

	case LINE_APPNEWCALL: // There is a new incoming call
		this->LineNewCall((HLINE)p_dwDevice, p_dwParam1, (HCALL)p_dwParam2, p_dwParam3);
		// The following demonstrates screen pop!!!
		break;

	case LINE_DEVSPECIFIC: // The device specific data has changed
	case LINE_DEVSPECIFICFEATURE: // The device specific features have changed
		ICC_LOG_DEBUG(m_pLog, "Device Specific Event");
		break;

	case LINE_GATHERDIGITS: // A digit has been received
		ICC_LOG_DEBUG(m_pLog, "Gather Digits Event");
		break;

	case LINE_GENERATE: // A tone has been generated
		ICC_LOG_DEBUG(m_pLog, "Generate Event");
		break;

	case LINE_MONITORDIGITS: // A digit has been received
		ICC_LOG_DEBUG(m_pLog, "Monitor Digits Event");
		break;

	case LINE_MONITORMEDIA:
		ICC_LOG_DEBUG(m_pLog, "Monitor Media Event");
		break;

	case LINE_MONITORTONE: // A tone has been detected
		ICC_LOG_DEBUG(m_pLog, "Monitor Tone Event");
		break;

	case LINE_REQUEST:
		ICC_LOG_DEBUG(m_pLog, "Assisted Telephony Request Event");
		break;

	case PHONE_BUTTON: // Phone events are not supported
	case PHONE_CLOSE:
	case PHONE_DEVSPECIFIC:
	case PHONE_REPLY:
	case PHONE_STATE:
	case PHONE_CREATE:
	case PHONE_REMOVE:
		ICC_LOG_DEBUG(m_pLog, "Phone event: [%u] are not supported", p_dwMsg);
		break;

	case LINE_CREATE: // Not supported
		ICC_LOG_DEBUG(m_pLog, "Line Create Event: [%u] not supported", p_dwMsg);
		break;

	case LINE_AGENTSPECIFIC: // Not Supported
	case LINE_AGENTSTATUS:  // Not supported
		ICC_LOG_DEBUG(m_pLog, "Agent Event: [%u] not supported", p_dwMsg);
		break;

	case LINE_PROXYREQUEST: // Not supported
		ICC_LOG_DEBUG(m_pLog, "Proxy Request Event: [%u] not supported", p_dwMsg);
		break;

	case LINE_REMOVE: // Not supported
		ICC_LOG_DEBUG(m_pLog, "Line Remove Event: [%u] not supported", p_dwMsg);
		break;

	default:
		break;
	}
}

void CAvayaEvent::LineReply(LONG p_lInvokeId, HRESULT p_hResult, DWORD p_dwParam)
{
	ICC_LOG_DEBUG(m_pLog, "Line Reply, ExcutedId: [%u], Result: [%s], Param: [%u]", 
		p_lInvokeId, p_hResult == 0 ? "Success" : "Failed", p_dwParam);

	//	֪ͨ�����������
	bool l_bResult = p_hResult == 0 ? true : false;
	long l_lRequestId = CTaskManager::Instance()->GetTaskIdByInvokeId(p_lInvokeId);
	E_TASK_NAME l_nTaskName = CTaskManager::Instance()->GetTaskNameByInvokeId(p_lInvokeId);
	switch (l_nTaskName)
	{
	case Task_AgentLogin:
		this->SetAgentStateConf(l_lRequestId, l_bResult);
		break;
	case Task_AgentLogout:
		this->SetAgentStateConf(l_lRequestId, l_bResult);
		break;
	case Task_SetAgentState:
		this->SetAgentStateConf(l_lRequestId, l_bResult);
		break;
	case Task_MakeCall:
		this->MakeCallConf(l_lRequestId, l_bResult, p_dwParam);
		break;
	case  Task_AnswerCall:
		this->PostSwitchConf(l_lRequestId, Task_AnswerCallConf, l_bResult);
		break;
	case  Task_Hangup:
		this->PostSwitchConf(l_lRequestId, Task_ClearConnectionConf, l_bResult);
		break;
	case  Task_RefuseAnswer:
		this->PostSwitchConf(l_lRequestId, Task_ClearCallConf, l_bResult);
		break;
	case Task_RefuseBlackCall:
		this->PostSwitchConf(l_lRequestId, Task_ClearCallConf, l_bResult);
		break;
	case  Task_ClearCall:
		this->PostSwitchConf(l_lRequestId, Task_ClearCallConf, l_bResult);
		break;
	case  Task_ForcePopCall:
		this->PostSwitchConf(l_lRequestId, Task_ClearConnectionConf, l_bResult);
		break;
	case  Task_BargeInCall:
		this->PostSwitchConf(l_lRequestId, Task_SingleStepConferenceConf, l_bResult);
		break; 
	case  Task_ListenCall:
		this->PostSwitchConf(l_lRequestId, Task_SingleStepConferenceConf, l_bResult);
		break;
	case  Task_HoldCall:
		this->PostSwitchConf(l_lRequestId, Task_HoldCallConf, l_bResult);
		break;
	case  Task_RetrieveCall:
		this->PostSwitchConf(l_lRequestId, Task_RetrieveCallConf, l_bResult);
		break;
	case  Task_ConsultationCall:
		this->ConsultationCallConf(l_lRequestId, l_bResult, p_dwParam);
		break;
	case  Task_ConferenceCall:
		this->PostSwitchConf(l_lRequestId, Task_ConferenceCallConf, l_bResult);
		break;
	case  Task_PickupCall:
		this->PostSwitchConf(l_lRequestId, Task_PickupCallConf, l_bResult);
		break;
	case  Task_DeflectCall:
		this->PostSwitchConf(l_lRequestId, Task_DeflectCallConf, l_bResult);
		break;
	case  Task_TransferCall:
		this->TransferCallConf(l_lRequestId, l_bResult);
		break;
	default:
		break;
	}
}
void CAvayaEvent::LineClose(HLINE p_hLine)
{
	ICC_LOG_DEBUG(m_pLog, "Line: [%u] Close", p_hLine);
}
void CAvayaEvent::LineAddressState(HLINE p_hLine, DWORD p_dwAddressID, DWORD p_dwAddressState)
{
	std::string l_strState;
	std::string l_strInfo = m_pStrUtil->Format("Line Address State Event, Line: [%u], AddressId: [%u], AddressState: [%s]",
		p_hLine, p_dwAddressID, CAvayaResult::Instance()->DescribeAddressStatus(l_strState, p_dwAddressState).c_str());
	ICC_LOG_DEBUG(m_pLog, "%s", l_strInfo.c_str());
	ShowMsg(l_strInfo);

	// Get further information about the address change
	LINEADDRESSSTATUS l_pAddressStatus = { sizeof(LINEADDRESSSTATUS) };
	HRESULT l_hResult = ::lineGetAddressStatus(p_hLine, p_dwAddressID, &l_pAddressStatus);
	l_strInfo = m_pStrUtil->Format("Get Address Status, Line: [%u], NumInUse: [%u], NumActiveCalls: [%u], NumOnHoldCalls: [%u], NumOnHoldPendCalls: [%u]",
		p_hLine,
		l_pAddressStatus.dwNumInUse, l_pAddressStatus.dwNumActiveCalls,
		l_pAddressStatus.dwNumOnHoldCalls, l_pAddressStatus.dwNumOnHoldPendCalls);
	ICC_LOG_DEBUG(m_pLog, "%s", l_strInfo.c_str());
	ShowMsg(l_strInfo);
}
void CAvayaEvent::LineCallInfo(HCALL p_hCall, DWORD p_dwCallInfoState)
{
	std::string l_strInfo = m_pStrUtil->Format("Line Call Info Event, Call: [%u], CallInfoState: [%u]", p_hCall, p_dwCallInfoState);
	ICC_LOG_DEBUG(m_pLog, "%s", l_strInfo.c_str());
	ShowMsg(l_strInfo);

	// Get further information about the call info
	std::string l_strCallOrigin = "";
	std::string l_strCallReason = "";
	std::string l_strTrunk = "";
	std::string l_strCallerID = "";
	std::string l_strCalledID = "";
	std::string l_strConnectedID = "";
	std::string l_strRedirectingID = "";
	std::string l_strRedirectionID = "";
	std::string l_strCallData = "";

	LINECALLINFO* l_pCallInfo = NULL;
	CDeviceManager::Instance()->LoopLineGetCallInfo(p_hCall, l_pCallInfo);
	if (l_pCallInfo)
	{
		if (l_pCallInfo->dwTrunk == 0xffffffff)
			l_strTrunk = "Unknown";
		else
			l_strTrunk = std::to_string(l_pCallInfo->dwTrunk);

		CDeviceManager::Instance()->GetTapiString(l_strCallerID, l_pCallInfo, l_pCallInfo->dwCallerIDSize, l_pCallInfo->dwCallerIDOffset);
		CDeviceManager::Instance()->GetTapiString(l_strCalledID, l_pCallInfo, l_pCallInfo->dwCalledIDSize, l_pCallInfo->dwCalledIDOffset);
		CDeviceManager::Instance()->GetTapiString(l_strConnectedID, l_pCallInfo, l_pCallInfo->dwConnectedIDSize, l_pCallInfo->dwConnectedIDOffset);
		CDeviceManager::Instance()->GetTapiString(l_strRedirectingID, l_pCallInfo, l_pCallInfo->dwRedirectingIDSize, l_pCallInfo->dwRedirectingIDOffset);
		CDeviceManager::Instance()->GetTapiString(l_strRedirectionID, l_pCallInfo, l_pCallInfo->dwRedirectionIDSize, l_pCallInfo->dwRedirectionIDOffset);
		CDeviceManager::Instance()->GetTapiString(l_strCallData, l_pCallInfo, l_pCallInfo->dwCallDataSize, l_pCallInfo->dwCallDataOffset);

		l_strInfo = m_pStrUtil->Format("Line Call Info, Call: [%u], AddressID: [%u], Origin: [%s], Reason: [%s], Trunk: [%s], CallRefId: [%u], RelateCallrefId: [%u], "
			"CallerID: [%s], CalledID: [%s], ConnectedID: [%s], RedirectingID: [%s], RedirectionID: [%s], CallData: [%s]",
			p_hCall,
			l_pCallInfo->dwAddressID,
			CAvayaResult::Instance()->DescribeCallOrigin(l_strCallOrigin, l_pCallInfo->dwOrigin).c_str(),
			CAvayaResult::Instance()->DescribeCallReason(l_strCallReason, l_pCallInfo->dwReason).c_str(),
			l_strTrunk.c_str(), l_pCallInfo->dwCallID, l_pCallInfo->dwRelatedCallID,
			l_strCallerID.c_str(), l_strCalledID.c_str(), l_strConnectedID.c_str(), l_strRedirectingID.c_str(), l_strRedirectionID.c_str(), l_strCallData.c_str());
		ICC_LOG_DEBUG(m_pLog, "%s", l_strInfo.c_str());
		ShowMsg(l_strInfo);

		delete[] l_pCallInfo;
		l_pCallInfo = NULL;
	}
}

void CAvayaEvent::LineCallState(HCALL p_hCall, DWORD p_dwCallState, DWORD p_dwCallStateDetail, DWORD p_dwCallPrivilege)
{
	std::string l_strInfo = m_pStrUtil->Format("Line Call State Event, Call: [%u], CallState: [%u], CallStateDetail: [%u], CallPrivilege: [%u]", 
		p_hCall, p_dwCallState, p_dwCallStateDetail, p_dwCallPrivilege);
	ICC_LOG_DEBUG(m_pLog, "%s", l_strInfo.c_str());
	ShowMsg(l_strInfo);

	// Get further information about the call info
	std::string l_strCallOrigin = "";
	std::string l_strCallReason = "";
	std::string l_strTrunk = "";
	std::string l_strDeviceNum = "";
	std::string l_strCallerID = "";
	std::string l_strCalledID = "";
	std::string l_strConnectedID = "";
	std::string l_strRedirectingID = "";
	std::string l_strRedirectionID = "";
	std::string l_strCallData = "";

	HLINE l_hLine = NULL;
	long l_lCallOrigin = 0;
	long l_lReason = 0;
	long l_lCSTACallRefId = DEFAULT_CALLREFID;
	long l_lRelatedCSTACallRefId = DEFAULT_CALLREFID;

	LINECALLINFO* l_pCallInfo = NULL;
	CDeviceManager::Instance()->LoopLineGetCallInfo(p_hCall, l_pCallInfo);
	if (l_pCallInfo)
	{
		l_hLine = l_pCallInfo->hLine;
		l_lCallOrigin = l_pCallInfo->dwOrigin;
		l_lReason = l_pCallInfo->dwReason;
		l_lCSTACallRefId = l_pCallInfo->dwCallID;
		l_lRelatedCSTACallRefId = l_pCallInfo->dwRelatedCallID;
		l_strDeviceNum = CDeviceManager::Instance()->GetDnByLineHandle(l_hLine);

		if (l_pCallInfo->dwTrunk == 0xffffffff)
			l_strTrunk = "Unknown";
		else
			l_strTrunk = std::to_string(l_pCallInfo->dwTrunk);

		CDeviceManager::Instance()->GetTapiString(l_strCallerID, l_pCallInfo, l_pCallInfo->dwCallerIDSize, l_pCallInfo->dwCallerIDOffset);
		CDeviceManager::Instance()->GetTapiString(l_strCalledID, l_pCallInfo, l_pCallInfo->dwCalledIDSize, l_pCallInfo->dwCalledIDOffset);
		CDeviceManager::Instance()->GetTapiString(l_strConnectedID, l_pCallInfo, l_pCallInfo->dwConnectedIDSize, l_pCallInfo->dwConnectedIDOffset);
		CDeviceManager::Instance()->GetTapiString(l_strRedirectingID, l_pCallInfo, l_pCallInfo->dwRedirectingIDSize, l_pCallInfo->dwRedirectingIDOffset);
		CDeviceManager::Instance()->GetTapiString(l_strRedirectionID, l_pCallInfo, l_pCallInfo->dwRedirectionIDSize, l_pCallInfo->dwRedirectionIDOffset);
		CDeviceManager::Instance()->GetTapiString(l_strCallData, l_pCallInfo, l_pCallInfo->dwCallDataSize, l_pCallInfo->dwCallDataOffset);
		
		l_strInfo = m_pStrUtil->Format("Line Call State, Call: [%u], Line: [%u], DeviceNum: [%s], AddressID: [%u], State: [%s], Origin: [%s], Reason: [%s], "
			"Trunk: [%s], CallRefId: [%d],  RelatedCallRefId: [%d],CallerID: [%s], CalledID: [%s], "
			"ConnectedID: [%s], RedirectingID: [%s], RedirectionID: [%s], CallData: [%s]",
			p_hCall,
			l_hLine,
			l_strDeviceNum.c_str(),
			l_pCallInfo->dwAddressID,
			CAvayaResult::Instance()->DescribeCallStateDetail(p_dwCallState, p_dwCallStateDetail).c_str(),
			CAvayaResult::Instance()->DescribeCallOrigin(l_strCallOrigin, l_pCallInfo->dwOrigin).c_str(),
			CAvayaResult::Instance()->DescribeCallReason(l_strCallReason, l_pCallInfo->dwReason).c_str(),
			l_strTrunk.c_str(), 
			l_lCSTACallRefId, l_lRelatedCSTACallRefId, l_strCallerID.c_str(), l_strCalledID.c_str(), 
			l_strConnectedID.c_str(),l_strRedirectingID.c_str(), l_strRedirectionID.c_str(), l_strCallData.c_str());
		ICC_LOG_DEBUG(m_pLog, "%s", l_strInfo.c_str());
		ShowMsg(l_strInfo);

		delete[] l_pCallInfo;
		l_pCallInfo = NULL;

		switch (p_dwCallState)
		{
		case LINECALLSTATE_IDLE:		//1������
		{
		//	ICC_LOG_DEBUG(m_pLog, "Line Call State: Idel");

			HRESULT l_bResult = ::lineDeallocateCall(p_hCall);
			if (l_bResult == S_OK)
			{
				ICC_LOG_DEBUG(m_pLog, "Line Deallocate Call [%u]", p_hCall);
				CDeviceManager::Instance()->ProcessCallHandle(l_hLine, p_hCall, FREE_CALL);
			}
		}
		break;
		case LINECALLSTATE_OFFERING:	// 2, ����
		{
			ICC_LOG_DEBUG(m_pLog, "Line Call State: Offering");

			//	CDeviceManager::Instance()->ProcessCallHandle(l_hLine, p_hCall, PENDING_CALL);
			if (l_lReason == LINECALLREASON_TRANSFER)
			{
				ICC_LOG_DEBUG(m_pLog, "Line Call State: Offering : DeliveredEvent111");
				this->DeliveredEvent(l_hLine, p_hCall, p_dwCallState, l_lCSTACallRefId, l_lRelatedCSTACallRefId, true, l_strCallerID, l_strCalledID);
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "Line Call State: Offering : DeliveredEvent222");
				this->DeliveredEvent(l_hLine, p_hCall, p_dwCallState, l_lCSTACallRefId, l_lRelatedCSTACallRefId, false, l_strCallerID, l_strCalledID);
			}

		}
		break;
		case LINECALLSTATE_ACCEPTED:
		{
			//ICC_LOG_DEBUG(m_pLog, "Line Call State: Accepted");
		}
		break;
		case LINECALLSTATE_DIALTONE:		// 8, ��ժ�����ٲ��� �� �ȱ������ٲ���
		{
			//ICC_LOG_DEBUG(m_pLog, "Line Call State: Dialtone");

			if (p_dwCallStateDetail == LINEDIALTONEMODE_NORMAL)
			{
				if (l_lRelatedCSTACallRefId != DEFAULT_CALLREFID && l_lRelatedCSTACallRefId != 0)
				{
					// ��ѯ��ԭ������
					this->HeldEvent(l_hLine, p_hCall, l_lRelatedCSTACallRefId);
				}

				// ���飬�����µĻ���ID
			//	this->ServiceInitiatedEvent(l_hLine, p_hCall, l_lCSTACallRefId, l_lRelatedCSTACallRefId, l_strDeviceNum);
			} 

			if (CCallManager::Instance()->GetDialTime(l_lCSTACallRefId).empty())
			{
				//���ժ���������ų�ʱ������ LINECALLSTATE_DIALING �¼���������ڴ˴���ժ���¼�
				this->ServiceInitiatedEvent(l_hLine, p_hCall, l_lCSTACallRefId, l_lRelatedCSTACallRefId, l_strDeviceNum);
			}
		}
		break;
		case LINECALLSTATE_DIALING:			// 16, ժ���ٲ��Ż�ժ��ֱ�Ӳ��ţ����и��¼�
		{
			//ICC_LOG_DEBUG(m_pLog, "Line Call State: Dialing");

			if (CCallManager::Instance()->GetDialTime(l_lCSTACallRefId).empty())
			{
				this->ServiceInitiatedEvent(l_hLine, p_hCall, l_lCSTACallRefId, l_lRelatedCSTACallRefId, l_strDeviceNum);
			}
		}
		break;
		case LINECALLSTATE_RINGBACK:		//32, ����
		{
			//ICC_LOG_DEBUG(m_pLog, "Line Call State: Ringback");

			if (!l_strRedirectionID.empty())
			{
				l_strCallerID = l_strRedirectionID;		// ��ѯ�����Ļ���ȡ��ʵ����
			}

			if (CCallManager::Instance()->GetDialTime(l_lCSTACallRefId).empty())
			{
				// ��ժ����ֱ�ӵ�Mallcall�ӿڷ�����У��� LINECALLSTATE_DIALTONE/LINECALLSTATE_DIALING �¼�
				// ����ڴ˴�����ժ���¼�
				this->ServiceInitiatedEvent(l_hLine, p_hCall, l_lCSTACallRefId, l_lRelatedCSTACallRefId, l_strDeviceNum);
			}
			ICC_LOG_DEBUG(m_pLog, "Line Call State: Ringback : DeliveredEvent");
			this->DeliveredEvent(l_hLine, p_hCall, p_dwCallState, l_lCSTACallRefId, l_lRelatedCSTACallRefId, false, l_strCallerID, l_strCalledID);
		}
		break;
		case LINECALLSTATE_BUSY:
		{
			//ICC_LOG_DEBUG(m_pLog, "Line Call State: Busy");
		}
		break;
		case LINECALLSTATE_SPECIALINFO:
		{
			//ICC_LOG_DEBUG(m_pLog, "Line Call State: SpecialInfo");
		}
		break;
#if 0
		case LINECALLSTATE_CONNECTED:	// 256 ͨ��
		{
			ICC_LOG_DEBUG(m_pLog, "Line Call State: Connected");
			// 3
			if (p_dwCallStateDetail == LINECONNECTEDMODE_ACTIVE)
			{
				if (l_lCallOrigin == LINECALLORIGIN_CONFERENCE)
				{
					ICC_LOG_DEBUG(m_pLog, "Line Call State: Connected : ConferenceConnectEvent111");
					ConferenceConnectEvent(l_hLine, p_hCall, l_lCSTACallRefId, l_lRelatedCSTACallRefId);
				}
				else
				{
					// Talking
					if (l_lReason == LINECALLREASON_TRANSFER)
					{
						ICC_LOG_DEBUG(m_pLog, "Line Call State: Connected : EstablishedTransferEvent");
						this->EstablishedTransferEvent(l_hLine, p_hCall, l_lCSTACallRefId, l_strCallerID, l_strCalledID);
					}
					else
					{
						ICC_LOG_DEBUG(m_pLog, "Line Call State: Connected : EstablishedEvent111");
						this->EstablishedEvent(l_hLine, p_hCall, l_lCSTACallRefId, l_lRelatedCSTACallRefId, l_strCallerID, l_strCalledID);
					}
				}
			}
			else if (p_dwCallStateDetail == LINECONNECTEDMODE_ACTIVEHELD)
			{
				ICC_LOG_DEBUG(m_pLog, "Line Call State: Connected : ProcessCallHandle");
				// Talking to Held
				CDeviceManager::Instance()->ProcessCallHandle(l_hLine, p_hCall, HELD_CALL);
				this->HeldEvent(l_hLine, p_hCall, l_lCSTACallRefId);
			}
			else
			{
				if (l_lCallOrigin == LINECALLORIGIN_CONFERENCE)
				{
					ICC_LOG_DEBUG(m_pLog, "Line Call State: Connected : ConferenceConnectEvent222");
					ConferenceConnectEvent(l_hLine, p_hCall, l_lCSTACallRefId, l_lRelatedCSTACallRefId);
				}
				else
				{
					ICC_LOG_DEBUG(m_pLog, "Line Call State: Connected : EstablishedEvent222");
					this->EstablishedEvent(l_hLine, p_hCall, l_lCSTACallRefId, l_lRelatedCSTACallRefId, l_strCallerID, l_strCalledID);
				}
			}
		}
		break;
#else
		case LINECALLSTATE_CONNECTED:	// 256 ͨ��
		{
			//ICC_LOG_DEBUG(m_pLog, "Line Call State: Connected");

			// 3
			if (p_dwCallStateDetail == LINECONNECTEDMODE_ACTIVE)
			{
				if (l_lCallOrigin == LINECALLORIGIN_CONFERENCE)
				{
					//Conference
					CDeviceManager::Instance()->ProcessCallHandle(l_hLine, p_hCall, CONFERENCE_CALL);

					// 8001 -> 8000(ID 69) , 8000 + 8002
					// 8002 �Ļ���ID+1(ID 70)��8000�Ļ���ID+2(ID 71)
					long l_lRelatedCSTACallRefId = DEFAULT_CALLREFID;
					if (CCallManager::Instance()->FindCallByCSTACallRefId(l_lCSTACallRefId))
					{
						ICC_LOG_DEBUG(m_pLog, "CallConference-----------------1111");
						// 8002 ����(70)
						this->CallConference(l_lCSTACallRefId, l_strDeviceNum);
					}
					else if (CCallManager::Instance()->GetRelatedCSTACallRefId(l_lCSTACallRefId, l_lRelatedCSTACallRefId))
					{
						ICC_LOG_DEBUG(m_pLog, "CallConference-----------------2222");
						// 8001 ����(69)
						this->CallConference(l_lRelatedCSTACallRefId, l_strDeviceNum);
					} 
					else
					{
						std::string l_strCTICallRefId = "";
						if (CConferenceManager::Instance()->GetCTICallRefIdByParty(l_strDeviceNum, l_strCTICallRefId))
						{
							if (CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lRelatedCSTACallRefId))
							{
								// 8000 ����(71)
								CCallManager::Instance()->CopyCall(l_lCSTACallRefId, l_lRelatedCSTACallRefId);
								CCallManager::Instance()->AddRelatedCSTACallRefId(l_lCSTACallRefId, l_lRelatedCSTACallRefId);
								CCallManager::Instance()->DeleteCall(l_lRelatedCSTACallRefId);

								CDeviceManager::Instance()->CopyLogicalCallState(l_lCSTACallRefId, l_lRelatedCSTACallRefId);
								CDeviceManager::Instance()->DeleteAllLogicalCallState(l_lRelatedCSTACallRefId);
								ICC_LOG_DEBUG(m_pLog, "CallConference-----------------3333");
								this->CallConference(l_lCSTACallRefId, l_strDeviceNum);
							}
						}
					}
				} 

				else
				{

					// Talking
					/*
					if (l_lReason == LINECALLREASON_TRANSFER)
					{
						ICC_LOG_DEBUG(m_pLog, "Line Call State: Connected : EstablishedTransferEvent");
						this->EstablishedTransferEvent(l_hLine, p_hCall, l_lCSTACallRefId, l_strCallerID, l_strCalledID);
					}
					else
					{
						ICC_LOG_DEBUG(m_pLog, "Line Call State: Connected : EstablishedEvent111");
						this->EstablishedEvent(l_hLine, p_hCall, l_lCSTACallRefId, l_lRelatedCSTACallRefId, l_strCallerID, l_strCalledID);
					}
					*/
					/*
					// һ���ͨ�¼�
					Line Call State Event, Call: [66286], CallState : [256], CallStateDetail : [1], CallPrivilege : [0]
					Line Call State, DeviceNum = [8000], Line = [66525], AddressID = [0], State = [Active], Origin = [Internal], Reason = [Direct], Trunk = [Unknown], CallRefId = [57], RelatedCallRefId = [0], CallerID = [8001], CalledID = [8000], ConnectedID = [8001], RedirectingID = [], RedirectionID = [], CallData = []
					Device : [8000] talking, CallerId : [8001], CalledId : [8000], CallRefID : [57]

					// ת�����������/���У�8000/8002���Ҷ��¼�
					Line Call State Event, Call : [66235], CallState : [256], CallStateDetail : [1], CallPrivilege : [0]
					Line Call State, DeviceNum = [8000], Line = [66525], AddressID = [0], State = [Active], Origin = [Outbound], Reason = [Direct], Trunk = [1376267], CallRefId = [58], RelatedCallRefId = [57], CallerID = [8001], CalledID = [8002], ConnectedID = [], RedirectingID = [8002], RedirectionID = [], CallData = []
					Device : [8000] talking, CallerId : [8001], CalledId : [8002], CallRefID : [58]

					// ת��������ͨ�¼�
					Line Call State Event, Call : [66235], CallState : [256], CallStateDetail : [1], CallPrivilege : [0]
					Line Call State, DeviceNum = [8000], Line = [66525], AddressID = [0], State = [Active], Origin = [Outbound], Reason = [Direct], Trunk = [Unknown], CallRefId = [66], RelatedCallRefId = [65], CallerID = [8001], CalledID = [8002], ConnectedID = [8002], RedirectingID = [], RedirectionID = [], CallData = []
					Device : [8000] talking, CallerId : [8001], CalledId : [8002], CallRefID : [66]*/
					// Talking
					// tzx 2023-3-17
					if (l_lRelatedCSTACallRefId != 0 && l_lRelatedCSTACallRefId != DEFAULT_CALLREFID && !l_strRedirectingID.empty())
					{
						ICC_LOG_DEBUG(m_pLog, "ConnectClearedEvent : EstablishedEvent111,l_lCSTACallRefId:[%d]");
						// ת�����������/���У�8000/8002���Ҷ��¼�
						this->ConnectClearedEvent(l_hLine, p_hCall, l_lCSTACallRefId);
					} 
					else
					{
						//this->EstablishedEvent(l_hLine, p_hCall, l_lCSTACallRefId, l_strCallerID, l_strCalledID);
						this->EstablishedEvent(l_hLine, p_hCall, l_lCSTACallRefId, l_lRelatedCSTACallRefId, l_strCallerID, l_strCalledID);
					}
				}
			} 
			else if (p_dwCallStateDetail == LINECONNECTEDMODE_ACTIVEHELD)
			{
				// Talking to Held
				CDeviceManager::Instance()->ProcessCallHandle(l_hLine, p_hCall, HELD_CALL);
				this->HeldEvent(l_hLine, p_hCall, l_lCSTACallRefId);
			}
			else
			{
				//
			}
		}
		break;
#endif
		case LINECALLSTATE_PROCEEDING:	//512
		{
			//ICC_LOG_DEBUG(m_pLog, "Line Call State: Proceeding");
		}
		break;
		case LINECALLSTATE_ONHOLD:	//1024
		{
			//ICC_LOG_DEBUG(m_pLog, "Line Call State: OnHold");

			CDeviceManager::Instance()->ProcessCallHandle(l_hLine, p_hCall, HELD_CALL);
			this->HeldEvent(l_hLine, p_hCall, l_lCSTACallRefId);
		}
		break;
		case LINECALLSTATE_CONFERENCED:	// 2048
		{
			//Conference
			//this->ConferencedEvent(l_hLine, p_hCall, l_lCSTACallRefId, l_strConnectedID);
			//ICC_LOG_DEBUG(m_pLog, "Line Call State: Conference");
		}
		break;
		case LINECALLSTATE_ONHOLDPENDCONF:	// 4096
		{
			// �������»���ID��ȡ
		//	ICC_LOG_DEBUG(m_pLog, "Line Call State: OnHoldPendConf");
		}
		break;
		case LINECALLSTATE_ONHOLDPENDTRANSFER:		//8192
		{
		//	ICC_LOG_DEBUG(m_pLog, "Line Call State: OnHoldPendTransfer");

			//���顢ת����ѯʱ���յ����¼�
			// Talking to Held
			/*CDeviceManager::Instance()->ProcessCallHandle(l_hLine, p_hCall, HELD_CALL);
			this->HeldEvent(l_hLine, l_lCSTACallRefId);*/

			/*CDeviceManager::Instance()->ProcessCallHandle(l_hLine, p_hCall, WAITING_CALL);
			this->ServiceInitiatedEvent(l_hLine, p_hCall, l_lCSTACallRefId, l_strDeviceNum);*/
		}
		break;
		case LINECALLSTATE_DISCONNECTED: // 16384
		{
			//ICC_LOG_DEBUG(m_pLog, "Line Call State: Disconnected");
			if (p_dwCallStateDetail == LINEDISCONNECTMODE_FORWARDED) //�����ɿ���ת����
			{
				ICC_LOG_DEBUG(m_pLog, "----1111--------------------Line Call State: Disconnected : DisconnectTransferredEvent");  //z
				//	����ƫת
				this->TransferredEvent(l_hLine, p_hCall, l_lCSTACallRefId, l_lRelatedCSTACallRefId);
			}
			else if (p_dwCallStateDetail == LINEDISCONNECTMODE_PICKUP) //�����Ǵ������ط�������
			{
				ICC_LOG_DEBUG(m_pLog, "----1111--------------------Line Call State: Disconnected : FindLineByHandle");  //z
				boost::shared_ptr<CTapiLine> l_pLineDevice = CDeviceManager::Instance()->FindLineByHandle(l_hLine);
				if (!l_pLineDevice)
				{
					ICC_LOG_WARNING(m_pLog, "Line Call State: Disconnected : ConnectClearedEvent not find device by line handle: [%u]", l_hLine);

					return;
				}
				ICC_LOG_DEBUG(m_pLog, "Line Call State: Disconnected : ProcessCallHandle");
				CDeviceManager::Instance()->ProcessCallHandle(l_hLine, p_hCall, FREE_CALL);
				std::string l_strDeviceNum = l_pLineDevice->GetDeviceNum();
				ICC_LOG_DEBUG(m_pLog, "Line Call State: Disconnected : DeviceHangup");
				this->DeviceHangup(l_lCSTACallRefId, l_strDeviceNum, l_strDeviceNum);
				ICC_LOG_DEBUG(m_pLog, "Line Call State: Disconnected : DeviceFree");
				this->DeviceFree(l_lCSTACallRefId, l_strDeviceNum);
			}
			else if (p_dwCallStateDetail == 524289)
			{
				//this->DisconnectTransferredEvent(l_hLine, p_hCall, l_lCSTACallRefId, l_lRelatedCSTACallRefId);
			}
			else
			{
				std::string l_strDeviceNum;
				boost::shared_ptr<CTapiLine> l_pLineDevice = CDeviceManager::Instance()->FindLineByHandle(l_hLine);
				if (l_pLineDevice)
				{
					l_strDeviceNum = l_pLineDevice->GetDeviceNum();
				}

				ICC_LOG_DEBUG(m_pLog, "device<%s>----1111-------Line Call State: Disconnected,p_dwCallStateDetail:[0x%x],l_lReason:[0x%x],l_lRelatedCSTACallRefId:[%d],l_lCSTACallRefId:[%d]", 
					l_strDeviceNum.c_str(),
					p_dwCallStateDetail, l_lReason, l_lRelatedCSTACallRefId, l_lCSTACallRefId);

				if (l_lReason == LINECALLREASON_TRANSFER)
				{
					//��������
					if ((l_lRelatedCSTACallRefId > l_lCSTACallRefId) && (l_lRelatedCSTACallRefId - l_lCSTACallRefId > 100))
					{
						ICC_LOG_DEBUG(m_pLog, "----1111-------Line Call State: Disconnected : ConferenceDisconnectEvent222");
						//this->ConferenceDisconnectEvent(l_hLine, p_hCall, l_lCSTACallRefId, l_lRelatedCSTACallRefId);
					}
					else
					{
						ICC_LOG_DEBUG(m_pLog, "----1111-------Line Call State: Disconnected : DisconnectReasonTransferredEvent");
						//this->DisconnectReasonTransferredEvent(l_hLine, p_hCall, l_lCSTACallRefId, l_lRelatedCSTACallRefId);
					}
				}
				else
				{
					//��������
					if ((l_lRelatedCSTACallRefId > l_lCSTACallRefId) && (l_lRelatedCSTACallRefId - l_lCSTACallRefId > 100))
					{
						ICC_LOG_DEBUG(m_pLog, "----1111-------Line Call State: Disconnected : ConferenceDisconnectEvent333");   //z
						//this->ConferenceDisconnectEvent(l_hLine, p_hCall, l_lCSTACallRefId, l_lRelatedCSTACallRefId);
					}
					else
					{
						if (CDeviceManager::Instance()->FindLogicalCallState(l_lCSTACallRefId, l_strDeviceNum))
						{
							// �����ж���һ��¼�
							ICC_LOG_DEBUG(m_pLog, "----1111-------Line Call State: Disconnected : ConnectClearedEvent");   //z
							// tzx  this->ConnectClearedEvent(l_hLine, p_hCall, l_lCSTACallRefId);
						}
						else
						{
							ICC_LOG_ERROR(m_pLog, "----1111-------Line Call State: Disconnected : ConnectClearedEvent, LogicalDevice: [%s], CSTACallRefId: [%u], Logical Call State Not Exist!",
								l_strDeviceNum.c_str(), l_lCSTACallRefId);
						}
					}
				}

				// tzx 2023-3-17
				long l_lTempCSTACallRefId = l_lCSTACallRefId;
				if (l_lCallOrigin == LINECALLORIGIN_CONFERENCE || l_lReason == LINECALLREASON_TRANSFER) //����ܽ�
				{
					//Conference, ȡԭ����ID
					if (!CCallManager::Instance()->FindCallByCSTACallRefId(l_lCSTACallRefId))
					{
						CCallManager::Instance()->GetRelatedCSTACallRefId(l_lCSTACallRefId, l_lTempCSTACallRefId);
					}
				}

				if (CDeviceManager::Instance()->FindLogicalCallState(l_lTempCSTACallRefId, l_strDeviceNum))
				{

					ICC_LOG_DEBUG(m_pLog, "---123------------------Line Call State: LINECALLSTATE_DISCONNECTED");
					// �����ж���һ��¼�
					this->ConnectClearedEvent(l_hLine, p_hCall, l_lTempCSTACallRefId);
				}
				else
				{
					ICC_LOG_DEBUG(m_pLog, "ConnectClearedEvent, LogicalDevice: [%s], CSTACallRefId: [%u], Logical Call State Not Exist!", 
						l_strDeviceNum.c_str(), l_lTempCSTACallRefId);
				}
			}
		}
		break;
		case LINECALLSTATE_UNKNOWN:
		{
			//ICC_LOG_WARNING(m_pLog, "Line Call State: Unknown !");
		}
		break;
		default:
		{
			//ICC_LOG_WARNING(m_pLog, "Line Call State Is Not Processed !");
		}
		break;
		}
	}
}
void CAvayaEvent::LineNewCall(HLINE p_hLine, DWORD p_dwAddressID, HCALL p_hCall, DWORD p_dwCallPrivilege)
{
	boost::shared_ptr<CTapiLine> l_pLineDevice = CDeviceManager::Instance()->FindLineByHandle(p_hLine);
	if (!l_pLineDevice)
	{
		ICC_LOG_WARNING(m_pLog, "LineNewCall not find device by line handle: [%u]", p_hLine);

		return;
	}

	CDeviceManager::Instance()->ProcessCallHandle(p_hLine, p_hCall, PENDING_CALL);

	std::string l_strInfo = m_pStrUtil->Format("Line New Call Event, DeviceNum: [%s], Line: [%u], AddressId: [%u], Call: [%u], CallPrivilege: [%u]",
		l_pLineDevice->GetDeviceNum().c_str(), p_hLine, p_dwAddressID, p_hCall, p_dwCallPrivilege);
	ICC_LOG_DEBUG(m_pLog, "%s", l_strInfo.c_str());
	ShowMsg(l_strInfo);
}
void CAvayaEvent::LineDevState(HLINE p_hLine, DWORD p_dwDeviceState, DWORD p_dwDeviceStateDetail1, DWORD p_dwDeviceStateDetail2)
{
	boost::shared_ptr<CTapiLine> l_pLineDevice = CDeviceManager::Instance()->FindLineByHandle(p_hLine);
	if (!l_pLineDevice)
	{
		ICC_LOG_WARNING(m_pLog, "LineDevState not find device by line handle: [%u]", p_hLine);

		return;
	}

	std::string l_strDeviceNum = l_pLineDevice->GetDeviceNum();
	std::string l_strInfo = m_pStrUtil->Format("Line Device State Event, DeviceNum: [%s], Line: [%u], DeviceState: [%u], DeviceStateDetail1: [%u], DeviceStateDetail2: [%u]",
		l_strDeviceNum.c_str(), p_hLine, p_dwDeviceState, p_dwDeviceStateDetail1, p_dwDeviceStateDetail2);
	ICC_LOG_DEBUG(m_pLog, "%s", l_strInfo.c_str());
	ShowMsg(l_strInfo);
	 
	switch (p_dwDeviceState)
	{
	case LINEDEVSTATE_OUTOFSERVICE:	//128
		break;
	case LINEDEVSTATE_DEVSPECIFIC: //InGrp,OutGrp
		this->AgentStateEvent(l_strDeviceNum, p_dwDeviceState);
		break;
	default:
		break;
	}
}


//////////////////////////////////////////////////////////////////////////
void CAvayaEvent::AgentStateEvent(const std::string& p_strDeviceNum, DWORD p_dwDeviceState)
{
	std::string l_strResult = "";
	std::string l_strInfo = m_pStrUtil->Format("Line Agent State, AgentId: [%s], Status: [%s]",
		p_strDeviceNum.c_str(), CAvayaResult::Instance()->DescribeDeviceStatus(l_strResult, p_dwDeviceState).c_str());
	ICC_LOG_DEBUG(m_pLog, "%s", l_strInfo.c_str());
	ShowMsg(l_strInfo);

	if (CAgentManager::Instance()->IsInitOver())
	{
		CAgentManager::Instance()->ProcesAgentState(p_strDeviceNum);
	}

	l_strInfo = m_pStrUtil->Format("Line Agent State, ProcesAgentState ");
	ICC_LOG_DEBUG(m_pLog, "%s", l_strInfo.c_str());
}


bool CAvayaEvent::DeliveredEvent(HLINE p_hLine, HCALL p_hCall, DWORD p_dwCallState, long p_lCSTACallRefId, long p_lRelatedCSTACallRefId, bool p_bIsTransferCall, const std::string& p_strCallerID, const std::string& p_strCalledID)
{
	boost::shared_ptr<CTapiLine> l_pLineDevice = CDeviceManager::Instance()->FindLineByHandle(p_hLine);
	bool bRes = false;
	if (!l_pLineDevice)
	{
		ICC_LOG_WARNING(m_pLog, "DeliveredEvent not find device by line handle: [%u]", p_hLine);

		return false;
	}

	int l_nLineDeviceType = l_pLineDevice->GetDeviceType();
	std::string l_strAlteringDevice = l_pLineDevice->GetDeviceNum();
	std::string l_strInfo = m_pStrUtil->Format("Delivered Event, Line: [%u], DevNum:[%s], LineDeviceType:[%u], CallState:[%u], CSTACallRefId: [%u], RelatesCSTACallRefId: [%u], CallerId: [%s], CalledId: [%s], AlteringDevice: [%s]",
		p_hLine, l_strAlteringDevice.c_str(), l_nLineDeviceType, p_dwCallState, p_lCSTACallRefId, p_lRelatedCSTACallRefId, p_strCallerID.c_str(), p_strCalledID.c_str(), l_strAlteringDevice.c_str());
	ICC_LOG_DEBUG(m_pLog, "%s", l_strInfo.c_str());
	ShowMsg(l_strInfo);
	//Delivered Event, Line: [65791], DevNum : [110], CallState : [2], CSTACallRefId : [125], CallerId : [8002], CalledId : [110], AlteringDevice : [110]
	//	8002 -> 110 -> 8001
	// Delivered Event, Line: [66525], DevNum: [8001], CallState: [2], CSTACallRefId: [101], CallerId: [8002], CalledId: [110], AlteringDevice: [8001]
	// Delivered Event, Line: [66457], DevNum : [8002], CallState : [32], CSTACallRefId : [101], CallerId : [8002], CalledId : [110], AlteringDevice : [8002]
	// Delivered Event, Line: [66303], DevNum: [110], CallState: [2], CSTACallRefId: [101], CallerId: [8002], CalledId: [110], AlteringDevice: [110]
	switch (p_dwCallState)
	{
	case LINECALLSTATE_OFFERING:	// ����
	{
		if (l_nLineDeviceType == CTC_TYPE_IPPHONE || l_nLineDeviceType == CTC_TYPE_DIGITAL || l_nLineDeviceType == CTC_TYPE_ANALOG)
		{
			boost::shared_ptr<CTapiLine> l_pCalledDevice = boost::dynamic_pointer_cast<CTapiLine>(CDeviceManager::Instance()->FindDeviceByDn(p_strCalledID));
			if (l_pCalledDevice)
			{
				std::string l_strCalledDevice = l_pCalledDevice->GetDeviceNum();
				int l_nCalledDeviceType = l_pCalledDevice->GetDeviceType();
				ICC_LOG_DEBUG(m_pLog, "DeliveredEvent CalledDevice[%s], CalledDeviceType[%u]", l_strCalledDevice.c_str(), l_nCalledDeviceType);

				if (p_bIsTransferCall)
				{
					std::string l_strRelateDev = "";
					if (CCallManager::Instance()->GetDeviceNum(p_lCSTACallRefId, l_strRelateDev))
					{
						if (CDeviceManager::Instance()->FindLogicalCallState(p_lCSTACallRefId, l_strRelateDev))
						{
							this->DeviceHangup(p_lCSTACallRefId, l_strRelateDev, l_strRelateDev);
							this->DeviceFree(p_lCSTACallRefId, l_strRelateDev);

							ICC_LOG_DEBUG(m_pLog, "--2222222222---- PostCallOverNotif: [%u]", p_lCSTACallRefId);
							CCallManager::Instance()->PostCallOverNotif(p_lCSTACallRefId, l_strRelateDev);
						}
					}

					this->CallIncoming(p_lCSTACallRefId, p_strCallerID, p_strCalledID, p_strCalledID);

					this->CallAssign(p_lCSTACallRefId, p_strCallerID, p_strCalledID, l_strAlteringDevice);

					this->CallAlerting(p_lCSTACallRefId, p_strCallerID, p_strCalledID, l_strAlteringDevice);
				}
				else
				{
					if ((l_nCalledDeviceType == CTC_TYPE_ACDGROUP))
					{
						// ���е�ΪACD���룬��Ҫ����incoming, assign, alerting ��Ϣ
						this->CallIncoming(p_lCSTACallRefId, p_strCallerID, p_strCalledID, p_strCalledID);

						this->CallAssign(p_lCSTACallRefId, p_strCallerID, p_strCalledID, l_strAlteringDevice);

						this->CallAlerting(p_lCSTACallRefId, p_strCallerID, p_strCalledID, l_strAlteringDevice);

						bRes = true;
					}
					else
					{
						// ���к���Ϊ�ֻ����룬ֻ��Alerting ��Ϣ
						this->CallAlerting(p_lCSTACallRefId, p_strCallerID, p_strCalledID, l_strAlteringDevice);

						bRes = true;
					}
				}

			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "DeliveredEvent Cannot FindDeviceByDn : [%s]", p_strCalledID.c_str());
			}
		}
		else if (l_nLineDeviceType == CTC_TYPE_ACDGROUP)
		{
			//	���Ŷӣ�IPO ���յ�����Ŀ���豸���¼������յ� ACD �¼�
			std::string l_strIncomingTime = CCallManager::Instance()->GetIncomingTime(p_lCSTACallRefId);
			if (l_strIncomingTime.empty())
			{
				this->CallIncoming(p_lCSTACallRefId, p_strCallerID, p_strCalledID, p_strCalledID);

				this->CallWaiting(p_lCSTACallRefId, p_strCallerID, p_strCalledID, p_strCalledID);
				bRes = true;
			}
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "DeliveredEvent l_nLineDeviceType: [%d]", l_nLineDeviceType);
		}
	}
	break;
	case LINECALLSTATE_RINGBACK:	// ����
	{
		this->CallAlteringBack(p_lCSTACallRefId, p_lRelatedCSTACallRefId, p_strCallerID, p_strCalledID, l_strAlteringDevice);
		true;
	}
	break;
	default:
		break;
	}

	return bRes;
}


void CAvayaEvent::EstablishedEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId, long p_lRelatedCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID)
{
	boost::shared_ptr<CTapiLine> l_pLineDevice = CDeviceManager::Instance()->FindLineByHandle(p_hLine);
	if (!l_pLineDevice)
	{
		ICC_LOG_WARNING(m_pLog, "EstablishedEvent not find device by line handle: [%u]", p_hLine);

		return;
	}

	CDeviceManager::Instance()->ProcessCallHandle(p_hLine, p_hCall, CONNECTED_CALL);

	std::string l_strDeviceNum = l_pLineDevice->GetDeviceNum();

	boost::shared_ptr<CDevice> l_pDevice = CDeviceManager::Instance()->FindDeviceByDn(l_strDeviceNum);
	if (l_pDevice)
	{
		std::string l_strDeviceState = CallStateString[STATE_FREE];
		std::string l_strStateTime = m_pDateTime->CurrentDateTimeStr();
		long l_lCSTACallRefId = DEFAULT_CALLREFID;
		l_pDevice->GetCurrentLogicalCall(l_lCSTACallRefId, l_strDeviceState, l_strStateTime);
		if ((l_strDeviceState == CallStateString[STATE_RING]) || (l_strDeviceState == CallStateString[STATE_SIGNALBACK]))
		{
			if (l_lCSTACallRefId != p_lCSTACallRefId)
			{
				ICC_LOG_WARNING(m_pLog, "EstablishedEvent CallState Error device=[%s], state=[%s], OldCallRefId[%u], NewCallRefId[%u]", l_strDeviceNum.c_str(), l_strDeviceState.c_str(), l_lCSTACallRefId, p_lCSTACallRefId);

				this->PostDeviceStateEx(p_lCSTACallRefId, l_lCSTACallRefId, l_strDeviceNum, l_strDeviceState);

				CDeviceManager::Instance()->UpdateLogicalCallState(l_lCSTACallRefId, l_strDeviceNum, CallStateString[STATE_HANGUP]);
				CDeviceManager::Instance()->UpdateLogicalCallState(l_lCSTACallRefId, l_strDeviceNum, CallStateString[STATE_FREE]);
				CDeviceManager::Instance()->DeleteLogicalCallState(l_lCSTACallRefId, l_strDeviceNum);
			}
		}
	}

	this->CallTalking(p_lCSTACallRefId, p_strCallerID, p_strCalledID, l_strDeviceNum);
}


void CAvayaEvent::EstablishedTransferEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID)
{
	boost::shared_ptr<CTapiLine> l_pLineDevice = CDeviceManager::Instance()->FindLineByHandle(p_hLine);
	if (!l_pLineDevice)
	{
		ICC_LOG_WARNING(m_pLog, "EstablishedTransferEvent not find device by line handle: [%u]", p_hLine);

		return;
	}

	std::string l_strDeviceNum = l_pLineDevice->GetDeviceNum();

	//long relatedCSTACallRefId = CTransferCallManager::Instance()->GetRelatedCSTACallRefId(p_lCSTACallRefId);

	//this->CallTalking(relatedCSTACallRefId, p_strCallerID, p_strCalledID, l_strDeviceNum);

	CDeviceManager::Instance()->ProcessCallHandle(p_hLine, p_hCall, CONNECTED_CALL);

	this->CallTalking(p_lCSTACallRefId, p_strCallerID, p_strCalledID, l_strDeviceNum);
}

void CAvayaEvent::HoldTransferEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId, long p_lRelatedCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID)
{
	boost::shared_ptr<CTapiLine> l_pLineDevice = CDeviceManager::Instance()->FindLineByHandle(p_hLine);
	if (!l_pLineDevice)
	{
		ICC_LOG_WARNING(m_pLog, "HoldTransferEvent not find device by line handle: [%u]", p_hLine);

		return;
	}

	CTransferCallManager::Instance()->AddTransferCall(p_lCSTACallRefId, p_lRelatedCSTACallRefId);
}


void CAvayaEvent::ConferenceConnectEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId, long p_lRelatedCSTACallRefId)
{

}


void CAvayaEvent::ServiceInitiatedEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId, long p_lRelatedCSTACallRefId, const std::string& p_strOffHookDevice)
{
	boost::shared_ptr<CTapiLine> l_pLineDevice = CDeviceManager::Instance()->FindLineByHandle(p_hLine);
	if (!l_pLineDevice)
	{
		ICC_LOG_WARNING(m_pLog, "ServiceInitiatedEvent not find device by line handle: [%u]", p_hLine);

		return;
	}

	CDeviceManager::Instance()->ProcessCallHandle(p_hLine, p_hCall, WAITING_CALL);
	this->DeviceOffHook(p_lCSTACallRefId, p_lRelatedCSTACallRefId, p_strOffHookDevice);
}
void CAvayaEvent::HeldEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId)
{
	boost::shared_ptr<CTapiLine> l_pLineDevice = CDeviceManager::Instance()->FindLineByHandle(p_hLine);
	if (!l_pLineDevice)
	{
		ICC_LOG_WARNING(m_pLog, "HeldEvent not find device by line handle: [%u]", p_hLine);

		return;
	}

	std::string l_strHeldDevice = l_pLineDevice->GetDeviceNum();
	this->CallHold(p_lCSTACallRefId, l_strHeldDevice);
}
void CAvayaEvent::ConferencedEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId, const std::string& p_strConnectedId)
{
	boost::shared_ptr<CTapiLine> l_pLineDevice = CDeviceManager::Instance()->FindLineByHandle(p_hLine);
	if (!l_pLineDevice)
	{
		ICC_LOG_WARNING(m_pLog, "ConferencedEvent not find device by line handle: [%u]", p_hLine);

		return;
	}
	
	std::string l_strCTICallRefId = "";
	std::string l_strConferenceDevice = l_pLineDevice->GetDeviceNum();
	if (CCallManager::Instance()->GetCallByCSTACallRefId(p_lCSTACallRefId, l_strCTICallRefId))
	{
		long l_lRelatedCSTACallRefId = p_lCSTACallRefId;
		std::string l_strRelatedCTICallRefId = "";
		if (CConferenceManager::Instance()->GetCTICallRefIdByActiveCallRefId(l_strCTICallRefId, l_strRelatedCTICallRefId))
		{
			//	���飬����ԭ����
			CCallManager::Instance()->GetCSTACallRefId(l_strRelatedCTICallRefId, l_lRelatedCSTACallRefId);
		}
		
		CCallManager::Instance()->AddConferenceParty(l_lRelatedCSTACallRefId, l_strConferenceDevice);
		CCallManager::Instance()->AddConferenceParty(l_lRelatedCSTACallRefId, p_strConnectedId);
	}
}
void CAvayaEvent::TransferredEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId, long p_lRelatedCSTACallRefId)
{
	boost::shared_ptr<CTapiLine> l_pLineDevice = CDeviceManager::Instance()->FindLineByHandle(p_hLine);
	if (!l_pLineDevice)
	{
		ICC_LOG_WARNING(m_pLog, "TransferredEvent not find device by line handle: [%u]", p_hLine);

		return;
	}

	std::string l_strCTICallRefId = "";
	std::string l_strCallerId = "";
	std::string l_strCalledId = "";
	std::string l_strDeviceNum = l_pLineDevice->GetDeviceNum();
	CCallManager::Instance()->GetCTICallRefId(p_lCSTACallRefId, l_strCTICallRefId);
	if (CTransferCallManager::Instance()->FindTransferCallByHeldCallRefId(l_strCTICallRefId))
	{
		CDeviceManager::Instance()->ProcessCallHandle(p_hLine, p_hCall, FREE_CALL);

		// ԭ���������ת�Ʒ��𷽹һ�
		this->DeviceHangup(p_lCSTACallRefId, l_strDeviceNum, l_strDeviceNum, HangupTypeString[HANGUP_TYPE_TRANSFER]);
		this->DeviceFree(p_lCSTACallRefId, l_strDeviceNum);

		// ԭ���������ԭ���йһ�
		if (CCallManager::Instance()->GetCallByCTICallRefId(l_strCTICallRefId, l_strCallerId, l_strCalledId))
		{
			this->DeviceHangup(p_lCSTACallRefId, l_strCallerId, l_strCallerId, HangupTypeString[HANGUP_TYPE_TRANSFER]);
			this->DeviceFree(p_lCSTACallRefId, l_strCallerId);
		}
		ICC_LOG_WARNING(m_pLog, "---123------------------ConnectClearedEvent11");
		this->CheckCallOver(p_lCSTACallRefId);
	} 
	else if (CTransferCallManager::Instance()->FindTransferCallByActiveCallRefId(l_strCTICallRefId))
	{
		// �»���ת�Ʒ��𷽹һ�
		this->DeviceHangup(p_lCSTACallRefId, l_strDeviceNum, l_strDeviceNum, HangupTypeString[HANGUP_TYPE_TP_TRANSFER]);
		this->DeviceFree(p_lCSTACallRefId, l_strDeviceNum);

		// �»���ԭ����ͨ��
		if (CDeviceManager::Instance()->FindLineByDn(l_strCallerId))
		{
			this->CallTalking(p_lCSTACallRefId, l_strCallerId, l_strCalledId, l_strCallerId);
		}

		// ת�ƺ�Ļ���ID���ԭ����ID,�����»����豸��Ϣ
		CCallManager::Instance()->CopyCall(p_lRelatedCSTACallRefId, p_lCSTACallRefId);
		CCallManager::Instance()->DeleteCall(p_lCSTACallRefId);

		CDeviceManager::Instance()->CopyLogicalCallState(p_lRelatedCSTACallRefId, p_lCSTACallRefId);
		CDeviceManager::Instance()->DeleteAllLogicalCallState(p_lCSTACallRefId);
	}
	else
	{
		CDeviceManager::Instance()->ProcessCallHandle(p_hLine, p_hCall, FREE_CALL);

		// ���𣬱����𷽹һ�
		this->DeviceHangup(p_lCSTACallRefId, l_strDeviceNum, l_strDeviceNum, HangupTypeString[HANGUP_TYPE_OP_TRANSFER]);
		this->DeviceFree(p_lCSTACallRefId, l_strDeviceNum);
	}
}
void CAvayaEvent::ConnectClearedEvent(HLINE p_hLine, HCALL p_hCall, long p_lCSTACallRefId)
{
	boost::shared_ptr<CTapiLine> l_pLineDevice = CDeviceManager::Instance()->FindLineByHandle(p_hLine);
	if (!l_pLineDevice)
	{
		ICC_LOG_WARNING(m_pLog, "ConnectClearedEvent not find device by line handle: [%u]", p_hLine);

		return;
	}

	CDeviceManager::Instance()->ProcessCallHandle(p_hLine, p_hCall, FREE_CALL);

	std::string l_strDeviceNum = l_pLineDevice->GetDeviceNum();
	this->DeviceHangup(p_lCSTACallRefId, l_strDeviceNum, l_strDeviceNum);
	this->DeviceFree(p_lCSTACallRefId, l_strDeviceNum);

	ICC_LOG_WARNING(m_pLog, "---123------------------ConnectClearedEvent");
	this->CheckCallOver(p_lCSTACallRefId);
}

//////////////////////////////////////////////////////////////////////////
void CAvayaEvent::CallIncoming(long p_lCSTACallRefId, const std::string& p_strCallerID,
	const std::string& p_strCalledID, const std::string& p_strAlteringDevice)
{
	std::string strShowMsg = m_pStrUtil->Format("Device: [%s] Call Incoming, CallerId: [%s], CalledId: [%s], CallRefID: [%u]",
		p_strAlteringDevice.c_str(), p_strCallerID.c_str(), p_strCalledID.c_str(), p_lCSTACallRefId);
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);
	//Device 110: Call Incoming, CallerId=7002, CalledId=110, CallRefID=483

	std::string l_strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(p_lCSTACallRefId, l_strCTICallRefId))
	{
		l_strCTICallRefId = CCallManager::Instance()->CreateNewCall(p_lCSTACallRefId);
	}

	bool bIsBlackCall = false;
	if (CBlackListManager::Instance()->FindBlackNumber(p_strCallerID))
	{
		bIsBlackCall = true;

		IAnswerCallNotifPtr l_pRequestNotif = boost::make_shared<CAnswerCallNotif>();
		if (l_pRequestNotif)
		{
			l_pRequestNotif->SetCSTACallRefId(p_lCSTACallRefId);
			l_pRequestNotif->SetCTICallRefId(l_strCTICallRefId);
			l_pRequestNotif->SetDeviceNum(p_strAlteringDevice);
			l_pRequestNotif->SetCallerId(p_strCallerID);
			l_pRequestNotif->SetCalledId(p_strCalledID);

			long l_lTaskId = CTaskManager::Instance()->AddCmdTask(Task_RefuseBlackCall, l_pRequestNotif);
			ICC_LOG_DEBUG(m_pLog, "AddCmdTask BlackListCall RefuseAnswer, NewTaskId: [%u], CallerId: [%s], CalledId: [%s]",
				l_lTaskId, p_strCallerID.c_str(), p_strCalledID.c_str());
		}
		else
		{
			ICC_LOG_FATAL(m_pLog, "Create CAnswerCallNotif Object Failed !!!");
		}
	}

	std::string l_strIncomingTime = CCallManager::Instance()->GetIncomingTime(p_lCSTACallRefId);
	if (l_strIncomingTime.empty())
	{
		//	�ŶӺ��ٷ��䣬������Ӧ�ò㷢�ͺ���״̬
		std::vector<CProperty> l_vPropertieQueue;
		//SetPropertyValue(l_vPropertieQueue, Pr_AgentId, "");
		SetPropertyValue(l_vPropertieQueue, Pr_ACDGrp, p_strCalledID);
		SetPropertyValue(l_vPropertieQueue, Pr_CTICallRefId, l_strCTICallRefId);
		SetPropertyValue(l_vPropertieQueue, Pr_CallerId, p_strCallerID);
		//SetPropertyValue(l_vPropertieQueue, Pr_CalledId, "");
		SetPropertyValue(l_vPropertieQueue, Pr_IsAlarmCall, Result_True);
		SetPropertyValue(l_vPropertieQueue, Pr_IsBlackCall, bIsBlackCall ? Result_True : Result_False);
		SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_INCOMING]);
		SetPropertyValue(l_vPropertieQueue, Pr_CallDirection, CallDirectionString[CALL_DIRECTION_IN]);
		CCallManager::Instance()->UpdateCall(p_lCSTACallRefId, l_vPropertieQueue);

		CDeviceManager::Instance()->UpdateLogicalCallState(p_lCSTACallRefId, p_strAlteringDevice, CallStateString[STATE_INCOMING]);
		CCallManager::Instance()->PostCallState(p_lCSTACallRefId);
	}
}

void CAvayaEvent::CallWaiting(long p_lCSTACallRefId, const std::string& p_strCallerID,
	const std::string& p_strCalledID, const std::string& p_strAlteringDevice)
{
	std::string strShowMsg = m_pStrUtil->Format("Device: [%s] Call Waiting, CallerId: [%s], CalledId: [%s], CallRefID: [%u]",
		p_strAlteringDevice.c_str(), p_strCallerID.c_str(), p_strCalledID.c_str(), p_lCSTACallRefId);
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	std::string strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(p_lCSTACallRefId, strCTICallRefId))
	{
		strCTICallRefId = CCallManager::Instance()->CreateNewCall(p_lCSTACallRefId);
	}
	std::vector<CProperty> l_vPropertieQueue;
	//SetPropertyValue(l_vPropertieQueue, Pr_AgentId, "");
	SetPropertyValue(l_vPropertieQueue, Pr_ACDGrp, p_strCalledID);
	SetPropertyValue(l_vPropertieQueue, Pr_CTICallRefId, strCTICallRefId);
	SetPropertyValue(l_vPropertieQueue, Pr_CallerId, p_strCallerID);
	//SetPropertyValue(l_vPropertieQueue, Pr_CalledId, "");
	//	SetPropertyValue(l_vPropertieQueue, Pr_IsBlackCall, /*bIsBlackCall ? Result_True :*/ Result_False);
	SetPropertyValue(l_vPropertieQueue, Pr_IsAlarmCall, Result_True);
	SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_QUEUE]);
	SetPropertyValue(l_vPropertieQueue, Pr_CallDirection, CallDirectionString[CALL_DIRECTION_IN]);
	CCallManager::Instance()->UpdateCall(p_lCSTACallRefId, l_vPropertieQueue);

	CDeviceManager::Instance()->UpdateLogicalCallState(p_lCSTACallRefId, p_strAlteringDevice, CallStateString[STATE_QUEUE]);
	CCallManager::Instance()->PostCallState(p_lCSTACallRefId);
}

void CAvayaEvent::CallAssign(long p_lCSTACallRefId, const std::string& p_strCallerID,
	const std::string& p_strCalledID, const std::string& p_strAlteringDevice)
{
	std::string strShowMsg = m_pStrUtil->Format("Device: [%s] Call Assign, CallerId: [%s], CalledId: [%s], CallRefID: [%u]",
		p_strAlteringDevice.c_str(), p_strCallerID.c_str(), p_strCalledID.c_str(), p_lCSTACallRefId);
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);
	//Device 7004: Call Assign, CallerId=7002, CalledId=110, CallRefID=483

	std::string strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(p_lCSTACallRefId, strCTICallRefId))
	{
		strCTICallRefId = CCallManager::Instance()->CreateNewCall(p_lCSTACallRefId);
	}
	std::vector<CProperty> l_vPropertieQueue;
	SetPropertyValue(l_vPropertieQueue, Pr_AgentId, p_strAlteringDevice);
	SetPropertyValue(l_vPropertieQueue, Pr_ACDGrp, p_strCalledID);
	SetPropertyValue(l_vPropertieQueue, Pr_CTICallRefId, strCTICallRefId);
	SetPropertyValue(l_vPropertieQueue, Pr_CallerId, p_strCallerID);
	SetPropertyValue(l_vPropertieQueue, Pr_CalledId, p_strAlteringDevice);
	SetPropertyValue(l_vPropertieQueue, Pr_IsAlarmCall, Result_True);
	//SetPropertyValue(l_vPropertieQueue, Pr_IsBlackCall, /*bIsBlackCall ? Result_True :*/ Result_False);
	SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_ASSIGN]);
	SetPropertyValue(l_vPropertieQueue, Pr_CallDirection, CallDirectionString[CALL_DIRECTION_IN]);
	CCallManager::Instance()->UpdateCall(p_lCSTACallRefId, l_vPropertieQueue);

	CDeviceManager::Instance()->UpdateLogicalCallState(p_lCSTACallRefId, p_strAlteringDevice, CallStateString[STATE_ASSIGN]);
	CCallManager::Instance()->PostCallState(p_lCSTACallRefId);
}

void CAvayaEvent::DeviceOffHook(long p_lCSTACallRefId, long p_lRelatedCSTACallRefId, const std::string& p_strOffHookDevice)
{
	std::string strShowMsg = m_pStrUtil->Format("Device: [%s] OffHook, CallerId: [%s], CallRefID: [%u]",
		p_strOffHookDevice.c_str(), p_strOffHookDevice.c_str(), p_lCSTACallRefId);
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	std::string strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(p_lCSTACallRefId, strCTICallRefId))
	{
		strCTICallRefId = CCallManager::Instance()->CreateNewCall(p_lCSTACallRefId);
	}
	std::vector<CProperty> l_vPropertieQueue;
	//SetPropertyValue(l_vPropertieQueue, Pr_AgentId, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_ACDGrp, "");
	SetPropertyValue(l_vPropertieQueue, Pr_CallerId, p_strOffHookDevice);
	//SetPropertyValue(l_vPropertieQueue, Pr_CalledId, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_IsBlackCall, Result_False);
	SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_DIAL]);
	//SetPropertyValue(l_vPropertieQueue, Pr_CallDirection, CallDirectionString[CALL_DIRECTION_OUT]);
	CCallManager::Instance()->UpdateCall(p_lCSTACallRefId, l_vPropertieQueue);

	CDeviceManager::Instance()->UpdateLogicalCallState(p_lCSTACallRefId, p_strOffHookDevice, CallStateString[STATE_DIAL]);
	this->PostDeviceState(p_lCSTACallRefId, strCTICallRefId, p_strOffHookDevice, CallStateString[STATE_DIAL]);
}
void CAvayaEvent::CallAlerting(long p_lCSTACallRefId, const std::string& p_strCallerID,
	const std::string& p_strCalledID, const std::string& p_strAlteringDevice)
{
	std::string strShowMsg = m_pStrUtil->Format("Device: [%s] Alerting, CallerId: [%s], CalledId: [%s], CallRefID: [%u]",
		p_strAlteringDevice.c_str(), p_strCallerID.c_str(), p_strCalledID.c_str(), p_lCSTACallRefId);
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	std::string strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(p_lCSTACallRefId, strCTICallRefId))
	{
		strCTICallRefId = CCallManager::Instance()->CreateNewCall(p_lCSTACallRefId);
	}
	std::vector<CProperty> l_vPropertieQueue;
	//SetPropertyValue(l_vPropertieQueue, Pr_AgentId, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_ACDGrp, "");
	SetPropertyValue(l_vPropertieQueue, Pr_CTICallRefId, strCTICallRefId);
	SetPropertyValue(l_vPropertieQueue, Pr_CallerId, p_strCallerID);
	SetPropertyValue(l_vPropertieQueue, Pr_CalledId, p_strAlteringDevice);
	//SetPropertyValue(l_vPropertieQueue, Pr_IsBlackCall, /*bIsBlackCall ? Result_True :*/ Result_False);
	SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_RING]);
	//SetPropertyValue(l_vPropertieQueue, Pr_CallDirection, CallDirectionString[CALL_DIRECTION_IN]);	//�ڲ��������������յ������¼����յ������¼�
	CCallManager::Instance()->UpdateCall(p_lCSTACallRefId, l_vPropertieQueue);

	CDeviceManager::Instance()->UpdateLogicalCallState(p_lCSTACallRefId, p_strAlteringDevice, CallStateString[STATE_RING]);
	this->PostDeviceState(p_lCSTACallRefId, strCTICallRefId, p_strAlteringDevice, CallStateString[STATE_RING]);
}

void CAvayaEvent::CallAlteringBack(long p_lCSTACallRefId, long p_lRelatedCSTACallRefId, const std::string& p_strCallerID,
	const std::string& p_strCalledID, const std::string& p_strAlteringDevice)
{
	std::string strShowMsg = m_pStrUtil->Format("Device: [%s] AlertingBack, CallRefID: [%u], RelatedCallRefID: [%u], CallerId: [%s], CalledId: [%s]",
		p_strAlteringDevice.c_str(), p_lCSTACallRefId, p_lRelatedCSTACallRefId, p_strCallerID.c_str(), p_strCalledID.c_str());
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	std::string l_strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(p_lCSTACallRefId, l_strCTICallRefId))
	{
		l_strCTICallRefId = CCallManager::Instance()->CreateNewCall(p_lCSTACallRefId);
	}
	std::vector<CProperty> l_vPropertieQueue;
	//SetPropertyValue(l_vPropertieQueue, Pr_AgentId, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_ACDGrp, "");
	SetPropertyValue(l_vPropertieQueue, Pr_CTICallRefId, l_strCTICallRefId);
	SetPropertyValue(l_vPropertieQueue, Pr_CallerId, p_strAlteringDevice/*p_strCallerID*/);//
	//SetPropertyValue(l_vPropertieQueue, Pr_CalledId, p_strCalledID);	// 110 ���룬�˴��� Called Ϊ 110
	//SetPropertyValue(l_vPropertieQueue, Pr_IsBlackCall, /*bIsBlackCall ? Result_True :*/ Result_False);
	SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_SIGNALBACK]);
	//SetPropertyValue(l_vPropertieQueue, Pr_CallDirection, CallDirectionString[CALL_DIRECTION_OUT]);
	if (!CCallManager::Instance()->IsAlarmCall(p_lCSTACallRefId))
	{
		SetPropertyValue(l_vPropertieQueue, Pr_CalledId, p_strCalledID);	// 110 ���룬�˴��� Called Ϊ 110
	}
	CCallManager::Instance()->UpdateCall(p_lCSTACallRefId, l_vPropertieQueue);

	CDeviceManager::Instance()->UpdateLogicalCallState(p_lCSTACallRefId, p_strAlteringDevice, CallStateString[STATE_SIGNALBACK]);
	this->PostDeviceState(p_lCSTACallRefId, l_strCTICallRefId, p_strAlteringDevice, CallStateString[STATE_SIGNALBACK]);
}

void CAvayaEvent::CallTalking(long p_lCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID, const std::string& p_strTalkingDevice)
{
	std::string strShowMsg = m_pStrUtil->Format("Device: [%s] talking, CallerId: [%s], CalledId: [%s], CallRefID: [%u]",
		p_strTalkingDevice.c_str(), p_strCallerID.c_str(), p_strCalledID.c_str(), p_lCSTACallRefId);
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	std::string l_strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(p_lCSTACallRefId, l_strCTICallRefId))
	{
		l_strCTICallRefId = CCallManager::Instance()->CreateNewCall(p_lCSTACallRefId);
	}

	std::string l_strCallerId = p_strCallerID;
	std::string l_strCalledId = p_strCalledID;
	bool bIsTransferCall = CCallManager::Instance()->GetIsTransferCall(p_lCSTACallRefId);
	std::string strOriginalCallerId = CCallManager::Instance()->GetOriginalCallerId(p_lCSTACallRefId);
	std::string strOriginalCalledId = CCallManager::Instance()->GetOriginalCalledId(p_lCSTACallRefId);
	if (bIsTransferCall && !strOriginalCallerId.empty())
	{
		l_strCallerId = strOriginalCallerId;
	}
	/*if (!strOriginalCalledId.empty())
	{
	l_strCalledId = strOriginalCalledId;
	}*/

	std::vector<CProperty> l_vPropertieQueue;
	//SetPropertyValue(l_vPropertieQueue, Pr_AgentId, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_ACDGrp, "");
	SetPropertyValue(l_vPropertieQueue, Pr_CTICallRefId, l_strCTICallRefId);
	SetPropertyValue(l_vPropertieQueue, Pr_CallerId, l_strCallerId);
	//SetPropertyValue(l_vPropertieQueue, Pr_CalledId, l_strCalledId);
	//SetPropertyValue(l_vPropertieQueue, Pr_IsBlackCall, /*bIsBlackCall ? Result_True :*/ Result_False);
	SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_TALK]);
	//SetPropertyValue(l_vPropertieQueue, Pr_CallDirection, CallDirectionString[CALL_DIRECTION_IN]);
	CCallManager::Instance()->UpdateCall(p_lCSTACallRefId, l_vPropertieQueue);

	std::string l_strLogicCallState = CDeviceManager::Instance()->GetLogicalCallState(p_lCSTACallRefId, p_strTalkingDevice);
	CDeviceManager::Instance()->UpdateLogicalCallState(p_lCSTACallRefId, p_strTalkingDevice, CallStateString[STATE_TALK]);
	if (l_strLogicCallState.compare(CallStateString[STATE_HOLD]) == 0)
	{
		//	ȡ�����֣����ͻ��˷���STATE_TALK����������һ������.....
		this->PostDeviceState(p_lCSTACallRefId, l_strCTICallRefId, p_strTalkingDevice, CallStateString[STATE_CANCELHOLD]);
	}
	else
	{
		if (p_strCallerID.compare(p_strTalkingDevice) != 0)
		{
			//���ùһ����ԣ���Ҫ���ھܽӺ����·��䣬������ֹһ�����Ϊ�����͡��Ĵ���
			CCallManager::Instance()->ResetHangupAttribute(p_lCSTACallRefId);
		}

		this->PostDeviceState(p_lCSTACallRefId, l_strCTICallRefId, p_strTalkingDevice, CallStateString[STATE_TALK]);
	}
	
}
void CAvayaEvent::CallConference(long p_lCSTACallRefId, const std::string& p_strConferenceDevice)
{
	std::string strShowMsg = m_pStrUtil->Format("Device: [%s] conference, CallRefId: [%u]",
		p_strConferenceDevice.c_str(), p_lCSTACallRefId);
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	long lNewCSTACallRefId = p_lCSTACallRefId;
	long lOldCSTACallRefId = p_lCSTACallRefId;

	std::string l_strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(p_lCSTACallRefId, l_strCTICallRefId))
	{
		l_strCTICallRefId = CCallManager::Instance()->CreateNewCall(p_lCSTACallRefId);
	}
	std::vector<CProperty> l_vPropertieQueue;
	/*SetPropertyValue(l_vPropertieQueue, Pr_AgentId, "");
	SetPropertyValue(l_vPropertieQueue, Pr_ACDGrp, "");
	SetPropertyValue(l_vPropertieQueue, Pr_CallerId, "");
	SetPropertyValue(l_vPropertieQueue, Pr_CalledId, "");
	SetPropertyValue(l_vPropertieQueue, Pr_IsBlackCall, bIsBlackCall ? Result_True : Result_False);*/
	SetPropertyValue(l_vPropertieQueue, Pr_CTICallRefId, l_strCTICallRefId);

	if (lNewCSTACallRefId == lOldCSTACallRefId)
	{
		SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_TALK]);
	}
	else
	{
		SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_CONFERENCE]);
	}

	SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_CONFERENCE]);
	//	SetPropertyValue(l_vPropertieQueue, Pr_CallDirection, CallDirectionString[CALL_DIRECTION_IN]);
//	CCallManager::Instance()->AddConferenceParty(p_lCSTACallRefId, p_strConferenceDevice);
	CCallManager::Instance()->UpdateCall(p_lCSTACallRefId, l_vPropertieQueue);

	CCallManager::Instance()->AddConferenceParty(p_lCSTACallRefId, p_strConferenceDevice);

	//CDeviceManager::Instance()->UpdateLogicalCallState(p_lCSTACallRefId, p_strConferenceDevice, CallStateString[STATE_CONFERENCE]);
	bool l_bPostState = true;
	std::string l_strCallState;
	if (lNewCSTACallRefId == lOldCSTACallRefId)
	{
		int nRes = CDeviceManager::Instance()->UpdateLogicalCallState(lNewCSTACallRefId, p_strConferenceDevice, CallStateString[STATE_TALK]);
		if (nRes == 1) //֮ǰ״̬��ΪSTATE_TALK������Ҫ֪ͨ
		{
			l_bPostState = false;
		}
		l_strCallState = CallStateString[STATE_TALK];
	}
	else
	{
		l_strCallState = CallStateString[STATE_CONFERENCE];
		CDeviceManager::Instance()->UpdateLogicalCallState(lNewCSTACallRefId, p_strConferenceDevice, CallStateString[STATE_CONFERENCE]);
	}

	if (l_bPostState)
	{
		this->PostDeviceState(lNewCSTACallRefId, l_strCTICallRefId, p_strConferenceDevice, l_strCallState);
	}
}

void CAvayaEvent::CallSilentMonitor(long p_lCSTACallRefId, const std::string& p_strMonitorDevice)
{
	std::string strShowMsg = m_pStrUtil->Format("Device: [%s] SilentMonitor, CallRefID: [%u]",
		p_strMonitorDevice.c_str(), p_lCSTACallRefId);
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	std::string l_strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(p_lCSTACallRefId, l_strCTICallRefId))
	{
		l_strCTICallRefId = CCallManager::Instance()->CreateNewCall(p_lCSTACallRefId);
	}
	std::vector<CProperty> l_vPropertieQueue;
	//SetPropertyValue(l_vPropertieQueue, Pr_AgentId, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_ACDGrp, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_CallerId, strCallerID);
	//SetPropertyValue(l_vPropertieQueue, Pr_CalledId, strCalledID);
	//SetPropertyValue(l_vPropertieQueue, Pr_IsBlackCall, /*bIsBlackCall ? Result_True :*/ Result_False);
	SetPropertyValue(l_vPropertieQueue, Pr_CTICallRefId, l_strCTICallRefId);
	SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_SILENTMONITOR]);
	//SetPropertyValue(l_vPropertieQueue, Pr_CallDirection, CallDirectionString[CALL_DIRECTION_IN]);
	CCallManager::Instance()->UpdateCall(p_lCSTACallRefId, l_vPropertieQueue);

	CDeviceManager::Instance()->UpdateLogicalCallState(p_lCSTACallRefId, p_strMonitorDevice, CallStateString[STATE_SILENTMONITOR]);
	this->PostDeviceState(p_lCSTACallRefId, l_strCTICallRefId, p_strMonitorDevice, CallStateString[STATE_SILENTMONITOR]);
}

void CAvayaEvent::CallHold(long p_lCSTACallRefId, const std::string& p_strHoldDevice)
{
	std::string strShowMsg = m_pStrUtil->Format("Device: [%s] HoldCall, CallRefID: [%u]",
		p_strHoldDevice.c_str(), p_lCSTACallRefId);
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	std::string l_strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(p_lCSTACallRefId, l_strCTICallRefId))
	{
		l_strCTICallRefId = CCallManager::Instance()->CreateNewCall(p_lCSTACallRefId);
	}
	std::vector<CProperty> l_vPropertieQueue;
	//SetPropertyValue(l_vPropertieQueue, Pr_AgentId, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_ACDGrp, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_CallerId, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_CalledId, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_IsBlackCall, / *bIsBlackCall ? Result_True :* / Result_False);
	SetPropertyValue(l_vPropertieQueue, Pr_CTICallRefId, l_strCTICallRefId);
	SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_HOLD]);
	//SetPropertyValue(l_vPropertieQueue, Pr_CallDirection, ""/*CallDirectionString[CALL_DIRECTION_IN] */);
	CCallManager::Instance()->UpdateCall(p_lCSTACallRefId, l_vPropertieQueue);

	CDeviceManager::Instance()->UpdateLogicalCallState(p_lCSTACallRefId, p_strHoldDevice, CallStateString[STATE_HOLD]);
	this->PostDeviceState(p_lCSTACallRefId, l_strCTICallRefId, p_strHoldDevice, CallStateString[STATE_HOLD]);
}
void CAvayaEvent::OnHoldPendConf(long p_lCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID)
{
	std::string l_strCTICallRefId = "";
	if (CConferenceManager::Instance()->GetCTICallRefIdByParty(p_strCallerID, l_strCTICallRefId))
	{
		// �������µĻ���ID������ TAPI �����¼��޷�ȷ������֮��Ĺ��������ͨ�������Ա�����ҹ�����ϵ
		long l_lRelatedCSTACallRefId = DEFAULT_CALLREFID;

		if (CCallManager::Instance()->GetCSTACallRefId(l_strCTICallRefId, l_lRelatedCSTACallRefId))
		{
			CCallManager::Instance()->CopyCall(p_lCSTACallRefId, l_lRelatedCSTACallRefId);
			CCallManager::Instance()->DeleteCall(l_lRelatedCSTACallRefId);
		}
	}
}

void CAvayaEvent::CallRetrieve(long p_lCSTACallRefId, const std::string& p_strRetrieveDevice)
{
	std::string strShowMsg = m_pStrUtil->Format("Device: [%s] RetrieveCall, CallRefID: [%u]",
		p_strRetrieveDevice.c_str(), p_lCSTACallRefId);
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	std::string l_strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(p_lCSTACallRefId, l_strCTICallRefId))
	{
		l_strCTICallRefId = CCallManager::Instance()->CreateNewCall(p_lCSTACallRefId);
	}
	std::vector<CProperty> l_vPropertieQueue;
	//SetPropertyValue(l_vPropertieQueue, Pr_AgentId, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_ACDGrp, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_CallerId, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_CalledId, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_IsBlackCall, / *bIsBlackCall ? Result_True :* / Result_False);
	SetPropertyValue(l_vPropertieQueue, Pr_CTICallRefId, l_strCTICallRefId);
	SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_TALK]);
	//SetPropertyValue(l_vPropertieQueue, Pr_CallDirection, ""/*CallDirectionString[CALL_DIRECTION_IN] */);
	CCallManager::Instance()->UpdateCall(p_lCSTACallRefId, l_vPropertieQueue);

	CDeviceManager::Instance()->UpdateLogicalCallState(p_lCSTACallRefId, p_strRetrieveDevice, CallStateString[STATE_CANCELHOLD]);
	this->PostDeviceState(p_lCSTACallRefId, l_strCTICallRefId, p_strRetrieveDevice, CallStateString[STATE_CANCELHOLD]);
}

void CAvayaEvent::DeviceHangup(long p_lCSTACallRefId, const std::string& p_strHangupDevice, const std::string& p_strMonitorDevice, const std::string& p_strHangupType)
{
	std::string strShowMsg = m_pStrUtil->Format("Device: [%s]-[%s] Hangup, CallRefID: [%u],HangupType: [%s]",
		p_strMonitorDevice.c_str(), p_strHangupDevice.c_str(), p_lCSTACallRefId, p_strHangupType.c_str());
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	std::string l_strCTICallRefId = "";
	if (CCallManager::Instance()->GetCallByCSTACallRefId(p_lCSTACallRefId, l_strCTICallRefId))
	{
		if (p_strHangupType.compare(HangupTypeString[HANGUP_TYPE_TIMEOUT]) == 0 ||
			p_strHangupType.compare(HangupTypeString[HANGUP_TYPE_TRANSFER]) == 0 ||
			p_strHangupType.compare(HangupTypeString[HANGUP_TYPE_BLACKCALL]) == 0)
		{
			// ���峬ʱ �� ����ת�ƣ�ԭ���񣩡��������ܽ�
			CCallManager::Instance()->SetHangupState(p_lCSTACallRefId, p_strHangupDevice, p_strHangupType);
		}
		else if (p_strHangupType.compare(HangupTypeString[HANGUP_TYPE_TP_TRANSFER]) == 0 ||
			p_strHangupType.compare(HangupTypeString[HANGUP_TYPE_OP_TRANSFER]) == 0)
		{
			//	ת�Ʒ��𷽹һ����������»���
			//	ת�ƣ�����ƫת����������
		}
		else
		{
			std::string strDeviceType = CDeviceManager::Instance()->GetDeviceType(p_strHangupDevice);
			if (strDeviceType.compare(DEVICE_TYPE_ACDGROUP) != 0 && strDeviceType.compare(DEVICE_TYPE_ACDSUPER) != 0)
			{
				CCallManager::Instance()->ProcessDeviceHangup(p_lCSTACallRefId, p_strHangupDevice);
			}

			if (strDeviceType.compare(DEVICE_TYPE_ACDGROUP) == 0 || strDeviceType.compare(DEVICE_TYPE_ACDSUPER) == 0)
			{
				if (!CDeviceManager::Instance()->FindLogicalCallExcludeDevice(p_lCSTACallRefId, p_strHangupDevice))
				{
					// �Ŷ�δ���䣬���йһ�
					CCallManager::Instance()->SetHangupState(p_lCSTACallRefId, p_strHangupDevice, HangupTypeString[HANGUP_TYPE_RELEASE]);
				}
			}
		}

		if (CCallManager::Instance()->GetCallMode(p_lCSTACallRefId).compare(CallModeString[CALL_MODE_CONFERENCE]) == 0)
		{
			this->ConferenceHangup(p_lCSTACallRefId, l_strCTICallRefId, p_strHangupDevice);
		}

		if (p_strMonitorDevice.compare(p_strHangupDevice) == 0)
		{
			CDeviceManager::Instance()->UpdateLogicalCallState(p_lCSTACallRefId, p_strHangupDevice, CallStateString[STATE_HANGUP]);
			this->PostDeviceState(p_lCSTACallRefId, l_strCTICallRefId, p_strHangupDevice, CallStateString[STATE_HANGUP]);
		}
	}
	else
	{
		ICC_LOG_WARNING(m_pLog, "DeviceHangup, Call CSTACallRefId: [%u] Not Exist !", p_lCSTACallRefId);
	}
}
void CAvayaEvent::DeviceFree(long p_lCSTACallRefId, const std::string& p_strFreeDevice)
{
	std::string strShowMsg = m_pStrUtil->Format("Device: [%s] Free, CallRefID: [%u]",
		p_strFreeDevice.c_str(), p_lCSTACallRefId);
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	std::string l_strCTICallRefId = "";
	if (CCallManager::Instance()->GetCallByCSTACallRefId(p_lCSTACallRefId, l_strCTICallRefId))
	{
		//	������ϣ��豸����
		CDeviceManager::Instance()->UpdateLogicalCallState(p_lCSTACallRefId, p_strFreeDevice, CallStateString[STATE_FREE]);
		CDeviceManager::Instance()->DeleteLogicalCallState(p_lCSTACallRefId, p_strFreeDevice);
		this->PostDeviceState(p_lCSTACallRefId, l_strCTICallRefId, p_strFreeDevice, CallStateString[STATE_FREE]);

	//	this->CheckCallOver(p_lCSTACallRefId, l_strCTICallRefId);
	}
	else
	{
		ICC_LOG_WARNING(m_pLog, "DeviceFree, Call CSTACallRefId: [%u] Not Exist !", p_lCSTACallRefId);
	}
}


void CAvayaEvent::ConferenceHangup(long p_lCSTACallRefId, const std::string& p_strCTICallRefId, const std::string& p_strHangupDevice)
{
	if (CCallManager::Instance()->DeleteConferenceParty(p_lCSTACallRefId, p_strHangupDevice))
	{
		//ttttt
		//CConferenceManager::Instance()->CR_DeleteConferenceParty(p_lCSTACallRefId, p_strCTICallRefId, p_strHangupDevice);

		IDeviceStateNotifPtr l_pHangupStateNotif = boost::make_shared<CDeviceStateNotif>();
		if (l_pHangupStateNotif)
		{
			l_pHangupStateNotif->SetDeviceNum(p_strHangupDevice);
			l_pHangupStateNotif->SetDeviceState(CallStateString[STATE_HANGUP]);
			l_pHangupStateNotif->SetCSTACallRefId(p_lCSTACallRefId);
			l_pHangupStateNotif->SetCTICallRefId(p_strCTICallRefId);
			l_pHangupStateNotif->SetStateTime(m_pDateTime->CurrentDateTimeStr());

			long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_ConferenceHangupEvent, l_pHangupStateNotif);
			ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask ConferenceHangup, DeviceNum: [%s], TaskId: [%u]",
				p_strHangupDevice.c_str(), l_lTaskId);
		}
		else
		{
			ICC_LOG_FATAL(m_pLog, "Create CDeviceStateNotif Object Failed !!!");
		}
	}
}

void CAvayaEvent::ForcePopCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor)
{
	IForcePopCallNotifPtr l_pRequestNotif = boost::make_shared<CForcePopCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetTarget(p_strSponsor);

		long l_lTaskId = CTaskManager::Instance()->AddCmdTask(Task_ForcePopCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], ForcePopCall, CallRefId [%s], Sponsor [%s], Target [%s]",
			l_lTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strSponsor.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CForcePopCallNotif Object Failed !!!");
	}
}
void CAvayaEvent::CheckCallOver(long p_lCSTACallRefId)
{
	ICC_LOG_DEBUG(m_pLog, "--2222222222---- CheckCallOver: [%u]",p_lCSTACallRefId);
	if (!CDeviceManager::Instance()->FindLogicalCall(p_lCSTACallRefId) /*&& CCallManager::Instance()->GetConfMemCount(p_lCSTACallRefId) == 0*/)
	{
		int nCurrMemCount = CCallManager::Instance()->GetConfMemCount(p_lCSTACallRefId);
		if (CCallManager::Instance()->IsReleaseCall(p_lCSTACallRefId))
		{
			//	����֪ͨ
			CCallManager::Instance()->PostReleaseCall(p_lCSTACallRefId);
		}

		ICC_LOG_DEBUG(m_pLog, "--2222222222---- CheckCallOver: [%u],nCurrMemCount=[%d]", p_lCSTACallRefId, nCurrMemCount);
		CCallManager::Instance()->PostCallOverNotif(p_lCSTACallRefId);

		if (CCallManager::Instance()->GetCallMode(p_lCSTACallRefId).compare(CallModeString[CALL_MODE_CONFERENCE]) != 0)
		{
			//	�ǻ��黰��ֱ��ɾ���ڴ��еĸû���
			CCallManager::Instance()->DeleteCall(p_lCSTACallRefId);
		}
		else
		{
			if (CCallManager::Instance()->GetConfMemCount(p_lCSTACallRefId) == 0)
			{
				//	�ڴ��еĻ���������ɾ�����ϲ���ܻ�ִ������������ɾ���˽��Ҳ�����Ӧ�� CSTACallRefId
				//	�������δ���ߵĻ����Ա������ǿ�����ɾ��

				std::vector<CProperty> l_vPropertieQueue;
				SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_HANGUP]);
				CCallManager::Instance()->UpdateCall(p_lCSTACallRefId, l_vPropertieQueue);
			}
		}
	}
}

void CAvayaEvent::SetAgentStateConf(long p_lRequestId, bool p_bResullt, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ISetAgentStateResultNotifPtr l_pResultNotif = boost::make_shared<CSetAgentStateResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(p_bResullt);
		l_pResultNotif->SetErrorCode(p_nErrorCode);
		l_pResultNotif->SetErrorMsg(p_strErrorMsg);

		long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_SetAgentStateConf, l_pResultNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask SetAgentStateConf, NewTaskId: [%u], RequestId: [%u]",
			l_lTaskId, p_lRequestId);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSetAgentStateResultNotif Object Failed !!!");
	}
}
void CAvayaEvent::MakeCallConf(long p_lRequestId, bool p_bResullt, HCALL p_hCall, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	::Sleep(50);				// ��ȴ�������ȡ���Ļ���IDֵΪ 0

	long l_lCSTACallRefId = DEFAULT_CALLREFID;
	std::string l_strCTICallRefId = "";
	LINECALLINFO* l_pCallInfo = NULL;
	CDeviceManager::Instance()->LoopLineGetCallInfo(p_hCall, l_pCallInfo);
	if (l_pCallInfo)
	{
		l_lCSTACallRefId = l_pCallInfo->dwCallID;
		if (!CCallManager::Instance()->GetCallByCSTACallRefId(l_lCSTACallRefId, l_strCTICallRefId))
		{
			l_strCTICallRefId = CCallManager::Instance()->CreateNewCall(l_lCSTACallRefId);
		}

		delete[] l_pCallInfo;
		l_pCallInfo = NULL;
	}

	IMakeCallResultNotifPtr l_pMakeCallResultNotif = boost::make_shared<CMakeCallResultNotif>();
	if (l_pMakeCallResultNotif)
	{
		l_pMakeCallResultNotif->SetRequestId(p_lRequestId);
		l_pMakeCallResultNotif->SetResult(p_bResullt);
		l_pMakeCallResultNotif->SetCSTACallRefId(l_lCSTACallRefId);
		l_pMakeCallResultNotif->SetCTICallRefId(l_strCTICallRefId);
		l_pMakeCallResultNotif->SetErrorCode(p_nErrorCode);
		l_pMakeCallResultNotif->SetErrorMsg(p_strErrorMsg);

		long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_MakeCallConf, l_pMakeCallResultNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask MakeCallConf, NewTaskId: [%u], RequestId: [%u]",
			l_lTaskId, p_lRequestId);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CMakeCallResultNotif Object Failed !!!");
	}
}
void CAvayaEvent::ConsultationCallConf(long p_lRequestId, bool p_bResullt, HCALL p_hCall, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	::Sleep(500);				// ��ȴ�������ȡ���Ļ���IDֵΪ 0

	std::string l_strCTICallRefId = "";
	std::string l_strRelatedCTICallRefId = "";

	if (p_hCall)
	{
		LINECALLINFO* l_pCallInfo = NULL;
		CDeviceManager::Instance()->LoopLineGetCallInfo(p_hCall, l_pCallInfo);
		if (l_pCallInfo)
		{
			long l_lCSTACallRefId = l_pCallInfo->dwCallID;
			long l_lRelatedCSTACallRefId = l_pCallInfo->dwRelatedCallID;

			ICC_LOG_DEBUG(m_pLog, "ConsultationCallConf, RequestId: [%u], CSTACallRefId: [%u], RelatedCSTACallRefId: [%u]",
				p_lRequestId, l_lCSTACallRefId, l_lRelatedCSTACallRefId);

			if (!CCallManager::Instance()->GetCallByCSTACallRefId(l_lCSTACallRefId, l_strCTICallRefId))
			{
				l_strCTICallRefId = CCallManager::Instance()->CreateNewCall(l_lCSTACallRefId);
			}

			if (l_lRelatedCSTACallRefId != 0)
			{
				CCallManager::Instance()->AddRelatedCSTACallRefId(l_lCSTACallRefId, l_lRelatedCSTACallRefId);

				if (CCallManager::Instance()->GetCallByCSTACallRefId(l_lRelatedCSTACallRefId, l_strRelatedCTICallRefId))
				{
					if (CConferenceManager::Instance()->FindConferenceByCTICallRefId(l_strRelatedCTICallRefId))
					{
						CConferenceManager::Instance()->SetActiveCallRefId(l_strRelatedCTICallRefId, l_strCTICallRefId);
					}
					else if (CTransferCallManager::Instance()->FindTransferCallByHeldCallRefId(l_strRelatedCTICallRefId))
					{
						CTransferCallManager::Instance()->SetActiveCallRefId(l_strRelatedCTICallRefId, l_strCTICallRefId);
					}
					else
					{
						//
					}
				}
			}


			delete[] l_pCallInfo;
			l_pCallInfo = NULL;
		}
		else
		{
			ICC_LOG_WARNING(m_pLog, "ConsultationCallConf CallInfo Null, RequestId: [%u]", p_lRequestId);
		}
	}
	else
	{
		ICC_LOG_WARNING(m_pLog, "ConsultationCallConf HCALL Null, RequestId: [%u]", p_lRequestId);
	}


	IConsultationCallResultNotifPtr l_pResultNotif = boost::make_shared<CConsultationCallResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetHeldCTICallRefId(l_strRelatedCTICallRefId);
		l_pResultNotif->SetActiveCTICallRefId(l_strCTICallRefId);
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(p_bResullt);

		long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_ConsultationCallConf, l_pResultNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask ConsultationCallConf, NewTaskId: [%u], RequestId: [%u]",
			l_lTaskId, p_lRequestId);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CConsultationCallResultNotif Object Failed !!!");
	}
}
void CAvayaEvent::TransferCallConf(long p_lRequestId, bool p_bResullt, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ITransferCallResultNotifPtr l_pResultNotif = boost::make_shared<CTransferCallResultNotif>();
	if (l_pResultNotif)
	{
		l_pResultNotif->SetRequestId(p_lRequestId);
		l_pResultNotif->SetResult(p_bResullt);

		long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_TransferCallConf, l_pResultNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask TransferCallConf, NewTaskId: [%u], RequestId: [%u]",
			l_lTaskId, p_lRequestId);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create TransferCallResultNotif Object Failed !!!");
	}
}

void CAvayaEvent::PostDeviceState(long p_lCSTACallRefId, const std::string& p_strCTICallRefId, const std::string& p_strDeviceNum, const std::string& p_strState)
{
	IDeviceStateNotifPtr l_pDeviceNotif = boost::make_shared<CDeviceStateNotif>();
	if (l_pDeviceNotif)
	{
		std::string l_strDeviceType = CDeviceManager::Instance()->GetDeviceType(p_strDeviceNum);
		if (l_strDeviceType.compare(DEVICE_TYPE_ACDGROUP) != 0)
		{
			std::string l_strCTICallRefId = "";
			std::string l_strCallerId = "";
			std::string l_strCalledParty = "";
			std::string l_strOriginalCallerId = "";
			std::string l_strOriginalCalledId = "";
			std::string l_strCallDirection = "";
			CCallManager::Instance()->GetCallByCSTACallRefId(p_lCSTACallRefId, l_strCTICallRefId, l_strCallerId, l_strCalledParty, l_strOriginalCallerId, l_strOriginalCalledId, l_strCallDirection);

			l_pDeviceNotif->SetCallerId(l_strCallerId);
			l_pDeviceNotif->SetCalledId(l_strCalledParty);
			l_pDeviceNotif->SetOriginalCallerId(l_strOriginalCallerId);
			l_pDeviceNotif->SetOriginalCalledId(l_strOriginalCalledId);
			l_pDeviceNotif->SetCallDirection(l_strCallDirection);
			l_pDeviceNotif->SetDeviceNum(p_strDeviceNum);
			l_pDeviceNotif->SetDeviceType(l_strDeviceType);
			l_pDeviceNotif->SetCSTACallRefId(p_lCSTACallRefId);
			l_pDeviceNotif->SetCTICallRefId(p_strCTICallRefId);
			l_pDeviceNotif->SetDeviceState(p_strState);
			l_pDeviceNotif->SetStateTime(m_pDateTime->CurrentDateTimeStr());

			long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_DeviceStateEvent, l_pDeviceNotif);
			ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask , TaskId: [%u]", l_lTaskId);
		}
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create IDeviceStateNotif Object Failed !!!");
	}
}

void CAvayaEvent::PostDeviceStateEx(long p_lCSTACallRefId, long p_lCSTARelatedCallRefId, const std::string& p_strDeviceNum, const std::string& p_strState)
{
	IDeviceStateNotifPtr l_pDeviceNotif = boost::make_shared<CDeviceStateNotif>();
	if (l_pDeviceNotif)
	{
		std::string l_strDeviceType = CDeviceManager::Instance()->GetDeviceType(p_strDeviceNum);
		if (l_strDeviceType.compare(DEVICE_TYPE_ACDGROUP) != 0)
		{
			std::string l_strCTICallRefId = "";
			std::string l_strCTIRelatedCallRefId = "";
			std::string l_strCallerId = "";
			std::string l_strCalledParty = "";
			std::string l_strOriginalCallerId = "";
			std::string l_strOriginalCalledId = "";
			std::string l_strCallDirection = "";
			CCallManager::Instance()->GetCallByCSTACallRefId(p_lCSTACallRefId, l_strCTICallRefId, l_strCallerId, l_strCalledParty, l_strOriginalCallerId, l_strOriginalCalledId, l_strCallDirection);

			CCallManager::Instance()->GetCallByCSTACallRefId(p_lCSTARelatedCallRefId, l_strCTIRelatedCallRefId);

			l_pDeviceNotif->SetCallerId(l_strCallerId);
			l_pDeviceNotif->SetCalledId(l_strCalledParty);
			l_pDeviceNotif->SetOriginalCallerId(l_strOriginalCallerId);
			l_pDeviceNotif->SetOriginalCalledId(l_strOriginalCalledId);
			l_pDeviceNotif->SetCallDirection(l_strCallDirection);
			l_pDeviceNotif->SetDeviceNum(p_strDeviceNum);
			l_pDeviceNotif->SetDeviceType(l_strDeviceType);
			l_pDeviceNotif->SetCSTACallRefId(p_lCSTACallRefId);
			l_pDeviceNotif->SetCTICallRefId(l_strCTICallRefId);
			//l_pDeviceNotif->SetCTIRelatedCallRefId(l_strCTIRelatedCallRefId);
			l_pDeviceNotif->SetDeviceState(p_strState);
			l_pDeviceNotif->SetStateTime(m_pDateTime->CurrentDateTimeStr());

			long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_DeviceStateEvent, l_pDeviceNotif);
			ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask , TaskId: [%u]", l_lTaskId);
		}
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create IDeviceStateNotif Object Failed !!!");
	}
}

void CAvayaEvent::PostSwitchConf(long p_lIinvokeID, E_TASK_NAME p_nTaskName, bool p_bResullt, int p_nErrorCode, const std::string& p_strErrorMsg)
{
	ISwitchResultNotifPtr l_pSwitchResultNotif = boost::make_shared<CSwitchResultNotif>();
	if (l_pSwitchResultNotif)
	{
		l_pSwitchResultNotif->SetRequestId(p_lIinvokeID);
		l_pSwitchResultNotif->SetResult(p_bResullt);
		l_pSwitchResultNotif->SetErrorCode(p_nErrorCode);
		l_pSwitchResultNotif->SetErrorMsg(p_strErrorMsg);

		long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(p_nTaskName, l_pSwitchResultNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask TaskName: [%s], NewTaskId: [%u], RequestId: [%u]",
			TaskNameString[p_nTaskName].c_str(), l_lTaskId, p_lIinvokeID);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchResultNotif Object Failed !!!");
	}
}
