#include "Boost.h"
//Boost
#include <boost/make_shared.hpp>
//Project
#include "TcpServerImpl.h"

CTcpServerImpl::CTcpServerImpl(IResourceManagerPtr p_ResourceManagerPtr) : m_pResourceManager(p_ResourceManagerPtr)
{
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_EffectiveAcceptorPtr = boost::make_shared<EffectiveAcceptor>(m_pLog);
}
CTcpServerImpl::~CTcpServerImpl()
{

}

bool CTcpServerImpl::Listen(std::string p_strIp, unsigned short p_ushPort, unsigned int p_ThreadCount, IConnectionCreaterPtr p_ConnectionCreaterPtr)
{
	return m_EffectiveAcceptorPtr->Listen(p_strIp, p_ushPort, p_ThreadCount, p_ConnectionCreaterPtr);
}

void CTcpServerImpl::Close()
{
	return m_EffectiveAcceptorPtr->Close();
}

unsigned int CTcpServerImpl::Send(std::string p_strClientTag, const char* p_pData, unsigned int p_ilength)
{
	return m_EffectiveAcceptorPtr->Send(p_strClientTag, p_pData, p_ilength);
}

IResourceManagerPtr CTcpServerImpl::GetResourceManager()
{
	return m_pResourceManager;
}