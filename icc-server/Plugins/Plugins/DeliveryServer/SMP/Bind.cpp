#include "Boost.h"
#include "Bind.h"

using namespace ICC;
using namespace Data;

CBind::CBind(std::vector<Data::CBindInfo> p_vecBind, IResourceManagerPtr p_pIResourceManager, const std::string& strType):
m_vecBind(p_vecBind),
m_pResourceManager(p_pIResourceManager), m_strType(strType)
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

CBind::~CBind()
{

}

int CBind::ReceiveTotalDataSize()
{
	return m_iTotalReceiveDataSize;
}

void CBind::AppendReceiveDataSize(int iReceiveSize)
{
	m_iTotalReceiveDataSize += iReceiveSize;
}



void CBind::Append(std::vector<Data::CBindInfo>& p_vecInfo)
{
	int iCount = p_vecInfo.size();
	for (int i = 0; i < iCount; ++i)
	{
		m_vecBind.push_back(p_vecInfo[i]);
	}
}

bool CBind::SynUpdate()
{
	if (m_vecBind.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no role data need update!!");
		return true;
	}

	std::map<std::string, std::string> mapOldDatas;
	if (!_GetAllOldData(mapOldDatas))
	{
		ICC_LOG_ERROR(m_pLog, "get all role old datas failed!!");
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "get all role old datas success! size[%d] ", mapOldDatas.size());

	std::vector<Data::CBindInfo> addDatas;
	std::vector<Data::CBindInfo> updateDatas;
	std::vector<Data::CBindInfo> deleteDatas;
	_CompareData(mapOldDatas, m_vecBind, addDatas, updateDatas, deleteDatas);

	if (!_UpdateData(addDatas, updateDatas, deleteDatas))
	{
		LoadData();
		return false;
	}

	return true;
}

bool CBind::SingleAdd()
{
	if (m_vecBind.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no bind data need add!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "will add bind data size[%d]!!", m_vecBind.size());

	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	std::string l_strGuid = m_pDBConn->BeginTransaction();

	if (!_Add(m_vecBind, l_strGuid))
	{
		return false;
	}

	m_pDBConn->Commit(l_strGuid);

	ICC_LOG_DEBUG(m_pLog, "add bind data complete! ");

	return true;
}

bool CBind::SingleModify()
{
	if (m_vecBind.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no bind data need update!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "will update bind data size[%d]!!", m_vecBind.size());

	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	std::string l_strGuid = m_pDBConn->BeginTransaction();	

	if (!_Update(m_vecBind, l_strGuid))
	{
		return false;
	}	

	m_pDBConn->Commit(l_strGuid);

	ICC_LOG_DEBUG(m_pLog, "update bind data complete! ");

	return true;
}

bool CBind::SingleDelete()
{
	if (m_vecBind.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no bind data need delete!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "will delete bind data size[%d]!!", m_vecBind.size());

	std::vector<Data::CBindInfo> vecAllDatas;
	if (!_GetAllData(vecAllDatas))
	{
		ICC_LOG_DEBUG(m_pLog, "get all bind data failed!! type[%s]", m_strType.c_str());
		return false;
	}

	std::vector<Data::CBindInfo> vecDeletes;
	for (Data::CBindInfo l_bind : m_vecBind)
	{
		if (l_bind.m_strFromGuid.empty())
		{
			for (Data::CBindInfo l_bindOld : vecAllDatas)
			{
				if (l_bindOld.m_strToGuid == l_bind.m_strToGuid)
				{
					vecDeletes.push_back(l_bindOld);
				}
			}
		}
		else if (l_bind.m_strToGuid.empty())
		{
			for (Data::CBindInfo l_bindOld : vecAllDatas)
			{
				if (l_bindOld.m_strFromGuid == l_bind.m_strFromGuid)
				{
					vecDeletes.push_back(l_bindOld);
				}
			}
		}
		else
		{
			vecDeletes.push_back(l_bind);
		}		
	}

	if (vecDeletes.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no bind data need delete!!");
		return true;
	}
	
	std::string l_strGuid = m_pDBConn->BeginTransaction();	

	if (!_Del(vecDeletes, l_strGuid))
	{
		return false;
	}

	m_pDBConn->Commit(l_strGuid);

	ICC_LOG_DEBUG(m_pLog, "delete func data complete! ");

	return true;
}



