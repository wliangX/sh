#pragma once
#pragma warning(disable:4996)

#include <map>

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim_all.hpp>

#include <ICC/ICCSetResource.h>
#include <ICC/ICCGetResource.h>
#include <ICC/ICCLibExport.h>
#include <ICC/ICCLibImport.h>
#include <ICC/ICCFactoryImplDefault.h>
#include <Base/FactoryBase.h>
#include <Base/ComponentBase.h>
#include <IResourceManager.h>
#include <Log/ILogFactory.h>
#include <Xml/IXmlFactory.h>
#include <Json/IJsonFactory.h>
#include <Config/IConfigFactory.h>
#include <StringUtil/IStringFactory.h>
#include <SqlBuilder/ISqlRequestFactory.h>
#include <SqlBuilder/ISqlBuilderFactory.h>
#include <Lock/ILockFactory.h>
// tinyxml2
#include "tinyxml2.h"

#define MODULE_NAME "sqlbuilder"

using namespace ICC;
using namespace DataBase;