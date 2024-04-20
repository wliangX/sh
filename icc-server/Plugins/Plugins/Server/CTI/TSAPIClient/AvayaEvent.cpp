/**
 @Copyright Copyright (C), 2018
 @file		AvayaEvent.cpp
 @created	2018/01/18
 @brief		交换机事件接收类
 负责接收所有来自交换机的事件,根据事件类型解析,交给相应处理类
 该类与Avaya交换机相匹配,与AvayaSwitch类配合使用

 @author	psy
 */

#include "Boost.h"

#include "SysConfig.h"
#include "AvayaEvent.h"
#include "AvayaResult.h"
#include "Device.h"
#include "DeviceManager.h"
#include "TaskManager.h"
#include "Property.h"
#include "Call.h"
#include "CallManager.h"
#include "BlackListManager.h"

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
	if (m_pEventInstance)
	{
		m_pEventInstance.reset();
	}
}

void CAvayaEvent::Init(IResourceManagerPtr pResourceManager)
{
	m_pLog = ICCGetResourceEx(Log::ILogFactory, ICCILogFactoryResourceName, pResourceManager)->GetLogger(MODULE_NAME);
	m_pStrUtil = ICCGetResourceEx(StringUtil::IStringFactory, ICCIStringFactoryResourceName, pResourceManager)->CreateString();
	m_pDateTimePtr = ICCGetResourceEx(DateTime::IDateTimeFactory, ICCIDateTimeFactoryResourceName, pResourceManager)->CreateDateTime();
}

//////////////////////////////////////////////////////////////////////////
void CAvayaEvent::DispatchEvent(CSTAEvent_t* pEventData, ATTEvent_t* pATTEvent)
{
	switch (pEventData->eventHeader.eventClass)
	{
	case ACSCONFIRMATION:
		//ACS证实的消息
		// call the handleACSConfirmation function
		// for handling acs information
		HandleACSConfirmation(pEventData);
		break;
	case ACSUNSOLICITED:
		//ACS主动汇报的消息
		HandACSUnsolicited(pEventData);
		break;
	case CSTAREQUEST:
		//CSTA请求消息
		RecCSTARequestEvent(pEventData);
		break;
	case CSTACONFIRMATION:
		//CSTA证实消息
		// call the handleCSTAConfirmation function for processing
		// CSTA Confirmation Events
		HandleCSTAConfirmation(pEventData, pATTEvent);
		break;
	case CSTAUNSOLICITED:
		//收到CSTA主动发送的消息
		// call the handler for processing all the CSTAUNSOLICITED Event.
		HandleCSTAUnsolicited(pEventData, pATTEvent);
		break;
	case CSTAEVENTREPORT:
		//收到CSTA汇报消息
		RecCSTAReportEvent(pEventData);
		break;
	default:
	{
		ICC_LOG_WARNING(m_pLog, "Unknown Event. Event ID: [%d]", pEventData->eventHeader.eventClass);
	}
	break;
	}
}


void CAvayaEvent::HandleACSConfirmation(CSTAEvent_t* pEventData)
{
	EventType_t eventType = pEventData->eventHeader.eventType;
	switch (eventType)
	{
	case ACS_OPEN_STREAM_CONF:
		//ACSOpenStreamConfEvent_t		acsopen;
		OpenStreamConf(pEventData);
		break;
	case ACS_UNIVERSAL_FAILURE_CONF:
		//ACSUniversalFailureConfEvent_t	failureEvent;
		OpenStreamFail(pEventData->event.acsConfirmation.u.failureEvent.error);
		break;
	case ACS_CLOSE_STREAM_CONF:
		CloseStreamConf();
		break;
	default:
		StreamException();
		break;
	}
}

void CAvayaEvent::HandACSUnsolicited(CSTAEvent_t* pEventData)
{
	EventType_t eventType = pEventData->eventHeader.eventType;

	//主动汇报的消息只有ACS_UNIVERSAL_FAILURE消息才有数据结构,
	//故而只处理了ACS_UNIVERSAL_FAILURE消息
	if (eventType != ACS_UNIVERSAL_FAILURE)
	{
		ICC_LOG_WARNING(m_pLog, "AES post message EventType: [%d] not invoke", eventType);

		return;
	}

	//显示错误消息,以后再细分处理.
	ACSUniversalFailure_t error;
	error = pEventData->event.acsUnsolicited.u.failureEvent.error;
	std::string strError = CAvayaResult::Instance()->GetACSUnsoliEventString(error);
	std::string strEventType = CAvayaResult::Instance()->GetEventTypeString(eventType);
	ICC_LOG_ERROR(m_pLog, "AES post error message: [%s], EventType: [%d]-[%s]", 
		strError.c_str(), eventType, strEventType.c_str());

	//重新连接ACS流
	if (error == TSERVER_STREAM_FAILED)
	{
		StreamException();
	}
}


void CAvayaEvent::RecCSTARequestEvent(CSTAEvent_t* pEventData)
{
	ICC_LOG_DEBUG(m_pLog, "Receive CSTARequestEvent: [%s]",
		CAvayaResult::Instance()->GetEventTypeString(pEventData->eventHeader.eventType).c_str());

	switch (pEventData->eventHeader.eventType)
	{
	case CSTA_ROUTE_REQUEST:
		//CSTARouteRequestEvent_t		routeRequest;
		//CSTARouteRequestExtEvent_t	routeRequestExt;
	case CSTA_RE_ROUTE_REQUEST:
		//CSTAReRouteRequest_t		reRouteRequest;
	case CSTA_ESCAPE_SVC_REQ:
		//CSTAEscapeSvcReqEvent_t		escapeSvcReqeust;
	case CSTA_SYS_STAT_REQ:
		//CSTASysStatReqEvent_t		sysStatRequest;	
		break;
		//	路由请求
	case CSTA_ROUTE_REQUEST_EXT:
	{
		// 在重选路由中有用
		RouteRequestExtEvent(pEventData->event.cstaRequest.u.routeRequestExt.routeRegisterReqID,
			pEventData->event.cstaRequest.u.routeRequestExt);
	}
	break;
	default:
		break;
	}
}

void CAvayaEvent::HandleCSTAConfirmation(CSTAEvent_t* pEventData, ATTEvent_t* pATTEvent)
{
	CSTAMonitorCrossRefID_t lMonitorId = 0;

	switch (pEventData->eventHeader.eventType)
	{
	case CSTA_ALTERNATE_CALL_CONF://CSTAAlternateCallConfEvent_t alternateCall;
		AlternateCallConf(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.alternateCall);
		break;
	case CSTA_ANSWER_CALL_CONF://CSTAAnswerCallConfEvent_t answerCall;
		AnswerCallConf(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.answerCall);
		break;
	case CSTA_CALL_COMPLETION_CONF://CSTACallCompletionConfEvent_t callCompletion;
		CallCompletionRet(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.callCompletion);
		break;
	case CSTA_CLEAR_CALL_CONF://CSTAClearCallConfEvent_t clearCall;
		ClearCallConf(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.clearCall);
		break;
	case CSTA_CLEAR_CONNECTION_CONF://CSTAClearConnectionConfEvent_t clearConnection;
		ClearConnectionConf(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.clearConnection);
		break;
	case CSTA_CONFERENCE_CALL_CONF://CSTAConferenceCallConfEvent_t conferenceCall;
		ConferenceConf(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.conferenceCall);
		break;
	case CSTA_CONSULTATION_CALL_CONF://CSTAConsultationCallConfEvent_t consultationCall;
		ConsultationConf(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.consultationCall);
		break;
	case CSTA_DEFLECT_CALL_CONF://CSTADeflectCallConfEvent_t deflectCall;
		DeflectCallConf(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.deflectCall);
		break;
	case CSTA_PICKUP_CALL_CONF://CSTAPickupCallConfEvent_t pickupCall;
		PickupCallConf(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.pickupCall);
		break;
	case CSTA_GROUP_PICKUP_CALL_CONF://CSTAGroupPickupCallConfEvent_t groupPickupCall;
		GroupPickupCallRet(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.groupPickupCall);
		break;
	case CSTA_HOLD_CALL_CONF://CSTAHoldCallConfEvent_t holdCall;
		HoldCallConf(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.holdCall);
		break;
	case CSTA_MAKE_CALL_CONF: //CSTAMakeCallConfEvent_t makeCall;
		MakeCallConf(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.makeCall);
		break;
	case CSTA_MAKE_PREDICTIVE_CALL_CONF://CSTAMakePredictiveCallConfEvent_t makePredictiveCall;
		MakePredictiveCallRet(pEventData->event.cstaConfirmation.invokeID,
			pEventData->event.cstaConfirmation.u.makePredictiveCall);
		break;
	case CSTA_QUERY_MWI_CONF: //CSTAQueryMwiConfEvent_t queryMwi;			
		QueryMwiRet(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.queryMwi);
		break;
	case CSTA_QUERY_DND_CONF: //CSTAQueryDndConfEvent_t	queryDnd;
		QueryDndRet(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.queryDnd);
		break;
	case CSTA_QUERY_FWD_CONF: //CSTAQueryFwdConfEvent_t	queryFwd;
		QueryFwdRet(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.queryFwd);
		break;
	case CSTA_QUERY_AGENT_STATE_CONF: //CSTAQueryAgentStateConfEvent_t queryAgentState;
		QueryAgentStateConf(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.queryAgentState);
		break;
	case CSTA_QUERY_LAST_NUMBER_CONF: //CSTAQueryLastNumberConfEvent_t queryLastNumber;
		QueryLastNumberRet(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.queryLastNumber);
		break;
	case CSTA_QUERY_DEVICE_INFO_CONF: //CSTAQueryDeviceInfoConfEvent_t queryDeviceInfo;
		QueryDeviceInfoConf(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.queryDeviceInfo);
		break;
	case CSTA_RECONNECT_CALL_CONF: //CSTAReconnectCallConfEvent_t reconnectCall;
		ReconnectCallRet(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.reconnectCall);
		break;
	case CSTA_RETRIEVE_CALL_CONF: //CSTARetrieveCallConfEvent_t	retrieveCall;
		RetrieveCallConf(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.retrieveCall);
		break;
	case CSTA_SET_MWI_CONF: //CSTASetMwiConfEvent_t	setMwi;
		SetMwiRet(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.setMwi);
		break;
	case CSTA_SET_DND_CONF: //CSTASetDndConfEvent_t	setDnd;
		SetDndRet(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.setDnd);
		break;
	case CSTA_SET_FWD_CONF: //CSTASetFwdConfEvent_t	setFwd;
		SetFwdRet(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.setFwd);
		break;
	case CSTA_SET_AGENT_STATE_CONF: //CSTASetAgentStateConfEvent_t setAgentState;
		SetAgentStateConf(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.setAgentState);
		break;
	case CSTA_TRANSFER_CALL_CONF: //CSTATransferCallConfEvent_t	transferCall;
		TransferCallConf(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.transferCall);
		break;
	case CSTA_UNIVERSAL_FAILURE_CONF://CSTAUniversalFailureConfEvent_t universalFailure;
		UniversalFailRet(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.universalFailure);
		break;
	case CSTA_ROUTE_REGISTER_REQ_CONF: //CSTARouteRegisterReqConfEvent_t routeRegister;
		RouteRegisterReqConf(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.routeRegister);
		break;
	case CSTA_ROUTE_REGISTER_CANCEL_CONF: //CSTARouteRegisterCancelConfEvent_t routeCancel;
		RouteRegisterCancelRet(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.routeCancel);
		break;
	case CSTA_ESCAPE_SVC_CONF: //CSTAEscapeSvcConfEvent_t escapeService;
		EscapeServiceConf(pEventData->event.cstaConfirmation.invokeID,
			pEventData->event.cstaConfirmation.u.escapeService, pATTEvent);
		break;
	case CSTA_ESCAPE_SVC_REQ_CONF:
		ICC_LOG_DEBUG(m_pLog, "CSTA_ESCAPE_SVC_REQ_CONF");
		break;
	case CSTA_SYS_STAT_REQ_CONF: //CSTASysStatReqConfEvent_t sysStatReq;
		SysStatReqConf(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.sysStatReq);
		break;
	case CSTA_SYS_STAT_START_CONF: //CSTASysStatStartConfEvent_t sysStatStart;
		SysStatStartRet(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.sysStatStart);
		break;
	case CSTA_SYS_STAT_STOP_CONF: //CSTASysStatStopConfEvent_t sysStatStop;
		SysStatStopRet(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.sysStatStop);
		break;
	case CSTA_CHANGE_SYS_STAT_FILTER_CONF: //CSTAChangeSysStatFilterConfEvent_t	changeSysStatFilter;
		ChangeSysStatFilterRet(pEventData->event.cstaConfirmation.invokeID,
			pEventData->event.cstaConfirmation.u.changeSysStatFilter);
		break;
	case CSTA_REQ_SYS_STAT_CONF:
		ICC_LOG_DEBUG(m_pLog, "CSTA_REQ_SYS_STAT_CONF");
		break;
	case CSTA_MONITOR_CONF: //CSTAMonitorConfEvent_t monitorStart;
		// retrived the monitor cross reference ID	
		MonitorDeviceConf(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.monitorStart);
		break;
	case CSTA_CHANGE_MONITOR_FILTER_CONF: //CSTAChangeMonitorFilterConfEvent_t changeMonitorFilter;
		ChangeMonitorFilterRet(pEventData->event.cstaConfirmation.invokeID,
			pEventData->event.cstaConfirmation.u.changeMonitorFilter);
		break;
	case CSTA_MONITOR_STOP_CONF: //CSTAMonitorStopConfEvent_t monitorStop;
		// confirmation event when monitor has been removed
		MonitorStopRet(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.monitorStop);
		break;
	case CSTA_SNAPSHOT_CALL_CONF: //CSTASnapshotCallConfEvent_t snapshotCall;
		SnapshotCallRet(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.snapshotCall);
		break;
	case CSTA_SNAPSHOT_DEVICE_CONF: //CSTASnapshotDeviceConfEvent_t snapshotDevice;
		SnapshotDeviceRet(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.snapshotDevice);
		break;
	case CSTA_GETAPI_CAPS_CONF: //CSTAGetAPICapsConfEvent_t	getAPICaps;
		GetAPICapsRet(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.getAPICaps);
		break;
	case CSTA_GET_DEVICE_LIST_CONF:	//CSTAGetDeviceListConfEvent_t getDeviceList;
		GetDeviceListRet(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.getDeviceList);
		break;
	case CSTA_QUERY_CALL_MONITOR_CONF: //CSTAQueryCallMonitorConfEvent_t queryCallMonitor;
		QueryCallMonitorRet(pEventData->event.cstaConfirmation.invokeID, pEventData->event.cstaConfirmation.u.queryCallMonitor);
		break;
	default:
	{
		ICC_LOG_WARNING(m_pLog, "CSTA Confirmation: [%s]",
			CAvayaResult::Instance()->GetEventTypeString(pEventData->eventHeader.eventType).c_str());
	}
	break;
	}
}

void CAvayaEvent::HandleCSTAUnsolicited(CSTAEvent_t* pEventData, ATTEvent_t* pATTEvent)
{
	//现在只考虑监视设备，不考虑监视Call
	//如果以后要监视Call,则这里要分两种情况.一个是返回Call,一个返回设备

	long lMonitorId = pEventData->event.cstaUnsolicited.monitorCrossRefId;
	boost::shared_ptr<CDevice> pTSDevice = CDeviceManager::Instance()->FindDeviceByMonitorId(lMonitorId);
	if (!pTSDevice)
	{
		ICC_LOG_WARNING(m_pLog, "CSTA Event, MonitorId: [%u], EventType: [%s], Not find device by monitorId!",
			lMonitorId, CAvayaResult::Instance()->GetEventTypeString(pEventData->eventHeader.eventType).c_str());

		return;
	}

	switch (pEventData->eventHeader.eventType)
	{
	case CSTA_CALL_CLEARED: //CSTACallClearedEvent_t callCleared;
		CallClearedEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.callCleared);
		break;
	case CSTA_CONFERENCED: //CSTAConferencedEvent_t conferenced;
		ConferencedEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.conferenced);
		break;
	case CSTA_CONNECTION_CLEARED: //CSTAConnectionClearedEvent_t connectionCleared;
		ConnectClearedEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.connectionCleared);
		break;
	case CSTA_DELIVERED: //CSTADeliveredEvent_t delivered;
		DeliveredEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.delivered);
		break;
	case CSTA_DIVERTED: //CSTADivertedEvent_t diverted;
		DivertedEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.diverted);
		break;
	case CSTA_ESTABLISHED: //CSTAEstablishedEvent_t established;
		EstablishedEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.established);
		break;
	case CSTA_FAILED: //CSTAFailedEvent_t	failed;
		FailedEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.failed);
		break;
	case CSTA_HELD: //CSTAHeldEvent_t held;
		HeldEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.held);
		break;
	case CSTA_NETWORK_REACHED: //CSTANetworkReachedEvent_t networkReached;
		NetworkReachedEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.networkReached);
		break;
	case CSTA_ORIGINATED: //CSTAOriginatedEvent_t originated;
		OriginatedEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.originated);
		break;
	case CSTA_QUEUED: //CSTAQueuedEvent_t queued;
		QueuedEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.queued);
		break;
	case CSTA_RETRIEVED: //CSTARetrievedEvent_t retrieved;
		RetrievedEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.retrieved);
		break;
	case CSTA_SERVICE_INITIATED://CSTAServiceInitiatedEvent_t serviceInitiated;
		ServiceInitiatedEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.serviceInitiated);
		break;
	case CSTA_TRANSFERRED: //CSTATransferredEvent_t transferred;
		TransferredEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.transferred);
		break;
	case CSTA_CALL_INFORMATION: //CSTACallInformationEvent_t callInformation;
		CallInformationEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.callInformation);
		break;
	case CSTA_DO_NOT_DISTURB://CSTADoNotDisturbEvent_t doNotDisturb;
		DoNotDisturbEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.doNotDisturb);
		break;
	case CSTA_FORWARDING: //CSTAForwardingEvent_t	forwarding;
		ForwardingEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.forwarding);
		break;
	case CSTA_MESSAGE_WAITING://CSTAMessageWaitingEvent_t messageWaiting;
		MessageWaitingEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.messageWaiting);
		break;
	case CSTA_LOGGED_ON://CSTALoggedOnEvent_t loggedOn;
		LoggedOnEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.loggedOn);
		break;
	case CSTA_LOGGED_OFF://CSTALoggedOffEvent_t loggedOff;
		LoggedOffEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.loggedOff);
		break;
	case CSTA_NOT_READY://CSTANotReadyEvent_t notReady;
		NotReadyEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.notReady);
		break;
	case CSTA_READY://CSTAReadyEvent_t ready;
		ReadyEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.ready);
		break;
	case CSTA_WORK_NOT_READY://CSTAWorkNotReadyEvent_t workNotReady;
		WorkNotReadyEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.workNotReady);
		break;
	case CSTA_WORK_READY://CSTAWorkReadyEvent_t workReady;
		WorkReadyEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.workReady);
		break;
	case CSTA_BACK_IN_SERVICE: //CSTABackInServiceEvent_t backInService;
		BackInServiceEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.backInService);
		break;
	case CSTA_OUT_OF_SERVICE://CSTAOutOfServiceEvent_t outOfService;
		OutOfServiceEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.outOfService);
		break;
	case CSTA_PRIVATE_STATUS: //CSTAPrivateStatusEvent_t privateStatus;
		PrivateStatusEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.privateStatus);
		break;
	case CSTA_MONITOR_ENDED://CSTAMonitorEndedEvent_t monitorEnded;
		MonitorEndEvent(lMonitorId, pEventData->event.cstaUnsolicited.u.monitorEnded);
		break;
	default:
	{
		ICC_LOG_WARNING(m_pLog, "Unhandle CSTA Unsolicited: [%s]",
			CAvayaResult::Instance()->GetEventTypeString(pEventData->eventHeader.eventType).c_str());
	}
	break;
	}
}


