#ifndef __AlarmStatisticsProcesser_H__
#define __AlarmStatisticsProcesser_H__

#include "Boost.h"
#include "INotifyProcesser.h"
#include "Protocol/StatisticsAlarm.h"
#include "CommonTimer.h"

using namespace ICC;

struct DeptInfo
{
	std::string strDeptCode;
	std::string strDeptName;
	int         iAlarmCount;
	int         iCallRefCount;
	int         iCallReleaseCount;

	DeptInfo(){ iAlarmCount = 0; iCallRefCount = 0; iCallReleaseCount = 0; }
	DeptInfo(const DeptInfo& other)
	{
		_Assign(other);
	}
	DeptInfo& operator=(const DeptInfo& other)
	{
		if (&other != this)
		{
			_Assign(other);
		}
		return *this;
	}
	void _Assign(const DeptInfo& other)
	{
		strDeptCode = other.strDeptCode;
		strDeptName = other.strDeptName;
		iAlarmCount = other.iAlarmCount;
		iCallRefCount = other.iCallRefCount;
		iCallReleaseCount = other.iCallReleaseCount;
	}
};

class CAlarmStatisticsProcesser : public INotifyProcesser
{
	typedef std::map<std::string, DeptInfo> DEPTINFOMAP;

public:
	virtual std::string ProcNotify(const std::string& strMsg);
	virtual bool        InitProcesser();
	virtual void        UninitProcesser();

public:
	CAlarmStatisticsProcesser(JsonParser::IJsonFactoryPtr pJsonFty, DataBase::IDBConnPtr pDBConn, StringUtil::IStringUtilPtr pStrUtil, DateTime::IDateTimePtr pDateTime, 
		Log::ILogPtr pLog, Config::IConfigPtr pConfig);
	virtual ~CAlarmStatisticsProcesser();

private:
	bool        _QueryAlarmDeptsByTime(const std::string& strBeginTime, const std::string& strEndTime, DEPTINFOMAP& depts);
	int         _QueryAlarmCountByDeptCodeAndTime(const std::string& strDeptCode, const std::string& strBeginTime, const std::string& strEndTime);
	void        _QueryAlarmCountInfo(DEPTINFOMAP& depts, const std::string& strBeginTime, const std::string& strEndTime);
	
	bool        _QueryCallRefDeptsByTime(const std::string& strBeginTime, const std::string& strEndTime, DEPTINFOMAP& depts);
	int         _QueryCallRefReleaseCountByDeptCodeAndTime(const std::string& strDeptCode, const std::string& strBeginTime, const std::string& strEndTime);

	int         _QueryCallRefCountByDeptCodeAndTime(const std::string& strDeptCode, const std::string& strBeginTime, const std::string& strEndTime);
	int         _QueryCallBackRefCountByDeptCodeAndTime(const std::string& strDeptCode, const std::string& strBeginTime, const std::string& strEndTime);

	void        _QueryCallRefInfo(DEPTINFOMAP& depts, const std::string& strBeginTime, const std::string& strEndTime);

	void        _MergeResult(DEPTINFOMAP& alarmDepts, DEPTINFOMAP& callRefDepts);
		
	void        _PacketHeader(PROTOCOL::CHeader& header);
	void        _PacketBody(const DEPTINFOMAP& alarmDepts, PROTOCOL::CStatisticsAlarm::CBody& body);
	std::string _PacketResult(const DEPTINFOMAP& alarmDepts);
	

	void        _StatisticsAlarmCountInfo();	

	int         _QueryCount(DataBase::SQLRequest& sqlRequest);

private:
	JsonParser::IJsonFactoryPtr     m_pJsonFty;	
	DataBase::IDBConnPtr            m_pDBConn;
	ICC::StringUtil::IStringUtilPtr m_pStrUtil;
	DateTime::IDateTimePtr          m_pDateTime;	
	Log::ILogPtr                    m_pLog;
	Config::IConfigPtr              m_pConfig;
};

#endif