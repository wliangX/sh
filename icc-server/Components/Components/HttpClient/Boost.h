#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

#include <mutex>
#include <vector>
#include <ICC/ICCExceptionDefaultHandle.h>
#include <ICC/ICCFactoryImplDefault.h>
#include <ICC/ICCLibExport.h>
#include <ICC/ICCGetResource.h>
#include <ICC/ICCSetResource.h>
#include <Base/FactoryBase.h>
#include <Base/ComponentBase.h>
#include <HttpClient/IHttpClientFactory.h>
#include <HttpClient/IHttpClient.h>
#include <StringUtil/IStringFactory.h>

#include <Log/ILogFactory.h>


#define MODULE_NAME "HttpClient"

using namespace ICC;
using namespace Http;