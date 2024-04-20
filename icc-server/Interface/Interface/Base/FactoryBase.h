#pragma once
#include <IFactory.h>

namespace ICC
{
	class CFactoryBase :
		public IFactory
	{
	private:
		virtual IObjectPtr CreateObject()
		{
			return OnCreateObject();
		}

	public:
		virtual IObjectPtr OnCreateObject()
		{
			return IObjectPtr();
		}
	};
}
