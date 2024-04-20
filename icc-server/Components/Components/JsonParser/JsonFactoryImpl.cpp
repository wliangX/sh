#include "Boost.h"
#include "JsonFactoryImpl.h"
#include "JsonImpl.h"

ICCFactoryCppInitialize(CJsonFactoryImpl)

IJsonPtr CJsonFactoryImpl::CreateJson()
{
	return boost::make_shared<CJsonImpl>(m_IResourceManagerPtr);
}
