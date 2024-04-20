#pragma once
#include <IResource.h>
#include <AmqClient/IConsumer.h>
#include <AmqClient/IProducer.h>

namespace ICC 
{
	class IAmqClient :
		public IResource
	{
	public:
		virtual IConsumerPtr CreateAsyncConsumer() = 0;
		virtual IProducerPtr CreateProducer() = 0;
	};

	typedef boost::shared_ptr<IAmqClient> IAmqClientPtr;

} /*namespace ICC*/

#define ICCIAmqClientResourceName "ICC.Com.IAmqClient.ResourceName"

#define ICCSetIAmqClient(ImplClass) \
this->GetResourceManager()->SetResource(ICCIAmqClientResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetIAmqClientNull \
this->GetResourceManager()->SetResource(ICCIAmqClientResourceName, nullptr) \

#define ICCGetAmqClient() \
boost::dynamic_pointer_cast<ICC::IAmqClient>(this->GetResourceManager()->GetResource(ICCIAmqClientResourceName))