#include "Boost.h"
#include "Staff.h"

using namespace ICC;
using namespace Data;

CStaff::CStaff(std::vector<Data::CStaffInfo> p_vecStaffInfo, IResourceManagerPtr p_pIResourceManager) :
m_vecStaff(p_vecStaffInfo),
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

CStaff::~CStaff()
{

}

int CStaff::ReceiveTotalDataSize()
{
	return (int)m_vecStaff.size();
}

void CStaff::AppendReceiveDataSize(int iReceiveSize)
{
	m_iTotalReceiveDataSize += iReceiveSize;
}

void CStaff::Append(std::vector<Data::CStaffInfo>& p_vecInfo)
{
	int iCount = p_vecInfo.size();
	for (int i = 0; i < iCount; ++i)
	{
		m_vecStaff.push_back(p_vecInfo[i]);
	}
}

bool CStaff::SynUpdate()
{
	/*if (m_vecStaff.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no staff data need update!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "begin update staff data, size[%d]", m_vecStaff.size());

	std::map<std::string, std::string> mapGuidMaps;
	std::map<std::string, std::string> mapCodeMaps;
	for (Data::CStaffInfo staffInfo : m_vecStaff)
	{
		std::string strTmp = staffInfo.ToJson(m_pJsonFty->CreateJson());
		mapGuidMaps[staffInfo.m_strGuid] = strTmp;
		mapCodeMaps[staffInfo.m_strCode] = strTmp;
	}

	if (!m_pRedisClient->HMSet(STAFF_INFO_KEY, mapGuidMaps))
	{
		ICC_LOG_DEBUG(m_pLog, "set staff key failed!!");
		return false;
	}

	if (!m_pRedisClient->HMSet(STAFF_INFO_MAP_KEY, mapCodeMaps))
	{
		ICC_LOG_DEBUG(m_pLog, "set staff key failed!!");
		return false;
	}

	return true;*/


	if (m_vecStaff.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no staff data need update!!");
		return true;
	}

	std::map<std::string, std::string> mapOldDatas;
	if (!_GetAllOldData(mapOldDatas))
	{
		ICC_LOG_ERROR(m_pLog, "get all staff old datas failed!!");
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "get all staff old datas success! size[%d] ", mapOldDatas.size());

	std::vector<Data::CStaffInfo> addDatas;
	std::vector<Data::CStaffInfo> updateDatas;
	std::vector<Data::CStaffInfo> deleteDatas;
	_CompareData(mapOldDatas, m_vecStaff, addDatas, updateDatas, deleteDatas);

	if (!_UpdateData(addDatas, updateDatas, deleteDatas))
	{
		//LoadData();
		return false;
	}

	return true;	
}


bool CStaff::SingleAdd()
{
	if (m_vecStaff.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no staff data need add!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "will add staff data size[%d]!!", m_vecStaff.size());

	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	std::string l_strGuid = m_pDBConn->BeginTransaction();

	if (!_Add(m_vecStaff, l_strGuid))
	{
		return false;
	}		

	m_pDBConn->Commit(l_strGuid);

	ICC_LOG_DEBUG(m_pLog, "add staff data complete! ");

	return true;
}

bool CStaff::SingleModify()
{
	if (m_vecStaff.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no staff data need update!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "will update staff data size[%d]!!", m_vecStaff.size());

	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	std::string l_strGuid = m_pDBConn->BeginTransaction();
	
	if (!_Update(m_vecStaff, l_strGuid))
	{
		return false;
	}
	
	m_pDBConn->Commit(l_strGuid);

	ICC_LOG_DEBUG(m_pLog, "update staff data complete! ");

	return true;
}

bool CStaff::SingleDelete()
{
	if (m_vecStaff.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no staff data need delete!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "will delete staff data size[%d]!!", m_vecStaff.size());

	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	std::string l_strGuid = m_pDBConn->BeginTransaction();

	if (!_Del(m_vecStaff, l_strGuid))
	{
		return false;
	}

	m_pDBConn->Commit(l_strGuid);

	ICC_LOG_DEBUG(m_pLog, "delete staff data complete! ");

	return true;
}




