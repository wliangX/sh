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

		std::string GetLocalIP();
		std::string GetCTIServerPort();
		std::string GetHCPConnectLocalIP();

		bool GetPhoneTransferEnable();
		// AE Server
		std::string GetAEServerIP() const { return m_strAEServerIP; }
		std::string GetAEServerPort() const { return m_strAEServerPort; }
		std::string GetAESUserName() const { return m_strAESUserName; }
		std::string GetAESPassword() const { return m_strAESPsw; }

		std::string GetAESlaveServerIP() const { return m_strSlaveAEServerIP; }

		std::string GetAESNodeName() const { return m_strAESNodeName; }

		// 心跳间隔
		int	GetHeartbeatTime() const { return m_nHeartbeatTime; }
		// 返回交换机执行命令超时时间设置，超过该时间，交换机没有任何回应，则认为异常，删除任务
		int	GetSwitchCmdTimeOut() const { return m_nSwitchCmdTimeOut; }
		// 检查异常呼叫, 超过该时间则认为是异常，残留在CallManage中的呼叫需要删除
		int	GetCallInvalidTime() const { return m_nCallInvalidTime; }
		// 最小在席数
		int	GetMinOnlineAgentNum() const { return m_nMinOnlineAgentNum; }
		// 轮呼呼叫时间
		int	GetPollingCallTime() const { return m_nPollingCallTime; }
		//	Agent 状态查询时间间隔
		int	GetAgentStateTime() const { return m_nAgentStateTime; }
		//  设备监视时间间隔，单位：秒
		int GetMonitorTimeSpan() const { return m_nMonitorTimeSpan; }
		//	设备监视次数
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

		//	按路由规则获取原始号码
		std::string GetCheckNum(const std::string& p_strSrcNum);
		//	按号码前缀添加出局号
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

		int GetWaitAnswerMaxTime() { return m_nWaitAnswerMaxTime; };
	public:
		std::list<CDevicePhone>	m_DevicePhoneList;
		std::list<CDeviceVir>	m_DeviceVirList;
		std::list<CACDGroup>	m_ACDGroupList;
		
		std::list<std::string>	m_DefaultReadyAgentList;	// 默认置闲的坐席列表

	private:
		std::list<CDialRule>	m_DialRuleList;
		std::list<CDialRule>	m_CheckNumList;

	private:
		void OnLoadConfigSuccessNotify(ObserverPattern::INotificationPtr p_pNotifiRequest);

		bool _ExistDeviceNumber(const std::string& strNumber);
		void _AddDeviceNumber(const std::string& strNumber);
		void _LoadMergeHuntGrpConver();		
		
		void _LoadCheckNumList();
		void _LoadDialRuleList();

	private:
		std::mutex m_mutexLoadCheckNumList;
		std::mutex m_mutexLoadDialRuleList;


		std::map<std::string, std::string> m_mapDevices;

		static boost::shared_ptr<CSysConfig> m_pInstance;

		bool						m_bParamterIsLoad;
		Log::ILogPtr				m_pLog;
		Config::IConfigPtr			m_ConfigPtr;
		StringUtil::IStringUtilPtr	m_StringUtilPtr;
		Redis::IRedisClientPtr		m_pRedisClient;

		ObserverPattern::IObserverCenterPtr	m_pObserverCenter;

		// FSAES Server地址(主)
		std::string m_strAEServerIP;
		// FSAES Server地址(从)
		std::string m_strSlaveAEServerIP;

		// FSAES Server端口
		std::string m_strAEServerPort;
		// CTI UserName
		std::string m_strAESUserName;
		// CTI Password
		std::string m_strAESPsw;
		std::string m_strAESNodeName;
		//启用从配置中读取的号码列表中判断是否被叫是外线号码
		std::string m_strEnbaleConfigCalloutCheck;


		// CTI Server 与 TSAPI Server 的心跳间隔, 单位:秒
		int		m_nHeartbeatTime;
		// CTI 执行命令超时时间设置，超过该时间则认为异常, 单位:秒
		int		m_nSwitchCmdTimeOut;
		// 检查异常呼叫, 超过该时间则认为是异常，残留在CallManage中, 单位:分钟
		int		m_nCallInvalidTime;
		//  设备监视时间间隔，单位：秒
		int		m_nMonitorTimeSpan;		
		//	设备监视次数
		int		m_nMaxMonitorCount;

		int m_nWaitAnswerMaxTime;

		int m_nMinLocalPhoneLen;		//	本地号码最小长度
		int m_nMaxLocalPhoneLen;		//	本地号码最大长度
		int m_nMinZoneLen;				//	区号最小长度
		int m_nMaxZoneLen;				//	区号最大长度
		int m_nMobilePreLen;			//	手机号码前缀

		int m_nAddrZeroPreLen;

		//	出局号
		std::string m_strOutCallHead;
		/*std::string m_strLocalPhoneCallHead;
		std::string m_strRemotePhoneCallHead;
		std::string m_strLocalMobileCallHead;
		std::string m_strRemoteMobileCallHead;
		std::string m_strProvince;*/
		//	入局号
		std::string m_strInCallHead;
		// 拨号规则
	//	std::string m_strDialRuleList;
		
		// PDTo
		std::string m_strWirelessCallHead;
		// Rule 判断无线号码的正则表达式
		std::string m_strTrunkRule;
		// 最小在席数
		int		m_nMinOnlineAgentNum;
		// 轮呼时间间隔
		int		m_nPollingCallTime;
		// 坐席状态扫描时间间隔
		int		m_nAgentStateTime;
		// 话机物理按键监控坐席
		std::string m_strMonitorSeatList;
		//	黑名单路由地址
		std::string m_strRouteDest;

		bool m_bCTITestFlag;
		int m_nCTITestPort;

		std::map<std::string, std::string> m_mapMergeHuntGrpConvers;

		
	};
}	// end namespace
