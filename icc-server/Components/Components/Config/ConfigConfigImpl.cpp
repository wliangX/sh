#include "Boost.h"
#include "ConfigConfigImpl.h"

CConfigConfigImpl::CConfigConfigImpl(IResourceManagerPtr p_ResourceManagerPtr) :m_pResourceManager(p_ResourceManagerPtr)
{	
	printf("config will read config.config!\n");
	try
	{
		const std::string strTmpFile = "Config.Config";
		int l_res = 0;

#if defined(_WIN32) || defined(_WIN64)
		l_res = 0;
#else
		access(strTmpFile.c_str(), 0);
#endif
		if (l_res == 0)
		{
			printf("find config file! will read.\n");
			boost::property_tree::read_ini(strTmpFile.c_str(), m_ini);
		}
		else
		{
			printf("not find file! file = %s\n", strTmpFile.c_str());
		}
	}
	catch(...)
	{
		printf("read ini error!\n");
	}
	
	printf("config read config.config success!\n");
	printf("ConfigPath=%s\n", GetValue("ICC.ConfigPath","").c_str());
	printf("ConfigFileDir=%s\n", GetValue("ICC.ConfigFileDir", "").c_str());
	printf("StaticConfigPath=%s\n", GetValue("ICC.StaticConfigPath","").c_str());
	printf("SqlConfigPath=%s\n", GetValue("ICC.SqlConfigPath","").c_str());
}

CConfigConfigImpl::~CConfigConfigImpl()
{
	
}

std::string CConfigConfigImpl::GetValue(std::string p_strNodePath, std::string p_strDefault)
{
	try
	{
		return m_ini.get<std::string>(p_strNodePath, p_strDefault);
	}	
	catch (...)
	{
		return "";
	}
	
}

bool CConfigConfigImpl::SetValue(std::string p_strNodePath, std::string p_strValue)
{
	return false;
}

unsigned int CConfigConfigImpl::GetNodeCount(std::string p_strNodePath, std::string p_strNodeName)
{
	return 0;
}

IResourceManagerPtr CConfigConfigImpl::GetResourceManager()
{
	return m_pResourceManager;
}