void CAvayaEvent::RecCSTAReportEvent(CSTAEvent_t* pEventData)
{
	ICC_LOG_DEBUG(m_pLog, "Receive CSTAReportEvent, EventType: [%d]-[%s]",
		pEventData->eventHeader.eventType,
		CAvayaResult::Instance()->GetEventTypeString(pEventData->eventHeader.eventType).c_str());

	switch (pEventData->eventHeader.eventType)
	{
	case CSTA_ROUTE_REGISTER_ABORT:
		//CSTARouteRegisterAbortEvent_t   registerAbort;
		break;
	case CSTA_ROUTE_USED:
		//CSTARouteUsedEvent_t			routeUsed;
		//CSTARouteUsedExtEvent_t			routeUsedExt;
		break;
	case CSTA_ROUTE_END:
		//CSTARouteEndEvent_t				routeEnd;
		break;
	case CSTA_PRIVATE:
		//CSTAPrivateEvent_t				privateEvent;
		break;
	case CSTA_SYS_STAT:
		//CSTASysStatEvent_t				sysStat;
		break;
	case CSTA_SYS_STAT_ENDED:
		//CSTASysStatEndedEvent_t			sysStatEnded;
		break;
	default:
		break;
	}
}


//-----------------------------------------------------------------------------------------------
void CAvayaEvent::OpenStreamConf(CSTAEvent_t* pEventData)
{
	std::string strShowMsg = m_pStrUtil->Format("Open AES Stream success! apiVer: [%s], libVer: [%s], tsrvVer: [%s], drvrVer: [%s]",
		pEventData->event.acsConfirmation.u.acsopen.apiVer,
		pEventData->event.acsConfirmation.u.acsopen.libVer,
		pEventData->event.acsConfirmation.u.acsopen.tsrvVer,
		pEventData->event.acsConfirmation.u.acsopen.drvrVer);
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	ICTIConnStateNotifPtr pAESStateNotif = boost::make_shared<CCTIConnStateNotif>();
	if (pAESStateNotif)
	{
		pAESStateNotif->SetStreamOpen(true);
		pAESStateNotif->SetSwitchType(SWITCH_TYPE_TSAPI);

		long lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_ConnectAesEvent, pAESStateNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask ConnectAesEvent, Open Stream Success, TaskId: [%u]", lTaskId);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CAESStateNotif Object Failed !!!");
	}
}

void CAvayaEvent::OpenStreamFail(ACSUniversalFailure_t error)
{
	std::string strShowMsg = m_pStrUtil->Format("Open AES Stream Fail! The error is: [%s]",
		CAvayaResult::Instance()->GetACSUnsoliEventString(error).c_str());
	ICC_LOG_ERROR(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	ICTIConnStateNotifPtr pAESStateNotif = boost::make_shared<CCTIConnStateNotif>();
	if (pAESStateNotif)
	{
		pAESStateNotif->SetStreamOpen(false);
	//	pAESStateNotif->SetStreamException(true);
		pAESStateNotif->SetSwitchType(SWITCH_TYPE_TSAPI);

		long lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_ConnectAesEvent, pAESStateNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask ConnectAesEvent, Open Stream Failed, TaskId: [%u]", lTaskId);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CAESStateNotif Object Failed !!!");
	}
}

void CAvayaEvent::CloseStreamConf()
{
	ICC_LOG_DEBUG(m_pLog, "AES Stream Close Success!");
	ShowMsg("AES Stream Close Success!");

	/*ICTIConnStateNotifPtr pAESStateNotif = boost::make_shared<CCTIConnStateNotif>();
	if (pAESStateNotif)
	{
		pAESStateNotif->SetStreamOpen(false);

		ITaskPtr pTask = CTaskManager::Instance()->CreateNewTask();
		if (pTask)
		{
			long lTaskId = pTask->GetTaskId();

			pTask->SetTaskName(Task_ConnectAesEvent);
			pTask->SetSwitchNotif(pAESStateNotif);
			CTaskManager::Instance()->AddSwitchEventTask(pTask);

			ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask ConnectAesEvent, Close Stream Success, TaskId: [%u]", lTaskId);
		}
		else
		{
			ICC_LOG_FATAL(m_pLog, "Create ConnectAesEvent Task Object Failed !!!");
		}
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CAESStateNotif Object Failed !!!");
	}*/
}

//流异常
void CAvayaEvent::StreamException()
{
	ICC_LOG_FATAL(m_pLog, "AES Stream Exception!");
	ShowMsg("AES Stream Exception!");

	ICTIConnStateNotifPtr pAESStateNotif = boost::make_shared<CCTIConnStateNotif>();
	if (pAESStateNotif)
	{
		pAESStateNotif->SetStreamOpen(false);
		pAESStateNotif->SetStreamException(true);
		pAESStateNotif->SetSwitchType(SWITCH_TYPE_TSAPI);

		long lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_ConnectAesEvent, pAESStateNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask ConnectAesEvent, Stream Exception, TaskId: [%u]", lTaskId);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CAESStateNotif Object Failed !!!");
	}
}

void CAvayaEvent::MonitorDeviceConf(long invokeID, CSTAMonitorConfEvent_t& monitorStart)
{
	long lMonitorId = monitorStart.monitorCrossRefID;

	ICC_LOG_DEBUG(m_pLog, "InvokeID: [%u], CSTA_MONITOR_CONF event=MonitorDeviceConf, MonitorId: [%u]",
		invokeID, lMonitorId);

	IMonitorDeviceResultNotifPtr pMonitorResultNotif = boost::make_shared<CMonitorDeviceResultNotif>();
	if (pMonitorResultNotif)
	{
		pMonitorResultNotif->SetRequestId(invokeID);
		pMonitorResultNotif->SetMonitorId(lMonitorId);
		pMonitorResultNotif->SetResult(true);

		long lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_MonitorDeviceConf, pMonitorResultNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask MonitorDeviceConf, TaskId: [%u], RequestId: [%u], MonitorId: [%u]",
			lTaskId, invokeID, lMonitorId);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CMonitorDeviceResultNotif Object Failed !!!");
	}
}

void CAvayaEvent::MakeCallConf(long invokeID, CSTAMakeCallConfEvent_t& makeCall)
{
	std::string strConnect = CAvayaResult::Instance()->GetConnectionID_tString(makeCall.newCall);
	ICC_LOG_DEBUG(m_pLog, "InvokeID: [%u], CSTA_MAKE_CALL_CONF  Event=MakeCallRet, NewCall: [%s]",
		invokeID, strConnect.c_str());

	long lCSTACallRefId = makeCall.newCall.callID;
	std::string strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(lCSTACallRefId, strCTICallRefId))
	{
		strCTICallRefId = CCallManager::Instance()->CreateNewCall(lCSTACallRefId);
	}

	IMakeCallResultNotifPtr pMakeCallResultNotif = boost::make_shared<CMakeCallResultNotif>();
	if (pMakeCallResultNotif)
	{
		pMakeCallResultNotif->SetRequestId(invokeID);
		pMakeCallResultNotif->SetCSTACallRefId(lCSTACallRefId);
		pMakeCallResultNotif->SetCTICallRefId(strCTICallRefId);
		pMakeCallResultNotif->SetResult(true);

		long lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_MakeCallConf, pMakeCallResultNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask MakeCallConf, TaskId: [%u], RequestId: [%u], CSTACallRefId: [%u]",
			lTaskId, invokeID, lCSTACallRefId);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CMakeCallResultNotif Object Failed !!!");
	}
}

void CAvayaEvent::AnswerCallConf(long invokeID, CSTAAnswerCallConfEvent_t& answerCall)
{
	ICC_LOG_DEBUG(m_pLog, "InvokeID: [%u], CSTA_ANSWER_CALL_CONF  event=AnswerCallConf",
		invokeID);

	PostSwitchConf(invokeID, Task_AnswerCallConf);
}

void CAvayaEvent::DeliveredEvent(long lMonitorId, CSTADeliveredEvent_t& delivered)
{
	boost::shared_ptr<CDevice> pDevice = CDeviceManager::Instance()->FindDeviceByMonitorId(lMonitorId);
	if (!pDevice)
	{
		ICC_LOG_WARNING(m_pLog, "DeliveredEvent not find device by lMonitorId: [%u]",
			lMonitorId);

		return;
	}

	long lCSTACallRefId = delivered.connection.callID;					//	交换机话务ID
	std::string strDn(delivered.connection.deviceID);					//  话务成员
	std::string strAlertingDn(delivered.alertingDevice.deviceID);		//	振铃的分机号码
	std::string strCallerId(delivered.callingDevice.deviceID);			//	主叫号码
	std::string strCalledId(delivered.calledDevice.deviceID);			//	被叫号码(110，119)
	std::string strRedirDn(delivered.lastRedirectionDevice.deviceID);	//	重定向号码
	std::string strState = CAvayaResult::Instance()->GetConnectionStateString(delivered.localConnectionInfo);
	std::string strEventCause = CAvayaResult::Instance()->GetCSTAEventCauseString(delivered.cause);
	std::string strDeviceNum = pDevice->GetDeviceNum();

	std::string strShowMsg = m_pStrUtil->Format("Device: %s-%s, CSTA_DELIVERED Event=DeliveredEvent, State: [%s], CallRefId: [%u],"
		"AlertingDn: [%s], CallerId: [%s], CalledId: [%s], RedirId: [%s], EventCause: [%s]",
		strDeviceNum.c_str(), strDn.c_str(), strState.c_str(), lCSTACallRefId,
		strAlertingDn.c_str(), strCallerId.c_str(), strCalledId.c_str(), strRedirDn.c_str(), strEventCause.c_str());
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	strCallerId = CSysConfig::Instance()->GetCheckNum(strCallerId);
	strCalledId = CSysConfig::Instance()->GetCheckNum(strCalledId);

	if (strCallerId.find("T") != std::string::npos)
	{
		// 主叫号码为空
	}

	switch (delivered.localConnectionInfo)
	{
	case CS_ALERTING://==InboundCall use for phone
	{
		// This event is generated when Agent receives a call.
		if (delivered.cause == EC_REDIRECTED ||
			delivered.cause == EC_NEW_CALL ||
			delivered.cause == EC_CALL_FORWARD_BUSY ||
			delivered.cause == EC_CALL_FORWARD)
		{
			// Device:7004, 7004, CSTA_DELIVERED Event=DeliveredEvent, State=CS_ALERTING, CallRefId=483,AlertingDn=7004, 
			// CallerId=7002, CalledId=110, RedirId=110, EventCause=EC_REDIRECTED
			CallAlerting(lCSTACallRefId, strCallerId, strCalledId, strAlertingDn);
		}
	}
	break;
	case CS_CONNECT://==DestSeized
	{
		if (delivered.cause == EC_NEW_CALL || delivered.cause == EC_REDIRECTED)
		{
			if (strAlertingDn.find("T") != std::string::npos)
			{
				CCallManager::Instance()->SetE1RelayIndexNumber(lCSTACallRefId, strAlertingDn, strCalledId);
				strAlertingDn = strCalledId;
			}

			// Device:7002, 7004, CSTA_DELIVERED Event=DeliveredEvent, State=CS_CONNECT, CallRefId=483,AlertingDn=7004, 
			// CallerId=7002, CalledId=110, RedirId=110, EventCause=EC_REDIRECTED
			CallAlteringBack(lCSTACallRefId, strCallerId, strCalledId, strAlertingDn);
		}
	}
	// This Delivered event is generated when Agent has Inititated a call.
	//PostMsgToLogic(PHY_EVENT_DESTSEIZED,0,lCSTACallRefId, 0,strCallerId,strCalledId);
	break;
	case CS_NONE://==InboundCall  use for acd
	{
		if (delivered.cause == EC_NEW_CALL)
		{
			// Device:110, 110, CSTA_DELIVERED Event=DeliveredEvent, State=CS_NONE, CallRefId=483,AlertingDn=110, CallerId=7002, CalledId=110, 
			// RedirId=, EventCause=EC_NEW_CALL
			CallIncoming(lCSTACallRefId, strCallerId, strCalledId, strAlertingDn);
		}

		if (delivered.cause == EC_REDIRECTED)
		{
			// Device:110, 7004, CSTA_DELIVERED Event=DeliveredEvent, State=CS_NONE, CallRefId=483,AlertingDn=7004, CallerId=7002, CalledId=110, 
			// RedirId=110, EventCause=EC_REDIRECTED
			CallAssign(lCSTACallRefId, strCallerId, strCalledId, strAlertingDn);
		}

		break;
	}

	case CS_NULL:
	case CS_INITIATE:
	case CS_HOLD:
	case CS_QUEUED:
	case CS_FAIL:
		ICC_LOG_WARNING(m_pLog, "Event unhandle msg: %s", strShowMsg.c_str());
		break;
	default:
		break;
	}
}

void CAvayaEvent::EstablishedEvent(long lMonitorId, CSTAEstablishedEvent_t& established)
{
	boost::shared_ptr<CDevice> pDevice = CDeviceManager::Instance()->FindDeviceByMonitorId(lMonitorId);
	if (!pDevice)
	{
		ICC_LOG_WARNING(m_pLog, "EstablishedEvent not find device by lMonitorId: [%u]",
			lMonitorId);

		return;
	}

	long lCSTACallRefId = established.establishedConnection.callID;
	std::string strDn(established.establishedConnection.deviceID);
	std::string strAnswerId(established.answeringDevice.deviceID);
	std::string strCallerId(established.callingDevice.deviceID);
	std::string strCalledId(established.calledDevice.deviceID);
	std::string strRedirectId(established.lastRedirectionDevice.deviceID);
	std::string strState = CAvayaResult::Instance()->GetConnectionStateString(established.localConnectionInfo);
	std::string strCause = CAvayaResult::Instance()->GetCSTAEventCauseString(established.cause);
	std::string strDeviceNum = pDevice->GetDeviceNum();

	std::string strShowMsg = m_pStrUtil->Format("Device: %s-%s, CSTA_DELIVERED Event=EstablishedEvent, State: [%s], CSTACallRefId: [%u],"
		"AnswerId: [%s], CallerId: [%s], CalledId: [%s], RedirId: [%s], EventCause: [%s]",
		strDeviceNum.c_str(), strDn.c_str(), strState.c_str(), lCSTACallRefId,
		strAnswerId.c_str(), strCallerId.c_str(), strCalledId.c_str(), strRedirectId.c_str(), strCause.c_str());
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	if (strAnswerId.empty() || strAnswerId.find("T") != std::string::npos)
	{
		//	呼叫手台，strAnswerId 为空
		//	中继呼出，AnswerId为 T1008#1 的格式
		strAnswerId = strCalledId;
	}

	strCallerId = CSysConfig::Instance()->GetCheckNum(strCallerId);
	strCalledId = CSysConfig::Instance()->GetCheckNum(strCalledId);
	strAnswerId = CSysConfig::Instance()->GetCheckNum(strAnswerId);

	// Device:110, 7004, CSTA_DELIVERED Event = EstablishedEvent, State = CS_NONE, CallRefId = 483, AnswerId = 7004, 
	// CallerId = 7002, CalledId = 110, RedirId = 110, EventCause = EC_NEW_CALL

	// Device:7004, 7004, CSTA_DELIVERED Event = EstablishedEvent, State = CS_CONNECT, CallRefId = 483, AnswerId = 7004, 
	// CallerId = 7002, CalledId = 110, RedirId = 110, EventCause = EC_NEW_CALL

	// Device:7002, 7004, CSTA_DELIVERED Event=EstablishedEvent, State=CS_CONNECT, CallRefId=483,AnswerId=7004, 
	// CallerId=7002, CalledId=110, RedirId=110, EventCause=EC_NEW_CALL

	//CS_CONNECT Unhandle Msg : Device : 7004, 7004, CSTA_DELIVERED Event = EstablishedEvent, State = CS_CONNECT, CallRefId = 360, AnswerId = 7004, 
	//CallerId = 7002, CalledId = 7001, RedirId = , EventCause = EC_SILENT_MONITOR

	switch (established.localConnectionInfo)
	{
	case CS_CONNECT:
		switch (established.cause)
		{
		case EC_NEW_CALL:
		case EC_TRANSFER:
		case EC_PARK://第三方拉会议可能会出现这个
		case EC_KEY_CONFERENCE:
			CallTalking(lCSTACallRefId, strCallerId, strAnswerId, strDeviceNum);
			/*if (strDeviceNum.compare(strAnswerId) == 0)//==TpAnswered 己方应答
				CalledTalking(lCSTACallRefId, strCallerId, strCalledId, strAnswerId, false);
			else//==OpAnswered	主叫接通
				CalledTalking(lCSTACallRefId, strCallerId, strCalledId, strCallerId, true);*/
			break;
		case EC_ACTIVE_MONITOR:	// 强插
			//Device:7004, 7004, CSTA_DELIVERED Event=EstablishedEvent, State=CS_CONNECT, CallRefId=366,AnswerId=7004, 
			//CallerId=7002, CalledId=7001, RedirId=, EventCause=EC_ACTIVE_MONITOR
			//Device:7002, 7004, CSTA_DELIVERED Event=EstablishedEvent, State=CS_CONNECT, CallRefId=366,AnswerId=7004, 
			//CallerId=7002, CalledId=7001, RedirId=, EventCause=EC_ACTIVE_MONITOR
			//Device:7001, 7004, CSTA_DELIVERED Event=EstablishedEvent, State=CS_CONNECT, CallRefId=366,AnswerId=7004, 
			//CallerId=7002, CalledId=7001, RedirId=, EventCause=EC_ACTIVE_MONITOR
			break;
		case EC_SILENT_MONITOR: // 监听
			// 7002 -> 7001, 7004 -> 7001
			//Device:7004, 7004, CSTA_DELIVERED Event=EstablishedEvent, State=CS_CONNECT, CallRefId=360,AnswerId=7004,
			//CallerId=7002, CalledId=7001, RedirId=, EventCause=EC_SILENT_MONITOR
			//Device:7002, 7004, CSTA_DELIVERED Event=EstablishedEvent, State=CS_CONNECT, CallRefId=360,AnswerId=7004, 
			//CallerId=7002, CalledId=7001, RedirId=, EventCause=EC_SILENT_MONITOR
			//Device:7001, 7004, CSTA_DELIVERED Event=EstablishedEvent, State=CS_CONNECT, CallRefId=360,AnswerId=7004, 
			//CallerId=7002, CalledId=7001, RedirId=, EventCause=EC_SILENT_MONITOR
			break;
		default:
			ICC_LOG_WARNING(m_pLog, "CS_CONNECT Unhandle Msg: %s", strShowMsg.c_str());
			break;
		}
		break;
	case CS_HOLD://==hold
		break;
	case CS_NONE:
		//
		break;
	default:
		ICC_LOG_WARNING(m_pLog, "EstablishedEvent Unhandle Msg: %s", strShowMsg.c_str());
		break;
	}
}

