#pragma once

//#include <thread>
//boost
#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include <string>
#include <vector>

#include "pg_type.h"
#include "libpq-fe.h"

#include <ICC/ICCExceptionDefaultHandle.h>
#include <ICC/ICCFactoryImplDefault.h>
#include <ICC/ICCLibExport.h>
#include <ICC/ICCSetResource.h>
#include <ICC/ICCGetResource.h>
#include <Base/FactoryBase.h>
#include <Base/ComponentBase.h>

#include <Log/ILogFactory.h>
#include <DateTime/IDateTimeFactory.h>
#include <StringUtil/IStringFactory.h>
#include <PGClient/IPGClientFactory.h>
#include <PGClient/IResultSetFactory.h>
#include <Lock/ILockFactory.h>
#include <Config/IConfigFactory.h>
#include <StringUtil/IStringFactory.h>
#include <HelpTool/HelpToolFactory.h>

#define MODULE_NAME "postgresex"

using namespace ICC;
using namespace DataBase;