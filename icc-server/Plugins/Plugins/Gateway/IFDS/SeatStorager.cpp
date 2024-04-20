#include "SeatStorager.h"
#include "Format.h"
#include "CommonDef.h"

const unsigned long INTERVAL_BASE = 2000;

const unsigned long TIMERID_REQUESTAGENTLIST = 2000;
const unsigned long TIMERID_QUERYSEAT = 2001;

const unsigned long TIMERINTERVAL_REQUESTAGENTLIST = INTERVAL_BASE;
const unsigned long TIMERINTERVAL_QUERYSEAT = INTERVAL_BASE;

/************************************************************************/
/*                                                                      */
/************************************************************************/

bool CSeatStorager::InitStorager()
{	
	m_timerInit.AddTimer(this, TIMERID_REQUESTAGENTLIST, TIMERINTERVAL_REQUESTAGENTLIST);
	//_QueryAgentList();
	return true;
}

void CSeatStorager::QueryAgentInfo()
{
	_QueryAgentList();
}

void CSeatStorager::RequestAgentListReponse()
{
	m_timerInit.RemoveTimer(TIMERINTERVAL_REQUESTAGENTLIST);
}

void CSeatStorager::AddSeat(const SeatInfo& seatInfo)
{
	bool bFind = true;
	{
		std::lock_guard<std::mutex> lock(m_mutexSeats);
		if (m_mapSeats.find(seatInfo.strNumber) == m_mapSeats.end())
		{
			m_mapSeats.insert(std::make_pair(seatInfo.strNumber, seatInfo));
			bFind = false;
		}	
		else
		{
			m_mapSeats[seatInfo.strNumber].strAlias = seatInfo.strAlias;
			m_mapSeats[seatInfo.strNumber].strDeptCode = seatInfo.strDeptCode;
			m_mapSeats[seatInfo.strNumber].strDeptName = seatInfo.strDeptName;
		}
	}	

	std::string  strTmpLog = StringCommon::format("add seat! find = %s number = %s", bFind?std::string("true"):std::string("false"), seatInfo.strNumber);
	ICC_LOG_WARNING(m_pLog, "%s", strTmpLog.c_str());	
}

void CSeatStorager::DeleteSeat(const std::string& strSeatNumber)
{
	std::lock_guard<std::mutex> lock(m_mutexSeats);
	m_mapSeats.erase(strSeatNumber);
}

bool CSeatStorager::ExistSeat(const std::string& strSeatNumber)
{
	std::lock_guard<std::mutex> lock(m_mutexSeats);
	return m_mapSeats.find(strSeatNumber) != m_mapSeats.end();
}

void CSeatStorager::Seats(std::vector<SeatInfo>& vecSeats)
{
	std::lock_guard<std::mutex> lock(m_mutexSeats);
	for (SEATMAP::const_iterator itr = m_mapSeats.begin(); itr != m_mapSeats.end(); ++itr)
	{
		vecSeats.push_back(itr->second);
	}
}

void CSeatStorager::UpdateSeatStatus(const SeatInfo& info)
{
	bool bFind = false;
	{
		std::lock_guard<std::mutex> lock(m_mutexSeats);
		SEATMAP::iterator itr = m_mapSeats.find(info.strNumber);
		if (itr != m_mapSeats.end())
		{
			itr->second.strAcd = info.strAcd;
			itr->second.strLoginMode = info.strLoginMode;
			itr->second.strStatus = info.strStatus;
			itr->second.strUpdateTime = info.strUpdateTime;
			bFind = true;
		}	
		else
		{
			m_mapSeats.insert(std::make_pair(info.strNumber, info));
		}
	}
	
	std::string  strTmpLog = StringCommon::format("update seat status ! find = %s number = %s", bFind?std::string("true"):std::string("false"),info.strNumber);
	ICC_LOG_WARNING(m_pLog, "%s", strTmpLog.c_str());
}

void CSeatStorager::UpdateSeatInfo(const SeatInfo& info)
{
	bool bFind = false;
	{
		std::lock_guard<std::mutex> lock(m_mutexSeats);
		SEATMAP::iterator itr = m_mapSeats.find(info.strNumber);
		if (itr != m_mapSeats.end())
		{
			itr->second.strAlias = info.strAlias;
			itr->second.strDeptCode = info.strDeptCode;
			itr->second.strDeptName = info.strDeptName;			
			bFind = true;
		}		
	}
	if (!bFind)
	{
		std::string  strTmpLog = StringCommon::format("update seat info failed! not find number, number = %s", info.strNumber);
		ICC_LOG_WARNING(m_pLog, "%s", strTmpLog.c_str());
	}
}

