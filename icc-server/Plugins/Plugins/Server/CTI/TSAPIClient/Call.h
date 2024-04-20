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

		std::map<std::string, std::string> m_mapRelayIndexNumber;	//E1时序对应的号码表

		void AddMemberDevice(const std::string& strDevice);
		void DeleteDevice(const std::string& strDevice);
		int  MemberDeviceCount();
		bool ExistDevice(const std::string& strDevice);

	private:
		bool			m_bIsPostCallOver;			//	是否已发送话务结束事件
		bool			m_bIsAlarmCall;				//	是否为报警呼入
		bool			m_bIsBlackCall;				// 是否为黑名单号码的呼叫
		bool			m_bIsTransferCall;			// 是否为转移话务

		long			m_lCSTACallRefId;			// 交换机产生的话务ID
		long			m_lRelateCATSCallRefId;		//	交换机产生的关联话务ID(咨询)
		std::string		m_strCTICallRefId;			//	上层应用话务 ID
		std::string		m_strRelateCTICallRefId;	//	上层应用关联话务 ID

		std::string		m_strCTIoldCallRefId;

		std::string		m_strOriginalCallerId;		// 原始主叫号码
		std::string		m_strOriginalCalledId;		// 原始被叫号码

		std::string		m_strCallerId;				// 主叫号码
		std::string		m_strCalledId;				// 被叫号码(110, 119等)
		std::string		m_strCalledPaty;			// 被叫分机号(7001,8001)

		std::string		m_strCallState;				// 话务状态(incoming, waiting, assing, ring,connect,hangup...)
		std::string		m_strStateTime;				// 当前话务状态时间

		std::string		m_strIncomingTime;			//	呼入时间
		std::string		m_strAssignTime;			//	分配时间
		std::string		m_strRingTime;				//	振铃时间
		std::string		m_strSignalBackTime;		//	回铃时间
		std::string		m_strDialTime;				//	摘机时间
		std::string		m_strReleaseTime;			//	早释时间
		std::string		m_strTalkTime;				//	开始通话时间
		std::string		m_strHangupTime;			//	挂机时间

		std::string		m_strCallDierection;		// 呼出: Out  呼入: In

		std::string		m_strHangupType;			// 挂机类型
		std::string		m_strHangupDevice;			// 挂机设备

		std::string		m_strCallMode;				// 单一呼叫：single, 会议呼叫：Conference

		long			m_lConferenceCallRefId;			// 添加会议成员过程中产生的话务 ID
		int				m_nConfMembersCount;			// 会议成员数量
		std::string		m_strCompere;					// 会议主持
		std::vector<std::string> m_vConfMembersQueue;	// 会议各方

		std::mutex m_mutexMemberDevices;
		std::map<std::string, std::string> m_mapMemberDevices;
	};
}// end namespace