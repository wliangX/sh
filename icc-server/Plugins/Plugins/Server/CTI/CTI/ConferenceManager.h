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
		std::string m_strActiveCallRefId;   //咨询呼id
		std::string m_strTarget;			//	会议成员号码
		std::string m_strTargetDeviceType;	//	会议成员类型
		std::string m_strCallState;			//会议成员呼叫状态
		std::string m_strCallId;
		std::string m_strAddTime;
		std::string m_strDisEnable;

		std::string			m_strRingTime;			//振铃时间
		std::string			m_strTalkTime;			//通话时间
		std::string			m_strJoinTime;			//入会时间
		std::string			m_strHangupTime;		//挂断时间

		int m_nIndex;
		long m_lConferenceCallTaskId;
	}CConferenceParty;

	class CConference
	{
	public:
		CConference();
		CConference(bool p_bBargein, 
			const std::string& p_strConferenceId,
			const std::string& p_strCTICallRefId,
			const std::string& p_strSrcCaller, 
			const std::string& p_strSrcCallerType,
			const std::string& p_strTmpState, 
			const std::string& p_strCompere, 
			const std::string& p_strParty,
			const std::string& p_strCreateTime,
			ISwitchClientPtr p_SwitchClientPtr);

		CConference(std::string& p_strConferenceId,
			const std::string& p_strCTICallRefId,
			const std::string& p_strActiveCallRefId,
			const std::string& p_strCompere,
			const std::string& p_strTarget,
			const std::string& p_strCreateTime,
			ISwitchClientPtr p_SwitchClientPtr);

		virtual ~CConference();

		void AddConferencePartyEx(const std::string& p_strCallId,
			const std::string &p_strConsultCallId, 
			const std::string& p_strTarget, 
			const std::string& p_strTime,
			ISwitchClientPtr p_SwitchClientPtr);
		bool GetPartyListEx(std::map<std::string, CConferenceParty>& p_ConferencePartyList, std::string& p_strLastTarget);
	public:
		void DeleteConferenceParty(const std::string& p_strDeviceNum);
		bool UpdateConferencePartyState(const std::string& p_strDeviceNum, const std::string p_DeviceState, const std::string& p_strCurrTime, Log::ILogPtr m_log);
		void ClearConferenceParty();

		bool FindConferenceParty(const std::string& p_strDeviceNum);
		std::string GetPartyByActiveCTICallRefId(const std::string& ActiveCTICallRefId);
		std::string GetPartyState(const std::string& p_strParty);
		std::string GetActiveCTICallRefIdByParty(const std::string& p_strParty);

		bool SetConferenceTarskId(const std::string& p_strParty, const std::string& p_strActiveCallRefId, long p_lTaskId);
		bool FindConferenceCallTaskId(long p_lTaskId, std::string& p_strParty, std::string& p_strActiveCallRefId);

		std::string GetCTICallRefId() { return m_strCTICallRefId; };
		std::string GetCompere() { return m_strCompere; };

		void SetCreateTime(const std::string p_strCreateTime) { m_strCreateTime = p_strCreateTime; };
		std::string GetCreateTime() { return m_strCreateTime; };
		bool SetDisenableParty(const std::string& p_strTarget, bool p_bDisEnable);

		std::string GetBargeinSponsor() { return m_strBargeinSponsor; };
		bool isBargein() { return m_bBargein; };

		void AddConsultationCallTaskId(long p_lConsultationCallTaskId, const std::string& p_strTarget);
		bool FindConsultationCallTaskId(long p_lConsultationCallTaskId);
		void DeleteConsultationCallTaskId(long p_lConsultationCallTaskId);
	public:
		std::map<long, std::string>				m_mapConsultationTaskIdTB;   //咨询的任务ID 列表
		std::string								m_strRingTime;				//	新增会议成员振铃时间
		std::map<std::string, CConferenceParty>	m_mapPartys;
	private:
		std::string			m_strCompere;			//	会议主持人
		std::string			m_strConferenceId;	
		std::string			m_strCTICallRefId;		//	原始话务 ID
		std::string			m_strCreateTime;
		bool				m_bBargein; //是否是强插生成的会员
		std::string			m_strBargeinSponsor;
		int					m_nIndex;
	};

	//////////////////////////////////////////////////////////////////////////
	class CConferenceManager
	{
	public:
		CConferenceManager(void);
		virtual ~CConferenceManager(void);

		static boost::shared_ptr<CConferenceManager> Instance();
		void ExitInstance();

		std::string CreateConferenceEx(const std::string& p_strActiveCallId, 
			const std::string& p_strHoldCallId, 
			const std::string& p_strTarget, 
			const std::string& p_strCompere);

		std::string _CreateBargeinConference(const std::string& p_strCompere,
			const std::string& l_strTarget,
			const std::string& p_strCTICallRefId);

		std::string AddConferencePartyNew(const std::string& p_strCTICallRefId,
			const std::string& p_strCompere,
			const std::string& l_strTarget,
			const std::string& l_strTargetType, bool p_bBargein = false);
	public:
		void ProcessDeviceStateNotif(const std::string& p_strDeviceNum, const std::string& p_strCalledId, const std::string& p_strDeviceState, const std::string& p_strCTICallRefId, const std::string& p_strConferenceId);
		void ProcessConferenceHangup(const std::string& p_strHangupDeviceNum, const std::string& p_strCTICallRefId, const std::string& p_strConferenceId);

		void ProcessConsultationCallResult(long p_lTaskId, const std::string& p_strHeldCTICallRefId, 
			const std::string& p_strActiveCTICallRefId,
			const std::string& p_strTargetDevice, bool p_bResult);
		void ProcessConferenceCallResult(long p_lTaskId, bool p_bResult);
	public:
		void SetRingTimeout(int p_nTimeSpan){ m_nRingTimeout = p_nTimeSpan; }
		int GetRingTimeout(){ return m_nRingTimeout; }

		void SetLogPtr(Log::ILogPtr p_logPtr){ m_LogPtr = p_logPtr; }
		void SetDateTimePtr(DateTime::IDateTimePtr p_dateTimePtr){ m_DateTimePtr = p_dateTimePtr; }
		void SetStringUtilPtr(StringUtil::IStringUtilPtr p_stringUtilPtr){ m_StringUtilPtr = p_stringUtilPtr; }
		void SetSwitchClientPtr(ISwitchClientPtr p_switchClientPtr){ m_SwitchClientPtr = p_switchClientPtr; }

		
		bool GetConferenceParty(const std::string& p_strConferenceId, PROTOCOL::CGetConferencePartyRespond& p_oOutRespond);
		bool DisenableConferenceParty(const std::string& p_strConferenceId, const std::string& p_strParty);

		void DeleteConferenceParty(const std::string& p_strConferenceId, const std::string& p_strParty);
		void DeleteConference(const std::string& p_strConferenceId, const std::string& p_strCTICallRefId);
		void ClearConferenceList();

		std::string FindConferenceIdByCallId(const std::string& p_strCTICallRefId, std::string& p_strCompere, std::string& p_strOriginalCTICallRefId);
		bool FindConferenceByConsultationCallTaskId(const long p_lTaskId);
		bool FindConferenceByConferenceCallTaskId(const long p_lTaskId);

		bool IsBargeinCreate(const std::string& p_strConferenceId, std::string& p_strBargeinSponsor);
		bool GetBargeinCreateSponsor(const std::string& p_strConferenceId, std::string p_strSponsor, std::string& p_strTarget);
		//	新建一个通话
		

		void _ProcessAddConferenceFailedEvent(const std::string& p_strConferenceId, const std::string& p_strActiveCallRefId);
		void ProcessFailedEvent(const std::string& p_strDeviceNum, const std::string& p_strCalledId, const std::string& p_strActiveCallRefId);

		//	被咨询方振铃，开始振铃超时计算
		void ResetCheckTime();
		bool CheckIsTimeout();
		void ProcessRingTimeout(const std::string& p_strConferenceId, bool p_bIsMakeCall);
		bool FindRingTimeout(std::string& p_strConferenceId, bool& p_bIsMakeCall);
		/*static */void DoCheckRingTime();

		void StartThread();
		void StopThread();
	private:
		// 开始添加会议成员,咨询
		long _ConsultationCall(const std::string& p_strHeldCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget, const std::string& p_strTargetDeviceType);
		//	咨询成功，合并会议
		long _ConferenceCall(const std::string& p_strTarget, const std::string& p_strHeldCTICallRefId, const std::string& p_strActiveCTICallRefId);

		//	咨询 或 会议失败，重连被保留的话务
		void _ReconnectCall(const std::string& p_strActiveCTICallRefId, const std::string& p_strHeldCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);
		//	主持人挂机，强拆会议
		void _ClearCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);
		void _HangupCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		bool _GetConferenceByCTICallRefId(const std::string& p_strCTICallRefId, std::string& p_strConferenceId);

		void _ProcessRingState(const std::string& p_strDeviceNum, const std::string& p_strCalledId, const std::string& p_strCTICallRefId, const std::string& p_strConferenceId);
		void _ProcessTalkState(const std::string& p_strDeviceNum, const std::string& p_strCalledId, const std::string& p_strCTICallRefId, const std::string& p_strConferenceId);
		void _ProcessHangupState(const std::string& p_strDeviceNum, const std::string& p_strCTICallRefId, const std::string& p_strConferenceId);
		void _ProcessHoldorUnholdState(bool p_bIsHold,const std::string& p_strDeviceNum, const std::string& p_strCTICallRefId, const std::string& p_strConferenceId);
		boost::shared_ptr<CConference> _GetConferenceObject(const std::string& p_strConferenceId);
	private:
		static boost::shared_ptr<CConferenceManager> m_pManagerInstance;

		std::mutex	m_conferenceMutex;
		std::map<std::string, boost::shared_ptr<CConference>> m_mConferenceList;

		int									m_nRingTimeout;
		bool								m_bThreadAlive;
		Log::ILogPtr						m_LogPtr;
		DateTime::IDateTimePtr				m_DateTimePtr;
		DateTime::CDateTime					m_oLastCheckTime;
		StringUtil::IStringUtilPtr			m_StringUtilPtr;
		ISwitchClientPtr					m_SwitchClientPtr;
		boost::shared_ptr<boost::thread>	m_pCheckRingTimeoutThread;
	};

}// end namespace