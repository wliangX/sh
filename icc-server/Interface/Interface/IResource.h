#pragma once
#include <IObject.h>

namespace ICC
{
	class IResource :
		public IObject
	{
	public:
	};

	typedef boost::shared_ptr<IResource> IResourcePtr;
}