void CBind::GetBindInfo(std::vector<Data::CBindInfo>& p_vecBind)
{ 
	p_vecBind = m_vecTempBind;
}

bool CBind::LoadData()
{
	ICC_LOG_DEBUG(m_pLog, "load bind from redis Begin");

	std::map<std::string, std::string> mapTmps;
	if (!_GetAllOldData(mapTmps))
	{
		ICC_LOG_ERROR(m_pLog, "get bind from redis failed!!");
		return false;
	}

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_bind";
	//l_SqlRequest.param["is_delete"] = "false";
	ICC_LOG_DEBUG(m_pLog, "LoadBindInfo Begin");
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, " ExecQuery Error Load Time Error Message :[%s]",
			l_result->GetErrorMsg().c_str());
		return true;
	}
	while (l_result->Next())
	{
		CBindInfo l_oBind;
		l_oBind.m_strGuid = l_result->GetValue("guid");
		l_oBind.m_strFromGuid = l_result->GetValue("from_guid");
		l_oBind.m_strToGuid = l_result->GetValue("to_guid");
		l_oBind.m_strType = l_result->GetValue("type");;
		l_oBind.m_strSort = l_result->GetValue("sort");
		std::string l_strVal = l_oBind.ToJson(m_pJsonFty->CreateJson());
		std::string strTmpGuid = l_oBind.m_strFromGuid + std::string("_") + l_oBind.m_strToGuid;


		bool bNeedWrite = false;

		std::map<std::string, std::string>::iterator itr;
		itr = mapTmps.find(strTmpGuid);
		if (itr != mapTmps.end())
		{
			Data::CBindInfo oldInfo;
			if (!oldInfo.Parse(itr->second, m_pJsonFty->CreateJson()))
			{
				bNeedWrite = true;
			}
			else
			{
				if (!_IsEqual(oldInfo, l_oBind))
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
			bool bRs = m_pRedisClient->HSet(BIND_INFO, strTmpGuid, l_strVal);
			if (!bRs)
			{
				ICC_LOG_ERROR(m_pLog, "set bind info to redis failed! [%s:%s]", strTmpGuid.c_str(), l_strVal.c_str());
				return false;
			}
		}		
	}

	std::map<std::string, std::string>::const_iterator itr_const;
	for (itr_const = mapTmps.begin(); itr_const != mapTmps.end(); ++itr_const)
	{
		bool bSet = m_pRedisClient->HDel(BIND_INFO, itr_const->first);
		if (!bSet)
		{
			ICC_LOG_ERROR(m_pLog, "delete bind info from redis failed! [%s]", itr_const->first.c_str());
			return false;
		}
	}

	ICC_LOG_DEBUG(m_pLog, "LoadBindInfo Success,Bind Size[%d]", l_result->RecordSize());
	return true;
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
bool CBind::_GetAllOldData(std::map<std::string, std::string>& mapOldDatas)
{
	if (m_pRedisClient->HGetAllEx(BIND_INFO, mapOldDatas))
	{
		return true;
	}

	ICC_LOG_ERROR(m_pLog, "get all old bind datas failed!!!");

	return false;
}

bool CBind::_CompareData(std::map<std::string, std::string>& mapOldDatas, const std::vector<Data::CBindInfo>& vecNewDatas,
	std::vector<Data::CBindInfo>& addDatas, std::vector<Data::CBindInfo>& updateDatas, std::vector<Data::CBindInfo>& deleteDatas)
{
	int iNewDataCount = vecNewDatas.size();
	for (int i = 0; i < iNewDataCount; ++i)
	{
		std::map<std::string, std::string>::iterator itr = mapOldDatas.find(vecNewDatas[i].m_strGuid);
		if (itr != mapOldDatas.end())
		{
			Data::CBindInfo oldInfo;
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
		Data::CBindInfo oldInfo;
		if (!oldInfo.Parse(itr_const->second, m_pJsonFty->CreateJson()))
		{
			return false;
		}

		if (oldInfo.m_strType == m_strType)
		{
			deleteDatas.push_back(oldInfo);
		}		
	}

	return true;
}

bool CBind::_Add(const std::vector<Data::CBindInfo>& vecDatas, const std::string& strTransGuid)
{
	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());

	for (Data::CBindInfo l_bind : vecDatas)
	{
		/*DataBase::SQLRequest l_oSetSQLReq;
		l_oSetSQLReq.sql_id = "insert_icc_t_bind";
		l_oSetSQLReq.param["guid"] = l_bind.m_strGuid;
		l_oSetSQLReq.param["type"] = l_bind.m_strType;
		l_oSetSQLReq.param["from_guid"] = l_bind.m_strFromGuid;
		l_oSetSQLReq.param["to_guid"] = l_bind.m_strToGuid;
		if (!l_bind.m_strSort.empty())
		{
			l_oSetSQLReq.param["sort"] = l_bind.m_strSort;
		}
		l_oSetSQLReq.param["is_delete"] = "false";
		l_oSetSQLReq.param["create_user"] = "BaseData Server";
		l_oSetSQLReq.param["create_time"] = l_strCurrentTime;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSetSQLReq, false, strTransGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "add bind data to db failed! sql[%s]", l_pRSet->GetSQL().c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "add bind data to db success!!! sql[%s]", l_pRSet->GetSQL().c_str());*/

		std::string strTmp = l_bind.ToJson(m_pJsonFty->CreateJson());
		if (!m_pRedisClient->HSet(BIND_INFO, l_bind.m_strGuid, strTmp))
		{
			ICC_LOG_ERROR(m_pLog, "add bind data to redis failed! content[%s]", strTmp.c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "add bind data to redis success! content[%s]", strTmp.c_str());
	}

	return true;
}

bool CBind::_Update(const std::vector<Data::CBindInfo>& vecDatas, const std::string& strTransGuid)
{
	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());

	for (Data::CBindInfo l_bind : vecDatas)
	{
		DataBase::SQLRequest l_oSetSQLReq;
		l_oSetSQLReq.sql_id = "update_icc_t_bind";
		l_oSetSQLReq.param["guid"] = l_bind.m_strGuid;
		l_oSetSQLReq.set["type"] = l_bind.m_strType;
		l_oSetSQLReq.set["from_guid"] = l_bind.m_strFromGuid;
		l_oSetSQLReq.set["to_guid"] = l_bind.m_strToGuid;
		if (!l_bind.m_strSort.empty())
		{
			l_oSetSQLReq.set["sort"] = l_bind.m_strSort;
		}
		l_oSetSQLReq.set["is_delete"] = "false";
		l_oSetSQLReq.set["update_user"] = "BaseData Server";
		l_oSetSQLReq.set["update_time"] = l_strCurrentTime;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSetSQLReq, false, strTransGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "update bind data to db failed! sql[%s]", l_pRSet->GetSQL().c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "update bind data to db success!!! sql[%s]", l_pRSet->GetSQL().c_str());

		std::string strTmp = l_bind.ToJson(m_pJsonFty->CreateJson());
		if (!m_pRedisClient->HSet(BIND_INFO, l_bind.m_strGuid, strTmp))
		{
			ICC_LOG_ERROR(m_pLog, "update bind data to redis failed! content[%s]", strTmp.c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "update bind data to redis success! content[%s]", strTmp.c_str());
	}

	return true;
}

bool CBind::_Del(const std::vector<Data::CBindInfo>& vecDatas, const std::string& strTransGuid)
{
	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());

	for (Data::CBindInfo l_bind : vecDatas)
	{
		DataBase::SQLRequest l_oDeleteSQLReq;
		l_oDeleteSQLReq.sql_id = "delete_icc_t_bind";
		l_oDeleteSQLReq.param["guid"] = l_bind.m_strGuid;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oDeleteSQLReq, false, strTransGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "delete bind data to db failed! sql[%s]", l_pRSet->GetSQL().c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_ERROR(m_pLog, "delete bind data to db success!!! sql[%s]", l_pRSet->GetSQL().c_str());

		if (!m_pRedisClient->HDel(BIND_INFO, l_bind.m_strGuid))
		{
			ICC_LOG_ERROR(m_pLog, "delete bind data from redis failed! guid[%s]", l_bind.m_strGuid.c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "delete bind data from redis success! guid[%s]", l_bind.m_strGuid.c_str());
	}
	return true;
}

bool CBind::_UpdateData(const std::vector<Data::CBindInfo>& addDatas, const std::vector<Data::CBindInfo>& updateDatas, const std::vector<Data::CBindInfo>& deleteDatas)
{
	ICC_LOG_DEBUG(m_pLog, "update bind data begin! addDatas[%d], updateDatas[%d], deleteDatas[%d]",
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

	ICC_LOG_DEBUG(m_pLog, "update bind data complete! ");

	return true;
}

bool CBind::_GetAllData(std::vector<Data::CBindInfo>& vecDatas)
{
	std::map<std::string, std::string> mapOldDatas;
	if (!_GetAllOldData(mapOldDatas))
	{
		ICC_LOG_ERROR(m_pLog, "get all role old datas failed!!");
		return false;
	}

	std::map<std::string, std::string>::const_iterator itr_const;
	for (itr_const = mapOldDatas.begin(); itr_const != mapOldDatas.end(); ++itr_const)
	{
		Data::CBindInfo oldInfo;
		if (!oldInfo.Parse(itr_const->second, m_pJsonFty->CreateJson()))
		{
			return false;
		}

		if (!m_strType.empty())
		{
			if (oldInfo.m_strType == m_strType)
			{
				vecDatas.push_back(oldInfo);
			}
		}
		else
		{
			vecDatas.push_back(oldInfo);
		}		
	}
	return true;
}

bool CBind::_IsEqual(Data::CBindInfo p_old, Data::CBindInfo l_new)
{
	if (p_old.m_strGuid == l_new.m_strGuid &&
		p_old.m_strFromGuid == l_new.m_strFromGuid &&
		p_old.m_strToGuid == l_new.m_strToGuid)
	{

		return true;
	}
	return false;
}

bool CBind::_Delete()
{
	if (m_vecBind.empty())
	{
		return true;
	}

	m_vecDelete.clear();
	//用户信息关联了单位，警员，角色信息，在删除时需要做关联删除
	std::string l_strGuid = m_pDBConn->BeginTransaction();

	std::map<std::string, std::string> l_mapBindInfo;
	m_pRedisClient->HGetAll(BIND_INFO, l_mapBindInfo);

	Data::CBindInfo l_tempData;
	for (Data::CBindInfo l_bind : m_vecBind)
	{
		DataBase::SQLRequest l_oDeleteSQLReq;
		l_oDeleteSQLReq.sql_id = "delete_icc_t_bind";

		if (l_bind.m_strFromGuid.empty())
		{
			// 删除请求中只包含 type 和 to_guid
			for (auto var : l_mapBindInfo)
			{
				Data::CBindInfo l_bindInfo;
				l_bindInfo.Parse(var.second, m_pJsonFty->CreateJson());
				if (l_bindInfo.m_strType == l_bind.m_strType && l_bindInfo.m_strToGuid == l_bind.m_strToGuid)
				{
					l_tempData.m_strType = l_bindInfo.m_strType;
					l_tempData.m_strGuid = l_bindInfo.m_strGuid;
					l_tempData.m_strFromGuid = l_bindInfo.m_strFromGuid;
					l_tempData.m_strToGuid = l_bindInfo.m_strToGuid;
					m_vecTempBind.push_back(l_tempData);

					l_oDeleteSQLReq.param["type"] = l_bindInfo.m_strType;
					m_vecDelete.push_back(l_bindInfo.m_strGuid);
				}
			}
			l_oDeleteSQLReq.param["to_guid"] = l_bind.m_strToGuid;
		}
		else if (l_bind.m_strToGuid.empty())
		{
			//删除用户绑定警员关系,此时的删除请求中只包含 type 和 from_guid（user_guid）字段，没有to_guid
			for (auto var : l_mapBindInfo)
			{
				Data::CBindInfo l_bindInfo;
				l_bindInfo.Parse(var.second, m_pJsonFty->CreateJson());
				if (l_bindInfo.m_strType == l_bind.m_strType && l_bindInfo.m_strFromGuid == l_bind.m_strFromGuid)
				{
					l_tempData.m_strType = l_bindInfo.m_strType;
					l_tempData.m_strGuid = l_bindInfo.m_strGuid;
					l_tempData.m_strFromGuid = l_bindInfo.m_strFromGuid;
					l_tempData.m_strToGuid = l_bindInfo.m_strToGuid;
					m_vecTempBind.push_back(l_tempData);

					l_oDeleteSQLReq.param["type"] = l_bindInfo.m_strType;
					m_vecDelete.push_back(l_bindInfo.m_strGuid);
				}
			}

			l_oDeleteSQLReq.param["from_guid"] = l_bind.m_strFromGuid;
			l_oDeleteSQLReq.param["type"] = l_bind.m_strType;
		}
		else
		{
			//删除非用户绑定警员关系
			std::string l_strBindInfo;
			if (!m_pRedisClient->HGet(BIND_INFO, l_bind.m_strGuid, l_strBindInfo))
			{
				ICC_LOG_ERROR(m_pLog, "Not Found BindInfo:From[%s] To[%s]", l_bind.m_strFromGuid.c_str(), l_bind.m_strToGuid.c_str());
				continue;
			}
			Data::CBindInfo l_bindInfo;
			l_bindInfo.Parse(l_strBindInfo, m_pJsonFty->CreateJson());
			m_vecDelete.push_back(l_bindInfo.m_strGuid);

			l_tempData.m_strType = l_bindInfo.m_strType;
			l_tempData.m_strGuid = l_bindInfo.m_strGuid;
			l_tempData.m_strFromGuid = l_bindInfo.m_strFromGuid;
			l_tempData.m_strToGuid = l_bindInfo.m_strToGuid;
			m_vecTempBind.push_back(l_tempData);

			l_oDeleteSQLReq.param["from_guid"] = l_bind.m_strFromGuid;
			l_oDeleteSQLReq.param["to_guid"] = l_bind.m_strToGuid;
		}

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oDeleteSQLReq, false, l_strGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "delete bind info error, rollback: [%s]", l_pRSet->GetErrorMsg().c_str());
			m_pDBConn->Rollback(l_strGuid);
			return false;
		}
	}
	m_pDBConn->Commit(l_strGuid);
	return true;
}

bool CBind::_HSet()
{
	if (m_vecBind.empty())
	{
		return true;
	}

	std::map<std::string, std::string> tmp_mapInfo;
	for (Data::CBindInfo var : m_vecBind)
	{
		tmp_mapInfo[var.m_strGuid] = var.ToJson(m_pJsonFty->CreateJson());
	}

	bool bSet = m_pRedisClient->HMSet(BIND_INFO, tmp_mapInfo);

	ICC_LOG_DEBUG(m_pLog, "set bind info,result[%s] [%s]", bSet ? "true" : "false", m_vecBind.size() > 0 ? m_vecBind[0].m_strGuid.c_str() : "");

	return bSet;
}

void CBind::_HDel()
{
	if (!m_vecDelete.empty())
	{
		m_pRedisClient->HDelFields(BIND_INFO, m_vecDelete);
		ICC_LOG_DEBUG(m_pLog, "delete bind info, [%s]", m_vecDelete.size() > 0 ? m_vecDelete[0].c_str() : "");
	}
}
