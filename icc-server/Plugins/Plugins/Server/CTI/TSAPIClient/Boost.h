#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

#if defined(WIN32) || defined(WIN64)
#include <tchar.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#endif

#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>

#include <ICC/ICCExceptionDefaultHandle.h>
#include <ICC/ICCFactoryImplDefault.h>
#include <ICC/ICCLibExport.h>
#include <ICC/ICCSetResource.h>
#include <ICC/ICCGetResource.h>
#include <Base/FactoryBase.h>
#include <Base/ComponentBase.h>

#include <Config/IConfig.h>
#include <Config/IConfigFactory.h>
#include <StringUtil/IStringFactory.h>
#include <StringUtil/IStringUtil.h>
#include <Log/ILogFactory.h>
#include <DateTime/IDateTime.h>
#include <Redisclient/IRedisClientFactory.h>
#include <DateTime/IDateTimeFactory.h>
#include <Observer/IObserverFactory.h>
#include <Thread/IThreadFactory.h>

#include <HelpTool/HelpToolFactory.h>
#include <Udp/IUdpFactory.h>

#include "IResource.h"
#include "IResourceManager.h"

#include <string>
#include <map>
#include <list>
#include <vector>
#include <ctime>
#include <memory>
#include <mutex>
#include <thread>
#include <iostream>
#include <time.h>
#include <stdio.h>

#include "acs.h"
#include "csta.h"
#include "cstadefs.h"
#include "tsplatfm.h"
#include "attpriv.h"

#include "CAgentPhoneState.h"

#include <CTIServer/IProtocolDefine.h>
#include <CTIServer/ISwitchDefine.h>
#include <CTIServer/ISwitchNotifaction.h>
#include <CTIServer/ISwitchClientFactory.h>
#include <CTIServer/ISwitchClient.h>

#include <Json/IJsonFactory.h>


#include "Protocol/CSeatInfo.h"

#define MODULE_NAME "tsapiclient"

#if defined(WIN32) || defined(WIN64)
#define SLEEP_SEC(x)	Sleep(x*1000)
#else
#define SLEEP_SEC(x)	sleep(x)
#endif

#if defined(WIN32) || defined(WIN64)
#define SLEEP_MILLION_SEC(x)	Sleep(x)
#else
#define SLEEP_MILLION_SEC(x)	usleep(x)
#endif

#define	SAFE_LOCK(lk) std::lock_guard<std::mutex> l_guard(lk)

using namespace std;
using namespace ICC;
using namespace SwitchClient;

static void ShowMsg(const std::string& strMessage)
{
#if defined(_DEBUG) && (defined(WIN32) || defined(WIN64))
	std::cout << strMessage.c_str() << std::endl;
#endif
}