DataBase::SQLRequest CStaff::ConstructSQLStruct(Data::CStaffInfo& p_staffInfo, std::string p_strSqlID)
{
	DataBase::SQLRequest l_oSQLExec;

	if (p_strSqlID == "select_icc_t_staff")
	{
		l_oSQLExec.sql_id = "select_icc_t_staff";
		l_oSQLExec.param["code"] = p_staffInfo.m_strCode;
		//l_oSQLExec.param["is_delete"] = "true";
	}
	else if (p_strSqlID == "update_icc_t_staff")
	{
		l_oSQLExec.sql_id = "update_icc_t_staff";
		l_oSQLExec.set["dept_guid"] = p_staffInfo.m_strDeptGuid;
		l_oSQLExec.set["code"] = p_staffInfo.m_strCode;
		l_oSQLExec.set["name"] = p_staffInfo.m_strName;
		l_oSQLExec.set["sex"] = p_staffInfo.m_strSex;
		l_oSQLExec.set["type"] = p_staffInfo.m_strType;
		l_oSQLExec.set["post"] = p_staffInfo.m_strPosition;
		l_oSQLExec.set["mobile"] = p_staffInfo.m_strMobile;
		l_oSQLExec.set["phone"] = p_staffInfo.m_strPhone;
		l_oSQLExec.set["shortcut"] = p_staffInfo.m_strShortcut;
		l_oSQLExec.set["sort"] = p_staffInfo.m_strSort;
		l_oSQLExec.set["isleader"] = p_staffInfo.m_strIsLeader;

		l_oSQLExec.set["update_user"] = "smp_gateway";
		l_oSQLExec.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
		l_oSQLExec.param["guid"] = p_staffInfo.m_strGuid;
	}
	else if (p_strSqlID == "recover_icc_t_staff_flag")
	{
		l_oSQLExec.sql_id = "update_icc_t_staff";
		l_oSQLExec.set["guid"] = p_staffInfo.m_strGuid;
		l_oSQLExec.set["dept_guid"] = p_staffInfo.m_strDeptGuid;
		l_oSQLExec.set["name"] = p_staffInfo.m_strName;
		l_oSQLExec.set["sex"] = p_staffInfo.m_strSex;
		l_oSQLExec.set["type"] = p_staffInfo.m_strType;
		l_oSQLExec.set["post"] = p_staffInfo.m_strPosition;
		l_oSQLExec.set["mobile"] = p_staffInfo.m_strMobile;
		l_oSQLExec.set["phone"] = p_staffInfo.m_strPhone;
		l_oSQLExec.set["shortcut"] = p_staffInfo.m_strShortcut;
		l_oSQLExec.set["sort"] = p_staffInfo.m_strSort;
		l_oSQLExec.set["isleader"] = p_staffInfo.m_strIsLeader;
		l_oSQLExec.set["is_delete"] = "false";

		l_oSQLExec.set["update_user"] = "smp_gateway";
		l_oSQLExec.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
		l_oSQLExec.param["code"] = p_staffInfo.m_strCode;
	}
	else if (p_strSqlID == "insert_icc_t_staff")
	{
		l_oSQLExec.sql_id = "insert_icc_t_staff";
		l_oSQLExec.param["guid"] = p_staffInfo.m_strGuid;
		l_oSQLExec.param["dept_guid"] = p_staffInfo.m_strDeptGuid;
		l_oSQLExec.param["code"] = p_staffInfo.m_strCode;
		l_oSQLExec.param["name"] = p_staffInfo.m_strName;
		l_oSQLExec.param["sex"] = p_staffInfo.m_strSex;
		l_oSQLExec.param["type"] = p_staffInfo.m_strType;
		l_oSQLExec.param["post"] = p_staffInfo.m_strPosition;
		l_oSQLExec.param["mobile"] = p_staffInfo.m_strMobile;
		l_oSQLExec.param["phone"] = p_staffInfo.m_strPhone;
		l_oSQLExec.param["is_delete"] = "false";
		l_oSQLExec.param["shortcut"] = p_staffInfo.m_strShortcut;
		l_oSQLExec.param["sort"] = p_staffInfo.m_strSort;
		l_oSQLExec.param["isleader"] = p_staffInfo.m_strIsLeader;
		l_oSQLExec.param["create_user"] = "smp_gateway";
		l_oSQLExec.param["create_time"] = m_pDateTime->CurrentDateTimeStr();
	}
	else if (p_strSqlID == "delete_icc_t_staff")
	{
		l_oSQLExec.sql_id = "update_icc_t_staff";
		l_oSQLExec.set["is_delete"] = "true";
		l_oSQLExec.param["guid"] = p_staffInfo.m_strGuid;
		l_oSQLExec.set["update_user"] = "smp_gateway";
		l_oSQLExec.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
	}

	return l_oSQLExec;
}

