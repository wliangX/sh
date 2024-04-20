#pragma once
#include <PGClient/IPGClientFactory.h>

namespace ICC
{
	namespace DataBase
	{
		class CPGClientFactoryImpl :
			public IPGClientFactory
		{
			ICCFactoryHeaderInitialize(CPGClientFactoryImpl);
		public:
			virtual IPGClientPtr CreatePGClient();
		};
	}
}