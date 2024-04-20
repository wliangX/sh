#pragma once
#include"Boost.h"
#include "RequestResultEvent.h"
#include "FSAesEventProtocol.h"
#include "WorkThread.h"
namespace ICC
{
////////////////////////////////////////////////////////////////////////////////
//
class CCTICall
{
public:
	CCTICall(const std::string& p_strCTICallRefId, const long p_lCallIndex, CHcpDeviceCallEvent& p_HcpDeviceStateEvt);
	CCTICall(const std::string& p_strCTICallRefId, const long p_lCallIndex, const std::string& p_strCaller, const std::string& p_strCalled);
	virtual ~CCTICall() {};
public:
	void SetReplaceCallId(const std::string& p_strReplaceCallId) { m_strReplaceCallId = p_strReplaceCallId; };
	std::string GetReplaceCallId() const { return m_strReplaceCallId; };
	std::string GetCTICallRefId() const { return m_strCTICallRefId; };

	std::string GetCallState() const {return m_strCurrCallState;};
	std::string GetStateTime() { return m_strStateTime; };
	long GetCallIndex() { return m_lCallIndex; };
	
	std::string GetCallerId() const { return m_strCaller; };
	std::string GetCalledId() const { return m_strCalled; };
	std::string GetAgentExtension() const { return m_strAgentExtension; };

	void SetCallDirection(const std::string& p_strCallDirection) { m_strCallDirection = p_strCallDirection; };
	std::string GetCallDirection() const { return m_strCallDirection; };

	void SetCallState(const std::string& p_strCallState, const std::string& p_strTime);

	std::string GetDialTime() const { return m_strDialTime; };
	std::string GetIncomingTime() const { return m_strIncomingTime; };
	std::string GetWaitTime() const { return m_strWaitTime; };
	std::string GetAssignTime() const { return m_strAssignTime; };
	std::string GetRingTime() const { return m_strRingTime; };
	std::string GetRingbackTime() const { return m_strRingbackTime; };
	std::string GetTalkTime() const { return m_strTalkTime; };
	std::string GetHangupTime() const { return m_strHangupTime; };
	std::string GetReleaseTime() const { return m_strReleaseTime; };

	std::string GetAcdNum() const { return m_strAcdSkillNum; };
	void SetAcdNum(const std::string& p_strAcdNum) { m_strAcdSkillNum = p_strAcdNum; };

	std::string GetOriginalCalled() const { return m_strOriginalCalled; };
	void SetCaller(const std::string& p_strCaller) { m_strCaller = p_strCaller; };
	void SetCalled(const std::string& p_strCalled) { m_strCalled = p_strCalled; };
	void SetAgentExtension(const std::string& p_strNum) { m_strAgentExtension = p_strNum; };

	void SetCTIConferenceID(const std::string& p_strCTIConferenceId,const std::string &p_strCompere="");
	std::string GetCTIConferenceID() const { return m_strCTIConferenceId; };
	std::string GetConferenceCompere() { return m_strConferenceCompere; };

	bool UpdateCTICall(CHcpDeviceCallEvent& p_HcpDeviceStateEvt, const std::string& p_strCurrTime);
	void SetCallTransferType(int p_nTransferType, const std::string& p_strTarget, const std::string& p_strDeviceState = "");
	int GetCallTransferType();
	std::string GetCallTransferDstDevice();
	void SetBlackCall(bool p_bBlackCall) { m_bBlackCall = p_bBlackCall; };
	bool GetBlackCall() { return m_bBlackCall; };

	void SetHoldDeviceNum(const std::string& p_strHoldDeviceNum) { m_strHoldDeviceNum = p_strHoldDeviceNum; };
	std::string GetHoldDeviceNum() const { return m_strHoldDeviceNum; };

	bool CheckCallTimeOut(int p_nWaitAnswerMaxTime, int m_nTalkMaxTime);
private:
	std::string m_strReplaceCallId;
	std::string m_strCTICallRefId;
	std::string m_strCurrCallState;
	std::string m_strStateTime;

	long m_lCallIndex;
	std::string m_strCalled;	//被叫	
	std::string m_strAcdSkillNum;	// 被叫号码(110, 119等)

	std::string m_strCaller;
	std::string m_strOriginalCalled;
			
	std::string	m_strAgentExtension; 

	std::string m_strCallDirection;

	std::string m_strDialTime;  //拨号时间
	std::string m_strIncomingTime;
	std::string m_strWaitTime;
	std::string m_strAssignTime;
	std::string m_strRingTime;
	std::string m_strRingbackTime;
	std::string m_strTalkTime;
	std::string m_strHangupTime;
	std::string m_strReleaseTime;//早释时间

	std::string m_strHangupType;

	int m_nTransferType;
	std::string m_strTransferDstDevice;
	std::string m_strCTIConferenceId;  //如果该呼叫在会议，对应会议ID
	std::string m_strConferenceCompere;

	bool m_bBlackCall;
	std::string m_strHoldDeviceNum;

	int m_nSecondTicks;
};

typedef boost::shared_ptr<CCTICall> ICTICallPtr;
////////////////////////////////////////////////////////////////////////////////
//
class CCallManager
{
public:
	CCallManager(void);
	virtual ~CCallManager(void);

	static boost::shared_ptr<CCallManager> Instance();
	void ExitInstance();

	void OnInit(IResourceManagerPtr p_pResourceManager);
	void OnStart();
	void OnStop();
private:
	static boost::shared_ptr<CCallManager> m_pManagerInstance;

	void _ThreadFunCheckCall();
	bool _CheckIsTimeout();
	void _CheckCallTimeOut();
public:
	void ReleaseAllCall();
	int GetCallCount();
	bool GetAllCall(std::map<std::string, ICTICallPtr>& p_mapCallQueue);
	std::string GetTalkTime(const std::string &p_strHcpCallId);
	ICTICallPtr CreateCTICall(CHcpDeviceCallEvent& p_HcpDeviceStateEvt, long& p_lCallIndex);
	ICTICallPtr  CreateCTICallOut(const std::string& p_strHcpCallId, const std::string& p_strCaller, const std::string& p_strCalled);
	bool DeleteCTICall(const std::string& p_strHcpCallId);

	ICTICallPtr GetCallByHcpCallId(const std::string& p_strHcpCallId);
	ICTICallPtr GetCallByCTICallId(const std::string& p_strCTICallId);
	std::string GetHcpCallId(const std::string& p_strCITCallId);

	bool GetCallList(IGetCallListResultNotifPtr l_pResultNotif);

	void SetCallTransferType(const std::string& p_strCTICallId,int p_nTransferType, const std::string& p_strTarget);
private:
	std::string _CreateCTICallRefId(const std::string& p_strHcpCallId, long& p_lCallIndex);
public:

private:
	std::mutex	m_callQueueMutex;
	std::map<std::string, ICTICallPtr>		m_mapCallQueue;

	IWorkThreadPtr							m_pCallCheckThread;
	std::mutex m_CallIdHelperMutex;
	std::map<std::string, std::string>		m_mapCallIdHelperTB;

	Log::ILogPtr							m_pLogPtr;
	StringUtil::IStringUtilPtr				m_pStrUtil;
	DateTime::IDateTimePtr					m_pDateTimePtr;
	DateTime::CDateTime						m_oLastCheckTime;

	int	m_nWaitAnswerMaxTime;		//等待应答最大世界，单位分钟
	int m_nTalkMaxTime;				//通话最大时长，单位分钟， =0 不限制

};

}// end namespace
