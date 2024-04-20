#include "Boost.h"
#include "Department.h"

using namespace ICC;
using namespace Data;

#define DEPT_TYPE_CODE						("dept_type_code")

CDepartment::CDepartment(std::vector<Data::CDeptInfo> p_vecDeptInfo, IResourceManagerPtr p_pIResourceManager) : 
m_vecDeptInfo(p_vecDeptInfo), 
m_pResourceManager(p_pIResourceManager)
{	
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pJsonFty = ICCGetIJsonFactory();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);

	m_iTotalReceiveDataSize = 0;

	_SetCommandDept();
}

CDepartment::~CDepartment()
{

}

int CDepartment::ReceiveTotalDataSize()
{
	return (int)m_vecDeptInfo.size();
}

void CDepartment::AppendReceiveDataSize(int iReceiveSize)
{
	m_iTotalReceiveDataSize += iReceiveSize;
}

void CDepartment::Append(std::vector<Data::CDeptInfo>& p_vecInfo)
{
	int iCount = p_vecInfo.size();
	for (int i = 0; i < iCount; ++i)
	{
		m_vecDeptInfo.push_back(p_vecInfo[i]);
	}
}



bool CDepartment::SynUpdate()
{	
	if (m_vecDeptInfo.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no department data need update!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "begin update dept data, size[%d]", m_vecDeptInfo.size());

	std::map<std::string, CDeptInfo> mapTmpDepts;
	for (Data::CDeptInfo var : m_vecDeptInfo)
	{
		mapTmpDepts[var.m_strGuid] = var;
	}

	int iCount = m_vecDeptInfo.size();
	for (int i = 0; i < iCount; ++i)
	{		
		CDeptInfo tmp = m_vecDeptInfo[i];
		int iDepth = 0;
		while (true)
		{
			//如果patch为空，先将自己赋值
			if (m_vecDeptInfo[i].m_strCodeLevel.empty())
			{
				m_vecDeptInfo[i].m_strCodeLevel = tmp.m_strCode;
				iDepth = 1;

				if (tmp.m_strGuid == tmp.m_strParentGuid)
				{
					break;
				}
			}
			else
			{
				std::string strTmp = m_vecDeptInfo[i].m_strCodeLevel;
				m_vecDeptInfo[i].m_strCodeLevel = tmp.m_strCode;
				m_vecDeptInfo[i].m_strCodeLevel += "|";
				m_vecDeptInfo[i].m_strCodeLevel += strTmp;
				iDepth++;
			}

			std::map<std::string, CDeptInfo>::const_iterator itr_const;
			itr_const = mapTmpDepts.find(tmp.m_strParentGuid);
			if (itr_const != mapTmpDepts.end())
			{
				if (itr_const->second.m_strParentGuid == tmp.m_strGuid)
				{
					break;
				}
				tmp = itr_const->second;
			}
			else
			{
				break;
			}
		}

		m_vecDeptInfo[i].m_strDepth = m_pString->Format("%d", iDepth);
	}

	std::vector<CDeptInfo> vecTmpDatas;
	for (Data::CDeptInfo var : m_vecDeptInfo)
	{
		if (m_strIsFilter == "1")
		{
			if (var.m_strCodeLevel.find(m_strSystemRootDeptCode) == std::string::npos)
			{
				continue;
			}
		}
		
		vecTmpDatas.push_back(var);
	}


	/*std::map<std::string, std::string> mapDepts;
	std::map<std::string, std::string> mapProcessTimeoutDepts;
	std::map<std::string, std::string> mapCodeGuids;
	std::map<std::string, std::string> mapGuidCodes;
	std::map<std::string, std::vector<CDeptInfo>> mapLevelDepts;
	for (Data::CDeptInfo var : m_vecDeptInfo)
	{
		if (var.m_strCodeLevel.find(m_strSystemRootDeptCode) == std::string::npos)
		{
			continue;
		}

		mapCodeGuids.insert(std::make_pair(var.m_strCode, var.m_strGuid));
		mapGuidCodes.insert(std::make_pair(var.m_strGuid, var.m_strCode));

		std::string strTmp = var.ToJson(m_pJsonFty->CreateJson());
		mapDepts[var.m_strGuid] = strTmp;

		for (auto& processTimeoutDept : m_lstCommCode)
		{
			if (var.m_strType == processTimeoutDept)
			{
				mapProcessTimeoutDepts[var.m_strCode] = var.m_strType;				
				break;
			}
		}

		std::map<std::string, std::vector<CDeptInfo>>::iterator itr;
		itr = mapLevelDepts.find(var.m_strParentGuid);
		if (itr != mapLevelDepts.end())
		{
			itr->second.push_back(var);
		}
		else
		{
			std::vector<CDeptInfo> vecTmps;
			vecTmps.push_back(var);
			mapLevelDepts.insert(std::make_pair(var.m_strParentGuid, vecTmps));
		}

	}	

	if (!mapDepts.empty())
	{
		if (!m_pRedisClient->HMSet(DEPT_INFO_KEY, mapDepts))
		{
			ICC_LOG_DEBUG(m_pLog, "set dept key failed!!");
			return false;
		}
	}	
	
	if (!mapProcessTimeoutDepts.empty())
	{
		if (!m_pRedisClient->HMSet(DEPT_TYPE_CODE, mapProcessTimeoutDepts))
		{
			ICC_LOG_DEBUG(m_pLog, "set process timeout depts key failed!!");
			return false;
		}
	}
	else
	{
		ICC_LOG_WARNING(m_pLog, "process timeout depts is empty!!! ");
	}
	
	if (!mapCodeGuids.empty())
	{
		if (!m_pRedisClient->HMSet(DEPT_CODE_GUID_MAP, mapCodeGuids))
		{
			ICC_LOG_DEBUG(m_pLog, "set depts code guid map failed!!");
			return false;
		}
	}
	

	std::map<std::string, std::vector<CDeptInfo>>::const_iterator itr_const_depts;
	for (itr_const_depts = mapLevelDepts.begin(); itr_const_depts != mapLevelDepts.end(); ++itr_const_depts)
	{
		std::map<std::string, std::string>::const_iterator itrDepts;
		itrDepts = mapGuidCodes.find(itr_const_depts->first);
		if (itrDepts != mapGuidCodes.end())
		{
			std::map<std::string, std::string> mapTmpLevels;
			std::string strKey = DEPTLEVEL + itrDepts->second;
			int iChildCount = itr_const_depts->second.size();
			for (int i = 0; i < iChildCount; ++i)
			{
				CDeptInfo deptInfo = itr_const_depts->second[i];
				mapTmpLevels.insert(std::make_pair(deptInfo.m_strCode, deptInfo.ToJson(m_pJsonFty->CreateJson())));
			}

			if (!mapTmpLevels.empty())
			{
				if (!m_pRedisClient->HMSet(strKey, mapTmpLevels))
				{
					ICC_LOG_ERROR(m_pLog, "set dept level info from redis failed! [%s]", strKey.c_str());
					return false;
				}
			}
			
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "set dept level info not find guid! [%s]", itr_const_depts->first.c_str());
		}
	}


	return true;*/

	std::map<std::string, std::string> mapOldDatas;
	if (!_GetAllOldData(mapOldDatas))
	{
		ICC_LOG_ERROR(m_pLog, "get all old department datas failed!!");
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "get all old department datas success! size[%d] ", mapOldDatas.size());

	std::vector<Data::CDeptInfo> addDatas;
	std::vector<Data::CDeptInfo> updateDatas;
	std::vector<Data::CDeptInfo> deleteDatas;
	//_CompareData(mapOldDatas, m_vecDeptInfo, addDatas, updateDatas, deleteDatas);
	_CompareData(mapOldDatas, vecTmpDatas, addDatas, updateDatas, deleteDatas);

	if (!_UpdateData(addDatas, updateDatas, deleteDatas))
	{		
		return false;
	}

	//_LoadDataEx(vecTmpDatas);

	return true;


	//if (m_vecDeptInfo.empty())
	//{
	//	return true;
	//}

	//std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	//std::string l_strGuid = m_pDBConn->BeginTransaction();
	//for (Data::CDeptInfo l_dept : m_vecDeptInfo)
	//{
	//	DataBase::SQLRequest l_oSetSQLReq;
	//	if (m_pRedisClient->HExists(DEPT_INFO_KEY, l_dept.m_strGuid))
	//	{
	//		std::string l_strInfo;
	//		m_pRedisClient->HGet(DEPT_INFO_KEY, l_dept.m_strGuid, l_strInfo);
	//		Data::CDeptInfo l_deptInfo;
	//		l_deptInfo.Parse(l_strInfo, m_pJsonFty->CreateJson());

	//		if (IsEqual(l_deptInfo, l_dept))
	//		{
	//			//ICC_LOG_DEBUG(m_pILog, "dept[%s] info is equal", l_dept.m_strCode.c_str());
	//			continue;
	//		}

	//		ICC_LOG_DEBUG(m_pLog, "dept[%s] info is changed.", l_dept.m_strCode.c_str());

	//		l_oSetSQLReq = ConstructSQLStruct(l_dept, "update_icc_t_dept");
	//	}
	//	else
	//	{
	//		//查询单条记录，判断数据库中是否存在
	//		DataBase::SQLRequest l_oQueryDept;
	//		l_oQueryDept = ConstructSQLStruct(l_dept, "select_icc_t_dept");
	//		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oQueryDept, false, l_strGuid);

	//		if (!l_pRSet->IsValid())
	//		{
	//			ICC_LOG_ERROR(m_pLog, "select_icc_t_dept :[%s]", l_pRSet->GetErrorMsg().c_str());
	//			m_pDBConn->Rollback(l_strGuid);
	//			return false;
	//		}

	//		if (l_pRSet->RecordSize() == 1)
	//		{
	//			//恢复标志位false 更新现有记录
	//			l_oSetSQLReq = ConstructSQLStruct(l_dept, "recover_icc_t_dept_flag");
	//			ICC_LOG_DEBUG(m_pLog, "recover dept [%s]", l_dept.m_strCode.c_str());
	//		}
	//		else if (l_pRSet->RecordSize() == 0)
	//		{
	//			//不存在，直接写入一条记录
	//			l_oSetSQLReq = ConstructSQLStruct(l_dept, "insert_icc_t_dept");
	//			ICC_LOG_DEBUG(m_pLog, "insert dept [%s]", l_dept.m_strCode.c_str());
	//		}
	//		else
	//		{
	//			//数据错误
	//			ICC_LOG_ERROR(m_pLog, "dept data error, repeat user :[%s]", l_dept.m_strCode.c_str());
	//			m_pDBConn->Rollback(l_strGuid);
	//		}
	//	}

	//	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSetSQLReq, false, l_strGuid);
	//	if (!l_pRSet->IsValid())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "operate dept info error, rollback: [%s]", l_pRSet->GetErrorMsg().c_str());
	//		m_pDBConn->Rollback(l_strGuid);
	//		return false;
	//	}
	//}

	//m_pDBConn->Commit(l_strGuid);

	//return true;
}