bool CStaff::LoadData()
{
	ICC_LOG_DEBUG(m_pLog, "load staff from redis begin");	

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_staff";
	l_SqlRequest.param["is_delete"] = "false";

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "execquery error ,error message :[%s]", l_result->GetErrorMsg().c_str());
		return false;
	}

	std::vector<CStaffInfo> vecDatas;
	
	while (l_result->Next())
	{
		CStaffInfo l_oLoadStaff;
		l_oLoadStaff.m_strGuid = l_result->GetValue("guid");
		l_oLoadStaff.m_strDeptGuid = l_result->GetValue("dept_guid");
		l_oLoadStaff.m_strCode = l_result->GetValue("code");
		l_oLoadStaff.m_strName = l_result->GetValue("name");
		l_oLoadStaff.m_strSex = l_result->GetValue("sex");
		l_oLoadStaff.m_strType = l_result->GetValue("type");
		l_oLoadStaff.m_strPosition = l_result->GetValue("post");
		l_oLoadStaff.m_strMobile = l_result->GetValue("mobile");
		l_oLoadStaff.m_strPhone = l_result->GetValue("phone");
		l_oLoadStaff.m_strShortcut = l_result->GetValue("shortcut");
		l_oLoadStaff.m_strSort = l_result->GetValue("sort");
		l_oLoadStaff.m_strIsLeader = l_result->GetValue("isleader");		
		vecDatas.push_back(l_oLoadStaff);
	}

	int iDelCount = m_pRedisClient->Del(STAFF_INFO_KEY);
	ICC_LOG_DEBUG(m_pLog, "del STAFF_INFO_KEY success, size[%d]", iDelCount);

	iDelCount = m_pRedisClient->Del(STAFF_INFO_MAP_KEY);
	ICC_LOG_DEBUG(m_pLog, "del STAFF_INFO_MAP_KEY success, size[%d]", iDelCount);

	if (!_UpdateDateToRedis(vecDatas))
	{
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "load staff to redis end, staff size[%d]", l_result->RecordSize());
	return true;


	/*std::map<std::string, std::string> mapTmps;
	if (!_GetAllOldData(mapTmps))
	{
		ICC_LOG_ERROR(m_pLog, "get LanguageInfo from redis failed!!");
		return false;
	}

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_staff";
	l_SqlRequest.param["is_delete"] = "false";

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "execquery error ,error message :[%s]", l_result->GetErrorMsg().c_str());
		return false;
	}	

	std::map<std::string, std::string> tmp_mapStaffInfo;
	int l_iCount = 0;
	while (l_result->Next())
	{
		CStaffInfo l_oLoadStaff;
		l_oLoadStaff.m_strGuid = l_result->GetValue("guid");
		l_oLoadStaff.m_strDeptGuid = l_result->GetValue("dept_guid");
		l_oLoadStaff.m_strCode = l_result->GetValue("code");
		l_oLoadStaff.m_strName = l_result->GetValue("name");
		l_oLoadStaff.m_strSex = l_result->GetValue("sex");
		l_oLoadStaff.m_strType = l_result->GetValue("type");
		l_oLoadStaff.m_strPosition = l_result->GetValue("post");
		l_oLoadStaff.m_strMobile = l_result->GetValue("mobile");
		l_oLoadStaff.m_strPhone = l_result->GetValue("phone");
		l_oLoadStaff.m_strShortcut = l_result->GetValue("shortcut");
		l_oLoadStaff.m_strSort = l_result->GetValue("sort");
		l_oLoadStaff.m_strIsLeader = l_result->GetValue("isleader");

		bool bNeedWrite = false;

		std::map<std::string, std::string>::iterator itr;
		itr = mapTmps.find(l_oLoadStaff.m_strGuid);
		if (itr != mapTmps.end())
		{
			Data::CStaffInfo oldInfo;
			if (!oldInfo.Parse(itr->second, m_pJsonFty->CreateJson()))
			{
				bNeedWrite = true;
			}
			else
			{
				if (!_IsEqual(oldInfo, l_oLoadStaff))
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
			std::string l_strVal = l_oLoadStaff.ToJson(m_pJsonFty->CreateJson());
			bool bSet = m_pRedisClient->HSet(STAFF_INFO_KEY, l_oLoadStaff.m_strGuid, l_strVal);
			if (!bSet)
			{
				ICC_LOG_ERROR(m_pLog, "set staff info to redis failed! [%s : %s]", l_oLoadStaff.m_strGuid.c_str(), l_strVal.c_str());
				return false;
			}			

			if (!m_pRedisClient->HSet(STAFF_INFO_MAP_KEY, l_oLoadStaff.m_strCode, l_strVal))
			{
				ICC_LOG_ERROR(m_pLog, "add staff data to redis failed! content[%s]", l_strVal.c_str());
				return false;
			}
		}

	}

	std::map<std::string, std::string>::const_iterator itr_const;
	for (itr_const = mapTmps.begin(); itr_const != mapTmps.end(); ++itr_const)
	{
		bool bSet = m_pRedisClient->HDel(STAFF_INFO_KEY, itr_const->first);
		if (!bSet)
		{
			ICC_LOG_ERROR(m_pLog, "delete staff info from redis failed! [%s]", itr_const->first.c_str());
			return false;
		}

		Data::CStaffInfo oldInfo;
		if (!oldInfo.Parse(itr_const->second, m_pJsonFty->CreateJson()))
		{
			return false;;
		}

		if (!m_pRedisClient->HDel(STAFF_INFO_MAP_KEY, oldInfo.m_strCode))
		{
			ICC_LOG_ERROR(m_pLog, "delete staff map data from redis failed! code[%s]", oldInfo.m_strCode.c_str());			
			return false;
		}
	}	

	ICC_LOG_DEBUG(m_pLog, "loading staff info success, staff size[%d]", l_result->RecordSize());
	return true;*/
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
bool CStaff::_GetAllOldData(std::map<std::string, std::string>& mapOldDatas)
{
	if (m_pRedisClient->HGetAllEx(STAFF_INFO_KEY, mapOldDatas))
	{
		return true;
	}

	ICC_LOG_ERROR(m_pLog, "get all old staff datas failed!!!");

	return false;
}

