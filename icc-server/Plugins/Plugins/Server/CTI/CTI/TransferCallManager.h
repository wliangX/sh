#pragma once

/**
@Copyright Copyright (C), 2018
@file		TransferCallManager.h
@created	2018/01/23
@brief		ת�ƻ��������,����ת�ƻ���Ŀ��Ƽ�ʵ��

1��ת����Ϊתָ����ϯ��ת�������ࡣ
2��ת�����̣�
1��ת���ţ�CTI ���� ACD ��ѯ�÷���Ŀ�����ϯ�����޿�����ϯ��ת��ʧ�ܣ�
2��CTI ��ѯָ������ϯ��ת���Ų�ѯ����Ŀ�������ϯ������ѯʧ�ܣ�ȡ��ԭ����ת��ʧ�ܣ�
3����ѯ�ɹ�����ʼ���峬ʱ��⣬�����峬ʱ��ȡ��ԭ����ת��ʧ�ܣ�
4��ת��Ŀ�귽��������У��籨���˻�ת�����һ������������ת��ʧ�ܣ�
	��ת��Ŀ�귽�ܾ�����ת����ȡ��ԭ����ת��ʧ�ܡ�

@author psy
*/

namespace ICC
{
	class CTransferCall
	{
	public:
		CTransferCall();
		virtual ~CTransferCall();

	public:
		bool				m_bTargetIsACD;				//	�Ƿ�Ϊת����
		long				m_lGetReadyAgentTaskId;		//	��ѯ������ϯ������ ID
		long				m_lConsultationCallTaskId;	//	��ѯ���������� ID
		long				m_lTransferCallTaskId;		//	ת�Ʋ��������� ID

		/*std::string			m_strOriginalCallerId;		//	ԭʼ����
		std::string			m_strOriginalCalledId;		//	ԭʼ����*/
		std::string			m_strRelatedId;				//	����Э�����Ϣ ID
		std::string			m_strHeldCallRefId;			//	�����Ļ��� ID
		std::string			m_strActiveCallRefId;		//	��ѯ�����Ļ��� ID
		std::string			m_strRingTime;				//	����ѯ����ʼ����ʱ��
		std::string			m_strTargetState;			//	����ѯ����״̬�����塢ͨ�����һ�
		std::string			m_strSponsor;				//	������ѯ����ϯ��
		std::string			m_strTarget;				//	��ѯĿ����ϯ��
		std::string			m_strTargetDeviceType;		//	��ѯĿ����豸����

		ObserverPattern::INotificationPtr m_pNotifiRequest;
	};

	//////////////////////////////////////////////////////////////////////////
	class CTransferCallManager
	{
	public:
		CTransferCallManager(void);
		virtual ~CTransferCallManager(void);

		static boost::shared_ptr<CTransferCallManager> Instance();
		void ExitInstance();

		void SetFsAesMode(bool p_bFsAesMode) { m_bFsAesMode = p_bFsAesMode; };
	public:
		void SetRingTimeout(int p_nTimeSpan){ m_nRingTimeout = p_nTimeSpan; }
		int GetRingTimeout(){ return m_nRingTimeout; }

		void SetLogPtr(Log::ILogPtr p_logPtr){ m_LogPtr = p_logPtr;}
		void SetDateTimePtr(DateTime::IDateTimePtr p_dateTimePtr){ m_DateTimePtr = p_dateTimePtr; }
		void SetSwitchClientPtr(ISwitchClientPtr p_switchClientPtr){ m_SwitchClientPtr = p_switchClientPtr; }

		void AddNewTransferCall(const std::string& p_strHeldCallRefId, const std::string& p_strSponsor, const std::string& p_strTarget, 
			const std::string& p_strTargetDeviceType, const std::string& p_strMsgId, bool p_bIsACD, ObserverPattern::INotificationPtr p_pNotifiRequest = nullptr);

