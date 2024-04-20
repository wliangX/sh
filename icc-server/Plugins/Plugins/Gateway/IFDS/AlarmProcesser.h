#ifndef __AlarmProcesser_H__
#define __AlarmProcesser_H__

#include "Boost.h"
#include "INotifyProcesser.h"
#include "Json/IJsonFactory.h"
#include "Protocol/CWebAlarmSync.h"
#include "Protocol/ifdsAlarmInfo.h"
#include "CommonTimer.h"
#include <atomic>

using namespace ICC;

class CAlarmProcesser : public INotifyProcesser
{
public:
	virtual std::string ProcNotify(const std::string& strMsg);
	virtual bool        InitProcesser();
	virtual void        UninitProcesser();

public:
	CAlarmProcesser(JsonParser::IJsonFactoryPtr pJsonFty, StringUtil::IStringUtilPtr pStrUtil, DateTime::IDateTimePtr pDateTime, Log::ILogPtr pLog, 
		DataBase::IDBConnPtr pDbCon, Config::IConfigPtr pConfig);
	virtual ~CAlarmProcesser();

private:		
	bool         _InitProcesser();

	std::string _QueryDictGuidByCode(const std::string& strCode);
	std::string _QueryDictValue(const std::string& strDictGuid, const std::string& strLangGuid);
	std::string _QuseryLangGuidByLangCode(const std::string& strLangCode);
	std::string _TranslateCode(const std::string& strCode);

	bool        _InitActualAlarmBuffer(std::vector<PROTOCOL::CIfdsAlarmInfo::CActualAlarmInfo>& vecAlarms);
	void        _TransAlarmCode(std::vector<PROTOCOL::CIfdsAlarmInfo::CActualAlarmInfo>& vecAlarms);

	bool        _ParseMsg(const std::string& strMsg, PROTOCOL::CWebAlarmSync& alarmInfo);
	void        _TranslateMsg(PROTOCOL::CWebAlarmSync& alarmInfo);
	//void        _InsertBuffer(PROTOCOL::CWebAlarmSync& alarmInfo);
	void        _PacketHeader(PROTOCOL::CHeader& header);
	void        _PacketBody(PROTOCOL::CIfdsAlarmInfo::CBody& body, const std::vector<PROTOCOL::CIfdsAlarmInfo::CActualAlarmInfo>& vecAlarms);

	void        _ProcActualAlarm();
	std::string _QueryActualAlram();

private:
	JsonParser::IJsonFactoryPtr     m_pJsonFty;
	ICC::StringUtil::IStringUtilPtr m_pStrUtil;
	DateTime::IDateTimePtr          m_pDateTime;
	Log::ILogPtr                    m_pLog;
	DataBase::IDBConnPtr            m_pDbCon;
	Config::IConfigPtr              m_pConfig;
	std::map<std::string, std::string> m_mapAlarmStates;
	std::string                     m_strLangGuid;	
	
	std::atomic_bool                m_bInitCompleteFlag;
	std::atomic_ullong              m_ullPreUpdateTime;
};

#endif