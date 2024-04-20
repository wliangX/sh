#include "Boost.h"
#include "Role.h"

using namespace ICC;
using namespace Data;

CRole::CRole(std::vector<Data::CRoleInfo> p_vecRoleInfo, IResourceManagerPtr p_pIResourceManager) :
m_vecRole(p_vecRoleInfo),
m_pResourceManager(p_pIResourceManager)
{
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pJsonFty = ICCGetIJsonFactory();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);

	m_iTotalReceiveDataSize = 0;
}

CRole::~CRole()
{

}

int CRole::ReceiveTotalDataSize()
{
	return (int)m_vecRole.size();
}

void CRole::AppendReceiveDataSize(int iReceiveSize)
{
	m_iTotalReceiveDataSize += iReceiveSize;
}

void CRole::Append(std::vector<Data::CRoleInfo>& p_vecInfo)
{
	int iCount = p_vecInfo.size();
	for (int i = 0; i < iCount; ++i)
	{
		m_vecRole.push_back(p_vecInfo[i]);
	}
}

bool CRole::SynUpdate()
{
	/*if (m_vecRole.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no role data need update!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "begin update role data, size[%d]", m_vecRole.size());

	std::map<std::string, std::string> tmp_mapInfo;
	for (Data::CRoleInfo var : m_vecRole)
	{
		tmp_mapInfo[var.m_strGuid] = var.ToJson(m_pJsonFty->CreateJson());
	}

	if (!m_pRedisClient->HMSet(ROLE_INFO, tmp_mapInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "set role key failed!!");
		return false;
	}
	return true;*/

	if (m_vecRole.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no role data need update!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "begin update role data, size[%d]", m_vecRole.size());

	std::map<std::string, std::string> mapOldDatas;
	if (!_GetAllOldData(mapOldDatas))
	{
		ICC_LOG_ERROR(m_pLog, "get all role old datas failed!!");
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "get all role old datas success! size[%d] ", mapOldDatas.size());

	std::vector<Data::CRoleInfo> addDatas;
	std::vector<Data::CRoleInfo> updateDatas;
	std::vector<Data::CRoleInfo> deleteDatas;
	_CompareData(mapOldDatas, m_vecRole, addDatas, updateDatas, deleteDatas);

	if (!_UpdateData(addDatas, updateDatas, deleteDatas))
	{
		//LoadData();
		return false;
	}

	return true;	
}


bool CRole::SingleAdd()
{
	if (m_vecRole.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no role data need add!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "will add role data size[%d]!!", m_vecRole.size());

	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	std::string l_strGuid = m_pDBConn->BeginTransaction();

	if (!_Add(m_vecRole, l_strGuid))
	{
		return false;
	}

	m_pDBConn->Commit(l_strGuid);

	ICC_LOG_DEBUG(m_pLog, "add role data complete! ");

	return true;
}

bool CRole::SingleModify()
{
	if (m_vecRole.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no role data need update!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "will update role data size[%d]!!", m_vecRole.size());
	
	std::string l_strGuid = m_pDBConn->BeginTransaction();	

	if (!_Update(m_vecRole, l_strGuid))
	{
		return false;
	}		

	m_pDBConn->Commit(l_strGuid);

	ICC_LOG_DEBUG(m_pLog, "update role data complete! ");

	return true;
}

bool CRole::SingleDelete()
{
	if (m_vecRole.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no role data need delete!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "will delete role data size[%d]!!", m_vecRole.size());
	
	std::string l_strGuid = m_pDBConn->BeginTransaction();	

	if (!_Del(m_vecRole, l_strGuid))
	{
		return false;
	}

	m_pDBConn->Commit(l_strGuid);

	ICC_LOG_DEBUG(m_pLog, "delete role data complete! ");

	return true;
}



