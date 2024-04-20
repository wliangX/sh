#include "Boost.h"
#include "StaticConfigImpl.h"

CStaticConfigImpl::CStaticConfigImpl(IResourceManagerPtr p_ResourceManagerPtr) :m_pResourceManager(p_ResourceManagerPtr)
{
	IConfigPtr l_pConfigConfig = ICCGetIConfigFactory()->CreateConfigConfig();
	m_strFilePath = l_pConfigConfig->GetValue("ICC.StaticConfigPath", "./Config/StaticConfig.xml");

	m_XmlPtr = ICCGetIXmlFactory()->CreateXml();
	if (!m_XmlPtr->LoadFile(m_strFilePath))
	{
		std::cout << "load config file failed." << std::endl;
	}
}

CStaticConfigImpl::~CStaticConfigImpl()
{

}

std::string CStaticConfigImpl::GetValue(std::string p_strNodePath, std::string p_strDefault)
{
	return m_XmlPtr->GetText(p_strNodePath, p_strDefault);
}

bool CStaticConfigImpl::SetValue(std::string p_strNodePath, std::string p_strValue)
{
	m_XmlPtr->SetText(p_strNodePath, p_strValue);
	return m_XmlPtr->SaveAs(m_strFilePath);
}

unsigned int CStaticConfigImpl::GetNodeCount(std::string p_strNodePath, std::string p_strNodeName)
{
	return m_XmlPtr->GetChildCount(p_strNodePath, p_strNodeName);
}

IResourceManagerPtr CStaticConfigImpl::GetResourceManager()
{
	return m_pResourceManager;
}
