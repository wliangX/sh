#pragma once
#include <Json/IJsonFactory.h>

#include <Data/IBaseData.h>


#define USER_INFO "UserInfo"
#define BIND_INFO "BindInfo"

namespace ICC
{
	namespace Data
	{
		class CUserInfo
		{
		public:
			std::string ToJson(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				p_pJson->SetNodeValue("/guid", m_strGuid);	
				p_pJson->SetNodeValue("/code", m_strCode);
				p_pJson->SetNodeValue("/name", m_strName);
				p_pJson->SetNodeValue("/pwd", m_strPwd);
				p_pJson->SetNodeValue("/remark", m_strRemark);				
				p_pJson->SetNodeValue("/online_status", m_strOnlineStatus);
				p_pJson->SetNodeValue("/login_time", m_strLoginTime);
				p_pJson->SetNodeValue("/user_type", m_strUserType);
				p_pJson->SetNodeValue("/staff_guid", m_strStaffGuid);
				p_pJson->SetNodeValue("/org_guid", m_strOrgGuid);
				p_pJson->SetNodeValue("/create_user", m_strCreateUser);
				p_pJson->SetNodeValue("/create_time", m_strCreateTime);
				p_pJson->SetNodeValue("/update_user", m_strUpdateUser);
				p_pJson->SetNodeValue("/update_time", m_strUpdateTime);

				return p_pJson->ToString();
			}

			bool Parse(std::string l_strJson, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson || !p_pJson->LoadJson(l_strJson))
				{
					return false;
				}
				m_strGuid = p_pJson->GetNodeValue("/guid", "");		
				m_strCode = p_pJson->GetNodeValue("/code", "");
				m_strName = p_pJson->GetNodeValue("/name", "");
				m_strPwd = p_pJson->GetNodeValue("/pwd", "");
				m_strRemark = p_pJson->GetNodeValue("/remark", "");
				m_strOnlineStatus = p_pJson->GetNodeValue("/online_status", "");
				m_strLoginTime = p_pJson->GetNodeValue("/login_time", "");				
				m_strUserType = p_pJson->GetNodeValue("/user_type", "");
				m_strStaffGuid = p_pJson->GetNodeValue("/staff_guid", "");
				m_strOrgGuid = p_pJson->GetNodeValue("/org_guid", "");
				m_strCreateUser = p_pJson->GetNodeValue("/create_user", "");
				m_strCreateTime = p_pJson->GetNodeValue("/create_time", "");
				m_strUpdateUser = p_pJson->GetNodeValue("/update_user", "");
				m_strUpdateTime = p_pJson->GetNodeValue("/update_time", "");
				return true;
			}

		public:
			std::string m_strGuid;	
			std::string m_strCode;
			std::string m_strName;
			std::string m_strPwd;
			std::string m_strOnlineStatus;
			std::string m_strLoginTime;
			std::string m_strRemark;
			std::string m_strUserType;
			std::string m_strStaffGuid;
			std::string m_strOrgGuid;			
			std::string m_strCreateUser;
			std::string m_strCreateTime;
			std::string m_strUpdateUser;
			std::string m_strUpdateTime;
		};

		class CUser : public Data::IBaseData
		{
		public:
			CUser(std::vector<Data::CUserInfo> p_vecUser, IResourceManagerPtr p_pIResourceManager);
			virtual~ CUser();
		public:
			virtual bool LoadData();
			virtual int  ReceiveTotalDataSize();
			virtual void  AppendReceiveDataSize(int iReceiveSize);

			virtual bool SynUpdate();

			virtual bool SingleAdd();
			virtual bool SingleModify();
			virtual bool SingleDelete();

			

			void         Append(std::vector<Data::CUserInfo>& p_vecInfo);

		private:
			bool _Delete();

			bool _HSet();
			void _HDel();
			bool _IsEqual(Data::CUserInfo p_old, Data::CUserInfo l_new);

			DataBase::SQLRequest _ConstructSQLStruct(Data::CUserInfo& p_userInfo, std::string p_strSqlID);

			bool _DeleteBindInfo(std::string p_strUserGuid, const std::string& strTransGuid);
			bool _GetBindStaffGuid(std::string p_strFromGuid, std::string& p_strToGuid);
			bool _GetBindDeptGuid(std::string p_strFromGuid, std::string& p_strToGuid);
			bool _GetBindRoleGuid(std::string p_strFromGuid, std::vector<std::string>& l_vecBindGuid);

			bool         _GetAllOldData(std::map<std::string, std::string>& mapOldDatas);
			bool         _CompareData(std::map<std::string, std::string>& mapOldDatas, const std::vector<Data::CUserInfo>& vecNewDatas,
				std::vector<Data::CUserInfo>& addDatas, std::vector<Data::CUserInfo>& updateDatas, std::vector<Data::CUserInfo>& deleteDatas);
			bool         _UpdateData(std::vector<Data::CUserInfo>& addDatas, std::vector<Data::CUserInfo>& updateDatas,
				std::vector<Data::CUserInfo>& deleteDatas);

			bool _Add(std::vector<Data::CUserInfo>& vecDatas, std::string& strTransGuid);
			bool _Update(std::vector<Data::CUserInfo>& vecDatas, std::string& strTransGuid);
			bool _Del(std::vector<Data::CUserInfo>& vecDatas, std::string& strTransGuid);

			bool _UpdateDataToRedis(const std::vector<Data::CUserInfo>& vecDatas);


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
			std::vector<Data::CUserInfo> m_vecUser;
		};
	}

};