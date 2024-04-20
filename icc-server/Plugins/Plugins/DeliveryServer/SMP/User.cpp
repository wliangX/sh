#include "Boost.h"
#include "User.h"

using namespace ICC;
using namespace Data;

CUser::CUser(std::vector<Data::CUserInfo> p_vecUser, IResourceManagerPtr p_pIResourceManager):
m_vecUser(p_vecUser),
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

CUser::~CUser()
{

}

int CUser::ReceiveTotalDataSize()
{
	return (int)m_vecUser.size();
}

void CUser::AppendReceiveDataSize(int iReceiveSize)
{
	m_iTotalReceiveDataSize += iReceiveSize;
}

void CUser::Append(std::vector<Data::CUserInfo>& p_vecInfo)
{
	int iCount = p_vecInfo.size();
	for (int i = 0; i < iCount; ++i)
	{
		m_vecUser.push_back(p_vecInfo[i]);
	}
}

bool CUser::SynUpdate()
{
	/*if (m_vecUser.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no user data need update!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "begin update user data, size[%d]", m_vecUser.size());
	
	std::map<std::string, std::string> tmp_mapInfo;
	for (Data::CUserInfo userInfo : m_vecUser)
	{
		tmp_mapInfo[userInfo.m_strName] = userInfo.ToJson(m_pJsonFty->CreateJson());
	}

	if (!m_pRedisClient->HMSet(USER_INFO, tmp_mapInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "set user key failed!!");
		return false;
	}

	return true;*/

	if (m_vecUser.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no user data need update!!");
		return true;
	}

	std::map<std::string, std::string> mapOldDatas;
	if (!_GetAllOldData(mapOldDatas))
	{
		ICC_LOG_ERROR(m_pLog, "get all user old datas failed!!");
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "get all user old datas success! size[%d] ", mapOldDatas.size());

	std::vector<Data::CUserInfo> addDatas;
	std::vector<Data::CUserInfo> updateDatas;
	std::vector<Data::CUserInfo> deleteDatas;
	_CompareData(mapOldDatas, m_vecUser, addDatas, updateDatas, deleteDatas);

	while (true)
	{
		if (!_UpdateData(addDatas, updateDatas, deleteDatas))
		{
		//LoadData();
			continue;
		}

		ICC_LOG_DEBUG(m_pLog, "addDatas size[%d],updateDatas size[%d], deleteDatas   size[%d]", addDatas.size(), updateDatas.size(), deleteDatas.size());
		break;
	}

	return true;	
}


bool CUser::SingleAdd()
{
	if (m_vecUser.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no user data need add!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "will add user data size[%d]!!", m_vecUser.size());

	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	std::string l_strGuid = m_pDBConn->BeginTransaction();

	if (!_Add(m_vecUser, l_strGuid))
	{
		return false;
	}		

	m_pDBConn->Commit(l_strGuid);

	ICC_LOG_DEBUG(m_pLog, "add user data complete! ");

	return true;
}

bool CUser::SingleModify()
{
	if (m_vecUser.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no user data need update!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "will update user data size[%d]!!", m_vecUser.size());

	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	std::string l_strGuid = m_pDBConn->BeginTransaction();	

	if (!_Update(m_vecUser, l_strGuid))
	{
		return false;
	}		

	m_pDBConn->Commit(l_strGuid);

	ICC_LOG_DEBUG(m_pLog, "update user data complete! ");

	return true;
}

bool CUser::SingleDelete()
{
	if (m_vecUser.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no user data need delete!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "will delete user data size[%d]!!", m_vecUser.size());

	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	std::string l_strGuid = m_pDBConn->BeginTransaction();	

	if (!_Del(m_vecUser, l_strGuid))
	{
		return false;
	}

	m_pDBConn->Commit(l_strGuid);

	ICC_LOG_DEBUG(m_pLog, "delete user data complete! ");

	return true;
}

