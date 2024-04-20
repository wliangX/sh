#include "Boost.h"
#include "Auth.h"

using namespace ICC;
using namespace Data;

CAuth::CAuth(std::vector<Data::CAuthInfo> p_vecAuthInfo, IResourceManagerPtr p_pIResourceManager) :
m_vecAuth(p_vecAuthInfo),
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

CAuth::~CAuth()
{

}

int CAuth::ReceiveTotalDataSize()
{
	return (int)m_vecAuth.size();
}

void CAuth::AppendReceiveDataSize(int iReceiveSize)
{
	m_iTotalReceiveDataSize += iReceiveSize;
}

void CAuth::Append(std::vector<Data::CAuthInfo>& p_vecInfo)
{
	int iCount = p_vecInfo.size();
	for (int i = 0; i < iCount; ++i)
	{
		m_vecAuth.push_back(p_vecInfo[i]);
	}
}

bool CAuth::SynUpdate()
{
	/*if (m_vecAuth.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no func data need update!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "begin update func data, size[%d]", m_vecAuth.size());

	std::map<std::string, std::string> tmp_mapInfo;
	for (Data::CAuthInfo var : m_vecAuth)
	{
		tmp_mapInfo[var.m_strGuid] = var.ToJson(m_pJsonFty->CreateJson());
	}

	if (!m_pRedisClient->HMSet(Func, tmp_mapInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "set func key failed!!");
		return false;
	}

	return true;*/

	if (m_vecAuth.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no func data need update!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "begin update func data, size[%d]", m_vecAuth.size());

	std::map<std::string, std::string> mapOldDatas;
	if (!_GetAllOldData(mapOldDatas))
	{
		ICC_LOG_ERROR(m_pLog, "get all old func datas failed!!");
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "get all old func datas success! size[%d] ", mapOldDatas.size());

	std::vector<Data::CAuthInfo> addDatas;
	std::vector<Data::CAuthInfo> updateDatas;
	std::vector<Data::CAuthInfo> deleteDatas;
	_CompareData(mapOldDatas, m_vecAuth, addDatas, updateDatas, deleteDatas);

	if (!_UpdateData(addDatas, updateDatas, deleteDatas))
	{
		//LoadData();
		return false;
	}

	return true;
}

bool CAuth::SingleAdd()
{
	if (m_vecAuth.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no func data need add!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "will add func data size[%d]!!", m_vecAuth.size());
	
	std::string l_strGuid = m_pDBConn->BeginTransaction();
	if (l_strGuid.empty())
	{
		ICC_LOG_ERROR(m_pLog, "begin transaction failed! ");
		return false;
	}

	if (!_Add(m_vecAuth, l_strGuid))
	{
		return false;
	}

	m_pDBConn->Commit(l_strGuid);

	ICC_LOG_DEBUG(m_pLog, "add func data complete! ");

	return true;
}

bool CAuth::SingleModify()
{
	if (m_vecAuth.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no func data need update!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "will update func data size[%d]!!", m_vecAuth.size());
	
	std::string l_strGuid = m_pDBConn->BeginTransaction();	
	if (l_strGuid.empty())
	{
		ICC_LOG_ERROR(m_pLog, "begin transaction failed! ");
		return false;
	}

	if (!_Update(m_vecAuth, l_strGuid))
	{
		return false;
	}

	m_pDBConn->Commit(l_strGuid);

	ICC_LOG_DEBUG(m_pLog, "update func data complete! ");

	return true;
}

bool CAuth::SingleDelete()
{
	if (m_vecAuth.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no func data need delete!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "will delete func data size[%d]!!", m_vecAuth.size());
	
	std::string l_strGuid = m_pDBConn->BeginTransaction();
	if (l_strGuid.empty())
	{
		ICC_LOG_ERROR(m_pLog, "begin transaction failed! ");
		return false;
	}

	if (!_Del(m_vecAuth, l_strGuid))
	{
		return false;
	}

	m_pDBConn->Commit(l_strGuid);

	ICC_LOG_DEBUG(m_pLog, "delete func data complete! ");

	return true;
}

