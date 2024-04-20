#include "Boost.h"
#include "FactoryImpl.h"
#include "ComponentImpl.h"

namespace ICC 
{

	IObjectPtr CFactoryImpl::OnCreateObject()
	{
		return boost::make_shared<CComponentImpl>();
	}

} /*namespace ICC*/
