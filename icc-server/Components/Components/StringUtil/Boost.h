#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

#include <string>
#include <stdarg.h>

// boost
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>
#include <boost/format.hpp>
#include <boost/format/group.hpp>
#include <boost/uuid/uuid.hpp>  
#include <boost/uuid/uuid_generators.hpp>  
#include <boost/uuid/uuid_io.hpp> 
#include <boost/algorithm/string/case_conv.hpp>  
#include <boost/algorithm/string/find.hpp>  
#include <boost/regex.hpp> 
#include <boost/algorithm/string/regex.hpp>
#include <boost/lexical_cast.hpp>  
#include <boost/random.hpp>
#include <boost/atomic.hpp>
#include <boost/xpressive/xpressive_dynamic.hpp>
//icc
#include <ICC/ICCExceptionDefaultHandle.h>
#include <ICC/ICCFactoryImplDefault.h>
#include <ICC/ICCLibExport.h>
#include <ICC/ICCSetResource.h>
#include <Base/FactoryBase.h>
#include <Base/ComponentBase.h>
#include <StringUtil/IStringFactory.h>
#include <StringUtil/IStringUtil.h>

using namespace ICC;
using namespace StringUtil;