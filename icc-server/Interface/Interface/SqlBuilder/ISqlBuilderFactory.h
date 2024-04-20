#pragma once
#include <IResource.h>
#include <SqlBuilder/ISqlBuilder.h>

namespace ICC
{
	namespace DataBase
	{
		class ISqlBuilderFactory : public IResource
		{
		public:
			virtual ISqlBuilderPtr CreateSqlBuilder() = 0;
		};

		typedef boost::shared_ptr<ISqlBuilderFactory> ISqlBuilderFactoryPtr;
	}
}

#define ICCISqlBuilderFactoryResourceName "ICC.Com.ISqlBuilderFactory.ResourceName"

#define ICCSetISqlBuilderFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCISqlBuilderFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetISqlBuilderFactoryNull \
this->GetResourceManager()->SetResource(ICCISqlBuilderFactoryResourceName, nullptr) \

#define ICCGetISqlBuilderFactory() \
boost::dynamic_pointer_cast<DataBase::ISqlBuilderFactory>(this->GetResourceManager()->GetResource(ICCISqlBuilderFactoryResourceName))