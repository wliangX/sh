#pragma once

namespace ICC
{
	namespace JsonParser
	{
		class CJsonImpl :
			public IJson
		{
		public:
			CJsonImpl(IResourceManagerPtr pResourceManager);
			~CJsonImpl();

		private:
			virtual IResourceManagerPtr GetResourceManager();

		private:
			IResourceManagerPtr m_pResourceManager;

		public:
			virtual bool LoadFile(std::string p_strFile);
			virtual bool SaveAs(std::string p_strFile);
			virtual bool LoadJson(std::string p_strJson);
			virtual std::string ToString();
			virtual std::string GetNodeValue(std::string p_strNodePath, std::string p_strDefault);
			virtual void SetNodeValue(std::string p_strNodePath, std::string p_strValue);

			virtual void SetNodeValue(std::string p_strNodePath, int p_nValue);
			virtual int GetCount(std::string p_strNodePath);
			virtual bool Find(std::string p_strNodePath);

		private:
			rapidjson::Document		 m_oDoc;
			StringUtil::IStringUtilPtr			 m_pStrUti;
		};
	}
}