bool CAuth::LoadData()
{
	ICC_LOG_DEBUG(m_pLog, "load func to redis begin");	

	bool l_bLoadFuncSuccess = false;
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_func";

	ICC_LOG_DEBUG(m_pLog, "LoadFuncInfo Begin");
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());

	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error Error Message :[%s]",
			l_result->GetErrorMsg().c_str());
		return false;
	}

	std::vector< CAuthInfo> vecDatas;
	while (l_result->Next())
	{
		CAuthInfo l_oFunc;
		l_oFunc.m_strGuid = l_result->GetValue("guid");
		l_oFunc.m_strCode = l_result->GetValue("code");
		l_oFunc.m_strParentGuid = l_result->GetValue("parent_guid");
		l_oFunc.m_strName = l_result->GetValue("name");
		l_oFunc.m_strSort = l_result->GetValue("sort");
		vecDatas.push_back(l_oFunc);
	}

	int iDelCount = m_pRedisClient->Del(Func);
	if (!_SetDataToRedis(vecDatas))
	{
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "load func to redis begin, count[%d]", vecDatas.size());

	return true;


	/*std::map<std::string, std::string> mapTmps;
	if (!_GetAllOldData(mapTmps))
	{
		ICC_LOG_ERROR(m_pLog, "get all old func datas failed!!");
		return false;
	}

	bool l_bLoadFuncSuccess = false;
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_func";

	ICC_LOG_DEBUG(m_pLog, "LoadFuncInfo Begin");
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());

	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error Error Message :[%s]",
			l_result->GetErrorMsg().c_str());
		return false;
	}

	while (l_result->Next())
	{
		CAuthInfo l_oFunc;
		l_oFunc.m_strGuid = l_result->GetValue("guid");
		l_oFunc.m_strCode = l_result->GetValue("code");
		l_oFunc.m_strParentGuid = l_result->GetValue("parent_guid");
		l_oFunc.m_strName = l_result->GetValue("name");
		l_oFunc.m_strSort = l_result->GetValue("sort");

		JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
		if (!l_pJson)
		{
			ICC_LOG_ERROR(m_pLog, "set language info to redis failed! [json is null]");
			return false;
		}

		bool bNeedWrite = false;

		std::map<std::string, std::string>::iterator itr;
		itr = mapTmps.find(l_oFunc.m_strGuid);
		if (itr != mapTmps.end())
		{
			Data::CAuthInfo oldInfo;
			if (!oldInfo.Parse(itr->second, m_pJsonFty->CreateJson()))
			{
				bNeedWrite = true;
			}
			else
			{
				if (!_IsEqual(oldInfo, l_oFunc))
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
			std::string l_strVal = l_oFunc.ToJson(m_pJsonFty->CreateJson());
			bool bSet = m_pRedisClient->HSet(Func, l_oFunc.m_strGuid, l_strVal);
			if (!bSet)
			{
				ICC_LOG_ERROR(m_pLog, "set func info to redis failed!! guid[%s : %s]", l_oFunc.m_strGuid.c_str(), l_strVal.c_str());
				return false;
			}
		}		
	}

	std::map<std::string, std::string>::const_iterator itr_const;
	for (itr_const = mapTmps.begin(); itr_const != mapTmps.end(); ++itr_const)
	{
		bool bSet = m_pRedisClient->HDel(Func, itr_const->first);
		if (!bSet)
		{
			ICC_LOG_ERROR(m_pLog, "delete func info from redis failed! [%s]", itr_const->first.c_str());
			return false;
		}
	}

	ICC_LOG_DEBUG(m_pLog, "LoadFuncInfo Success,Func Size[%d]", l_result->RecordSize());

	return true;*/
}



/************************************************************************/
/*                                                                      */
/************************************************************************/
bool CAuth::_GetAllOldData(std::map<std::string, std::string>& mapOldDatas)
{
	if (m_pRedisClient->HGetAllEx(Func, mapOldDatas))
	{
		return true;
	}

	ICC_LOG_ERROR(m_pLog, "get all old func datas failed!!!");

	return false;
}

