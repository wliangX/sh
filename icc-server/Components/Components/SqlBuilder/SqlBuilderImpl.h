#pragma once
#include <string>
#include <map>
namespace ICC
{
	namespace DataBase
	{
		class CSqlBuilderImpl :
			public ISqlBuilder
		{
		public:
			CSqlBuilderImpl(IResourceManagerPtr p_pResourceManager);
			virtual ~CSqlBuilderImpl();

		public:
			//************************************************************************
			// ����:  GetErrorMsg
			// ȫ��:  CSqlBuilderImpl::GetErrorMsg
			// ����:  transform an unsigned int error code to a std::string description
			// ����:  virtual public 
			// ����:  unsigned int p_uiErrorCode, error code returned by other functions
			// ����:  std::string  description of the input error code
			//************************************************************************
			virtual std::string GetErrorMsg(unsigned int p_uiErrorCode);

			/************************************************************************
			// ����:  BuildSql
			// ȫ��:  CSqlBuilderImpl::BuildSql
			// ����:
			// ����:  virtual public
			// ����:  std::string p_strXmlSqlRequest json��ʽ��sql��������
			// ����:  std::string & p_strSqlResult
			// ����:  unsigned int
			//************************************************************************/
			virtual unsigned int BuildSql(std::string p_strSqlRequest, std::string& p_strSqlResult);
			virtual unsigned int BuildSql(ISqlRequestPtr p_pSQLRquest, std::string& p_strSqlResult);

		public:
			IResourceManagerPtr GetResourceManager();
		private:
			/*Method to check the syntax of the xml file*/
			unsigned int ReadConfig();
			unsigned int CheckSqlConfig(tinyxml2::XMLDocument& p_oDoc);

			bool IsBracesMatched(std::string p_strText);
			bool IsBracesMatched(tinyxml2::XMLElement* p_pXMLElem);

			std::string BuildUpdate(tinyxml2::XMLElement* p_pSqlElem, ISqlRequestPtr p_oRequest);
			std::string BuildOthers(tinyxml2::XMLElement* p_pSqlElem, ISqlRequestPtr p_oRequest);//test
			std::string BuildInsert(tinyxml2::XMLElement* p_pSqlItem, ISqlRequestPtr p_oRequest);
			std::string BuildDynamic(tinyxml2::XMLElement* p_pSqlItem, const std::map<std::string,
				std::string>& p_mapReqParams,std::string p_strPrepend = "");

			//Method to handle XML
			std::string GetText(tinyxml2::XMLElement*p_pElem, std::string p_strDefaultVlaue = "");
			std::string GetAttribute(tinyxml2::XMLElement*p_pElem, std::string p_strAttrName, std::string p_strDefaultVlaue = "");
			bool ReplacePlaceHolder(std::string& p_strSource, const std::map<std::string, std::string>& p_mapParams,
				bool isFieldString = true, bool isNotTrans =false, std::string p_strDefaults = "");

		private:
			Log::ILogPtr m_pLog;
			Config::IConfigPtr m_pConfigConfig;
			JsonParser::IJsonPtr	m_pJson;
			JsonParser::IJsonFactoryPtr m_pJsonFty;
			StringUtil::IStringUtilPtr	m_pString;
			Lock::ILockPtr m_lock;
			tinyxml2::XMLDocument m_oDocCfg;
			//map ,  key = id , value = ��Ӧ<sql>Ԫ�ص�tinyxml2::XMLElement*
			std::map<std::string, tinyxml2::XMLElement*> m_mapSqlElem;
			IResourceManagerPtr m_pResourceManager;
		};
	}
}