		void CancelTransferCall(const std::string& p_strHeldCallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);
		void DeleteTransferCall(const std::string& p_strHeldCallRefId);
		void ClearTransferCallList();

		bool FindTransferCallByHeldCallRefId(const std::string& p_strHeldCallRefId);
		bool FindTransferCallByActiveCallRefId(const std::string& p_strActiveCallRefId);
		bool FindTransferCallByGetReadyAgentTaskId(long p_lTaskId);
		bool FindTransferCallByConsultationCallTaskId(long p_lTaskId);
		bool FindTransferCallByTransferCallTaskId(long p_lTaskId);

		//	ת�����
		void SendTransferCallResult(const std::string& p_strHeldCallRefId, int p_nResult);

		//	�� ACD ���ѯ������ϯ
		long GetReadyAgent(const std::string& p_strACDGrp);
		void ProcessGetReadyAgentResult(long p_lTaskId, const std::string& p_strTarget, bool p_bResult);

		//	��ѯת��Ŀ��
		long ConsultationCall(const std::string& p_strHeldCallRefId, const std::string& p_strSponsor, const std::string& p_strTarget, const std::string& p_strTargetDeviceType);
		void ProcessConsultationCallResult(long p_lTaskId, const std::string& p_strHeldCallRefId, const std::string& p_strActiveCallRefId, bool p_bResult, int p_nErrorCode);
		
		void ProcessRingState(const std::string& p_strDeviceNum, const std::string& p_strActiveCallRefId, const std::string& p_strStateTime);
		void ProcessTalkState(const std::string& p_strDeviceNum, const std::string& p_strActiveCallRefId);
		void ProcessHangupState(const std::string& p_strDeviceNum, const std::string& p_strCTICallRefId);
		void DeviceStateNotif(const std::string& p_strDeviceNum, const std::string& p_strActiveCallRefId,
			const std::string& p_strDeviceState, const std::string& p_strStateTime);

		//	ת�ƻ���
		long TransferCall(const std::string& p_strActiveCallRefId, const std::string& p_strHeldCallRefId,
			const std::string& p_strSponsor, const std::string& p_strTarget);
		void ProcessTransferCallResult(long p_lTaskId, const std::string& p_strHeldCallRefId, const std::string& p_strActiveCallRefId, bool p_bResult);

		//	ת��Ŀ�����峬ʱ �� ȡ��ת�ƣ�����ԭ����
		void ReconnectCall(const std::string& p_strActiveCallRefId, const std::string& p_strHeldCallRefId,
			const std::string& p_strSponsor, const std::string& p_strTarget);

		//	ԭ���йһ����Ҷ���ѯ����ת��ʧ��
		void ForcePopCall(const std::string& p_strActiveCallRefId, const std::string& p_strTarget);

		void ProcessFailedEvent(const std::string& p_strDeviceNum, const std::string& p_strCalledId, const std::string& p_strCTICallRefId);

		void ResetCheckTime();
		bool CheckIsTimeout();
		void ProcessRingTimeOut(const std::string& p_strHeldCallRefId);
		bool FindRingTimeOut(std::string& p_strHeldCallRefId);
		/*static */void DoCheckRingTime();
		void StartThread();
		void StopThread();

	private:
		static boost::shared_ptr<CTransferCallManager> m_pManagerInstance;

		std::mutex	m_transferCallMutex;
		std::map<std::string, boost::shared_ptr<CTransferCall>> m_mTransferCallList;

		int									m_nRingTimeout;
		bool								m_bThreadAlive;
		Log::ILogPtr						m_LogPtr;
		DateTime::IDateTimePtr				m_DateTimePtr;
		DateTime::CDateTime					m_oLastCheckTime;
		ISwitchClientPtr					m_SwitchClientPtr;	
		boost::shared_ptr<boost::thread>	m_pCheckRingTimeoutThread;

		bool m_bFsAesMode;
	};

}// end namespace