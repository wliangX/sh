#include "Boost.h"
#include "ResultSetImpl.h"
#include "ResultSetFactoryImpl.h"

ICCFactoryCppInitialize(CResultSetFactoryImpl)

IResultSetPtr CResultSetFactoryImpl::CreateResultSet()
{
	return boost::make_shared<CResultSetImpl>();
}
