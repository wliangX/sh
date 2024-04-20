#pragma once

namespace ICC
{
	class CCall
	{
	public:
		CCall(long p_lCSTACallRefId);
		virtual ~CCall(void);

	public:
		void SetIsPostCallOver(bool p_bIsPostCallOver){ m_bIsPostCallOver = p_bIsPostCallOver; }
		bool GetIsPostCallOver(){ return m_bIsPostCallOver; }

		void SetIsAlarmCall(bool p_bAlarmCall){ m_bIsAlarmCall = p_bAlarmCall; }
		bool GetIsAlarmCall(){ return m_bIsAlarmCall; }

		void SetCSTACallRefId(long p_lCSTACallRefId){ m_lCSTACallRefId = p_lCSTACallRefId; }
		long GetCSTACallRefId() const { return m_lCSTACallRefId; }

		void SetRelateCSTACallRefId(long p_lRelateCSTACallRefId){ m_lRelateCATSCallRefId = p_lRelateCSTACallRefId; }
		long GetRelateCSTACallRefId() const { return m_lRelateCATSCallRefId; }

		void SetCTICallRefId(const std::string& p_strCallRefId){ m_strCTICallRefId = p_strCallRefId; }
		std::string GetCTICallRefId() const { return m_strCTICallRefId; }

		void SetCTIOldCallRefId(const std::string& p_strCallRefId) { m_strCTIoldCallRefId = p_strCallRefId; }
		std::string GetCTIOldCallRefId() const { return m_strCTIoldCallRefId; }

		void SetRelateCTICallRefId(const std::string& p_strRelateCallRefId){ m_strRelateCTICallRefId = p_strRelateCallRefId; }
		std::string GetRelateCTICallRefId() const { return m_strRelateCTICallRefId; }

		void SetOriginalCallerId(const std::string& p_strCallerId){ m_strOriginalCallerId = p_strCallerId; }
		std::string GetOriginalCallerId() const { return m_strOriginalCallerId; }

		void SetOriginalCalledId(const std::string& p_strCalledId){ m_strOriginalCalledId = p_strCalledId; }
		std::string GetOriginalCalledId() const { return m_strOriginalCalledId; }

		void SetIsTransferCall(bool p_bIsTransferCall){ m_bIsTransferCall = p_bIsTransferCall; }
		bool GetIsTransferCall(){ return m_bIsTransferCall; }

		void SetCallerId(const std::string& p_strCallerId){ /*if (m_strCallerId.empty())*/ m_strCallerId = p_strCallerId; }
		std::string GetCallerId() const { return m_strCallerId; }

		void SetCalledId(const std::string& p_strCalledId){ /*if (p_strCalledId.empty())*/ m_strCalledId = p_strCalledId; }
		std::string GetCalledId() const { return m_strCalledId; }

		void SetCalledParty(const std::string& p_strCalledParty){ /*if (m_strCalledPaty.empty())*/ m_strCalledPaty = p_strCalledParty; }
		std::string GetCalledParty() const { return m_strCalledPaty; }

		void SetCallState(const std::string& p_strCallState, const std::string& p_strStateTime);
		std::string GetCallState() const { return m_strCallState; }

		void SetStateTime(const std::string& p_strStateTime);
		std::string GetStateTime() const { return m_strStateTime; }

		void SetIncomingTime(const std::string& p_strIncomingTime){ if (m_strIncomingTime.empty()) m_strIncomingTime = p_strIncomingTime; }
		std::string GetIncomingTime() const { return m_strIncomingTime; }

		void SetAssignTime(const std::string& p_strAssignTime){ if (m_strAssignTime.empty()) m_strAssignTime = p_strAssignTime; }
		std::string GetAssignTime() const { return m_strAssignTime; }

		void SetRingTime(const std::string& p_strRingTime){ if (m_strRingTime.empty()) m_strRingTime = p_strRingTime; }
		std::string GetRingTime() const { return m_strRingTime; }

		void SetSignalBackTime(const std::string& p_strSignalBackTime){ if (m_strSignalBackTime.empty()) m_strSignalBackTime = p_strSignalBackTime; }
		std::string GetSignalBackTime() const { return m_strSignalBackTime; }

		void SetDialTime(const std::string& p_strDialTime){ if (m_strDialTime.empty()) m_strDialTime = p_strDialTime; }
		std::string GetDialTime() const { return m_strDialTime; }

		void SetReleaseTime(const std::string& p_strReleaseTime){ if (m_strReleaseTime.empty()) m_strReleaseTime = p_strReleaseTime; }
		std::string GetReleaseTime() const { return m_strReleaseTime; }

		void SetTalkTime(const std::string& p_strTalkTime){ if (m_strTalkTime.empty()) m_strTalkTime = p_strTalkTime; }
		std::string GetTalkTime() const { return m_strTalkTime; }

		void SetHangupTime(const std::string& p_strHangupTime){ if (m_strHangupTime.empty()) m_strHangupTime = p_strHangupTime; }
		std::string GetHangupTime() const { return m_strHangupTime; }
		
