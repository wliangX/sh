#include "Boost.h"
#include "PriorityLevelDef.h"
#include "ComponentImpl.h"
#include "AmqClientImpl.h"
#include "amqinc.h"

namespace ICC 
{

	unsigned int CComponentImpl::GetPriorityLevel()
	{
		return PRIORITY_LEVEL_AMQCLIENT;
	}

	void CComponentImpl::OnInit()
	{
		printf("OnInit = %s\n", "activemq::library::ActiveMQCPP::initializeLibrary()");
		activemq::library::ActiveMQCPP::initializeLibrary();
	}

	void CComponentImpl::OnStart()
	{
		ICCSetIAmqClient(CAmqClientImpl);
	}

	void CComponentImpl::OnStop()
	{
		ICCSetIAmqClientNull;
	}

	void CComponentImpl::OnDestroy()
	{
		activemq::library::ActiveMQCPP::shutdownLibrary();
	}

} /*namespace ICC*/
