#pragma once

namespace ICC
{
	namespace JsonParser
	{
		class CJsonFactoryImpl :
			public IJsonFactory
		{
			ICCFactoryHeaderInitialize(CJsonFactoryImpl)
		public:
			virtual IJsonPtr CreateJson();
		};
	}
}

