#include "Boost.h"
#include "Dict.h"

using namespace ICC;
using namespace Data;

CDictionary::CDictionary(std::vector<Data::CDictInfo> p_vecDict, IResourceManagerPtr p_pIResourceManager) :
m_vecDict(p_vecDict),
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

CDictionary::~CDictionary()
{

}

int CDictionary::ReceiveTotalDataSize()
{
	return (int)m_vecDict.size();
}

void CDictionary::AppendReceiveDataSize(int iReceiveSize)
{
	m_iTotalReceiveDataSize += iReceiveSize;
}

bool CDictionary::SynUpdate()
{	
	/*if (m_vecDict.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no dict data need update!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "begin update dict data, size[%d]", m_vecDict.size());

	std::map<std::string, std::vector<CDictInfo>> mapDictMaps;
	std::map<std::string, std::string> tmp_mapInfo;
	for (Data::CDictInfo dictInfo : m_vecDict)
	{
		tmp_mapInfo[dictInfo.m_strKey] = dictInfo.ToJson(m_pJsonFty->CreateJson());
		std::map<std::string, std::vector<CDictInfo>>::iterator itr;
		itr = mapDictMaps.find(dictInfo.m_strParentKey);
		if (itr != mapDictMaps.end())
		{
			itr->second.push_back(dictInfo);
		}
		else
		{
			std::vector<CDictInfo> vecTmps;
			vecTmps.push_back(dictInfo);
			mapDictMaps.insert(std::make_pair(dictInfo.m_strParentKey, vecTmps));
		}
	}

	if (!m_pRedisClient->HMSet(DICT_KEY, tmp_mapInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "set dict key failed!!");
		return false;
	}

	std::map<std::string, std::vector<CDictInfo>>::const_iterator itr_const_dicts;
	for (itr_const_dicts = mapDictMaps.begin(); itr_const_dicts != mapDictMaps.end(); ++itr_const_dicts)
	{		
		std::map<std::string, std::string> mapTmpLevels;
		std::string strKey = DICT_LEVEL + itr_const_dicts->first;
		int iChildCount = itr_const_dicts->second.size();
		for (int i = 0; i < iChildCount; ++i)
		{
			CDictInfo dictInfo = itr_const_dicts->second[i];
			mapTmpLevels.insert(std::make_pair(dictInfo.m_strKey, dictInfo.ToJson(m_pJsonFty->CreateJson())));
		}
		if (!m_pRedisClient->HMSet(strKey, mapTmpLevels))
		{
			ICC_LOG_ERROR(m_pLog, "set dict level info from redis failed! [%s]", strKey.c_str());
			return false;
		}		
	}

	return true;*/
	

	if (m_vecDict.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no dict data need update!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "begin update dict data, size[%d]", m_vecDict.size());

	std::map<std::string, std::string> mapOldDatas;
	if (!_GetAllOldData(mapOldDatas))
	{
		ICC_LOG_ERROR(m_pLog, "get all old datas failed!!");
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "get all old func datas success! size[%d] ", mapOldDatas.size());

	std::vector<Data::CDictInfo> addDatas;
	std::vector<Data::CDictInfo> updateDatas;
	std::vector<Data::CDictInfo> deleteDatas;
	_CompareData(mapOldDatas, m_vecDict, addDatas, updateDatas, deleteDatas);

	if (!_UpdateData(addDatas, updateDatas, deleteDatas))
	{		
		return false;
	}

	//_LoadDataEx(m_vecDict);

	return true;



	//if (m_vecDict.empty())
	//{
	//	return true;
	//}

	//std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();
	//std::string l_strGuid = m_pDBConn->BeginTransaction();

	//for (Data::CDictInfo l_dict : m_vecDict)
	//{
	//	DataBase::SQLRequest l_oSetSQLReq;
	//	bool l_isExist = m_pRedisClient->HExists(DICT_KEY, l_dict.m_strGuid);
	//	if (l_isExist)
	//	{
	//		std::string l_strInfo;
	//		if (!m_pRedisClient->HGet(DICT_KEY, l_dict.m_strGuid, l_strInfo))
	//		{
	//			ICC_LOG_DEBUG(m_pLog, "get dict failed: %s", l_dict.m_strGuid.c_str());
	//			continue;
	//		}

	//		Data::CDictInfo l_dictInfo;
	//		l_dictInfo.Parse(l_strInfo, m_pJsonFty->CreateJson());
	//		if (IsEqual(l_dictInfo, l_dict))
	//		{
	//			//ICC_LOG_DEBUG(m_pLog, "dict[%s] info is equal", l_dict.m_strCode.c_str());
	//			continue;
	//		}

	//		ICC_LOG_DEBUG(m_pLog, "dict[%s] info is changed.", l_dict.m_strCode.c_str());
	//		l_oSetSQLReq = ConstructSQLStruct(l_dict, "update_icc_t_dict");
	//	}
	//	else
	//	{
	//		//查询单条记录，判断数据库中是否存在
	//		DataBase::SQLRequest l_oQueryDict;
	//		l_oQueryDict = ConstructSQLStruct(l_dict, "select_icc_t_dict");
	//		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oQueryDict, false, l_strGuid);

	//		if (!l_pRSet->IsValid())
	//		{
	//			ICC_LOG_ERROR(m_pLog, "select_icc_t_dict :[%s]", l_pRSet->GetErrorMsg().c_str());
	//			m_pDBConn->Rollback(l_strGuid);
	//			return false;
	//		}

	//		if (l_pRSet->RecordSize() == 1)
	//		{
	//			//恢复标志位false 更新现有记录
	//			l_oSetSQLReq = ConstructSQLStruct(l_dict, "recover_icc_t_dict_flag");
	//			ICC_LOG_DEBUG(m_pLog, "recover dict [%s]", l_dict.m_strCode.c_str());
	//		}
	//		else if (l_pRSet->RecordSize() == 0)
	//		{
	//			//不存在，直接写入一条记录
	//			l_oSetSQLReq = ConstructSQLStruct(l_dict, "insert_icc_t_dict");
	//			ICC_LOG_DEBUG(m_pLog, "insert dict [%s]", l_dict.m_strCode.c_str());
	//		}
	//		else
	//		{
	//			//数据错误
	//			ICC_LOG_ERROR(m_pLog, "dict data error, repeat user :[%s]", l_dict.m_strCode.c_str());
	//			m_pDBConn->Rollback(l_strGuid);
	//		}
	//	}

	//	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSetSQLReq, false, l_strGuid);
	//	if (!l_pRSet->IsValid())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "operate dict info error, rollback: [%s]", l_pRSet->GetErrorMsg().c_str());
	//		m_pDBConn->Rollback(l_strGuid);
	//		return false;
	//	}

	//	for (Data::CValue var : l_dict.m_vecValue)
	//	{
	//		DataBase::SQLRequest l_oValSQLReq;
	//		if (!l_isExist)
	//		{
	//			l_oValSQLReq.sql_id = "insert_icc_t_dict_value";
	//			l_oValSQLReq.param["guid"] = var.m_strGuid;
	//			l_oValSQLReq.param["value"] = var.m_strValue;
	//			l_oValSQLReq.param["lang_guid"] = var.m_strLangGuid;
	//			l_oValSQLReq.param["dict_guid"] = l_dict.m_strGuid;
	//			l_oValSQLReq.param["is_delete"] = "false";
	//			l_oValSQLReq.param["create_user"] = "smp_gateway";
	//			l_oValSQLReq.param["create_time"] = l_strCurrentTime;
	//		}
	//		else
	//		{
	//			l_oValSQLReq.sql_id = "update_icc_t_dict_value";				
	//			l_oValSQLReq.set["value"] = var.m_strValue;
	//			l_oValSQLReq.set["lang_guid"] = var.m_strLangGuid;
	//			l_oValSQLReq.set["dict_guid"] = l_dict.m_strGuid;
	//			l_oValSQLReq.set["is_delete"] = "false";
	//			l_oValSQLReq.set["update_user"] = "smp_gateway";
	//			l_oValSQLReq.set["update_time"] = l_strCurrentTime;
	//			l_oValSQLReq.param["guid"] = var.m_strGuid;
	//		}

	//		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oValSQLReq, false, l_strGuid);
	//		if (!l_pRSet->IsValid())
	//		{
	//			ICC_LOG_ERROR(m_pLog, "operate dict value info error, rollback: [%s]", l_pRSet->GetErrorMsg().c_str());
	//			m_pDBConn->Rollback(l_strGuid);
	//			return false;
	//		}
	//	}
	//}

	//m_pDBConn->Commit(l_strGuid);

	//return true;
}


