#pragma once

namespace ICC
{
	class CStaticConfigImpl :
		public IConfig
	{
	public:
		CStaticConfigImpl(IResourceManagerPtr p_ResourceManagerPtr);
		~CStaticConfigImpl();
	public:
		virtual std::string GetValue(std::string p_strNodePath, std::string p_strDefault);
		virtual bool SetValue(std::string p_strNodePath, std::string p_strValue);
		virtual unsigned int GetNodeCount(std::string p_strNodePath, std::string p_strNodeName);

	private:
		virtual IResourceManagerPtr GetResourceManager();
	private:
		std::string m_strFilePath;
		Xml::IXmlPtr  m_XmlPtr;
		IResourceManagerPtr m_pResourceManager;
	};
}