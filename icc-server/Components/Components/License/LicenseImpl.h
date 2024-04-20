#pragma once

namespace ICC
{
	namespace License
	{
		struct LicenseInfo
		{
			std::string m_strEnableTimeLimit;		//是否启用限制时间控制
			std::string m_strEnableSuperdogCode;	//是否启用超级狗控制
			std::string m_strNodeDesp;				//系统编码
			std::string m_strNodeName;				//系统名
			std::string m_strSuperdogCode;			//超级狗码
			std::string m_strMachineCode;			//机器码
			std::string m_strGenDate;				//License key生成日期
			std::string m_strDefaultNum;			//客户端授权数量
			std::string m_strValidDate;				//有效天数
			std::string m_strVenderCode;			//厂商代码
			std::string m_strEnableMachineCode;		//是否启用机器码控制
			std::string m_strLicenseDesp;			//LicenseDesp
			std::string m_strValidDays;				//截止日期
			std::string m_strStatus;				//License状态
		};

		class CLicenseImpl :
			public ILicense
		{
		public:
			CLicenseImpl(IResourceManagerPtr p_ResourceManagerPtr);
			virtual ~CLicenseImpl();
		public:

			//是否启用限制时间控制
			virtual bool IsEnableTimeLimit();
			//是否启用超级狗控制
			virtual bool IsEnableSuperdogCode();
			//是否启用机器码控制
			virtual bool IsEnableMachineCode();
			//获取系统编码
			virtual std::string GetSystemCode();
			//获取系统名称
			virtual std::string GetSystemName();
			//获取超级狗编码
			virtual std::string GetSuperdogCode();
			//获取机器码
			virtual std::string GetMachineCode();
			//获取License key生成日期
			virtual std::string GetGenDate();
			//获取厂商代码
			virtual std::string GetVenderCode();
			//获取截止日期
			virtual std::string GetValidDate();
			//License状态
			virtual std::string GetStatus();
			//获取客户端授权数量
			virtual unsigned int GetClientNum();
			//获取有效天数
			virtual unsigned int GetValidDays();
			//License状态
			virtual std::string GetLicenseDesp();

		public:
			virtual IResourceManagerPtr GetResourceManager();

		public:
			void OnResponse(std::string p_strMessage);

		private:
			bool ConnectMQ();
			void GetLicenseInfo();

			//定时器响应
			void OnTimer(std::string p_strTimerName);
		private:
			IResourceManagerPtr m_pResourceManager;
			StringUtil::IStringUtilPtr m_pString;
			DateTime::IDateTimePtr m_pDateTime;
			JsonParser::IJsonFactoryPtr m_pJsonFty;
			Config::IConfigPtr m_pConfig;
			Config::IConfigPtr m_pStaticConfig;
			Log::ILogPtr m_pLog;
			IConsumerPtr m_pConsumer;
			IProducerPtr m_pProducer;
			HelpTool::IHelpToolPtr m_pHelpTool;
		private:
			std::string m_strbrokerURI;
			std::string m_strClientID;
			std::string m_strUserName;
			std::string m_strPassword;
			std::string m_strTopics;
			std::string m_strQueues;
			std::string m_strLicenseVersion;
		private:
			//线程实体采用boost的线程
			boost::shared_ptr<boost::thread> m_pThread;
			//io服务，用作事件驱动引擎
			boost::asio::io_service m_IoService;
			//该对象是为了保证io_service在不调用stop的情况永远不退出，确保其生命周期
			boost::asio::io_service::work m_Permanence;
			//定时器
			boost::asio::deadline_timer m_Timer;
			//
			boost::mutex m_Mutex;

		private:
			LicenseInfo m_licenseInfo;
		};


		class CResCallImpl : public IResCallback
		{
		public:
			CResCallImpl(CLicenseImpl* p_pLicense)
				: m_pLicense(p_pLicense){}
			~CResCallImpl(){}
		public:
			virtual void OnResponse(std::string l_strMessage);
		private:
			CLicenseImpl* m_pLicense;
		};
	}
}