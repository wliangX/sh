#pragma once
#include "Boost.h"
#include <condition_variable>
#include "WorkThread.h"
#include "RequestResultEvent.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#define DEF_CTI_CALL_BACKFUN(funName,classTypePtr,classType) inline void _DoNotify##funName(const std::string &cmdname, ISwitchNotifPtr data) { \
	if(m_pSwitchEventCallback)\
	{\
		classTypePtr l_pNotify = boost::dynamic_pointer_cast<classType>(data);\
		if(l_pNotify != nullptr)\
		{\
			m_pSwitchEventCallback->funName(l_pNotify);\
		}\
		else{\
			ICC_LOG_ERROR(m_pLogPtr,"data type error!!!");\
		}\
	}\
}\

#define REGISTER_CALLBACK_FUN(funName)  m_mapCTICallbackEvent.insert(std::pair<std::string,PCTICallbackEvent>(#funName,&CCTICallbackFunHelper::_DoNotify##funName))
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
namespace ICC
{

class CCallbackTask
{
public:
	CCallbackTask() { m_bIsResultMsg = false; };
	virtual ~CCallbackTask(void) {};

public:
	void SetCmdName(const std::string &p_strCmdName) { m_strCmdName = p_strCmdName; }
	std::string GetCmdName() const { return m_strCmdName; }

	void SetCallbackFunName(const std::string& p_strFunName) { m_strCallbackFunName = p_strFunName; }
	std::string GetCallbackFunName() const { return m_strCallbackFunName; }

	void SetResultMsg() { m_bIsResultMsg = true; };
	bool IsResultMsg() { return m_bIsResultMsg; };
	void SetCallbackNotify(ISwitchNotifPtr p_pCallbackNotify) { m_pCallbackNotify = p_pCallbackNotify; }
	ISwitchNotifPtr GetCallbackNotify() { return m_pCallbackNotify; }

private:
	std::string			m_strCmdName;
	std::string			m_strCallbackFunName;
	bool				m_bIsResultMsg;
	ISwitchNotifPtr		m_pCallbackNotify;
};

typedef boost::shared_ptr<CCallbackTask> ICallbackTaskPtr;

class CCTICallbackFunHelper {
public:
	CCTICallbackFunHelper();
	~CCTICallbackFunHelper();

	void SetCallbackInterface(ISwitchEventCallbackPtr p_pSwitchEventCallback) {m_pSwitchEventCallback = p_pSwitchEventCallback;};
	void SetLog(Log::ILogPtr p_pLogPtr) { m_pLogPtr = p_pLogPtr;};
	void DispatchRequestResult(const std::string& p_strCallbackFunName, ISwitchNotifPtr m_pCallbackNotify, const std::string& p_strCmdName ="");
	void DispatchCTIEvent(const std::string& p_strCallbackFunName, ISwitchNotifPtr m_pCallbackNotify, const std::string& p_strCmdName = "");

	bool Start();
	void Stop();
	typedef void (CCTICallbackFunHelper::* PCTICallbackEvent)(const std::string&, ISwitchNotifPtr p_pCTINotifyEvent);
	
private:
	DEF_CTI_CALL_BACKFUN(Event_CTIConnState, ICTIConnStateNotifPtr, CCTIConnStateNotif)
	DEF_CTI_CALL_BACKFUN(Event_AgentState, IAgentStateNotifPtr, CAgentStateNotif)
	DEF_CTI_CALL_BACKFUN(Event_CallState, ICallStateNotifPtr, CCallStateNotif)
	DEF_CTI_CALL_BACKFUN(Event_CallOver, ICallOverNotifPtr, CCallOverNotif)
	DEF_CTI_CALL_BACKFUN(Event_DeviceState, IDeviceStateNotifPtr, CDeviceStateNotif)
	DEF_CTI_CALL_BACKFUN(Event_ConferenceHangup, IDeviceStateNotifPtr, CDeviceStateNotif)
	DEF_CTI_CALL_BACKFUN(Event_RefuseCall, IRefuseCallNotifPtr, CRefuseCallNotif)
	DEF_CTI_CALL_BACKFUN(Event_Failed, IFailedEventNotifPtr, CFailedEventNotif)
	DEF_CTI_CALL_BACKFUN(Event_BlackTransfer, IBlackTransferEventNotifPtr, CBlackTransferEventNotif)

