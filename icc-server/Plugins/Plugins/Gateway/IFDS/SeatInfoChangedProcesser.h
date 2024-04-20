#ifndef __SeatInfoChangedProcesser_H__
#define __SeatInfoChangedProcesser_H__

#include "Boost.h"
#include "INotifyProcesser.h"
#include "Json/IJsonFactory.h"
#include "Protocol/IfdsSeatSynAnalyzer.h"
#include "Protocol/IfdsSeatInfo.h"
#include "SeatStorager.h"

using namespace ICC;


class CSeatInfoChangedProcesser : public INotifyProcesser
{
public:
	virtual std::string ProcNotify(const std::string& strMsg);

public:
	CSeatInfoChangedProcesser(JsonParser::IJsonFactoryPtr pJsonFty, StringUtil::IStringUtilPtr pStrUtil, DateTime::IDateTimePtr pDateTime, Log::ILogPtr pLog, std::shared_ptr<CSeatStorager> pSeatStorager);
	virtual ~CSeatInfoChangedProcesser();

private:
	bool        _ParseMsg(const std::string& strMsg, PROTOCOL::CIfdsSeatSynAnalyzer& seatInfo);
	void        _UpdateSeatInfo(const PROTOCOL::CIfdsSeatSynAnalyzer& seatInfo);
	void        _PacketHeader(PROTOCOL::CHeader& header);
	void        _PacketBody(const std::vector<SeatInfo>& vecSeats, PROTOCOL::CIfdsSeatInfo::CBody& body);

private:
	JsonParser::IJsonFactoryPtr     m_pJsonFty;
	ICC::StringUtil::IStringUtilPtr m_pStrUtil;
	DateTime::IDateTimePtr          m_pDateTime;	
	Log::ILogPtr                    m_pLog;
	std::shared_ptr<CSeatStorager>  m_pSeatStorager;
};

#endif