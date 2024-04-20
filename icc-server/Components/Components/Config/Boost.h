#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

// c++
#include <map>

//boost
//#include <boost/program_options.hpp>
#include <boost/property_tree/ini_parser.hpp>

//icc
#include <ICC/ICCExceptionDefaultHandle.h>
#include <ICC/ICCFactoryImplDefault.h>
#include <ICC/ICCLibExport.h>
#include <ICC/ICCSetResource.h>
#include <ICC/ICCGetResource.h>
#include <Base/FactoryBase.h>
#include <Base/ComponentBase.h>
#include <Config/IConfigFactory.h>
#include <Config/IConfig.h>
#include <Lock/ILockFactory.h>
#include <StringUtil/IStringFactory.h>
#include <Xml/IXmlFactory.h>

using namespace ICC;
using namespace Config;
