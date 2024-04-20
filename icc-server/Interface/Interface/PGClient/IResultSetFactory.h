#pragma once
#include <IResource.h>
#include <PGClient/IResultSet.h>

namespace ICC
{
	namespace DataBase
	{
		class IResultSetFactory :
			public IResource
		{
		public:
			virtual IResultSetPtr CreateResultSet() = 0;
		};

		typedef boost::shared_ptr<IResultSetFactory> IResultSetFactoryPtr;
	}
}
//IResultSetFactory
#define ICCIResultSetFactoryResourceName "ICC.Com.IResultSetFactory.ResourceName"

#define ICCSetIResultSetFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCIResultSetFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetIResultSetFactoryNull \
this->GetResourceManager()->SetResource(ICCIResultSetFactoryResourceName, nullptr) \

#define ICCGetIResultSetFactory() \
boost::dynamic_pointer_cast<DataBase::IResultSetFactory>(this->GetResourceManager()->GetResource(ICCIResultSetFactoryResourceName))