bool CAuth::_CompareData(std::map<std::string, std::string>& mapOldDatas, const std::vector<Data::CAuthInfo>& vecNewDatas,
	std::vector<Data::CAuthInfo>& addDatas, std::vector<Data::CAuthInfo>& updateDatas, std::vector<Data::CAuthInfo>& deleteDatas)
{
	int iNewDataCount = vecNewDatas.size();
	for (int i = 0; i < iNewDataCount; ++i)
	{
		std::map<std::string, std::string>::iterator itr = mapOldDatas.find(vecNewDatas[i].m_strGuid);
		if (itr != mapOldDatas.end())
		{
			Data::CAuthInfo oldInfo;
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
		Data::CAuthInfo oldInfo;
		if (!oldInfo.Parse(itr_const->second, m_pJsonFty->CreateJson()))
		{
			return false;
		}

		deleteDatas.push_back(oldInfo);
	}

	return true;
}

bool CAuth::_SetDataToRedis(const std::vector<Data::CAuthInfo>& vecDatas)
{
	std::map<std::string, std::string> mapFuncs;
	for (Data::CAuthInfo var : vecDatas)
	{
		std::string strTmp = var.ToJson(m_pJsonFty->CreateJson());
		mapFuncs[var.m_strGuid] = strTmp;
	}

	if (!mapFuncs.empty())
	{
		if (!m_pRedisClient->HMSet(Func, mapFuncs))
		{
			ICC_LOG_ERROR(m_pLog, "add func data to redis failed! content count[%d]", mapFuncs.size());
			return false;
		}
	}	

	ICC_LOG_DEBUG(m_pLog, "add func data to redis success! content count[%d]", mapFuncs.size());

	return true;
}

bool CAuth::_Add(const std::vector<Data::CAuthInfo>& vecDatas, const std::string& strTransGuid)
{
	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	
	std::map<std::string, std::string> mapTmpDatas;

	for (Data::CAuthInfo var : vecDatas)
	{
		if (mapTmpDatas.find(var.m_strGuid) != mapTmpDatas.end())
		{
			ICC_LOG_WARNING(m_pLog, "same func data ! guid[%s]", var.m_strGuid.c_str());
			continue;
		}

		DataBase::SQLRequest l_oSetSQLReq;
		l_oSetSQLReq.sql_id = "insert_icc_t_func";
		l_oSetSQLReq.param["guid"] = var.m_strGuid;
		l_oSetSQLReq.param["name"] = var.m_strName;
		l_oSetSQLReq.param["parent_guid"] = var.m_strParentGuid;
		l_oSetSQLReq.param["code"] = var.m_strCode;
		l_oSetSQLReq.param["sort"] = var.m_strSort;
		l_oSetSQLReq.param["create_user"] = "smp_gateway";
		l_oSetSQLReq.param["create_time"] = l_strCurrentTime;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSetSQLReq, false, strTransGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "add func data to db failed! sql[%s]", l_pRSet->GetSQL().c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "add func data to db success!!! sql[%s]", l_pRSet->GetSQL().c_str());

		mapTmpDatas.insert(std::make_pair(var.m_strGuid, var.m_strGuid));		
	}

	if (!_SetDataToRedis(vecDatas))
	{
		m_pDBConn->Rollback(strTransGuid);
		return false;
	}

	return true;
}

