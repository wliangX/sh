#pragma once
#include <PGClient/IPGClientFactory.h>

namespace ICC
{
	namespace DataBase
	{
		class CPGClientExFactoryImpl :
			public IPGClientFactory
		{
			ICCFactoryHeaderInitialize(CPGClientExFactoryImpl);
		public:
			virtual IPGClientPtr CreatePGClient();
		};
	}
}