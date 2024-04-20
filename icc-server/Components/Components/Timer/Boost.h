#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

#include <ICC/ICCExceptionDefaultHandle.h>
#include <ICC/ICCFactoryImplDefault.h>
#include <ICC/ICCLibExport.h>
#include <ICC/ICCSetResource.h>
#include <ICC/ICCGetResource.h>
#include <Base/FactoryBase.h>
#include <Base/ComponentBase.h>
#include <Timer/ITimerFactory.h>
#include <Json/IJsonFactory.h>
#include <Observer/IObserverFactory.h>
#include <StringUtil/IStringFactory.h>

//boost
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>  
//#include <boost/bind.hpp>    
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/condition.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <Protocol/Timer.h>

using namespace ICC;
using namespace Timer;
using namespace ObserverPattern;
