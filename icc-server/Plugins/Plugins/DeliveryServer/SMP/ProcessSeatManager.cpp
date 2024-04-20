#include "Boost.h"
#include "ProcessSeatManager.h"

using namespace ICC;
using namespace Data;

CProcessSeatManager::CProcessSeatManager(Log::ILogPtr pLog, StringUtil::IStringUtilPtr pString, DataBase::IDBConnPtr pDBConn, 
	JsonParser::IJsonFactoryPtr pJsonFty, DateTime::IDateTimePtr pDateTime)
: IBasicDataManager(pJsonFty), m_pLog(pLog),m_pString(pString),m_pDBConn(pDBConn),m_pJsonFty(pJsonFty), m_pDateTime(pDateTime)
{
	
}

CProcessSeatManager::~CProcessSeatManager()
{

}

/*****************************************************************
 * 
 ****************************************************************/
IBasicDataPtr CProcessSeatManager::_CreateBasicDataPtr()
{
	return boost::make_shared<CProcessSeat>();
}

bool CProcessSeatManager::_LoadDataFromDb(std::map<std::string, IBasicDataPtr>& mapSources)
{
	DataBase::SQLRequest sql;
	sql.sql_id = "select_icc_t_process_seat_telephones";

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(sql);
	ICC_LOG_DEBUG(m_pLog, "_LoadDataFromDb sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "_LoadDataFromDb query db failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	while (l_pResult->Next())
	{
		ProcessSeatPtr pData = boost::make_shared<CProcessSeat>();
		if (!pData)
		{
			return false;
		}

		pData->m_strGuid = l_pResult->GetValue("guid");
		pData->m_strSeatsGuid = l_pResult->GetValue("seats_guid");
		pData->m_strTelephone = l_pResult->GetValue("telephone");
		pData->m_strIp = l_pResult->GetValue("ip");
		pData->m_strOrgCode = l_pResult->GetValue("org_code");
		pData->m_strOrgName = l_pResult->GetValue("org_name");
		mapSources.insert(std::make_pair(pData->m_strGuid, pData));
	}

	ICC_LOG_DEBUG(m_pLog, "_LoadDataFromDb complete, size:%d", mapSources.size());

	return true;
}

bool CProcessSeatManager::_AddData(const std::vector<IBasicDataPtr>& vecAdds)
{
	ICC_LOG_DEBUG(m_pLog, "_AddData begin, size:%d", vecAdds.size());

	DataBase::SQLRequest sql;
	sql.sql_id = "insert_icc_t_process_seat_telephones";

	int iCount = vecAdds.size();
	for (int i = 0; i < iCount; ++i)
	{
		boost::shared_ptr<CProcessSeat> pData = boost::dynamic_pointer_cast<CProcessSeat>(vecAdds[i]);
		sql.param["guid"] = pData->m_strGuid;
		sql.param["seats_guid"] = pData->m_strSeatsGuid;
		sql.param["org_code"] = pData->m_strOrgCode;
		sql.param["org_name"] = pData->m_strOrgName;
		sql.param["telephone"] = pData->m_strTelephone;
		sql.param["ip"] = pData->m_strIp;
		sql.param["update_time"] = m_pDateTime->CurrentDateTimeStr();

		DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(sql);
		ICC_LOG_DEBUG(m_pLog, "_AddData sql:[%s]", l_pResult->GetSQL().c_str());

		if (!l_pResult->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "_AddData db failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
			return false;
		}
	}

	ICC_LOG_DEBUG(m_pLog, "_AddData end, size:%d", vecAdds.size());

	return true;
}

bool CProcessSeatManager::_UpdateData(const std::vector<IBasicDataPtr>& vecUpdates)
{
	ICC_LOG_DEBUG(m_pLog, "_UpdateData begin, size:%d", vecUpdates.size());

	DataBase::SQLRequest sql;
	sql.sql_id = "update_icc_t_process_seat_telephones";

	int iCount = vecUpdates.size();
	for (int i = 0; i < iCount; ++i)
	{
		boost::shared_ptr<CProcessSeat> pData = boost::dynamic_pointer_cast<CProcessSeat>(vecUpdates[i]);
		sql.param["guid"] = pData->m_strGuid;
		sql.param["seats_guid"] = pData->m_strSeatsGuid;
		sql.param["org_code"] = pData->m_strOrgCode;
		sql.param["org_name"] = pData->m_strOrgName;
		sql.param["telephone"] = pData->m_strTelephone;
		sql.param["ip"] = pData->m_strIp;
		sql.param["update_time"] = m_pDateTime->CurrentDateTimeStr();

		DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(sql);
		ICC_LOG_DEBUG(m_pLog, "_UpdateData sql:[%s]", l_pResult->GetSQL().c_str());

		if (!l_pResult->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "_UpdateData db failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
			return false;
		}
	}

	ICC_LOG_DEBUG(m_pLog, "_UpdateData end, size:%d", vecUpdates.size());

	return true;
}

bool CProcessSeatManager::_DeleteData(const std::vector<IBasicDataPtr>& vecDeletes)
{
	ICC_LOG_DEBUG(m_pLog, "_DeleteData begin, size:%d", vecDeletes.size());

	DataBase::SQLRequest sql;
	sql.sql_id = "delete_icc_t_process_seat_telephones";
	
	int iCount = vecDeletes.size();
	for (int i = 0; i < iCount; ++i)
	{
		sql.param["guid"] = vecDeletes[i]->m_strGuid;

		DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(sql);
		ICC_LOG_DEBUG(m_pLog, "_DeleteData sql:[%s]", l_pResult->GetSQL().c_str());

		if (!l_pResult->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "_AddData db failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
			return false;
		}
	}

	ICC_LOG_DEBUG(m_pLog, "_DeleteData end, size:%d", vecDeletes.size());

	return true;
}

bool CProcessSeatManager::_ParseData(const std::string& strData, std::map<std::string, IBasicDataPtr>& mapInputs)
{
	ICC_LOG_DEBUG(m_pLog, "_ParseData begin, data:[%s]", strData.c_str());

	JsonParser::IJsonPtr pJson = m_pJsonFty->CreateJson();
	if (!pJson)
	{
		return false;
	}

	if (!pJson->LoadJson(strData))
	{
		return false;
	}

    std::string strDeptCode = pJson->GetNodeValue("/orgCode", "");
	std::string strDeptName = pJson->GetNodeValue("/orgName", "");

	int iCount = pJson->GetCount("/books");
	for (int i = 0; i < iCount; ++i)
	{
		std::string l_strPrefixPath("/books/" + std::to_string(i));

		ProcessSeatPtr pData = boost::make_shared<CProcessSeat>();
		if (!pData)
		{
			return false;
		}

		pData->m_strGuid = pJson->GetNodeValue(l_strPrefixPath + "/guid", "");
		pData->m_strSeatsGuid = pJson->GetNodeValue(l_strPrefixPath + "/seatsGuid", "");
		pData->m_strTelephone = pJson->GetNodeValue(l_strPrefixPath + "/telephone", "");
		pData->m_strIp = pJson->GetNodeValue(l_strPrefixPath + "/ip", "");
		pData->m_strOrgCode = strDeptCode;
		pData->m_strOrgName = strDeptName;

		mapInputs.insert(std::make_pair(pData->m_strGuid, pData));
	}	

	return true;
}