bool CDictionary::SingleAdd()
{
	if (m_vecDict.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no dict data need add!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "will add dict data size[%d]!!", m_vecDict.size());
	
	std::string l_strGuid = m_pDBConn->BeginTransaction();

	if (!_Add(m_vecDict, l_strGuid))
	{
		return false;
	}		

	m_pDBConn->Commit(l_strGuid);

	ICC_LOG_DEBUG(m_pLog, "add dict data complete! ");

	return true;
}

bool CDictionary::SingleModify()
{
	if (m_vecDict.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no dict data need update!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "will update dict data size[%d]!!", m_vecDict.size());
	
	std::string l_strGuid = m_pDBConn->BeginTransaction();

	if (!_Update(m_vecDict, l_strGuid))
	{
		return false;
	}		

	m_pDBConn->Commit(l_strGuid);

	ICC_LOG_DEBUG(m_pLog, "update dict data complete! ");

	return true;
}

bool CDictionary::SingleDelete()
{
	if (m_vecDict.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "no dict data need delete!!");
		return true;
	}

	ICC_LOG_DEBUG(m_pLog, "will delete dict data size[%d]!!", m_vecDict.size());
	
	std::string l_strGuid = m_pDBConn->BeginTransaction();	

	if (!_Del(m_vecDict, l_strGuid))
	{
		return false;
	}

	m_pDBConn->Commit(l_strGuid);

	ICC_LOG_DEBUG(m_pLog, "delete dict data complete! ");

	return true;
}




