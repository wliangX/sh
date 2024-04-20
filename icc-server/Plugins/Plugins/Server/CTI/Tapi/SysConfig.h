#pragma once

namespace ICC
{
	class CSysConfig
	{
	public:
		CSysConfig(void);
		virtual ~CSysConfig(void);

	public:
		static boost::shared_ptr<CSysConfig> Instance();
		void ExitInstance();

		void SetLogPtr(Log::ILogPtr p_pLog){ m_pLog = p_pLog; }
		void SetConfigPtr(Config::IConfigPtr p_pConfig){ m_pConfig = p_pConfig; }
		void SetStringUtilPtr(StringUtil::IStringUtilPtr p_pStringUtil){ m_pStringUtil = p_pStringUtil; }
		void SetRedisClientPtr(Redis::IRedisClientPtr p_pRedisClient){ m_pRedisClient = p_pRedisClient; }
		
		void LoadParamter();
		void ClearParamter();

		bool GetUseAnalogueTrunk() { return m_bUseAnalogueTrunk; }
		// �������
		int	GetHeartbeatTime() const { return m_nHeartbeatTime; }
		// ���ؽ�����ִ�����ʱʱ�����ã�������ʱ�䣬������û���κλ�Ӧ������Ϊ�쳣��ɾ������
		int	GetSwitchCmdTimeOut() const { return m_nSwitchCmdTimeOut; }
		// ����쳣����, ������ʱ������Ϊ���쳣��������CallManage�еĺ�����Ҫɾ��
		int	GetCallInvalidTime() const { return m_nCallInvalidTime; }
		// ��С��ϯ��
		int	GetMinOnlineAgentNum() const { return m_nMinOnlineAgentNum; }
		// �ֺ�����ʱ��
		int	GetPollingCallTime() const { return m_nPollingCallTime; }
		//	Agent ״̬��ѯʱ����
		int	GetAgentStateTime() const { return m_nAgentStateTime; }
		int GetConferenceRingTimeout() const { return m_nConferenceRingTimeout; }
		int GetTransferRingTimeout() const { return m_nTransferRingTimeout; }

		std::string GetDialRule() const { return m_strDialRule; }
		std::string GetOutCallHead() const { return m_strOutCallHead; }
		std::string GetWirelessCallHead() const { return m_strWirelessCallHead; }
		std::string GetTrunkRule() const { return m_strTrunkRule; }

		std::string GetRouteDest() const { return m_strRouteDest; }
		//	��·�ɹ����ȡԭʼ����
		std::string GetCheckNum(const std::string& p_strSrcNum);
		//	������ǰ׺��ӳ��ֺ�
		bool GetOutCallNum(const std::string& p_strSrcNum, std::string& p_strDestNum);

		bool GetACDGrpConver(std::string& p_strDestACDGrp, const std::string& p_strSrcACDGrp);
		bool GetACDGrpTypeByDn(const std::string& p_strInDeviceNum, std::string& p_strOutDeviceType);
		bool GetDeviceTypeByDn(const std::string& p_strInDeviceNum, std::string& p_strOutDeviceType);

		bool FindDefaultReadyAgent(const std::string& p_strAgent);

	public:
		std::list<CDevicePhone>	m_DevicePhoneList;	// �����б�
		std::list<CDeviceVir>	m_DeviceVirList;
		std::list<CACDGroup>	m_ACDGroupList;		// ACD ���Ա�б�
		std::list<CDialRule>	m_DialRuleList;		// ���й���
		std::list<std::string>	m_DefaultReadyAgentList;	// Ĭ�����е���ϯ�б�

	private:
		static boost::shared_ptr<CSysConfig> m_pInstance;

		bool						m_bParamterIsLoad;
		Log::ILogPtr				m_pLog;
		Config::IConfigPtr			m_pConfig;
		StringUtil::IStringUtilPtr	m_pStringUtil;
		Redis::IRedisClientPtr		m_pRedisClient;

		//	Redis �������
		/*std::string m_strRedisAddr;
		bool l_bRedisIsCluster;
		int l_nRedisTTL;*/
		bool m_bUseAnalogueTrunk;

		// CTI Server �� TSAPI Server ���������, ��λ:��
		int		m_nHeartbeatTime;
		// CTI ִ�����ʱʱ�����ã�������ʱ������Ϊ�쳣, ��λ:��
		int		m_nSwitchCmdTimeOut;
		// ����쳣����, ������ʱ������Ϊ���쳣��������CallManage��, ��λ:����
		int		m_nCallInvalidTime;

		int m_nMinLocalPhoneLen;		//	���غ�����С����
		int m_nMaxLocalPhoneLen;		//	���غ�����󳤶�
		int m_nMinZoneLen;				//	������С����
		int m_nMaxZoneLen;				//	������󳤶�
		int m_nMobilePreLen;			//	�ֻ�����ǰ׺

		//	���ֺ�
		std::string m_strOutCallHead;
		// ���Ź���
		std::string m_strDialRule;
		
		// PDTǰ׺
		std::string m_strWirelessCallHead;
		// Rule �ж����ߺ����������ʽ
		std::string m_strTrunkRule;
		// ��С��ϯ��
		int		m_nMinOnlineAgentNum;
		// �ֺ�ʱ����
		int		m_nPollingCallTime;
		// ��ϯ״̬ɨ��ʱ����
		int		m_nAgentStateTime;
		// ���������������ϯ
		std::string m_strMonitorSeatList;
		//	������·�ɵ�ַ
		std::string m_strRouteDest;

		int									m_nTransferRingTimeout;			//	���峬ʱʱ������λ���룩
		int									m_nConferenceRingTimeout;		//	���峬ʱʱ������λ���룩
		std::map<std::string, std::string>	m_mapACDGrpConversion;			//	ACD ת���б�
	};
}	// end namespace
