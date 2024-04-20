#include "Boost.h"
#include "SqlConfigImpl.h"

CSqlConfigImpl::CSqlConfigImpl(IResourceManagerPtr p_ResourceManagerPtr) :m_pResourceManager(p_ResourceManagerPtr)
{
	IConfigPtr l_pConfigConfig = ICCGetIConfigFactory()->CreateConfigConfig();
	m_strFilePath = l_pConfigConfig->GetValue("ICC.SqlConfigPath", "./Config/Sql.xml");

	m_XmlPtr = ICCGetIXmlFactory()->CreateXml();
	if (!m_XmlPtr->LoadFile(m_strFilePath))
	{
		std::cout << "load config file failed." << std::endl;
	}
}

CSqlConfigImpl::~CSqlConfigImpl()
{
	
}

std::string CSqlConfigImpl::GetValue(std::string p_strNodePath, std::string p_strDefault)
{
	return m_XmlPtr->GetText(p_strNodePath, p_strDefault);
}

bool CSqlConfigImpl::SetValue(std::string p_strNodePath, std::string p_strValue)
{
	m_XmlPtr->SetText(p_strNodePath, p_strValue);
	return m_XmlPtr->SaveAs(m_strFilePath);
}

unsigned int CSqlConfigImpl::GetNodeCount(std::string p_strNodePath, std::string p_strNodeName)
{
	return m_XmlPtr->GetChildCount(p_strNodePath, p_strNodeName);
}

IResourceManagerPtr CSqlConfigImpl::GetResourceManager()
{
	return m_pResourceManager;
}