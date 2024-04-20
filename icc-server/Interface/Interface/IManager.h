#pragma once
#include <IObject.h>

namespace ICC
{
	class IManager :
		public IObject
	{
	public:
	};

	typedef boost::shared_ptr<IManager> IManagerPtr;
}
