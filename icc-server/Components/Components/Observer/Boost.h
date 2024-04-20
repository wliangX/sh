#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

#include <string>
#include <list>
#include <map>
//boost
#include <boost/thread.hpp>
#include <boost/asio.hpp>
//icc
#include <ICC/ICCExceptionDefaultHandle.h>
#include <ICC/ICCFactoryImplDefault.h>
#include <ICC/ICCLibExport.h>
#include <ICC/ICCSetResource.h>
#include <ICC/ICCGetResource.h>
#include <Base/FactoryBase.h>
#include <Base/ComponentBase.h>
#include <Observer/IObserverFactory.h>
#include <Observer/IObserver.h>
#include <Lock/ILockFactory.h>
#include <Json/IJsonFactory.h>
#include <AmqClient/IAsyncCallback.h>

#define MODULE_NAME "observer"

using namespace ICC;
using namespace ObserverPattern;