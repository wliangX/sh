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

		void OnInit(IResourceManagerPtr p_pResourceManager);
		void OnStart();
		void OnStop();
		
	public:
		void AddAgent(std::string& p_strAgentId, std::string& p_strAgentPsw, std::string& p_strDeviceNum);

		bool GetAgent(IGetAgentListResultNotifPtr p_pResultNotif, const std::string p_strACDNum = "");
		bool GetAgent(IGetFreeAgentResultNotifPtr p_pResultNotif, const std::string p_strACDNum = "");
		bool GetAgent(IGetReadyAgentResultNotifPtr p_pResultNotif, const std::string p_strACDNum = "");
		bool GetACDList(IGetACDListResultNotifPtr p_pResultNotif);

		void InitFreeAgentList(const std::vector<std::string>& p_aesExtensions);
	public:

		void SetSwitchConnect(bool p_bConnectFlag);
		void ReadyStateSync(const std::string& p_strDevice, const std::string& p_strACDGrp, E_AGENT_MODE_TYPE l_agentMode, E_AGENT_READY_TYPE l_agentReadyType);
		int GetReadyAgentCount(const std::string& p_strACDGrp);
		void GetAgentState(const std::string& p_strInDeviceNum, std::string& p_strOutLodinMode, std::string& p_strOutReadyState);
		std::string GetAgentACDGrpList(const std::string& p_strDeviceNum);
	private:
		void _LoadAllAgent();
		void _ClearAllAgent();
		void _PostAgentState(const std::string& p_strDevice, const std::string& p_strACDGrp,
				const std::string& p_strLoginMode, const std::string& p_strReadyState, const std::string& p_strStateTime);
	public:
		static boost::shared_ptr<CAgentManager> m_pAgentManager;

		Log::ILogPtr				m_pLog;
		DateTime::IDateTimePtr		m_pDateTimePtr;
		StringUtil::IStringUtilPtr	m_pStrUtil;
	private:
		bool						m_bAgentIsLoad;
		std::mutex					m_mapAgentTBMutex;

		std::map<std::string, boost::shared_ptr<CAgent>> m_mapAgentTB;
	};
};	//end namespace