#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

#include <mutex>
#include <vector>
#include <list>
#include <ICC/ICCExceptionDefaultHandle.h>
#include <ICC/ICCFactoryImplDefault.h>
#include <ICC/ICCLibExport.h>
#include <ICC/ICCGetResource.h>
#include <ICC/ICCSetResource.h>
#include <Base/FactoryBase.h>
#include <Base/ComponentBase.h>
#include <Log/ILogFactory.h>
#include <Json/IJsonFactory.h>
#include <StringUtil/IStringFactory.h>
#include <Config/IConfigFactory.h>
#include <StringUtil/IStringFactory.h>
#include <Config/IConfigFactory.h>
#include <DateTime/IDateTime.h>
#include <DateTime/IDateTimeFactory.h>
#include <HttpsServer/IHttpsServerFactory.h>
#include <HttpsServer/IHttpsServer.h>

#define SAFE_LOCK(mtx) std::unique_lock<std::mutex> guard(mtx)

#define MODULE_NAME "HttpsServer"

using namespace ICC;