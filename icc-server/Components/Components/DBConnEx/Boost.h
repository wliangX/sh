#pragma once
#include "libpq-fe.h"
#include <boost/chrono.hpp>
#include <boost/timer/timer.hpp>
#include <boost/thread.hpp>

#include <ICC/ICCLibExport.h>
#include <ICC/ICCSetResource.h>
#include <ICC/ICCGetResource.h>
#include <ICC/ICCFactoryImplDefault.h>
#include <ICC/ICCExceptionDefaultHandle.h>
#include <Base/FactoryBase.h>
#include <Base/ComponentBase.h>

#include <Log/ILogFactory.h>
#include <Config/IConfigFactory.h>
#include <DBConn/IDBConnFactory.h>
#include <DateTime/IDateTimeFactory.h>
#include <PGClient/IPGClientFactory.h>
#include <PGClient/IResultSetFactory.h>
#include <SqlBuilder/ISqlRequestFactory.h>
#include <SqlBuilder/ISqlBuilderFactory.h>

#define MODULE_NAME "dbconnex"

using namespace ICC;
using namespace DataBase;