bool CUser::LoadData()
{
	ICC_LOG_DEBUG(m_pLog, "load user from redis begin");

	CUserInfo l_tAccountInfo;	
	DataBase::SQLRequest l_oQuery = _ConstructSQLStruct(l_tAccountInfo, "select_icc_t_user");
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oQuery);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "query db failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	std::vector<CUserInfo> vecDatas;

	for (unsigned int i = 0; i < l_pResult->RecordSize(); i++)
	{
		CUserInfo l_tAccountInfo;
		l_tAccountInfo.m_strGuid = l_pResult->GetValue(i, "guid");
		l_tAccountInfo.m_strCode = l_pResult->GetValue(i, "code");
		l_tAccountInfo.m_strName = l_pResult->GetValue(i, "name");
		l_tAccountInfo.m_strPwd = l_pResult->GetValue(i, "pwd");		
		l_tAccountInfo.m_strCreateUser = l_pResult->GetValue(i, "create_user");
		l_tAccountInfo.m_strCreateTime = l_pResult->GetValue(i, "create_time");
		l_tAccountInfo.m_strUpdateUser = l_pResult->GetValue(i, "update_user");
		l_tAccountInfo.m_strUpdateTime = l_pResult->GetValue(i, "update_time");
		l_tAccountInfo.m_strRemark = l_pResult->GetValue(i, "remark");
		l_tAccountInfo.m_strUserType = l_pResult->GetValue(i, "remark");
		l_tAccountInfo.m_strStaffGuid = l_pResult->GetValue(i, "remark");
		l_tAccountInfo.m_strOrgGuid = l_pResult->GetValue(i, "remark");

		vecDatas.push_back(l_tAccountInfo);
	}

	int iRs = m_pRedisClient->Del(USER_INFO);
	ICC_LOG_ERROR(m_pLog, "delete user from redis count:[%d]", iRs);

	if (!_UpdateDataToRedis(vecDatas))
	{
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "load user to redis end [count:%d]", l_pResult->RecordSize());

	return true;

	/*ICC_LOG_DEBUG(m_pLog, "load user from redis Begin");

	std::map<std::string, std::string> mapTmps;
	if (!_GetAllOldData(mapTmps))
	{
		ICC_LOG_ERROR(m_pLog, "get user from redis failed!!");
		return false;
	}


	CUserInfo l_tAccountInfo;
	l_tAccountInfo.m_strIsDelete = "false";
	DataBase::SQLRequest l_oQuery = _ConstructSQLStruct(l_tAccountInfo, "select_icc_t_user");
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oQuery);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "query db failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}*/

	/*int iRs = m_pRedisClient->Del(USER_INFO);
	if (iRs < 0)
	{
		ICC_LOG_ERROR(m_pLog, "del user info from redis failed!!");
		return false;
	}*/

	//初始化用户信息缓存
	//for (unsigned int i = 0; i < l_pResult->RecordSize(); i++)
	//{
	//	CUserInfo l_tAccountInfo;
	//	l_tAccountInfo.m_strGuid = l_pResult->GetValue(i, "guid");
	//	l_tAccountInfo.m_strCode = l_pResult->GetValue(i, "code");
	//	l_tAccountInfo.m_strName = l_pResult->GetValue(i, "name");
	//	l_tAccountInfo.m_strPwd = l_pResult->GetValue(i, "pwd");
	//	l_tAccountInfo.m_strIsDelete = l_pResult->GetValue(i, "is_delete");
	//	l_tAccountInfo.m_strLevel = l_pResult->GetValue(i, "level");
	//	l_tAccountInfo.m_strSort = l_pResult->GetValue(i, "sort");
	//	l_tAccountInfo.m_strShortCut = l_pResult->GetValue(i, "shortcut");
	//	l_tAccountInfo.m_strCreateUser = l_pResult->GetValue(i, "create_user");
	//	l_tAccountInfo.m_strCreateTime = l_pResult->GetValue(i, "create_time");
	//	l_tAccountInfo.m_strUpdateUser = l_pResult->GetValue(i, "update_user");
	//	l_tAccountInfo.m_strUpdateTime = l_pResult->GetValue(i, "update_time");
	//	l_tAccountInfo.m_strRemark = l_pResult->GetValue(i, "remark");


	//	bool bNeedWrite = false;

	//	std::map<std::string, std::string>::iterator itr;
	//	itr = mapTmps.find(l_tAccountInfo.m_strGuid);
	//	if (itr != mapTmps.end())
	//	{
	//		Data::CUserInfo oldInfo;
	//		if (!oldInfo.Parse(itr->second, m_pJsonFty->CreateJson()))
	//		{
	//			bNeedWrite = true;
	//		}
	//		else
	//		{
	//			if (!_IsEqual(oldInfo, l_tAccountInfo))
	//			{
	//				bNeedWrite = true;
	//			}
	//		}

	//		mapTmps.erase(itr);
	//	}
	//	else
	//	{
	//		bNeedWrite = true;
	//	}

	//	if (bNeedWrite)
	//	{
	//		//更新Redis缓存
	//		std::string l_strVal = l_tAccountInfo.ToJson(m_pJsonFty->CreateJson());
	//		bool bSet = m_pRedisClient->HSet(USER_INFO, l_tAccountInfo.m_strGuid, l_strVal);
	//		if (!bSet)
	//		{
	//			ICC_LOG_ERROR(m_pLog, "set user info to redis failed!! [%s : %s]", l_tAccountInfo.m_strGuid.c_str(), l_strVal.c_str());
	//			return false;
	//		}
	//	}		
	//}

	//std::map<std::string, std::string>::const_iterator itr_const;
	//for (itr_const = mapTmps.begin(); itr_const != mapTmps.end(); ++itr_const)
	//{
	//	bool bSet = m_pRedisClient->HDel(USER_INFO, itr_const->first);
	//	if (!bSet)
	//	{
	//		ICC_LOG_ERROR(m_pLog, "delete language info from redis failed! [%s]", itr_const->first.c_str());
	//		return false;
	//	}
	//}

	//ICC_LOG_DEBUG(m_pLog, "load user data success [count:%d]", l_pResult->RecordSize());
	return true;
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
bool CUser::_GetAllOldData(std::map<std::string, std::string>& mapOldDatas)
{
	if (m_pRedisClient->HGetAllEx(USER_INFO, mapOldDatas))
	{
		return true;
	}

	ICC_LOG_ERROR(m_pLog, "get all old user datas failed!!!");

	return false;
}

