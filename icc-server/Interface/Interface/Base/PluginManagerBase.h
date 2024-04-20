#pragma once
#include <IPluginManager.h>

namespace ICC
{
	class CPluginManagerBase :
		public IPluginManager
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