bool CRole::LoadData()
{
	ICC_LOG_DEBUG(m_pLog, "load role to redis Begin");

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_role";
	ICC_LOG_DEBUG(m_pLog, "LoadRoleInfo Begin");
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, " ExecQuery Error Load RoleInfo Error Message :[%s]",
			l_result->GetErrorMsg().c_str());
		return false;
	}

	std::vector<CRoleInfo> vecDatas;
	while (l_result->Next())
	{
		CRoleInfo l_oRole;
		l_oRole.m_strGuid = l_result->GetValue("guid");
		l_oRole.m_strCode = l_result->GetValue("code");
		l_oRole.m_strOrgGuid = l_result->GetValue("org_guid");
		l_oRole.m_strName = l_result->GetValue("name");
		vecDatas.push_back(l_oRole);
	}

	int iDelCount = m_pRedisClient->Del(ROLE_INFO);
	ICC_LOG_DEBUG(m_pLog, "del role from redis, count %d", iDelCount);

	if (!_UpdateDataToRedis(vecDatas))
	{
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "load role to redis end, count %d", vecDatas.size());

	return true;

	

	/*std::map<std::string, std::string> mapTmps;
	if (!_GetAllOldData(mapTmps))
	{
		ICC_LOG_ERROR(m_pLog, "get role from redis failed!!");
		return false;
	}

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_role";
	ICC_LOG_DEBUG(m_pLog, "LoadRoleInfo Begin");
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, " ExecQuery Error Load RoleInfo Error Message :[%s]",
			l_result->GetErrorMsg().c_str());
		return false;
	}
	while (l_result->Next())
	{
		CRoleInfo l_oRole;
		l_oRole.m_strGuid = l_result->GetValue("guid");
		l_oRole.m_strCode = l_result->GetValue("code");
		l_oRole.m_strParentGuid = l_result->GetValue("parent_guid");
		l_oRole.m_strName = l_result->GetValue("name");
		l_oRole.m_strSort = l_result->GetValue("sort");


		bool bNeedWrite = false;

		std::map<std::string, std::string>::iterator itr;
		itr = mapTmps.find(l_oRole.m_strGuid);
		if (itr != mapTmps.end())
		{
			Data::CRoleInfo oldInfo;
			if (!oldInfo.Parse(itr->second, m_pJsonFty->CreateJson()))
			{
				bNeedWrite = true;
			}
			else
			{
				if (!_IsEqual(oldInfo, l_oRole))
				{
					bNeedWrite = true;
				}
			}

			mapTmps.erase(itr);
		}
		else
		{
			bNeedWrite = true;
		}

		if (bNeedWrite)
		{
			std::string l_strVal = l_oRole.ToJson(m_pJsonFty->CreateJson());
			if (!m_pRedisClient->HSet(ROLE_INFO, l_oRole.m_strGuid, l_strVal))
			{
				ICC_LOG_ERROR(m_pLog, "LoadRoleInfo In Redis Failed Role Guid[%s]", l_oRole.m_strGuid.c_str());
				return false;
			}
		}		
	}

	std::map<std::string, std::string>::const_iterator itr_const;
	for (itr_const = mapTmps.begin(); itr_const != mapTmps.end(); ++itr_const)
	{
		bool bSet = m_pRedisClient->HDel(ROLE_INFO, itr_const->first);
		if (!bSet)
		{
			ICC_LOG_ERROR(m_pLog, "delete language info from redis failed! [%s]", itr_const->first.c_str());
			return false;
		}
	}

	ICC_LOG_DEBUG(m_pLog, "LoadRoleInfo Success,Role Size[%d]", l_result->RecordSize());
	return true;*/
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
bool CRole::_GetAllOldData(std::map<std::string, std::string>& mapOldDatas)
{
	if (m_pRedisClient->HGetAllEx(ROLE_INFO, mapOldDatas))
	{
		return true;
	}

	ICC_LOG_ERROR(m_pLog, "get all old role datas failed!!!");

	return false;
}

bool CRole::_CompareData(std::map<std::string, std::string>& mapOldDatas, const std::vector<Data::CRoleInfo>& vecNewDatas,
	std::vector<Data::CRoleInfo>& addDatas, std::vector<Data::CRoleInfo>& updateDatas, std::vector<Data::CRoleInfo>& deleteDatas)
{
	int iNewDataCount = vecNewDatas.size();
	for (int i = 0; i < iNewDataCount; ++i)
	{
		std::map<std::string, std::string>::iterator itr = mapOldDatas.find(vecNewDatas[i].m_strGuid);
		if (itr != mapOldDatas.end())
		{
			Data::CRoleInfo oldInfo;
			if (!oldInfo.Parse(itr->second, m_pJsonFty->CreateJson()))
			{
				return false;
			}

			mapOldDatas.erase(itr);

			if (_IsEqual(oldInfo, vecNewDatas[i]))
			{

				continue;
			}
			else
			{
				updateDatas.push_back(vecNewDatas[i]);
			}
		}
		else
		{
			addDatas.push_back(vecNewDatas[i]);
		}
	}

	std::map<std::string, std::string>::const_iterator itr_const;
	for (itr_const = mapOldDatas.begin(); itr_const != mapOldDatas.end(); ++itr_const)
	{
		Data::CRoleInfo oldInfo;
		if (!oldInfo.Parse(itr_const->second, m_pJsonFty->CreateJson()))
		{
			return false;
		}

		deleteDatas.push_back(oldInfo);
	}

	return true;
}

