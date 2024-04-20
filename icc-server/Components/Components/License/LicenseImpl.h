#pragma once

namespace ICC
{
	namespace License
	{
		struct LicenseInfo
		{
			std::string m_strEnableTimeLimit;		//�Ƿ���������ʱ�����
			std::string m_strEnableSuperdogCode;	//�Ƿ����ó���������
			std::string m_strNodeDesp;				//ϵͳ����
			std::string m_strNodeName;				//ϵͳ��
			std::string m_strSuperdogCode;			//��������
			std::string m_strMachineCode;			//������
			std::string m_strGenDate;				//License key��������
			std::string m_strDefaultNum;			//�ͻ�����Ȩ����
			std::string m_strValidDate;				//��Ч����
			std::string m_strVenderCode;			//���̴���
			std::string m_strEnableMachineCode;		//�Ƿ����û��������
			std::string m_strLicenseDesp;			//LicenseDesp
			std::string m_strValidDays;				//��ֹ����
			std::string m_strStatus;				//License״̬
		};

		class CLicenseImpl :
			public ILicense
		{
		public:
			CLicenseImpl(IResourceManagerPtr p_ResourceManagerPtr);
			virtual ~CLicenseImpl();
		public:

			//�Ƿ���������ʱ�����
			virtual bool IsEnableTimeLimit();
			//�Ƿ����ó���������
			virtual bool IsEnableSuperdogCode();
			//�Ƿ����û��������
			virtual bool IsEnableMachineCode();
			//��ȡϵͳ����
			virtual std::string GetSystemCode();
			//��ȡϵͳ����
			virtual std::string GetSystemName();
			//��ȡ����������
			virtual std::string GetSuperdogCode();
			//��ȡ������
			virtual std::string GetMachineCode();
			//��ȡLicense key��������
			virtual std::string GetGenDate();
			//��ȡ���̴���
			virtual std::string GetVenderCode();
			//��ȡ��ֹ����
			virtual std::string GetValidDate();
			//License״̬
			virtual std::string GetStatus();
			//��ȡ�ͻ�����Ȩ����
			virtual unsigned int GetClientNum();
			//��ȡ��Ч����
			virtual unsigned int GetValidDays();
			//License״̬
			virtual std::string GetLicenseDesp();

		public:
			virtual IResourceManagerPtr GetResourceManager();

		public:
			void OnResponse(std::string p_strMessage);

		private:
			bool ConnectMQ();
			void GetLicenseInfo();

			//��ʱ����Ӧ
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
			//�߳�ʵ�����boost���߳�
			boost::shared_ptr<boost::thread> m_pThread;
			//io���������¼���������
			boost::asio::io_service m_IoService;
			//�ö�����Ϊ�˱�֤io_service�ڲ�����stop�������Զ���˳���ȷ������������
			boost::asio::io_service::work m_Permanence;
			//��ʱ��
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