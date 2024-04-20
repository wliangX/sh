#pragma once

namespace ICC
{
	class CLogicalCall
	{
	public:
		long			m_lCSTACallRefId;
		std::string		m_strDeviceState;
		std::string		m_strStateTime;
	};

	class CDevice
	{
	public:
		CDevice(const std::string& p_strDeviceNum, int p_iDeviceType, const std::string& p_strAgentId = "", const std::string& p_strPsw = "");
		virtual ~CDevice();

	public:
		std::string GetDeviceNum() const { return m_strDeviceNum; }
		int GetDeviceType() const { return m_iDeviceType; }
		std::string GetDeviceTypeString();

		void SetDeviceState(long p_lCSTACallRefId, const std::string& p_strDeviceState, const std::string& p_strStateTime);
		std::string GetDeviceState();// const { return m_strDeviceState; }

		void SetStateTime();
		std::string GetStateTime(long p_lCSTACallRefId);

		void SetMonitorId(long p_lMonitorId){ m_lMonitorId = p_lMonitorId; }
		long GetMonitorId(){ return m_lMonitorId; }

		void SetRouteRegId(long p_lRouteRegId){ m_lRouteRegId = p_lRouteRegId; }
		long GetRouteRegId(){ return m_lRouteRegId; }

		void SetCSTACallRefId(long p_lCallRefId){ m_lCSTACallRefId = p_lCallRefId; }
		long GetCSTACallRefId(){ return m_lCSTACallRefId; }

		void SetIsAgent(bool p_bIsAgent){ m_bIsAgent = p_bIsAgent; }
		std::string GetAgentId() const{ return m_strAgentId; }
		std::string GetAgentPsw() const { return m_strPsw; }

		bool GetIsMonistor() const { return m_bIsMonitor; }
		void SetIsMonistor(bool p_bIsMonistor) { m_bIsMonitor = p_bIsMonistor; }

	public:
		void AddLogicalCall(long p_lCSTACallRefId, const std::string& p_strDeviceState, const std::string& p_strStateTime);
		void UpdateLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceState, const std::string& p_strStateTime);
		void DeleteLogicalCall(long p_lCSTACallRefId);

		bool FindLogicalCall(long p_lCSTACallRefId);
		int GetLogicalCallCount();
		bool GetLastLogicalCall(long &p_lCSTACallRefId);
		std::string GetLastLogicalCallState();
		std::string GetLogicalCallState(long p_lCSTACallRefId);
		std::string GetLogicalCallStateTime(long p_lCSTACallRefId);
		bool GetCurrentLogicalCall(long &p_lCSTACallRefId, std::string& p_strDeviceState, std::string& p_strStateTime);

	protected:
		std::string		m_strDeviceNum;
		int				m_iDeviceType;

		long			m_lMonitorId;
		long			m_lRouteRegId;
		long			m_lCSTACallRefId;
		bool			m_bIsMonitor;
		
		bool			m_bIsAgent;
		std::string		m_strAgentId;
		std::string		m_strPsw;

		std::map<long, boost::shared_ptr<CLogicalCall>>	m_mapLogicalCallQueue;
	};

}// end namespace ICC
