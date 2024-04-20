#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

// c++
#include <map>
#include <iostream>

#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>  
//#include <boost/bind.hpp>   

//icc
#include <ICC/ICCExceptionDefaultHandle.h>
#include <ICC/ICCFactoryImplDefault.h>
#include <ICC/ICCLibExport.h>
#include <ICC/ICCSetResource.h>
#include <ICC/ICCGetResource.h>
#include <Base/FactoryBase.h>
#include <Base/ComponentBase.h>

#include <StringUtil/IStringFactory.h>
#include <License/LicenselFactory.h>
#include <AmqClient/IAmqClient.h>
#include <Config/IConfigFactory.h>
#include <Json/IJsonFactory.h>
#include <Log/ILogFactory.h>
#include <DateTime/IDateTimeFactory.h>
#include <HelpTool/HelpToolFactory.h>

#include <Protocol/CGetLicenseRequest.h>
#include <Protocol/CSmpSyncRequest.h>

#define MODULE_NAME "license"

using namespace ICC;
using namespace License;
