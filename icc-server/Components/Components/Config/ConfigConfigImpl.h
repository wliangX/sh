#pragma once

namespace ICC
{
	class CConfigConfigImpl :
		public IConfig
	{
	public:
		CConfigConfigImpl(IResourceManagerPtr p_ResourceManagerPtr);
		~CConfigConfigImpl();
	public:
		virtual std::string GetValue(std::string p_strNodePath, std::string p_strDefault);
		virtual bool SetValue(std::string p_strNodePath, std::string p_strValue);
		virtual unsigned int GetNodeCount(std::string p_strNodePath, std::string p_strNodeName);

	private:
		virtual IResourceManagerPtr GetResourceManager();
	private:
		IResourceManagerPtr m_pResourceManager;
		boost::property_tree::ptree m_ini;
	};
}