bool CUser::_CompareData(std::map<std::string, std::string>& mapOldDatas, const std::vector<Data::CUserInfo>& vecNewDatas,
	std::vector<Data::CUserInfo>& addDatas, std::vector<Data::CUserInfo>& updateDatas, std::vector<Data::CUserInfo>& deleteDatas)
{
	int iNewDataCount = vecNewDatas.size();
	for (int i = 0; i < iNewDataCount; ++i)
	{
		std::map<std::string, std::string>::iterator itr = mapOldDatas.find(vecNewDatas[i].m_strGuid);
		if (itr != mapOldDatas.end())
		{
			Data::CUserInfo oldInfo;
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
		Data::CUserInfo oldInfo;
		if (!oldInfo.Parse(itr_const->second, m_pJsonFty->CreateJson()))
		{
			return false;
		}

		deleteDatas.push_back(oldInfo);
	}

	return true;
}

bool CUser::_UpdateDataToRedis(const std::vector<Data::CUserInfo>& vecDatas)
{
	std::map<std::string, std::string> mapDatas;
	for (Data::CUserInfo p_userInfo : vecDatas)
	{
		std::string strTmp = p_userInfo.ToJson(m_pJsonFty->CreateJson());
		mapDatas.insert(std::make_pair(p_userInfo.m_strGuid, strTmp));
	}

	if (!mapDatas.empty())
	{
		if (!m_pRedisClient->HMSet(USER_INFO, mapDatas))
		{
			ICC_LOG_ERROR(m_pLog, "add user data to redis failed! content size[%d]", vecDatas.size());
			return false;
		}
	}	

	ICC_LOG_DEBUG(m_pLog, "add user data to redis success! content count[%d]", vecDatas.size());

	return true;
}

bool CUser::_Add(std::vector<Data::CUserInfo>& vecDatas, std::string& strTransGuid)
{
	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	std::map<std::string, std::string> mapDatas;
	std::map<std::string, std::string> mapCodes;

	std::vector<Data::CUserInfo>::iterator it = vecDatas.begin();
	for (; it != vecDatas.end(); ++it)
	{
		if (mapDatas.find(it->m_strGuid) != mapDatas.end() || mapCodes.find(it->m_strCode) != mapCodes.end())
		{
			ICC_LOG_WARNING(m_pLog, "same user data ! guid[%s]", it->m_strGuid.c_str());
			continue;
		}

		DataBase::SQLRequest l_oSetSQLReq;
		l_oSetSQLReq.sql_id = "insert_icc_t_user";
		l_oSetSQLReq.param["guid"] = it->m_strGuid;
		l_oSetSQLReq.param["code"] = it->m_strCode.empty() ? it->m_strName: it->m_strCode;
		l_oSetSQLReq.param["name"] = it->m_strName;
		l_oSetSQLReq.param["pwd"] = it->m_strPwd;
		l_oSetSQLReq.param["is_delete"] = "false";
		l_oSetSQLReq.param["create_user"] = it->m_strCreateUser;
		l_oSetSQLReq.param["create_time"] = it->m_strCreateTime;
		l_oSetSQLReq.param["update_user"] = it->m_strUpdateUser;
		l_oSetSQLReq.param["update_time"] = it->m_strUpdateTime;
		l_oSetSQLReq.param["user_type"] = it->m_strUserType;
		l_oSetSQLReq.param["staff_guid"] = it->m_strStaffGuid;
		l_oSetSQLReq.param["org_guid"] = it->m_strOrgGuid;
		l_oSetSQLReq.param["remark"] = it->m_strRemark;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSetSQLReq, false, strTransGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "add user data to db failed! sql[%s]", l_pRSet->GetSQL().c_str());
			m_pDBConn->Rollback(strTransGuid);
			vecDatas.erase(it);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "add user data to db success!!! sql[%s]", l_pRSet->GetSQL().c_str());

		mapDatas.insert(std::make_pair(it->m_strGuid, it->m_strGuid));
		if (it->m_strCode.empty())
		{
			it->m_strCode = it->m_strName;
		}
		mapCodes.insert(std::make_pair(it->m_strCode, it->m_strCode));

	}

	if (!_UpdateDataToRedis(vecDatas))
	{
		m_pDBConn->Rollback(strTransGuid);
		return false;
	}

	return true;
}

