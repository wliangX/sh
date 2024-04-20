#pragma once
#include <Json/IJsonFactory.h>

#include <Data/IBaseData.h>

#define BIND_INFO "BindInfo"

namespace ICC
{
	namespace Data
	{

		class CBindInfo
		{
		public:
			std::string ToJson(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				p_pJson->SetNodeValue("/guid", m_strGuid);
				p_pJson->SetNodeValue("/type", m_strType);
				p_pJson->SetNodeValue("/from_guid", m_strFromGuid);
				p_pJson->SetNodeValue("/to_guid", m_strToGuid);
				p_pJson->SetNodeValue("/sort", m_strSort);

				return p_pJson->ToString();
			}

			bool Parse(std::string l_strJson, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson || !p_pJson->LoadJson(l_strJson))
				{
					return false;
				}
				m_strGuid = p_pJson->GetNodeValue("/guid", "");
				m_strType = p_pJson->GetNodeValue("/type", "");
				m_strFromGuid = p_pJson->GetNodeValue("/from_guid", "");
				m_strToGuid = p_pJson->GetNodeValue("/to_guid", "");
				m_strSort = p_pJson->GetNodeValue("/sort", "");
				return true;
			}

		public:
			std::string m_strGuid;
			std::string m_strType;
			std::string m_strFromGuid;
			std::string m_strToGuid;
			std::string m_strSort;
		};

		class CBind : public Data::IBaseData
		{
		public:
			CBind(std::vector<Data::CBindInfo> p_vecBind, IResourceManagerPtr p_pIResourceManager, const std::string& strType);
			virtual~CBind();
		public:
			virtual bool LoadData();
			virtual int  ReceiveTotalDataSize();
			virtual void  AppendReceiveDataSize(int iReceiveSize);

			virtual bool SynUpdate();

			virtual bool SingleAdd();
			virtual bool SingleModify();
			virtual bool SingleDelete();

			

			void GetBindInfo(std::vector<Data::CBindInfo>& p_vecBind);

			void         Append(std::vector<Data::CBindInfo>& p_vecInfo);

		private:

			bool _Delete();

			bool _HSet();
			void _HDel();

			bool _IsEqual(Data::CBindInfo p_old, Data::CBindInfo l_new);

			virtual IResourceManagerPtr GetResourceManager()
			{
				return m_pResourceManager;
			}


			bool         _GetAllOldData(std::map<std::string, std::string>& mapOldDatas);
			bool         _CompareData(std::map<std::string, std::string>& mapOldDatas, const std::vector<Data::CBindInfo>& vecNewDatas,
				std::vector<Data::CBindInfo>& addDatas, std::vector<Data::CBindInfo>& updateDatas, std::vector<Data::CBindInfo>& deleteDatas);
			bool         _UpdateData(const std::vector<Data::CBindInfo>& addDatas, const std::vector<Data::CBindInfo>& updateDatas,
				const std::vector<Data::CBindInfo>& deleteDatas);

			bool _Add(const std::vector<Data::CBindInfo>& vecDatas, const std::string& strTransGuid);
			bool _Update(const std::vector<Data::CBindInfo>& vecDatas, const std::string& strTransGuid);
			bool _Del(const std::vector<Data::CBindInfo>& vecDatas, const std::string& strTransGuid);

			bool         _GetAllData(std::vector<Data::CBindInfo>& vecDatas);

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

			std::string m_strType;
			int m_iTotalReceiveDataSize;
		public:
			std::vector<Data::CBindInfo> m_vecBind;
			std::vector<Data::CBindInfo> m_vecTempBind;
			//É¾³ýµÄGUIDÁÐ±í
			std::vector<std::string> m_vecDelete;
		};
	}

};