void CAvayaEvent::ConnectClearedEvent(long lMonitorId, CSTAConnectionClearedEvent_t& connectionCleared)
{
	// This event is generated when Agent Logged out manually
	boost::shared_ptr<CDevice> pDevice = CDeviceManager::Instance()->FindDeviceByMonitorId(lMonitorId);
	if (!pDevice)
	{
		ICC_LOG_ERROR(m_pLog, "ConnectClearedEvent not find device by lMonitorId: [%u]",
			lMonitorId);

		return;
	}

	long lCSTACallRefId = connectionCleared.droppedConnection.callID;
	std::string strDn(connectionCleared.droppedConnection.deviceID);
	std::string strHangupDn(connectionCleared.releasingDevice.deviceID);
	std::string strState = CAvayaResult::Instance()->GetConnectionStateString(connectionCleared.localConnectionInfo);
	std::string strEventCause = CAvayaResult::Instance()->GetCSTAEventCauseString(connectionCleared.cause);
	std::string strDeviceNum = pDevice->GetDeviceNum();

	/*if (strHangupDn.empty())
	{
	//	呼叫手台，strHangupDn 为空
	strAnswerId = strCalledId;
	}*/

	std::string strShowMsg;
	strShowMsg = m_pStrUtil->Format("Device: %s-%s, event=ConnectClearedEvent, HangupDn: [%s], state: [%s], callRefId: [%u], eventCause: [%s]",
		strDeviceNum.c_str(), strDn.c_str(), strHangupDn.c_str(), strState.c_str(), lCSTACallRefId, strEventCause.c_str());
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);
	
	strDn = CSysConfig::Instance()->GetCheckNum(strDn);
	strHangupDn = CSysConfig::Instance()->GetCheckNum(strHangupDn);

	// 7001 -> 7004
	// Device:7004, 7004, event = ConnectClearedEvent, HangupDn = 7004, state = CS_NULL, callRefId = 479, eventCause = EC_NONE
	// Device : 7001, 7004, event = ConnectClearedEvent, HangupDn = 7004, state = CS_CONNECT, callRefId = 479, eventCause = EC_NONE
	// Device : 7001, 7001, event = ConnectClearedEvent, HangupDn = 7001, state = CS_NULL, callRefId = 479, eventCause = EC_NONE

	// 7001 -> 110 -> 7003
	// Device:110, 7003, event = ConnectClearedEvent, HangupDn = 7003, state = CS_NONE, callRefId = 498, eventCause = EC_NONE
	// Device : 7003, 7003, event = ConnectClearedEvent, HangupDn = 7003, state = CS_NULL, callRefId = 498, eventCause = EC_NONE
	// Device : 7001, 7003, event = ConnectClearedEvent, HangupDn = 7003, state = CS_CONNECT, callRefId = 498, eventCause = EC_NONE
	// Device : 7001, 7001, event = ConnectClearedEvent, HangupDn = 7001, state = CS_NULL, callRefId = 498, eventCause = EC_NONE
	// Device:110, 0, Event = CallClearedEvent, State = CS_NONE, CallRefId = 498, EventCause = EC_NONE
	
	// 4102 -> 100 -> 7005
	//Device:110, 4102, event = ConnectClearedEvent, HangupDn = 4102, state = CS_NONE, callRefId = 141,eventCause = EC_NONE
	//Device : 7005, 4102, event = ConnectClearedEvent, HangupDn = 4102, state = CS_CONNECT, callRefId = 141,eventCause = EC_NONE
	//Device : 7005, 7005, event = ConnectClearedEvent, HangupDn = 7005, state = CS_NULL, callRefId = 141,eventCause = EC_NONE
	//Device : 110, 0, Event = CallClearedEvent, State = CS_NONE, CallRefId = 141, EventCause = EC_NONE

//	E_HANGUP_TYPE nHangupType = HANGUP_TYPE_CALLER;

	switch (connectionCleared.localConnectionInfo)
	{
	case CS_NONE://==TpDisconnected use for vdn
	case CS_NULL://==TpDisconnected	use for phone
	{
		// 分机主动挂断(被叫挂机)
		if (connectionCleared.cause != EC_REDIRECTED && strDeviceNum == strDn)
		{
			//nHangupType = HANGUP_TYPE_CALLED;
			DeviceHangup(lCSTACallRefId, strHangupDn, strDeviceNum);
		}	
	}
	break;
	case CS_CONNECT:
		ICC_LOG_DEBUG(m_pLog, "hangup connectionCleared.localConnectionInfo:[CS_CONNECT],call:[%ld],hang up device:[%s], device:[%s]", lCSTACallRefId, strHangupDn.c_str(), strDeviceNum.c_str());
		if (strHangupDn.find("T") != std::string::npos)
		{
			if (CCallManager::Instance()->GetCallMode(lCSTACallRefId).compare(CallModeString[CALL_MODE_CONFERENCE]) != 0)
			{
				//中继挂机并且不在会议
				strHangupDn = strDeviceNum;
			}
		}
		else
		{
			if (strDn.find("T") != std::string::npos)
			{
				if (CCallManager::Instance()->GetCallMode(lCSTACallRefId).compare(CallModeString[CALL_MODE_CONFERENCE]) == 0)
				{
					ICC_LOG_DEBUG(m_pLog, "hangup connectionCleared.conference change hangupDn,call:[%ld],HangupDn:[%s], Dn:[%s]", lCSTACallRefId, strHangupDn.c_str(), strDn.c_str());
					strHangupDn = strDn;
				}
			}
		}
		DeviceHangup(lCSTACallRefId, strHangupDn, strDeviceNum);
		break;
	case CS_INITIATE://==OpDisconnected
	case CS_ALERTING:
	case CS_HOLD:
	case CS_QUEUED:
		{
			//nHangupType = HANGUP_TYPE_CALLER;//主叫挂断
			DeviceHangup(lCSTACallRefId, strHangupDn, strDeviceNum);
		}
		break;
	case CS_FAIL:
		{	
			if (strHangupDn.find("T") != std::string::npos)
			{
				//中继挂机
				ICC_LOG_DEBUG(m_pLog, "CS_FAIL, HangupDn:[%s],DeviceNum:[%s],lCSTACallRefId:[%d]", strHangupDn.c_str(), strDeviceNum.c_str(), lCSTACallRefId);
				if (CCallManager::Instance()->GetCallMode(lCSTACallRefId).compare(CallModeString[CALL_MODE_CONFERENCE]) != 0)
				{
					//中继failed并且不在会议
					strHangupDn = strDeviceNum;
				}
				//nHangupType = HANGUP_TYPE_CALLER;//主叫挂断
				DeviceHangup(lCSTACallRefId, strHangupDn, strDeviceNum);
			}
			else
			{
				DeviceHangup(lCSTACallRefId, strHangupDn, strDeviceNum);
			}
		}
		break;
	default:
		break;
	}
}

void CAvayaEvent::ServiceInitiatedEvent(long lMonitorId, CSTAServiceInitiatedEvent_t& serviceInitiated)
{
	boost::shared_ptr<CDevice> pDevice = CDeviceManager::Instance()->FindDeviceByMonitorId(lMonitorId);
	if (!pDevice)
	{
		ICC_LOG_ERROR(m_pLog, "ServiceInitiatedEvent not find device by lMonitorId: [%u]",
			lMonitorId);

		return;
	}

	long lCSTACallRefId = serviceInitiated.initiatedConnection.callID;
	std::string strDn(serviceInitiated.initiatedConnection.deviceID);
	std::string strState = CAvayaResult::Instance()->GetConnectionStateString(serviceInitiated.localConnectionInfo);
	std::string strEventCause = CAvayaResult::Instance()->GetCSTAEventCauseString(serviceInitiated.cause);
	std::string strDeviceNum = pDevice->GetDeviceNum();

	std::string strShowMsg = m_pStrUtil->Format("Device: %s-%s, Event=ServiceInitiatedEvent, CallRefId: [%u], State: [%s], EventCause: [%s]",
		strDeviceNum.c_str(), strDn.c_str(), lCSTACallRefId, strState.c_str(), strEventCause.c_str());
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);
	//Device:7002,7002, Event=ServiceInitiatedEvent, CallRefId=483, State=CS_INITIATE, EventCause=EC_NONE

	DeviceOffHook(lCSTACallRefId, strDn);
}

void CAvayaEvent::OriginatedEvent(long lMonitorId, CSTAOriginatedEvent_t& originated)
{
	// This event is generated in sequence after the tehe CSTA_SERVICE_INITIATED_EVENT
	// when Agent makes a call

	// When Agent makes request to logs out manaully , This event is not generated
	boost::shared_ptr<CDevice> pDevice = CDeviceManager::Instance()->FindDeviceByMonitorId(lMonitorId);
	if (!pDevice)
	{
		ICC_LOG_ERROR(m_pLog, "OriginatedEvent not find device by lMonitorId: [%u]",
			lMonitorId);

		return;
	}

	long lCSTACallRefId = originated.originatedConnection.callID;
	std::string strDn(originated.originatedConnection.deviceID);
	std::string strCallerId(originated.callingDevice.deviceID);
	std::string strCalledId(originated.calledDevice.deviceID);
	std::string strState = CAvayaResult::Instance()->GetConnectionStateString(originated.localConnectionInfo);
	std::string strEventCause = CAvayaResult::Instance()->GetCSTAEventCauseString(originated.cause);
	std::string strDeviceNum = pDevice->GetDeviceNum();

	std::string strShowMsg = m_pStrUtil->Format("Device: %s-%s,Event=OriginatedEvent, State: [%s], CallRefId: [%u], CallerId: [%s], CalledId: [%s], EventCause: [%s]",
		strDeviceNum.c_str(), strDn.c_str(), strState.c_str(), lCSTACallRefId, strCallerId.c_str(), strCalledId.c_str(), strEventCause.c_str());
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);
	//Device:7002, 7002,Event=OriginatedEvent, State=CS_INITIATE, CallRefId=483, CallerId=7002, CalledId=110, EventCause=EC_NEW_CALL

	/*
	out 内线呼内线有连续两个event
	号码:91105,91105,event=OriginatedEvent,state=CS_INITIATE,callRefId=2151,
	callerId=91105,calledId=91107,eventCause=EC_NEW_CALL
	号码:91105,91107,event=DeliveredEvent,state=CS_CONNECT,callRefId=2151,
	AlertingDn=91107,CallerId=91105,CalledId=91107,redirId=,eventCause=EC_NEW_CALL


	号码:91105,91105,event=OriginatedEvent,state=CS_INITIATE,callRefId=2210,
	callerId=91105,calledId=91108,eventCause=EC_NEW_CALL
	out 手机
	号码:91105,91105,event=OriginatedEvent,state=CS_CONNECT,callRefId=2224,
	callerId=91105,calledId=013613035046,eventCause=EC_NEW_CALL

	号码:91105,91105,event=OriginatedEvent,state=CS_INITIATE,callRefId=2354,
	callerId=91105,calledId=110,eventCause=EC_NEW_CALL

	号码:49014,49014,event=OriginatedEvent,state=CS_CONNECT,callRefId=6574,
	callerId=49014,calledId=62273110,eventCause=EC_NEW_CALL
	*/
	switch (originated.localConnectionInfo)
	{
	case CS_INITIATE://寻址到对方,但并不表示对方振铃
		break;
	case CS_CONNECT://==DestSeized
		break;
	default:
		ICC_LOG_WARNING(m_pLog, "OriginatedEvent Unhandle Msg: %s", strShowMsg.c_str());
		break;
	}
}

void CAvayaEvent::CallClearedEvent(long lMonitorId, CSTACallClearedEvent_t& callCleared)
{
	boost::shared_ptr<CDevice> pDevice = CDeviceManager::Instance()->FindDeviceByMonitorId(lMonitorId);
	if (!pDevice)
	{
		ICC_LOG_ERROR(m_pLog, "CallClearedEvent not find device by lMonitorId: [%u]",
			lMonitorId);

		return;
	}

	long lCSTACallRefId = callCleared.clearedCall.callID;
	std::string strDn(callCleared.clearedCall.deviceID);
	LocalConnectionState_t state = callCleared.localConnectionInfo;
	std::string strState = CAvayaResult::Instance()->GetConnectionStateString(state);
	std::string strEventCause = CAvayaResult::Instance()->GetCSTAEventCauseString(callCleared.cause);
	std::string strDeviceNum = pDevice->GetDeviceNum();

	std::string strShowMsg = m_pStrUtil->Format("Device: %s-%s, Event=CallClearedEvent, State: [%s], CallRefId: [%u], EventCause: [%s]",
		strDeviceNum.c_str(), strDn.c_str(), strState.c_str(), lCSTACallRefId, strEventCause.c_str());
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);
	//Device:110,0,event=CallClearedEvent, state=CS_NONE,callRefId=3878,eventCause=EC_NONE

//	std::string strCTICallRefId = "";
	if (CCallManager::Instance()->FindCallByCSTACallRefId(lCSTACallRefId))
	{
		bool bRemoveCall = false;
		if (CCallManager::Instance()->IsBlackCall(lCSTACallRefId))
		{
			bRemoveCall = true;
			ICC_LOG_DEBUG(m_pLog, "black call change bRemoveCall to true");

			//	黑名单拒接通知
			CCallManager::Instance()->SetHangupState(lCSTACallRefId, strDeviceNum, HangupTypeString[HANGUP_TYPE_BLACKCALL]);
			CCallManager::Instance()->PostRefuseCallNotif(lCSTACallRefId);
		}

		CCallManager::Instance()->ProcessDeviceHangup(lCSTACallRefId, strDeviceNum);

		if (CCallManager::Instance()->IsReleaseCall(lCSTACallRefId))
		{
			bRemoveCall = true;
			ICC_LOG_DEBUG(m_pLog, "release call change bRemoveCall to true");

			//	早释通知
			CCallManager::Instance()->PostReleaseCall(lCSTACallRefId);
		}

		CDeviceManager::Instance()->DeleteLogicalCallState(lCSTACallRefId, strDeviceNum);

		this->CheckCallOver(lCSTACallRefId, bRemoveCall);
	}
}

void CAvayaEvent::QueuedEvent(long lMonitorId, CSTAQueuedEvent_t& queued)
{
	boost::shared_ptr<CDevice> pDevice = CDeviceManager::Instance()->FindDeviceByMonitorId(lMonitorId);
	if (!pDevice)
	{
		ICC_LOG_ERROR(m_pLog, "QueuedEvent not find device by lMonitorId: [%u]",
			lMonitorId);

		return;
	}

	long lCSTACallRefId = queued.queuedConnection.callID;
	std::string strDn(queued.queuedConnection.deviceID);
	std::string strCallerId(queued.callingDevice.deviceID);
	std::string strCalledId(queued.calledDevice.deviceID);
	std::string strReDirId(queued.lastRedirectionDevice.deviceID);
	std::string strState = CAvayaResult::Instance()->GetConnectionStateString(queued.localConnectionInfo);
	std::string strEventCause = CAvayaResult::Instance()->GetCSTAEventCauseString(queued.cause);
	short	nQueue = queued.numberQueued;	//numberQueued为当前在strReDirId上排队的数目
	std::string strDeviceNum = pDevice->GetDeviceNum();

	std::string strShowMsg = m_pStrUtil->Format("Device: %s-%s, Event=QueuedEvent, State: [%s], CallRefId: [%u],"
		"CallerId: [%s], CalledId: [%s], RedirDnId: [%s], EventCause: [%s], NumberQueued:%d",
		strDeviceNum.c_str(), strDn.c_str(), strState.c_str(), lCSTACallRefId,
		strCallerId.c_str(), strCalledId.c_str(), strReDirId.c_str(), strEventCause.c_str(), nQueue);
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	strCallerId = CSysConfig::Instance()->GetCheckNum(strCallerId);
	strCalledId = CSysConfig::Instance()->GetCheckNum(strCalledId);

	/*
	号码:110,46110,event=QueuedEvent,state=CS_NONE,callRefId=1107,callerId=91103,
	calledId=110,RedirDnId=110,eventCause=EC_REDIRECTED
	号码:91103,46110,event=QueuedEvent,state=CS_CONNECT,callRefId=1107,callerId=91103,
	calledId=110,RedirDnId=110,eventCause=EC_REDIRECTED
	*/
	if (pDevice->GetDeviceType() == CTC_TYPE_ACDGROUP)
	{
		switch (queued.localConnectionInfo)
		{
		case CS_NONE:	//==Diverted
			ICC_LOG_DEBUG(m_pLog, "QueuedEvent, CallerId: [%s]", strCallerId.c_str());
			CallWaiting(lCSTACallRefId, strCallerId, strCalledId, strReDirId);
			break;
		case CS_CONNECT:
			break;
		default:
			break;
		}
	}
}

