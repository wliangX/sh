#pragma once

#include <ICC/ICCLibExport.h>
#include <ICC/ICCFactoryImplDefault.h>
#include <ICC/ICCGetResource.h>
#include <ICC/ICCSetResource.h>
#include <Base/FactoryBase.h>
#include <Base/ComponentBase.h>
#include <AmqClient/IAmqClient.h>
#include <Xml/IXmlFactory.h>
#include <Log/ILogFactory.h>
#include <Json/IJsonFactory.h>
#include <StringUtil/IStringFactory.h>
#include <Config/IConfigFactory.h>
#include <StringUtil/IStringFactory.h>
#include <Config/IConfigFactory.h>
#include <boost/exception/diagnostic_information.hpp>

#define Activemq_Advisory_Connection		"ActiveMQ.Advisory.Connection"
#define Activemq_Advisory					"ActiveMQ.Advisory."

#define ICCExceptionDefaultHandle			boost::current_exception_diagnostic_information()

#define ICCTempQueueExceptionLog			"Cannot publish to a deleted Destination: temp-queue"

#define MODULE_NAME "amqclient"

using namespace ICC;