bool CRole::_UpdateDataToRedis(const std::vector<Data::CRoleInfo>& vecDatas)
{
	std::map<std::string, std::string> mapDatas;
	for (Data::CRoleInfo l_roleInfo : vecDatas)
	{
		std::string strTmp = l_roleInfo.ToJson(m_pJsonFty->CreateJson());
		mapDatas.insert(std::make_pair(l_roleInfo.m_strGuid, strTmp));
	}

	if (!mapDatas.empty())
	{
		if (!m_pRedisClient->HMSet(ROLE_INFO, mapDatas))
		{
			return false;
		}
	}	

	ICC_LOG_DEBUG(m_pLog, "add role data to redis success! content count[%d]", mapDatas.size());

	return true;
}

bool CRole::_Add(const std::vector<Data::CRoleInfo>& vecDatas, const std::string& strTransGuid)
{
	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	std::map<std::string, std::string> mapDatas;
	for (Data::CRoleInfo l_roleInfo : vecDatas)
	{
		if (mapDatas.find(l_roleInfo.m_strGuid) != mapDatas.end())
		{
			ICC_LOG_ERROR(m_pLog, "same role data ! guid[%s]", l_roleInfo.m_strGuid.c_str());
			continue;;
		}

		DataBase::SQLRequest l_oSetSQLReq;
		l_oSetSQLReq.sql_id = "insert_icc_t_role";
		l_oSetSQLReq.param["guid"] = l_roleInfo.m_strGuid;
		l_oSetSQLReq.param["org_guid"] = l_roleInfo.m_strOrgGuid;
		l_oSetSQLReq.param["name"] = l_roleInfo.m_strName;
		l_oSetSQLReq.param["code"] = l_roleInfo.m_strCode;
		l_oSetSQLReq.param["create_user"] = "smp_gateway";
		l_oSetSQLReq.param["create_time"] = l_strCurrentTime;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSetSQLReq, false, strTransGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "add role data to db failed! sql[%s]", l_pRSet->GetSQL().c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "add role data to db success!!! sql[%s]", l_pRSet->GetSQL().c_str());

		mapDatas.insert(std::make_pair(l_roleInfo.m_strGuid, l_roleInfo.m_strGuid));

		/*std::string strTmp = l_roleInfo.ToJson(m_pJsonFty->CreateJson());
		if (!m_pRedisClient->HSet(ROLE_INFO, l_roleInfo.m_strGuid, strTmp))
		{
			ICC_LOG_ERROR(m_pLog, "add role data to redis failed! content[%s]", strTmp.c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "add role data to redis success! content[%s]", strTmp.c_str());*/
	}

	if (!_UpdateDataToRedis(vecDatas))
	{
		m_pDBConn->Rollback(strTransGuid);
		return false;
	}

	return true;
}

bool CRole::_Update(const std::vector<Data::CRoleInfo>& vecDatas, const std::string& strTransGuid)
{
	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());

	for (Data::CRoleInfo l_roleInfo : vecDatas)
	{
		DataBase::SQLRequest l_oSetSQLReq;
		l_oSetSQLReq.sql_id = "update_icc_t_role";
		l_oSetSQLReq.set["org_guid"] = l_roleInfo.m_strOrgGuid;
		l_oSetSQLReq.set["name"] = l_roleInfo.m_strName;
		l_oSetSQLReq.set["code"] = l_roleInfo.m_strCode;
		l_oSetSQLReq.set["update_user"] = "smp_gateway";
		l_oSetSQLReq.set["update_time"] = l_strCurrentTime;
		l_oSetSQLReq.param["guid"] = l_roleInfo.m_strGuid;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSetSQLReq, false, strTransGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "update role data to db failed! sql[%s]", l_pRSet->GetSQL().c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "update role data to db success!!! sql[%s]", l_pRSet->GetSQL().c_str());

		/*std::string strTmp = l_roleInfo.ToJson(m_pJsonFty->CreateJson());
		if (!m_pRedisClient->HSet(ROLE_INFO, l_roleInfo.m_strGuid, strTmp))
		{
			ICC_LOG_ERROR(m_pLog, "update role data to redis failed! content[%s]", strTmp.c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "update role data to redis success! content[%s]", strTmp.c_str());*/
	}

	if (!_UpdateDataToRedis(vecDatas))
	{
		m_pDBConn->Rollback(strTransGuid);
		return false;
	}

	return true;
}

