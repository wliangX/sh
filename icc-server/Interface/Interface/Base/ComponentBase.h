#pragma once
#include <IComponent.h>

namespace ICC
{
	class CComponentBase :
		public IComponent
	{
	private:
		virtual void Start(IResourceManagerPtr p_pResourceManager)
		{
			m_pResourceManager = p_pResourceManager;
			OnInit();
			OnStart();
		}
		virtual void Stop()
		{
			OnStop();
			OnDestroy();
		}

	public:
		virtual unsigned int GetPriorityLevel()
		{
			return 0;
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
		virtual IResourceManagerPtr GetResourceManager()
		{
			return m_pResourceManager;
		}

	private:
		IResourceManagerPtr m_pResourceManager;
	};
}
