#pragma once
#include <PGClient/IResultSetFactory.h>

namespace ICC
{
	namespace DataBase
	{
		class CResultSetFactoryImpl :
			public IResultSetFactory
		{
			ICCFactoryHeaderInitialize(CResultSetFactoryImpl);
		public:
			virtual IResultSetPtr CreateResultSet();
		};
	}
}