bool CUser::_Update(std::vector<Data::CUserInfo>& vecDatas, std::string& strTransGuid)
{
	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());

	std::vector<Data::CUserInfo>::iterator it = vecDatas.begin();
	for (; it!= vecDatas.end(); ++it)
	{
		DataBase::SQLRequest l_oSetSQLReq;
		l_oSetSQLReq.sql_id = "update_icc_t_user";
		l_oSetSQLReq.param["guid"] = it->m_strGuid;
		l_oSetSQLReq.set["code"] = it->m_strCode.empty() ? it->m_strName : it->m_strCode;
		l_oSetSQLReq.set["name"] = it->m_strName;
		l_oSetSQLReq.set["pwd"] = it->m_strPwd;
		l_oSetSQLReq.set["is_delete"] = "false";
		l_oSetSQLReq.set["remark"] = it->m_strRemark;
		l_oSetSQLReq.set["update_user"] = it->m_strUpdateUser;
		l_oSetSQLReq.set["update_time"] = it->m_strUpdateTime;
		l_oSetSQLReq.set["user_type"] = it->m_strUserType;
		l_oSetSQLReq.set["staff_guid"] = it->m_strStaffGuid;
		l_oSetSQLReq.set["org_guid"] = it->m_strOrgGuid;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSetSQLReq, false, strTransGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "update user data to db failed! sql[%s]", l_pRSet->GetSQL().c_str());
			m_pDBConn->Rollback(strTransGuid);
			vecDatas.erase(it);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "update user data to db success!!! sql[%s]", l_pRSet->GetSQL().c_str());

	}

	if (!_UpdateDataToRedis(vecDatas))
	{
		m_pDBConn->Rollback(strTransGuid);
		return false;
	}

	return true;
}