void CAvayaEvent::ConferencedEvent(long lMonitorId, CSTAConferencedEvent_t& conferenced)
{
	boost::shared_ptr<CDevice> pDevice = CDeviceManager::Instance()->FindDeviceByMonitorId(lMonitorId);
	if (!pDevice)
	{
		ICC_LOG_WARNING(m_pLog, "ConferencedEvent not find device by lMonitorId: [%u]",
			lMonitorId);

		return;
	}

	long lCallRefId1 = conferenced.primaryOldCall.callID;
	std::string strDn1(conferenced.primaryOldCall.deviceID);

	long lCallRefId2(conferenced.secondaryOldCall.callID);
	std::string strDn2(conferenced.secondaryOldCall.deviceID);

	std::string strCtrlDn(conferenced.confController.deviceID);
	std::string strAddDn(conferenced.addedParty.deviceID);
	int iMemCount = conferenced.conferenceConnections.count;
	long lConfCallRefId = conferenced.conferenceConnections.connection->party.callID;
	std::string strConfDn(conferenced.conferenceConnections.connection->party.deviceID);
	std::string strConfStaticDn(conferenced.conferenceConnections.connection->staticDevice.deviceID);
	std::string strState = CAvayaResult::Instance()->GetConnectionStateString(conferenced.localConnectionInfo);
	std::string strEventCause = CAvayaResult::Instance()->GetCSTAEventCauseString(conferenced.cause);
	std::string strDeviceNum = pDevice->GetDeviceNum();

	std::string strTmp;
	std::string strConf;
	Connection_t* pList = conferenced.conferenceConnections.connection;
	for (int i = 0; i < iMemCount; i++)
	{
		strTmp = m_pStrUtil->Format(" confidx %d call%d %s",
			i, i, CAvayaResult::Instance()->GetConnectionID_tString((pList++)->party).c_str());
		strConf += strTmp;
	}

	std::string strShowMsg = m_pStrUtil->Format("Device: %s, Event=ConferencedEvent, State: [%s], ConfCallRefId: [%u],"
		"MemCount=%d, CallRefId1=%d, Dn1: [%s], CallRefId2: [%u], Dn2: [%s], CtrlDn: [%s], AddDn: [%s],"
		"ConfDn: [%s] ,ConfStaticDn: [%s] ,EventCause: [%s], ConfMems %s",
		strDeviceNum.c_str(), strState.c_str(), lConfCallRefId,
		iMemCount, lCallRefId1, strDn1.c_str(), lCallRefId2, strDn2.c_str(), strCtrlDn.c_str(), strAddDn.c_str(),
		strConfDn.c_str(), strConfStaticDn.c_str(), strEventCause.c_str(), strConf.c_str());
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	strAddDn = CSysConfig::Instance()->GetCheckNum(strAddDn);

	/*
	91105ring -->conference 91103 press conf
	号码:91105,event=ConferencedEvent,state=CS_ALERTING,ConfCallRefId=742,
	MemCount=3,callRefId1=741,Dn1=91103,CallRefId2=742,Dn2=91103,
	CtrlDn=91103,AddDn=91105,ConfDn=91103,ConfStaticDn=91103,EventCause=EC_NONE
	号码:91103,event=ConferencedEvent,state=CS_CONNECT,ConfCallRefId=742,
	MemCount=3,callRefId1=741,Dn1=91103,CallRefId2=742,Dn2=91103,
	CtrlDn=91103,AddDn=91105,ConfDn=91103,ConfStaticDn=91103,EventCause=EC_NONE
	号码:91104,event=ConferencedEvent,state=CS_CONNECT,ConfCallRefId=742,
	MemCount=3,callRefId1=741,Dn1=91103,CallRefId2=742,Dn2=91103,
	CtrlDn=91103,AddDn=91105,ConfDn=91103,ConfStaticDn=91103,EventCause=EC_NONE

	91105talk -->conference 91103 press conf
	号码:91103,event=ConferencedEvent,state=CS_CONNECT,ConfCallRefId=744,
	MemCount=3,callRefId1=743,Dn1=91103,CallRefId2=744,Dn2=91103,
	CtrlDn=91103,AddDn=91105,ConfDn=91103,ConfStaticDn=91103,EventCause=EC_NONE
	号码:91105,event=ConferencedEvent,state=CS_CONNECT,ConfCallRefId=744,
	MemCount=3,callRefId1=743,Dn1=91103,CallRefId2=744,Dn2=91103,
	CtrlDn=91103,AddDn=91105,ConfDn=91103,ConfStaticDn=91103,EventCause=EC_NONE
	号码:91104,event=ConferencedEvent,state=CS_CONNECT,ConfCallRefId=744,
	MemCount=3,callRefId1=743,Dn1=91103,CallRefId2=744,Dn2=91103,
	CtrlDn=91103,AddDn=91105,ConfDn=91103,ConfStaticDn=91103,EventCause=EC_NONE


	第二方为外线
	号码:91103,event=ConferencedEvent,state=CS_CONNECT,ConfCallRefId=751,
	MemCount=3,callRefId1=750,Dn1=91103,CallRefId2=751,Dn2=91103,
	CtrlDn=91103,AddDn=91105,ConfDn=91103,ConfStaticDn=91103,EventCause=EC_NONE
	号码:91105,event=ConferencedEvent,state=CS_CONNECT,ConfCallRefId=751,
	MemCount=3,callRefId1=750,Dn1=91103,CallRefId2=751,Dn2=91103,
	CtrlDn=91103,AddDn=91105,ConfDn=91103,ConfStaticDn=91103,EventCause=EC_NONE


	被加入方为外线
	号码:91103,event=ConferencedEvent,state=CS_CONNECT,ConfCallRefId=755,
	MemCount=3,callRefId1=754,Dn1=91103,CallRefId2=755,Dn2=91103,
	CtrlDn=91103,AddDn=T755#2,ConfDn=91103,ConfStaticDn=91103,EventCause=EC_NONE
	号码:91105,event=ConferencedEvent,state=CS_CONNECT,ConfCallRefId=755,
	MemCount=3,callRefId1=754,Dn1=91103,CallRefId2=755,Dn2=91103,
	CtrlDn=91103,AddDn=T755#2,ConfDn=91103,ConfStaticDn=91103,EventCause=EC_NONE
	*/

	//确认旧的呼叫ID;
	long lCallOldRefId = (lConfCallRefId == lCallRefId1) ? lCallRefId2 : lCallRefId1;
	long lCallNewRefId = (lConfCallRefId == lCallRefId1) ? lCallRefId1 : lCallRefId2;

	if (strEventCause.compare("EC_ACTIVE_MONITOR") == 0)
	{
		//	强插,话务 ID 不会改变
		CallConference(lCallNewRefId, lCallNewRefId, strDeviceNum, strAddDn);
	}
	else if (strEventCause.compare("EC_SILENT_MONITOR") == 0)
	{
		//	监听
		if (strDeviceNum.compare(strAddDn) == 0)
		{
			CallSilentMonitor(lCallNewRefId, strDeviceNum);
		}
	}
	else
	{
		//	多方会议
		if (CCallManager::Instance()->FindCallByCSTACallRefId(lCallOldRefId))
		{
			std::string strConfCompere = CCallManager::Instance()->GetConfCompere(lCallNewRefId);
			std::string strOriginalCallerId = CCallManager::Instance()->GetOriginalCallerId(lCallNewRefId);
			std::string strOriginalCalledId = CCallManager::Instance()->GetOriginalCalledId(lCallNewRefId);
			std::string strCallState = CCallManager::Instance()->GetCallState(lCallNewRefId);

			CCallManager::Instance()->CopyCall(lCallNewRefId, lCallOldRefId, strCallState);

			CCallManager::Instance()->SetOriginalCallerId(lCallNewRefId, strOriginalCallerId);
			CCallManager::Instance()->SetOriginalCalledId(lCallNewRefId, strOriginalCalledId);
			CCallManager::Instance()->SetConfCompere(lCallNewRefId, strConfCompere);
			CCallManager::Instance()->AddConferenceParty(lCallNewRefId, strAddDn);

			CCallManager::Instance()->DeleteCall(lCallOldRefId);
			CDeviceManager::Instance()->DeleteAllLogicalCallState(lCallOldRefId);
		}

		CDeviceManager::Instance()->UpdateLogicalCallState(lCallNewRefId, strDeviceNum, CallStateString[STATE_CONFERENCE]);
	}
}

void CAvayaEvent::DivertedEvent(long lMonitorId, CSTADivertedEvent_t& diverted)
{
	boost::shared_ptr<CDevice> pDevice = CDeviceManager::Instance()->FindDeviceByMonitorId(lMonitorId);
	if (!pDevice)
	{
		ICC_LOG_WARNING(m_pLog, "DivertedEvent not find device by lMonitorId: [%u]",
			lMonitorId);

		return;
	}

	long lCSTACallRefId = diverted.connection.callID;
	std::string strDn(diverted.connection.deviceID);
	std::string strDivDn(diverted.divertingDevice.deviceID);
	std::string strDest(diverted.newDestination.deviceID);
	std::string strState = CAvayaResult::Instance()->GetConnectionStateString(diverted.localConnectionInfo);
	std::string strEventCause = CAvayaResult::Instance()->GetCSTAEventCauseString(diverted.cause);
	std::string strDeviceNum = pDevice->GetDeviceNum();

	std::string strShowMsg = m_pStrUtil->Format("Device: %s-%s, Event=DivertedEvent, State: [%s], "
		" CallRefId: [%u], DirDnId: [%s], DestDnId: [%s] ,EventCause: [%s]",
		strDeviceNum.c_str(), strDn.c_str(), strState.c_str(), 
		lCSTACallRefId, strDivDn.c_str(), strDest.c_str(), strEventCause.c_str());
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	

	/*
	话务DeflectCall走了
	号码:91103,91103,event=DivertedEvent,state=CS_NULL,callRefId=221,
	DirDnId=91103,DestDnId=91105,EventCause=EC_REDIRECTED

	原先:
	Device2<------call<-----Device1
	Device3

	后来:
	Device2       call<-----Device1
	Device3<--------|

	//119组电话溢出，但溢出位置未知
	号码:119,119,event=DivertedEvent,state=CS_NONE,callRefId=293,
	DirDnId=119,DestDnId=,EventCause=EC_REDIRECTED
	*/
	E_HANGUP_TYPE nHangupType = HANGUP_TYPE_CALLER;

	switch (diverted.localConnectionInfo)
	{
	case CS_CONNECT://signleback
		ICC_LOG_DEBUG(m_pLog, "DivertedEvent CS_CONNECT");
		break;
	case CS_ALERTING://ring
		ICC_LOG_DEBUG(m_pLog, "DivertedEvent CS_ALERTING");
		break;
	case CS_NULL:		
		if (strDeviceNum != strDn)
		{
			ICC_LOG_DEBUG(m_pLog, "device is not equal dn, will not process!! call:[%d]", lCSTACallRefId);
			return;
		}

		//==TpDisconnected
		if (strDest.empty())
		{
			DeviceHangup(lCSTACallRefId, strDeviceNum, strDeviceNum, HangupTypeString[HANGUP_TYPE_TIMEOUT]); //主叫挂断/振铃超时
		}
		else
		{
			// 代答 或 偏转
			// 7001 -》 110，7004，7002代答 7004
			/*Device:7001, 7004, Event = DivertedEvent, State = CS_CONNECT, CallRefId = 568, DirDnId = 7004, DestDnId = 7002, EventCause = EC_REDIRECTED
			Device : 7002, 7004, Event = DivertedEvent, State = CS_ALERTING, CallRefId = 568, DirDnId = 7004, DestDnId = 7002, EventCause = EC_REDIRECTED
			Device : 110, 7004, Event = DivertedEvent, State = CS_NULL, CallRefId = 568, DirDnId = 7004, DestDnId = 7002, EventCause = EC_REDIRECTED
			Device : 7004, 7004, Event = DivertedEvent, State = CS_NULL, CallRefId = 568, DirDnId = 7004, DestDnId = 7002, EventCause = EC_REDIRECTED*/
			DeviceHangup(lCSTACallRefId, strDeviceNum, strDeviceNum, HangupTypeString[HANGUP_TYPE_OP_TRANSFER]);
		}
		break;
	case CS_NONE://溢出
		//7002 振铃超时，溢出
		//Device: 7002-7002, Event=DivertedEvent, State: [CS_NONE],  CallRefId: [335], DirDnId: [7002], DestDnId: [] ,EventCause: [EC_REDIRECTED]
		DeviceHangup(lCSTACallRefId, strDeviceNum, strDeviceNum);
		ICC_LOG_DEBUG(m_pLog, "DivertedEvent CS_NONE");
		break;
	default:
		ICC_LOG_WARNING(m_pLog, "Unhandle Msg: [%s]", strShowMsg.c_str());
		break;
	}
}

void CAvayaEvent::FailedEvent(long lMonitorId, CSTAFailedEvent_t& failed)
{
	boost::shared_ptr<CDevice> pDevice = CDeviceManager::Instance()->FindDeviceByMonitorId(lMonitorId);
	if (!pDevice)
	{
		ICC_LOG_ERROR(m_pLog, "FailedEvent not find device by lMonitorId: [%u]",
			lMonitorId);

		return;
	}

	long lCSTACallRefId = failed.failedConnection.callID;
	std::string strDn(failed.failedConnection.deviceID);
	std::string strFailDn(failed.failingDevice.deviceID);
	std::string strCalledId(failed.calledDevice.deviceID);
	std::string strState = CAvayaResult::Instance()->GetConnectionStateString(failed.localConnectionInfo);
	std::string strEventCause = CAvayaResult::Instance()->GetCSTAEventCauseString(failed.cause);
	std::string strDeviceNum = pDevice->GetDeviceNum();

	std::string strShowMsg = m_pStrUtil->Format("Device: %s-%s, Event=FailedEvent, State: [%s],"
		"CallRefId: [%u], FailedDnId: [%s], CalledId: [%s], EventCause: [%s]",
		strDeviceNum.c_str(), strDn.c_str(), strState.c_str(),
		lCSTACallRefId, strFailDn.c_str(), strCalledId.c_str(), strEventCause.c_str());
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	if (strFailDn.empty() || strFailDn.find("T") != std::string::npos)
	{
		strFailDn = strCalledId;
	}
	strFailDn = CSysConfig::Instance()->GetCheckNum(strFailDn);

	/*
	号码:91105,T2188#2,event=FailedEvent,state=CS_FAIL,callRefId=2188,
	failedDnId=T2188#2,calledId=,eventCause=EC_DEST_NOT_OBTAINABLE

	号码:91105,91108,event=FailedEvent,state=CS_FAIL,callRefId=2187,
	failedDnId=91108,calledId=91108,eventCause=EC_BUSY

	号码:91501,91501,event=FailedEvent,state=CS_FAIL,callRefId=6196,
	failedDnId=91501,calledId=,eventCause=EC_RESOURCES_NOT_AVAILABLE

	号码:91103,T14150#2,event=FailedEvent,state=CS_FAIL,callRefId=14150,
	failedDnId=,calledId=87211431,eventCause=EC_NETWORK_CONGESTION
	*/

//	E_FAILED_TYPE failedType = FAILED_DEST_INVALID;
	E_ERROR_CODE nErrorCode = ERROR_UNKNOWN;
	switch (failed.cause)
	{
	case EC_RESOURCES_NOT_AVAILABLE:
	{
	//	failedType = FAILED_DEST_INVALID;
		nErrorCode = ERROR_RESOURCES_NOT_AVAILABLE;
		ICC_LOG_ERROR(m_pLog, "EC_RESOURCES_NOT_AVAILABLE");
	}
	break;
	case EC_DEST_NOT_OBTAINABLE://==DestNotObtainable
	{
	//	failedType = FAILED_DEST_NOT_OBTAINABLE;
		nErrorCode = ERROR_DEST_NOT_CONNECT;
		ICC_LOG_ERROR(m_pLog, "EC_DEST_NOT_OBTAINABLE");
	}
	break;
	//case EC_CALL_NOT_ANSWERED:
	//	if (strDn.find("T") != std::string::npos)
	//	{
	//		//CAvayaSwitch::Instance()->ClearCall()
	//		//CCallManager::Instance()->GetE1RelayIndexNumber(lCSTACallRefId, strAlertingDn, strCalledId);
	//	}
	//	break;
	case EC_BUSY://==DestBusy
	{
	//	failedType = FAILED_DEST_BUSY;
		nErrorCode = ERROR_RESOURCE_BUSY;
		ICC_LOG_ERROR(m_pLog, "EC_BUSY");
	}
	break;
	default://==DestBusy
	{
		std::string strShowMsg = CAvayaResult::Instance()->GetCSTAEventCauseString(failed.cause);
		ICC_LOG_ERROR(m_pLog, "%s", strShowMsg.c_str());
	}
	break;
	}

	std::string strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(lCSTACallRefId, strCTICallRefId))
	{
		if (failed.cause == EC_BUSY)
		{
			return;
		}
		strCTICallRefId = CCallManager::Instance()->CreateNewCall(lCSTACallRefId);
	}

	std::vector<CProperty> l_vPropertieQueue;
	SetPropertyValue(l_vPropertieQueue, Pr_CTICallRefId, strCTICallRefId);
	SetPropertyValue(l_vPropertieQueue, Pr_CalledId, strFailDn);
	CCallManager::Instance()->UpdateCall(lCSTACallRefId, l_vPropertieQueue);

	IFailedEventNotifPtr pFailedEventNotif = boost::make_shared<CFailedEventNotif>();
	if (pFailedEventNotif)
	{
		pFailedEventNotif->SetFailedDeviceNum(strDeviceNum);
		pFailedEventNotif->SetCalledId(strFailDn);
		pFailedEventNotif->SetCSTACallRefId(lCSTACallRefId);
		pFailedEventNotif->SetCTICallRefId(strCTICallRefId);
		pFailedEventNotif->SetResult(false);
		pFailedEventNotif->SetErrorCode(nErrorCode);
		pFailedEventNotif->SetErrorMsg(CAvayaResult::Instance()->GetCSTAEventCauseString(failed.cause));

		long lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_FailedEvent, pFailedEventNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask FailedEvent, DeviceNum: [%s], CSTACallRefId: [%u], CTICallRefId: [%s], TaskId: [%u]",
			strDeviceNum.c_str(), lCSTACallRefId, strCTICallRefId.c_str(), lTaskId);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CFailedEventNotif Object Failed !!!");
	}
}

void CAvayaEvent::HeldEvent(long lMonitorId, CSTAHeldEvent_t& held)
{
	boost::shared_ptr<CDevice> pDevice = CDeviceManager::Instance()->FindDeviceByMonitorId(lMonitorId);
	if (!pDevice)
	{
		ICC_LOG_WARNING(m_pLog, "ConferencedEvent not find device by lMonitorId: [%u]",
			lMonitorId);

		return;
	}

	long lCSTACallRefId = held.heldConnection.callID;
	std::string strDn(held.heldConnection.deviceID);
	std::string strHoldDn(held.holdingDevice.deviceID);
	std::string strState = CAvayaResult::Instance()->GetConnectionStateString(held.localConnectionInfo);
	std::string strEventCause = CAvayaResult::Instance()->GetCSTAEventCauseString(held.cause);
	std::string strDeviceNum = pDevice->GetDeviceNum();

	std::string strShowMsg = m_pStrUtil->Format("Device: %s-%s, Event=HeldEvent, State: [%s], CallRefId: [%u], HoldDn: [%s], EventCause: [%s]",
		strDeviceNum.c_str(), strDn.c_str(), strState.c_str(), lCSTACallRefId, strHoldDn.c_str(), strEventCause.c_str());
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	strHoldDn = CSysConfig::Instance()->GetCheckNum(strHoldDn);

	/*
	press hold key
	号码:91103,91103,event=HeldEvent,state=CS_HOLD,callRefId=58,HoldDn=91103,eventCause=EC_NONE
	号码:91104,91103,event=HeldEvent,state=CS_CONNECT,callRefId=58,HoldDn=91103,eventCause=EC_NONE
	号码:91105,91103,event=HeldEvent,state=CS_ALERTING,callRefId=9596,HoldDn=91103,eventCause=EC_NONE

	acd group
	这条是挂机
	号码:110,  48056,event=HeldEvent,state=CS_NONE,callRefId=14778,HoldDn=48056,eventCause=EC_NONE
	这条是分配出去后对方Held
	号码:110,  91104,event=HeldEvent,state=CS_NONE,callRefId=14821,HoldDn=91104,eventCause=EC_NONE
	*/

	/*	咨询，保留(7005咨询7002，7003保留)
	Device:7005,7005, Event=HeldEvent, State=CS_HOLD, CallRefId=343, HoldDn=7005, EventCause=EC_NONE
	Device:7003,7005, Event=HeldEvent, State=CS_CONNECT, CallRefId=343, HoldDn=7005, EventCause=EC_NONE 
	*/

	switch (held.localConnectionInfo)
	{
	case CS_NONE:	//VDN,OpHold,不需要处理
		//CCallState::PostMsgToLogic(PHY_EVENT_TPDISCONNECTED,m_iLogicCur,lCSTACallRefId,0,"","");
		//TS_DeleteTSCallFromQueue(lCSTACallRefId);
		return;
	case CS_HOLD: //TpHold
		CallHold(lCSTACallRefId, strDeviceNum);
		break;
	case CS_CONNECT://OpHold
		CallHold(lCSTACallRefId, strDeviceNum);
		break;
	case CS_ALERTING://OpHold
		//CCallState::PostMsgToLogic(PHY_EVENT_OPHELD,m_iLogicCur, lCSTACallRefId,0,"","");
		break;
	default:
		ICC_LOG_WARNING(m_pLog, "Unhandle Msg: [%s]", strShowMsg.c_str());
		break;
	}
}

