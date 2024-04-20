#pragma once
#include <IPlugin.h>

namespace ICC
{
	class CPluginBase :
		public IPlugin
	{
	private:
		virtual void Start(IResourceManagerPtr p_pResourceManager)
		{
			m_pResourceManager = p_pResourceManager;
			OnStart();
		}
		virtual void Stop()
		{
			OnStop();
		}

	public:
		virtual unsigned int GetPriorityLevel()
		{
			return 0;
		}
		virtual void OnStart()
		{
		}
		virtual void OnStop()
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