bool CUser::_Del(std::vector<Data::CUserInfo>& vecDatas, std::string& strTransGuid)
{
	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());

	std::vector<Data::CUserInfo>::iterator it = vecDatas.begin();
	for (; it != vecDatas.end(); ++it)
	{
		DataBase::SQLRequest l_oDeleteSQLReq;
		l_oDeleteSQLReq.sql_id = "delete_icc_t_user";
		l_oDeleteSQLReq.param["guid"] = it->m_strGuid;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oDeleteSQLReq, false, strTransGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "delete user data to db failed! sql[%s]", l_pRSet->GetSQL().c_str());
			m_pDBConn->Rollback(strTransGuid);
			vecDatas.erase(it);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "delete user data to db success!!! sql[%s]", l_pRSet->GetSQL().c_str());


	//	//删除绑定数据
	//	if (!_DeleteBindInfo(p_userInfo.m_strGuid, strTransGuid))
	//	{
	//		ICC_LOG_ERROR(m_pLog, "delete user[%s] bind info error, rollback", p_userInfo.m_strGuid.c_str());
	//		m_pDBConn->Rollback(strTransGuid);
	//		return false;
	//	}

	//	//删除用户下所有的绑定数据
	//	std::vector<std::string> l_vecBindGuid;
	//	std::string l_bindStaffGuid, l_bindDeptGuid;
	//	_GetBindStaffGuid(p_userInfo.m_strGuid, l_bindStaffGuid);
	//	_GetBindDeptGuid(p_userInfo.m_strGuid, l_bindDeptGuid);
	//	_GetBindRoleGuid(p_userInfo.m_strGuid, l_vecBindGuid);

	//	if (!l_bindStaffGuid.empty())
	//	{
	//		l_vecBindGuid.push_back(l_bindStaffGuid);
	//	}
	//	if (!l_bindDeptGuid.empty())
	//	{
	//		l_vecBindGuid.push_back(l_bindDeptGuid);
	//	}

	//	ICC_LOG_DEBUG(m_pLog, "delete user[%s] bind data[%d].", p_userInfo.m_strCode.c_str(), l_vecBindGuid.size());

	//	for (auto l_strGuid : l_vecBindGuid)
	//	{
	//		if (!l_strGuid.empty())
	//		{
	//			if (!m_pRedisClient->HDel(BIND_INFO, p_userInfo.m_strGuid + l_strGuid))
	//			{
	//				ICC_LOG_ERROR(m_pLog, "delete user bind data from redis failed! guid[%s:%s]", p_userInfo.m_strGuid.c_str(), l_strGuid.c_str());
	//				m_pDBConn->Rollback(strTransGuid);

	//				return false;
	//			}
	//		}
	//	}


		if (!m_pRedisClient->HDel(USER_INFO, it->m_strGuid))
		{
			ICC_LOG_ERROR(m_pLog, "delete user data from redis failed! guid[%s]", it->m_strGuid.c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "delete user data from redis success! guid[%s]", it->m_strGuid.c_str());
	}

	return true;
}

