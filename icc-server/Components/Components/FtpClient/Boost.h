#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

#include <ICC/ICCExceptionDefaultHandle.h>
#include <ICC/ICCFactoryImplDefault.h>
#include <ICC/ICCLibExport.h>
#include <ICC/ICCSetResource.h>
#include <Base/FactoryBase.h>
#include <Base/ComponentBase.h>
#include <FtpClient/IFtpClient.h>
#include <FtpClient/IFtpClientFactory.h>
#include <Lock/ILockFactory.h>
#include <HelpTool/HelpToolFactory.h>
#include "Xml//IXmlFactory.h"
#include "IResource.h"
#include "IResourceManager.h"

//boost
#include <boost/regex.hpp>  
#include <boost/thread/thread.hpp>
#include <boost/asio/ip/tcp.hpp>  
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/asio.hpp>  
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/proto/matches.hpp>
#include <boost/thread/condition.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string.hpp>

#include <Log/ILogFactory.h>
//
#include <list>
#include <string>
#include <iostream>
using namespace ICC;
using namespace Ftp;
#define		MODULE_NAME				            "FtpClient"