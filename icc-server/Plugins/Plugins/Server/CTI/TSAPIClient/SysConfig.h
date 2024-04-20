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

		void OnInit(IResourceManagerPtr p_pResourceManager);
		void LoadParamter();
		void ClearParamter();

		// AE Server
		std::string GetAEServerIP() const { return m_strAEServerIP; }
		std::string GetAEServerPort() const { return m_strAEServerPort; }
		std::string GetAESUserName() const { return m_strAESUserName; }
		std::string GetAESPassword() const { return m_strAESPsw; }

		std::string GetSlaveAEServerIP() const { return m_strSlaveAEServerIP; }
		std::string GetSlaveAEServerPort() const { return m_strSlaveAEServerPort; }
		std::string GetSlaveAESUserName() const { return m_strSlaveAESUserName; }
		std::string GetSlaveAESPassword() const { return m_strSlaveAESPsw; }

		int GetInitFailedMaxCount() { return m_nInitFailedMaxCount; };

		std::string GetLastConnectAvayaIP();
		bool SetCurrConnectAvayaIP(const string& p_lstrAvayaAESServerIP);

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
		//  �豸����ʱ��������λ����
		int GetMonitorTimeSpan() const { return m_nMonitorTimeSpan; }
		//	�豸���Ӵ���
		int GetMaxMonitorCount() const { return m_nMaxMonitorCount; }

	//	std::string GetDialRuleList() const { return m_strDialRuleList; }
		std::string GetInCallHead() const { return m_strInCallHead; }
		std::string GetOutCallHead() const { return m_strOutCallHead; }
		/*std::string GetLocalPhoneCallHead() const { return m_strLocalPhoneCallHead; }
		std::string GetRemotePhoneCallHead() const { return m_strRemotePhoneCallHead; }
		std::string GetLocalMobileCallHead() const { return m_strLocalMobileCallHead; }
		std::string GetRemoteMobileCallHead() const { return m_strRemoteMobileCallHead; }*/
		std::string GetWirelessCallHead() const { return m_strWirelessCallHead; }
		std::string GetTrunkRule() const { return m_strTrunkRule; }
		std::string	GetmonitorSeatList() const { return m_strMonitorSeatList; }

		std::string GetRouteDest() const { return m_strRouteDest; }

		//	��·�ɹ����ȡԭʼ����
		std::string GetCheckNum(const std::string& p_strSrcNum);
		//	������ǰ׺��ӳ��ֺ�
		std::string GetOutCallNumEx(const std::string& p_strSrcNum);
		bool GetOutCallNum(const std::string& p_strSrcNum, std::string& p_strDestNum);

		bool FindDefaultReadyAgent(const std::string& p_strAgent);

		bool GetCTITestFlag() { return m_bCTITestFlag; };
		int GetCTITestPort() { return m_nCTITestPort; };
		/*bool IsLocalPhone(const std::string& p_strOutCallNum);
		bool IsRemotePhone(const std::string& p_strOutCallNum);
		bool IsLocalMobile(const std::string& p_strOutCallNum);
		bool IsRemoteMobile(const std::string& p_strOutCallNum);*/

		bool GetMergeHuntGrpConver(std::string& p_strDestAcd, const std::string& p_strSrcNum);

        std::string GetIsUseBlackRoute() const { return m_strIsUseBlackRoute; }
		std::string GetBlackRouteDest() const { return m_strBlackRouteDest; }

		bool IsResonanceGroupNumber(const std::string& strDeviceNumber);

	public:
		std::list<CDevicePhone>	m_DevicePhoneList;
		std::list<CDeviceVir>	m_DeviceVirList;
		std::list<CACDGroup>	m_ACDGroupList;
		
		

	private:		
		std::mutex m_mutexLoadDialRuleList;
		std::list<CDialRule>	m_DialRuleList;

		std::mutex m_mutexLoadCheckNumList;
		std::list<CDialRule>	m_CheckNumList;

		std::mutex m_mutexDefaultReadyAgent;
		std::list<std::string>	m_DefaultReadyAgentList;	// Ĭ�����е���ϯ�б�

	private:
		void OnLoadConfigSuccessNotify(ObserverPattern::INotificationPtr p_pNotifiRequest);

		bool _ExistDeviceNumber(const std::string& strNumber);
		void _AddDeviceNumber(const std::string& strNumber);
		void _LoadMergeHuntGrpConver();		
		
		void _LoadCheckNumList();
		void _LoadDialRuleList();

		void _LoadResonanceGroups();

		void _LoadCommonParam();

		void _LoadDefaultReadyAgents();

	private:
		
		


		std::map<std::string, std::string> m_mapDevices;

		static boost::shared_ptr<CSysConfig> m_pInstance;

		bool						m_bParamterIsLoad;
		Log::ILogPtr				m_pLog;
		Config::IConfigPtr			m_ConfigPtr;
		StringUtil::IStringUtilPtr	m_StringUtilPtr;
		Redis::IRedisClientPtr		m_pRedisClient;

		ObserverPattern::IObserverCenterPtr	m_pObserverCenter;

		// TSAPI Server��ַ
		std::string m_strAEServerIP;
		// TSAPI Server�˿�
		std::string m_strAEServerPort;
		// CTI UserName
		std::string m_strAESUserName;
		// CTI Password
		std::string m_strAESPsw;
		//���ô������ж�ȡ�ĺ����б����ж��Ƿ񱻽������ߺ���
		std::string m_strEnbaleConfigCalloutCheck;


		// TSAPI ������
		std::string m_strSlaveAEServerIP;
		std::string m_strSlaveAEServerPort;
		std::string m_strSlaveAESUserName;
		std::string m_strSlaveAESPsw;


		// CTI Server �� TSAPI Server ���������, ��λ:��
		int		m_nHeartbeatTime;
		// CTI ִ�����ʱʱ�����ã�������ʱ������Ϊ�쳣, ��λ:��
		int		m_nSwitchCmdTimeOut;
		// ����쳣����, ������ʱ������Ϊ���쳣��������CallManage��, ��λ:����
		int		m_nCallInvalidTime;
		//  �豸����ʱ��������λ����
		int		m_nMonitorTimeSpan;		
		//	�豸���Ӵ���
		int		m_nMaxMonitorCount;

		int		m_nInitFailedMaxCount;


		int m_nMinLocalPhoneLen;		//	���غ�����С����
		int m_nMaxLocalPhoneLen;		//	���غ�����󳤶�
		int m_nMinZoneLen;				//	������С����
		int m_nMaxZoneLen;				//	������󳤶�
		int m_nMobilePreLen;			//	�ֻ�����ǰ׺

		int m_nAddrZeroPreLen;

		//	���ֺ�
		std::string m_strOutCallHead;
		/*std::string m_strLocalPhoneCallHead;
		std::string m_strRemotePhoneCallHead;
		std::string m_strLocalMobileCallHead;
		std::string m_strRemoteMobileCallHead;
		std::string m_strProvince;*/
		//	��ֺ�
		std::string m_strInCallHead;
		// ���Ź���
	//	std::string m_strDialRuleList;
		
		// PDTo
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

		//�Ƿ����ú�����·��
		std::string m_strIsUseBlackRoute;
		//������·�ɵ�ַ
		std::string m_strBlackRouteDest;

		bool m_bCTITestFlag;
		int m_nCTITestPort;

		std::mutex m_mutexMergeHuntGrpConvers;
		std::map<std::string, std::string> m_mapMergeHuntGrpConvers;

		std::mutex m_mutexLoadResonanceGroups;
		std::map<std::string, std::string> m_mapResonanceGroups;

		std::mutex m_mutexCalloutParam;
	};
}	// end namespace
