#include "Boost.h"
#include "FactoryImpl.h"
#include "ComponentImpl.h"

IObjectPtr CFactoryImpl::OnCreateObject()
{
	return boost::make_shared<CComponentImpl>();
}