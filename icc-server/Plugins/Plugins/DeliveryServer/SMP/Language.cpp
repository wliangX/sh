#include "Boost.h"
#include "Language.h"

using namespace ICC;
using namespace Data;

CLanguage::CLanguage(std::vector<Data::CLanguageInfo>& p_vecLanguageInfo, IResourceManagerPtr p_pIResourceManager) :
m_vecLanguage(p_vecLanguageInfo),
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

CLanguage::~CLanguage()
{

}

void CLanguage::Append(std::vector<Data::CLanguageInfo>& p_vecInfo)
{
	int iCount = p_vecInfo.size();
	for (int i = 0; i < iCount; ++i)
	{
		m_vecLanguage.push_back(p_vecInfo[i]);
	}
}

bool CLanguage::SynUpdate()
{
	if (m_vecLanguage.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no language data need update!!");
		return true;
	}

	std::map<std::string, std::string> mapOldDatas;
	if (!_GetAllOldData(mapOldDatas))
	{
		ICC_LOG_ERROR(m_pLog, "get all old datas failed!!");
		return false;
	}
	
	ICC_LOG_DEBUG(m_pLog, "get all old datas success! size[%d] ", mapOldDatas.size());

	std::vector<Data::CLanguageInfo> addDatas;
	std::vector<Data::CLanguageInfo> updateDatas;
	std::vector<Data::CLanguageInfo> deleteDatas;
	_CompareData(mapOldDatas, m_vecLanguage, addDatas, updateDatas, deleteDatas);

	if (!_UpdateData(addDatas, updateDatas, deleteDatas))
	{
		LoadData();
		return false;
	}

	return true;	
}


bool CLanguage::SingleAdd()
{
	if (m_vecLanguage.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no language data need add!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "will add language data size[%d]!!", m_vecLanguage.size());

	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	std::string l_strGuid = m_pDBConn->BeginTransaction();

	if (!_Add(m_vecLanguage, l_strGuid))
	{
		return false;
	}

	m_pDBConn->Commit(l_strGuid);

	ICC_LOG_DEBUG(m_pLog, "add language data complete! ");

	return true;
}

bool CLanguage::SingleModify()
{
	if (m_vecLanguage.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no language data need update!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "will language func data size[%d]!!", m_vecLanguage.size());
	
	std::string l_strGuid = m_pDBConn->BeginTransaction();

	if (!_Update(m_vecLanguage, l_strGuid))
	{
		return false;
	}

	m_pDBConn->Commit(l_strGuid);

	ICC_LOG_DEBUG(m_pLog, "update func data complete! ");

	return true;
}

bool CLanguage::SingleDelete()
{
	if (m_vecLanguage.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no language data need delete!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "will delete language data size[%d]!!", m_vecLanguage.size());
	
	std::string l_strGuid = m_pDBConn->BeginTransaction();	

	if (!_Del(m_vecLanguage, l_strGuid))
	{
		return false;
	}

	m_pDBConn->Commit(l_strGuid);

	ICC_LOG_DEBUG(m_pLog, "delete language data complete! ");

	return true;
}



void CLanguage::AppendReceiveDataSize(int iReceiveSize)
{
	m_iTotalReceiveDataSize += iReceiveSize;
}

int CLanguage::ReceiveTotalDataSize()
{
	return m_iTotalReceiveDataSize;
}

