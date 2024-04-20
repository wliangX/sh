#pragma once

namespace ICC
{
	namespace ObserverPattern
	{
		/*
		* class   �۲��߹���ʵ����
		* author  w16314
		* purpose
		* note
		*/
		class CObserverFactoryImpl : public IObserverFactory
		{
			ICCFactoryHeaderInitialize(CObserverFactoryImpl)
		public:
			virtual ObserverPattern::IObserverCenterPtr GetObserverCenter(std::string p_strObserverName);
			virtual IObserverCenterList GetObserverCenterList();
			virtual void DestroyObserverCenter(std::string p_strObserverName);
		private:
			Lock::ILockPtr m_lock;
			ObserverCenterMap m_ObserverCenterMap;
		};
	}
}