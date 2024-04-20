#pragma once
#include <IObject.h>
#include <IResourceManager.h>

namespace ICC
{
	class IComponent :
		public IObject
	{
	public:
		virtual unsigned int GetPriorityLevel() = 0;
		virtual void Start(IResourceManagerPtr) = 0;
		virtual void Stop() = 0;
	};

	typedef boost::shared_ptr<IComponent> IComponentPtr;
}