bool CDepartment::SingleAdd()
{
	if (m_vecDeptInfo.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no dept data need add!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "will add dept data size[%d]!!", m_vecDeptInfo.size());

	_ComDeptPath(m_vecDeptInfo);

	std::string l_strGuid = m_pDBConn->BeginTransaction();

	if (!_Add(m_vecDeptInfo, l_strGuid))
	{
		return false;
	}		

	m_pDBConn->Commit(l_strGuid);

	ICC_LOG_DEBUG(m_pLog, "add dept data complete! ");

	return true;
}

bool CDepartment::SingleModify()
{
	if (m_vecDeptInfo.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no dept data need update!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "will update dept data size[%d]!!", m_vecDeptInfo.size());
	
	_ComDeptPath(m_vecDeptInfo);

	std::string l_strGuid = m_pDBConn->BeginTransaction();	

	if (!_Update(m_vecDeptInfo, l_strGuid))
	{
		return false;
	}		

	m_pDBConn->Commit(l_strGuid);

	ICC_LOG_DEBUG(m_pLog, "update dept data complete! ");

	return true;
}

bool CDepartment::SingleDelete()
{
	if (m_vecDeptInfo.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no dept data need delete!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "will dept data size[%d]!!", m_vecDeptInfo.size());
	
	std::string l_strGuid = m_pDBConn->BeginTransaction();
	
	if (!_Del(m_vecDeptInfo, l_strGuid))
	{
		return false;
	}	

	m_pDBConn->Commit(l_strGuid);

	ICC_LOG_DEBUG(m_pLog, "delete dept data complete! ");

	return true;
}