void CAvayaEvent::TransferredEvent(long lMonitorId, CSTATransferredEvent_t& transferred)
{
	boost::shared_ptr<CDevice> pDevice = CDeviceManager::Instance()->FindDeviceByMonitorId(lMonitorId);
	if (!pDevice)
	{
		ICC_LOG_WARNING(m_pLog, "ConferencedEvent not find device by lMonitorId: [%u]",
			lMonitorId);

		return;
	}

	long lCallRefId1 = transferred.primaryOldCall.callID;
	std::string strDn1(transferred.primaryOldCall.deviceID);

	long lCallRefId2 = transferred.secondaryOldCall.callID;
	std::string strDn2(transferred.secondaryOldCall.deviceID);

	std::string strTransferDn(transferred.transferringDevice.deviceID);
	std::string strTransfedDn(transferred.transferredDevice.deviceID);

	int iMemCount = transferred.transferredConnections.count;
	long lCallConRefId = transferred.transferredConnections.connection->party.callID;
	std::string strTranfConDn(transferred.transferredConnections.connection->party.deviceID);
	std::string strTranfConStaticDn(transferred.transferredConnections.connection->staticDevice.deviceID);
	std::string strState = CAvayaResult::Instance()->GetConnectionStateString(transferred.localConnectionInfo);
	std::string strEventCause = CAvayaResult::Instance()->GetCSTAEventCauseString(transferred.cause);
	std::string strDeviceNum = pDevice->GetDeviceNum();

	std::string strShowMsg = m_pStrUtil->Format("Device: %s, Event=TransferredEvent, State: [%s], CallConRefId: [%u], CallRefId1: [%u], Dn1: [%s],"
		"CallRefId2: [%u], Dn2: [%s],TransferDn: [%s],TransfedDn: [%s],MemCount=%d, "
		"TransfConDn: [%s],TransfConStaticDn: [%s], EventCause: [%s]",
		strDeviceNum.c_str(), strState.c_str(), lCallConRefId, lCallRefId1, strDn1.c_str(),
		lCallRefId2, strDn2.c_str(), strTransferDn.c_str(), strTransfedDn.c_str(), iMemCount,
		strTranfConDn.c_str(), strTranfConStaticDn.c_str(), strEventCause.c_str());
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);
/*
7001 -> 7002, 7002 转 7004（Talk 后 转移）
Device:7002, Event = TransferredEvent, State = CS_NULL, CallConRefId = 60, CallRefId1 = 59, Dn1 = 7002, CallRefId2 = 60, 
Dn2 = 7002, TransferDn = 7002, TransfedDn = 7004, MemCount = 2, TransfConDn = 7001, TransfConStaticDn = 7001, EventCause = EC_TRANSFER

Device:7001, Event=TransferredEvent, State=CS_CONNECT, CallConRefId=60, CallRefId1=59, Dn1=7002,CallRefId2=60, Dn2=7002,
TransferDn=7002,TransfedDn=7004,MemCount=2, TransfConDn=7001,TransfConStaticDn=7001, EventCause=EC_TRANSFER

Device:7004, Event=TransferredEvent, State=CS_CONNECT, CallConRefId=60, CallRefId1=59, Dn1=7002,CallRefId2=60, Dn2=7002,
TransferDn=7002,TransfedDn=7004,MemCount=2, TransfConDn=7001,TransfConStaticDn=7001, EventCause=EC_TRANSFER
*/

	switch (transferred.localConnectionInfo)
	{
	case CS_ALERTING:	// 转移目标方振铃
	{
		CallAlerting(lCallConRefId, strTransferDn, strTransfedDn, strTransfedDn);
	}
		break;;
	case CS_CONNECT:	 // 转移目标方通话
	{
		//
	}
		break;
	case CS_NULL:		// 原话务，转移发起方挂断
	{
		DeviceHangup(lCallRefId1, strTranfConStaticDn, strTranfConStaticDn, HangupTypeString[HANGUP_TYPE_TRANSFER]);	//原话务，主叫方挂机
		DeviceHangup(lCallRefId1, strTransferDn, strDeviceNum, HangupTypeString[HANGUP_TYPE_TRANSFER]);					//原话务，转移发起方挂机

		//	转移新话务，发起方挂机
		DeviceHangup(lCallConRefId, strTransferDn, strDeviceNum, HangupTypeString[HANGUP_TYPE_TP_TRANSFER]);
	}
	break;
	case CS_NONE:
	{
		std::string strDeviceType = CDeviceManager::Instance()->GetDeviceType(strDeviceNum);
		if (strDeviceType.compare(DEVICE_TYPE_ACDGROUP) == 0 || strDeviceType.compare(DEVICE_TYPE_ACDSUPER) == 0)
		{
			CCallManager::Instance()->SetOriginalCallerId(lCallConRefId, strTranfConStaticDn);
			CCallManager::Instance()->SetOriginalCalledId(lCallConRefId, strTransferDn);

			// 转外线，ACD 组状态
			CDeviceManager::Instance()->UpdateLogicalCallState(lCallConRefId, strDeviceNum, CallStateString[STATE_TALK]);
		}
	}
	break;
	default:
	{
		ICC_LOG_WARNING(m_pLog, "Unhandle Msg: %s", strShowMsg.c_str());
	}
	break;
	}
	/*
	ring --> transfer 91103press transfer
	号码:91105,event=TransferredEvent,state=CS_ALERTING,callrefId=657,callRefId1=656,
	Dn1=91103,callRefId2=657,Dn2=91103,TransferDn=91103,TransfedDn=91105,
	MemCount=2,TransfConDn=91104,TransfConStaticDn=91104,EventCause=EC_TRANSFER

	号码:91103,event=TransferredEvent,state=CS_NULL,callrefId=657,callRefId1=656,
	Dn1=91103,callRefId2=657,Dn2=91103,TransferDn=91103,TransfedDn=91105,
	MemCount=2,TransfConDn=91104,TransfConStaticDn=91104,EventCause=EC_TRANSFER

	号码:91104,event=TransferredEvent,state=CS_CONNECT,callrefId=657,callRefId1=656,
	Dn1=91103,callRefId2=657,Dn2=91103,TransferDn=91103,TransfedDn=91105,
	MemCount=2,TransfConDn=91104,TransfConStaticDn=91104,EventCause=EC_TRANSFER


	talk-->transfer91103press transfer
	号码:91103,event=TransferredEvent,state=CS_NULL,callrefId=660,callRefId1=659,
	Dn1=91103,callRefId2=660,Dn2=91103,TransferDn=91103,TransfedDn=91105,
	MemCount=2,TransfConDn=91104,TransfConStaticDn=91104,EventCause=EC_TRANSFER

	号码:91104,event=TransferredEvent,state=CS_CONNECT,callrefId=660,callRefId1=659,
	Dn1=91103,callRefId2=660,Dn2=91103,TransferDn=91103,TransfedDn=91105,
	MemCount=2,TransfConDn=91104,TransfConStaticDn=91104,EventCause=EC_TRANSFER

	号码:91105,event=TransferredEvent,state=CS_CONNECT,callrefId=660,callRefId1=659,
	Dn1=91103,callRefId2=660,Dn2=91103,TransferDn=91103,TransfedDn=91105,
	MemCount=2,TransfConDn=91104,TransfConStaticDn=91104,EventCause=EC_TRANSFER
	*/
}

void CAvayaEvent::RetrievedEvent(long lMonitorId, CSTARetrievedEvent_t& retrieved)
{
	boost::shared_ptr<CDevice> pDevice = CDeviceManager::Instance()->FindDeviceByMonitorId(lMonitorId);
	if (!pDevice)
	{
		ICC_LOG_WARNING(m_pLog, "ConferencedEvent not find device by lMonitorId: [%u]",
			lMonitorId);

		return;
	}

	long lCSTACallRefId = retrieved.retrievedConnection.callID;
	std::string strDn(retrieved.retrievedConnection.deviceID);
	std::string strRetDn(retrieved.retrievingDevice.deviceID);
	std::string strState = CAvayaResult::Instance()->GetConnectionStateString(retrieved.localConnectionInfo);
	std::string strEventCause = CAvayaResult::Instance()->GetCSTAEventCauseString(retrieved.cause);
	std::string strDeviceNum = pDevice->GetDeviceNum();

	std::string strShowMsg = m_pStrUtil->Format("Device: %s-%s, Event=RetrievedEvent, State: [%s], CallRefId: [%u], RetDn: [%s], EventCause: [%s]",
		strDeviceNum.c_str(), strDn.c_str(), strState.c_str(), lCSTACallRefId, strRetDn.c_str(), strEventCause.c_str());
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	strRetDn = CSysConfig::Instance()->GetCheckNum(strRetDn);

	/*
	号码:91103,91103,event=RetrievedEvent,state=CS_CONNECT,callRefId=143,
	RetDn=91103,eventCause=EC_NONE
	号码:91104,91103,event=RetrievedEvent,state=CS_CONNECT,callRefId=143,
	RetDn=91103,eventCause=EC_NONE
	*/
	switch (retrieved.localConnectionInfo)
	{
	case CS_CONNECT:
	{
		if (strDeviceNum.compare(strRetDn) == 0)
		{
			CallRetrieve(lCSTACallRefId, strRetDn);
		}
		else
		{
			CallRetrieve(lCSTACallRefId, strDeviceNum);
		}
			
	}
	break;
	default:
		break;
	}
}

void CAvayaEvent::MonitorEndEvent(long lMonitorId, CSTAMonitorEndedEvent_t& monitorEnded)
{
	boost::shared_ptr<CDevice> pDevice = CDeviceManager::Instance()->FindDeviceByMonitorId(lMonitorId);
	if (!pDevice)
	{
		ICC_LOG_ERROR(m_pLog, "MonitorEndEvent not find device by lMonitorId: [%u]",
			lMonitorId);

		return;
	}

	std::string strEventCause = CAvayaResult::Instance()->GetCSTAEventCauseString(monitorEnded.cause);
	std::string strDeviceNum = pDevice->GetDeviceNum();

	/*
	1,当监视着数字分机时，在维护台删除不掉数字分机。
	2,当正监视着这个分机(LineSide)时，在维护台删除该分机，则出现该事件
	号码:4829,event=MonitorEndEvent,EventCause=EC_RESOURCES_NOT_AVAILABLE
	3,对交换机做复位操作，也会出现该事件
	*/

	ICC_LOG_DEBUG(m_pLog, "Device: [%s], event=MonitorEndEvent, EventCause: [%s]",
		strDeviceNum.c_str(), strEventCause.c_str());

	// 重新监视设备
	CDeviceManager::Instance()->ResetMonitorDevice(strDeviceNum);
	//CDeviceManager::Instance()->StartMonitorThread();

	// 重新发送monitor事件
	/*IMonitorDeviceNotifPtr l_pRequestNotif = boost::make_shared<CMonitorDeviceNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetDeviceNum(strDeviceNum);

		ITaskPtr l_pTask = CTaskManager::Instance()->CreateNewTask();
		if (l_pTask)
		{
			long l_lTaskId = l_pTask->GetTaskId();

			l_pRequestNotif->SetRequestId(l_lTaskId);
			l_pTask->SetSwitchNotif(l_pRequestNotif);

			std::string strDeviceType = CDeviceManager::Instance()->GetDeviceType(strDeviceNum);
			if (strDeviceType.compare(DEVICE_TYPE_ACDGROUP) == 0)
			{
				l_pTask->SetTaskName(Task_MonitorCallVirDevice);
			}
			else
			{
				l_pTask->SetTaskName(Task_MonitorDevice);
			}

			ICC_LOG_DEBUG(m_pLog, "AddCmdTask ReMonitorDevice, TaskId: [%u] Device: [%s]",
				l_lTaskId, strDeviceNum.c_str());
			CTaskManager::Instance()->AddCmdTask(l_pTask);
		}
		else
		{
			ICC_LOG_WARNING(m_pLog, "ReMonitorDevice, Device: [%s], Create Task Obj Failed !!!",
				strDeviceNum.c_str());
		}
	}
	else
	{
		ICC_LOG_WARNING(m_pLog, "ReMonitorDevice, Device: [%s], Create MonitorDeviceNotif Obj Failed !!!",
			strDeviceNum.c_str());
	}*/
}

void CAvayaEvent::RouteRequestExtEvent(long lnvokeID, CSTARouteRequestExtEvent_t& routeRequestExtEvent)
{
	long lReqID = routeRequestExtEvent.routeRegisterReqID;
	long lCSTACallRefId = routeRequestExtEvent.routingCrossRefID;
	std::string strCallerId(routeRequestExtEvent.callingDevice.deviceID);
	std::string strDeviceNum(routeRequestExtEvent.currentRoute.deviceID);
	std::string  strRoutedNum(routeRequestExtEvent.routedCall.deviceID);

	ICC_LOG_DEBUG(m_pLog, "Event=RouteRequestExt, RouteRegisterReqID: [%u], RoutingCrossRefID: [%u],"
		"CallingDevice.deviceID: [%s], CurrentRoute.deviceID: [%s], RoutedCall.deviceID: [%s] ",
		lReqID, lCSTACallRefId, 
		strCallerId.c_str(), strDeviceNum.c_str(), strRoutedNum.c_str());

	strCallerId = CSysConfig::Instance()->GetCheckNum(strCallerId);

	bool bIsBlackCall = false;
	if (CBlackListManager::Instance()->FindBlackNumber(strCallerId))
	{
		// 黑名单检测
		bIsBlackCall = true;
	}

	// 重新路由
	IRouteCallNotifPtr pSwitchNotif = boost::make_shared<CRouteCallNotif>();
	if (pSwitchNotif)
	{
		pSwitchNotif->SetRequestId(lnvokeID);
		pSwitchNotif->SetCallerId(strCallerId);
		pSwitchNotif->SetRouteDest(CSysConfig::Instance()->GetRouteDest());
		pSwitchNotif->SetCSTACallRefId(lCSTACallRefId);
		pSwitchNotif->SetRegisterReqId(lReqID);
		pSwitchNotif->SetCrossRefId(lCSTACallRefId);
		pSwitchNotif->SetIsBlackCall(bIsBlackCall);

		long lTaskId = CTaskManager::Instance()->AddCmdTask(Task_RouteRequest, pSwitchNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask RouteRequest, TaskId[%u]", lTaskId);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CRouteCallNotif Object Failed !!!");
	}
}

void CAvayaEvent::NetworkReachedEvent(long lMonitorId, CSTANetworkReachedEvent_t& networkReached)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_NETWORK_REACHED");
}

void CAvayaEvent::CallInformationEvent(long lMonitorId, CSTACallInformationEvent_t& callInformation)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_CALL_INFORMATION");
}

void CAvayaEvent::DoNotDisturbEvent(long lMonitorId, CSTADoNotDisturbEvent_t& doNotDisturb)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_DO_NOT_DISTURB");
}

void CAvayaEvent::ForwardingEvent(long lMonitorId, CSTAForwardingEvent_t&	forwarding)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_FORWARDING");
}

void CAvayaEvent::MessageWaitingEvent(long lMonitorId, CSTAMessageWaitingEvent_t& messageWaiting)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_MESSAGE_WAITING");
}

void CAvayaEvent::LoggedOnEvent(long lMonitorId, CSTALoggedOnEvent_t& loggedOn)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_LOGGED_ON");
}

void CAvayaEvent::LoggedOffEvent(long lMonitorId, CSTALoggedOffEvent_t& loggedOff)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_LOGGED_OFF");
}

void CAvayaEvent::NotReadyEvent(long lMonitorId, CSTANotReadyEvent_t& notReady)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_NOT_READY");
}

void CAvayaEvent::ReadyEvent(long lMonitorId, CSTAReadyEvent_t& ready)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_READY");
}

void CAvayaEvent::WorkNotReadyEvent(long lMonitorId, CSTAWorkNotReadyEvent_t& workNotReady)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_WORK_NOT_READY");
}

void CAvayaEvent::WorkReadyEvent(long lMonitorId, CSTAWorkReadyEvent_t& workReady)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_WORK_READY");
}

void CAvayaEvent::BackInServiceEvent(long lMonitorId, CSTABackInServiceEvent_t& backInService)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_BACK_IN_SERVICE");
}

void CAvayaEvent::OutOfServiceEvent(long lMonitorId, CSTAOutOfServiceEvent_t& outOfService)
{
	ICC_LOG_WARNING(m_pLog, "CSTA_OUT_OF_SERVICE");
}

void CAvayaEvent::PrivateStatusEvent(long lMonitorId, CSTAPrivateStatusEvent_t& privateStatus)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_PRIVATE_STATUS");
}

//////////////////////////////////////////////////////////////////////////
void CAvayaEvent::ClearCallConf(long invokeID, CSTAClearCallConfEvent_t& clearCall)
{
	ICC_LOG_DEBUG(m_pLog, "InvokeID: [%u], CSTA_CLEAR_CALL_CONF  Event=ClearCallRet",
		invokeID);

	PostSwitchConf(invokeID, Task_ClearCallConf);
}

void CAvayaEvent::ClearConnectionConf(long invokeID, CSTAClearConnectionConfEvent_t& clearConnection)
{
	ICC_LOG_DEBUG(m_pLog, "InvokeID: [%u], CSTA_CLEAR_CONNECT_CONF  Event=ClearConnectionConf",
		invokeID);

	PostSwitchConf(invokeID, Task_ClearConnectionConf);
}

void CAvayaEvent::DeflectCallConf(long invokeID, CSTADeflectCallConfEvent_t& deflectCall)
{
	ICC_LOG_DEBUG(m_pLog, "InvokeID: [%u], CSTA_DEFELECT_CALL_CONF  Event=DeflectCallRet", 
		invokeID);

	PostSwitchConf(invokeID, Task_DeflectCallConf);
}

void CAvayaEvent::PickupCallConf(long invokeID, CSTAPickupCallConfEvent_t& pickupCall)
{
	ICC_LOG_DEBUG(m_pLog, "InvokeID: [%u], CSTA_PICKUP_CALL_CONF Event=PickupCallRet",
		invokeID);

	PostSwitchConf(invokeID, Task_PickupCallConf);
}

void CAvayaEvent::EscapeServiceConf(long invokeID, CSTAEscapeSvcConfEvent_t& escapeService, ATTEvent_t* pATTEvent)
{
	ICC_LOG_DEBUG(m_pLog, "InvokeID: [%u], EscapeServiceConf CSTA_ESCAPE_SVC_CONF",
		invokeID);

	if (pATTEvent == NULL)
	{
		return;
	}

	//TSAPI的私有函数返回值
	switch (pATTEvent->eventType)
	{
	case ATT_SINGLE_STEP_CONFERENCE_CALL:
	{
		ICC_LOG_DEBUG(m_pLog, "EscapeServiceConf ATT_SINGLE_STEP_CONFERENCE_CALL");

		break;
	}
	case ATT_SINGLE_STEP_CONFERENCE_CALL_CONF:
	{
		ICC_LOG_DEBUG(m_pLog, "EscapeServiceConf ATT_SINGLE_STEP_CONFERENCE_CALL_CONF");
		SingleStepConferenceConf(invokeID, &pATTEvent->u.ssconference);

		break;
	}
	default:
		break;
	}
}

