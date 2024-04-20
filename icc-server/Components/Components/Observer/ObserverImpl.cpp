#include "Boost.h"

//Project
#include "ObserverImpl.h"



CObserverCenterImpl::CObserverCenterImpl(IResourceManagerPtr p_ResourceManagerPtr)
	: m_pThread(nullptr),
	m_Permanence(m_IoService),
	m_pResourceManager(p_ResourceManagerPtr)	
	
	/*m_pThread(nullptr),
	m_Permanence(nullptr),
	m_IoService(nullptr),
	m_pResourceManager(p_ResourceManagerPtr),
	m_usThreadCount(4),
	m_bSyncThread(false)*/
{
	//m_lock = ICCGetILockFactory()->CreateLock(Lock::TypeRecursiveMutex);
	m_pJsonFty = ICCGetIJsonFactory();
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pLogPtr = ICCGetILogFactory()->GetLogger(MODULE_NAME);

	m_usThreadCount = m_pString->ToInt(m_pConfig->GetValue("ICC/Component/Observer/ThreadCount", "4"));//线程数
	if (m_usThreadCount <= 0)
	{
		m_usThreadCount = 1;
	}

	//限制每个消息中心分发线程为1个，多线程处理移到具体业务插件
	m_usThreadCount = 1;

	m_pThread = boost::make_shared<boost::thread>([this]()
		{
			this->m_IoService.run();
		});

	/*m_pThread = new ThreadPtr[m_usThreadCount];
	m_IoService = new AsioEngine[m_usThreadCount];

	m_Permanence = new boost::asio::io_service::work *[m_usThreadCount];

	for (unsigned short i = 0; i < m_usThreadCount; ++i)
	{
		m_Permanence[i] = new boost::asio::io_service::work(m_IoService[i]);

		m_pThread[i] = boost::make_shared<boost::thread>([this, i]()
			{
				this->m_IoService[i].run();
			});
	}*/
}

CObserverCenterImpl::~CObserverCenterImpl()
{
	m_IoService.stop();

	if (m_pThread)
	{
		m_pThread->join();
	}

	//if (NULL != m_IoService)
	//{
	//	for (unsigned short i = 0; i < m_usThreadCount; ++i)
	//	{
	//		m_IoService[i].stop();
	//	}
	//}
	//

	//if (NULL != m_pThread)
	//{
	//	for (unsigned short i = 0; i < m_usThreadCount; ++i)
	//	{
	//		if (m_pThread[i])
	//		{
	//			m_pThread[i]->join();
	//		}
	//	}

	//	delete[]m_pThread;
	//	m_pThread = NULL;
	//}

	////析构
	//if (NULL != m_Permanence)
	//{
	//	for (unsigned short i = 0; i < m_usThreadCount; ++i)
	//	{
	//		if (NULL != m_Permanence[i])
	//		{
	//			delete m_Permanence[i];
	//			m_Permanence[i] = NULL;
	//		}
	//		
	//	}
	//	delete []m_Permanence;
	//	m_Permanence = NULL;
	//}

	//if (NULL != m_IoService)
	//{
	//	delete[]m_IoService;
	//	m_IoService = NULL;
	//}
	
}

void CObserverCenterImpl::NotifyPrivate(INotificationPtr p_NtfPtr, bool bIsToMq /* = false */, ObserverPattern::ENotifyType p_eNotifyType /* = ObserverPattern::ENotifyType::Post */)
{
	if (p_NtfPtr == nullptr)
	{
		ICC_LOG_ERROR(m_pLogPtr, "CObserverCenterImpl::Notify notify is null!!!");
		return;
	}

	if (bIsToMq)
	{
		p_NtfPtr->SetBehavior(ObserverPattern::EBehavior::Send);
		Notify(p_NtfPtr, p_eNotifyType);
	}
	else
	{
		p_NtfPtr->SetBehavior(ObserverPattern::EBehavior::Received);
		if (m_pObserverCenterList.size() == 0)
		{
			m_pObserverCenterList = ICCGetIObserverFactory()->GetObserverCenterList();
			ICC_LOG_ERROR(m_pLogPtr, "CObserverCenterImpl::Notify observercenter size : %d", m_pObserverCenterList.size());
		}

		for (auto l_observer : m_pObserverCenterList)
		{
			l_observer->Notify(p_NtfPtr, p_eNotifyType);
		}
	}	
}

