#include "Boost.h"
#include "SqlRequestImpl.h"
#include "SqlRequestFactoryImpl.h"

ICCFactoryCppInitialize(CSqlRequestFactoryImpl)

ISqlRequestPtr CSqlRequestFactoryImpl::CreateSqlRequest()
{
	return boost::make_shared<CSqlRequest>();
}