bool CUser::_UpdateData(std::vector<Data::CUserInfo>& addDatas, std::vector<Data::CUserInfo>& updateDatas, std::vector<Data::CUserInfo>& deleteDatas)
{
	ICC_LOG_DEBUG(m_pLog, "update user data begin! addDatas[%d], updateDatas[%d], deleteDatas[%d]",
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

	ICC_LOG_DEBUG(m_pLog, "update user data complete! ");

	return true;
}


bool CUser::_Delete()
{
	if (m_vecUser.empty())
	{
		return true;
	}

	//用户信息关联了单位，警员，角色信息，在删除时需要做关联删除
	//std::string l_strGuid = m_pDBConn->BeginTransaction();
	//for (Data::CUserInfo l_user : m_vecUser)
	//{
	//	//删除绑定数据
	//	if (!_DeleteBindInfo(l_user.m_strGuid, l_strGuid))
	//	{
	//		ICC_LOG_ERROR(m_pLog, "delete user[%s] bind info error, rollback", l_user.m_strCode.c_str());
	//		m_pDBConn->Rollback(l_strGuid);
	//		return false;
	//	}

	//	DataBase::SQLRequest l_oDeleteSQLReq;
	//	l_oDeleteSQLReq.sql_id = "update_icc_t_user";
	//	l_oDeleteSQLReq.set["is_delete"] = "true";
	//	l_oDeleteSQLReq.set["update_user"] = "AuthBusiness";
	//	l_oDeleteSQLReq.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
	//	l_oDeleteSQLReq.param["guid"] = l_user.m_strGuid;

	//	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oDeleteSQLReq, false, l_strGuid);
	//	if (!l_pRSet->IsValid())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "delete user info failed, rollback");

	//		m_pDBConn->Rollback(l_strGuid);
	//		return false;
	//	}
	//}

	//m_pDBConn->Commit(l_strGuid);

	return true;
}

bool CUser::_HSet()
{
	if (m_vecUser.empty())
	{
		return true;
	}

	std::map<std::string, std::string> tmp_mapInfo;
	for (Data::CUserInfo var : m_vecUser)
	{
		tmp_mapInfo[var.m_strGuid] = var.ToJson(m_pJsonFty->CreateJson());
	}

	return m_pRedisClient->HMSet(USER_INFO, tmp_mapInfo);
}

void CUser::_HDel()
{
	//for (Data::CUserInfo l_user : m_vecUser)
	//{
	//	//删除用户下所有的绑定数据
	//	std::vector<std::string> l_vecBindGuid;
	//	std::string l_bindStaffGuid, l_bindDeptGuid;
	//	_GetBindStaffGuid(l_user.m_strGuid, l_bindStaffGuid);
	//	_GetBindDeptGuid(l_user.m_strGuid, l_bindDeptGuid);
	//	_GetBindRoleGuid(l_user.m_strGuid, l_vecBindGuid);

	//	if (!l_bindStaffGuid.empty())
	//	{
	//		l_vecBindGuid.push_back(l_bindStaffGuid);
	//	}
	//	if (!l_bindDeptGuid.empty())
	//	{
	//		l_vecBindGuid.push_back(l_bindDeptGuid);
	//	}

	//	ICC_LOG_DEBUG(m_pLog, "delete user[%s] bind data[%d].", l_user.m_strCode.c_str(), l_vecBindGuid.size());

	//	for (auto l_strGuid : l_vecBindGuid)
	//	{
	//		if (!l_strGuid.empty())
	//		{
	//			m_pRedisClient->HDel(BIND_INFO, l_user.m_strGuid + l_strGuid);
	//			m_pRedisClient->HDel(BIND_INFO, l_user.m_strGuid + l_strGuid);
	//		}
	//	}

	//	m_pRedisClient->HDel(USER_INFO, l_user.m_strGuid);
	//}
}

bool CUser::_IsEqual(Data::CUserInfo p_old, Data::CUserInfo l_new)
{
	bool bResult = p_old.m_strGuid == l_new.m_strGuid		
		&& p_old.m_strName == l_new.m_strName
		&& p_old.m_strPwd == l_new.m_strPwd
		&& p_old.m_strRemark == l_new.m_strRemark
		&& p_old.m_strUserType == l_new.m_strUserType
		&& p_old.m_strStaffGuid == l_new.m_strStaffGuid
		&& p_old.m_strOrgGuid == l_new.m_strOrgGuid;

	if (!bResult)
	{
		ICC_LOG_DEBUG(m_pLog, "is not equal, old data[%s:%s:%s:%s:%s:%s:%s], new data[%s:%s:%s:%s:%s:%s:%s]",
			p_old.m_strGuid.c_str(),  p_old.m_strName.c_str(), p_old.m_strPwd.c_str(), p_old.m_strRemark.c_str(), p_old.m_strUserType.c_str(), p_old.m_strStaffGuid.c_str(), p_old.m_strOrgGuid.c_str(),
			l_new.m_strGuid.c_str(),  l_new.m_strName.c_str(), l_new.m_strPwd.c_str(), l_new.m_strRemark.c_str(), l_new.m_strUserType.c_str(), l_new.m_strStaffGuid.c_str(), l_new.m_strOrgGuid.c_str());
		return false;
	}

	return true;
}


DataBase::SQLRequest CUser::_ConstructSQLStruct(Data::CUserInfo& p_userInfo, std::string p_strSqlID)
{
	DataBase::SQLRequest l_oSQLExec;
	if (p_strSqlID == "select_icc_t_user")
	{
		l_oSQLExec.sql_id = "select_icc_t_user";		
	}
	else if (p_strSqlID == "select_icc_t_user_one")
	{
		l_oSQLExec.sql_id = "select_icc_t_user";
		l_oSQLExec.param["code"] = p_userInfo.m_strCode;
		l_oSQLExec.param["is_delete"] = "true";
	}
	else if (p_strSqlID == "update_icc_t_user")
	{
		l_oSQLExec.sql_id = "update_icc_t_user";
		l_oSQLExec.param["guid"] = p_userInfo.m_strGuid;
		l_oSQLExec.set["code"] = p_userInfo.m_strCode;
		l_oSQLExec.set["name"] = p_userInfo.m_strName;
		l_oSQLExec.set["pwd"] = p_userInfo.m_strPwd;		
		l_oSQLExec.set["remark"] = p_userInfo.m_strRemark;
		l_oSQLExec.set["update_user"] = "AuthBusiness";
		l_oSQLExec.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
	}
	else if (p_strSqlID == "recover_icc_t_user_flag")
	{
		l_oSQLExec.sql_id = "update_icc_t_user";
		l_oSQLExec.param["code"] = p_userInfo.m_strCode;
		l_oSQLExec.set["guid"] = p_userInfo.m_strGuid;
		l_oSQLExec.set["name"] = p_userInfo.m_strName;
		l_oSQLExec.set["pwd"] = p_userInfo.m_strPwd;
		l_oSQLExec.set["is_delete"] = "false";
		l_oSQLExec.set["remark"] = p_userInfo.m_strRemark;
		l_oSQLExec.set["update_user"] = "AuthBusiness";
		l_oSQLExec.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
	}
	else if (p_strSqlID == "insert_icc_t_user")
	{
		l_oSQLExec.sql_id = "insert_icc_t_user";
		l_oSQLExec.param["guid"] = p_userInfo.m_strGuid;
		l_oSQLExec.param["code"] = p_userInfo.m_strCode;
		l_oSQLExec.param["name"] = p_userInfo.m_strName;
		l_oSQLExec.param["pwd"] = p_userInfo.m_strPwd;	
		l_oSQLExec.param["create_user"] = "AuthBusiness";
		l_oSQLExec.param["create_time"] = m_pDateTime->CurrentDateTimeStr();
		l_oSQLExec.param["remark"] = p_userInfo.m_strRemark;

	}
	else if (p_strSqlID == "delete_icc_t_user")
	{
		l_oSQLExec.sql_id = "update_icc_t_user";
		l_oSQLExec.param["guid"] = p_userInfo.m_strGuid;	
		l_oSQLExec.set["update_user"] = "AuthBusiness";
		l_oSQLExec.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
	}

	return l_oSQLExec;
}


bool CUser::_DeleteBindInfo(std::string p_strUserGuid, const std::string& strTransGuid)
{
	//删除用户下所有的绑定数据
	std::vector<std::string> l_vecBindGuid;
	std::string l_bindStaffGuid, l_bindDeptGuid;
	_GetBindStaffGuid(p_strUserGuid, l_bindStaffGuid);
	_GetBindDeptGuid(p_strUserGuid, l_bindDeptGuid);
	_GetBindRoleGuid(p_strUserGuid, l_vecBindGuid);

	if (!l_bindStaffGuid.empty())
	{
		l_vecBindGuid.push_back(l_bindStaffGuid);
	}
	if (!l_bindDeptGuid.empty())
	{
		l_vecBindGuid.push_back(l_bindDeptGuid);
	}

	ICC_LOG_DEBUG(m_pLog, "delete user[%s] bind data[%d].", p_strUserGuid.c_str(), l_vecBindGuid.size());

	for (auto var : l_vecBindGuid)
	{
		if (!var.empty())
		{
			DataBase::SQLRequest l_oDeleteBind;
			l_oDeleteBind.sql_id = "delete_icc_t_user_bind";
			l_oDeleteBind.param["from_guid"] = p_strUserGuid;
			l_oDeleteBind.param["to_guid"] = var;
			DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oDeleteBind, false, strTransGuid);
			if (!l_pRSet->IsValid())
			{
				return false;
			}
		}
	}

	return true;
}

