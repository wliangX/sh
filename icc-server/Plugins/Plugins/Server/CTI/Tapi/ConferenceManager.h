#pragma once

/**
@Copyright Copyright (C), 2018
@file		ConferenceManager.h
@created	2018/01/23
@brief		会议管理类
1, 负责全新会议的建立及会议成员的添加
2, 负责在已有通话基础上添加会议成员

@author
*/

/*新建会议流程：
1、主持人呼叫第一个会议成员，建立一个通话；如果第一个会议成员呼叫失败，继续呼叫下一个会议成员；
2、在已有呼叫基础上，咨询下一个会议成员；若咨询失败或者振铃超时，则继续呼叫下一个会议成员；
3、被咨询方应答(通话态)，合并到会议通话中；若合并失败，则继续咨询下一个成员；
4、会议成员添加完毕，会议结束
*/

namespace ICC
{
	typedef struct S_CONFERENCE_PARTY
	{
		std::string m_strTarget;			//	会议成员号码
		std::string m_strTargetDeviceType;	//	会议成员类型
		std::string m_strCallState;			//会议成员呼叫状态
		std::string m_strAddTime;
		std::string m_strDisEnable;
	}CConferenceParty;
	
	class CConference
	{
	public:
		CConference();
		virtual ~CConference();

	public:
		void AddConferenceParty(std::vector<CConferenceParty>& p_vPartyQueue);
		void DeleteConferenceParty(const std::string& p_strDeviceNum);
		void ClearConferenceParty();

		bool FindConferenceParty(const std::string& p_strDeviceNum);
		std::size_t GetConferencePartyCount();
		bool AddConferencePartyIsOver();

		std::string GetCurrentParty();
		bool GetCurrentParty(std::string& p_strTarget, std::string& p_strTargetType);

		void SetCreateTime(const std::string p_strCreateTime) { m_strCreateTime = p_strCreateTime; };
		std::string GetCreateTime() { return m_strCreateTime; };
		bool SetConferencePartyState(const std::string& p_strDeviceNum, const string& p_strState);
		bool GetPartyList(std::map<std::string, CConferenceParty>& p_ConferencePartyList, std::string& p_strLastTarget, DateTime::IDateTimePtr p_DateTimePtr);
		bool SetDisenableParty(const std::string& p_strTarget, bool p_bDisEnable);
	public:
		long							m_lMakeCallTaskId;			//	单呼的任务 ID
		long							m_lConsultationCallTaskId;	//	咨询的任务 ID
		long							m_lConferenceCallTaskId;	//	合并会议的任务 ID
		long							m_lAnswerCallTaskId;		//	接通正在振铃话务的任务 ID
		long							m_lRetrieveCallTaskId;		//	重拾话务的任务 ID
		bool							m_bIsMakeCall;			//	单呼：true，加会议成员：false
		bool							m_bDoingConference;		//	是否在执行添加会议中
		bool							m_bAddResult;			//	添加当前会议成员是否成功标志
		std::size_t						m_nCurrentPartyIndex;	//	当前加入会议的成员下标

		std::string						m_strCreateTime;
		std::string						m_strCompere;			//	会议主持人
		std::string						m_strSrcCaller;			//	原主叫
		std::string						m_strSrcCallerState;	//  原主叫呼叫状态
		std::string						m_strSrcCalled;			//	原被叫
		std::string						m_strSrcCalledState;	//  原被叫呼叫状态

		std::string						m_strSrcCallerDisEnable;
		std::string						m_strSrcCalledDisEnable;


		std::string						m_strCTICallRefId;		//	原始话务 ID
		std::string						m_strActiveCTICallRefId;//	咨询产生的话务 ID
		std::string						m_strConferenceId;		//	会议 ID
		std::string						m_strDialTime;			//	新增会议成员拨号时间
		std::string						m_strRingTime;			//	新增会议成员振铃时间
		std::string						m_strTargetState;		//	新增会员成员状态（振铃、通话、挂机）
		
		std::vector<CConferenceParty>	m_vPartyQueue;			//	会议成员列表

		bool m_bBargein;//是否是强插生成的会员
	};

	//////////////////////////////////////////////////////////////////////////
	class CConferenceManager
	{
	public:
		CConferenceManager(void);
		virtual ~CConferenceManager(void);

		static boost::shared_ptr<CConferenceManager> Instance();
		void ExitInstance();

	public:
		void SetRingTimeout(int p_nTimeSpan){ m_nRingTimeout = p_nTimeSpan; }
		int GetRingTimeout(){ return m_nRingTimeout; }

		void SetLogPtr(Log::ILogPtr p_pLog){ m_pLog = p_pLog; }
		void SetDateTimePtr(DateTime::IDateTimePtr p_pDateTime){ m_pDateTime = p_pDateTime; }
		void SetStringUtilPtr(StringUtil::IStringUtilPtr p_pStringUtil){ m_pStrUtil = p_pStringUtil; }

		std::string CreateNewConference(const std::string& p_strCompere, std::vector<CConferenceParty>& p_vPartyQueue, const std::string& p_strCTICallRefId = "");

		std::string AddConferenceParty(const std::string& p_strCTICallRefId, const std::string& p_strCompere, std::vector<CConferenceParty>& p_vPartyQueue);

		bool GetConferenceParty(const std::string& p_strConferenceId, PROTOCOL::CGetConferencePartyRespond& p_oOutRespond);
		bool DisenableConferenceParty(const std::string& p_strConferenceId, const std::string& p_strParty);

		void DeleteConferenceParty(const std::string& p_strConferenceId, const std::string& p_strParty);
		bool CR_DeleteConferenceParty(long p_lCSTACallRefId, const std::string& p_strCTICallRefId, const std::string& p_strHangupDevice);

