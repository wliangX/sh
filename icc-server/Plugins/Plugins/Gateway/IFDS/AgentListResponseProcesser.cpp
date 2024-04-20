#include "AgentListResponseProcesser.h"

#include "Format.h"


using namespace ICC;

/************************************************************************/
/*                                                                      */
/************************************************************************/
std::string CAgentListResponseProcesser::ProcNotify(const std::string& strMsg)
{
	PROTOCOL::CIfdsAgentListReponseAnalyzer anlyzer;
	if (!_ParseMsg(strMsg, anlyzer))
	{		
		return "";
	}

	std::vector<SeatInfo> vecSeats;
	_UpdateSeatInfo(anlyzer, vecSeats);	

	PROTOCOL::CIfdsSeatInfo ifdsSeatInfo;

	_PacketHeader(ifdsSeatInfo.m_oHeader);
	
	_PacketBody(vecSeats, ifdsSeatInfo.m_oBody);

	return ifdsSeatInfo.ToString(m_pJsonFty->CreateJson());
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
CAgentListResponseProcesser::CAgentListResponseProcesser(JsonParser::IJsonFactoryPtr pJsonFty, StringUtil::IStringUtilPtr pStrUtil, DateTime::IDateTimePtr pDateTime,
	Log::ILogPtr pLog, std::shared_ptr<CSeatStorager> pSeatStorager)
	: m_pJsonFty(pJsonFty), m_pStrUtil(pStrUtil), m_pDateTime(pDateTime), m_pLog(pLog), m_pSeatStorager(pSeatStorager)
{

}

CAgentListResponseProcesser::~CAgentListResponseProcesser()
{

}

bool CAgentListResponseProcesser::_ParseMsg(const std::string& strMsg, PROTOCOL::CIfdsAgentListReponseAnalyzer& agentListInfo)
{
	std::string strTmpLog;

	if (!agentListInfo.ParseString(strMsg, m_pJsonFty->CreateJson()))
	{
		strTmpLog = StringCommon::format("ParseString Error:[%s]", strMsg);
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return false;
	}

	return true;
}

void CAgentListResponseProcesser::_UpdateSeatInfo(const PROTOCOL::CIfdsAgentListReponseAnalyzer& agentListInfo, std::vector<SeatInfo>& vecSeatInfos)
{
	if (!m_pSeatStorager)
	{
		ICC_LOG_ERROR(m_pLog, "%s", "SeatStorager is null!");
		return;
	}

	m_pSeatStorager->RequestAgentListReponse();

	int iCount = agentListInfo.m_oBody.m_vecSeats.size();
	std::map<std::string, SeatInfo> mapSeatInfos;
	for (int i = 0; i < iCount; ++i)
	{
		SeatInfo info;
		info.strNumber = agentListInfo.m_oBody.m_vecSeats[i].m_strAgentNumber;
		info.strAcd = agentListInfo.m_oBody.m_vecSeats[i].m_strAcdName;
		info.strLoginMode = agentListInfo.m_oBody.m_vecSeats[i].m_strLoginMode;
		info.strStatus = agentListInfo.m_oBody.m_vecSeats[i].m_strReadyStatus;
		info.strUpdateTime = agentListInfo.m_oBody.m_vecSeats[i].m_strTime;

		if (mapSeatInfos.find(info.strNumber) == mapSeatInfos.end())
		{
			m_pSeatStorager->QuerySeatDeptByNo(info.strNumber, info.strDeptCode, info.strDeptName, info.strAlias);
			mapSeatInfos.insert(std::make_pair(info.strNumber, info));
		}	
		//vecSeatInfos.push_back(info);	
	}	
	
	std::map<std::string, SeatInfo>::iterator itr;
	for (itr = mapSeatInfos.begin(); itr != mapSeatInfos.end(); ++itr)
	{
		vecSeatInfos.push_back(itr->second);
	}
}

void CAgentListResponseProcesser::_PacketHeader(PROTOCOL::CHeader& header)
{
	header.m_strSystemID = SYSTEMID;
	header.m_strSubsystemID = SUBSYSTEMID;
	header.m_strMsgid = m_pStrUtil->CreateGuid();
	header.m_strCmd = "icc_seatinfo";
	header.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	header.m_strRequest = "topic_clientStatusReport";
	header.m_strRequestType = REQESTMODE_TOPIC;
}

void CAgentListResponseProcesser::_PacketBody(const std::vector<SeatInfo>& vecSeats, PROTOCOL::CIfdsSeatInfo::CBody& body)
{
	int iSize = vecSeats.size();
	body.m_strResult = "result";
	body.m_strTotalCount = StringCommon::format("%d", iSize);

	for (int i = 0; i < iSize; ++i)
	{
		PROTOCOL::CIfdsSeatInfo::CInfo info;
		info.strDeptCode = vecSeats[i].strDeptCode;
		info.strDeptName = vecSeats[i].strDeptName;
		info.strUserName = vecSeats[i].strNumber;
		info.strAlias = vecSeats[i].strAlias;
		info.strStatus = vecSeats[i].strStatus;
		info.strLoginMode = vecSeats[i].strLoginMode;
		info.strAcd = vecSeats[i].strAcd;
		body.m_vecInfos.push_back(info);
	}
}