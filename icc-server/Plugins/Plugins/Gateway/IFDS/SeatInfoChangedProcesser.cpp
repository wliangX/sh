#include "SeatInfoChangedProcesser.h"
#include "CommonLogger.h"
#include "Format.h"
#include "Protocol/CAgentStateSyncRequest.h"

const std::string SEATINFO_ADD = "1";
const std::string SEATINFO_UPDATE = "2";
const std::string SEATINFO_DELETE = "3";

/************************************************************************/
/*                                                                      */
/************************************************************************/
std::string CSeatInfoChangedProcesser::ProcNotify(const std::string& strMsg)
{
	/*std::string strTmpLog;
	PROTOCOL::CIfdsSeatSynAnalyzer analyzer;
	if (!_ParseMsg(strMsg, analyzer))
	{
		return "";
	}

	_UpdateSeatInfo(analyzer);

	
	PROTOCOL::CIfdsSeatInfo ifdsSeatInfo;

	_PacketHeader(ifdsSeatInfo.m_oHeader);

	std::vector<SeatInfo> vecSeats;
	m_pSeatStorager->Seats(vecSeats);
	
	_PacketBody(vecSeats, ifdsSeatInfo.m_oBody);

	return ifdsSeatInfo.ToString(m_pJsonFty->CreateJson());*/

	m_pSeatStorager->QueryAgentInfo();
	return "";
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
CSeatInfoChangedProcesser::CSeatInfoChangedProcesser(JsonParser::IJsonFactoryPtr pJsonFty, StringUtil::IStringUtilPtr pStrUtil, DateTime::IDateTimePtr pDateTime, Log::ILogPtr pLog, std::shared_ptr<CSeatStorager> pSeatStorager)
	: m_pJsonFty(pJsonFty), m_pStrUtil(pStrUtil), m_pDateTime(pDateTime), m_pLog(pLog), m_pSeatStorager(pSeatStorager)
{

}

CSeatInfoChangedProcesser::~CSeatInfoChangedProcesser()
{

}

bool CSeatInfoChangedProcesser::_ParseMsg(const std::string& strMsg, PROTOCOL::CIfdsSeatSynAnalyzer& seatInfo)
{
	std::string strTmpLog;
	
	if (!seatInfo.ParseString(strMsg, m_pJsonFty->CreateJson()))
	{
		strTmpLog = StringCommon::format("ParseString Error:[%s]", strMsg);
		ICC_LOG_ERROR(m_pLog, "%s", strTmpLog.c_str());
		return false;
	}

	return true;
}

void CSeatInfoChangedProcesser::_UpdateSeatInfo(const PROTOCOL::CIfdsSeatSynAnalyzer& seatInfo)
{
	if (seatInfo.m_oBody.m_strSyncType == SEATINFO_ADD)
	{
		SeatInfo info;
		info.strNumber = seatInfo.m_oBody.m_strNo;
		info.strAlias = seatInfo.m_oBody.m_strName;
		info.strDeptCode = seatInfo.m_oBody.m_strDeptCode;
		info.strDeptName = seatInfo.m_oBody.m_strDeptName;

		m_pSeatStorager->AddSeat(info);
	}
	else if (seatInfo.m_oBody.m_strSyncType == SEATINFO_UPDATE)
	{
		SeatInfo info;
		info.strNumber = seatInfo.m_oBody.m_strNo;
		info.strAlias = seatInfo.m_oBody.m_strName;
		info.strDeptCode = seatInfo.m_oBody.m_strDeptCode;
		info.strDeptName = seatInfo.m_oBody.m_strDeptName;

		m_pSeatStorager->UpdateSeatInfo(info);
	}
	else if (seatInfo.m_oBody.m_strSyncType == SEATINFO_DELETE)
	{
		m_pSeatStorager->DeleteSeat(seatInfo.m_oBody.m_strNo);
	}
	else
	{
		std::string strTmpLog = StringCommon::format("not support seat syntype, number = %s, syntype = %s", seatInfo.m_oBody.m_strName, seatInfo.m_oBody.m_strSyncType);
		ICC_LOG_WARNING(m_pLog, "%s", strTmpLog.c_str());
	}
}

void CSeatInfoChangedProcesser::_PacketHeader(PROTOCOL::CHeader& header)
{
	header.m_strSystemID = SYSTEMID;
	header.m_strSubsystemID = SUBSYSTEMID;
	header.m_strMsgid = m_pStrUtil->CreateGuid();
	header.m_strCmd = "icc_seatinfo";
	header.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	header.m_strRequest = "topic_clientStatusReport";
	header.m_strRequestType = REQESTMODE_TOPIC;
}

void CSeatInfoChangedProcesser::_PacketBody(const std::vector<SeatInfo>& vecSeats, PROTOCOL::CIfdsSeatInfo::CBody& body)
{
	int iSize = (int)vecSeats.size();
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

		body.m_vecInfos.push_back(info);
	}
}
