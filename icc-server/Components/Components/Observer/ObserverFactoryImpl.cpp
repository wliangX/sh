#include "Boost.h"
#include "ObserverFactoryImpl.h"
#include "ObserverImpl.h"

ICCFactoryCppInitialize(CObserverFactoryImpl)

ObserverPattern::IObserverCenterPtr CObserverFactoryImpl::GetObserverCenter(std::string p_strObserverName)
{
	Lock::ILockFactoryPtr l_pLockFty = boost::dynamic_pointer_cast<Lock::ILockFactory>(m_IResourceManagerPtr->GetResource(ICCILockFactoryResourceName));

	if (m_lock == nullptr)
	{
		m_lock = l_pLockFty->CreateLock(Lock::TypeMutex);
	}	

	Lock::AutoLock l_lock(m_lock);
	ObserverCenterMap::iterator it = m_ObserverCenterMap.find(p_strObserverName);
	if (it == m_ObserverCenterMap.end())
	{
		boost::shared_ptr<CObserverCenterImpl> pImpl = boost::make_shared<CObserverCenterImpl>(m_IResourceManagerPtr);
		pImpl->SetObserverName(p_strObserverName);
		m_ObserverCenterMap[p_strObserverName] = pImpl;
		
	}
	return m_ObserverCenterMap[p_strObserverName];
}

void CObserverFactoryImpl::DestroyObserverCenter(std::string p_strObserverName)
{
	Lock::AutoLock l_lock(m_lock);
	ObserverCenterMap::iterator it = m_ObserverCenterMap.find(p_strObserverName);
	if (it != m_ObserverCenterMap.end())
	{
		m_ObserverCenterMap.erase(it);
	}
}

IObserverCenterList CObserverFactoryImpl::GetObserverCenterList()
{
	IObserverCenterList l_tmpList;
	for (auto var : m_ObserverCenterMap)
	{
		l_tmpList.push_back(var.second);
	}
	return l_tmpList;
}