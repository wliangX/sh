#pragma once
#include <Json/IJsonFactory.h>

#include <Data/IBaseData.h>


#define ROLE_INFO "RoleInfo"

namespace ICC
{
	namespace Data
	{
		class CRoleInfo
		{
		public:

			std::string ToJson(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				p_pJson->SetNodeValue("/guid", m_strGuid);
				p_pJson->SetNodeValue("/org_guid", m_strOrgGuid);
				p_pJson->SetNodeValue("/code", m_strCode);
				p_pJson->SetNodeValue("/name", m_strName);
				
				return p_pJson->ToString();
			}

			bool Parse(std::string l_strJson, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson || !p_pJson->LoadJson(l_strJson))
				{
					return false;
				}
				m_strGuid = p_pJson->GetNodeValue("/guid", "");
				m_strOrgGuid = p_pJson->GetNodeValue("/org_guid", "");
				m_strCode = p_pJson->GetNodeValue("/code", "");
				m_strName = p_pJson->GetNodeValue("/name", "");				
				
				return false;
			}

			bool operator <(const Data::CRoleInfo& l_roleInfo)
			{
				return m_strGuid < l_roleInfo.m_strGuid;
			}

			bool operator ==(const Data::CRoleInfo& l_roleInfo)
			{
				return m_strGuid == l_roleInfo.m_strGuid;
			}

		public:
			std::string m_strGuid;
			std::string m_strOrgGuid;
			std::string m_strCode;
			std::string m_strName;			
		};

		class CRole : public Data::IBaseData
		{
		public:
			CRole(std::vector<Data::CRoleInfo> p_vecStaffInfo, IResourceManagerPtr p_pIResourceManager);
			virtual ~CRole();
		public:
			virtual bool LoadData();
			virtual int  ReceiveTotalDataSize();
			virtual void AppendReceiveDataSize(int iReceiveSize);

			virtual bool SynUpdate();

			virtual bool SingleAdd();
			virtual bool SingleModify();
			virtual bool SingleDelete();

			

			void         Append(std::vector<Data::CRoleInfo>& p_vecInfo);

		private:

			bool _Delete();

			bool _HSet();
			void _HDel();

			bool _IsEqual(Data::CRoleInfo p_old, Data::CRoleInfo l_new);


			bool         _GetAllOldData(std::map<std::string, std::string>& mapOldDatas);
			bool         _CompareData(std::map<std::string, std::string>& mapOldDatas, const std::vector<Data::CRoleInfo>& vecNewDatas,
				std::vector<Data::CRoleInfo>& addDatas, std::vector<Data::CRoleInfo>& updateDatas, std::vector<Data::CRoleInfo>& deleteDatas);
			bool         _UpdateData(const std::vector<Data::CRoleInfo>& addDatas, const std::vector<Data::CRoleInfo>& updateDatas,
				const std::vector<Data::CRoleInfo>& deleteDatas);

			bool _Add(const std::vector<Data::CRoleInfo>& vecDatas, const std::string& strTransGuid);
			bool _Update(const std::vector<Data::CRoleInfo>& vecDatas, const std::string& strTransGuid);
			bool _Del(const std::vector<Data::CRoleInfo>& vecDatas, const std::string& strTransGuid);

			bool _UpdateDataToRedis(const std::vector<Data::CRoleInfo>& vecDatas);

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
			std::vector<Data::CRoleInfo> m_vecRole;
		};
	}

};