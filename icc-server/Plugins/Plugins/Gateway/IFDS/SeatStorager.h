#ifndef __SeatStorager_H__
#define __SeatStorager_H__

#include "Boost.h"
#include "INotifyProcesser.h"
#include "Json/IJsonFactory.h"
#include "Protocol/IfdsAgentListRequest.h"
#include <mutex>
#include "CommonTimer.h"

using namespace ICC;

class CSeatStorager : public CCommonTimerCore
{
public:	
	bool InitStorager();
	void QueryAgentInfo();
	void RequestAgentListReponse();
	void AddSeat(const SeatInfo& seatInfo);
	void DeleteSeat(const std::string& strSeatNumber);
	bool ExistSeat(const std::string& strSeatNumber);
	void Seats(std::vector<SeatInfo>& vecSeats);
	void UpdateSeatStatus(const SeatInfo& info);
	void UpdateSeatInfo(const SeatInfo& info);

	bool QuerySeatDeptByNo(const std::string& strNo, std::string& strDeptCode, std::string& strDeptName, std::string& strAlias);

public:
	CSeatStorager(DataBase::IDBConnPtr pDbCon, Log::ILogPtr pLog, JsonParser::IJsonFactoryPtr pJsonFty, StringUtil::IStringUtilPtr pStrUtil, DateTime::IDateTimePtr pDateTime);
	~CSeatStorager();

private:
	bool        _QuerySeats(std::vector<SeatInfo>& vecSeats);
	void        _InitSeats();

	void        _PacketHeader(PROTOCOL::CHeader& header);
	std::string _RequestAgentList();
	void        _QueryAgentList();

	virtual void OnTimer(unsigned long uMsgId);

private:
	typedef std::map<std::string, SeatInfo> SEATMAP;
	std::mutex                      m_mutexSeats;
	SEATMAP                         m_mapSeats;

	DataBase::IDBConnPtr            m_pDBConn;
	Log::ILogPtr                    m_pLog;
	JsonParser::IJsonFactoryPtr     m_pJsonFty;
	StringUtil::IStringUtilPtr      m_pStrUtil;
	DateTime::IDateTimePtr          m_pDateTime;

	CCommonTimer                    m_timerInit;
};

#endif