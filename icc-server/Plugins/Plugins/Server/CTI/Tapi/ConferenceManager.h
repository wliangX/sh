#pragma once

/**
@Copyright Copyright (C), 2018
@file		ConferenceManager.h
@created	2018/01/23
@brief		���������
1, ����ȫ�»���Ľ����������Ա�����
2, ����������ͨ����������ӻ����Ա

@author
*/

/*�½��������̣�
1�������˺��е�һ�������Ա������һ��ͨ���������һ�������Ա����ʧ�ܣ�����������һ�������Ա��
2�������к��л����ϣ���ѯ��һ�������Ա������ѯʧ�ܻ������峬ʱ�������������һ�������Ա��
3������ѯ��Ӧ��(ͨ��̬)���ϲ�������ͨ���У����ϲ�ʧ�ܣ��������ѯ��һ����Ա��
4�������Ա�����ϣ��������
*/

namespace ICC
{
	typedef struct S_CONFERENCE_PARTY
	{
		std::string m_strTarget;			//	�����Ա����
		std::string m_strTargetDeviceType;	//	�����Ա����
		std::string m_strCallState;			//�����Ա����״̬
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
		long							m_lMakeCallTaskId;			//	���������� ID
		long							m_lConsultationCallTaskId;	//	��ѯ������ ID
		long							m_lConferenceCallTaskId;	//	�ϲ���������� ID
		long							m_lAnswerCallTaskId;		//	��ͨ�������廰������� ID
		long							m_lRetrieveCallTaskId;		//	��ʰ��������� ID
		bool							m_bIsMakeCall;			//	������true���ӻ����Ա��false
		bool							m_bDoingConference;		//	�Ƿ���ִ����ӻ�����
		bool							m_bAddResult;			//	��ӵ�ǰ�����Ա�Ƿ�ɹ���־
		std::size_t						m_nCurrentPartyIndex;	//	��ǰ�������ĳ�Ա�±�

		std::string						m_strCreateTime;
		std::string						m_strCompere;			//	����������
		std::string						m_strSrcCaller;			//	ԭ����
		std::string						m_strSrcCallerState;	//  ԭ���к���״̬
		std::string						m_strSrcCalled;			//	ԭ����
		std::string						m_strSrcCalledState;	//  ԭ���к���״̬

		std::string						m_strSrcCallerDisEnable;
		std::string						m_strSrcCalledDisEnable;


		std::string						m_strCTICallRefId;		//	ԭʼ���� ID
		std::string						m_strActiveCTICallRefId;//	��ѯ�����Ļ��� ID
		std::string						m_strConferenceId;		//	���� ID
		std::string						m_strDialTime;			//	���������Ա����ʱ��
		std::string						m_strRingTime;			//	���������Ա����ʱ��
		std::string						m_strTargetState;		//	������Ա��Ա״̬�����塢ͨ�����һ���
		
		std::vector<CConferenceParty>	m_vPartyQueue;			//	�����Ա�б�

		bool m_bBargein;//�Ƿ���ǿ�����ɵĻ�Ա
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
		//	�½�һ��ͨ��
		
		void ProcessMakeCallResult(long p_lTaskId, const std::string& p_strCTICallRefId, bool p_bResult);

		// ��ʼ��ӻ����Ա,��ѯ
		void ConsultationCallEx(const std::string& p_strConferenceId);
		long ConsultationCall(const std::string& p_strHeldCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget, const std::string& p_strTargetDeviceType);
		void ProcessConsultationCallResult(long p_lTaskId, const std::string& p_strHeldCTICallRefId, const std::string& p_strActiveCTICallRefId, bool p_bResult);
		void ProcessConsultationCallResult(long p_lTaskId, bool p_bResult);

		//	��ѯ�ɹ����ϲ�����
		long ConferenceCall(const std::string& p_strTarget, const std::string& p_strHeldCTICallRefId, const std::string& p_strActiveCTICallRefId);
		void ProcessConferenceCallResult(long p_lTaskId, bool p_bResult);

		//	��ѯ �� ����ʧ�ܣ������������Ļ���
		long RetrieveCall(const std::string& p_strHeldCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);
		void ProcessRetrieveCallResult(long p_lTaskId, bool p_bResult);

		void ReconnectCall(const std::string& p_strActiveCTICallRefId, const std::string& p_strHeldCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);

		//	�����˹һ���ǿ�����
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

		//	����ѯ�����壬��ʼ���峬ʱ����
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