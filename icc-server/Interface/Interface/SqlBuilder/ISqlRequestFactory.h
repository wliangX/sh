#pragma once
#include <IResource.h>
#include <SqlBuilder/ISqlBuilder.h>

namespace ICC
{
	namespace DataBase
	{
		class ISqlRequestFactory : public IResource
		{
		public:
			virtual ISqlRequestPtr CreateSqlRequest() = 0;
		};

		typedef boost::shared_ptr<ISqlRequestFactory> ISqlRequestFactoryPtr;
	}
}

#define ICCISqlRequestFactoryResourceName "ICC.Com.ISqlRequestFactory.ResourceName"

#define ICCSetISqlRequestFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCISqlRequestFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetISqlRequestFactoryNull \
this->GetResourceManager()->SetResource(ICCISqlRequestFactoryResourceName, nullptr) \

#define ICCGetISqlRequestFactory() \
boost::dynamic_pointer_cast<DataBase::ISqlRequestFactory>(this->GetResourceManager()->GetResource(ICCISqlRequestFactoryResourceName))