bool CSeatStorager::QuerySeatDeptByNo(const std::string& strNo, std::string& strDeptCode, std::string& strDeptName, std::string& strAlias)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_seat_dept_and_name_by_no";
	l_SqlRequest.param["number"] = strNo;

	std::string strTmpLog;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	if (l_pResult == nullptr)
	{
		strTmpLog = StringCommon::format("exec sql error, sql = %s", l_SqlRequest.sql_id);
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return false;
	}

	if (!l_pResult->IsValid())
	{
		strTmpLog = StringCommon::format("exec sql error, sql = %s", l_pResult->GetSQL());
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return false;
	}

	strTmpLog = StringCommon::format("exec sql success!, sql = %s", l_pResult->GetSQL());
	ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());

	int iRecordCount = 0;
	if (l_pResult->Next())
	{	
		strDeptCode = l_pResult->GetValue("dept_code");
		strDeptName = l_pResult->GetValue("dept_name");
		strAlias = l_pResult->GetValue("name");
		return true;
	}

	return false;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
CSeatStorager::CSeatStorager(DataBase::IDBConnPtr pDbCon, Log::ILogPtr pLog, JsonParser::IJsonFactoryPtr pJsonFty, StringUtil::IStringUtilPtr pStrUtil, DateTime::IDateTimePtr pDateTime)
	: m_pDBConn(pDbCon), m_pLog(pLog), m_pJsonFty(pJsonFty), m_pStrUtil(pStrUtil), m_pDateTime(pDateTime)
{

}

CSeatStorager::~CSeatStorager()
{

}

bool CSeatStorager::_QuerySeats(std::vector<SeatInfo>& vecSeats)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_seat_all";

	std::string strTmpLog;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	if (l_pResult == nullptr)
	{
		strTmpLog = StringCommon::format("exec sql error, sql = %s", l_SqlRequest.sql_id);
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return false;
	}
	
	if (!l_pResult->IsValid())
	{
		strTmpLog = StringCommon::format("exec sql error, sql = %s", l_pResult->GetSQL());
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return false;
	}

	strTmpLog = StringCommon::format("exec sql success!, sql = %s", l_pResult->GetSQL());
	ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());

	int iRecordCount = 0;
	while (l_pResult->Next())
	{
		SeatInfo seatInfo;
		seatInfo.strDeptCode = l_pResult->GetValue("dept_code");
		seatInfo.strDeptName = l_pResult->GetValue("dept_name");
		seatInfo.strAlias = l_pResult->GetValue("name");
		seatInfo.strNumber = l_pResult->GetValue("no");

		vecSeats.push_back(seatInfo);

		strTmpLog = StringCommon::format("query seat info success::{index = %d, no = %s, name = %s, deptcode = %s, deptname = %s } ", 
			iRecordCount, seatInfo.strNumber, seatInfo.strAlias, seatInfo.strDeptCode, seatInfo.strDeptName);
		ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());
		iRecordCount++;
	}

	return true;
}

void CSeatStorager::_InitSeats()
{
	std::vector<SeatInfo> vecSeats;
	if (!_QuerySeats(vecSeats))
	{
		return;
	}
	m_timerInit.RemoveTimer(TIMERID_QUERYSEAT);
	int iSize = vecSeats.size();
	std::string strTmpLog = StringCommon::format("query seat success! count = %d", iSize);
	ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());
	for (int i = 0; i < iSize; ++i)
	{
		AddSeat(vecSeats[i]);		
	}

	m_timerInit.AddTimer(this, TIMERID_REQUESTAGENTLIST, TIMERINTERVAL_REQUESTAGENTLIST);
}

void CSeatStorager::_PacketHeader(PROTOCOL::CHeader& header)
{
	header.m_strSystemID = SYSTEMID;
	header.m_strSubsystemID = SUBSYSTEMID;
	header.m_strMsgid = m_pStrUtil->CreateGuid();
	header.m_strCmd = "get_agent_list_request";
	header.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	header.m_strRequest = "queue_cti_control";
	header.m_strRequestType = REQESTMODE_QUEUE;
	header.m_strResponse = "";
	header.m_strResponseType = "";
}

std::string CSeatStorager::_RequestAgentList()
{
	PROTOCOL::CIfdsIfdsAgentListRequest request;
	_PacketHeader(request.m_oHeader);
	return request.ToString(m_pJsonFty->CreateJson());
}

void CSeatStorager::_QueryAgentList()
{
	std::string strRequest = _RequestAgentList();
	gfunc_SendMessage(strRequest, msgSendType_RequestRespond);
}

void CSeatStorager::OnTimer(unsigned long uMsgId)
{
        printf("seatstorager ontimer enter\n!");
	if (uMsgId == TIMERID_REQUESTAGENTLIST)
	{
		_QueryAgentList();
	}
	else if (uMsgId == TIMERID_QUERYSEAT)
	{
		//_InitSeats();
	}
}