#pragma once

namespace ICC
{
	namespace Xml
	{
		class CXmlFactoryImpl :
			public IXmlFactory
		{
			ICCFactoryHeaderInitialize(CXmlFactoryImpl)
		public:
			virtual IXmlPtr CreateXml();
		};
	}
}
