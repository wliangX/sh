#pragma once

namespace ICC
{
	namespace DataBase
	{
		class CSqlBuilderFactoryImpl :
			public ISqlBuilderFactory
		{
			ICCFactoryHeaderInitialize(CSqlBuilderFactoryImpl)

		public:
			virtual ISqlBuilderPtr CreateSqlBuilder();
		};
	}
}