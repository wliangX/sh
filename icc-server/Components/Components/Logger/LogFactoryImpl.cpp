#include "Boost.h"
#include "LogFactoryImpl.h"
#include "LogImpl.h"

ICCFactoryCppInitialize(CLogFactoryImpl)

ILogPtr CLogFactoryImpl::GetLogger(const std::string& p_strModuleName)
{
	if (m_mapLog[ICCILogFactoryResourceName] == ILogPtr())
	{
		m_mapLog[ICCILogFactoryResourceName] = boost::make_shared<CLogImpl>(m_IResourceManagerPtr);
		m_mapLog[ICCILogFactoryResourceName]->SetModuleName("icc");
	}

	ILogPtr l_logPtr = m_mapLog[ICCILogFactoryResourceName];
	return l_logPtr;
}

