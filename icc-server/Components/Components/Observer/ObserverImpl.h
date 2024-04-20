#pragma once
#include <Config/IConfigFactory.h>
#include <StringUtil/IStringFactory.h>
#include <Log/ILogFactory.h>
#include <Json/IJsonFactory.h>


//#define        SAFE_LOCK(lk) std::lock_guard<std::recursive_mutex> l_guard(lk)	
#define        SAFE_LOCK(lk) boost::lock_guard<boost::mutex> l_guard(lk)	

namespace ICC
{
	namespace ObserverPattern
	{
		typedef boost::shared_ptr<boost::thread> ThreadPtr;
		typedef boost::asio::io_service AsioEngine;
		typedef boost::shared_ptr<AsioEngine> AsioEnginePtr;
		/*
		* class   观察者中心实现
		* author  w16314
		* purpose
		* note
		*/
		class CObserverCenterImpl : public IObserverCenter
		{
		public:
			CObserverCenterImpl(IResourceManagerPtr p_ResourceManagerPtr);
			virtual ~CObserverCenterImpl();
		public:
			//通知观察者  
			virtual void Notify(INotificationPtr p_NtfPtr, ObserverPattern::ENotifyType p_eNotifyType = ObserverPattern::ENotifyType::Post);
			virtual void NotifyPrivate(INotificationPtr p_NtfPtr, bool bIsToMq = false, ObserverPattern::ENotifyType p_eNotifyType = ObserverPattern::ENotifyType::Post);
			//注册观察者  
			virtual void AddObserver(IAbstractObserverPtr p_AbstractObserver);
			//取消注册观察者  
			virtual void RemoveObserver(IAbstractObserverPtr p_AbstractObserver);
			//获取观察者列表
			virtual IObserverList GetObservers();

			bool GetObserversByCmdName(const std::string& strCmdName, IObserverListEx& lsObservers);
			void SetObserverName(const std::string& strObserverName) { m_strObserverName = strObserverName; }

		private:
			virtual IResourceManagerPtr GetResourceManager();

		private:
			void Notifyimpl(INotificationPtr p_NtfPtr);

			//获取线程索引
			unsigned short GetIndex(ObserverPattern::ENotifyType);			

		private:
			////线程池实体采用boost的线程
			//ThreadPtr *m_pThread;
			////io服务，用作事件驱动引擎
			//AsioEngine *m_IoService;
			////该对象是为了保证io_service在不调用stop的情况永远不退出
			//boost::asio::io_service::work **m_Permanence;

			//线程实体采用boost的线程
			ThreadPtr m_pThread;
			//io服务，用作事件驱动引擎
			AsioEngine m_IoService;
			//该对象是为了保证io_service在不调用stop的情况永远不退出
			boost::asio::io_service::work m_Permanence;

			//线程池数
			unsigned short m_usThreadCount;

			//字符转换实体
			StringUtil::IStringUtilPtr  m_pString;
			//配置工具实体
			Config::IConfigPtr		m_pConfig;

			unsigned short m_usIndex;

			Log::ILogPtr m_pLogPtr;

		private:
			IObserversMap m_Observers;
			//Lock::ILockPtr m_lock;
			boost::mutex m_lock;
			JsonParser::IJsonFactoryPtr	m_pJsonFty;
			IResourceManagerPtr m_pResourceManager;

			//std::recursive_mutex m_lkIndex;  //可重入锁
			boost::mutex m_lkIndex;
			bool m_bSyncThread;
			std::string m_strObserverName;

			ObserverPattern::IObserverCenterList m_pObserverCenterList;
		};
	}
}