#pragma once

/**
@Copyright Copyright (C), 2018
@file		BlackListManage.h
@created	2018/4/2
@brief		������������
1, ����Ӧ�ò㷢�͵���Ӻ���������
2, ����Ӧ�ò㷢�͵�ɾ������������
3, �ṩ�������Ĳ�ѯ
4, ά��������

@author
*/

namespace ICC
{
	class CLimitNum
	{
	public:
		CLimitNum();
		virtual ~CLimitNum();

	public:
		void SetLimitType(const std::string& p_strLimitType) { m_strLimitType = p_strLimitType; }
		std::string GetLimitType() const { return m_strLimitType; }

		void SetLimitNum(const std::string& p_strLimitNum){ m_strLimitNum = p_strLimitNum; }
		std::string GetLimitNum() const { return m_strLimitNum; }

		void SetLimitMin(const std::string& p_strLimitMin){ m_strLimitMin = p_strLimitMin; }
		std::string GetLimitMin() const { return m_strLimitMin; }

		void SetLimitReason(const std::string& p_strLimitReason){ m_strLimitReason = p_strLimitReason; }
		std::string GetLimitReason() const { return m_strLimitReason; }

		void SetBeginTime(const std::string& p_strBeginTime){ m_strBeginTime = p_strBeginTime; }
		std::string GetBeginTime() const { return m_strBeginTime; }

		void SetEndTime(const std::string& p_strEndTime){ m_strEndTime = p_strEndTime; }
		std::string GetEndTime() const { return m_strEndTime; }

		void SetStaffCode(const std::string& p_strStaffCode){ m_strStaffCode = p_strStaffCode; }
		std::string GetStaffCode() const { return m_strStaffCode; }

		void SetStaffName(const std::string& p_strStaffName){ m_strStaffName = p_strStaffName; }
		std::string GetStaffName() const { return m_strStaffName; }

		void SetApprover(const std::string& p_strApprover) { m_strApprover = p_strApprover; }
		std::string GetApprover() const { return m_strApprover; }

		void SetApprovedInfo(const std::string& p_strApprovedInfo) { m_strApprovedInfo = p_strApprovedInfo; }
		std::string GetApprovedInfo() const { return m_strApprovedInfo; }

		void SetReceiptDeptName(const std::string& p_strReceiptDeptName) { m_strReceiptDeptName = p_strReceiptDeptName; }
		std::string GetReceiptDeptName() const { return m_strReceiptDeptName; }

		void SetReceiptDeptDistrictCode(const std::string& p_strReceiptDeptDistrictCode) { m_strReceiptDeptDistrictCode = p_strReceiptDeptDistrictCode; }
		std::string GetReceiptDeptDistrictCode() const { return m_strReceiptDeptDistrictCode; }
	private:
		std::string m_strLimitType;		// ��������
		std::string m_strLimitNum;		// ���޺���
		std::string m_strLimitMin;		// ����ʱ��
		std::string m_strLimitReason;	// ����ԭ��
		std::string m_strBeginTime;		// ��ʼʱ��
		std::string m_strEndTime;		// ����ʱ��
		std::string m_strStaffCode;		// ��Ա���
		std::string m_strStaffName;		// ��Ա����
		std::string m_strApprover;		// ������
		std::string m_strApprovedInfo;	// ������Ϣ
		std::string m_strReceiptDeptName;	// ������������
		std::string m_strReceiptDeptDistrictCode;	// ��������
	};

	//////////////////////////////////////////////////////////////////////////
	class CBlackListManager
	{
	public:
		CBlackListManager(void);
		virtual ~CBlackListManager(void);

		static boost::shared_ptr<CBlackListManager> Instance();
		void ExitInstance();

	public:
		void SetLogPtr(Log::ILogPtr	p_LogPtr){ m_LogPtr = p_LogPtr; }
		void SetDateTime(DateTime::IDateTimePtr p_DateTimePtr){ m_DateTimePtr = p_DateTimePtr; }
		void SetSwitchClientPtr(ISwitchClientPtr p_SwitchClientPtr){ m_SwitchClientPtr = p_SwitchClientPtr; }

		void InitBlackList(std::map<std::string, boost::shared_ptr<CLimitNum>> p_mapBlackList);
		void WriteBlackListToSwitch();

		void SetBlackListToSwitch(const std::string& p_strLimitType, const std::string& p_strLimitNum);
		void DeleteBlackListFromSwitch(const std::string& p_strLimitType, const std::string& p_strLimitNum);
		void DeleteAllBlackListFromSwitch();

		bool FindBlackNumber(const std::string& p_strLimitNum);
		void AddBlackList(const std::string& p_strLimitNum, boost::shared_ptr<CLimitNum> p_pLimitNumObj);
		void DeleteBlackList(const std::string& p_strLimitNum);
		void ClearBlackList();

		bool GetLimitTime(const std::string& p_strDictionary, std::string& p_strBeginTime, std::string& p_strEndTime);
		void GetAllBlackList(PROTOCOL::CGetAllBlackListRespond& p_oOutRespond, const std::string& p_strInLimitNum,
			int p_nInPageSize, int p_nInPageIndex);

		bool BlackListIsTimeout(std::string& p_strLimitType, std::string& p_strLimitNum);	// ��ʱ���������Ƿ����
		void ProcessBlackListTimeout(const std::string& p_strLimitType, const std::string& p_strLimitNum);

		void StartThread();
		void StopThread();
		/*static */void DoCheckBlackList();

	private:
		static boost::shared_ptr<CBlackListManager>	m_pInstance;

		bool						m_bThreadAlive;
		ISwitchClientPtr			m_SwitchClientPtr;
		DateTime::IDateTimePtr		m_DateTimePtr;
		Log::ILogPtr				m_LogPtr;

		std::mutex											m_blackListMutex;
		std::map<std::string, boost::shared_ptr<CLimitNum>>	m_mapBlackList;

		boost::shared_ptr<boost::thread>	m_pCheckTimeoutThread;
	};
}

