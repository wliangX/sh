#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

#include <cstdlib>

//Boost
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/format.hpp>
#include <boost/random.hpp>
#include <boost/xpressive/xpressive_dynamic.hpp>
#include <boost/thread.hpp>

//ICC
#include <ICC/ICCExceptionDefaultHandle.h>
#include <ICC/ICCFactoryImplDefault.h>
#include <ICC/ICCLibExport.h>
#include <ICC/ICCSetResource.h>
#include <Base/FactoryBase.h>
#include <Base/ComponentBase.h>
#include <DateTime/IDateTimeFactory.h>
#include <DateTime/IDateTime.h>

//Project
#include "Timeduration.h"
#include "DateTimeImpl.h"

using namespace ICC;
using namespace DateTime;