void CAvayaEvent::ConsultationConf(long invokeID, CSTAConsultationCallConfEvent_t& consultationCall)
{
	long lCSTACallRefId = consultationCall.newCall.callID;
	std::string strDn = std::string(consultationCall.newCall.deviceID);

	ICC_LOG_DEBUG(m_pLog, "InvokeID: [%u], CSTA_CONSULTATION_CALL_CONF Event=ConsultationRet, NewCall: [%s], DeviceID: [%s]",
		invokeID, CAvayaResult::Instance()->GetConnectionID_tString(consultationCall.newCall).c_str(), strDn.c_str());

	std::string strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(lCSTACallRefId, strCTICallRefId))
	{
		strCTICallRefId = CCallManager::Instance()->CreateNewCall(lCSTACallRefId);
	}

	IConsultationCallResultNotifPtr pResultNotif = boost::make_shared<CConsultationCallResultNotif>();
	if (pResultNotif)
	{
		pResultNotif->SetRequestId(invokeID);
		pResultNotif->SetResult(true);
		pResultNotif->SetActiveCSTACallRefId(lCSTACallRefId);
		pResultNotif->SetActiveCTICallRefId(strCTICallRefId);
		pResultNotif->SetSponsor(strDn);

		long lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_ConsultationCallConf, pResultNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask ConsultationCallConf, TaskId: [%u], RequestId: [%u]",
			lTaskId, invokeID);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CConsultationCallResultNotif Object Failed !!!");
	}
}

void CAvayaEvent::TransferCallConf(long invokeID, CSTATransferCallConfEvent_t& transferCall)
{
	long lCSTACallRefId = transferCall.newCall.callID;
	std::string strDn = std::string(transferCall.newCall.deviceID);
	int iCount = transferCall.connList.count;
	Connection_t* pList = transferCall.connList.connection;

	std::string strConf = ""; 
	for (int i = 0; i < iCount; i++)
	{
		std::string strTmp = m_pStrUtil->Format(" Confidx %d call %d %s",
			i, i, CAvayaResult::Instance()->GetConnectionID_tString((pList++)->party).c_str());
		strConf += strTmp;
	}
	ICC_LOG_DEBUG(m_pLog, "InvokeID: [%u], Event=TransferCallRet NewCall %s ConnList.count %d  Connection %s",
		invokeID, CAvayaResult::Instance()->GetConnectionID_tString(transferCall.newCall).c_str(), iCount, strConf.c_str());
	//Event=TransferCallRet NewCall CallId: 344, DeviceId: 0, DeviceType: 1 ConnList.count 2  Connection   
	//Confidx 0 call 0 CallId: 344, DeviceId : 7003, DeviceType : 0

	std::string strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(lCSTACallRefId, strCTICallRefId))
	{
		strCTICallRefId = CCallManager::Instance()->CreateNewCall(lCSTACallRefId);
	}

	ITransferCallResultNotifPtr pResult = boost::make_shared<CTransferCallResultNotif>();
	if (pResult)
	{
		pResult->SetActiveCSTACallRefId(lCSTACallRefId);
		pResult->SetActiveCTICallRefId(strCTICallRefId);
		pResult->SetRequestId(invokeID);
		pResult->SetResult(true);

		long lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_TransferCallConf, pResult);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask TransferCallConf, TaskId: [%u], RequestId: [%u]",
			lTaskId, invokeID);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CTransferCallResultNotif Object Failed !!!");
	}
}

void CAvayaEvent::HoldCallConf(long invokeID, CSTAHoldCallConfEvent_t& holdCall)
{
	ICC_LOG_DEBUG(m_pLog, "InvokeID: [%u], Event=HoldCallConf",
		invokeID);

	PostSwitchConf(invokeID, Task_HoldCallConf);
}

void CAvayaEvent::RetrieveCallConf(long invokeID, CSTARetrieveCallConfEvent_t& retrieveCall)
{
	ICC_LOG_DEBUG(m_pLog, "InvokeID: [%u], Event=RetrieveCallConf",
		invokeID);

	PostSwitchConf(invokeID, Task_RetrieveCallConf);
}

void CAvayaEvent::ConferenceConf(long invokeID, CSTAConferenceCallConfEvent_t& conferenceCall)
{
	long lCSTACallRefId = conferenceCall.newCall.callID;
	std::string strDn(conferenceCall.newCall.deviceID);
	int iCount = conferenceCall.connList.count;
	Connection_t* pList = conferenceCall.connList.connection;

	std::vector<std::string> confPartys;
	std::string strConf = "";
	for (int i = 0; i < iCount; i++)
	{
		confPartys.push_back(std::string(pList->party.deviceID));

		std::string strTmp = m_pStrUtil->Format(" confidx %d call%d %s",
			i, i, CAvayaResult::Instance()->GetConnectionID_tString((pList++)->party).c_str());
		strConf += strTmp;
	}

	ICC_LOG_DEBUG(m_pLog, "InvokeID: [%u], Event=ConferenceCallConf NewCall %s, NewCallID %u, ConnList.count %d  Connection %s",
		invokeID, strDn.c_str(), lCSTACallRefId, iCount, strConf.c_str());

	std::string strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(lCSTACallRefId, strCTICallRefId))
	{
		strCTICallRefId = CCallManager::Instance()->CreateNewCall(lCSTACallRefId);
	}

	PostSwitchConf(invokeID, Task_ConferenceCallConf);
}

void CAvayaEvent::SingleStepConferenceConf(long invokeID, ATTSingleStepConferenceCallConfEvent_t* ssconference)
{
	long lNewCallRefId = ssconference->newCall.callID;
	std::string strDn = std::string(ssconference->newCall.deviceID);
	//ssconference->newCall.devIDType;

	int iCount = ssconference->connList.count;
	Connection_t * pList = ssconference->connList.connection;

	std::vector<std::string> confPartys;
	std::string strConf, strTmp;
	for (int i = 0; i < iCount; i++)
	{
		confPartys.push_back(std::string(pList->party.deviceID));

		strTmp = m_pStrUtil->Format(" confidx %d call%d %s",
			i, i, CAvayaResult::Instance()->GetConnectionID_tString((pList++)->party).c_str());
		strConf += strTmp;
	}
	ICC_LOG_DEBUG(m_pLog, "InvokeID: [%u], Event=SingleStepConferenceConf NewCall %s, NewCallID %u, ConnList.count %d  Connection %s ",
		invokeID, strDn.c_str(), lNewCallRefId, iCount, strConf.c_str());

	std::string strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(lNewCallRefId, strCTICallRefId))
	{
		strCTICallRefId = CCallManager::Instance()->CreateNewCall(lNewCallRefId);
	}

	PostSwitchConf(invokeID, Task_SingleStepConferenceConf);
}

void CAvayaEvent::RouteRegisterReqConf(long invokeID, CSTARouteRegisterReqConfEvent_t& routeRegister)
{
	long routeRegId = routeRegister.registerReqID;

	ICC_LOG_DEBUG(m_pLog, "InvokeID: [%u], RegisterReqID: [%u], Event=RouteRegisterReqConf register success.",
		invokeID, routeRegId);

	IRouteRegisterResultNotifPtr pResultNotif = boost::make_shared<CRouteRegisterResultNotif>();
	if (pResultNotif)
	{
		pResultNotif->SetRequestId(invokeID);
		pResultNotif->SetRouteRegId(routeRegId);
		pResultNotif->SetResult(true);

		long lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_RouteRegisterConf, pResultNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask RouteRegisterConf, TaskId: [%u], RequestId: [%u]",
			lTaskId, invokeID);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CRouteRegisterResultNotif Object Failed !!!");
	}
}

void CAvayaEvent::AlternateCallConf(long invokeID, CSTAAlternateCallConfEvent_t& alternateCall)
{
	ICC_LOG_DEBUG(m_pLog, "InvokeID: [%u], AlternateCallConf",
		invokeID);
}

void CAvayaEvent::CallCompletionRet(long invokeID, CSTACallCompletionConfEvent_t& callCompletion)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_CALL_COMPLETION_CONF");
}

void CAvayaEvent::GroupPickupCallRet(long invokeID, CSTAGroupPickupCallConfEvent_t& groupPickupCall)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_GROUP_PICKUP_CALL_CONF");
}

void CAvayaEvent::MakePredictiveCallRet(long invokeID, CSTAMakePredictiveCallConfEvent_t& makePredictiveCall)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_MAKE_PREDICTIVE_CALL_CONF");
}

void CAvayaEvent::QueryMwiRet(long invokeID, CSTAQueryMwiConfEvent_t& queryMwi)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_QUERY_MWI_CONF");
}

void CAvayaEvent::QueryDndRet(long invokeID, CSTAQueryDndConfEvent_t& queryDnd)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_QUERY_DND_CONF");
}

void CAvayaEvent::QueryFwdRet(long invokeID, CSTAQueryFwdConfEvent_t& queryFwd)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_QUERY_FWD_CONF");
}

void CAvayaEvent::QueryLastNumberRet(long invokeID, CSTAQueryLastNumberConfEvent_t& queryLastNumber)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_QUERY_LAST_NUMBER_CONF");
}

void CAvayaEvent::ReconnectCallRet(long invokeID, CSTAReconnectCallConfEvent_t& reconnectCall)
{
	ICC_LOG_DEBUG(m_pLog, "InvokeID: [%u], CSTA_RECONNECT_CALL_CONF, Event=ReconnectCallConf ",
		invokeID);

	PostSwitchConf(invokeID, Task_ReconnectCallConf);
}

void CAvayaEvent::SetMwiRet(long invokeID, CSTASetMwiConfEvent_t& setMwi)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_SET_MWI_CONF");
}

void CAvayaEvent::SetDndRet(long invokeID, CSTASetDndConfEvent_t& setDnd)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_SET_DND_CONF");
}

void CAvayaEvent::SetFwdRet(long invokeID, CSTASetFwdConfEvent_t& setFwd)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_SET_FWD_CONF");
}

void CAvayaEvent::RouteRegisterCancelRet(long invokeID, CSTARouteRegisterCancelConfEvent_t& routeCancel)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_ROUTE_REGISTER_CANCEL_CONF");
}

void CAvayaEvent::SysStatStartRet(long invokeID, CSTASysStatStartConfEvent_t& sysStatStart)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_SYS_STAT_START_CONF");
}

void CAvayaEvent::SysStatStopRet(long invokeID, CSTASysStatStopConfEvent_t& sysStatStop)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_SYS_STAT_STOP_CONF");
}

void CAvayaEvent::ChangeSysStatFilterRet(long invokeID, CSTAChangeSysStatFilterConfEvent_t& changeSysStatFilter)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_CHANGE_SYS_STAT_FILTER_CONF");
}

void CAvayaEvent::ChangeMonitorFilterRet(long invokeID, CSTAChangeMonitorFilterConfEvent_t& changeMonitorFilter)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_CHANGE_MONITOR_FILTER_CONF");
}

void CAvayaEvent::MonitorStopRet(long invokeID, CSTAMonitorStopConfEvent_t& monitorStop)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_MONITOR_STOP_CONF");
}

void CAvayaEvent::SnapshotCallRet(long invokeID, CSTASnapshotCallConfEvent_t& snapshotCall)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_SNAPSHOT_CALL_CONF");
}

void CAvayaEvent::SnapshotDeviceRet(long invokeID, CSTASnapshotDeviceConfEvent_t& snapshotDevice)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_SNAPSHOT_DEVICE_CONF");
}

void CAvayaEvent::GetAPICapsRet(long invokeID, CSTAGetAPICapsConfEvent_t& getAPICaps)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_GETAPI_CAPS_CONF");
}

void CAvayaEvent::GetDeviceListRet(long invokeID, CSTAGetDeviceListConfEvent_t& getDeviceList)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_GET_DEVICE_LIST_CONF");
}

void CAvayaEvent::QueryCallMonitorRet(long invokeID, CSTAQueryCallMonitorConfEvent_t& queryCallMonitor)
{
	ICC_LOG_DEBUG(m_pLog, "CSTA_QUERY_CALL_MONITOR_CONF");
}

void CAvayaEvent::QueryAgentStateConf(long invokeID, CSTAQueryAgentStateConfEvent_t& queryAgentState)
{
	std::string strAgentState = CAvayaResult::Instance()->GetAgentStateString(queryAgentState);

	ICC_LOG_LOWDEBUG(m_pLog, "InvokeID: [%u], Event=QueryAgentStateConf, State: [%s]-[%u]",
		invokeID, strAgentState.c_str(), queryAgentState.agentState);

	IQueryAgentStateResultNotifPtr pResultNotif = boost::make_shared<CQueryAgentStateResultNotif>();
	if (pResultNotif)
	{
		pResultNotif->SetRequestId(invokeID);
		pResultNotif->SetAgentState(strAgentState);
		pResultNotif->SetResult(true);

		long lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_QueryAgentStateRet, pResultNotif);
		ICC_LOG_LOWDEBUG(m_pLog, "AddSwitchEventTask QueryAgentStateRet, TaskId: [%u], RequestId: [%u]",
			lTaskId, invokeID);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CQueryAgentStateResultNotif Object Failed !!!");
	}
}

void CAvayaEvent::SetAgentStateConf(long invokeID, CSTASetAgentStateConfEvent_t& setAgentState)
{
	ICC_LOG_DEBUG(m_pLog, "InvokeID: [%u], Event=SetAgentStateConf",
		invokeID);

	ISetAgentStateResultNotifPtr pResultNotif = boost::make_shared<CSetAgentStateResultNotif>();
	if (pResultNotif)
	{
		pResultNotif->SetRequestId(invokeID);
		pResultNotif->SetResult(true);

		long lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_SetAgentStateConf, pResultNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask SetAgentStateConf, TaskId: [%u], RequestId: [%u]",
			lTaskId, invokeID);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSetAgentStateResultNotif Object Failed !!!");
	}
}

void CAvayaEvent::UniversalFailRet(long invokeID, CSTAUniversalFailureConfEvent_t& universalFailure)
{
	std::string strError = CAvayaResult::Instance()->GetCSTAUnsoliEventString(universalFailure.error);
	int nErrorCode = CAvayaResult::Instance()->GetCSTAUnsoliEventErrorCode(universalFailure.error);

	ICC_LOG_ERROR(m_pLog, "InvokeId: [%u], CSTA_UNIVERSAL_FAILURE_CONF Error: [%s]", 
		invokeID, strError.c_str());

	PostSwitchConf(invokeID, Task_UniversalFailRet, false, nErrorCode, strError);

	// switch universalFailure.error
	// GENERIC_UNSPECIFIED --- Agentlogout,已经logout状态, 执行了logout动作
	// RESOURCE_BUSY	   --- 执行了logout后，摘机, 正在振铃，正在打电话, 再登录有该事件
}

void CAvayaEvent::QueryDeviceInfoConf(long invokeID, CSTAQueryDeviceInfoConfEvent_t& queryDeviceInfo)
{
	/*ICC_LOG_DEBUG(m_pLog, "InvokeID: %u, CSTA_QUERY_DEVICE_INFO_CONF, event=QueryDeviceInfoConf", 
		invokeID);*/

	IQueryDeviceInfoResultNotifPtr pResultNotif = boost::make_shared<CQueryDeviceInfoResultNotif>();
	if (pResultNotif)
	{
		pResultNotif->SetRequestId(invokeID);
		pResultNotif->SetResult(true);
		pResultNotif->SetDeviceNum(queryDeviceInfo.device);

		long lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_QueryDeviceInfoRet, pResultNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask QueryDeviceInfoRet, TaskId: [%u], RequestId: [%u]",
			lTaskId, invokeID);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CQueryDeviceInfoResultNotif Object Failed !!!");
	}
}

void CAvayaEvent::SysStatReqConf(long invokeID, CSTASysStatReqConfEvent_t& SysStatReq)
{
	/*unsigned int nStatus = SysStatReq.systemStatus;

	NotifSysStatReqConf *pact =new NotifSysStatReqConf();
	if(NULL != pact)
	{
	// 设置成功
	pact->SetStatus(nStatus);
	pact->SetTaskID(invokeID);

	CObserverCenter::GetInstance().PostNotification(pact);
	pact->release();
	}*/
}


//////////////////////////////////////////////////////////////////////////
void CAvayaEvent::DeviceOffHook(long lCSTACallRefId, const std::string& strOffHookDevice)
{
	std::string strShowMsg = m_pStrUtil->Format("Device: [%s] OffHook, CallerId: [%s], CallRefID: [%u]",
		strOffHookDevice.c_str(), strOffHookDevice.c_str(), lCSTACallRefId);
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	std::string strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(lCSTACallRefId, strCTICallRefId))
	{
		strCTICallRefId = CCallManager::Instance()->CreateNewCall(lCSTACallRefId);
	}
	std::vector<CProperty> l_vPropertieQueue;
	//SetPropertyValue(l_vPropertieQueue, Pr_AgentId, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_ACDGrp, "");
	SetPropertyValue(l_vPropertieQueue, Pr_CallerId, strOffHookDevice);
	//SetPropertyValue(l_vPropertieQueue, Pr_CalledId, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_IsBlackCall, Result_False);
	SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_DIAL]);
	SetPropertyValue(l_vPropertieQueue, Pr_CallDirection, CallDirectionString[CALL_DIRECTION_OUT]);
	CCallManager::Instance()->UpdateCall(lCSTACallRefId, l_vPropertieQueue);

	CDeviceManager::Instance()->UpdateLogicalCallState(lCSTACallRefId, strOffHookDevice, CallStateString[STATE_DIAL]);
	this->PostDeviceState(lCSTACallRefId, strCTICallRefId, strOffHookDevice);
}