bool CLanguage::LoadData()
{
	ICC_LOG_DEBUG(m_pLog, "load LanguageInfo to redis Begin");

	std::map<std::string, std::string> mapTmps;
	if (!_GetAllOldData(mapTmps))
	{
		ICC_LOG_ERROR(m_pLog, "get LanguageInfo from redis failed!!");
		return false;
	}

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_language";
	l_SqlRequest.param["is_delete"] = "false";	

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		return false;
	}
	while (l_result->Next())
	{
		Data::CLanguageInfo l_oLanguage;
		l_oLanguage.m_strGuid = l_result->GetValue("guid");
		l_oLanguage.m_strName = l_result->GetValue("name");
		l_oLanguage.m_strSort = l_result->GetValue("sort");
		l_oLanguage.m_strCode = l_result->GetValue("code");

		JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
		if (!l_pJson)
		{
			ICC_LOG_ERROR(m_pLog, "set language info to redis failed! [json is null]");
			return false;
		}

		bool bNeedWrite = false;

		std::map<std::string, std::string>::iterator itr;
		itr = mapTmps.find(l_oLanguage.m_strGuid);
		if (itr != mapTmps.end())
		{			
			Data::CLanguageInfo oldInfo;
			if (!oldInfo.Parse(itr->second, m_pJsonFty->CreateJson()))
			{
				bNeedWrite =  true;
			}
			else
			{
				if (!_IsEqual(oldInfo, l_oLanguage))
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
			std::string l_strVal = l_oLanguage.ToJson(l_pJson);
			bool bSet = m_pRedisClient->HSet(LANGUAGE_KEY, l_oLanguage.m_strGuid, l_strVal);
			if (!bSet)
			{
				ICC_LOG_ERROR(m_pLog, "set language info to redis failed! [%s : %s]", l_oLanguage.m_strGuid.c_str(), l_strVal.c_str());
				return false;
			}
		}
	}

	std::map<std::string, std::string>::const_iterator itr_const;
	for (itr_const = mapTmps.begin(); itr_const != mapTmps.end(); ++itr_const)
	{
		bool bSet = m_pRedisClient->HDel(LANGUAGE_KEY, itr_const->first);
		if (!bSet)
		{
			ICC_LOG_ERROR(m_pLog, "delete language info from redis failed! [%s]", itr_const->first.c_str());
			return false;
		}
	}

	ICC_LOG_DEBUG(m_pLog, "LodingLanguageInfo Success,Language Size[%d]", l_result->RecordSize());

	return true;
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
bool CLanguage::_GetAllOldData(std::map<std::string, std::string>& mapOldDatas)
{
	if (m_pRedisClient->HGetAllEx(LANGUAGE_KEY, mapOldDatas))
	{
		return true;
	}

	ICC_LOG_ERROR(m_pLog, "get all old language datas failed!!!");

	return false;
}

bool CLanguage::_CompareData(std::map<std::string, std::string>& mapOldDatas, const std::vector<Data::CLanguageInfo>& vecNewDatas,
	std::vector<Data::CLanguageInfo>& addDatas, std::vector<Data::CLanguageInfo>& updateDatas, std::vector<Data::CLanguageInfo>& deleteDatas)
{
	int iNewDataCount = vecNewDatas.size();
	for (int i = 0; i < iNewDataCount; ++i)
	{
		std::map<std::string, std::string>::iterator itr = mapOldDatas.find(vecNewDatas[i].m_strGuid);
		if (itr != mapOldDatas.end())
		{
			Data::CLanguageInfo oldInfo;
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
		Data::CLanguageInfo oldInfo;
		if (!oldInfo.Parse(itr_const->second, m_pJsonFty->CreateJson()))
		{
			return false;
		}

		deleteDatas.push_back(oldInfo);
	}

	return true;
}

bool CLanguage::_Add(const std::vector<Data::CLanguageInfo>& vecDatas, const std::string& strTransGuid)
{
	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());

	for (Data::CLanguageInfo l_language : vecDatas)
	{
		DataBase::SQLRequest l_oSetSQLReq;
		l_oSetSQLReq.sql_id = "insert_icc_t_language";
		l_oSetSQLReq.param["guid"] = l_language.m_strGuid;
		l_oSetSQLReq.param["name"] = l_language.m_strName;
		l_oSetSQLReq.param["code"] = l_language.m_strCode;
		l_oSetSQLReq.param["sort"] = l_language.m_strSort;
		l_oSetSQLReq.param["is_delete"] = "false";
		l_oSetSQLReq.param["create_user"] = "smp_gateway";
		l_oSetSQLReq.param["create_time"] = l_strCurrentTime;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSetSQLReq, false, strTransGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "add language data to db failed! sql[%s]", l_pRSet->GetSQL().c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "add language data to db success!!! sql[%s]", l_pRSet->GetSQL().c_str());

		std::string strTmp = l_language.ToJson(m_pJsonFty->CreateJson());
		if (!m_pRedisClient->HSet(LANGUAGE_KEY, l_language.m_strGuid, strTmp))
		{
			ICC_LOG_ERROR(m_pLog, "add language data to redis failed! content[%s]", strTmp.c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "add language data to redis success! content[%s]", strTmp.c_str());
	}
	return true;
}

bool CLanguage::_Update(const std::vector<Data::CLanguageInfo>& vecDatas, const std::string& strTransGuid)
{
	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());

	for (Data::CLanguageInfo l_language : vecDatas)
	{
		DataBase::SQLRequest l_oSetSQLReq;
		l_oSetSQLReq.sql_id = "update_icc_t_language";
		l_oSetSQLReq.set["name"] = l_language.m_strName;
		l_oSetSQLReq.set["code"] = l_language.m_strCode;
		l_oSetSQLReq.set["sort"] = l_language.m_strSort;
		l_oSetSQLReq.set["update_user"] = "smp_gateway";
		l_oSetSQLReq.set["update_time"] = l_strCurrentTime;
		l_oSetSQLReq.param["guid"] = l_language.m_strGuid;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSetSQLReq, false, strTransGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "update language data to db failed! sql[%s]", l_pRSet->GetSQL().c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "update language data to db success!!! sql[%s]", l_pRSet->GetSQL().c_str());

		std::string strTmp = l_language.ToJson(m_pJsonFty->CreateJson());
		if (!m_pRedisClient->HSet(LANGUAGE_KEY, l_language.m_strGuid, strTmp))
		{
			ICC_LOG_ERROR(m_pLog, "update language data to redis failed! content[%s]", strTmp.c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "update language data to redis success! content[%s]", strTmp.c_str());
	}

	return true;
}

