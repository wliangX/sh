#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

#include <iostream>

// boost
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast/lexical_cast_old.hpp>

// icc
#include <ICC/ICCLibExport.h>
#include <Base/FactoryBase.h>
#include <Base/PluginBase.h>
#include <Base/BusinessBase.h>

#include <AmqClient/IAmqClient.h>
#include <Config/IConfigFactory.h>
#include <Json/IJsonFactory.h>
#include <Lock/ILockFactory.h>
#include <Log/ILogFactory.h>
#include <Observer/IObserverFactory.h>
#include <StringUtil/IStringFactory.h>
#include <Thread/IThreadFactory.h>
#include <TcpClient/ITcpClientFactory.h>
#include <DateTime/IDateTimeFactory.h>
#include <HelpTool/HelpToolFactory.h>

// Protocol
#include <Protocol/CFast110RequestAndRespond.h>

#define MODULE_NAME "fast110"

using namespace ICC;