bool CDepartment::LoadData()
{
	ICC_LOG_DEBUG(m_pLog, "load dept to redis begin");
	
	bool loadtag = false;
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_dept";
	l_SqlRequest.param["is_delete"] = "false";

	ICC_LOG_DEBUG(m_pLog, "LoadingDeptInfo Begin");
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());

	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		return false;
	}	

	std::vector<CDeptInfo> vecDatas;

	while (l_result->Next())
	{
		CDeptInfo pCDeptInfo;
		pCDeptInfo.m_strGuid = l_result->GetValue("guid");
		pCDeptInfo.m_strParentGuid = l_result->GetValue("parent_guid");
		pCDeptInfo.m_strCode = l_result->GetValue("code");
		pCDeptInfo.m_strDistrictCode = l_result->GetValue("district_code");
		pCDeptInfo.m_strName = l_result->GetValue("name");
		pCDeptInfo.m_strType = l_result->GetValue("type");
		pCDeptInfo.m_strPhone = l_result->GetValue("phone");
		pCDeptInfo.m_strPucOrgIdentifier = l_result->GetValue("pucorgidentifier");
		pCDeptInfo.m_strCodeLevel = l_result->GetValue("path");
		pCDeptInfo.m_strDepth = l_result->GetValue("depth");
		pCDeptInfo.m_strShortcut = l_result->GetValue("shortcut");
		pCDeptInfo.m_strSort = l_result->GetValue("sort");

		vecDatas.push_back(pCDeptInfo);
	}

	int iDelCount = m_pRedisClient->Del(DEPT_CODE_GUID_MAP);

	ICC_LOG_DEBUG(m_pLog, "del DEPT_CODE_GUID_MAP count %d", iDelCount);

	iDelCount = m_pRedisClient->Del(DEPT_TYPE_CODE);

	ICC_LOG_DEBUG(m_pLog, "del DEPT_TYPE_CODE count %d", iDelCount);

	iDelCount = m_pRedisClient->Del(DEPT_INFO_KEY);

	ICC_LOG_DEBUG(m_pLog, "del DEPT_INFO_KEY count %d", iDelCount);
	
	if (!_UpdateDataToRedis(vecDatas))
	{
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "load dept to redis end count[%d]", vecDatas.size());

	return true;

	/*ICC_LOG_DEBUG(m_pLog, "load dept from redis Begin");

	std::map<std::string, std::string> mapTmps;
	if (!_GetAllOldData(mapTmps))
	{
		ICC_LOG_ERROR(m_pLog, "get dept from redis failed!!");
		return false;
	}

	std::map<std::string, std::string> mapTmpProcessTimeouts;
	if (!_GetAllProcessTimeoutData(mapTmpProcessTimeouts))
	{
		ICC_LOG_ERROR(m_pLog, "get process timeout dept from redis failed!!");
		return false;
	}

	bool loadtag = false;
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_dept";
	l_SqlRequest.param["is_delete"] = "false";

	ICC_LOG_DEBUG(m_pLog, "LoadingDeptInfo Begin");
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());

	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		return false;
	}	

	
	std::map<std::string, std::string> tmpCodeGuidMaps;
	std::map<std::string, std::string> tmpGuidCodeMaps;
	std::map<std::string, std::vector<CDeptInfo>> mapDeptMaps;

	while (l_result->Next())
	{
		CDeptInfo pCDeptInfo;
		pCDeptInfo.m_strGuid = l_result->GetValue("guid");
		pCDeptInfo.m_strParentGuid = l_result->GetValue("parent_guid");
		pCDeptInfo.m_strCode = l_result->GetValue("code");
		pCDeptInfo.m_strDistrictCode = l_result->GetValue("district_code");
		pCDeptInfo.m_strName = l_result->GetValue("name");
		pCDeptInfo.m_strType = l_result->GetValue("type");
		pCDeptInfo.m_strPhone = l_result->GetValue("phone");
		pCDeptInfo.m_strLevel = l_result->GetValue("level");
		pCDeptInfo.m_strShortcut = l_result->GetValue("shortcut");
		pCDeptInfo.m_strSort = l_result->GetValue("sort");
		
		tmpCodeGuidMaps.insert(std::make_pair(pCDeptInfo.m_strCode, pCDeptInfo.m_strGuid));
		tmpGuidCodeMaps.insert(std::make_pair(pCDeptInfo.m_strGuid, pCDeptInfo.m_strCode));

		{
			std::map<std::string, std::vector<CDeptInfo>>::iterator itr;
			itr = mapDeptMaps.find(pCDeptInfo.m_strParentGuid);
			if (itr != mapDeptMaps.end())
			{
				itr->second.push_back(pCDeptInfo);
			}
			else
			{
				std::vector<CDeptInfo> vecTmps;
				vecTmps.push_back(pCDeptInfo);
				mapDeptMaps.insert(std::make_pair(pCDeptInfo.m_strParentGuid, vecTmps));
			}
		}


		bool bNeedWrite = false;
		std::map<std::string, std::string>::iterator itr;
		itr = mapTmps.find(pCDeptInfo.m_strGuid);
		if (itr != mapTmps.end())
		{
			Data::CDeptInfo oldInfo;
			if (!oldInfo.Parse(itr->second, m_pJsonFty->CreateJson()))
			{
				bNeedWrite = true;
			}
			else
			{
				if (!_IsEqual(oldInfo, pCDeptInfo))
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
				
		std::map<std::string, std::string>::iterator itr_processtimeout;
		itr_processtimeout = mapTmpProcessTimeouts.find(pCDeptInfo.m_strCode);
		if (itr_processtimeout != mapTmpProcessTimeouts.end())
		{
			if (itr_processtimeout->second != pCDeptInfo.m_strType)
			{
				bNeedWrite = true;
			}
			mapTmpProcessTimeouts.erase(itr_processtimeout);
		}

		if (bNeedWrite)
		{
			if (!_SetRedisDetpInfo(pCDeptInfo))
			{
				ICC_LOG_ERROR(m_pLog, "Load DeptInfo to Redis Error");
				return false;
			}
		}		
	}

	{
		if (m_pRedisClient->Del(DEPT_CODE_GUID_MAP) < 0)
		{
			ICC_LOG_ERROR(m_pLog, "delete dept code guid map from redis failed! [%s]", DEPT_CODE_GUID_MAP);
			return false;
		}
			

		if (!m_pRedisClient->HMSet(DEPT_CODE_GUID_MAP, tmpCodeGuidMaps))
		{
			ICC_LOG_ERROR(m_pLog, "set dict code guid map to redis failed! [%s]", DEPT_CODE_GUID_MAP);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "set dept code guid map info success! size[%d]", tmpCodeGuidMaps.size());

		std::map<std::string, std::vector<CDeptInfo>>::const_iterator itr_const_depts;
		for (itr_const_depts = mapDeptMaps.begin(); itr_const_depts != mapDeptMaps.end(); ++itr_const_depts)
		{
			std::map<std::string, std::string>::const_iterator itrDepts;
			itrDepts = tmpGuidCodeMaps.find(itr_const_depts->first);
			if (itrDepts != tmpGuidCodeMaps.end())
			{
				std::map<std::string, std::string> mapTmpLevels;
				std::string strKey = DEPTLEVEL + itrDepts->second;
				int iChildCount = itr_const_depts->second.size();
				for (int i = 0; i < iChildCount; ++i)
				{
					CDeptInfo deptInfo = itr_const_depts->second[i];
					mapTmpLevels.insert(std::make_pair(deptInfo.m_strCode, deptInfo.ToJson(m_pJsonFty->CreateJson())));					
				}
				if (!m_pRedisClient->HMSet(strKey, mapTmpLevels))
				{
					ICC_LOG_ERROR(m_pLog, "set dept level info from redis failed! [%s]", strKey.c_str());
					return false;
				}
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "set dept level info not find guid! [%s]", itr_const_depts->first.c_str());
			}
		}

		ICC_LOG_DEBUG(m_pLog, "set dept level info success! size[%d]", mapDeptMaps.size());
	}


	ICC_LOG_DEBUG(m_pLog, "update DeptInfo Success,Dept Size[%d], begin clear dept[%d], processtimeout dept[%d]", l_result->RecordSize(), mapTmps.size(), mapTmpProcessTimeouts.size());

	std::map<std::string, std::string>::const_iterator itr_const;
	for (itr_const = mapTmps.begin(); itr_const != mapTmps.end(); ++itr_const)
	{
		bool bSet = m_pRedisClient->HDel(DEPT_INFO_KEY, itr_const->first);
		if (!bSet)
		{
			ICC_LOG_ERROR(m_pLog, "delete dept info from redis failed! [%s]", itr_const->first.c_str());
			return false;
		}
	}

	std::map<std::string, std::string>::const_iterator itr_const_processtimeout;
	for (itr_const_processtimeout = mapTmpProcessTimeouts.begin(); itr_const_processtimeout != mapTmpProcessTimeouts.end(); ++itr_const_processtimeout)
	{
		bool bSet = m_pRedisClient->HDel(DEPT_TYPE_CODE, itr_const_processtimeout->first);
		if (!bSet)
		{
			ICC_LOG_ERROR(m_pLog, "delete process timeout dept info from redis failed! [%s]", itr_const->first.c_str());
			return false;
		}
	}

	ICC_LOG_DEBUG(m_pLog, "LoadingDeptInfo Success,Dept Size[%d]", l_result->RecordSize());*/
	return true;
}