bool CLanguage::_Del(const std::vector<Data::CLanguageInfo>& vecDatas, const std::string& strTransGuid)
{
	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());

	for (Data::CLanguageInfo l_language : vecDatas)
	{
		DataBase::SQLRequest l_oDeleteSQLReq;
		l_oDeleteSQLReq.sql_id = "delete_icc_t_language";
		l_oDeleteSQLReq.param["guid"] = l_language.m_strGuid;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oDeleteSQLReq, false, strTransGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "delete language data to db failed! sql[%s]", l_pRSet->GetSQL().c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_ERROR(m_pLog, "delete language data to db success!!! sql[%s]", l_pRSet->GetSQL().c_str());

		if (!m_pRedisClient->HDel(LANGUAGE_KEY, l_language.m_strGuid))
		{
			ICC_LOG_ERROR(m_pLog, "delete language data from redis failed! guid[%s]", l_language.m_strGuid.c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "delete language data from redis success! guid[%s]", l_language.m_strGuid.c_str());
	}

	return true;
}

bool CLanguage::_UpdateData(const std::vector<Data::CLanguageInfo>& addDatas, const std::vector<Data::CLanguageInfo>& updateDatas, const std::vector<Data::CLanguageInfo>& deleteDatas)
{
	ICC_LOG_DEBUG(m_pLog, "update language data begin! addDatas[%d], updateDatas[%d], deleteDatas[%d]", 
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

	ICC_LOG_DEBUG(m_pLog, "update language data complete! ");

	return true;	
}


bool CLanguage::_Delete()
{
	if (m_vecLanguage.empty())
	{
		return true;
	}

	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	std::string l_strGuid = m_pDBConn->BeginTransaction();
	for (Data::CLanguageInfo l_language : m_vecLanguage)
	{
		DataBase::SQLRequest l_oDeleteSQLReq;
		l_oDeleteSQLReq.sql_id = "delete_icc_t_language";
		l_oDeleteSQLReq.param["guid"] = l_language.m_strGuid;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oDeleteSQLReq, false, l_strGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "delete language info error, rollback: [%s]", l_pRSet->GetErrorMsg().c_str());
			m_pDBConn->Rollback(l_strGuid);
			return false;
		}
	}

	m_pDBConn->Commit(l_strGuid);
	return true;
}

bool CLanguage::_HSet()
{
	if (m_vecLanguage.empty())
	{
		return true;
	}

	std::map<std::string, std::string> tmp_mapInfo;
	for (Data::CLanguageInfo l_language : m_vecLanguage)
	{
		tmp_mapInfo[l_language.m_strGuid] = l_language.ToJson(m_pJsonFty->CreateJson());
	}

	return m_pRedisClient->HMSet(LANGUAGE_KEY, tmp_mapInfo);
}

void CLanguage::_HDel()
{
	for (Data::CLanguageInfo l_language : m_vecLanguage)
	{
		m_pRedisClient->HDel(LANGUAGE_KEY, l_language.m_strGuid);
	}
}

bool CLanguage::_IsEqual(Data::CLanguageInfo p_old, Data::CLanguageInfo l_new)
{
	if (p_old.m_strCode == l_new.m_strCode &&
		p_old.m_strName == l_new.m_strName &&
		p_old.m_strSort == l_new.m_strSort)
	{

		return true;
	}
	return false;
}

