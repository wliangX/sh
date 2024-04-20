#pragma once

namespace ICC {

	class CAgent
	{
	public:
		CAgent(std::string p_strACDGroup, std::string p_strAgent, std::string p_strPsw, std::string p_strDeviceNum);
		virtual ~CAgent();

	public:
		bool Login();
		bool Logout();

		void SetDefaultReady(bool p_bDefaultReady){ m_bDefaultReady = p_bDefaultReady; }
		bool GetDefaultReady(){ return m_bDefaultReady; }

		void GetAgentPar(std::string& p_strGroupNum, std::string& p_strAgent, std::string& p_strPsw, std::string& p_strDeviceNum);
		std::string GetAgent() const { return m_strAgent; }
		std::string GetACDGroup() const { return m_strACDGroup; }
		std::string GetDeviceNum() const { return m_strDeviceNum; }

		bool IsLogin(){ return (m_agentMode == AGENT_MODE_LOGIN); }
		bool IsReady();

		void SetAgentStateString(const std::string& p_strAgentMode, const std::string& p_strAgentReady);
		void SetAgentState(E_AGENT_MODE_TYPE p_agentMode, E_AGENT_READY_TYPE p_agentReadyType, const std::string& p_strStateTime);

		std::string GetAgentModeString() const{ return AgentModeTypeString[m_agentMode]; }
		std::string GetAgentReadyString() const{ return AgentReadyTypeString[m_agentReady]; }
		
		void SetAgentMode(E_AGENT_MODE_TYPE p_agentMode, const std::string& p_strStateTime);
		void SetAgentState(E_AGENT_READY_TYPE p_agentState, const std::string& p_strStateTime);
		E_AGENT_MODE_TYPE GetAgentMode() const{ return m_agentMode; }
		E_AGENT_READY_TYPE GetAgentReady() const{ return m_agentReady; }

		void SetAgentStateTime(const std::string& p_strStateTime){ m_strStateTime = p_strStateTime; }
		std::string GetAgentStateTime() const{ return m_strStateTime; }

	private:
		E_AGENT_MODE_TYPE	m_agentMode;		//AGENT_MODE_LOGIN,AGENT_MODE_LOGOUT
		E_AGENT_READY_TYPE	m_agentReady;		//AGENT_READY,AGENT_NOTREADY

		bool			m_bDefaultReady;
		std::string		m_strStateTime;
		std::string		m_strACDGroup;		//所属ACD组的名字
		std::string		m_strAgent;			//Agent
		std::string		m_strPsw;			//密码
		std::string		m_strDeviceNum;
	};

}//namespace ICC end


