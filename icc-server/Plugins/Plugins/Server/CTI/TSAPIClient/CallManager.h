#pragma once

/**
 @Copyright Copyright (C), 2017-2018
 @file		CallManage.h
 @created	2018/01/28
 @brief		呼叫管理类
            管理所有正在通话的呼叫,记录相关的呼叫信息
			维护交换机产生的呼叫ID和应用层呼叫ID的映射关系
 @author	psy
*/

#include "Call.h"
#include "Property.h"

namespace ICC
{
class CCallManager
{
public:
	CCallManager(void);
	virtual ~CCallManager(void);

	static boost::shared_ptr<CCallManager> Instance();
	void ExitInstance();
private:
	static boost::shared_ptr<CCallManager> m_pManagerInstance;

public:
	void OnInit(IResourceManagerPtr p_pResourceManager);
	void OnStart();
	void OnStop();

private:
	std::string CreateCTICallRefId(long p_lCSTACallRefId);
	bool AddCall(boost::shared_ptr<CCall> p_pCall);

public:
	std::string CreateNewCall(long p_lCSTACallRefId);
	bool DeleteCall(long p_lCSTACallRefId);
	void ClearAll();
	bool RemoveCallQueueHead();
	int GetCallQueueSize();

	void AddMemberDevice(long lcstaCallId,const std::string& strDevice);
	void DeleteMemberDevice(long lcstaCallId, const std::string& strDevice);
	bool ExistMemberDevice(long lcstaCallId, const std::string& strDevice);
	int  MemberDeviceCount(long lcstaCallId);

	//////////////////////////////////////////////////////////////////////////
	//	原始主叫
	void SetOriginalCallerId(long p_lCSTACallRefId, const std::string& p_strOriginalCallerId);
	std::string GetOriginalCallerId(long p_lCSTACallRefId);

	//	原始被叫
	void SetOriginalCalledId(long p_lCSTACallRefId, const std::string& p_strOriginalCalledId);
	std::string GetOriginalCalledId(long p_lCSTACallRefId);

	void SetIsTransferCall(long p_lCSTACallRefId, bool p_bIsTransferCall);
	bool GetIsTransferCall(long p_lCSTACallRefId);

	//	会议主持人
	void SetConfCompere(long p_lCSTACallRefId, const std::string& p_strCompere);
	std::string GetConfCompere(long p_lCSTACallRefId);

	//	关联话务，新话务关联到已存在的话务
	void SetRelateCSTACallRefId(const std::string& p_strCTICallRefId, long p_lRelateCSTACallRefId);
	long GetRelateCSTACallRefId(const std::string& p_strCTICallRefId);

	void SetRelateCTICallRefId(long p_lCSTACallRefId, const std::string& p_strRelateCTICallRefId);
	bool GetRelateCTICallRefId(long p_lCSTACallRefId, std::string& p_strRelateCTICallRefId);

	//////////////////////////////////////////////////////////////////////////
	bool IsAlarmCall(long p_lCSTACallRefId);
	bool IsBlackCall(long p_lCSTACallRefId);
	bool IsReleaseCall(long p_lCSTACallRefId);

	std::string GetCallMode(long p_lCSTACallRefId);

	bool FindCallByCSTACallRefId(long p_lCSTACallRefId);
	bool FindCallByCTICallRefId(const std::string& p_strCTICallRefId);
	bool GetCTICallRefId(long p_lCSTACallRefId, std::string& p_strCTICallRefId);
	bool GetCSTACallRefId(const std::string& p_strCTICallRefId, long& p_lCSTACallRefId);

	bool GetOriginalCallByCTICallRefId(std::string& p_strCTICallRefId, std::string& p_strOriginalCallerId, std::string& p_strOriginalCalledParty);
	bool GetCallByCTICallRefId(std::string& p_strCTICallRefId, std::string& p_strCallerId, std::string& p_strCalledParty);
	bool GetCallTime(const std::string& p_strCTICallRefId, std::string& p_strRingTime, std::string& p_strTalkTime);

	bool GetCallByCSTACallRefId(long p_lCSTACallRefId, std::string& p_stCTICallRefId);
	bool GetCallByCSTACallRefId(long p_lCSTACallRefId, std::string& p_strCTICallRefId, std::string& p_strCallerId, std::string& p_strCalledParty,
		std::string& p_strOriginalCallerId, std::string& p_strOriginalCalledId, std::string& p_strCallDirection,std::string& p_strTalkTime);
	bool GetCallByCSTACallRefIdEx(long p_lCSTACallRefId, std::string& p_strCTICallRefId, std::string& p_strCallerId, std::string& p_strCalledParty,
		std::string& p_strOriginalCallerId, std::string& p_strOriginalCalledId, std::string& p_strCallDirection, std::string& p_strAcdGrp, std::string& p_strTalkTime);
	
	void UpdateCall(long p_lCSTACallRefId, std::vector<CProperty>& p_vProperties);
	bool CopyCall(long p_lDestCSTACallRefId, long p_lSrcCSTACallRefId, const std::string& p_strCallState);

	void SetHangupState(long p_lCSTACallRefId, const std::string& p_strHangupDevice, const std::string& p_strHangupType);
	void ProcessDeviceHangup(long p_lCSTACallRefId, const std::string& p_strHangupDevice);

	// conference
	bool DeleteConferenceParty(long p_lCSTACallRefId, const std::string& p_strHangupPhone);
	bool AddConferenceParty(long p_lCSTACallRefId, const std::string& p_strJoinPhone);
	int GetConfMemCount(long p_lCSTACallRefId);

	//////////////////////////////////////////////////////////////////////////
	//	拒接话务通知
	void PostRefuseCallNotif(long p_lCSTACallRefId);
	//	早释话务通知
	void PostReleaseCall(long p_lCSTACallRefId);
	//	话务结束通知
	void PostCallOverNotif(long p_lCSTACallRefId);
	//	话务状态通知
	void PostCallState(long p_lCSTACallRefId);
	//	获取所有当前话务
	void GetCallList(long p_lRequestId);

	std::string GetCallState(long p_lCSTACallRefId);

	void SetE1RelayIndexNumber(long p_lCSTACallRefId, const std::string& p_strE1RelayIndex, std::string& p_strNumber);
	std::string GetE1RelayIndexNumber(long p_lCSTACallRefId, const std::string& p_strE1RelayIndex,bool p_bDeleteFlag);
	std::string GetE1RelayIndexByNumber(long p_lCSTACallRefId, const std::string& p_strNumber);
	//////////////////////////////////////////////////////////////////////////
	//	无效话务检测,24 小时后定时删除
	void ResetCheckTime();
	bool CheckIsTimeout();
	bool FindTimeoutCall(long &p_lCSTACallRefId);
	void ProcessTimeoutCall(long p_lCSTACallRefId);
	void StartCheckThread();
	void StopCheckThread();
	/*static */void DoCheckCall();

private:
	std::mutex	m_callQueueMutex;
	std::map<long, boost::shared_ptr<CCall>>	m_mapCallQueue;

	Log::ILogPtr							m_pLogPtr;
	StringUtil::IStringUtilPtr				m_pStrUtil;
	DateTime::IDateTimePtr					m_pDateTimePtr;
	DateTime::CDateTime						m_oLastCheckTime;

	bool									m_bCheckThreadAlive;
	boost::shared_ptr<boost::thread>		m_pCheckCallThread;
};

}// end namespace
