#pragma once

namespace ICC
{
	namespace DataBase
	{
		class CSqlRequestFactoryImpl :
			public ISqlRequestFactory
		{
			ICCFactoryHeaderInitialize(CSqlRequestFactoryImpl)
		public:
			virtual ISqlRequestPtr CreateSqlRequest();
		};
	}
}