		void DeleteConference(const std::string& p_strCTICallRefId);
		void ClearConferenceList();

		//bool GetCallRefId(std::string& p_strCompere, std::vector<long>& InfoVec, int& ConfType);
		//void GetCallRefId(long lConferenceId, std::string& p_strCompere, std::vector<long>& InfoVec, int& ConfType);
		bool IsConferenceExist(const std::string& p_strCompere);
		bool IsConferenceOver(const std::string& p_strCompere);

		bool FindConference(const std::string& p_strCTICallRefId, const std::string& p_strCompere);
		bool FindConferenceByCTICallRefId(const std::string& p_strCTICallRefId);
		bool FindConferenceByMakeCallTaskId(const long p_lTaskId);
		bool FindConferenceByConsultationCallTaskId(const long p_lTaskId);
		bool FindConferenceByConferenceCallTaskId(const long p_lTaskId);
		bool FindConferenceByAnswerCallTaskId(const long p_lTaskId);
	//	bool FindConferenceByHeldCallRefId(const std::string& p_strHeldCTICallRefId);
		bool FindConferenceByActiveCallRefId(const std::string& p_strActiveCTICallRefId);
		bool GetConferenceByCTICallRefId(const std::string& p_strCTICallRefId, std::string& p_strConferenceId);
		bool GetCTICallRefIdByActiveCallRefId(const std::string& p_strActiveCTICallRefId, std::string& p_strCTICallRefId);
		bool GetCTICallRefIdByParty(const std::string& p_strParty, std::string& p_strCTICallRefId);
		void SetActiveCallRefId(const std::string& p_strHeldCTICallRefId, const std::string& p_strActiveCTICallRefId);
		bool GetAddCurrentPartyResult(const std::string& p_strCompere, const std::string& p_strActiveCTICallRefId);

	//	void ProcessDialState(const std::string& p_strDeviceNum, const std::string& p_strCTICallRefId);
		void ProcessRingState(const std::string& p_strDeviceNum, const std::string& p_strCTICallRefId);
		void ProcessTalkState(const std::string& p_strDeviceNum, const std::string& p_strCTICallRefId);
		void ProcessConferenceHangup(const std::string& p_strHangupDeviceNum, const std::string& p_strCTICallRefId);
		void ProcessHangupState(const std::string& p_strDeviceNum, const std::string& p_strCTICallRefId);
		void DeviceStateNotif(const std::string& p_strDeviceNum, const std::string& p_strDeviceState, const std::string& p_strCTICallRefId);

		bool IsDoingConference(const std::string& p_strConferenceId);
		//	新建一个通话
		
		void ProcessMakeCallResult(long p_lTaskId, const std::string& p_strCTICallRefId, bool p_bResult);

		// 开始添加会议成员,咨询
		void ConsultationCallEx(const std::string& p_strConferenceId);
		long ConsultationCall(const std::string& p_strHeldCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget, const std::string& p_strTargetDeviceType);
		void ProcessConsultationCallResult(long p_lTaskId, const std::string& p_strHeldCTICallRefId, const std::string& p_strActiveCTICallRefId, bool p_bResult);
		void ProcessConsultationCallResult(long p_lTaskId, bool p_bResult);

		//	咨询成功，合并会议
		long ConferenceCall(const std::string& p_strTarget, const std::string& p_strHeldCTICallRefId, const std::string& p_strActiveCTICallRefId);
		void ProcessConferenceCallResult(long p_lTaskId, bool p_bResult);

		//	咨询 或 会议失败，重连被保留的话务
		long RetrieveCall(const std::string& p_strHeldCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);
		void ProcessRetrieveCallResult(long p_lTaskId, bool p_bResult);

		void ReconnectCall(const std::string& p_strActiveCTICallRefId, const std::string& p_strHeldCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		//	主持人挂机，强拆会议
	//	void ForcePopCall(const std::string& p_strConferenceId);
		void ClearCall(const std::string& p_strConferenceId);

		void AnswerCall(const std::string& p_strConferenceId);
		long AnswerCall(const std::string& p_strCTICallRefId, const std::string& p_strTarget);
		void ProcessAnswerCallResult(long p_lTaskId, const std::string& p_strTarget, bool p_bResult);

		void HangupCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);
		void RemoveFromConference(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		void ProcessMakeCallFailedEvent(const std::string& p_strConferenceId);
		void ProcessAddConferenceFailedEvent(const std::string& p_strConferenceId);
		void ProcessFailedEvent(const std::string& p_strDeviceNum, const std::string& p_strCalledId, const std::string& p_strActiveCallRefId);

		//	被咨询方振铃，开始振铃超时计算
		void ProcessDialTimeout(const std::string& p_strConferenceId, bool p_bIsMakeCall);
		void ProcessRingTimeout(const std::string& p_strConferenceId, bool p_bIsMakeCall);
		bool FindDialTimeout(std::string& p_strConferenceId, bool& p_bIsMakeCall);
		bool FindRingTimeout(std::string& p_strConferenceId, bool& p_bIsMakeCall);
	private:
		long MakeCall(const std::string& p_strCallerId, const std::string& p_strCalledId, const std::string& p_strTargetType);
	private:
		static boost::shared_ptr<CConferenceManager> m_pManagerInstance;

		Log::ILogPtr						m_pLog;
		DateTime::IDateTimePtr				m_pDateTime;
		StringUtil::IStringUtilPtr			m_pStrUtil;

		int									m_nRingTimeout;

		std::mutex	m_conferenceMutex;
		std::map<std::string, boost::shared_ptr<CConference>> m_mConferenceList;
	};

}// end namespace