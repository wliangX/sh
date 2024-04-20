#include "Boost.h"
#include "StringFactoryImpl.h"
#include "StringUtilImpl.h"

ICCFactoryCppInitialize(CStringFactoryImpl)

StringUtil::IStringUtilPtr CStringFactoryImpl::CreateString()
{
	return boost::make_shared<CStringImpl>();
}
