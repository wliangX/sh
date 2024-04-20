#pragma once
#include <IObject.h>

namespace ICC
{
	class IService :
		public IObject
	{
	public:
		virtual void Run() = 0;
	};

	typedef boost::shared_ptr<IService> IServicePtr;
}
