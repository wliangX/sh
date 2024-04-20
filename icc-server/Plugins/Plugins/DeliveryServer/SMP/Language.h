#pragma once
#include <Json/IJsonFactory.h>

#include <Data/IBaseData.h>


#define LANGUAGE_KEY "LanguageInfoKey"

namespace ICC
{
	namespace Data
	{
		class CLanguageInfo
		{
		public:
			std::string ToJson(JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson)
				{
					return "";
				}
				p_pJson->SetNodeValue("/body/guid", m_strGuid);
				p_pJson->SetNodeValue("/body/code", m_strCode);
				p_pJson->SetNodeValue("/body/name", m_strName);
				p_pJson->SetNodeValue("/body/sort", m_strSort);
				return p_pJson->ToString();
			}

			bool Parse(std::string l_strJson, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson || !p_pJson->LoadJson(l_strJson))
				{
					return false;
				}
				m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				m_strCode = p_pJson->GetNodeValue("/body/code", "");
				m_strName = p_pJson->GetNodeValue("/body/name", "");
				m_strSort = p_pJson->GetNodeValue("/body/sort", "");
				return true;
			}

		public:
			std::string m_strGuid;
			std::string m_strCode;
			std::string m_strName;
			std::string m_strSort;
		};

		class CLanguage : public Data::IBaseData
		{
		public:			
			CLanguage(std::vector<Data::CLanguageInfo>& p_vecStaffInfo, IResourceManagerPtr p_pIResourceManager);
			virtual ~CLanguage();
		public:
			virtual bool LoadData();
			virtual int  ReceiveTotalDataSize();
			virtual void  AppendReceiveDataSize(int iReceiveSize);

			virtual bool SynUpdate();

			virtual bool SingleAdd();
			virtual bool SingleModify();
			virtual bool SingleDelete();			

			void         Append(std::vector<Data::CLanguageInfo>& p_vecInfo);

		private:
			bool _Delete();

			bool _HSet();
			void _HDel();

			bool _IsEqual(Data::CLanguageInfo p_old, Data::CLanguageInfo l_new);
			
			bool         _GetAllOldData(std::map<std::string, std::string>& mapOldDatas);			
			bool         _CompareData(std::map<std::string, std::string>& mapOldDatas, const std::vector<Data::CLanguageInfo>& vecNewDatas,
				std::vector<Data::CLanguageInfo>& addDatas, std::vector<Data::CLanguageInfo>& updateDatas, std::vector<Data::CLanguageInfo>& deleteDatas);
			bool         _UpdateData(const std::vector<Data::CLanguageInfo>& addDatas, const std::vector<Data::CLanguageInfo>& updateDatas, 
				const std::vector<Data::CLanguageInfo>& deleteDatas);	

			bool _Add(const std::vector<Data::CLanguageInfo>& vecDatas, const std::string& strTransGuid);
			bool _Update(const std::vector<Data::CLanguageInfo>& vecDatas, const std::string& strTransGuid);
			bool _Del(const std::vector<Data::CLanguageInfo>& vecDatas, const std::string& strTransGuid);
			       

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

		public:
			std::vector<Data::CLanguageInfo> m_vecLanguage;
		};
	}

};