void CAvayaEvent::CallIncoming(long lCSTACallRefId, const std::string& strCallerID,
	const std::string& strCalledID, const std::string& strAlteringDevice)
{
	std::string strShowMsg = m_pStrUtil->Format("Device: [%s] Call Incoming, CallerId: [%s], CalledId: [%s], CallRefID: [%u]",
		strAlteringDevice.c_str(), strCallerID.c_str(), strCalledID.c_str(), lCSTACallRefId);
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);
	//Device 110: Call Incoming, CallerId=7002, CalledId=110, CallRefID=483

	std::string strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(lCSTACallRefId, strCTICallRefId))
	{
		strCTICallRefId = CCallManager::Instance()->CreateNewCall(lCSTACallRefId);
	}

	bool bIsBlackCall = false;
	if (CBlackListManager::Instance()->FindBlackNumber(strCallerID))
	{
		bIsBlackCall = true;

		ICC_LOG_DEBUG(m_pLog, "Cur BlackType Is : %s", CSysConfig::Instance()->GetIsUseBlackRoute().c_str());
		if (CSysConfig::Instance()->GetIsUseBlackRoute() == Disable_Black_Route)
		{
			IAnswerCallNotifPtr pRequestNotif = boost::make_shared<CAnswerCallNotif>();
			if (pRequestNotif)
			{
				pRequestNotif->SetCSTACallRefId(lCSTACallRefId);
				pRequestNotif->SetCTICallRefId(strCTICallRefId);
				pRequestNotif->SetDeviceNum(strAlteringDevice);
				pRequestNotif->SetCallerId(strCallerID);
				pRequestNotif->SetCalledId(strCalledID);

				long lTaskId = CTaskManager::Instance()->AddCmdTask(Task_RefuseBlackCall, pRequestNotif);
				ICC_LOG_DEBUG(m_pLog, "AddCmdTask BlackListCall RefuseAnswer, TaskId: [%u], CallerId: [%s], CalledId: [%s]",
					lTaskId, strCallerID.c_str(), strCalledID.c_str());
			}
			else
			{
				ICC_LOG_FATAL(m_pLog, "Create CAnswerCallNotif Object Failed !!!");
			}
		}
		else
		{
			ICC_LOG_FATAL(m_pLog, "Create CAnswerCallNotif Object Failed !!!");
		}
	}

	std::vector<CProperty> l_vPropertieQueue;
	//SetPropertyValue(l_vPropertieQueue, Pr_AgentId, "");
	/*std::string l_strACDAlias;
	if (CSysConfig::Instance()->GetMergeHuntGrpConver(l_strACDAlias, strCalledID))
	{
		SetPropertyValue(l_vPropertieQueue, Pr_ACDGrp, l_strACDAlias);
	}
	else*/
	{
		SetPropertyValue(l_vPropertieQueue, Pr_ACDGrp, strAlteringDevice);
	}
	SetPropertyValue(l_vPropertieQueue, Pr_OriginalCallerId, strCallerID);
	SetPropertyValue(l_vPropertieQueue, Pr_OriginalCalledId, strCalledID);

	SetPropertyValue(l_vPropertieQueue, Pr_CTICallRefId, strCTICallRefId);
	SetPropertyValue(l_vPropertieQueue, Pr_CallerId, strCallerID);
	//SetPropertyValue(l_vPropertieQueue, Pr_CalledId, "");
	SetPropertyValue(l_vPropertieQueue, Pr_IsAlarmCall, Result_True);
	SetPropertyValue(l_vPropertieQueue, Pr_IsBlackCall, bIsBlackCall ? Result_True : Result_False);
	SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_INCOMING]);
	SetPropertyValue(l_vPropertieQueue, Pr_CallDirection, CallDirectionString[CALL_DIRECTION_IN]);
	CCallManager::Instance()->UpdateCall(lCSTACallRefId, l_vPropertieQueue);

	CCallManager::Instance()->PostCallState(lCSTACallRefId);
}

void CAvayaEvent::CallWaiting(long lCSTACallRefId, const std::string& strCallerID,
	const std::string& strCalledID, const std::string& strAlteringDevice)
{
	std::string strShowMsg = m_pStrUtil->Format("Device: [%s] Call Waiting, CallerId: [%s], CalledId: [%s], CallRefID: [%u]",
		strAlteringDevice.c_str(), strCallerID.c_str(), strCalledID.c_str(), lCSTACallRefId);
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	std::string strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(lCSTACallRefId, strCTICallRefId))
	{
		strCTICallRefId = CCallManager::Instance()->CreateNewCall(lCSTACallRefId);
	}
	std::vector<CProperty> l_vPropertieQueue;
	//SetPropertyValue(l_vPropertieQueue, Pr_AgentId, "");
	/*std::string l_strACDAlias;
	if (CSysConfig::Instance()->GetMergeHuntGrpConver(l_strACDAlias, strCalledID))
	{
		SetPropertyValue(l_vPropertieQueue, Pr_ACDGrp, l_strACDAlias);
	}
	else*/
	{
		SetPropertyValue(l_vPropertieQueue, Pr_ACDGrp, strAlteringDevice);
	}
	SetPropertyValue(l_vPropertieQueue, Pr_CTICallRefId, strCTICallRefId);
	SetPropertyValue(l_vPropertieQueue, Pr_CallerId, strCallerID);
	//SetPropertyValue(l_vPropertieQueue, Pr_CalledId, "");
//	SetPropertyValue(l_vPropertieQueue, Pr_IsBlackCall, /*bIsBlackCall ? Result_True :*/ Result_False);
	SetPropertyValue(l_vPropertieQueue, Pr_IsAlarmCall, Result_True);
	SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_QUEUE]);
	SetPropertyValue(l_vPropertieQueue, Pr_CallDirection, CallDirectionString[CALL_DIRECTION_IN]);
	CCallManager::Instance()->UpdateCall(lCSTACallRefId, l_vPropertieQueue);

	CCallManager::Instance()->PostCallState(lCSTACallRefId);
}

//话务分配
void CAvayaEvent::CallAssign(long lCSTACallRefId, const std::string& strCallerID,
	const std::string& strCalledID, const std::string& strAlteringDevice)
{
	std::string strShowMsg = m_pStrUtil->Format("Device: [%s] Call Assign, CallerId: [%s], CalledId: [%s], CallRefID: [%u]",
		strAlteringDevice.c_str(), strCallerID.c_str(), strCalledID.c_str(), lCSTACallRefId);
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);
	//Device 7004: Call Assign, CallerId=7002, CalledId=110, CallRefID=483

	std::string strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(lCSTACallRefId, strCTICallRefId))
	{
		strCTICallRefId = CCallManager::Instance()->CreateNewCall(lCSTACallRefId);
	}
	std::vector<CProperty> l_vPropertieQueue;
	SetPropertyValue(l_vPropertieQueue, Pr_AgentId, strAlteringDevice);
	//SetPropertyValue(l_vPropertieQueue, Pr_ACDGrp, strCalledID);
	SetPropertyValue(l_vPropertieQueue, Pr_CTICallRefId, strCTICallRefId);
	SetPropertyValue(l_vPropertieQueue, Pr_CallerId, strCallerID);
	SetPropertyValue(l_vPropertieQueue, Pr_CalledId, strAlteringDevice);
	SetPropertyValue(l_vPropertieQueue, Pr_IsAlarmCall, Result_True);
	//SetPropertyValue(l_vPropertieQueue, Pr_IsBlackCall, /*bIsBlackCall ? Result_True :*/ Result_False);
	SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_ASSIGN]);
	SetPropertyValue(l_vPropertieQueue, Pr_CallDirection, CallDirectionString[CALL_DIRECTION_IN]);
	CCallManager::Instance()->UpdateCall(lCSTACallRefId, l_vPropertieQueue);

	CCallManager::Instance()->PostCallState(lCSTACallRefId);
}

void CAvayaEvent::CallAlerting(long lCSTACallRefId, const std::string& strCallerID,
	const std::string& strCalledID, const std::string& strAlteringDevice)
{
	std::string strShowMsg = m_pStrUtil->Format("Device: [%s] Alerting, CallerId: [%s], CalledId: [%s], CallRefID: [%u]",
		strAlteringDevice.c_str(), strCallerID.c_str(), strCalledID.c_str(), lCSTACallRefId);
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	std::string strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(lCSTACallRefId, strCTICallRefId))
	{
		strCTICallRefId = CCallManager::Instance()->CreateNewCall(lCSTACallRefId);
		/*std::string l_strACDAlias;
		if (CSysConfig::Instance()->GetMergeHuntGrpConver(l_strACDAlias, strCalledID))
		{
			SetPropertyValue(l_vPropertieQueue, Pr_ACDGrp, l_strACDAlias);
		}*/
	}

	ICC_LOG_DEBUG(m_pLog, "Cur BlackType Is : %s", CSysConfig::Instance()->GetIsUseBlackRoute().c_str());
	if (CSysConfig::Instance()->GetIsUseBlackRoute() != Disable_Black_Route)
	{
		if (CBlackListManager::Instance()->FindBlackNumber(strCallerID))
		{
			std::string l_strBlackRouteDest = CSysConfig::Instance()->GetBlackRouteDest();
			if (!l_strBlackRouteDest.empty())
			{
				IDeflectCallNotifPtr l_pRequestNotif = boost::make_shared<CDeflectCallNotif>();
				if (l_pRequestNotif)
				{
					l_pRequestNotif->SetCSTACallRefId(lCSTACallRefId);
					l_pRequestNotif->SetCTICallRefId(strCTICallRefId);
					l_pRequestNotif->SetSponsor(strAlteringDevice);
					l_pRequestNotif->SetTarget(l_strBlackRouteDest);
					l_pRequestNotif->SetOrgCaller(strCallerID);
					l_pRequestNotif->SetOrgCalled(strCalledID);

					long lTaskId = CTaskManager::Instance()->AddCmdTask(Task_DeflectCall, l_pRequestNotif);
					ICC_LOG_DEBUG(m_pLog, "AddCmdTask BlackListCall Task_DeflectCall, TaskId: [%u], CallerId: [%s], CalledId: [%s]",
						lTaskId, strAlteringDevice.c_str(), l_strBlackRouteDest.c_str());
				}
				else
				{
					ICC_LOG_FATAL(m_pLog, "Create Task_DeflectCall Object Failed !!!");
				}
			}
			else
				ICC_LOG_FATAL(m_pLog, "GetBlackRouteDest Is NULL !!!");
		}
	}
	
	std::vector<CProperty> l_vPropertieQueue;
	//SetPropertyValue(l_vPropertieQueue, Pr_AgentId, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_ACDGrp, "");
	SetPropertyValue(l_vPropertieQueue, Pr_CTICallRefId, strCTICallRefId);
	SetPropertyValue(l_vPropertieQueue, Pr_CallerId, strCallerID);
	SetPropertyValue(l_vPropertieQueue, Pr_CalledId, strAlteringDevice);
	//SetPropertyValue(l_vPropertieQueue, Pr_IsBlackCall, /*bIsBlackCall ? Result_True :*/ Result_False);
	SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_RING]);
	SetPropertyValue(l_vPropertieQueue, Pr_CallDirection, CallDirectionString[CALL_DIRECTION_IN]);
	CCallManager::Instance()->UpdateCall(lCSTACallRefId, l_vPropertieQueue);

	CDeviceManager::Instance()->UpdateLogicalCallState(lCSTACallRefId, strAlteringDevice, CallStateString[STATE_RING]);
	this->PostDeviceState(lCSTACallRefId, strCTICallRefId, strAlteringDevice);
}

void CAvayaEvent::CallAlteringBack(long lCSTACallRefId, const std::string& strCallerID,
	const std::string& strCalledID, const std::string& strAlteringDevice)
{
	std::string strShowMsg = m_pStrUtil->Format("Device: [%s] AlertingBack, CallRefID: [%u], CallerId: [%s], CalledId: [%s]",
		strCallerID.c_str(), lCSTACallRefId, strCallerID.c_str(), strCalledID.c_str());
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	std::string strCTICallRefId = "";
	std::vector<CProperty> l_vPropertieQueue;
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(lCSTACallRefId, strCTICallRefId))
	{
		strCTICallRefId = CCallManager::Instance()->CreateNewCall(lCSTACallRefId);
		SetPropertyValue(l_vPropertieQueue, Pr_CallDirection, CallDirectionString[CALL_DIRECTION_OUT]);
	}
	
	//SetPropertyValue(l_vPropertieQueue, Pr_AgentId, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_ACDGrp, "");
	SetPropertyValue(l_vPropertieQueue, Pr_CTICallRefId, strCTICallRefId);
	SetPropertyValue(l_vPropertieQueue, Pr_CallerId, strCallerID);
	SetPropertyValue(l_vPropertieQueue, Pr_CalledId, strAlteringDevice);
	//SetPropertyValue(l_vPropertieQueue, Pr_IsBlackCall, /*bIsBlackCall ? Result_True :*/ Result_False);
	SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_SIGNALBACK]);
	//SetPropertyValue(l_vPropertieQueue, Pr_CallDirection, CallDirectionString[CALL_DIRECTION_OUT]);
	CCallManager::Instance()->UpdateCall(lCSTACallRefId, l_vPropertieQueue);

	CDeviceManager::Instance()->UpdateLogicalCallState(lCSTACallRefId, strCallerID, CallStateString[STATE_SIGNALBACK]);
	this->PostDeviceState(lCSTACallRefId, strCTICallRefId, strCallerID);
}

void CAvayaEvent::CallTalking(long p_lCSTACallRefId, const std::string& p_strCallerID, const std::string& p_strCalledID, const std::string& p_strTalkingDevice)
{
	std::string strShowMsg = m_pStrUtil->Format("Device: [%s] talking, CallerId: [%s], CalledId: [%s], CallRefID: [%u]",
		p_strTalkingDevice.c_str(), p_strCallerID.c_str(), p_strCalledID.c_str(), p_lCSTACallRefId);
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	std::string l_strCTICallRefId = "";
	std::vector<CProperty> l_vPropertieQueue;

	if (!CCallManager::Instance()->GetCallByCSTACallRefId(p_lCSTACallRefId, l_strCTICallRefId))
	{
		l_strCTICallRefId = CCallManager::Instance()->CreateNewCall(p_lCSTACallRefId);
		SetPropertyValue(l_vPropertieQueue, Pr_CallDirection, CallDirectionString[CALL_DIRECTION_IN]);
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

	//SetPropertyValue(l_vPropertieQueue, Pr_AgentId, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_ACDGrp, "");
	SetPropertyValue(l_vPropertieQueue, Pr_CTICallRefId, l_strCTICallRefId);
	SetPropertyValue(l_vPropertieQueue, Pr_CallerId, l_strCallerId);
	SetPropertyValue(l_vPropertieQueue, Pr_CalledId, l_strCalledId);
	//SetPropertyValue(l_vPropertieQueue, Pr_IsBlackCall, /*bIsBlackCall ? Result_True :*/ Result_False);
	SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_TALK]);
	//SetPropertyValue(l_vPropertieQueue, Pr_CallDirection, CallDirectionString[CALL_DIRECTION_IN]);
	CCallManager::Instance()->UpdateCall(p_lCSTACallRefId, l_vPropertieQueue);

	CDeviceManager::Instance()->UpdateLogicalCallState(p_lCSTACallRefId, p_strTalkingDevice, CallStateString[STATE_TALK]);
	this->PostDeviceState(p_lCSTACallRefId, l_strCTICallRefId, p_strTalkingDevice);
}
void CAvayaEvent::CallConference(long lNewCSTACallRefId, long lOldCSTACallRefId, const std::string& strMonitorDevice, const std::string& strConferenceDevice)
{
	std::string strShowMsg = m_pStrUtil->Format("Device: [%s]-[%s] conference, NewCallRefId: [%u], OldCallRefID: [%u]",
		strMonitorDevice.c_str(), strConferenceDevice.c_str(), lNewCSTACallRefId, lOldCSTACallRefId);
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	std::string strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(lNewCSTACallRefId, strCTICallRefId))
	{
		strCTICallRefId = CCallManager::Instance()->CreateNewCall(lNewCSTACallRefId);
	}
	std::vector<CProperty> l_vPropertieQueue;
	/*SetPropertyValue(l_vPropertieQueue, Pr_AgentId, "");
	SetPropertyValue(l_vPropertieQueue, Pr_ACDGrp, "");
	SetPropertyValue(l_vPropertieQueue, Pr_CallerId, "");
	SetPropertyValue(l_vPropertieQueue, Pr_CalledId, "");
	SetPropertyValue(l_vPropertieQueue, Pr_IsBlackCall, bIsBlackCall ? Result_True : Result_False);*/
	SetPropertyValue(l_vPropertieQueue, Pr_CTICallRefId, strCTICallRefId);
	if (lNewCSTACallRefId == lOldCSTACallRefId)
	{
		SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_TALK]);
	}
	else
	{
		SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_CONFERENCE]);
	}
	
//	SetPropertyValue(l_vPropertieQueue, Pr_CallDirection, CallDirectionString[CALL_DIRECTION_IN]);
	CCallManager::Instance()->AddConferenceParty(lNewCSTACallRefId, strConferenceDevice);
	CCallManager::Instance()->UpdateCall(lNewCSTACallRefId, l_vPropertieQueue);
	bool l_bPostState = true;
	if (lNewCSTACallRefId == lOldCSTACallRefId)
	{
		int nRes = CDeviceManager::Instance()->UpdateLogicalCallState(lNewCSTACallRefId, strMonitorDevice, CallStateString[STATE_TALK]);
		if (nRes == 1) //之前状态已为STATE_TALK，不需要通知
		{
			l_bPostState = false;
		}
	}
	else
	{
		CDeviceManager::Instance()->UpdateLogicalCallState(lNewCSTACallRefId, strMonitorDevice, CallStateString[STATE_CONFERENCE]);
	}

	if (l_bPostState)
	{
		this->PostDeviceState(lNewCSTACallRefId, strCTICallRefId, strMonitorDevice);
	}
	
}
void CAvayaEvent::CallSilentMonitor(long lCSTACallRefId, const std::string& strMonitorDevice)
{
	std::string strShowMsg = m_pStrUtil->Format("Device: [%s] SilentMonitor, CallRefID: [%u]",
		strMonitorDevice.c_str(), lCSTACallRefId);
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	std::string strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(lCSTACallRefId, strCTICallRefId))
	{
		strCTICallRefId = CCallManager::Instance()->CreateNewCall(lCSTACallRefId);
	}
	std::vector<CProperty> l_vPropertieQueue;
	//SetPropertyValue(l_vPropertieQueue, Pr_AgentId, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_ACDGrp, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_CallerId, strCallerID);
	//SetPropertyValue(l_vPropertieQueue, Pr_CalledId, strCalledID);
	//SetPropertyValue(l_vPropertieQueue, Pr_IsBlackCall, /*bIsBlackCall ? Result_True :*/ Result_False);
	SetPropertyValue(l_vPropertieQueue, Pr_CTICallRefId, strCTICallRefId);
	SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_SILENTMONITOR]);
	//SetPropertyValue(l_vPropertieQueue, Pr_CallDirection, CallDirectionString[CALL_DIRECTION_IN]);
	CCallManager::Instance()->UpdateCall(lCSTACallRefId, l_vPropertieQueue);

	CDeviceManager::Instance()->UpdateLogicalCallState(lCSTACallRefId, strMonitorDevice, CallStateString[STATE_SILENTMONITOR]);
	this->PostDeviceState(lCSTACallRefId, strCTICallRefId, strMonitorDevice);
}

void CAvayaEvent::CallHold(long lCSTACallRefId, const std::string& strHoldDevice)
{
	std::string strShowMsg = m_pStrUtil->Format("Device: [%s] HoldCall, CallRefID: [%u]",
		strHoldDevice.c_str(), lCSTACallRefId);
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	std::string strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(lCSTACallRefId, strCTICallRefId))
	{
		strCTICallRefId = CCallManager::Instance()->CreateNewCall(lCSTACallRefId);
	}
	std::vector<CProperty> l_vPropertieQueue;
	//SetPropertyValue(l_vPropertieQueue, Pr_AgentId, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_ACDGrp, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_CallerId, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_CalledId, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_IsBlackCall, / *bIsBlackCall ? Result_True :* / Result_False);
	SetPropertyValue(l_vPropertieQueue, Pr_CTICallRefId, strCTICallRefId);
	SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_HOLD]);
	//SetPropertyValue(l_vPropertieQueue, Pr_CallDirection, ""/*CallDirectionString[CALL_DIRECTION_IN] */);
	CCallManager::Instance()->UpdateCall(lCSTACallRefId, l_vPropertieQueue);


	CDeviceManager::Instance()->UpdateLogicalCallState(lCSTACallRefId, strHoldDevice, CallStateString[STATE_HOLD]);
	this->PostDeviceState(lCSTACallRefId, strCTICallRefId, strHoldDevice);
}