	DEF_CTI_CALL_BACKFUN(CR_AgentLogin, ISetAgentStateResultNotifPtr, CSetAgentStateResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_AgentLogout, ISetAgentStateResultNotifPtr, CSetAgentStateResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_SetAgentState, ISetAgentStateResultNotifPtr, CSetAgentStateResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_AnswerCall, ISwitchResultNotifPtr, CSwitchResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_RefuseAnswer, ISwitchResultNotifPtr, CSwitchResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_Hangup, ISwitchResultNotifPtr, CSwitchResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_ClearCall, ISwitchResultNotifPtr, CSwitchResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_ListenCall, ISwitchResultNotifPtr, CSwitchResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_PickupCall, ISwitchResultNotifPtr, CSwitchResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_BargeInCall, ISwitchResultNotifPtr, CSwitchResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_ForcePopCall, ISwitchResultNotifPtr, CSwitchResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_DeflectCall, ISwitchResultNotifPtr, CSwitchResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_HoldCall, ISwitchResultNotifPtr, CSwitchResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_RetrieveCall, ISwitchResultNotifPtr, CSwitchResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_ReconnectCall, ISwitchResultNotifPtr, CSwitchResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_ConferenceCall, ISwitchResultNotifPtr, CSwitchResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_MakeCall, IMakeCallResultNotifPtr, CMakeCallResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_ConsultationCall, IConsultationCallResultNotifPtr, CConsultationCallResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_TransferCall, ITransferCallResultNotifPtr, CTransferCallResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_TakeOverCall, ITakeOverCallResultNotifPtr, CTakeOverCallResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_GetCTIConnState, IGetCTIConnStateResultNotifPtr, CGetCTIConnStateResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_GetDeviceList, IGetDeviceListResultNotifPtr, CGetDeviceListResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_GetACDList, IGetACDListResultNotifPtr, CGetACDListResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_GetAgentList, IGetAgentListResultNotifPtr, CGetAgentListResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_GetCallList, IGetCallListResultNotifPtr, CGetCallListResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_GetReadyAgent, IGetReadyAgentResultNotifPtr, CGetReadyAgentResultNotif)
	DEF_CTI_CALL_BACKFUN(CR_GetFreeAgentList, IGetFreeAgentResultNotifPtr, CGetFreeAgentResultNotif)
		//DEF_CTI_CALL_BACKFUN(CR_FSAesEventEx, ISwitchNotifPtr, ISwitchNotif)
private:
	void _DoNotifyCR_FSAesEventEx(const std::string& p_strCmdName, ISwitchNotifPtr p_data)
	{
		if (m_pSwitchEventCallback)
		{
			m_pSwitchEventCallback->CR_FSAesEventEx(p_strCmdName, p_data);
		}
	}

	void _InitCTICallbackEventFun();
	void _ThreadFun();
	void _ExeCTICallBack(ICallbackTaskPtr p_pCallbackTask);
private:
	IWorkThreadPtr		m_pCallBackThread;
	bool				m_bThreadStart;

	Log::ILogPtr								m_pLogPtr;
	ISwitchEventCallbackPtr						m_pSwitchEventCallback;
	std::map<std::string, PCTICallbackEvent>	m_mapCTICallbackEvent;
	std::list<ICallbackTaskPtr>					m_listCallbackTask;

	std::mutex m_mtxCallbackTask;
	std::mutex m_mutexWait;
	std::condition_variable	m_cvMsg;

};
} // end namespac


