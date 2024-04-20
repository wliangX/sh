#ifndef __INotifyProcesser_H__
#define __INotifyProcesser_H__

#include <string>

enum MsgType
{
	MsgType_Unknown = 0,
	MsgType_Alarm,
	MsgType_SeatSatausChanged,
	MsgType_CtiStateChanged,
	MsgType_AlarmStatistics,
	MsgType_SeatInfoChanged,	
	MsgType_AgentListReponse,
};

struct SeatInfo
{
	std::string strDeptCode;
	std::string strDeptName;
	std::string strNumber;
	std::string strAlias;
	std::string strStatus;
	std::string strLoginMode;
	std::string strUpdateTime;
	std::string strAcd;

	SeatInfo(){ strLoginMode = "logout"; strStatus = "busy"; }
	SeatInfo(const SeatInfo& other)
	{
		_Assign(other);
	}
	SeatInfo& operator=(const SeatInfo& other)
	{
		if (&other != this)
		{
			_Assign(other);
		}
		return *this;
	}
	void _Assign(const SeatInfo& other)
	{
		strDeptCode = other.strDeptCode;
		strDeptName = other.strDeptName;
		strNumber = other.strNumber;
		strAlias = other.strAlias;
		strStatus = other.strStatus;
		strLoginMode = other.strLoginMode;
		strUpdateTime = other.strUpdateTime;
		strAcd = other.strAcd;
	}
};

class INotifyProcesser
{
public:
	virtual std::string ProcNotify(const std::string& strMsg) = 0;
	virtual bool        InitProcesser(){ return true; };
	virtual void        UninitProcesser(){};

public:
	INotifyProcesser(){}
protected:
	virtual ~INotifyProcesser(){}
};

#endif