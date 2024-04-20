#pragma once
/**
@Copyright Copyright (C), 2018
@file		ViolationManager.h
@created	2018/01/23
@brief		违规管理类
1, 负责违规检查
2, 负责违规数据查询

@author
*/
#include <mutex>

namespace ICC
{
	typedef struct receiveObjects
	{
		std::vector<std::string> noticeList;
		std::vector<std::string> syncList;
		std::string type;
	}ReceiveObject;

	typedef struct MessageSendDTOData
	{
		std::string appCode;
		std::string businessCode;
		int compensateType;
		std::string delayDuration;
		int delayType;
		std::string message;
		std::string moduleCode;
		std::string needStorage;
		std::vector<ReceiveObject> receiveObjects;
		std::string sendType;
		std::string sync;
		std::string title;
	}MessageSendDTOData;

	typedef enum E_VIOLATIONCHECK_TYPE
	{
		VIOLATIONCHECK_BUSY = 0,			//违规置忙超时检测
		VIOLATIONCHECK_CALL_RING = 1,		//违规振铃超时检测
		VIOLATIONCHECK_CALL_TALK = 2,		//违规通话超时检测

		VIOLATIONCHECK_UNKNOWN
	}EViolationCheckType;

	class CViolationCheckData
	{
	public:
		CViolationCheckData();
		virtual ~CViolationCheckData();
		bool ViolationCheck();
		void ChangeViolationCheck(const std::string &p_strViolationId, int p_nType, int p_nTimeOutValue);

		int GetCheckType() { return m_nVolationCheckType;}
		std::string GetViolationTarget() { return m_strTarget; };
		void SetViolationTarget(const std::string& p_strTarget) { m_strTarget = p_strTarget; };

		void SetViolationUserID(const std::string& p_strUserID) { m_strUserID = p_strUserID; };
		std::string GetViolationUserID() { return m_strUserID; };

		std::string GetStartTime() { return m_strStartTime; };
		void SetStartTime(const std::string& p_strStartTime) { m_strStartTime = p_strStartTime; };

		int GetTimeOutCount() { return m_nTimeOutCount; };
	public:
		int m_nVolationCheckType;
		int m_nTick;

		int m_nTimeOutValue;
		int m_nTimeOutCount;

		std::string m_strViolationId;
		std::string m_strUserID;
		std::string m_strStartTime;
		std::string m_strTarget;
	};

	//////////////////////////////////////////////////////////////////////////
	class CViolationManager
	{
	public:
		CViolationManager(void);
		virtual ~CViolationManager(void);

		static boost::shared_ptr<CViolationManager> Instance();
		void ExitInstance();
	public:
		void OnStart();

		void SetViolationCheck(int p_nCheckType, const std::string &p_strState,
			const std::string &p_strDevice,
			const std::string &p_strUserID,
			const std::string & p_strViolationID);

		bool GetViolationList(const PROTOCOL::CGetViolationRequest & p_oGetViolationRequest, PROTOCOL::CGetViolationRespond& p_oGetViolationRespond);

		void SetLogPtr(Log::ILogPtr p_logPtr) { m_LogPtr = p_logPtr; }
		void SetDateTimePtr(DateTime::IDateTimePtr p_dateTimePtr) { m_DateTimePtr = p_dateTimePtr; }
		void SetStringUtilPtr(StringUtil::IStringUtilPtr p_stringUtilPtr) { m_StringUtilPtr = p_stringUtilPtr; }
		void SetTimerManagerPtr(Timer::ITimerManagerPtr p_pTimerManager) { m_pTimerManager = p_pTimerManager; }
		void SetRequestDBConnPtr(DataBase::IDBConnPtr p_pRequestDBConn) { m_pRequestDBConn = p_pRequestDBConn; }
		void SetRedisClientPtr(Redis::IRedisClientPtr p_pRedisClient) { m_pRedisClient = p_pRedisClient; }
		void SetMsgCenterPtr(MsgCenter::IMessageCenterPtr p_pMsgCenter) { m_pMsgCenter = p_pMsgCenter; }

		void SetHttpClientPtr(IHttpClientPtr p_pHttpClient) { m_pHttpClient = p_pHttpClient; }
		void SetConfigPtr(Config::IConfigPtr p_pConfigPtr) { m_pConfig = p_pConfigPtr; }
		void SetStringPtr(StringUtil::IStringUtilPtr p_pStringPtr) { m_pString = p_pStringPtr; }

		void SetObserverCenterPtr(ObserverPattern::IObserverCenterPtr p_observerCenterPtr) { m_pObserverCenter = p_observerCenterPtr; }
		void SetJsonFactoryPtr(JsonParser::IJsonFactoryPtr p_jsonFactoryPtr) { m_JsonFactoryPtr = p_jsonFactoryPtr; }

		void ClearViolationCheckList();
		void ViolationCheckTimerProcess();

		void SetCallRingTimeOutValue(unsigned int p_nValue) { m_nCallRingTimeOutValue = p_nValue; };
		void SetCallTalkTimeOutValue(unsigned int p_nValue) { m_nCallTalkTimeOutValue = p_nValue; };
		void SetBusyTimeOutValue(unsigned int p_nValue) { m_nBusyTimeOutValue = p_nValue; };

		unsigned int GetCallRingTimeOutValue() { return m_nCallRingTimeOutValue; };
		unsigned int GetCallTalkTimeOutValue() { return m_nCallTalkTimeOutValue; };
		unsigned int GetBusyTimeOutValue() { return m_nBusyTimeOutValue; };

		//2022.10.25新增推送消息信息(在违规这里试一下)
		bool GetMsgCenterService(std::string& p_strIp, std::string& p_strPort);
		bool GetSmpService(std::string& p_strIp, std::string& p_strPort, std::string l_serverName);
		bool MessageSendDTO(std::string l_sendMsg);
		std::string BuildSendMsg(MessageSendDTOData l_sendData);
		std::string BuildViolationMsg(int l_violationcheckType);
		std::string DeptCodeToDeptGuid(std::string l_deptCode);
		std::vector<std::string> GetGuidbByDept(std::string l_orgGuid, std::string l_roleAttr);

		void SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName);
	private:
	private:
		static boost::shared_ptr<CViolationManager> m_pManagerInstance;

		std::mutex	m_violationCheckMutex;
		std::map<std::string, boost::shared_ptr<CViolationCheckData>> m_mViolationCheckList;

		Log::ILogPtr						m_LogPtr;
		DateTime::IDateTimePtr				m_DateTimePtr;
		DateTime::CDateTime					m_oLastCheckTime;
		StringUtil::IStringUtilPtr			m_StringUtilPtr;

		Timer::ITimerManagerPtr				m_pTimerManager;

		DataBase::IDBConnPtr				m_pRequestDBConn;

		ObserverPattern::IObserverCenterPtr	m_pObserverCenter;
		JsonParser::IJsonFactoryPtr			m_JsonFactoryPtr;

		Redis::IRedisClientPtr				m_pRedisClient;

		IHttpClientPtr 						m_pHttpClient;
		Config::IConfigPtr					m_pConfig;
		StringUtil::IStringUtilPtr			m_pString;
		MsgCenter::IMessageCenterPtr		m_pMsgCenter;

		unsigned int	m_nCallRingTimeOutValue;
		unsigned int	m_nCallTalkTimeOutValue;
		unsigned int	m_nBusyTimeOutValue;

		std::string m_strNacosServerIp;
		std::string m_strNacosServerPort;
		std::string m_strNacosServerNamespace;
		std::string m_strNacosServerGroupName;
		boost::mutex     m_mutexNacosParams;
	};

}// end namespace