bool CStaff::_CompareData(std::map<std::string, std::string>& mapOldDatas, const std::vector<Data::CStaffInfo>& vecNewDatas,
	std::vector<Data::CStaffInfo>& addDatas, std::vector<Data::CStaffInfo>& updateDatas, std::vector<Data::CStaffInfo>& deleteDatas)
{
	int iNewDataCount = vecNewDatas.size();
	for (int i = 0; i < iNewDataCount; ++i)
	{
		std::map<std::string, std::string>::iterator itr = mapOldDatas.find(vecNewDatas[i].m_strGuid);
		if (itr != mapOldDatas.end())
		{
			Data::CStaffInfo oldInfo;
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
		Data::CStaffInfo oldInfo;
		if (!oldInfo.Parse(itr_const->second, m_pJsonFty->CreateJson()))
		{
			return false;
		}

		deleteDatas.push_back(oldInfo);
	}

	return true;
}

bool CStaff::_UpdateDateToRedis(const std::vector<Data::CStaffInfo>& vecDatas)
{
	std::map<std::string, std::string> mapGuidDatas;
	std::map<std::string, std::string> mapCodeDatas;
	for (Data::CStaffInfo p_staffInfo : vecDatas)
	{
		std::string strTmp = p_staffInfo.ToJson(m_pJsonFty->CreateJson());
		mapGuidDatas.insert(std::make_pair(p_staffInfo.m_strGuid, strTmp));
		mapCodeDatas.insert(std::make_pair(p_staffInfo.m_strCode, strTmp));
	}

	if (!mapGuidDatas.empty())
	{
		if (!m_pRedisClient->HMSet(STAFF_INFO_KEY, mapGuidDatas))
		{
			ICC_LOG_ERROR(m_pLog, "add staff data to STAFF_INFO_KEY failed!");
			return false;
		}
	}

	if (!mapCodeDatas.empty())
	{
		if (!m_pRedisClient->HMSet(STAFF_INFO_MAP_KEY, mapCodeDatas))
		{
			ICC_LOG_ERROR(m_pLog, "add staff data to STAFF_INFO_MAP_KEY failed!");
			return false;
		}
	}	

	ICC_LOG_DEBUG(m_pLog, "add staff data to redis success! content count[%d]", vecDatas.size());

	return true;
}

bool CStaff::_Add(const std::vector<Data::CStaffInfo>& vecDatas, const std::string& strTransGuid)
{
	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());

	std::map<std::string, std::string> mapTmpDatas;

	for (Data::CStaffInfo p_staffInfo : vecDatas)
	{
		if (mapTmpDatas.find(p_staffInfo.m_strGuid) != mapTmpDatas.end())
		{
			ICC_LOG_WARNING(m_pLog, "same staff data! guid[%s]", p_staffInfo.m_strGuid.c_str());
			continue;
		}

		DataBase::SQLRequest l_oSetSQLReq;
		l_oSetSQLReq.sql_id = "insert_icc_t_staff";
		l_oSetSQLReq.param["guid"] = p_staffInfo.m_strGuid;
		l_oSetSQLReq.param["dept_guid"] = p_staffInfo.m_strDeptGuid;
		l_oSetSQLReq.param["code"] = p_staffInfo.m_strCode;
		l_oSetSQLReq.param["name"] = p_staffInfo.m_strName;
		l_oSetSQLReq.param["sex"] = p_staffInfo.m_strSex;
		l_oSetSQLReq.param["type"] = p_staffInfo.m_strType;
		l_oSetSQLReq.param["post"] = p_staffInfo.m_strPosition;
		l_oSetSQLReq.param["mobile"] = p_staffInfo.m_strMobile;
		l_oSetSQLReq.param["phone"] = p_staffInfo.m_strPhone;
		l_oSetSQLReq.param["is_delete"] = "false";
		l_oSetSQLReq.param["shortcut"] = p_staffInfo.m_strShortcut;
		l_oSetSQLReq.param["sort"] = p_staffInfo.m_strSort;
		l_oSetSQLReq.param["isleader"] = p_staffInfo.m_strIsLeader;
		l_oSetSQLReq.param["create_user"] = "smp_gateway";
		l_oSetSQLReq.param["create_time"] = m_pDateTime->CurrentDateTimeStr();

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSetSQLReq, false, strTransGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "add staff data to db failed! sql[%s]", l_pRSet->GetSQL().c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;						
		}

		ICC_LOG_DEBUG(m_pLog, "add staff data to db success!!! sql[%s]", l_pRSet->GetSQL().c_str());

		mapTmpDatas.insert(std::make_pair(p_staffInfo.m_strGuid, p_staffInfo.m_strGuid));


		/*std::string strTmp = p_staffInfo.ToJson(m_pJsonFty->CreateJson());
		if (!m_pRedisClient->HSet(STAFF_INFO_KEY, p_staffInfo.m_strGuid, strTmp))
		{
			ICC_LOG_ERROR(m_pLog, "add staff data to redis failed! content[%s]", strTmp.c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "add staff data to redis success! content[%s]", strTmp.c_str());

		if (!m_pRedisClient->HSet(STAFF_INFO_MAP_KEY, p_staffInfo.m_strCode, strTmp))
		{
			ICC_LOG_ERROR(m_pLog, "add staff data to redis failed! content[%s]", strTmp.c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "add staff data to redis success! content[%s]", strTmp.c_str());*/
	}

	if (!_UpdateDateToRedis(vecDatas))
	{
		m_pDBConn->Rollback(strTransGuid);
		return false;
	}

	return true;
}

