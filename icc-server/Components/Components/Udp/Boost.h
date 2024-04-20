#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

#include <ICC/ICCExceptionDefaultHandle.h>
#include <ICC/ICCFactoryImplDefault.h>
#include <ICC/ICCLibExport.h>
#include <ICC/ICCSetResource.h>
#include <Base/FactoryBase.h>
#include <Base/ComponentBase.h>
#include <Udp/IUdpFactory.h>
#include <Udp/IUdp.h>
#include <Log/ILogFactory.h>
//Boost
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread.hpp>

#define MODULE_NAME "udp"

using namespace ICC;
using namespace Udp;