void CAvayaEvent::CallRetrieve(long lCSTACallRefId, const std::string& strRetrieveDevice)
{
	std::string strShowMsg = m_pStrUtil->Format("Device: [%s] RetrieveCall, CallRefID: [%u]",
		strRetrieveDevice.c_str(), lCSTACallRefId);
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	std::string strCTICallRefId = "";
	if (!CCallManager::Instance()->GetCallByCSTACallRefId(lCSTACallRefId, strCTICallRefId))
	{
		strCTICallRefId = CCallManager::Instance()->CreateNewCall(lCSTACallRefId);
	}
	std::vector<CProperty> l_vPropertieQueue;
	//SetPropertyValue(l_vPropertieQueue, Pr_AgentId, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_ACDGrp, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_CallerId, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_CalledId, "");
	//SetPropertyValue(l_vPropertieQueue, Pr_IsBlackCall, / *bIsBlackCall ? Result_True :* / Result_False);
	SetPropertyValue(l_vPropertieQueue, Pr_CTICallRefId, strCTICallRefId);
	SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_TALK]);
	//SetPropertyValue(l_vPropertieQueue, Pr_CallDirection, ""/*CallDirectionString[CALL_DIRECTION_IN] */);
	CCallManager::Instance()->UpdateCall(lCSTACallRefId, l_vPropertieQueue);

	CDeviceManager::Instance()->UpdateLogicalCallState(lCSTACallRefId, strRetrieveDevice, CallStateString[STATE_CANCELHOLD]);
	this->PostDeviceState(lCSTACallRefId, strCTICallRefId, strRetrieveDevice);
}

void CAvayaEvent::DeviceHangup(long lCSTACallRefId, const std::string& strHangupDevice, const std::string& strMonitorDevice, const std::string& strHangupType)
{
	std::string strShowMsg = m_pStrUtil->Format("Device: [%s]-[%s] Hangup, CallRefID: [%u]",
		strMonitorDevice.c_str(), strHangupDevice.c_str(), lCSTACallRefId);
	ICC_LOG_DEBUG(m_pLog, "%s", strShowMsg.c_str());
	ShowMsg(strShowMsg);

	std::string strCTICallRefId = "";
	if (CCallManager::Instance()->GetCallByCSTACallRefId(lCSTACallRefId, strCTICallRefId))
	{
		ICC_LOG_DEBUG(m_pLog, "DeviceHangup find call, call:[%ld]", lCSTACallRefId);

		if (strHangupType.compare(HangupTypeString[HANGUP_TYPE_TIMEOUT]) == 0 ||
			strHangupType.compare(HangupTypeString[HANGUP_TYPE_TRANSFER]) == 0 ||
			strHangupType.compare(HangupTypeString[HANGUP_TYPE_BLACKCALL]) == 0)
		{
			ICC_LOG_DEBUG(m_pLog, "DeviceHangup timeout or transfer or black call, call:[%ld]", lCSTACallRefId);

			// 振铃超时 、 话务转移（原话务）、黑名单拒接
			if (CSysConfig::Instance()->IsResonanceGroupNumber(CCallManager::Instance()->GetOriginalCalledId(lCSTACallRefId)) && CCallManager::Instance()->MemberDeviceCount(lCSTACallRefId) > 1)
			{
				ICC_LOG_DEBUG(m_pLog, "DeviceHangup not last hang up device, not set hangup state, call:[%ld], device:[%s]", lCSTACallRefId, strHangupDevice.c_str());
			}
			else
			{
				CCallManager::Instance()->SetHangupState(lCSTACallRefId, strHangupDevice, strHangupType);
			}
		}
		else if (strHangupType.compare(HangupTypeString[HANGUP_TYPE_TP_TRANSFER]) == 0 ||
			strHangupType.compare(HangupTypeString[HANGUP_TYPE_OP_TRANSFER]) == 0)
		{
			//	转移发起方挂机，不处理（新话务）
			//	转移（代答、偏转），不处理
			ICC_LOG_DEBUG(m_pLog, "DeviceHangup HANGUP_TYPE_TP_TRANSFER or HANGUP_TYPE_OP_TRANSFER, call:[%ld]", lCSTACallRefId);
		}
		else
		{
			std::string strDeviceType = CDeviceManager::Instance()->GetDeviceType(strHangupDevice);
			if (strDeviceType.compare(DEVICE_TYPE_ACDGROUP) != 0 && strDeviceType.compare(DEVICE_TYPE_ACDSUPER) != 0)
			{
				if (CSysConfig::Instance()->IsResonanceGroupNumber(CCallManager::Instance()->GetOriginalCalledId(lCSTACallRefId)) && CCallManager::Instance()->MemberDeviceCount(lCSTACallRefId) > 1)
				{
					ICC_LOG_DEBUG(m_pLog, "DeviceHangup not last hang up device, not set hangup type, call:[%ld], device:[%s]", lCSTACallRefId, strHangupDevice.c_str());
				}
				else
				{
					CCallManager::Instance()->ProcessDeviceHangup(lCSTACallRefId, strHangupDevice);
				}
			}

			//监听，监听方挂断，挂机时间取值错误
			/*strDeviceType = CDeviceManager::Instance()->GetDeviceType(strMonitorDevice);
			if (strDeviceType.compare(DEVICE_TYPE_ACDGROUP) == 0 || strDeviceType.compare(DEVICE_TYPE_ACDSUPER) == 0)
			{
				std::string strTemp = HangupTypeString[HANGUP_TYPE_CALLED];
				if (!strHangupDevice.empty())
				{
					strTemp = HangupTypeString[HANGUP_TYPE_CALLER];
				}

				CCallManager::Instance()->SetHangupState(lCSTACallRefId, strMonitorDevice, strTemp);
			}*/
		}

		if (CCallManager::Instance()->GetCallMode(lCSTACallRefId).compare(CallModeString[CALL_MODE_CONFERENCE]) == 0)
		{
			this->ConferenceHangup(lCSTACallRefId, strCTICallRefId, strHangupDevice);
		}

		if (strMonitorDevice.compare(strHangupDevice) == 0)
		{
			ICC_LOG_DEBUG(m_pLog, "DeviceHangup monitor device is equal hangup device ,call:[%ld]", lCSTACallRefId);

			//	会议、转移内存中的话务不能马上删除，上层可能会执行重连操作，删除了将找不到对应的 CSTACallRefId
			//	如果还有未拆线的会议成员，话务强拆后再删除
			bool bRemoveCall = true;

			if (CCallManager::Instance()->GetCallMode(lCSTACallRefId).compare(CallModeString[CALL_MODE_CONFERENCE]) == 0)
			{
				bRemoveCall = false;
				std::string strConfCompere = CCallManager::Instance()->GetConfCompere(lCSTACallRefId);
				std::string strDeviceState = CDeviceManager::Instance()->GetLogicalCallState(lCSTACallRefId, strHangupDevice);

				ICC_LOG_DEBUG(m_pLog, "conference call change bRemoveCall to false,compere:[%s],lCSTACallRefId:[%d]", strConfCompere.c_str(), lCSTACallRefId);
				//注释会议主持人挂机会议结束处理,Modified by tzx on March 23, 2023 
				/**
				if (strConfCompere.compare(strHangupDevice) == 0 &&
					(strDeviceState.compare(CallStateString[STATE_TALK]) == 0 ||
					strDeviceState.compare(CallStateString[STATE_CONFERENCE]) == 0 ||
					strDeviceState.compare(CallStateString[STATE_CANCELHOLD]) == 0))
				{
					//会议主持人挂机，会议结束
					this->ClearCall(strCTICallRefId, strHangupDevice, strHangupDevice); 
				}
				*/

				/**
				//增加通过成员数量判断是否结束会议,add by tzx on March 23, 2023
				if (CCallManager::Instance()->GetConfMemCount(lCSTACallRefId) == 0)
				{
					bRemoveCall = true;
				}
				*/
			}
			if (CCallManager::Instance()->GetIsTransferCall(lCSTACallRefId))
			{
				bRemoveCall = false;
				ICC_LOG_DEBUG(m_pLog, "transfer call change bRemoveCall to false,lCSTACallRefId:[%d]", lCSTACallRefId);
			}

			std::string l_strCompere = CCallManager::Instance()->GetConfCompere(lCSTACallRefId);
			if (!l_strCompere.empty())
			{
				bRemoveCall = false;
				ICC_LOG_DEBUG(m_pLog, "conference ConsultationCall change bRemoveCall to false,compere:[%s],lCSTACallRefId:[%d],strCTICallRefId:[%s]", 
					l_strCompere.c_str(), lCSTACallRefId, strCTICallRefId.c_str());
			}

			if (CSysConfig::Instance()->IsResonanceGroupNumber(CCallManager::Instance()->GetOriginalCalledId(lCSTACallRefId)))
			{
				ICC_LOG_DEBUG(m_pLog, "DeviceHangup is resonance group muber, call:[%ld], device:[%s]", lCSTACallRefId,strHangupDevice.c_str());

				CCallManager::Instance()->DeleteMemberDevice(lCSTACallRefId, strHangupDevice);
				CDeviceManager::Instance()->UpdateLogicalCallState(lCSTACallRefId, strHangupDevice, CallStateString[STATE_HANGUP]);
				this->PostDeviceState(lCSTACallRefId, strCTICallRefId, strHangupDevice);

				//	拆线完毕，设备空闲
				CDeviceManager::Instance()->UpdateLogicalCallState(lCSTACallRefId, strHangupDevice, CallStateString[STATE_FREE]);
				this->PostDeviceState(lCSTACallRefId, strCTICallRefId, strHangupDevice);
				CDeviceManager::Instance()->DeleteLogicalCallState(lCSTACallRefId, strHangupDevice);

				int iMemberCount = CCallManager::Instance()->MemberDeviceCount(lCSTACallRefId);
				ICC_LOG_DEBUG(m_pLog, "DeviceHangup member device count:[%d], call:[%ld]", iMemberCount, lCSTACallRefId);

				if (!CCallManager::Instance()->IsAlarmCall(lCSTACallRefId) && iMemberCount == 0)
				{
					//	报警话务，中继拆线后（CallClearedEvent）再检查话务是否结束
					this->CheckCallOver(lCSTACallRefId, bRemoveCall);
				}
			}
			else
			{
				CDeviceManager::Instance()->UpdateLogicalCallState(lCSTACallRefId, strHangupDevice, CallStateString[STATE_HANGUP]);
				this->PostDeviceState(lCSTACallRefId, strCTICallRefId, strHangupDevice);

				//	拆线完毕，设备空闲
				CDeviceManager::Instance()->UpdateLogicalCallState(lCSTACallRefId, strHangupDevice, CallStateString[STATE_FREE]);
				this->PostDeviceState(lCSTACallRefId, strCTICallRefId, strHangupDevice);
				CDeviceManager::Instance()->DeleteLogicalCallState(lCSTACallRefId, strHangupDevice);				

				if (!CCallManager::Instance()->IsAlarmCall(lCSTACallRefId))
				{
					//	报警话务，中继拆线后（CallClearedEvent）再检查话务是否结束
					this->CheckCallOver(lCSTACallRefId, bRemoveCall);
				}
			}
			
		}
	}
}

void CAvayaEvent::ConferenceHangup(long lCSTACallRefId, const std::string& strCTICallRefId, const std::string& strHangupDevice)
{
	if (CCallManager::Instance()->DeleteConferenceParty(lCSTACallRefId, strHangupDevice))
	{
		std::string l_strHangupDevice = strHangupDevice;
		if (strHangupDevice.find("T") != std::string::npos)
		{
			l_strHangupDevice = CCallManager::Instance()->GetE1RelayIndexNumber(lCSTACallRefId, strHangupDevice,true);
			if (l_strHangupDevice.empty())
			{
				l_strHangupDevice = strHangupDevice;
			}
		}

		IDeviceStateNotifPtr pHangupStateNotif = boost::make_shared<CDeviceStateNotif>();
		if (pHangupStateNotif)
		{
			pHangupStateNotif->SetDeviceNum(l_strHangupDevice);
			pHangupStateNotif->SetDeviceState(CallStateString[STATE_HANGUP]);
			pHangupStateNotif->SetCSTACallRefId(lCSTACallRefId);
			pHangupStateNotif->SetCTICallRefId(strCTICallRefId);
			pHangupStateNotif->SetStateTime(m_pDateTimePtr->CurrentDateTimeStr());

			long lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_ConferenceHangupEvent, pHangupStateNotif);
			ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask ConferenceHangup, DeviceNum: [%s], TaskId: [%u],strCTICallRefId: [%s]",
				strHangupDevice.c_str(), lTaskId, strCTICallRefId.c_str());
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

		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_ForcePopCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], ForcePopCall, CallRefId [%s], Sponsor [%s], Target [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strSponsor.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CForcePopCallNotif Object Failed !!!");
	}
}
void CAvayaEvent::ClearCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget)
{
	IClearCallNotifPtr l_pRequestNotif = boost::make_shared<CClearCallNotif>();
	if (l_pRequestNotif)
	{
		l_pRequestNotif->SetCTICallRefId(p_strCTICallRefId);
		l_pRequestNotif->SetSponsor(p_strSponsor);
		l_pRequestNotif->SetTarget(p_strSponsor);

		long l_lNewTaskId = CTaskManager::Instance()->AddCmdTask(Task_ClearCall, l_pRequestNotif);
		ICC_LOG_DEBUG(m_pLog, "AddCmdTask TaskId [%u], ClearCall, CallRefId [%s], Sponsor [%s], Target [%s]",
			l_lNewTaskId, p_strCTICallRefId.c_str(), p_strSponsor.c_str(), p_strSponsor.c_str());
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CClearCallNotif Object Failed !!!");
	}
}
void CAvayaEvent::CheckCallOver(long lCSTACallRefId, bool bRemoveCall)
{
	if (!CDeviceManager::Instance()->FindLogicalCall(lCSTACallRefId) /*&& CCallManager::Instance()->GetConfMemCount(lCSTACallRefId) == 0*/)
	{
		CCallManager::Instance()->PostCallOverNotif(lCSTACallRefId);

		if (CCallManager::Instance()->GetConfMemCount(lCSTACallRefId) == 0)
		{
			if (bRemoveCall)
			{
				CCallManager::Instance()->DeleteCall(lCSTACallRefId);
			}
			else
			{
				std::vector<CProperty> l_vPropertieQueue;
				SetPropertyValue(l_vPropertieQueue, Pr_CallState, CallStateString[STATE_HANGUP]);
				CCallManager::Instance()->UpdateCall(lCSTACallRefId, l_vPropertieQueue);
			}
		}
	}
}


void CAvayaEvent::PostDeviceState(long p_lCSTACallRefId, const std::string& p_strCTICallRefId, const std::string& p_strDeviceNum)
{
	IDeviceStateNotifPtr l_pDeviceState = CDeviceManager::Instance()->GetDeviceState(p_lCSTACallRefId, p_strDeviceNum);
	if (l_pDeviceState)
	{
		std::string l_strCTICallRefId = "";
		std::string l_strCallerId = "";
		std::string l_strCalledParty = "";
		std::string l_strOriginalCallerId = "";
		std::string l_strOriginalCalledId = "";
		std::string l_strCallDirection = "";
		std::string l_strAcdGrp = "";
		std::string l_strTalkTime = "";
		CCallManager::Instance()->GetCallByCSTACallRefIdEx(p_lCSTACallRefId, l_strCTICallRefId, l_strCallerId, l_strCalledParty, l_strOriginalCallerId, l_strOriginalCalledId, l_strCallDirection, l_strAcdGrp, l_strTalkTime);
		l_pDeviceState->SetCallerId(l_strCallerId);
		if (!l_strAcdGrp.empty())
		{
			/*std::string l_strACDAlias;
			if (CSysConfig::Instance()->GetMergeHuntGrpConver(l_strACDAlias, l_strAcdGrp))
			{
				l_strAcdGrp = l_strACDAlias;
				l_pDeviceState->SetACDGrp(l_strAcdGrp);
			}
			else*/
			{
				std::string strDeviceType = CDeviceManager::Instance()->GetDeviceType(l_strAcdGrp);
				if (strDeviceType.compare(DEVICE_TYPE_ACDGROUP) == 0 || strDeviceType.compare(DEVICE_TYPE_ACDSUPER) == 0)
				{
					l_pDeviceState->SetACDGrp(l_strAcdGrp);
				}
			}		
		}

		if (CSysConfig::Instance()->IsResonanceGroupNumber(CCallManager::Instance()->GetOriginalCalledId(p_lCSTACallRefId)))
		{
			l_pDeviceState->SetCalledId(p_strDeviceNum);
		}
		else
		{
			l_pDeviceState->SetCalledId(l_strCalledParty);
		}
		
		l_pDeviceState->SetOriginalCallerId(l_strOriginalCallerId);
		l_pDeviceState->SetOriginalCalledId(l_strOriginalCalledId);
		l_pDeviceState->SetCallDirection(l_strCallDirection);
		l_pDeviceState->SetCTICallRefId(p_strCTICallRefId);

		l_pDeviceState->SetTalkTime(l_strTalkTime);
		l_pDeviceState->SetCallCount(CDeviceManager::Instance()->GetDeviceCallCount(p_strDeviceNum));

		long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(Task_DeviceStateEvent, l_pDeviceState);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask DeviceStateEvent AcdGrp:[%s-%s],CalledParty:[%s],OriginalCalledId:[%s],l_strCTICallRefId:[%s], TaskId: [%u]",
			l_strAcdGrp.c_str(), l_pDeviceState->GetACDGrp().c_str(), l_strCalledParty.c_str(), l_strOriginalCalledId.c_str(), l_strCTICallRefId.c_str(), l_lTaskId);
	}
}
void CAvayaEvent::PostSwitchConf(long invokeID, E_TASK_NAME nTaskName, bool bResullt, int nErrorCode, const std::string& strErrorMsg)
{
	ISwitchResultNotifPtr pSwitchResultNotif = boost::make_shared<CSwitchResultNotif>();
	if (pSwitchResultNotif)
	{
		pSwitchResultNotif->SetRequestId(invokeID);
		pSwitchResultNotif->SetResult(bResullt);
		pSwitchResultNotif->SetErrorCode(nErrorCode);
		pSwitchResultNotif->SetErrorMsg(strErrorMsg);

		long l_lTaskId = CTaskManager::Instance()->AddSwitchEventTask(nTaskName, pSwitchResultNotif);
		ICC_LOG_DEBUG(m_pLog, "AddSwitchEventTask TaskName: [%s], TaskId: [%u], RequestId: [%u]",
			TaskNameString[nTaskName].c_str(), l_lTaskId, invokeID);
	}
	else
	{
		ICC_LOG_FATAL(m_pLog, "Create CSwitchResultNotif Object Failed !!!");
	}
}
