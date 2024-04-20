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
		* class   定时器控制块
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
			//定时器响应
			void OnTimer(std::string p_strTimerName);
			//获取定时器名称
			std::string GetName();
		private:
			void NotifyObserverList(ObserverPattern::INotificationPtr p_pNotifiRequest, ObserverPattern::ENotifyType NotifyType = ObserverPattern::ENotifyType::Post);
		private:
			virtual IResourceManagerPtr GetResourceManager();
		private:
			unsigned int m_iInterval;                //定时器间隔时间
			unsigned int m_iDelayStartSec;           //延迟启动时间
			std::string m_strTimerName;				 //定时器名称
			std::string m_strCmdName;				 //定时器命令名
			boost::asio::deadline_timer m_Timer;     //asio定时器
			ObserverPattern::IObserverCenterPtr m_pObserverCenter;
			ObserverPattern::IObserverCenterList m_pObserverCenterList;
			StringUtil::IStringUtilPtr m_pString;
			IResourceManagerPtr m_pResourceManager;
		};

		typedef boost::shared_ptr<TimerController> TimerControllerPtr;

		/*
		* class   定时器控制器实现类
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
			//添加定时器
			virtual std::string AddTimer(std::string p_strCmdName, unsigned int p_iInterval, unsigned int p_iDelayStartSec);
		private:
			virtual IResourceManagerPtr GetResourceManager();
		private:
			//启动定时器控制块
			bool Start();
			//停止定时器控制块
			void Stop();

			//超时检测
			void TimeOut(ObserverPattern::INotificationPtr p_pNotifiRequest);
			//
			void NotifyObserverList(ObserverPattern::INotificationPtr p_pNotifiRequest, ObserverPattern::ENotifyType NotifyType = ObserverPattern::ENotifyType::Post);

		private:
			//线程的定时器, 异步timer		 
			boost::mutex m_TimerMapLock;
			std::map<std::string, TimerControllerPtr> m_TimerMap;
		private:
			//线程名称
			std::string m_strThreadName;
			//运行标志
			volatile bool m_bIsRunning;
			//线程实体采用boost的线程
			ThreadPtr m_pThread;
			//io服务，用作事件驱动引擎
			AsioEngine m_IoService;
			//该对象是为了保证io_service在不调用stop的情况永远不退出，确保其生命周期
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