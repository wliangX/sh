#pragma once

#include "Agent.h"

namespace ICC
{
	class CAgentManager
	{
	public:
		CAgentManager();
		virtual ~CAgentManager();

		static boost::shared_ptr<CAgentManager> Instance();
		void ExitInstance();

	public:
		void OnInit(IResourceManagerPtr p_pResourceManager);
		void OnStart();
		void OnStop();

		void SetSwitchConnect(bool p_bConnectFlag);
	public:
		boost::shared_ptr<CAgent> GetAgentByDn(const std::string& p_strACDGrp, const std::string& p_strDeviceNum);
		std::list<boost::shared_ptr<CAgent>> GetAgentList(){ return m_agentList; }

		void LoadAllAgent();
		void ClearAllAgent();
		void GetAgentListState();
		void GetACDList(long p_lRequestId);
		void GetAgentList(long p_lRequestId, const std::string& p_strACDGrp);
		void OnAgentState(const std::string& p_strDevice, const std::string& p_strState);
		void ResetDefaultReadyList();

		void GetFreeAgentList(long p_lRequestId, const std::string& p_strACDGrp, const std::string& p_strDeptCode);

		std::string GetLocalTime();
		bool GetACDGrpByDn(std::string& p_strDestACDGrp, const std::string& p_strSrcDeviceNum);
		int GetReadyAgentCount(const std::string& p_strACDGrp);
		void GetReadyAgent(const std::string& p_strACDGrp, long p_lRequestId);
	//	bool GetReadyAgent(const std::string& p_strACDGrp, std::string& p_strDevice);
		void GetAgentState(const std::string& p_strInDeviceNum, std::string& p_strOutLodinMode, std::string& p_strOutReadyState);

		void SetLastQueryTime();
	//	long GetQueryTimeSpan();
		bool QueryAgentStateIsTimeout();
		void StartQueryAgentStateThread();
		void StopQueryAgentStateThread();
		/*static */void DoQueryAgentState();

		bool AgentIsLogin(const std::string& p_strDevice, const std::string& p_strACDGrp);
		void AgentLogin(const std::string& p_strDevice, const std::string& p_strACDGrp);
		void SetAgentState(const std::string& p_strDevice, const std::string& p_strACDGrp, const std::string& p_strReadyState);
		void PostAgentState(const std::string& p_strDevice, const std::string& p_strACDGrp,
			const std::string& p_strLoginMode, const std::string& p_strReadyState, const std::string& p_strStateTime);
		void LoginModeSync(const std::string& p_strDevice, const std::string& p_strACDGrp, E_AGENT_MODE_TYPE p_nLoginMode);
		void ReadyStateSync(const std::string& p_strDevice, const std::string& p_strACDGrp, const std::string& p_strReadyState);

		void SetCTITestData();
	private:
		static boost::shared_ptr<CAgentManager> m_pAgentManager;
		Log::ILogPtr							m_pLog;
		StringUtil::IStringUtilPtr				m_pStrUtil;
		DateTime::IDateTimePtr					m_pDateTimePtr;

		bool								m_bAgentIsLoad;
		bool								m_bThreadAlive;
		DateTime::CDateTime					m_oLastQueryTime;

		std::mutex								m_agentListMutex;
		std::list<boost::shared_ptr<CAgent>>	m_agentList;
		boost::shared_ptr<boost::thread>		m_pQueryAgentStateThread;

		bool m_bSwitchConnectFlag;
	};
};	//end namespace