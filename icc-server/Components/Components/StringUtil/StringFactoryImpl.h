#pragma once

#include "StringUtil/IStringFactory.h"

namespace ICC
{
	namespace StringUtil
	{
		class CStringFactoryImpl :
			public IStringFactory
		{
			ICCFactoryHeaderInitialize(CStringFactoryImpl)
		public:
			virtual IStringUtilPtr CreateString();
		};
	}
}