bool CDictionary::LoadData()
{
	ICC_LOG_DEBUG(m_pLog, "load dict to redis begin");

	 // 获取所有字典Value信息
	DataBase::SQLRequest l_SqlRequestValue;
	l_SqlRequestValue.sql_id = "select_icc_t_dict_value";
	l_SqlRequestValue.param["is_delete"] = "false";

	DataBase::IResultSetPtr l_resultValue = m_pDBConn->Exec(l_SqlRequestValue);
	if (!l_resultValue->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_resultValue->GetErrorMsg().c_str());
		return false;
	}

	std::vector< CDictInfo> vecDatas;
	while (l_resultValue->Next())
	{
		CDictInfo l_oDict;
		l_oDict.m_strGuid = l_resultValue->GetValue("guid");
		l_oDict.m_strKey = l_resultValue->GetValue("dict_key");
		l_oDict.m_strParentKey = l_resultValue->GetValue("parent_key");
		l_oDict.m_strDictType = l_resultValue->GetValue("dict_type");
		l_oDict.m_strDictCode = l_resultValue->GetValue("dict_code");
		l_oDict.m_strValue = l_resultValue->GetValue("value");
		l_oDict.m_strLanguage = l_resultValue->GetValue("lang_code");
		l_oDict.m_strSort = l_resultValue->GetValue("sort");
		l_oDict.m_strShortCut = l_resultValue->GetValue("shortcut");

		vecDatas.push_back(l_oDict);
	}

	int iDelCount = m_pRedisClient->Del(DICT_KEY);

	ICC_LOG_DEBUG(m_pLog, "delete dict data, count[%d]", iDelCount);

	if (!_UpdateDataToRedis(vecDatas))
	{
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "load dict to redis end, count[%d]", vecDatas.size());

	return true;

	
	

	//std::map<std::string, std::string> mapTmps;
	//if (!_GetAllOldData(mapTmps))
	//{
	//	ICC_LOG_ERROR(m_pLog, "get dict from redis failed!!");
	//	return false;
	//}	

	//std::map<std::string, CDictInfo> tmp_mapDictInfo;

	//{
	//	// 获取所有字典Key信息
	//	DataBase::SQLRequest l_SqlRequest;
	//	l_SqlRequest.sql_id = "select_icc_t_dict_value";
	//	l_SqlRequest.param["is_delete"] = "false";

	//	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest);
	//	if (!l_result->IsValid())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
	//		return false;
	//	}


	//	while (l_result->Next())
	//	{
	//		CDictInfo l_oDict;
	//		l_oDict.m_strGuid = l_result->GetValue("guid");
	//		l_oDict.m_strKey = l_result->GetValue("dict_key");
	//		l_oDict.m_strParentKey = l_result->GetValue("parent_key");
	//		l_oDict.m_strDictType = l_result->GetValue("dict_type");
	//		l_oDict.m_strDictCode = l_result->GetValue("dict_code");
	//		l_oDict.m_strValue = l_result->GetValue("value");
	//		l_oDict.m_strLanguage = l_result->GetValue("lang_code");
	//		l_oDict.m_strSort = l_result->GetValue("sort");
	//		l_oDict.m_strShortCut = l_result->GetValue("shortcut");

	//		tmp_mapDictInfo[l_oDict.m_strGuid] = l_oDict;
	//	}
	//	ICC_LOG_DEBUG(m_pLog, "Load Dict Key map_number[%d] sql[%s]", tmp_mapDictInfo.size(), l_result->GetSQL().c_str());
	//}	

	//{ // 获取所有字典Value信息
	//	DataBase::SQLRequest l_SqlRequestValue;
	//	l_SqlRequestValue.sql_id = "select_icc_t_dict_value";
	//	l_SqlRequestValue.param["is_delete"] = "false";

	//	DataBase::IResultSetPtr l_resultValue = m_pDBConn->Exec(l_SqlRequestValue);
	//	if (!l_resultValue->IsValid())
	//	{
	//		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_resultValue->GetErrorMsg().c_str());
	//		return false;
	//	}

	//	int l_iCount = 0;
	//	int l_iCountValid = 0;
	//	while (l_resultValue->Next())
	//	{
	//		auto it = tmp_mapDictInfo.find(l_resultValue->GetValue("dict_guid"));
	//		if (it != tmp_mapDictInfo.end())
	//		{
	//			CValue l_oValue;
	//			l_oValue.m_strGuid = l_resultValue->GetValue("guid");
	//			l_oValue.m_strValue = l_resultValue->GetValue("value");
	//			l_oValue.m_strLangGuid = l_resultValue->GetValue("lang_guid");

	//			it->second.m_vecValue.push_back(l_oValue);
	//			l_iCountValid++;
	//		}
	//		l_iCount++;
	//	}
	//	ICC_LOG_DEBUG(m_pLog, "Load Dict Value valid_number[%d] db_number[%d] sql[%s]", l_iCountValid, l_iCount, l_resultValue->GetSQL().c_str());
	//}


	/*std::map<std::string, std::vector<std::string>> mapDicts;
	std::map<std::string, CDictInfo>::iterator itr_dict;
	for (itr_dict = tmp_mapDictInfo.begin(); itr_dict != tmp_mapDictInfo.end(); ++itr_dict)
	{
		JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
		if (!l_pJson)
		{
			ICC_LOG_ERROR(m_pLog, "set language info to redis failed! [json is null]");
			return false;
		}

		CDictInfo l_oDict = itr_dict->second;

		bool bNeedWrite = false;

		std::map<std::string, std::string>::iterator itr;
		itr = mapTmps.find(l_oDict.m_strGuid);
		if (itr != mapTmps.end())
		{
			Data::CDictInfo oldInfo;
			if (!oldInfo.Parse(itr->second, m_pJsonFty->CreateJson()))
			{
				bNeedWrite = true;
			}
			else
			{
				if (!_IsEqualEx(oldInfo, l_oDict))
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
			std::string strTmp = l_oDict.ToJson(m_pJsonFty->CreateJson());
			if (!m_pRedisClient->HSet(DICT_KEY, l_oDict.m_strGuid, strTmp))
			{
				ICC_LOG_ERROR(m_pLog, "set dict data to redis failed! content[%s]", strTmp.c_str());
				return false;
			}
		}
	}	

	std::map<std::string, std::string>::const_iterator itr_const;
	for (itr_const = mapTmps.begin(); itr_const != mapTmps.end(); ++itr_const)
	{
		bool bSet = m_pRedisClient->HDel(DICT_KEY, itr_const->first);
		if (!bSet)
		{
			ICC_LOG_ERROR(m_pLog, "delete dict info from redis failed! [%s]", itr_const->first.c_str());
			return false;
		}
	}

	ICC_LOG_DEBUG(m_pLog, "Load Dict Key success, size[%d]", tmp_mapDictInfo.size());

	{
		std::map<std::string, std::string> mapCodeGuidMaps;		
		std::map<std::string, std::vector<CDictInfo>> mapDictMaps;

		std::map<std::string, CDictInfo>::const_iterator itr_const_dict;
		for (itr_const_dict = tmp_mapDictInfo.begin(); itr_const_dict != tmp_mapDictInfo.end(); ++itr_const_dict)
		{
			mapCodeGuidMaps.insert(std::make_pair(itr_const_dict->second.m_strKey, itr_const_dict->first));			

			std::map<std::string, std::vector<CDictInfo>>::iterator itr;
			itr = mapDictMaps.find(itr_const_dict->second.m_strParentKey);
			if (itr != mapDictMaps.end())
			{
				itr->second.push_back(itr_const_dict->second);
			}
			else
			{
				std::vector<CDictInfo> vecTmps;
				vecTmps.push_back(itr_const_dict->second);
				mapDictMaps.insert(std::make_pair(itr_const_dict->second.m_strParentKey, vecTmps));
			}
		}

		{
			std::chrono::steady_clock::time_point clock = std::chrono::steady_clock::now();
			long long ullBeginTime = std::chrono::duration_cast<std::chrono::microseconds>(clock.time_since_epoch()).count();

			

			if (m_pRedisClient->Del(DICT_CODE_GUID_MAP) < 0)
			{
				ICC_LOG_ERROR(m_pLog, "delete dict code info from redis failed! [%s]", DICT_CODE_GUID_MAP);
				return false;
			}
						

			if (!m_pRedisClient->HMSet(DICT_CODE_GUID_MAP, mapCodeGuidMaps))
			{
				ICC_LOG_ERROR(m_pLog, "set dict code info from redis failed! [%s]", DICT_CODE_GUID_MAP);
				return false;
			}

			std::chrono::steady_clock::time_point clock2 = std::chrono::steady_clock::now();
			long long ullBeginTime2 = std::chrono::duration_cast<std::chrono::microseconds>(clock2.time_since_epoch()).count();

			ICC_LOG_DEBUG(m_pLog, "end load base data!! user time[%ld]", ullBeginTime2 - ullBeginTime);
		}

		ICC_LOG_DEBUG(m_pLog, "Load Dict code guid map success, size[%d]", mapCodeGuidMaps.size());

		std::map<std::string, std::vector<CDictInfo>>::const_iterator itr_const_dicts;
		for (itr_const_dicts = mapDictMaps.begin(); itr_const_dicts != mapDictMaps.end(); ++itr_const_dicts)
		{
			std::map<std::string, CDictInfo>::const_iterator itrDicts;
			itrDicts = tmp_mapDictInfo.find(itr_const_dicts->first);
			if (itrDicts != tmp_mapDictInfo.end())
			{
				std::map<std::string, std::string> mapTmpLevels;
				std::string strKey = DICT_LEVEL + itrDicts->second.m_strKey;
				int iChildCount = itr_const_dicts->second.size();
				for (int i = 0; i < iChildCount; ++i)
				{
					CDictInfo dictInfo = itr_const_dicts->second[i];
					mapTmpLevels.insert(std::make_pair(dictInfo.m_strKey, dictInfo.ToJson(m_pJsonFty->CreateJson())));
					
				}
				if (!m_pRedisClient->HMSet(strKey, mapTmpLevels))
				{
					ICC_LOG_ERROR(m_pLog, "set dict level info from redis failed! [%s]", strKey.c_str());
					return false;
				}
			}
			else
			{
				if (itr_const_dicts->first == "0")
				{
					std::map<std::string, std::string> mapTmpLevels;
					std::string strKey = DICT_LEVEL + itr_const_dicts->first;
					int iChildCount = itr_const_dicts->second.size();
					for (int i = 0; i < iChildCount; ++i)
					{
						CDictInfo dictInfo = itr_const_dicts->second[i];
						mapTmpLevels.insert(std::make_pair(dictInfo.m_strKey, dictInfo.ToJson(m_pJsonFty->CreateJson())));						

						if (!m_pRedisClient->HMSet(strKey, mapTmpLevels))
						{
							ICC_LOG_ERROR(m_pLog, "set dict level info from redis failed! [%s]", strKey.c_str());
							return false;
						}
					}
				}
				else
				{
					int i = 0;
				}
			}
		}

		ICC_LOG_DEBUG(m_pLog, "Load Dict level map success, size[%d]", mapDictMaps.size());
	}

	

	return true;*/
}

