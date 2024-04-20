#pragma once

//#include "../../../Gateway/SMP/Staff.h"
#include "Staff.h"
namespace ICC
{
	class CBusinessImpl :
		public CBusinessBase
	{
	public:
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();
	public:
		void OnNotifiInitChatWindowRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiAddChatGroupRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiEditChatGroupRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiDelChatGroupRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiGetChatGroupsRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiGetChatGroupInfoRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiAddChatGroupMemberRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiRemoveChatGroupMemberRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiExitChatGroupRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiSendChatMsgRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiGetChatRecordsRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiGetGroupChatRecordsRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnNotifiSetMsgStateRequest(ObserverPattern::INotificationPtr p_pNotify);
    public:
		void OnTimer(ObserverPattern::INotificationPtr p_pNotify);

	private:
		bool InitChatWindow(std::string p_strStaffCode, unsigned int uiPageSize, unsigned int uiPageIndex, ChatInitWindow& pChatInitWindow);
		unsigned int GetChatItemCount(std::string p_strStaffCode);
		unsigned int GetChatRecordsCount(std::string p_strStaffCode);
		unsigned int GetP2PRecordsCount(std::string p_strSender, std::string p_strReceiver);
		unsigned int GetGroupRecordsCount(std::string p_strGroupID);

		bool GetP2PRecords(std::string p_strSender,
			std::string p_strReceiver,
			unsigned int uiPageSize,
			unsigned int uiPageIndex,
			P2PRecords& pChatRecords);

		bool GetP2PRecordsEx(std::string p_strSender, 
			std::string p_strReceiver, 
			std::string p_strStaffCode,
			unsigned int uiPageSize,
			unsigned int uiPageIndex, 
			P2PRecords& pChatRecords, 
			P2PRecord& pLastChatRecord, 
			unsigned int & p_iUnreadCount);
		bool GetGroupRecords(std::string p_strGroupID, 
			unsigned int uiPageSize, 
			unsigned int uiPageIndex, 
			GroupRecords& pChatRecords);
		bool GetGroupRecordsEx(std::string p_strGroupID,
			std::string p_strStaffCode,
			unsigned int uiPageSize,
			unsigned int uiPageIndex,
			GroupRecords& pChatRecords,
			GroupRecord& pLastChatRecord,
			unsigned int & p_iUnreadCount);

		//���Ⱥ��
		std::string AddChatGroup(std::string p_strUserCode, std::string p_strGroupName, const std::string& strTransGuid);
		//�༭Ⱥ��
		bool EditChatGroup(std::string p_strGroupID, std::string p_strGroupName);
		//��ȡȺ��
		std::string GetChatGroupAdmin(std::string p_strGroupID);
		std::string GetNewGroupAdmin(std::string p_strGroupID, std::string p_strOldAdmin, const std::string& strTransGuid);
		//����Ⱥ��
		bool UpgradeGroupAdmin(std::string p_strGroupID, std::string p_strNew, const std::string& strTransGuid);
		//��ɢȺ
		bool DissolveGroup(std::string p_strGroupID, const std::string& strTransGuid);
		//��ȡȺ���б�
		bool GetChatGroups(ChatGroups& p_chatGroups);
		bool GetChatGroupInfo(std::string p_strGroupID, ChatGroupInfo& p_chatGroupInfo);
		//���Ⱥ�ĳ�Ա
		bool AddChatGroupMember(std::string p_strOperator, std::string p_strGroupID, const Members& p_groupMembers, const std::string& strTransGuid);
		//ɾ��Ⱥ�ĳ�Ա
		bool DelChatGroupMember(std::string p_strOperator, std::string p_strGroupID, std::string p_strStaffCode, const std::string& strTransGuid);
		//�˳�Ⱥ����
		bool ExitChatGroup(std::string p_strGroupID, std::string p_strStaffCode);
		//����������Ϣ
		void SendChatMessage(const CSendChatMsg& p_chatMessage, std::string l_strMsgID);
		//��������Ϣ��
		std::string SaveMessage(const CSendChatMsg& p_chatMessage);
		bool SaveMessageTag(const CSendChatMsg& p_chatMessage, std::string p_strMsgID);
		//ͨ����Ա�����ȡ��¼��ϯλ��
		std::string GetSeatByStaffCode(std::string p_strStaffCode);
		//ͨ����Ա�����ȡ��Ա��Ϣ
		bool GetStaffInfo(std::string p_strStaffCode, Data::CStaffInfo& p_staffInfo);
		//������Ϣ״̬
		bool SetMsgState(std::string p_strMsgID, std::string p_strReceiver);

	private:
		void BuildRespondHeader(const PROTOCOL::CHeader& p_oRequestHeader, PROTOCOL::CHeader& p_oRespondHeader, std::string p_strCmd);
		void BuildSyncHeader(std::string p_strTopic, std::string p_strCmd, PROTOCOL::CHeader& p_oRespondHeader);
	private:
        DataBase::IDBConnPtr m_pDBConn;
		Config::IConfigPtr m_pConfig;
        Log::ILogPtr m_pLog;
		Lock::ILockFactoryPtr m_pLockFty;
		Lock::ILockPtr m_pAlarmLock;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
        StringUtil::IStringUtilPtr m_pString;
		DateTime::IDateTimePtr m_pDateTime;
		Timer::ITimerManagerPtr m_pTimerMgr;
		Redis::IRedisClientPtr m_pRedisClient;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
	};
}
