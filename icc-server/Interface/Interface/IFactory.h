#pragma once
#include <IObject.h>

namespace ICC
{
	class IFactory :
		public IObject
	{
	public:
		virtual IObjectPtr CreateObject() = 0;
	};

	typedef boost::shared_ptr<IFactory> IFactoryPtr;
}