void CDictionary::Append(std::vector<Data::CDictInfo>& p_vecInfo)
{
	int iCount = p_vecInfo.size();
	for (int i = 0; i < iCount; ++i)
	{
		m_vecDict.push_back(p_vecInfo[i]);
	}
}

void CDictionary::SetTotalPackageCount(const std::string& strTotalPackageCount)
{
	m_strTotalPackageCount = strTotalPackageCount;
}

std::string CDictionary::TotalPackageCount()
{
	return m_strTotalPackageCount;
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
bool CDictionary::_GetAllOldData(std::map<std::string, std::string>& mapOldDatas)
{
	if (m_pRedisClient->HGetAllEx(DICT_KEY, mapOldDatas))
	{
		return true;
	}

	ICC_LOG_ERROR(m_pLog, "get all old dict datas failed!!!");

	return false;
}

bool CDictionary::_CompareData(std::map<std::string, std::string>& mapOldDatas, const std::vector<Data::CDictInfo>& vecNewDatas,
	std::vector<Data::CDictInfo>& addDatas, std::vector<Data::CDictInfo>& updateDatas, std::vector<Data::CDictInfo>& deleteDatas)
{
	int iNewDataCount = vecNewDatas.size();
	for (int i = 0; i < iNewDataCount; ++i)
	{
		std::map<std::string, std::string>::iterator itr = mapOldDatas.find(vecNewDatas[i].m_strGuid);
		if (itr != mapOldDatas.end())
		{
			Data::CDictInfo oldInfo;
			if (!oldInfo.Parse(itr->second, m_pJsonFty->CreateJson()))
			{
				return false;
			}

			mapOldDatas.erase(itr);

			if (_IsEqualEx(oldInfo, vecNewDatas[i]))
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
		Data::CDictInfo oldInfo;
		if (!oldInfo.Parse(itr_const->second, m_pJsonFty->CreateJson()))
		{
			return false;
		}

		deleteDatas.push_back(oldInfo);
	}

	return true;
}

bool CDictionary::_UpdateDataToRedis(const std::vector<Data::CDictInfo>& vecDatas)
{
	std::map<std::string, std::string> mapDatas;

	for (Data::CDictInfo var : vecDatas)
	{
		std::string strTmp = var.ToJson(m_pJsonFty->CreateJson());	

		mapDatas.insert(std::make_pair(var.m_strGuid, strTmp));

		/*if (!m_pRedisClient->HSet(DICT_CODE_GUID_MAP, var.m_strParentKey + "-" + var.m_strKey, var.m_strGuid))
		{
			ICC_LOG_ERROR(m_pLog, "add dict code guid map to redis failed! content[%s]", strTmp.c_str());			
			return false;
		}

		{
			std::string strKey = DICT_LEVEL;
			if (var.m_strParentKey == "0")
			{
				strKey += var.m_strParentKey;
			}
			else
			{
				std::string strDict;
				if (m_pRedisClient->HGet(DICT_KEY, var.m_strParentKey, strDict))
				{
					CDictInfo dictInfo;
					if (dictInfo.Parse(strDict, m_pJsonFty->CreateJson()))
					{
						strKey += dictInfo.m_strKey;
					}
				}
			}

			ICC_LOG_DEBUG(m_pLog, "will add dict level data to redis! key[%s], dict[%s]", strKey.c_str(), strTmp.c_str());

			std::map<std::string, std::string> mapLevels;
			if (m_pRedisClient->HGetAll(strKey, mapLevels))
			{
				mapLevels.insert(std::make_pair(var.m_strKey, strTmp));
				if (!m_pRedisClient->HMSet(strKey, mapLevels))
				{
					ICC_LOG_ERROR(m_pLog, "add dict level data to redis failed! key[%s], dict[%s]", strKey.c_str(), strTmp.c_str());
				}
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "add dict level data to redis not! key[%s], dict[%s]", strKey.c_str(), strTmp.c_str());
			}
		}*/


		//ICC_LOG_DEBUG(m_pLog, "add dict data to redis success! content[%s]", strTmp.c_str());
	}

	if (!mapDatas.empty())
	{
		if (!m_pRedisClient->HMSet(DICT_KEY, mapDatas))
		{
			return false;
		}
	}	

	ICC_LOG_DEBUG(m_pLog, "add dict data to redis success! content[%d]", mapDatas.size());

	return true;
}

bool CDictionary::_Add(const std::vector<Data::CDictInfo>& vecDatas, const std::string& strTransGuid)
{
	ICC_LOG_DEBUG(m_pLog, "will add dict size: [%d]", vecDatas.size());

	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	std::map<std::string, std::string> mapDatas;
	for (Data::CDictInfo var : vecDatas)
	{
		if (mapDatas.find(var.m_strGuid) != mapDatas.end())
		{
			ICC_LOG_WARNING(m_pLog, "same dict data ! guid[%s]", var.m_strGuid.c_str());
			continue;
		}

		DataBase::SQLRequest l_oSetSQLReq;
		l_oSetSQLReq.sql_id = "insert_icc_t_dict_value";
		l_oSetSQLReq.param["guid"] = var.m_strGuid;
		l_oSetSQLReq.param["dict_key"] = var.m_strKey;
		l_oSetSQLReq.param["parent_key"] = var.m_strParentKey;
		l_oSetSQLReq.param["dict_type"] = var.m_strDictType;
		l_oSetSQLReq.param["dict_code"] = var.m_strDictCode;
		l_oSetSQLReq.param["value"] = var.m_strValue;
		l_oSetSQLReq.param["lang_code"] = var.m_strLanguage;
		l_oSetSQLReq.param["dict_guid"] = var.m_strKey;
		l_oSetSQLReq.param["is_delete"] = "false";
		l_oSetSQLReq.param["create_user"] = "smp_gateway";
		l_oSetSQLReq.param["create_time"] = l_strCurrentTime;

		l_oSetSQLReq.param["union_key"] = var.m_strUnionKey;
		l_oSetSQLReq.param["parentunion_key"] = var.m_strParentUnionKey;
		l_oSetSQLReq.param["dictvalue_json"] = var.m_strDictValueJson;

		if (!var.m_strSort.empty())
		{
			l_oSetSQLReq.param["sort"] = var.m_strSort;
		}

		l_oSetSQLReq.param["is_delete"] = "false";
		l_oSetSQLReq.param["shortcut"] = var.m_strShortCut;
		l_oSetSQLReq.param["create_user"] = "smp_gateway";
		l_oSetSQLReq.param["create_time"] = m_pDateTime->CurrentDateTimeStr();

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSetSQLReq, false, strTransGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "add dict data to db failed! sql[%s]", l_pRSet->GetSQL().c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		////for (Data::CValue value : var.m_vecValue)
		//{
		//	DataBase::SQLRequest l_oValSQLReq;
		//	l_oValSQLReq.sql_id = "insert_icc_t_dict_value";
		//	l_oValSQLReq.param["guid"] = var.m_strGuid;
		//	l_oValSQLReq.param["value"] = var.m_strValue;
		//	l_oValSQLReq.param["lang_guid"] = var.m_strLanguage;
		//	l_oValSQLReq.param["dict_guid"] = var.m_strKey;
		//	l_oValSQLReq.param["is_delete"] = "false";
		//	l_oValSQLReq.param["create_user"] = "smp_gateway";
		//	l_oValSQLReq.param["create_time"] = l_strCurrentTime;

		//	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oValSQLReq, false, strTransGuid);
		//	if (!l_pRSet->IsValid())
		//	{
		//		ICC_LOG_ERROR(m_pLog, "operate dict value info error, rollback: [%s]", l_pRSet->GetErrorMsg().c_str());
		//		m_pDBConn->Rollback(strTransGuid);
		//		return false;
		//	}
		//}


		ICC_LOG_DEBUG(m_pLog, "add dict data to db success!!! sql[%s]", l_pRSet->GetSQL().c_str());		

		mapDatas.insert(std::make_pair(var.m_strGuid, var.m_strGuid));
	}

	if (!_UpdateDataToRedis(vecDatas))
	{
		m_pDBConn->Rollback(strTransGuid);
		return false;
	}

	return true;
}

bool CDictionary::_Update(const std::vector<Data::CDictInfo>& vecDatas, const std::string& strTransGuid)
{
	return true;

	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());

	for (Data::CDictInfo var : vecDatas)
	{
		DataBase::SQLRequest l_oSetSQLReq;
		l_oSetSQLReq.sql_id = "update_icc_t_dict_value";
		l_oSetSQLReq.set["dict_key"] = var.m_strKey;
		l_oSetSQLReq.set["parent_key"] = var.m_strParentKey;
		l_oSetSQLReq.set["dict_type"] = var.m_strDictType;
		l_oSetSQLReq.set["dict_code"] = var.m_strDictCode;
		l_oSetSQLReq.set["value"] = var.m_strValue;
		l_oSetSQLReq.set["lang_code"] = var.m_strLanguage;
		l_oSetSQLReq.set["dict_guid"] = var.m_strGuid;
		l_oSetSQLReq.set["is_delete"] = "false";
		l_oSetSQLReq.set["update_user"] = "smp_gateway";
		l_oSetSQLReq.set["update_time"] = l_strCurrentTime;
		l_oSetSQLReq.param["guid"] = var.m_strGuid;

		l_oSetSQLReq.set["union_key"] = var.m_strUnionKey;
		l_oSetSQLReq.set["parentunion_key"] = var.m_strParentUnionKey;
		l_oSetSQLReq.set["dictvalue_json"] = var.m_strDictValueJson;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSetSQLReq, false, strTransGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "update dict data to db failed! sql[%s]", l_pRSet->GetSQL().c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		/*
		for (Data::CValue value : var.m_vecValue)
		{
			DataBase::SQLRequest l_oValSQLReq;
			l_oValSQLReq.sql_id = "update_icc_t_dict_value";
			l_oValSQLReq.set["value"] = value.m_strValue;
			l_oValSQLReq.set["lang_guid"] = value.m_strLangGuid;
			l_oValSQLReq.set["dict_guid"] = var.m_strGuid;
			l_oValSQLReq.set["is_delete"] = "false";
			l_oValSQLReq.set["update_user"] = "smp_gateway";
			l_oValSQLReq.set["update_time"] = l_strCurrentTime;
			l_oValSQLReq.param["guid"] = value.m_strGuid;

			DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oValSQLReq, false, strTransGuid);
			if (!l_pRSet->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "operate dict value info error, rollback: [%s]", l_pRSet->GetErrorMsg().c_str());
				m_pDBConn->Rollback(strTransGuid);
				return false;
			}
		}

		ICC_LOG_DEBUG(m_pLog, "update dict data to db success!!! sql[%s]", l_pRSet->GetSQL().c_str());*/

		//std::string strTmp = var.ToJson(m_pJsonFty->CreateJson());
		//if (!m_pRedisClient->HSet(DICT_KEY, var.m_strGuid, strTmp))
		//{
		//	ICC_LOG_ERROR(m_pLog, "update dict data to redis failed! content[%s]", strTmp.c_str());
		//	m_pDBConn->Rollback(strTransGuid);
		//	return false;
		//}

		//if (!m_pRedisClient->HSet(DICT_CODE_GUID_MAP, var.m_strParentKey + "-" + var.m_strKey, var.m_strGuid))
		//{
		//	ICC_LOG_ERROR(m_pLog, "update dict code guid map to redis failed! content[%s]", strTmp.c_str());
		//	m_pDBConn->Rollback(strTransGuid);
		//	return false;
		//}

		//{
		//	std::string strKey = DICT_LEVEL;
		//	if (var.m_strParentKey == "0")
		//	{
		//		strKey += var.m_strParentKey;
		//	}
		//	else
		//	{
		//		std::string strDict;
		//		if (m_pRedisClient->HGet(DICT_KEY, var.m_strParentKey, strDict))
		//		{
		//			CDictInfo dictInfo;
		//			if (dictInfo.Parse(strDict, m_pJsonFty->CreateJson()))
		//			{
		//				strKey += dictInfo.m_strKey;
		//			}
		//		}
		//	}

		//	ICC_LOG_DEBUG(m_pLog, "will update dict level data to redis! key[%s], dict[%s]", strKey.c_str(), strTmp.c_str());

		//	std::map<std::string, std::string> mapLevels;
		//	if (m_pRedisClient->HGetAll(strKey, mapLevels))
		//	{
		//		//mapLevels.insert(std::make_pair(var.m_strCode, strTmp));
		//		mapLevels[var.m_strKey] = strTmp;
		//		if (!m_pRedisClient->HMSet(strKey, mapLevels))
		//		{
		//			ICC_LOG_ERROR(m_pLog, "update dict level data to redis failed! key[%s], dict[%s]", strKey.c_str(), strTmp.c_str());
		//		}
		//	}
		//	else
		//	{
		//		ICC_LOG_ERROR(m_pLog, "update dict level data to redis not! key[%s], dict[%s]", strKey.c_str(), strTmp.c_str());
		//	}
		//}

		//ICC_LOG_DEBUG(m_pLog, "update dict data to redis success! content[%s]", strTmp.c_str());
	}

	if (!_UpdateDataToRedis(vecDatas))
	{
		m_pDBConn->Rollback(strTransGuid);
		return false;
	}

	return true;
}

bool CDictionary::_Del(const std::vector<Data::CDictInfo>& vecDatas, const std::string& strTransGuid)
{
	return true;

	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());

	for (Data::CDictInfo var : vecDatas)
	{
		DataBase::SQLRequest l_oDeleteSQLReq;
		l_oDeleteSQLReq.sql_id = "delete_icc_t_dict_value";
		l_oDeleteSQLReq.param["guid"] = var.m_strGuid;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oDeleteSQLReq, false, strTransGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "delete dict data from db failed! sql[%s]", l_pRSet->GetSQL().c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}
/*
		{
			DataBase::SQLRequest l_oDeleteValueSQLReq;
			l_oDeleteSQLReq.sql_id = "delete_icc_t_dict_value";
			l_oDeleteSQLReq.param["dict_guid"] = var.m_strGuid;

			DataBase::IResultSetPtr l_pValueRSet = m_pDBConn->Exec(l_oDeleteValueSQLReq, false, strTransGuid);
			if (!l_pValueRSet->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "delete dict value from db failed! sql[%s]", l_pValueRSet->GetSQL().c_str());
				m_pDBConn->Rollback(strTransGuid);
				return false;
			}
		}*/


		ICC_LOG_ERROR(m_pLog, "delete dict data from db success!!! sql[%s]", l_pRSet->GetSQL().c_str());

		/*std::string strTmp;
		if (!m_pRedisClient->HGet(DICT_KEY, var.m_strGuid, strTmp))
		{
			ICC_LOG_WARNING(m_pLog, "delete dict data from redis not find data! guid[%s]", var.m_strGuid.c_str());
			return true;
		}*/

		if (!m_pRedisClient->HDel(DICT_KEY, var.m_strGuid))
		{
			ICC_LOG_ERROR(m_pLog, "delete dict data from redis failed! guid[%s]", var.m_strGuid.c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		/*CDictInfo tmpDict;
		if (!tmpDict.Parse(strTmp, ICCGetIJsonFactory()->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "delete dict data parse data failed! guid[%s], dict[%s]", var.m_strGuid.c_str(), strTmp.c_str());
			return false;
		}

		if (!m_pRedisClient->HDel(DICT_CODE_GUID_MAP, tmpDict.m_strParentKey + "-" + tmpDict.m_strKey))
		{
			ICC_LOG_ERROR(m_pLog, "delete dict code guid map to redis failed! content[%s]", var.m_strKey.c_str());
			m_pDBConn->Rollback(strTransGuid);
			return false;
		}

		{
			std::string strKey = DICT_LEVEL + tmpDict.m_strParentKey;			

			ICC_LOG_DEBUG(m_pLog, "will delete dict level data to redis! key[%s], value[%s]", strKey.c_str(), (tmpDict.m_strGuid + "-----" + tmpDict.m_strKey).c_str());

			std::map<std::string, std::string> mapLevels;
			if (m_pRedisClient->HGetAll(strKey, mapLevels))
			{
				mapLevels.erase(var.m_strKey);
				if (!m_pRedisClient->HMSet(strKey, mapLevels))
				{
					ICC_LOG_ERROR(m_pLog, "delete dict level data to redis failed! key[%s], dict[%s]", strKey.c_str(), (var.m_strGuid + "----" + var.m_strKey).c_str());
				}
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "delete dict level data to redis not! key[%s], dict[%s]", strKey.c_str(), (var.m_strGuid + "----" + var.m_strKey).c_str());
			}
		}*/

		ICC_LOG_DEBUG(m_pLog, "delete dict data from redis success! guid[%s]", var.m_strGuid.c_str());
	}

	return true;
}

bool CDictionary::_UpdateData(const std::vector<Data::CDictInfo>& addDatas, const std::vector<Data::CDictInfo>& updateDatas, const std::vector<Data::CDictInfo>& deleteDatas)
{
	ICC_LOG_DEBUG(m_pLog, "update dict data begin! addDatas[%d], updateDatas[%d], deleteDatas[%d]",
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

	ICC_LOG_DEBUG(m_pLog, "update dict data complete! ");

	return true;
}


bool CDictionary::_Delete()
{
	if (m_vecDict.empty())
	{
		return true;
	}

	std::string l_strGuid = m_pDBConn->BeginTransaction();
	for (Data::CDictInfo l_dict : m_vecDict)
	{
		DataBase::SQLRequest l_oDeleteSQLReq;
		l_oDeleteSQLReq.sql_id = "update_icc_t_dict";
		l_oDeleteSQLReq.set["is_delete"] = "true";
		l_oDeleteSQLReq.param["guid"] = l_dict.m_strGuid;

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oDeleteSQLReq, false, l_strGuid);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "delete dict info error, rollback: [%s]", l_pRSet->GetErrorMsg().c_str());
			m_pDBConn->Rollback(l_strGuid);
			return false;
		}

		for (Data::CDictInfo var : m_vecDict)
		{
			DataBase::SQLRequest l_SqlRequest;
			l_SqlRequest.sql_id = "update_icc_t_dict_value";
			l_SqlRequest.set["is_delete"] = "true";
			l_SqlRequest.param["dict_guid"] = var.m_strGuid;

			DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_SqlRequest, false, l_strGuid);
			if (!l_pRSet->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "delete dict value info error, rollback: [%s]", l_pRSet->GetErrorMsg().c_str());
				m_pDBConn->Rollback(l_strGuid);
				return false;
			}
		}
	}

	m_pDBConn->Commit(l_strGuid);
	return true;
}