		void SetCallDirection(const std::string& p_strCallDierection){ m_strCallDierection = p_strCallDierection; }
		std::string GetCallDirection() const { return m_strCallDierection; }

		void SetIsBlackCall(bool p_bBlackCall){ m_bIsBlackCall = p_bBlackCall; }
		bool GetIsBlackCall() const { return m_bIsBlackCall; }

		void SetHangupType(const std::string& p_strHangupType);
		std::string GetHangupType() const { return m_strHangupType; }

		void SetHangupDevice(const std::string& p_strHangupDevice);
		std::string GetHangupDevice() const { return m_strHangupDevice; }

		void SetCallMode(const std::string& p_strCallMode){ m_strCallMode = p_strCallMode; }
		std::string GetCallMode() const{ return m_strCallMode; }

		// conference
		void SetConferenceCallRefId(long p_lCSTACallRefId){ m_lConferenceCallRefId = p_lCSTACallRefId; }

		void SetConfMembersQueue(std::vector<std::string> p_membersQueue){ m_vConfMembersQueue = p_membersQueue; }
		std::vector<std::string> GetConfMembersQueue() const { return m_vConfMembersQueue; }

		void SetConfCompere(const std::string& p_strCompere){ m_strCompere = p_strCompere; }
		std::string GetConfCompere() const{ return m_strCompere; }

		bool AddConfTalkParty(const std::string& p_strParty);
		bool DeleteConfTalkParty(const std::string& p_strParty);

		void SetConfmembersCount(int p_nCount){ m_nConfMembersCount = p_nCount; }
		int GetConfMembersCount();

        void SetE1RelayIndexNumber(const std::string& p_strE1RelayIndex, const std::string& p_strNumber, int& p_nSize);
		std::string GetE1RelayIndexNumber(const std::string& p_strE1RelayIndex, int& p_nSize);
		std::string GetE1RelayIndexByNumber(const std::string& p_strNumber, int& p_nSize);
		void DeleteE1RelayIndexNumber(const std::string& p_strE1RelayIndex);
		
		void AddE1Relay(const std::map<std::string, std::string>& p_mapRelayIndexNumber);

		std::map<std::string, std::string> m_mapRelayIndexNumber;	//E1ʱ���Ӧ�ĺ����

		void AddMemberDevice(const std::string& strDevice);
		void DeleteDevice(const std::string& strDevice);
		int  MemberDeviceCount();
		bool ExistDevice(const std::string& strDevice);

	private:
		bool			m_bIsPostCallOver;			//	�Ƿ��ѷ��ͻ�������¼�
		bool			m_bIsAlarmCall;				//	�Ƿ�Ϊ��������
		bool			m_bIsBlackCall;				// �Ƿ�Ϊ����������ĺ���
		bool			m_bIsTransferCall;			// �Ƿ�Ϊת�ƻ���

		long			m_lCSTACallRefId;			// �����������Ļ���ID
		long			m_lRelateCATSCallRefId;		//	�����������Ĺ�������ID(��ѯ)
		std::string		m_strCTICallRefId;			//	�ϲ�Ӧ�û��� ID
		std::string		m_strRelateCTICallRefId;	//	�ϲ�Ӧ�ù������� ID

		std::string		m_strCTIoldCallRefId;

		std::string		m_strOriginalCallerId;		// ԭʼ���к���
		std::string		m_strOriginalCalledId;		// ԭʼ���к���

		std::string		m_strCallerId;				// ���к���
		std::string		m_strCalledId;				// ���к���(110, 119��)
		std::string		m_strCalledPaty;			// ���зֻ���(7001,8001)

		std::string		m_strCallState;				// ����״̬(incoming, waiting, assing, ring,connect,hangup...)
		std::string		m_strStateTime;				// ��ǰ����״̬ʱ��

		std::string		m_strIncomingTime;			//	����ʱ��
		std::string		m_strAssignTime;			//	����ʱ��
		std::string		m_strRingTime;				//	����ʱ��
		std::string		m_strSignalBackTime;		//	����ʱ��
		std::string		m_strDialTime;				//	ժ��ʱ��
		std::string		m_strReleaseTime;			//	����ʱ��
		std::string		m_strTalkTime;				//	��ʼͨ��ʱ��
		std::string		m_strHangupTime;			//	�һ�ʱ��

		std::string		m_strCallDierection;		// ����: Out  ����: In

		std::string		m_strHangupType;			// �һ�����
		std::string		m_strHangupDevice;			// �һ��豸

		std::string		m_strCallMode;				// ��һ���У�single, ������У�Conference

		long			m_lConferenceCallRefId;			// ��ӻ����Ա�����в����Ļ��� ID
		int				m_nConfMembersCount;			// �����Ա����
		std::string		m_strCompere;					// ��������
		std::vector<std::string> m_vConfMembersQueue;	// �������

		std::mutex m_mutexMemberDevices;
		std::map<std::string, std::string> m_mapMemberDevices;
	};
}// end namespace