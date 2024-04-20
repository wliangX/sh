#pragma once
#include <IResource.h>
#include <DBConn/IDBConn.h>

namespace ICC
{
	namespace DataBase
	{
		enum DBType
		{
			PostgreSQL = 0
		};

		class IDBConnFactory :
			public IResource
		{
		public:
			virtual IDBConnPtr CreateDBConn(DBType p_iDBType = PostgreSQL) = 0;
			virtual IDBConnPtr CreateNewDBConn(DBType p_iDBType = PostgreSQL) = 0;
		};

		typedef boost::shared_ptr<IDBConnFactory> IDBConnFactoryPtr;
	}
}

#define ICCIDBConnFactoryResourceName "ICC.Com.IDBConnFactory.ResourceName"

#define ICCSetIDBConnFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCIDBConnFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetIDBConnFactoryNull \
this->GetResourceManager()->SetResource(ICCIDBConnFactoryResourceName, nullptr) \

#define ICCGetIDBConnFactory() \
boost::dynamic_pointer_cast<DataBase::IDBConnFactory>(this->GetResourceManager()->GetResource(ICCIDBConnFactoryResourceName))

//ex
#define ICCIDBConnExFactoryResourceName "ICC.Com.IDBConnExFactory.ResourceName"

#define ICCSetIDBConnExFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCIDBConnExFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetIDBConnExFactoryNull \
this->GetResourceManager()->SetResource(ICCIDBConnExFactoryResourceName, nullptr) \

#define ICCGetIDBConnExFactory() \
boost::dynamic_pointer_cast<DataBase::IDBConnFactory>(this->GetResourceManager()->GetResource(ICCIDBConnExFactoryResourceName))