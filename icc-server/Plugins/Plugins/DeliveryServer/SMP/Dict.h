#pragma once
#include <Json/IJsonFactory.h>

#include <Data/IBaseData.h>


#define DICT_KEY "DictInfoKey"
#define DICT_CODE_GUID_MAP "DictCodeGuidMap"
#define DICT_LEVEL "DictLevel:"

namespace ICC
{
	namespace Data
	{
		struct  CValue
		{
			std::string m_strGuid;
			std::string m_strValue;
			std::string m_strLangGuid;
		};

		class CDictInfo
		{
		public:
			std::string ToJson(JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson)
				{
					return "";
				}
				p_pJson->SetNodeValue("/guid", m_strGuid);
				p_pJson->SetNodeValue("/parent_key", m_strParentKey);
				p_pJson->SetNodeValue("/dict_key", m_strKey);
				p_pJson->SetNodeValue("/dict_type", m_strDictType);
				p_pJson->SetNodeValue("/dict_code", m_strDictCode);
				p_pJson->SetNodeValue("/shortcut", m_strShortCut);
				p_pJson->SetNodeValue("/sort", m_strSort);
				p_pJson->SetNodeValue("/value", m_strValue);
				p_pJson->SetNodeValue("/language", m_strLanguage);

				p_pJson->SetNodeValue("/union_key", m_strUnionKey);
				p_pJson->SetNodeValue("/parentUnion_key", m_strParentUnionKey);
				p_pJson->SetNodeValue("/dictValue_json", m_strDictValueJson);
				/*
				for (size_t i = 0; i < m_vecValue.size(); i++)
				{
					std::string l_strPath = "/value/";
					std::string l_strNum = std::to_string(i);
					p_pJson->SetNodeValue(l_strPath + l_strNum + "/guid", m_vecValue.at(i).m_strGuid);
					p_pJson->SetNodeValue(l_strPath + l_strNum + "/value", m_vecValue.at(i).m_strValue);
					p_pJson->SetNodeValue(l_strPath + l_strNum + "/lang_guid", m_vecValue.at(i).m_strLangGuid);
				}*/
				return p_pJson->ToString();
			}

			bool Parse(std::string l_strJson, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson || !p_pJson->LoadJson(l_strJson))
				{
					return false;
				}
				m_strGuid = p_pJson->GetNodeValue("/guid", "");
				m_strParentKey = p_pJson->GetNodeValue("/parent_key", "");
				m_strKey = p_pJson->GetNodeValue("/dict_key", "");
				m_strDictType = p_pJson->GetNodeValue("/dict_type", "");
				m_strDictCode = p_pJson->GetNodeValue("/dict_code", "");
				m_strShortCut = p_pJson->GetNodeValue("/shortcut", "");
				m_strSort = p_pJson->GetNodeValue("/sort", "");
				m_strValue = p_pJson->GetNodeValue("/value", "");
				m_strLanguage = p_pJson->GetNodeValue("/language", "");

				m_strUnionKey = p_pJson->GetNodeValue("/union_key", "");
				m_strParentUnionKey = p_pJson->GetNodeValue("/parentUnion_key", "");
				m_strDictValueJson = p_pJson->GetNodeValue("/dictValue_json", "");
/*
				int l_iCount = p_pJson->GetCount("/value");
				for (int i = 0; i < l_iCount; i++)
				{
					std::string l_strValueNum = std::to_string(i);
					CValue l_CValue;
					l_CValue.m_strValue = p_pJson->GetNodeValue("/value/" + l_strValueNum + "/value", "");
					l_CValue.m_strGuid = p_pJson->GetNodeValue("/value/" + l_strValueNum + "/guid", "");
					l_CValue.m_strLangGuid = p_pJson->GetNodeValue("/value/" + l_strValueNum + "/lang_guid", "");
					m_vecValue.push_back(l_CValue);
				}*/
				return true;
			}

		public:
			std::string m_strGuid;
			std::string m_strParentKey;
			std::string m_strKey;
			std::string m_strDictCode;
			std::string m_strShortCut;
			std::string m_strSort;
			std::string m_strValue;
			std::string m_strLanguage;
			std::string m_strDictType;
			std::vector<CValue> m_vecValue;

			std::string m_strDictValueJson;
			std::string m_strUnionKey;
			std::string m_strParentUnionKey;
		};

		class CDictionary : public Data::IBaseData
		{
		public:
			CDictionary(std::vector<Data::CDictInfo> p_vecDict, IResourceManagerPtr p_pIResourceManager);
			virtual~CDictionary();
		public:
			virtual bool LoadData();
			virtual int  ReceiveTotalDataSize();
			virtual void AppendReceiveDataSize(int iReceiveSize);

			virtual bool SynUpdate();

			virtual bool SingleAdd();
			virtual bool SingleModify();
			virtual bool SingleDelete();			

			void         Append(std::vector<Data::CDictInfo>& p_vecInfo);
			virtual void         SetTotalPackageCount(const std::string& strTotalPackageCount);
			virtual std::string  TotalPackageCount();

		private:
			bool _LoadDataEx(const std::vector<Data::CDictInfo>& vecDatas);
			bool _Delete();

			bool _HSet();
			void _HDel();

			bool _IsEqual(Data::CDictInfo p_oldDict, Data::CDictInfo l_newDict);
			bool _IsEqualEx(Data::CDictInfo p_oldDict, Data::CDictInfo l_newDict);
			
			bool         _GetAllOldData(std::map<std::string, std::string>& mapOldDatas);
			bool         _CompareData(std::map<std::string, std::string>& mapOldDatas, const std::vector<Data::CDictInfo>& vecNewDatas,
				std::vector<Data::CDictInfo>& addDatas, std::vector<Data::CDictInfo>& updateDatas, std::vector<Data::CDictInfo>& deleteDatas);
			bool         _UpdateData(const std::vector<Data::CDictInfo>& addDatas, const std::vector<Data::CDictInfo>& updateDatas,
				const std::vector<Data::CDictInfo>& deleteDatas);

			bool _Add(const std::vector<Data::CDictInfo>& vecDatas, const std::string& strTransGuid);
			bool _Update(const std::vector<Data::CDictInfo>& vecDatas, const std::string& strTransGuid);
			bool _Del(const std::vector<Data::CDictInfo>& vecDatas, const std::string& strTransGuid);

			bool _UpdateDataToRedis(const std::vector<Data::CDictInfo>& vecDatas);

			DataBase::SQLRequest _ConstructSQLStruct(Data::CDictInfo& p_ditcInfo, std::string p_strSqlID);
			virtual IResourceManagerPtr GetResourceManager()
			{
				return m_pResourceManager;
			}


		private:
			IResourceManagerPtr m_pResourceManager;
		private:
			Config::IConfigPtr m_pConfig;
			Log::ILogPtr m_pLog;
			StringUtil::IStringUtilPtr  m_pString;
			DateTime::IDateTimePtr    m_pDateTime;
			DataBase::IDBConnPtr m_pDBConn;
			JsonParser::IJsonFactoryPtr m_pJsonFty;
			Redis::IRedisClientPtr m_pRedisClient;

			int m_iTotalReceiveDataSize;
			std::string m_strTotalPackageCount;
		public:
			std::vector<Data::CDictInfo> m_vecDict;
		};
	}

};