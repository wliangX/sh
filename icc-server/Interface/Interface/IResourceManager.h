#pragma once

#include <string>
#include <IManager.h>
#include <IResource.h>

namespace ICC
{
	class IResourceManager :
		public IManager
	{
	public:
		virtual void SetResource(std::string, IResourcePtr) = 0;
		virtual IResourcePtr GetResource(std::string) = 0;
	};

	typedef boost::shared_ptr<IResourceManager> IResourceManagerPtr;
}