bool CDictionary::_HSet()
{
	if (m_vecDict.empty())
	{
		return true;
	}

	std::map<std::string, std::string> tmp_mapInfo;
	for (Data::CDictInfo var : m_vecDict)
	{
		tmp_mapInfo[var.m_strGuid] = var.ToJson(m_pJsonFty->CreateJson());
	}

	return m_pRedisClient->HMSet(DICT_KEY, tmp_mapInfo);
}

void CDictionary::_HDel()
{
	for (Data::CDictInfo var : m_vecDict)
	{
		m_pRedisClient->HDel(DICT_KEY, var.m_strGuid);
	}
}

bool CDictionary::_IsEqualEx(Data::CDictInfo p_oldDict, Data::CDictInfo l_newDict)
{
	bool bResult = p_oldDict.m_strGuid == l_newDict.m_strGuid
		&& p_oldDict.m_strParentKey == l_newDict.m_strParentKey
		&& p_oldDict.m_strKey == l_newDict.m_strKey
		&& p_oldDict.m_strDictType == l_newDict.m_strDictType
		&& p_oldDict.m_strDictCode == l_newDict.m_strDictCode
		&& p_oldDict.m_strShortCut == l_newDict.m_strShortCut
		&& p_oldDict.m_strSort == l_newDict.m_strSort
	    && p_oldDict.m_strValue == l_newDict.m_strValue
	    && p_oldDict.m_strLanguage == l_newDict.m_strLanguage
		&& p_oldDict.m_strDictValueJson == l_newDict.m_strDictValueJson
		&& p_oldDict.m_strUnionKey == l_newDict.m_strUnionKey
		&& p_oldDict.m_strParentUnionKey == l_newDict.m_strParentUnionKey;
	return bResult;
}

