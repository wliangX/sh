#include "Boost.h"
#include "FactoryImpl.h"
#include "RecordPluginImpl.h"

IObjectPtr CFactoryImpl::OnCreateObject()
{
	return boost::make_shared<RecordPluginImpl>();
}
