#pragma once
#include <IObject.h>
#include <IResourceManager.h>

namespace ICC
{
	class IBusiness :
		public IObject 
	{
	public:
		virtual IResourceManagerPtr GetResourceManager() = 0;
		virtual void Init(IResourceManagerPtr) = 0;
		virtual void Start() = 0;
		virtual void Stop() = 0;
		virtual void Destroy() = 0;
	};

	typedef boost::shared_ptr<IBusiness> IBusinessPtr;
}