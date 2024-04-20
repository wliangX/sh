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
	void SetLogPtr(Log::ILogPtr p_pLog){ m_pLog = p_pLog; }
	void SetDateTimePtr(DateTime::IDateTimePtr p_pDateTime){ m_pDateTime = p_pDateTime; }
	void SetStringUtilPtr(StringUtil::IStringUtilPtr p_pStringUtil){ m_pStrUtil = p_pStringUtil; }

private:
	std::string CreateCTICallRefId(long p_lCSTACallRefId);
	bool AddCall(boost::shared_ptr<CCall> p_pCall);

public:
	std::string CreateNewCall(long p_lCSTACallRefId);
	bool DeleteCall(long p_lCSTACallRefId);
	void ClearCallList();
	bool RemoveCallQueueHead();
	int GetCallQueueSize();

	//////////////////////////////////////////////////////////////////////////
	//	原始主叫
	void SetOriginalCallerId(long p_lCSTACallRefId, const std::string& p_strOriginalCallerId);
	std::string GetOriginalCallerId(long p_lCSTACallRefId);

	//	原始被叫
	void SetOriginalCalledId(long p_lCSTACallRefId, const std::string& p_strOriginalCalledId);
	std::string GetOriginalCalledId(long p_lCSTACallRefId);

	void SetIsTransferCall(long p_lCSTACallRefId, bool p_bIsTransferCall);
	bool GetIsTransferCall(long p_lCSTACallRefId);

	void SetRelatedCSTACallRefId(long p_lCSTACallRefId, long p_lRelatedCSTACallRefId);
	long GetRelatedCSTACallRefId(long p_lCSTACallRefId);

	void SetCalledByCSTACallRefId(long p_lCSTACallRefId, std::string& p_strCalled);
	bool GetCalledByCSTACallRefId(long p_lCSTACallRefId, std::string& p_strCalled);

	//	会议主持人
	void SetConfCompere(long p_lCSTACallRefId, const std::string& p_strCompere);
	std::string GetConfCompere(long p_lCSTACallRefId);

	//	关联话务，新话务关联到已存在的话务
//	void SetRelatedCSTACallRefId(long p_lCSTACallRefId, long p_lRelateCSTACallRefId);
	bool GetRelatedCSTACallRefId(long p_lInCSTACallRefId, long& p_lOutRelateCSTACallRefId);

	/*void SetRelateCTICallRefId(long p_lCSTACallRefId, const std::string& p_strRelateCTICallRefId);
	bool GetRelateCTICallRefId(long p_lCSTACallRefId, std::string& p_strRelateCTICallRefId);*/

	std::string GetCallState(long p_lCSTACallRefId);
	std::string GetIncomingTime(long p_lCSTACallRefId);
	std::string GetDialTime(long p_lCSTACallRefId);

	//////////////////////////////////////////////////////////////////////////
	bool IsAlarmCall(long p_lCSTACallRefId);
	bool IsBlackCall(long p_lCSTACallRefId);
	bool IsReleaseCall(long p_lCSTACallRefId);

	std::string GetCallMode(long p_lCSTACallRefId);

	bool FindCallByCSTACallRefId(long p_lCSTACallRefId);
	bool FindCallByCTICallRefId(const std::string& p_strCTICallRefId);
	bool GetCTICallRefId(long p_lCSTACallRefId, std::string& p_strCTICallRefId);
	bool GetDeviceNum(long p_lCSTACallRefId, std::string& p_strDeviceNum);
	bool GetCSTACallRefId(const std::string& p_strInCTICallRefId, long& p_lOutCSTACallRefId);
	bool GetCallMembers(long p_lInCSTACallRefId, std::vector<std::string>& p_vCallMembers);

	bool GetOriginalCallByCTICallRefId(std::string& p_strCTICallRefId, std::string& p_strOriginalCallerId, std::string& p_strOriginalCalledParty);
	bool GetCallByCTICallRefId(const std::string& p_strInCTICallRefId, std::string& p_strOutCallerId, std::string& p_strOutCalledParty);

	bool GetCallByCSTACallRefId(long p_lCSTACallRefId, std::string& p_stCTICallRefId);
	bool GetCallByCSTACallRefId(long p_lCSTACallRefId, std::string& p_strCTICallRefId, std::string& p_strCallerId, std::string& p_strCalledParty,
		std::string& p_strOriginalCallerId, std::string& p_strOriginalCalledId, std::string& p_strCallDirection);
	
	void GetCallList(ISwitchNotifPtr p_pSwitchNotif);

	void UpdateCall(long p_lCSTACallRefId, std::vector<CProperty>& p_vProperties);
	bool CopyCall(long p_lDestCSTACallRefId, long p_lSrcCSTACallRefId);

	//重置挂机属性，主要用于拒接后重新分配，避免出现挂机类型为“早释”的错误
	void ResetHangupAttribute(long p_lCSTACallRefId);
	void SetHangupState(long p_lCSTACallRefId, const std::string& p_strHangupDevice, const std::string& p_strHangupType);
	void ProcessDeviceHangup(long p_lCSTACallRefId, const std::string& p_strHangupDevice);

	// p_lCSTACallRefId：主话务，p_lRelatedCSTACallRefId：关联话务
	bool AddRelatedCSTACallRefId(long p_lCSTACallRefId, long p_lRelatedCSTACallRefId);
	bool DeleteRelatedCSTACallRefId(long p_lCSTACallRefId, long p_lRelatedCSTACallRefId);
	bool FindRelatedCSTACallRefId(long p_lCSTACallRefId, long p_lRelatedCSTACallRefId);

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
	void PostCallOverNotif(long p_lCSTACallRefId, const std::string& p_strDeviceNum);
	//	话务状态通知
	void PostCallState(long p_lCSTACallRefId);
	//	获取所有当前话务
	void GetCallList(long p_lRequestId);
	void GetCallStateNotif(long p_lCSTACallRefId, ICallStateNotifPtr& p_pCallStateNotif);
	void GetCallOverNotif(long p_lCSTACallRefId, ICallOverNotifPtr& p_pCallOverNotif);

	//////////////////////////////////////////////////////////////////////////
	//	无效话务检测,8 小时后定时删除
	bool FindTimeoutCall(long &p_lCSTACallRefId);
	void ProcessTimeoutCall(long p_lCSTACallRefId);

private:
	std::mutex	m_callQueueMutex;
	std::map<long, boost::shared_ptr<CCall>>	m_mapCallQueue;

	Log::ILogPtr							m_pLog;
	StringUtil::IStringUtilPtr				m_pStrUtil;
	DateTime::IDateTimePtr					m_pDateTime;
};

}// end namespace
