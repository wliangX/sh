#include "Boost.h"
#include "DateTimeFactoryImpl.h"
#include "DateTimeImpl.h"

ICCFactoryCppInitialize(CDateTimeFactoryImpl)

IDateTimePtr CDateTimeFactoryImpl::CreateDateTime()
{
	return boost::make_shared<CDateTimeImpl>();
}