bool CRole::_Del(const std::vector<Data::CRoleInfo>& vecDatas, const std::string& strTransGuid)
{
	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());

	for (Data::CRoleInfo l_roleInfo : vecDatas)
	{
		DataBase::SQLRequest l_oDeleteSQLReq;
		l_oDeleteSQLReq.sql_id = "delete_icc_t_role";
		l_oDeleteSQLReq.param["guid"] = l_roleInfo.m_strGuid;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oDeleteSQLReq, false, strTransGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "delete role data to db failed! sql[%s]", l_pRSet->GetSQL().c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "delete role data to db success!!! sql[%s]", l_pRSet->GetSQL().c_str());

		if (!m_pRedisClient->HDel(ROLE_INFO, l_roleInfo.m_strGuid))
		{
			ICC_LOG_ERROR(m_pLog, "delete role data from redis failed! guid[%s]", l_roleInfo.m_strGuid.c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "delete role data from redis success! guid[%s]", l_roleInfo.m_strGuid.c_str());
	}

	return true;
}

bool CRole::_UpdateData(const std::vector<Data::CRoleInfo>& addDatas, const std::vector<Data::CRoleInfo>& updateDatas, const std::vector<Data::CRoleInfo>& deleteDatas)
{
	ICC_LOG_DEBUG(m_pLog, "update role data begin! addDatas[%d], updateDatas[%d], deleteDatas[%d]",
		addDatas.size(), updateDatas.size(), deleteDatas.size());

	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	std::string l_strGuid = m_pDBConn->BeginTransaction();

	if (!_Add(addDatas, l_strGuid))
	{
		return false;
	}

	if (!_Update(updateDatas, l_strGuid))
	{
		return false;
	}

	if (!_Del(deleteDatas, l_strGuid))
	{
		return false;
	}

	m_pDBConn->Commit(l_strGuid);

	ICC_LOG_DEBUG(m_pLog, "update role data complete! ");

	return true;
}

bool CRole::_Delete()
{
	if (m_vecRole.empty())
	{
		return true;
	}

	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	std::string l_strGuid = m_pDBConn->BeginTransaction();
	for (Data::CRoleInfo var : m_vecRole)
	{
		DataBase::SQLRequest l_oDeleteSQLReq;
		l_oDeleteSQLReq.sql_id = "delete_icc_t_role";
		l_oDeleteSQLReq.param["guid"] = var.m_strGuid;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oDeleteSQLReq, false, l_strGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "delete role info error, rollback: [%s]", l_pRSet->GetErrorMsg().c_str());
			m_pDBConn->Rollback(l_strGuid);
			return false;
		}
	}

	m_pDBConn->Commit(l_strGuid);
	return true;
}

bool CRole::_HSet()
{
	if (m_vecRole.empty())
	{
		return true;
	}

	std::map<std::string, std::string> tmp_mapInfo;
	for (Data::CRoleInfo var : m_vecRole)
	{
		tmp_mapInfo[var.m_strGuid] = var.ToJson(m_pJsonFty->CreateJson());
	}

	return m_pRedisClient->HMSet(ROLE_INFO, tmp_mapInfo);
}

void CRole::_HDel()
{
	for (Data::CRoleInfo var : m_vecRole)
	{
		m_pRedisClient->HDel(ROLE_INFO, var.m_strGuid);
	}
}

bool CRole::_IsEqual(Data::CRoleInfo p_old, Data::CRoleInfo l_new)
{
	if (p_old.m_strGuid == l_new.m_strGuid &&
		p_old.m_strCode == l_new.m_strCode &&
		p_old.m_strOrgGuid == l_new.m_strOrgGuid &&
		p_old.m_strName == l_new.m_strName)
	{

		return true;
	}
	return false;
}