bool CStaff::_UpdateDataToDb(const Data::CStaffInfo& p_staffInfo, const std::string& strTransGuid)
{
	DataBase::SQLRequest l_oSetSQLReq;
	l_oSetSQLReq.sql_id = "update_icc_t_staff";
	l_oSetSQLReq.set["guid"] = p_staffInfo.m_strGuid;
	l_oSetSQLReq.set["dept_guid"] = p_staffInfo.m_strDeptGuid;
	l_oSetSQLReq.set["name"] = p_staffInfo.m_strName;
	l_oSetSQLReq.set["sex"] = p_staffInfo.m_strSex;
	l_oSetSQLReq.set["type"] = p_staffInfo.m_strType;
	l_oSetSQLReq.set["post"] = p_staffInfo.m_strPosition;
	l_oSetSQLReq.set["mobile"] = p_staffInfo.m_strMobile;
	l_oSetSQLReq.set["phone"] = p_staffInfo.m_strPhone;
	l_oSetSQLReq.set["shortcut"] = p_staffInfo.m_strShortcut;
	l_oSetSQLReq.set["sort"] = p_staffInfo.m_strSort;
	l_oSetSQLReq.set["isleader"] = p_staffInfo.m_strIsLeader;
	l_oSetSQLReq.set["is_delete"] = "false";

	l_oSetSQLReq.set["update_user"] = "smp_gateway";
	l_oSetSQLReq.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
	l_oSetSQLReq.param["code"] = p_staffInfo.m_strCode;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSetSQLReq, false, strTransGuid);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update staff data to db failed! sql[%s]", l_pRSet->GetSQL().c_str());		
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "update staff data to db success!!! sql[%s]", l_pRSet->GetSQL().c_str());

	return true;
}

