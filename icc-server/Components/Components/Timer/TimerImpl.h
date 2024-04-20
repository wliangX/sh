#pragma once

namespace ICC
{
	namespace Timer
	{
		typedef boost::shared_ptr<boost::thread> ThreadPtr;
		typedef boost::function<void(void)> TimeCallbackFunc;
		typedef boost::asio::io_service AsioEngine;
		typedef boost::shared_ptr<AsioEngine> AsioEnginePtr;
		/*
		* class   ��ʱ�����ƿ�
		* author  w16314
		* purpose
		* note
		*/
		class TimerController
		{
		public:
			TimerController(std::string p_strCmdName, std::string p_strTimerName, AsioEngine& p_IoService, unsigned int p_iInterval, unsigned int p_iDelayStartSec, IResourceManagerPtr p_ResourceManager);
			virtual ~TimerController();
		public:
			//��ʱ����Ӧ
			void OnTimer(std::string p_strTimerName);
			//��ȡ��ʱ������
			std::string GetName();
		private:
			void NotifyObserverList(ObserverPattern::INotificationPtr p_pNotifiRequest, ObserverPattern::ENotifyType NotifyType = ObserverPattern::ENotifyType::Post);
		private:
			virtual IResourceManagerPtr GetResourceManager();
		private:
			unsigned int m_iInterval;                //��ʱ�����ʱ��
			unsigned int m_iDelayStartSec;           //�ӳ�����ʱ��
			std::string m_strTimerName;				 //��ʱ������
			std::string m_strCmdName;				 //��ʱ��������
			boost::asio::deadline_timer m_Timer;     //asio��ʱ��
			ObserverPattern::IObserverCenterPtr m_pObserverCenter;
			ObserverPattern::IObserverCenterList m_pObserverCenterList;
			StringUtil::IStringUtilPtr m_pString;
			IResourceManagerPtr m_pResourceManager;
		};

		typedef boost::shared_ptr<TimerController> TimerControllerPtr;

		/*
		* class   ��ʱ��������ʵ����
		* author  w16314
		* purpose
		* note
		*/
		class CTimerManagerImpl : public ITimerManager
		{
		public:
			CTimerManagerImpl(IResourceManagerPtr p_pResourceManager);
			virtual ~CTimerManagerImpl();
		public:
			//��Ӷ�ʱ��
			virtual std::string AddTimer(std::string p_strCmdName, unsigned int p_iInterval, unsigned int p_iDelayStartSec);
		private:
			virtual IResourceManagerPtr GetResourceManager();
		private:
			//������ʱ�����ƿ�
			bool Start();
			//ֹͣ��ʱ�����ƿ�
			void Stop();

			//��ʱ���
			void TimeOut(ObserverPattern::INotificationPtr p_pNotifiRequest);
			//
			void NotifyObserverList(ObserverPattern::INotificationPtr p_pNotifiRequest, ObserverPattern::ENotifyType NotifyType = ObserverPattern::ENotifyType::Post);

		private:
			//�̵߳Ķ�ʱ��, �첽timer		 
			boost::mutex m_TimerMapLock;
			std::map<std::string, TimerControllerPtr> m_TimerMap;
		private:
			//�߳�����
			std::string m_strThreadName;
			//���б�־
			volatile bool m_bIsRunning;
			//�߳�ʵ�����boost���߳�
			ThreadPtr m_pThread;
			//io���������¼���������
			AsioEngine m_IoService;
			//�ö�����Ϊ�˱�֤io_service�ڲ�����stop�������Զ���˳���ȷ������������
			boost::asio::io_service::work m_Permanence;

		private:
			boost::mutex m_Mutex;
			boost::condition m_Conditon;
			bool m_IsStarted;
			ObserverPattern::IObserverCenterPtr m_pObserverCenter;
			IResourceManagerPtr m_pResourceManager;
		};
	}
}