bool CDepartment::_SetRedisDetpInfo(CDeptInfo& p_oDeptInfo)
{
	for (auto& var : m_lstCommCode)
	{
		if (p_oDeptInfo.m_strType == var)
		{
			// 处警单超时使用 [7/5/2018 w26326]
			bool bSet = m_pRedisClient->HSet(DEPT_TYPE_CODE, p_oDeptInfo.m_strCode, p_oDeptInfo.m_strType);
			if (!bSet)
			{
				ICC_LOG_ERROR(m_pLog, "set process timeout dept info to redis failed! [%s : %s]", p_oDeptInfo.m_strCode.c_str(), p_oDeptInfo.m_strType.c_str());
				return false;
			}
			break;
		}
	}
	std::string l_strVal = p_oDeptInfo.ToJson(m_pJsonFty->CreateJson());
	bool bTmpSet = m_pRedisClient->HSet(DEPT_INFO_KEY, p_oDeptInfo.m_strGuid, l_strVal);
	if (!bTmpSet)
	{
		ICC_LOG_ERROR(m_pLog, "set dept info to redis failed! [%s : %s]", p_oDeptInfo.m_strGuid.c_str(), l_strVal.c_str());
		return false;
	}
	bool bDeptCodeInfo = m_pRedisClient->HSet(DEPT_CODE_INFO_KEY, p_oDeptInfo.m_strCode, l_strVal);
	if (!bDeptCodeInfo)
	{
		ICC_LOG_ERROR(m_pLog, "set dept code info to redis failed! [%s : %s]", p_oDeptInfo.m_strGuid.c_str(), l_strVal.c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "set dept info to redis success! [%s : %s]", p_oDeptInfo.m_strGuid.c_str(), l_strVal.c_str());

	return true;
}

void CDepartment::_SetCommandDept()
{
	unsigned int l_Count = m_pConfig->GetNodeCount("ICC/Plugin/Dept/ProcessTimeoutDeptTypes", "ProcessTimeoutDeptType");
	for (unsigned int l_index = 0; l_index < l_Count; l_index++)
	{
		std::string l_strIndex = m_pConfig->GetValue(m_pString->Format("ICC/Plugin/Dept/ProcessTimeoutDeptTypes/ProcessTimeoutDeptType[%d]", l_index), "");
		m_lstCommCode.push_back(l_strIndex);
	}

	m_strSystemRootDeptCode = m_pConfig->GetNodeCount("ICC/Plugin/SMP/SystemRootDeptCode", "");

	m_strIsFilter = m_pConfig->GetNodeCount("ICC/Plugin/SMP/isfilterbyrootdeptcode", "");

	ICC_LOG_DEBUG(m_pLog, "command dept size! [%d]", m_lstCommCode.size());
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
bool CDepartment::_GetAllOldData(std::map<std::string, std::string>& mapOldDatas)
{
	if (m_pRedisClient->HGetAllEx(DEPT_INFO_KEY, mapOldDatas))
	{
		return true;
	}

	ICC_LOG_ERROR(m_pLog, "get all old dept datas failed!!!");

	return false;
}

bool CDepartment::_GetAllProcessTimeoutData(std::map<std::string, std::string>& mapOldDatas)
{
	if (m_pRedisClient->HGetAllEx(DEPT_TYPE_CODE, mapOldDatas))
	{
		return true;
	}

	ICC_LOG_ERROR(m_pLog, "get all process timeout dept datas failed!!!");

	return false;
}

bool CDepartment::_CompareData(std::map<std::string, std::string>& mapOldDatas, const std::vector<Data::CDeptInfo>& vecNewDatas,
	std::vector<Data::CDeptInfo>& addDatas, std::vector<Data::CDeptInfo>& updateDatas, std::vector<Data::CDeptInfo>& deleteDatas)
{
	int iNewDataCount = vecNewDatas.size();
	for (int i = 0; i < iNewDataCount; ++i)
	{
		std::map<std::string, std::string>::iterator itr = mapOldDatas.find(vecNewDatas[i].m_strGuid);
		if (itr != mapOldDatas.end())
		{
			Data::CDeptInfo oldInfo;
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
		Data::CDeptInfo oldInfo;
		if (!oldInfo.Parse(itr_const->second, m_pJsonFty->CreateJson()))
		{
			return false;
		}

		deleteDatas.push_back(oldInfo);
	}

	return true;
}

bool CDepartment::_UpdateDataToRedis(const std::vector<Data::CDeptInfo>& vecDatas)
{
	for (Data::CDeptInfo p_deptInfo : vecDatas)
	{
		if (!_SetRedisDetpInfo(p_deptInfo))
		{
			ICC_LOG_ERROR(m_pLog, "set dept to Redis Error");			
			return false;
		}

		if (!m_pRedisClient->HSet(DEPT_CODE_GUID_MAP, p_deptInfo.m_strCode, p_deptInfo.m_strGuid))
		{
			ICC_LOG_ERROR(m_pLog, "add dept code guid map to redis failed! code[%s]", p_deptInfo.m_strCode.c_str());			
			return false;
		}

		/*std::string strKey = DEPTLEVEL;
		std::string strDict;
		if (m_pRedisClient->HGet(DEPT_INFO_KEY, p_deptInfo.m_strParentGuid, strDict))
		{
			CDeptInfo dictInfo;
			if (dictInfo.Parse(strDict, m_pJsonFty->CreateJson()))
			{
				strKey += dictInfo.m_strCode;
			}
		}

		ICC_LOG_DEBUG(m_pLog, "will add dept level data to redis! key[%s], dict[%s]", strKey.c_str(), (p_deptInfo.m_strGuid + "----" + p_deptInfo.m_strCode).c_str());

		std::map<std::string, std::string> mapLevels;
		if (m_pRedisClient->HGetAll(strKey, mapLevels))
		{
			mapLevels.insert(std::make_pair(p_deptInfo.m_strCode, p_deptInfo.ToJson(m_pJsonFty->CreateJson())));
			if (!m_pRedisClient->HMSet(strKey, mapLevels))
			{
				ICC_LOG_ERROR(m_pLog, "add dept level data to redis failed! key[%s], dict[%s]", strKey.c_str(), p_deptInfo.m_strCode.c_str());
			}
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "add dept level data to redis not! key[%s], dict[%s]", strKey.c_str(), p_deptInfo.m_strCode.c_str());
		}*/

		ICC_LOG_DEBUG(m_pLog, "add dept data to redis success! guid[%s]", p_deptInfo.m_strGuid.c_str());
	}

	return true;
}

void CDepartment::_ComDeptPath(std::vector<Data::CDeptInfo>& vecDatas)
{
	unsigned int tmp_uiSize = vecDatas.size();
	for (unsigned int i = 0; i < tmp_uiSize; ++i)
	{
		//如果父节点为空，则自己为父
		if (vecDatas[i].m_strParentGuid.empty())
		{
			vecDatas[i].m_strCodeLevel = vecDatas[i].m_strCode;
			vecDatas[i].m_strDepth = "1";
		}
		else
		{
			std::string l_strVal;
			//从redis中获取父的
			m_pRedisClient->HGet(DEPT_INFO_KEY, vecDatas[i].m_strParentGuid, l_strVal);

			if (l_strVal.empty())
			{
				ICC_LOG_ERROR(m_pLog, "Get parent info from redis failed, parent guid: %s", vecDatas[i].m_strParentGuid.c_str());
				continue;
			}

			Data::CDeptInfo tmp_oDeptInfo;
			
			if (!tmp_oDeptInfo.Parse(l_strVal, m_pJsonFty->CreateJson()))
			{
				ICC_LOG_ERROR(m_pLog, "Pares dept info from redis failed, parent guid: %s, value: %s", vecDatas[i].m_strParentGuid.c_str(), l_strVal.c_str());
				continue;
			}

			vecDatas[i].m_strCodeLevel = tmp_oDeptInfo.m_strCodeLevel;
			vecDatas[i].m_strCodeLevel += "|";
			vecDatas[i].m_strCodeLevel += vecDatas[i].m_strCode;

			int tmp_iCount = std::count(vecDatas[i].m_strCodeLevel.begin(), vecDatas[i].m_strCodeLevel.end(), '|');

			++tmp_iCount;
			vecDatas[i].m_strDepth = std::to_string(tmp_iCount);
		}
	}
}

bool CDepartment::_Add(const std::vector<Data::CDeptInfo>& vecDatas, const std::string& strTransGuid)
{
	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	std::map<std::string, std::string> mapTmpDatas;
	for (Data::CDeptInfo p_deptInfo : vecDatas)
	{
		if (mapTmpDatas.find(p_deptInfo.m_strGuid) != mapTmpDatas.end())
		{
			ICC_LOG_WARNING(m_pLog, "same dept data ! guid[%s]", p_deptInfo.m_strGuid.c_str());
			continue;;
		}

		DataBase::SQLRequest l_oSetSQLReq;
		l_oSetSQLReq.sql_id = "insert_icc_t_dept";
		l_oSetSQLReq.param["guid"] = p_deptInfo.m_strGuid;
		l_oSetSQLReq.param["parent_guid"] = p_deptInfo.m_strParentGuid;
		l_oSetSQLReq.param["code"] = p_deptInfo.m_strCode;
		l_oSetSQLReq.param["district_code"] = p_deptInfo.m_strDistrictCode;
		l_oSetSQLReq.param["type"] = m_pString->AnsiToUtf8(p_deptInfo.m_strType);
		l_oSetSQLReq.param["name"] = p_deptInfo.m_strName;
		l_oSetSQLReq.param["path"] = p_deptInfo.m_strCodeLevel;
		l_oSetSQLReq.param["depth"] = p_deptInfo.m_strDepth;
		l_oSetSQLReq.param["pucorgidentifier"] = p_deptInfo.m_strPucOrgIdentifier;
		l_oSetSQLReq.param["phone"] = p_deptInfo.m_strPhone;
		l_oSetSQLReq.param["shortcut"] = p_deptInfo.m_strShortcut;
		l_oSetSQLReq.param["sort"] = p_deptInfo.m_strSort;
		l_oSetSQLReq.param["is_delete"] = "false";
		l_oSetSQLReq.param["create_user"] = "smp_gateway";
		l_oSetSQLReq.param["create_time"] = m_pDateTime->CurrentDateTimeStr();

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSetSQLReq, false, strTransGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "add dept data to db failed! sql[%s]", l_pRSet->GetSQL().c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "add dept data to db success!!! sql[%s]", l_pRSet->GetSQL().c_str());	

		mapTmpDatas.insert(std::make_pair(p_deptInfo.m_strGuid, p_deptInfo.m_strGuid));
	}

	if (!_UpdateDataToRedis(vecDatas))
	{
		m_pDBConn->Rollback(strTransGuid);
		return false;
	}

	return true;
}

bool CDepartment::_Update(const std::vector<Data::CDeptInfo>& vecDatas, const std::string& strTransGuid)
{
	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());

	for (Data::CDeptInfo p_deptInfo : vecDatas)
	{
		DataBase::SQLRequest l_oSetSQLReq;
		l_oSetSQLReq.sql_id = "update_icc_t_dept";
		l_oSetSQLReq.set["code"] = p_deptInfo.m_strCode;
		l_oSetSQLReq.set["parent_guid"] = p_deptInfo.m_strParentGuid;
		l_oSetSQLReq.set["district_code"] = p_deptInfo.m_strDistrictCode;
		l_oSetSQLReq.set["type"] = m_pString->AnsiToUtf8(p_deptInfo.m_strType);
		l_oSetSQLReq.set["name"] = p_deptInfo.m_strName;
		l_oSetSQLReq.set["path"] = p_deptInfo.m_strCodeLevel;
		l_oSetSQLReq.set["depth"] = p_deptInfo.m_strDepth;
		l_oSetSQLReq.set["pucorgidentifier"] = p_deptInfo.m_strPucOrgIdentifier;
		l_oSetSQLReq.set["phone"] = p_deptInfo.m_strPhone;
		l_oSetSQLReq.set["shortcut"] = p_deptInfo.m_strShortcut;
		l_oSetSQLReq.set["sort"] = p_deptInfo.m_strSort;

		l_oSetSQLReq.set["update_user"] = "smp_gateway";
		l_oSetSQLReq.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
		l_oSetSQLReq.param["guid"] = p_deptInfo.m_strGuid;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSetSQLReq, false, strTransGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "update dept data to db failed! sql[%s]", l_pRSet->GetSQL().c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "update dept data to db success!!! sql[%s]", l_pRSet->GetSQL().c_str());		
	}

	if (!_UpdateDataToRedis(vecDatas))
	{
		m_pDBConn->Rollback(strTransGuid);
		return false;
	}

	return true;
}

