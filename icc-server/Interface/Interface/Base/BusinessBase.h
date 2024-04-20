#pragma once
#include <IBusiness.h>
#include <CommonlDef.h>
#include <Observer/IObserverFactory.h>

namespace ICC
{
	class CBusinessBase :
		public IBusiness
	{
	private:
		virtual void Init(IResourceManagerPtr p_pResourceManager)
		{
			m_pResourceManager = p_pResourceManager;
			OnInit();
		}
		virtual void Start()
		{
			OnStart();
		}
		virtual void Stop()
		{
			OnStop();
		}
		virtual void Destroy()
		{
			OnDestroy();
			m_AbstractObserverList.clear();
		}

	public:
		virtual IResourceManagerPtr GetResourceManager()
		{
			return m_pResourceManager;
		}
		virtual void OnInit()
		{
		}
		virtual void OnStart()
		{
		}
		virtual void OnStop()
		{
		}
		virtual void OnDestroy()
		{
		}
	public:
		std::list<ObserverPattern::IAbstractObserverPtr> m_AbstractObserverList;
	private:
		IResourceManagerPtr m_pResourceManager;
	};
}