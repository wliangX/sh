#ifndef __SeatStatusChangedProcesser_H__
#define __SeatStatusChangedProcesser_H__

#include "Boost.h"
#include "INotifyProcesser.h"
#include "Json/IJsonFactory.h"
#include "Protocol/CAgentStateSyncRequest.h"
#include "Protocol/IfdsSeatInfo.h"
#include "SeatStorager.h"

using namespace ICC;

class CSeatStatusChanngedProcesser : public INotifyProcesser
{
public:
	virtual std::string ProcNotify(const std::string& strMsg);

public:
	CSeatStatusChanngedProcesser(JsonParser::IJsonFactoryPtr pJsonFty, StringUtil::IStringUtilPtr pStrUtil, DateTime::IDateTimePtr pDateTime, 
		Log::ILogPtr pLog, std::shared_ptr<CSeatStorager> pSeatStorager);
	virtual ~CSeatStatusChanngedProcesser();

private:
	bool        _ParseMsg(const std::string& strMsg, PROTOCOL::CSetInOutInfo& seatInfo);	
	void        _UpdateSeatStatus(PROTOCOL::CSetInOutInfo& seatInfo);
	void        _PacketHeader(PROTOCOL::CHeader& header);
	void        _PacketBody(const std::vector<SeatInfo>& vecSeats, PROTOCOL::CIfdsSeatInfo::CBody& body);

private:
	JsonParser::IJsonFactoryPtr     m_pJsonFty;
	ICC::StringUtil::IStringUtilPtr m_pStrUtil;
	DateTime::IDateTimePtr          m_pDateTime;
	Log::ILogPtr                    m_pLog;
	std::shared_ptr<CSeatStorager> m_pSeatStorager;
};

#endif