bool CAuth::_Update(const std::vector<Data::CAuthInfo>& vecDatas, const std::string& strTransGuid)
{
	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());

	for (Data::CAuthInfo var : vecDatas)
	{
		DataBase::SQLRequest l_oSetSQLReq;
		l_oSetSQLReq.sql_id = "update_icc_t_func";
		l_oSetSQLReq.set["name"] = var.m_strName;
		l_oSetSQLReq.set["parent_guid"] = var.m_strParentGuid;
		l_oSetSQLReq.set["code"] = var.m_strCode;
		l_oSetSQLReq.set["sort"] = var.m_strSort;
		l_oSetSQLReq.set["update_user"] = "smp_gateway";
		l_oSetSQLReq.set["update_time"] = l_strCurrentTime;
		l_oSetSQLReq.param["guid"] = var.m_strGuid;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSetSQLReq, false, strTransGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "update func data to db failed! sql[%s]", l_pRSet->GetSQL().c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "update func data to db success!!! sql[%s]", l_pRSet->GetSQL().c_str());

		/*std::string strTmp = var.ToJson(m_pJsonFty->CreateJson());
		if (!m_pRedisClient->HSet(Func, var.m_strGuid, strTmp))
		{
			ICC_LOG_ERROR(m_pLog, "update func data to redis failed! content[%s]", strTmp.c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "update func data to redis success! content[%s]", strTmp.c_str());*/
	}

	if (!_SetDataToRedis(vecDatas))
	{
		m_pDBConn->Rollback(strTransGuid);
		return false;
	}

	return true;
}

bool CAuth::_Del(const std::vector<Data::CAuthInfo>& vecDatas, const std::string& strTransGuid)
{
	for (Data::CAuthInfo var : vecDatas)
	{
		DataBase::SQLRequest l_oDeleteSQLReq;
		l_oDeleteSQLReq.sql_id = "delete_icc_t_func";
		l_oDeleteSQLReq.param["guid"] = var.m_strGuid;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oDeleteSQLReq, false, strTransGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "delete func data to db failed! sql[%s]", l_pRSet->GetSQL().c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_ERROR(m_pLog, "delete func data to db success!!! sql[%s]", l_pRSet->GetSQL().c_str());

		if (!m_pRedisClient->HDel(Func, var.m_strGuid))
		{
			ICC_LOG_ERROR(m_pLog, "delete func data from redis failed! guid[%s]", var.m_strGuid.c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "delete func data from redis success! guid[%s]", var.m_strGuid.c_str());
	}

	return true;
}

bool CAuth::_UpdateData(const std::vector<Data::CAuthInfo>& addDatas, const std::vector<Data::CAuthInfo>& updateDatas, const std::vector<Data::CAuthInfo>& deleteDatas)
{
	ICC_LOG_DEBUG(m_pLog, "update func data begin! addDatas[%d], updateDatas[%d], deleteDatas[%d]",
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

	ICC_LOG_DEBUG(m_pLog, "update func data complete! ");

	return true;
}


bool CAuth::_Delete()
{
	if (m_vecAuth.empty())
	{
		return true;
	}

	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	std::string l_strGuid = m_pDBConn->BeginTransaction();
	for (Data::CAuthInfo var : m_vecAuth)
	{
		DataBase::SQLRequest l_oDeleteSQLReq;
		l_oDeleteSQLReq.sql_id = "delete_icc_t_func";
		l_oDeleteSQLReq.param["guid"] = var.m_strGuid;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oDeleteSQLReq, false, l_strGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "delete func error, rollback: [%s]", l_pRSet->GetErrorMsg().c_str());
			m_pDBConn->Rollback(l_strGuid);
			return false;
		}
	}

	m_pDBConn->Commit(l_strGuid);
	return true;
}


bool CAuth::_HSet()
{
	if (m_vecAuth.empty())
	{
		return true;
	}

	std::map<std::string, std::string> tmp_mapInfo;
	for (Data::CAuthInfo var : m_vecAuth)
	{
		tmp_mapInfo[var.m_strGuid] = var.ToJson(m_pJsonFty->CreateJson());
	}

	return m_pRedisClient->HMSet(Func, tmp_mapInfo);
}

void CAuth::_HDel()
{
	for (Data::CAuthInfo var : m_vecAuth)
	{
		m_pRedisClient->HDel(Func, var.m_strGuid);
	}
}

bool CAuth::_IsEqual(Data::CAuthInfo p_old, Data::CAuthInfo l_new)
{
	if (p_old.m_strGuid == l_new.m_strGuid &&
		p_old.m_strCode == l_new.m_strCode &&
		p_old.m_strName == l_new.m_strName &&
		p_old.m_strParentGuid == l_new.m_strParentGuid &&
		p_old.m_strSort == l_new.m_strSort)
	{

		return true;
	}
	return false;
}