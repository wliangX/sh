#include "Boost.h"
#include "SqlBuilderImpl.h"
#include "SqlBuilderFactoryImpl.h"

ICCFactoryCppInitialize(CSqlBuilderFactoryImpl)

ISqlBuilderPtr CSqlBuilderFactoryImpl::CreateSqlBuilder()
{	
	return boost::make_shared<CSqlBuilderImpl>(m_IResourceManagerPtr);
}
