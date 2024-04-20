#pragma once
#include <Json/IJsonFactory.h>

#include <Data/IBaseData.h>


#define Func "Func"

namespace ICC
{
	namespace Data
	{
		class CAuthInfo
		{
		public:
			std::string ToJson(JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson)
				{
					return "";
				}
				p_pJson->SetNodeValue("/guid", m_strGuid);
				p_pJson->SetNodeValue("/parent_guid", m_strParentGuid);
				p_pJson->SetNodeValue("/code", m_strCode);
				p_pJson->SetNodeValue("/name", m_strName);
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
				m_strParentGuid = p_pJson->GetNodeValue("/parent_guid", "");
				m_strCode = p_pJson->GetNodeValue("/code", "");
				m_strName = p_pJson->GetNodeValue("/name", "");
				m_strSort = p_pJson->GetNodeValue("/sort", "");

				return true;
			}

		public:
			std::string m_strGuid;
			std::string m_strParentGuid;
			std::string m_strCode;
			std::string m_strName;
			std::string m_strSort;
		};

		class CAuth : public Data::IBaseData
		{
		public:
			CAuth(std::vector<Data::CAuthInfo> p_vecStaffInfo, IResourceManagerPtr p_pIResourceManager);
			virtual ~CAuth();
		public:
			virtual bool LoadData();
			virtual int  ReceiveTotalDataSize();
			virtual void AppendReceiveDataSize(int iReceiveSize);

			virtual bool SynUpdate();

			virtual bool SingleAdd();
			virtual bool SingleModify();
			virtual bool SingleDelete();

			void         Append(std::vector<Data::CAuthInfo>& p_vecInfo);

		private:
			virtual bool _Delete();
			virtual bool _HSet();
			virtual void _HDel();


			bool _IsEqual(Data::CAuthInfo p_old, Data::CAuthInfo l_new);

			bool         _GetAllOldData(std::map<std::string, std::string>& mapOldDatas);
			bool         _CompareData(std::map<std::string, std::string>& mapOldDatas, const std::vector<Data::CAuthInfo>& vecNewDatas,
				std::vector<Data::CAuthInfo>& addDatas, std::vector<Data::CAuthInfo>& updateDatas, std::vector<Data::CAuthInfo>& deleteDatas);
			bool         _UpdateData(const std::vector<Data::CAuthInfo>& addDatas, const std::vector<Data::CAuthInfo>& updateDatas,
				const std::vector<Data::CAuthInfo>& deleteDatas);

			bool _Add(const std::vector<Data::CAuthInfo>& vecDatas, const std::string& strTransGuid);
			bool _Update(const std::vector<Data::CAuthInfo>& vecDatas, const std::string& strTransGuid);
			bool _Del(const std::vector<Data::CAuthInfo>& vecDatas, const std::string& strTransGuid);

			bool _SetDataToRedis(const std::vector<Data::CAuthInfo>& vecDatas);

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
			std::vector<Data::CAuthInfo> m_vecAuth;
		};
	}

};