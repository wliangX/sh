#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

// c++
#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>

//boost
#include <boost/foreach.hpp>
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast/lexical_cast_old.hpp>

// icc
#include <ICC/ICCExceptionDefaultHandle.h>
#include <ICC/ICCFactoryImplDefault.h>
#include <ICC/ICCLibExport.h>
#include <ICC/ICCSetResource.h>
#include <Base/FactoryBase.h>
#include <Base/ComponentBase.h>
#include <Json/IJsonFactory.h>
#include <Json/IJson.h>
#include <Lock/ILockFactory.h>
#include <StringUtil/IStringFactory.h>

// rapidjson
#include "document.h"
#include "stringbuffer.h"
#include "pointer.h"
#include "writer.h"
#include "reader.h"
#include "ostreamwrapper.h"
#include "istreamwrapper.h"
#include "prettywriter.h"

using namespace ICC;
using namespace JsonParser;
