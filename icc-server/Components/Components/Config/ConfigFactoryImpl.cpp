#include "Boost.h"
#include "ConfigFactoryImpl.h"
#include "ConfigConfigImpl.h"
#include "ConfigImpl.h"
#include "StaticConfigImpl.h"
#include "SqlConfigImpl.h"

ICCFactoryCppInitialize(CConfigFactoryImpl)

IConfigPtr CConfigFactoryImpl::CreateConfigConfig()
{
	if (!m_pConfigConfig)
	{
		try
		{
			m_pConfigConfig = boost::make_shared<CConfigConfigImpl>(m_IResourceManagerPtr);
		}		
		catch (...)
		{
		}
		
	}
	return m_pConfigConfig;
}

IConfigPtr CConfigFactoryImpl::CreateConfig()
{
	// ¶¯Ì¬¶ÁÈ¡
	return boost::make_shared<CConfigImpl>(m_IResourceManagerPtr);

	//if (!m_pConfig)
	//{
	//	m_pConfig = boost::make_shared<CConfigImpl>(m_IResourceManagerPtr);
	//}
	//return m_pConfig;
}

IConfigPtr CConfigFactoryImpl::CreateStaticConfig()
{
	if (!m_pStaticConfig)
	{
		m_pStaticConfig = boost::make_shared<CStaticConfigImpl>(m_IResourceManagerPtr);
	}
	return m_pStaticConfig;
}

IConfigPtr CConfigFactoryImpl::CreateSqlConfig()
{
	if (!m_pSqlConfig)
	{
		m_pSqlConfig = boost::make_shared<CSqlConfigImpl>(m_IResourceManagerPtr);
	}
	return m_pSqlConfig; 
}