bool CDepartment::_Del(const std::vector<Data::CDeptInfo>& vecDatas, const std::string& strTransGuid)
{
	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());

	for (Data::CDeptInfo p_deptInfo : vecDatas)
	{
		DataBase::SQLRequest l_oDeleteSQLReq;
		l_oDeleteSQLReq.sql_id = "delete_icc_t_dept";
		l_oDeleteSQLReq.param["guid"] = p_deptInfo.m_strGuid;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oDeleteSQLReq, false, strTransGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "delete dept data to db failed! sql[%s]", l_pRSet->GetSQL().c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_ERROR(m_pLog, "delete dept data to db success!!! sql[%s]", l_pRSet->GetSQL().c_str());

		if (!m_pRedisClient->HDel(DEPT_INFO_KEY, p_deptInfo.m_strGuid))
		{
			ICC_LOG_ERROR(m_pLog, "delete dept data from redis failed! guid[%s]", p_deptInfo.m_strGuid.c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "delete dept data from redis success! guid[%s]", p_deptInfo.m_strGuid.c_str());


		if (!m_pRedisClient->HDel(DEPT_TYPE_CODE, p_deptInfo.m_strCode))
		{
			ICC_LOG_ERROR(m_pLog, "delete process timeout dept data from redis failed! code[%s]", p_deptInfo.m_strCode.c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		if (!m_pRedisClient->HDel(DEPT_CODE_GUID_MAP, p_deptInfo.m_strCode))
		{
			ICC_LOG_ERROR(m_pLog, "delete dept code guid map to redis failed! content[%s]", p_deptInfo.m_strCode.c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		std::string strKey = DEPTLEVEL;
		std::string strDict;
		if (m_pRedisClient->HGet(DEPT_INFO_KEY, p_deptInfo.m_strParentGuid, strDict))
		{
			CDeptInfo dictInfo;
			if (dictInfo.Parse(strDict, m_pJsonFty->CreateJson()))
			{
				strKey += dictInfo.m_strCode;
			}
		}

		ICC_LOG_DEBUG(m_pLog, "will delete dict level data to redis! key[%s], dict[%s]", strKey.c_str(), (p_deptInfo.m_strGuid + "----" + p_deptInfo.m_strCode).c_str());

		std::map<std::string, std::string> mapLevels;
		if (m_pRedisClient->HGetAll(strKey, mapLevels))
		{
			mapLevels.erase(p_deptInfo.m_strCode);
			if (!m_pRedisClient->HMSet(strKey, mapLevels))
			{
				ICC_LOG_ERROR(m_pLog, "delete dept level data to redis failed! key[%s], dict[%s]", strKey.c_str(), p_deptInfo.m_strCode.c_str());
			}
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "get dept level data failed! key[%s], dict[%s]", strKey.c_str(), p_deptInfo.m_strCode.c_str());
		}

		ICC_LOG_DEBUG(m_pLog, "delete process timeout dept data from redis success! code[%s]", p_deptInfo.m_strCode.c_str());
	}
	return true;
}

bool CDepartment::_UpdateData(const std::vector<Data::CDeptInfo>& addDatas, const std::vector<Data::CDeptInfo>& updateDatas, const std::vector<Data::CDeptInfo>& deleteDatas)
{
	ICC_LOG_DEBUG(m_pLog, "update dept data begin! addDatas[%d], updateDatas[%d], deleteDatas[%d]",
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

	ICC_LOG_DEBUG(m_pLog, "update dept data complete! ");

	return true;
}


bool CDepartment::_Delete()
{
	if (m_vecDeptInfo.empty())
	{
		return true;
	}

	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	std::string l_strGuid = m_pDBConn->BeginTransaction();
	for (Data::CDeptInfo var : m_vecDeptInfo)
	{
		DataBase::SQLRequest l_oDeleteSQLReq;
		l_oDeleteSQLReq.sql_id = "update_icc_t_dept";
		l_oDeleteSQLReq.set["is_delete"] = "true";
		l_oDeleteSQLReq.param["guid"] = var.m_strGuid;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oDeleteSQLReq, false, l_strGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "delete dept info error, rollback: [%s]", l_pRSet->GetErrorMsg().c_str());
			m_pDBConn->Rollback(l_strGuid);
			return false;
		}
	}

	m_pDBConn->Commit(l_strGuid);
	return true;
}


bool CDepartment::_HSet()
{
	if (m_vecDeptInfo.empty())
	{
		return true;
	}

	std::map<std::string, std::string> tmp_mapStaffInfo;
	for (Data::CDeptInfo var : m_vecDeptInfo)
	{
		tmp_mapStaffInfo[var.m_strGuid] = var.ToJson(m_pJsonFty->CreateJson());
	}

	return m_pRedisClient->HMSet(DEPT_INFO_KEY, tmp_mapStaffInfo);
}

void CDepartment::_HDel()
{
	for (Data::CDeptInfo var : m_vecDeptInfo)
	{
		m_pRedisClient->HDel(DEPT_INFO_KEY, var.m_strGuid);
	}
}

bool CDepartment::_IsEqual(Data::CDeptInfo p_old, Data::CDeptInfo l_new)
{
	if (p_old.m_strGuid == l_new.m_strGuid &&
		p_old.m_strParentGuid == l_new.m_strParentGuid &&
		p_old.m_strCode == l_new.m_strCode &&
		p_old.m_strDistrictCode == l_new.m_strDistrictCode &&
		p_old.m_strCodeLevel == l_new.m_strCodeLevel &&
		p_old.m_strType == l_new.m_strType &&
		p_old.m_strName == l_new.m_strName &&
		p_old.m_strPhone == l_new.m_strPhone &&
		p_old.m_strShortcut == l_new.m_strShortcut &&		
		p_old.m_strSort == l_new.m_strSort)
	{
		return true;
	}

	return false;
}

DataBase::SQLRequest CDepartment::_ConstructSQLStruct(Data::CDeptInfo& p_deptInfo, std::string p_strSqlID)
{
	DataBase::SQLRequest l_oSQLExec;
	if (p_strSqlID == "select_icc_t_dept")
	{
		l_oSQLExec.sql_id = "select_icc_t_dept";
		l_oSQLExec.param["code"] = p_deptInfo.m_strCode;
		//l_oSQLExec.param["is_delete"] = "true";
	}
	else if (p_strSqlID == "update_icc_t_dept")
	{
		l_oSQLExec.sql_id = "update_icc_t_dept";
		l_oSQLExec.set["code"] = p_deptInfo.m_strCode;
		l_oSQLExec.set["parent_guid"] = p_deptInfo.m_strParentGuid;
		l_oSQLExec.set["district_code"] = p_deptInfo.m_strDistrictCode;
		l_oSQLExec.set["type"] = m_pString->AnsiToUtf8(p_deptInfo.m_strType);
		l_oSQLExec.set["name"] = p_deptInfo.m_strName;
		l_oSQLExec.set["phone"] = p_deptInfo.m_strPhone;
		l_oSQLExec.set["shortcut"] = p_deptInfo.m_strShortcut;
		l_oSQLExec.set["sort"] = p_deptInfo.m_strSort;

		l_oSQLExec.set["update_user"] = "smp_gateway";
		l_oSQLExec.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
		l_oSQLExec.param["guid"] = p_deptInfo.m_strGuid;
	}
	else if (p_strSqlID == "recover_icc_t_dept_flag")
	{
		l_oSQLExec.sql_id = "update_icc_t_dept";
		l_oSQLExec.set["guid"] = p_deptInfo.m_strGuid;
		l_oSQLExec.set["parent_guid"] = p_deptInfo.m_strParentGuid;
		l_oSQLExec.set["district_code"] = p_deptInfo.m_strDistrictCode;
		l_oSQLExec.set["type"] = m_pString->AnsiToUtf8(p_deptInfo.m_strType);
		l_oSQLExec.set["name"] = p_deptInfo.m_strName;
		l_oSQLExec.set["phone"] = p_deptInfo.m_strPhone;
		l_oSQLExec.set["shortcut"] = p_deptInfo.m_strShortcut;
		l_oSQLExec.set["sort"] = p_deptInfo.m_strSort;
		l_oSQLExec.set["is_delete"] = "false";

		l_oSQLExec.set["update_user"] = "smp_gateway";
		l_oSQLExec.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
		l_oSQLExec.param["code"] = p_deptInfo.m_strCode;
	}
	else if (p_strSqlID == "insert_icc_t_dept")
	{
		l_oSQLExec.sql_id = p_strSqlID;
		l_oSQLExec.param["guid"] = p_deptInfo.m_strGuid;
		l_oSQLExec.param["parent_guid"] = p_deptInfo.m_strParentGuid;
		l_oSQLExec.param["code"] = p_deptInfo.m_strCode;
		l_oSQLExec.param["district_code"] = p_deptInfo.m_strDistrictCode;
		l_oSQLExec.param["type"] = m_pString->AnsiToUtf8(p_deptInfo.m_strType);
		l_oSQLExec.param["name"] = p_deptInfo.m_strName;
		l_oSQLExec.param["phone"] = p_deptInfo.m_strPhone;
		l_oSQLExec.param["shortcut"] = p_deptInfo.m_strShortcut;
		l_oSQLExec.param["sort"] = p_deptInfo.m_strSort;
		l_oSQLExec.param["is_delete"] = "false";
		l_oSQLExec.param["create_user"] = "smp_gateway";
		l_oSQLExec.param["create_time"] = m_pDateTime->CurrentDateTimeStr();
	}
	else if (p_strSqlID == "delete_icc_t_user")
	{
		l_oSQLExec.sql_id = "update_icc_t_dept";
		l_oSQLExec.param["guid"] = p_deptInfo.m_strGuid;
		l_oSQLExec.set["is_delete"] = "true";
		l_oSQLExec.set["update_user"] = "smp_gateway";
		l_oSQLExec.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
	}

	return l_oSQLExec;
}

bool CDepartment::_LoadDataEx(const std::vector<Data::CDeptInfo>& vecDatas)
{
	ICC_LOG_DEBUG(m_pLog, "load dept from redis Begin");

	std::map<std::string, std::string> tmpCodeGuidMaps;
	std::map<std::string, std::string> tmpGuidCodeMaps;
	std::map<std::string, std::vector<CDeptInfo>> mapDeptMaps;

	for (auto dept : vecDatas)
	{
		tmpCodeGuidMaps.insert(std::make_pair(dept.m_strCode, dept.m_strGuid));
		tmpGuidCodeMaps.insert(std::make_pair(dept.m_strGuid, dept.m_strCode));

		{
			std::map<std::string, std::vector<CDeptInfo>>::iterator itr;
			itr = mapDeptMaps.find(dept.m_strParentGuid);
			if (itr != mapDeptMaps.end())
			{
				itr->second.push_back(dept);
			}
			else
			{
				std::vector<CDeptInfo> vecTmps;
				vecTmps.push_back(dept);
				mapDeptMaps.insert(std::make_pair(dept.m_strParentGuid, vecTmps));
			}
		}
	}	

	{		

		std::map<std::string, std::vector<CDeptInfo>>::const_iterator itr_const_depts;
		for (itr_const_depts = mapDeptMaps.begin(); itr_const_depts != mapDeptMaps.end(); ++itr_const_depts)
		{
			std::map<std::string, std::string>::const_iterator itrDepts;
			itrDepts = tmpGuidCodeMaps.find(itr_const_depts->first);
			if (itrDepts != tmpGuidCodeMaps.end())
			{
				std::map<std::string, std::string> mapTmpLevels;
				std::string strKey = DEPTLEVEL + itrDepts->second;
				int iChildCount = itr_const_depts->second.size();
				for (int i = 0; i < iChildCount; ++i)
				{
					CDeptInfo deptInfo = itr_const_depts->second[i];
					mapTmpLevels.insert(std::make_pair(deptInfo.m_strCode, deptInfo.ToJson(m_pJsonFty->CreateJson())));
				}
				if (!m_pRedisClient->HMSet(strKey, mapTmpLevels))
				{
					ICC_LOG_ERROR(m_pLog, "set dept level info from redis failed! [%s]", strKey.c_str());
					return false;
				}
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "set dept level info not find guid! [%s]", itr_const_depts->first.c_str());
			}
		}

		ICC_LOG_DEBUG(m_pLog, "set dept level info success! size[%d]", mapDeptMaps.size());
	}

	return true;
}