bool CDictionary::_IsEqual(Data::CDictInfo p_oldDict, Data::CDictInfo l_newDict)
{
	bool bResult = p_oldDict.m_strGuid == l_newDict.m_strGuid
		&& p_oldDict.m_strParentKey == l_newDict.m_strParentKey
		&& p_oldDict.m_strKey == l_newDict.m_strKey
		&& p_oldDict.m_strShortCut == l_newDict.m_strShortCut
		&& p_oldDict.m_strSort == l_newDict.m_strSort;

	if (!bResult || p_oldDict.m_vecValue.size() != l_newDict.m_vecValue.size())
	{
		return false;
	}

	for (auto value : p_oldDict.m_vecValue)
	{
		bResult = false;

		for (auto l_oRValue : l_newDict.m_vecValue)
		{
			if (l_oRValue.m_strLangGuid == value.m_strLangGuid
				&& l_oRValue.m_strGuid == value.m_strGuid
				&& l_oRValue.m_strValue == value.m_strValue)
			{
				bResult = true;
				break;
			}
		}
		if (!bResult)
		{
			return false;
		}
	}

	return true;
}

DataBase::SQLRequest CDictionary::_ConstructSQLStruct(Data::CDictInfo& p_ditcInfo, std::string p_strSqlID)
{
	DataBase::SQLRequest l_oSQLExec;
	if (p_strSqlID == "select_icc_t_dict")
	{
		l_oSQLExec.sql_id = "select_icc_t_dict";
		l_oSQLExec.param["code"] = p_ditcInfo.m_strKey;
		//l_oSQLExec.param["is_delete"] = "true";
	}
	else if (p_strSqlID == "update_icc_t_dict")
	{
		l_oSQLExec.sql_id = "update_icc_t_dict";
		l_oSQLExec.set["parent_guid"] = p_ditcInfo.m_strParentKey;
		l_oSQLExec.set["code"] = p_ditcInfo.m_strKey;
		if (!p_ditcInfo.m_strSort.empty())
		{
			l_oSQLExec.set["sort"] = p_ditcInfo.m_strSort;
		}
		l_oSQLExec.set["is_delete"] = "false";
		l_oSQLExec.set["shortcut"] = p_ditcInfo.m_strShortCut;
		l_oSQLExec.set["update_user"] = "smp_gateway";
		l_oSQLExec.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
		l_oSQLExec.param["guid"] = p_ditcInfo.m_strGuid;
	}
	else if (p_strSqlID == "recover_icc_t_dict_flag")
	{
		l_oSQLExec.sql_id = "update_icc_t_dict";
		l_oSQLExec.set["guid"] = p_ditcInfo.m_strGuid;
		l_oSQLExec.set["parent_guid"] = p_ditcInfo.m_strParentKey;
		l_oSQLExec.set["code"] = p_ditcInfo.m_strKey;
		if (!p_ditcInfo.m_strSort.empty())
		{
			l_oSQLExec.set["sort"] = p_ditcInfo.m_strSort;
		}
		l_oSQLExec.set["is_delete"] = "false";
		l_oSQLExec.set["shortcut"] = p_ditcInfo.m_strShortCut;
		l_oSQLExec.set["update_user"] = "smp_gateway";
		l_oSQLExec.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
		l_oSQLExec.param["code"] = p_ditcInfo.m_strKey;
	}
	else if (p_strSqlID == "insert_icc_t_dict")
	{
		l_oSQLExec.sql_id = "insert_icc_t_dict";
		l_oSQLExec.param["guid"] = p_ditcInfo.m_strGuid;
		l_oSQLExec.param["parent_guid"] = p_ditcInfo.m_strParentKey;
		l_oSQLExec.param["code"] = p_ditcInfo.m_strKey;

		if (!p_ditcInfo.m_strSort.empty())
		{
			l_oSQLExec.param["sort"] = p_ditcInfo.m_strSort;
		}

		l_oSQLExec.param["is_delete"] = "false";
		l_oSQLExec.param["shortcut"] = p_ditcInfo.m_strShortCut;
		l_oSQLExec.param["create_user"] = "smp_gateway";
		l_oSQLExec.param["create_time"] = m_pDateTime->CurrentDateTimeStr();
	}
	else if (p_strSqlID == "delete_icc_t_dict")
	{
		l_oSQLExec.sql_id = "delete_icc_t_dict";
		l_oSQLExec.param["guid"] = p_ditcInfo.m_strGuid;
		//l_oSQLExec.set["is_delete"] = "true";
		//l_oSQLExec.set["update_user"] = "smp_gateway";
		//l_oSQLExec.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
	}

	return l_oSQLExec;
}

