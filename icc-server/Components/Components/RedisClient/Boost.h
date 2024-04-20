#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <thread>

#include <boost/timer/timer.hpp>

#include <ICC/ICCExceptionDefaultHandle.h>
#include <ICC/ICCFactoryImplDefault.h>
#include <ICC/ICCLibExport.h>
#include <ICC/ICCGetResource.h>
#include <ICC/ICCSetResource.h>
#include <Base/FactoryBase.h>
#include <Base/ComponentBase.h>
#include <Redisclient/IRedisClientFactory.h>
#include <StringUtil/IStringFactory.h>
#include <Log/ILogFactory.h>
#include <Config/IConfigFactory.h>
#include <HelpTool/HelpToolFactory.h>
#include "Base.hpp"

#define MODULE_NAME "redisclient"

using namespace ICC;
using namespace Redis;