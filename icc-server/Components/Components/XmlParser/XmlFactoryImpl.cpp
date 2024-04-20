#include "Boost.h"
#include "XmlFactoryImpl.h"
#include "XmlImpl.h"

ICCFactoryCppInitialize(CXmlFactoryImpl)

IXmlPtr CXmlFactoryImpl::CreateXml()
{
	return boost::make_shared<CXmlImpl>();
}