//通知观察者  
void CObserverCenterImpl::Notify(INotificationPtr p_NtfPtr, ObserverPattern::ENotifyType p_eNotifyType /* = ObserverPattern::ENotifyType::Post */)
{
	if (p_NtfPtr == nullptr)
	{
		ICC_LOG_ERROR(m_pLogPtr, "CObserverCenterImpl::Notify notify is null!!!");
		return;
	}	

	//ICC_LOG_LOWDEBUG(m_pLogPtr, "CObserverCenterImpl::Notify begin: %s", p_NtfPtr->GetCmdGuid().empty()?p_NtfPtr->GetRequestGuid().c_str(): p_NtfPtr->GetCmdGuid().c_str());

	std::string strCmdName = p_NtfPtr->GetCmdName();
	if (strCmdName.empty())
	{
		PROTOCOL::CHeader l_oHeader;
		if (!l_oHeader.ParseString(p_NtfPtr->GetMessages(), m_pJsonFty->CreateJson()))
		{
			return;
		}

		strCmdName = l_oHeader.m_strCmd;
		p_NtfPtr->SetCmdName(strCmdName);
		p_NtfPtr->SetCmdGuid(l_oHeader.m_strMsgid);

		ICC_LOG_LOWDEBUG(m_pLogPtr, "CObserverCenterImpl::Notify cmd null process: %s:%s", p_NtfPtr->GetCmdName().c_str(), p_NtfPtr->GetCmdGuid().c_str());
	}


	if (p_eNotifyType == ObserverPattern::ENotifyType::Post)
	{
		m_IoService.post([this, p_NtfPtr]()
		{
			Notifyimpl(p_NtfPtr);
		});
	}
	else
	{
		m_IoService.dispatch([this, p_NtfPtr]()
		{
			this->Notifyimpl(p_NtfPtr);
		});
	}


	/*unsigned short tmp_usIndex = GetIndex(p_eNotifyType);
	if (p_eNotifyType == ObserverPattern::ENotifyType::Post)
	{
		m_IoService[tmp_usIndex].post([this, p_NtfPtr]()
		{
			Notifyimpl(p_NtfPtr);
		});
	}
	else
	{
		m_IoService[tmp_usIndex].dispatch([this, p_NtfPtr]()
		{
			this->Notifyimpl(p_NtfPtr);
		});
	}*/

	//ICC_LOG_LOWDEBUG(m_pLogPtr, "CObserverCenterImpl::Notify end: %s", p_NtfPtr->GetCmdGuid().c_str());
}

//注册观察者  
void CObserverCenterImpl::AddObserver(IAbstractObserverPtr p_AbstractObserver)
{
	//Lock::AutoLock l_lock(m_lock);
	boost::lock_guard<boost::mutex> l_lock(m_lock);
	//m_Observers.push_back(p_AbstractObserver);
	IObserversMap::iterator itr;
	itr = m_Observers.find(p_AbstractObserver->GetCmdName());
	if (itr != m_Observers.end())
	{
		itr->second.push_back(p_AbstractObserver);
	}
	else
	{
		IObserverListEx ls;
		ls.push_back(p_AbstractObserver);
		m_Observers.insert(std::make_pair(p_AbstractObserver->GetCmdName(), ls));
	}
}

