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
		* class   �۲�������ʵ��
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
			//֪ͨ�۲���  
			virtual void Notify(INotificationPtr p_NtfPtr, ObserverPattern::ENotifyType p_eNotifyType = ObserverPattern::ENotifyType::Post);
			virtual void NotifyPrivate(INotificationPtr p_NtfPtr, bool bIsToMq = false, ObserverPattern::ENotifyType p_eNotifyType = ObserverPattern::ENotifyType::Post);
			//ע��۲���  
			virtual void AddObserver(IAbstractObserverPtr p_AbstractObserver);
			//ȡ��ע��۲���  
			virtual void RemoveObserver(IAbstractObserverPtr p_AbstractObserver);
			//��ȡ�۲����б�
			virtual IObserverList GetObservers();

			bool GetObserversByCmdName(const std::string& strCmdName, IObserverListEx& lsObservers);
			void SetObserverName(const std::string& strObserverName) { m_strObserverName = strObserverName; }

		private:
			virtual IResourceManagerPtr GetResourceManager();

		private:
			void Notifyimpl(INotificationPtr p_NtfPtr);

			//��ȡ�߳�����
			unsigned short GetIndex(ObserverPattern::ENotifyType);			

		private:
			////�̳߳�ʵ�����boost���߳�
			//ThreadPtr *m_pThread;
			////io���������¼���������
			//AsioEngine *m_IoService;
			////�ö�����Ϊ�˱�֤io_service�ڲ�����stop�������Զ���˳�
			//boost::asio::io_service::work **m_Permanence;

			//�߳�ʵ�����boost���߳�
			ThreadPtr m_pThread;
			//io���������¼���������
			AsioEngine m_IoService;
			//�ö�����Ϊ�˱�֤io_service�ڲ�����stop�������Զ���˳�
			boost::asio::io_service::work m_Permanence;

			//�̳߳���
			unsigned short m_usThreadCount;

			//�ַ�ת��ʵ��
			StringUtil::IStringUtilPtr  m_pString;
			//���ù���ʵ��
			Config::IConfigPtr		m_pConfig;

			unsigned short m_usIndex;

			Log::ILogPtr m_pLogPtr;

		private:
			IObserversMap m_Observers;
			//Lock::ILockPtr m_lock;
			boost::mutex m_lock;
			JsonParser::IJsonFactoryPtr	m_pJsonFty;
			IResourceManagerPtr m_pResourceManager;

			//std::recursive_mutex m_lkIndex;  //��������
			boost::mutex m_lkIndex;
			bool m_bSyncThread;
			std::string m_strObserverName;

			ObserverPattern::IObserverCenterList m_pObserverCenterList;
		};
	}
}