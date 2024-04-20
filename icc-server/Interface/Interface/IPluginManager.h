#pragma once
#include <IManager.h>
#include <IResourceManager.h>

namespace ICC
{
	class IPluginManager :
		public IManager
	{
	public:
		virtual void Start(IResourceManagerPtr) = 0;
		virtual void Stop() = 0;
	};

	typedef boost::shared_ptr<IPluginManager> IPluginManagerPtr;
}
