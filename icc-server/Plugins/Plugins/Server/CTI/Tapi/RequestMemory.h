#pragma once

namespace ICC
{
	class CRequestMemory
	{
	public:
		CRequestMemory();
		virtual ~CRequestMemory();

	public:
		void SetNotifiRequest(ObserverPattern::INotificationPtr p_pNotifiRequest) { m_pNotifiRequest = p_pNotifiRequest; };
		ObserverPattern::INotificationPtr GetNotifiRequest(){ return m_pNotifiRequest; }

		void SetMsgId(const std::string& p_strMsgId){ m_strMsgId = p_strMsgId; }
		std::string GetMsgId() const { return m_strMsgId; }

		void SetCaseId(const std::string& p_strCaseId){ m_strCaseId = p_strCaseId; }
		std::string GetCaseId() const { return m_strCaseId; }

		void SetRelateCallRefId(const std::string& p_strRelateCallRefId){ m_strRelateCallRefId = p_strRelateCallRefId; }
		std::string GetRelateCallRefId() const { return m_strRelateCallRefId; }

		void SetReceiptCode(const std::string& p_strReceiptCode){ m_strReceiptCode = p_strReceiptCode; }
		std::string GetReceiptCode() const { return m_strReceiptCode; }

		void SetReceiptName(const std::string& p_strReceiptName){ m_strReceiptName = p_strReceiptName; }
		std::string GetReceiptName() const { return m_strReceiptName; }

		void SetRequestId(long p_lRequestId){ m_lRequestId = p_lRequestId; }
		long GetRequestId(){ return m_lRequestId; }

	private:
		ObserverPattern::INotificationPtr m_pNotifiRequest;
		std::string		m_strCaseId;
		std::string		m_strMsgId;
		std::string		m_strRelateCallRefId;
		std::string		m_strReceiptCode;
		std::string		m_strReceiptName;
		long			m_lRequestId;
	};

	//////////////////////////////////////////////////////////////////////////
	class CRequestMemoryManager
	{
	public:
		CRequestMemoryManager();
		virtual ~CRequestMemoryManager();

		static boost::shared_ptr<CRequestMemoryManager> Instance();
		void ExitInstance();
	public:
		void AddRequestMemory(long p_lRequestId, boost::shared_ptr<CRequestMemory> p_pRequest);
		boost::shared_ptr<CRequestMemory> GetRequestMemory(long p_lRequestId);
		ObserverPattern::INotificationPtr GetNotifiRequest(long p_lRequestId);
		void DeleteRequestMemory(long p_lRequestId);
		void ClearRequestList();

	private:
		static boost::shared_ptr<CRequestMemoryManager> m_pInstance;

		std::mutex	m_RequestListMutex;
		std::map<long, boost::shared_ptr<CRequestMemory>> m_mapRequestList;
	};
}