bool CStaff::_Update(const std::vector<Data::CStaffInfo>& vecDatas, const std::string& strTransGuid)
{
	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());

	for (Data::CStaffInfo p_staffInfo : vecDatas)
	{
		if (!_UpdateDataToDb(p_staffInfo, strTransGuid))
		{
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		/*std::string strTmp = p_staffInfo.ToJson(m_pJsonFty->CreateJson());
		if (!m_pRedisClient->HSet(STAFF_INFO_KEY, p_staffInfo.m_strGuid, strTmp))
		{
			ICC_LOG_ERROR(m_pLog, "update staff data to redis failed! content[%s]", strTmp.c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "update staff data to redis success! content[%s]", strTmp.c_str());

		if (!m_pRedisClient->HSet(STAFF_INFO_MAP_KEY, p_staffInfo.m_strCode, strTmp))
		{
			ICC_LOG_ERROR(m_pLog, "add staff data to redis failed! content[%s]", strTmp.c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "add staff data to redis success! content[%s]", strTmp.c_str());*/
	}

	if (!_UpdateDateToRedis(vecDatas))
	{
		m_pDBConn->Rollback(strTransGuid);
		return false;
	}

	return true;
}

bool CStaff::_Del(const std::vector<Data::CStaffInfo>& vecDatas, const std::string& strTransGuid)
{
	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());

	for (Data::CStaffInfo p_staffInfo : vecDatas)
	{
		DataBase::SQLRequest l_oDeleteSQLReq;
		l_oDeleteSQLReq.sql_id = "delete_icc_t_staff";
		l_oDeleteSQLReq.param["guid"] = p_staffInfo.m_strGuid;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oDeleteSQLReq, false, strTransGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "delete staff data to db failed! sql[%s]", l_pRSet->GetSQL().c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_ERROR(m_pLog, "delete staff data to db success!!! sql[%s]", l_pRSet->GetSQL().c_str());

		if (!m_pRedisClient->HDel(STAFF_INFO_KEY, p_staffInfo.m_strGuid))
		{
			ICC_LOG_ERROR(m_pLog, "delete staff data from redis failed! guid[%s]", p_staffInfo.m_strGuid.c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "delete staff data from redis success! guid[%s]", p_staffInfo.m_strGuid.c_str());

		if (!m_pRedisClient->HDel(STAFF_INFO_MAP_KEY, p_staffInfo.m_strCode))
		{
			ICC_LOG_ERROR(m_pLog, "delete staff data from redis failed! code[%s]", p_staffInfo.m_strCode.c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "delete staff data from redis success! code[%s]", p_staffInfo.m_strCode.c_str());
	}

	return true;
}

bool CStaff::_UpdateData(const std::vector<Data::CStaffInfo>& addDatas, const std::vector<Data::CStaffInfo>& updateDatas, const std::vector<Data::CStaffInfo>& deleteDatas)
{
	ICC_LOG_DEBUG(m_pLog, "update staff data begin! addDatas[%d], updateDatas[%d], deleteDatas[%d]",
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

	ICC_LOG_DEBUG(m_pLog, "update staff data complete! ");

	return true;
}


bool CStaff::_Delete()
{
	if (m_vecStaff.empty())
	{
		return true;
	}

	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	std::string l_strGuid = m_pDBConn->BeginTransaction();
	for (Data::CStaffInfo var : m_vecStaff)
	{
		DataBase::SQLRequest l_oDeleteSQLReq;
		l_oDeleteSQLReq.sql_id = "update_icc_t_staff";
		l_oDeleteSQLReq.set["is_delete"] = "true";
		l_oDeleteSQLReq.param["guid"] = var.m_strGuid;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oDeleteSQLReq, false, l_strGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "delete staff info error, rollback: [%s]", l_pRSet->GetErrorMsg().c_str());
			m_pDBConn->Rollback(l_strGuid);
			return false;
		}
	}

	m_pDBConn->Commit(l_strGuid);
	return true;
}


