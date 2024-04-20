#ifndef __CommonLogger_H__
#define __CommonLogger_H__

#include <string>
#include <atomic>
#include "Log/ILog.h"
#include "IResourceManager.h"

using namespace ICC;

#define WRITE_COMMON_LOG(level, strLog) \
	do \
					{ \
	CCommonLogger::Instance().WriteLog(strLog, level); \
					}while (0);


#define WRITE_INFO_LOG(strLog)        WRITE_COMMON_LOG(0, strLog)
#define WRITE_ERROR_LOG(strLog)       WRITE_COMMON_LOG(1, strLog)
#define WRITE_FATAL_LOG(strLog)       WRITE_COMMON_LOG(2, strLog)
#define WRITE_WARN_LOG(strLog)        WRITE_COMMON_LOG(3, strLog)
#define WRITE_DEBUG_LOG(strLog)       WRITE_COMMON_LOG(4, strLog)
#define WRITE_LOWDEBUG_LOG(strLog)    WRITE_COMMON_LOG(5, strLog)


class CCommonLogger
{
public:
	bool                  InitLogger(IResourceManagerPtr ptr);
	void                  WriteLog(const std::string& strTmpLog, int iLevel);
	void                  UninitLogger();

	IResourceManagerPtr   GetResourceManager();
	
public:
	static CCommonLogger& Instance();

public:
	CCommonLogger();
	~CCommonLogger();

private:
	Log::ILogPtr m_pLog;
	IResourceManagerPtr m_reSourcePtr;
	std::atomic_bool m_bInit;
};

#endif