#pragma once

#include <Xml/IXmlFactory.h>
#include <AmqClient/IAmqClient.h>

namespace ICC 
{
	class CAmqClientImpl :
		public IAmqClient
	{
		ICCFactoryHeaderInitialize(CAmqClientImpl);
	public:
		virtual IConsumerPtr CreateAsyncConsumer();
		virtual IProducerPtr CreateProducer();

	};
} /*namespace ICC*/
