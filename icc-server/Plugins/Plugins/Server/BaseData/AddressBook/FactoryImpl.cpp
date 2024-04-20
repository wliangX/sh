#include "Boost.h"
#include "FactoryImpl.h"
#include "PluginImpl.h"

IObjectPtr CFactoryImpl::OnCreateObject()
{
	return boost::make_shared<CPluginImpl>();
}
