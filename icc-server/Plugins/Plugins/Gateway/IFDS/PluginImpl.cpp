#include "Boost.h"
#include "PriorityLevelDef.h"
#include "PluginImpl.h"
#include "BusinessImpl.h"
#include "CommonLogger.h"

unsigned int CPluginImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_IFDS;
}

void CPluginImpl::OnStart()
{
	//CCommonLogger::Instance().InitLogger(GetResourceManager());

	//WRITE_DEBUG_LOG("CPluginImpl::OnStart() begin.");

	m_pBusiness = boost::make_shared<CBusinessImpl>();
	m_pBusiness->Init(GetResourceManager());	
	m_pBusiness->Start();

	//WRITE_DEBUG_LOG("CPluginImpl::OnStart() complete.");
}

void CPluginImpl::OnStop()
{
	//WRITE_DEBUG_LOG("CPluginImpl::OnStop() begin.");

	m_pBusiness->Stop();
	m_pBusiness->Destroy();

	//WRITE_DEBUG_LOG("CPluginImpl::OnStop() complete.");

	//CCommonLogger::Instance().UninitLogger();
}