bool CUser::_GetBindStaffGuid(std::string p_strFromGuid, std::string& p_strToGuid)
{
	DataBase::SQLRequest l_oSelect;
	l_oSelect.sql_id = "select_icc_t_bind";
	l_oSelect.param["from_guid"] = p_strFromGuid;
	l_oSelect.param["type"] = "user_bind_staff";
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSelect);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "user[%s] have no bind staff.", p_strFromGuid.c_str());
		return false;
	}
	else
	{
		if (l_pRSet->Next())
		{
			p_strToGuid = l_pRSet->GetValue("to_guid");
		}
	}

	ICC_LOG_DEBUG(m_pLog, "get user[%s] bind staff[%s].", p_strFromGuid.c_str(), p_strToGuid.c_str());

	return true;
}

bool CUser::_GetBindDeptGuid(std::string p_strFromGuid, std::string& p_strToGuid)
{
	DataBase::SQLRequest l_oSelect;
	l_oSelect.sql_id = "select_icc_t_bind";
	l_oSelect.param["from_guid"] = p_strFromGuid;
	l_oSelect.param["type"] = "user_bind_dept";
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSelect);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "user[%s] have no bind dept.", p_strFromGuid.c_str());
		return false;
	}
	else
	{
		if (l_pRSet->Next())
		{
			p_strToGuid = l_pRSet->GetValue("to_guid");
		}
	}
	ICC_LOG_DEBUG(m_pLog, "get user[%s] bind dept[%s].", p_strFromGuid.c_str(), p_strToGuid.c_str());
	return true;
}

bool CUser::_GetBindRoleGuid(std::string p_strFromGuid, std::vector<std::string>& l_vecBindGuid)
{
	DataBase::SQLRequest l_oSelect;
	l_oSelect.sql_id = "select_icc_t_bind";
	l_oSelect.param["from_guid"] = p_strFromGuid;
	l_oSelect.param["type"] = "user_bind_role";
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSelect);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "user[%s] have no bind role.", p_strFromGuid.c_str());
		return false;
	}
	else
	{
		while (l_pRSet->Next())
		{
			l_vecBindGuid.push_back(l_pRSet->GetValue("to_guid"));
		}
	}
	ICC_LOG_DEBUG(m_pLog, "get user[%s] bind role[%d].", p_strFromGuid.c_str(), l_vecBindGuid.size());
	return true;
}
