#include "Boost.h"
#include "DBConnImpl.h"
#include "DBConnFactoryImpl.h"

ICCFactoryCppInitialize(CDBConnFactoryImpl)

boost::mutex CDBConnFactoryImpl::sm_Mutex;
IDBConnPtr CDBConnFactoryImpl::sm_pDBConn = nullptr;

IDBConnPtr CDBConnFactoryImpl::CreateDBConn(DBType p_iDBType /*= PostgreSQL*/)
{
	return Instance(m_IResourceManagerPtr);
}

IDBConnPtr CDBConnFactoryImpl::CreateNewDBConn(DBType p_iDBType /*= PostgreSQL*/)
{
/*
	switch (p_iDBType)
	{
	case DBType::PostgreSQL:
		return boost::make_shared<CPGConnImpl>(m_IResourceManagerPtr);
	default:
		return boost::make_shared<CPGConnImpl>(m_IResourceManagerPtr);
	}*/
	return Instance(m_IResourceManagerPtr);
}

IDBConnPtr& CDBConnFactoryImpl::Instance(IResourceManagerPtr p_pResourceManagerPtr, DBType p_iDBType /* = PostgreSQL */)
{
	if (nullptr == sm_pDBConn)
	{
		boost::lock_guard<boost::mutex> lock(sm_Mutex);
		if (nullptr == sm_pDBConn)
		{
			switch (p_iDBType)
			{
			case DBType::PostgreSQL:
				sm_pDBConn = boost::make_shared<CPGConnImpl>(p_pResourceManagerPtr);
				break;
			default:
				sm_pDBConn = boost::make_shared<CPGConnImpl>(p_pResourceManagerPtr);
				break;
			}			
		}
	}
	return sm_pDBConn;
}