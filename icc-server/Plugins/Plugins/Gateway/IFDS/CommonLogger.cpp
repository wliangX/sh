#include "CommonLogger.h"
#include "Boost.h"

/************************************************************************/
/*                                                                      */
/************************************************************************/
bool CCommonLogger::InitLogger(IResourceManagerPtr ptr)
{
	m_reSourcePtr = ptr;
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_bInit = true;
	return true;
}

void CCommonLogger::UninitLogger()
{
	m_bInit = false;
	m_pLog = nullptr;
}

void CCommonLogger::WriteLog(const std::string& strTmpLog, int iLevel)
{
	if (!m_bInit)
	{
		return;
	}

	switch (iLevel)
	{
	case 0:
		ICC_LOG_INFO(m_pLog, "%s", strTmpLog.c_str());
		break;
	case 1:
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		break;
	case 2:
		ICC_LOG_FATAL(m_pLog, "%s", strTmpLog.c_str());
		break;
	case 3:
		ICC_LOG_WARNING(m_pLog, "%s", strTmpLog.c_str());
		break;
	case 4:
		ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());
		break;
	case 5:
		ICC_LOG_LOWDEBUG(m_pLog, "%s", strTmpLog.c_str());
		break;	
	default:
		ICC_LOG_LOWDEBUG(m_pLog, "%s", strTmpLog.c_str());
		break;
	}
}



CCommonLogger& CCommonLogger::Instance()
{
	static CCommonLogger logger;
	return logger;
}

IResourceManagerPtr CCommonLogger::GetResourceManager()
{
	return m_reSourcePtr;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
CCommonLogger::CCommonLogger()
{

}

CCommonLogger::~CCommonLogger()
{

}