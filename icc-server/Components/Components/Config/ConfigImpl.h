#pragma once

#include <mutex>

namespace ICC
{
	class CConfigImpl :	public IConfig
	{
	public:
		CConfigImpl(IResourceManagerPtr p_ResourceManagerPtr);
		~CConfigImpl();
	public:
		virtual std::string GetValue(std::string p_strNodePath, std::string p_strDefault);
		virtual bool SetValue(std::string p_strNodePath, std::string p_strValue);
		virtual unsigned int GetNodeCount(std::string p_strNodePath, std::string p_strNodeName);

		virtual void SetServerFlag(const std::string& strServerFlag);
		virtual std::string ServerFlag();

		virtual unsigned int GetPubicNodeCount(std::string p_strNodePath, std::string p_strNodeName);
		virtual std::string GetPublicValue(int p_nCfgType, std::string p_strNodePath, std::string p_strDefault);

		virtual bool ReloadConfig();

	private:
		virtual IResourceManagerPtr GetResourceManager();

		void _SetXmlPtr(Xml::IXmlPtr  pXmlPtr);
		Xml::IXmlPtr  _GetXmlPtr();

	private:
		std::string m_strFilePath;
		static std::string m_strServerFlag;
		Xml::IXmlPtr  m_XmlPtr;
		StringUtil::IStringUtilPtr m_pString;
		IResourceManagerPtr m_pResourceManager;

		//π´π≤≈‰÷√
		Xml::IXmlPtr  m_XmlRedisPtr;
		Xml::IXmlPtr  m_XmlMQPtr;
		Xml::IXmlPtr  m_XmlDBPtr;

		std::mutex m_mutexXml;

		std::time_t m_tmLastUpdateTime;
	};
}