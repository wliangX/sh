#pragma once

namespace ICC
{
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

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	class CAgentManager
	{
	public:
		CAgentManager();
		virtual ~CAgentManager();

		static boost::shared_ptr<CAgentManager> Instance();
		void ExitInstance();

	public:
		void SetLogPtr(Log::ILogPtr p_pLog){ m_pLog = p_pLog; }
		void SetDateTimePtr(DateTime::IDateTimePtr p_pDateTime){ m_pDateTime = p_pDateTime; }
		void SetStringUtilPtr(StringUtil::IStringUtilPtr p_pStringUtil){ m_pStrUtil = p_pStringUtil; }

	public:
		boost::shared_ptr<CAgent> GetAgentByDn(const std::string& p_strACDGrp, const std::string& p_strDeviceNum);
		std::list<boost::shared_ptr<CAgent>> GetAgentList(){ return m_agentList; }

		void LoadAllAgent();
		void ClearAllAgent();
		void SetDefaultAgentState();
		void GetAgentListState();
		void SetInitOver(bool p_bInitOver){ m_bInitOver = p_bInitOver; }
		bool IsInitOver(){ return m_bInitOver; }

		void GetACDList(long p_lRequestId);
		void GetAgentList(long p_lRequestId, const std::string& p_strACDGrp);
		void OnAgentState(const std::string& p_strDevice, const std::string& p_strState);
		void ProcesAgentState(const std::string& p_strDevice);

		std::string GetLocalTime();
		bool GetACDGrpByDn(std::string& p_strDestACDGrp, const std::string& p_strSrcDeviceNum);
		int GetReadyAgentCount(const std::string& p_strACDGrp);
		void GetReadyAgent(const std::string& p_strACDGrp, long p_lRequestId);
	//	bool GetReadyAgent(const std::string& p_strACDGrp, std::string& p_strDevice);
		void GetAgentState(const std::string& p_strInDeviceNum, std::string& p_strOutLodinMode, std::string& p_strOutReadyState);

		bool AgentIsLogin(const std::string& p_strDevice, const std::string& p_strACDGrp);
		void AgentLogin(const std::string& p_strDevice, const std::string& p_strACDGrp);
		void PostAgentState(const std::string& p_strDevice, const std::string& p_strACDGrp,
			const std::string& p_strLoginMode, const std::string& p_strReadyState, const std::string& p_strStateTime, const std::string& p_strLogoutReason ="");
		void LoginModeSync(const std::string& p_strDevice, const std::string& p_strACDGrp, E_AGENT_MODE_TYPE p_nLoginMode);
		void ReadyStateSync(const std::string& p_strDevice, const std::string& p_strACDGrp, const std::string& p_strReadyState,const std::string& p_strLogoutReason = "");

		void SetInitAgentState();
	private:
		static boost::shared_ptr<CAgentManager> m_pAgentManager;
		Log::ILogPtr							m_pLog;
		DateTime::IDateTimePtr					m_pDateTime;
		StringUtil::IStringUtilPtr				m_pStrUtil;

		std::mutex								m_agentListMutex;
		std::list<boost::shared_ptr<CAgent>>	m_agentList;

		bool									m_bInitOver;
	};
};	//end namespace