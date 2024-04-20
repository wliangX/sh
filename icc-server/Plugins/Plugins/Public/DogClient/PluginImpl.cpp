#include "Boost.h"
#include "PluginImpl.h"
#include "BusinessImpl.h"

unsigned int CPluginImpl::GetPriorityLevel()
{
	return 10;
}

void CPluginImpl::OnStart()
{
	m_pBusiness = boost::make_shared<CBusinessImpl>();
	m_pBusiness->Init(GetResourceManager());
	m_pBusiness->Start();
}

void CPluginImpl::OnStop()
{
	m_pBusiness->Stop();
	m_pBusiness->Destroy();
}
