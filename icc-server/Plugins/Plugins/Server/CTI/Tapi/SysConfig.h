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
		int GetConferenceRingTimeout() const { return m_nConferenceRingTimeout; }
		int GetTransferRingTimeout() const { return m_nTransferRingTimeout; }

		std::string GetDialRule() const { return m_strDialRule; }
		std::string GetOutCallHead() const { return m_strOutCallHead; }
		std::string GetWirelessCallHead() const { return m_strWirelessCallHead; }
		std::string GetTrunkRule() const { return m_strTrunkRule; }

		std::string GetRouteDest() const { return m_strRouteDest; }
		//	按路由规则获取原始号码
		std::string GetCheckNum(const std::string& p_strSrcNum);
		//	按号码前缀添加出局号
		bool GetOutCallNum(const std::string& p_strSrcNum, std::string& p_strDestNum);

		bool GetACDGrpConver(std::string& p_strDestACDGrp, const std::string& p_strSrcACDGrp);
		bool GetACDGrpTypeByDn(const std::string& p_strInDeviceNum, std::string& p_strOutDeviceType);
		bool GetDeviceTypeByDn(const std::string& p_strInDeviceNum, std::string& p_strOutDeviceType);

		bool FindDefaultReadyAgent(const std::string& p_strAgent);

	public:
		std::list<CDevicePhone>	m_DevicePhoneList;	// 话机列表
		std::list<CDeviceVir>	m_DeviceVirList;
		std::list<CACDGroup>	m_ACDGroupList;		// ACD 组成员列表
		std::list<CDialRule>	m_DialRuleList;		// 呼叫规则
		std::list<std::string>	m_DefaultReadyAgentList;	// 默认置闲的坐席列表

	private:
		static boost::shared_ptr<CSysConfig> m_pInstance;

		bool						m_bParamterIsLoad;
		Log::ILogPtr				m_pLog;
		Config::IConfigPtr			m_pConfig;
		StringUtil::IStringUtilPtr	m_pStringUtil;
		Redis::IRedisClientPtr		m_pRedisClient;

		//	Redis 服务参数
		/*std::string m_strRedisAddr;
		bool l_bRedisIsCluster;
		int l_nRedisTTL;*/
		bool m_bUseAnalogueTrunk;

		// CTI Server 与 TSAPI Server 的心跳间隔, 单位:秒
		int		m_nHeartbeatTime;
		// CTI 执行命令超时时间设置，超过该时间则认为异常, 单位:秒
		int		m_nSwitchCmdTimeOut;
		// 检查异常呼叫, 超过该时间则认为是异常，残留在CallManage中, 单位:分钟
		int		m_nCallInvalidTime;

		int m_nMinLocalPhoneLen;		//	本地号码最小长度
		int m_nMaxLocalPhoneLen;		//	本地号码最大长度
		int m_nMinZoneLen;				//	区号最小长度
		int m_nMaxZoneLen;				//	区号最大长度
		int m_nMobilePreLen;			//	手机号码前缀

		//	出局号
		std::string m_strOutCallHead;
		// 拨号规则
		std::string m_strDialRule;
		
		// PDT前缀
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

		int									m_nTransferRingTimeout;			//	振铃超时时长（单位：秒）
		int									m_nConferenceRingTimeout;		//	振铃超时时长（单位：秒）
		std::map<std::string, std::string>	m_mapACDGrpConversion;			//	ACD 转换列表
	};
}	// end namespace
