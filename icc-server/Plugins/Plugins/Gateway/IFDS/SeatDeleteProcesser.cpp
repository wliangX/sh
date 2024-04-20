#include "SeatDeleteProcesser.h"
#include "CommonLogger.h"
#include "Format.h"
#include "Protocol/CAgentStateSyncRequest.h"


/************************************************************************/
/*                                                                      */
/************************************************************************/
std::string CSeatDeleteProcesser::ProcNotify(const std::string& strMsg)
{
	std::string strTmpLog;
	PROTOCOL::CDeleteSeatRequest deleteRequest;
	if (!_ParseMsg(strMsg, deleteRequest))
	{
		return "";
	}
	
	if (m_pSeatStorager)
	{
		m_pSeatStorager->DeleteSeat(deleteRequest.m_oBody.m_strNo);
	}

	PROTOCOL::CIfdsSeatInfo ifdsSeatInfo;

	_PacketHeader(ifdsSeatInfo.m_oHeader);

	std::vector<SeatInfo> vecSeats;
	if (m_pSeatStorager)
	{
		m_pSeatStorager->Seats(vecSeats);
	}
	_PacketBody(vecSeats, ifdsSeatInfo.m_oBody);

	return ifdsSeatInfo.ToString(m_pJsonFty->CreateJson());
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
CSeatDeleteProcesser::CSeatDeleteProcesser(JsonParser::IJsonFactoryPtr pJsonFty, StringUtil::IStringUtilPtr pStrUtil, DateTime::IDateTimePtr pDateTime, Log::ILogPtr pLog, std::shared_ptr<CSeatStorager> pSeatStorager)
	: m_pJsonFty(pJsonFty), m_pStrUtil(pStrUtil), m_pDateTime(pDateTime), m_pLog(pLog), m_pSeatStorager(pSeatStorager)
{

}

CSeatDeleteProcesser::~CSeatDeleteProcesser()
{

}

bool CSeatDeleteProcesser::_ParseMsg(const std::string& strMsg, PROTOCOL::CDeleteSeatRequest& seatInfo)
{
	std::string strTmpLog;
	
	if (!seatInfo.ParseString(strMsg, m_pJsonFty->CreateJson()))
	{
		strTmpLog = StringCommon::format("ParseString Error:[%s]", strMsg);
		ICC_LOG_DEBUG(m_pLog, "%s", strTmpLog.c_str());
		return false;
	}

	return true;
}

void CSeatDeleteProcesser::_PacketHeader(PROTOCOL::CHeader& header)
{
	header.m_strSystemID = SYSTEMID;
	header.m_strSubsystemID = SUBSYSTEMID;
	header.m_strMsgid = m_pStrUtil->CreateGuid();
	header.m_strCmd = "icc_seatinfo";
	header.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	header.m_strRequest = "topic_clientStatusReport";
	header.m_strRequestType = "1";
}

void CSeatDeleteProcesser::_PacketBody(const std::vector<SeatInfo>& vecSeats, PROTOCOL::CIfdsSeatInfo::CBody& body)
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

		body.m_vecInfos.push_back(info);
	}
}

