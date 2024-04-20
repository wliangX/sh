#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

#include <ICC/ICCExceptionDefaultHandle.h>
#include <ICC/ICCFactoryImplDefault.h>
#include <ICC/ICCLibExport.h>
#include <ICC/ICCSetResource.h>
#include <Base/FactoryBase.h>
#include <Base/ComponentBase.h>
#include <TcpServer/ITcpServerFactory.h>
#include <TcpServer/ITcpServer.h>
#include <Log/ILogFactory.h>

#define MODULE_NAME "tcpserver"

using namespace ICC;
using namespace Tcp;