bool CStaff::_HSet()
{
	if (m_vecStaff.empty())
	{
		return true;
	}

	std::map<std::string, std::string> tmp_mapInfo;
	std::map<std::string, std::string> tmp_mapInfo2;
	for (Data::CStaffInfo var : m_vecStaff)
	{
		std::string l_str = var.ToJson(m_pJsonFty->CreateJson());
		tmp_mapInfo[var.m_strGuid] = l_str;
		tmp_mapInfo2[var.m_strCode] = l_str;
	}

	m_pRedisClient->HMSet(STAFF_INFO_MAP_KEY, tmp_mapInfo2);

	return m_pRedisClient->HMSet(STAFF_INFO_KEY, tmp_mapInfo);
}

void CStaff::_HDel()
{
	for (Data::CStaffInfo var : m_vecStaff)
	{
		m_pRedisClient->HDel(STAFF_INFO_KEY, var.m_strGuid);
	}
}

bool CStaff::_IsEqual(Data::CStaffInfo p_old, Data::CStaffInfo l_new)
{
	if (p_old.m_strDeptGuid == l_new.m_strDeptGuid &&
		p_old.m_strCode == l_new.m_strCode &&
		p_old.m_strName == l_new.m_strName &&
		p_old.m_strSex == l_new.m_strSex &&
		p_old.m_strType == l_new.m_strType &&
		p_old.m_strPosition == l_new.m_strPosition &&
		p_old.m_strMobile == l_new.m_strMobile &&
		p_old.m_strPhone == l_new.m_strPhone &&
		p_old.m_strShortcut == l_new.m_strShortcut &&
		p_old.m_strLevel == l_new.m_strLevel &&
		p_old.m_strSort == l_new.m_strSort &&
		p_old.m_strIsLeader == l_new.m_strIsLeader)
	{

		return true;
	}


	return false;
}
