#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

// boost
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>
// icc
#include <ICC/ICCLibExport.h>
#include <ICC/ICCGetResource.h>
#include <Base/FactoryBase.h>
#include <Base/PluginBase.h>
#include <Base/BusinessBase.h>

#include <boost/filesystem.hpp>

#include <AmqClient/IAmqClient.h>
#include <Config/IConfigFactory.h>
#include <DateTime/IDateTimeFactory.h>
#include <Json/IJsonFactory.h>
#include <Lock/ILockFactory.h>
#include <Log/ILogFactory.h>
#include <Observer/IObserverFactory.h>
#include <StringUtil/IStringFactory.h>
#include <HelpTool/HelpToolFactory.h>
#include <Timer/ITimerFactory.h>
#include <HttpClient/IHttpClientFactory.h>

#include <fstream>

// Protocol


#define MODULE_NAME "lsp"

using namespace ICC;