bool CDictionary::_LoadDataEx(const std::vector<Data::CDictInfo>& vecDatas)
{
	ICC_LOG_DEBUG(m_pLog, "load dict from redis Begin");

	std::map<std::string, CDictInfo> tmp_mapDictInfo;

	std::map<std::string, std::string> mapCodeGuidMaps;
	std::map<std::string, std::vector<CDictInfo>> mapDictMaps;

	for (auto dict : vecDatas)
	{
		tmp_mapDictInfo[dict.m_strGuid] = dict;
		mapCodeGuidMaps.insert(std::make_pair(dict.m_strKey, dict.m_strGuid));

		std::map<std::string, std::vector<CDictInfo>>::iterator itr;
		//itr = mapDictMaps.find(dict.m_strDictType + "-" + dict.m_strParentKey);
		itr = mapDictMaps.find(dict.m_strParentKey);
		if (itr != mapDictMaps.end())
		{
			itr->second.push_back(dict);
		}
		else
		{
			std::vector<CDictInfo> vecTmps;
			vecTmps.push_back(dict);
			//mapDictMaps.insert(std::make_pair(dict.m_strDictType + "-" + dict.m_strParentKey, vecTmps));
			mapDictMaps.insert(std::make_pair(dict.m_strParentKey, vecTmps));
		}
	}


	ICC_LOG_DEBUG(m_pLog, "Load Dict Key success, size[%d]", tmp_mapDictInfo.size());

	{	

		std::map<std::string, std::vector<CDictInfo>>::const_iterator itr_const_dicts;
		for (itr_const_dicts = mapDictMaps.begin(); itr_const_dicts != mapDictMaps.end(); ++itr_const_dicts)
		{
			std::map<std::string, std::string> mapTmpLevels;
			std::string strKey = DICT_LEVEL + itr_const_dicts->first;
			int iChildCount = itr_const_dicts->second.size();
			for (int i = 0; i < iChildCount; ++i)
			{
				CDictInfo dictInfo = itr_const_dicts->second[i];
				mapTmpLevels.insert(std::make_pair(dictInfo.m_strKey, dictInfo.ToJson(m_pJsonFty->CreateJson())));					
			}
			if (!mapTmpLevels.empty())
			{
				if (!m_pRedisClient->HMSet(strKey, mapTmpLevels))
				{
					ICC_LOG_ERROR(m_pLog, "set dict level info from redis failed! [%s]", strKey.c_str());
					return false;
				}
			}
					
		}

		ICC_LOG_DEBUG(m_pLog, "Load Dict level map success, size[%d]", mapDictMaps.size());
	}



	return true;
}

