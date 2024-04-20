#include "Boost.h"
#include "AmqClientImpl.h"
#include "AsyncConsumerImpl.h"
#include "ProducerImpl.h"

ICCFactoryCppInitialize(CAmqClientImpl)

IConsumerPtr CAmqClientImpl::CreateAsyncConsumer()
{
	return boost::make_shared<CAsyncConsumerImpl>(m_IResourceManagerPtr);
}

IProducerPtr CAmqClientImpl::CreateProducer()
{
	return boost::make_shared<CProducerImpl>(m_IResourceManagerPtr);
}