//取消注册观察者  
void CObserverCenterImpl::RemoveObserver(IAbstractObserverPtr p_AbstractObserver)
{
	//Lock::AutoLock l_lock(m_lock);
	boost::lock_guard<boost::mutex> l_lock(m_lock);
	IObserversMap::iterator itr;
	itr = m_Observers.find(p_AbstractObserver->GetCmdName());
	if (itr != m_Observers.end())
	{
		IObserverListEx::iterator itrRemove;
		for (itrRemove = itr->second.begin(); itrRemove != itr->second.end(); ++itrRemove)
		{
			if ((*itrRemove)->Equals(p_AbstractObserver))
			{
				itr->second.erase(itrRemove);
				break;
			}
		}
	}	
}

//获取观察者列表
IObserverList CObserverCenterImpl::GetObservers()
{
	//Lock::AutoLock l_lock(m_lock);
	//boost::lock_guard<boost::mutex> l_lock(m_lock);
	//return m_Observers;
	IObserverList ls;
	return ls;
}

unsigned short CObserverCenterImpl::GetIndex(ObserverPattern::ENotifyType p_eNotifyType)
{
	return 0;
	
	/*if (p_eNotifyType == ObserverPattern::ENotifyType::PostSync)
	{
		m_bSyncThread = true;
		return 0;
	}

	unsigned short l_usIndex = 0;
	{
		SAFE_LOCK(m_lkIndex);
		l_usIndex = (++m_usIndex) % m_usThreadCount;

		if (l_usIndex == 0 && m_bSyncThread)
		{
			l_usIndex++;
		}
	}
	
	return l_usIndex;*/

}

bool CObserverCenterImpl::GetObserversByCmdName(const std::string& strCmdName, IObserverListEx& lsObservers)
{
	boost::lock_guard<boost::mutex> l_lock(m_lock);
	IObserversMap::iterator itr;
	itr = m_Observers.find(strCmdName);
	if (itr != m_Observers.end())
	{
		lsObservers = itr->second;
		return true;
	}

	return false;
}

void CObserverCenterImpl::Notifyimpl(INotificationPtr p_NtfPtr)
{
	//ICC_LOG_LOWDEBUG(m_pLogPtr, "CObserverCenterImpl::Notifyimpl begin: %s", p_NtfPtr->GetCmdGuid().c_str());

	IObserverListEx l_observers;
	if (p_NtfPtr->GetBehavior() == EBehavior::Send)
	{
		ObserverPattern::IObserverCenterPtr l_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(BUSINESS_MESSAGE);		

		if (NULL != l_pObserverCenter)
		{
			if (!l_pObserverCenter->GetObserversByCmdName(NOTIFI_SEND_REQUEST, l_observers))
			{
				ICC_LOG_WARNING(m_pLogPtr, "CObserverCenterImpl::Notifyimpl not find observer: NOTIFI_SEND_REQUEST:%s", p_NtfPtr->GetCmdGuid().c_str());
				return;
			}
		}

		for (auto var : l_observers)
		{
			var->NotifyObserver(p_NtfPtr);		
		}
	}
	else
	{
		if (!GetObserversByCmdName(p_NtfPtr->GetCmdName(), l_observers))
		{
			//ICC_LOG_LOWDEBUG(m_pLogPtr, "CObserverCenterImpl::Notifyimpl not find observer: %s:%s",p_NtfPtr->GetCmdName().c_str(), p_NtfPtr->GetCmdGuid().c_str());
			return;
		}

		ICC_LOG_LOWDEBUG(m_pLogPtr, "CObserverCenterImpl::Notifyimpl find observer: %s:%s:%s:%d", m_strObserverName.c_str(), p_NtfPtr->GetCmdName().c_str(), p_NtfPtr->GetCmdGuid().c_str(), (int)l_observers.size());
		
		for (auto var : l_observers)
		{
			var->NotifyObserver(p_NtfPtr);
		}
	}

	//ICC_LOG_LOWDEBUG(m_pLogPtr, "CObserverCenterImpl::Notifyimpl end: %s", p_NtfPtr->GetCmdGuid().c_str());
}

IResourceManagerPtr CObserverCenterImpl::GetResourceManager()
{
	return m_pResourceManager;
}