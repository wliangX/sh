#pragma once


namespace ICC
{
	class CTransportQueue;
	typedef  std::map<std::string, CTransportQueue> TransportMap;

	class CSeatInfo
	{
	public:
		CSeatInfo();
		~CSeatInfo();
	public:
		std::string	ToJson(JsonParser::IJsonPtr p_pJson);
		bool Parse(std::string p_strSeatInfo, JsonParser::IJsonPtr p_pJson);

	public:
		std::string m_strNo;
		std::string m_strName;
		std::string m_strDeptCode;
		std::string m_strDeptName;
	};

	class CUserDept
	{
	public:
		CUserDept();
		~CUserDept();

	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson);
		bool Parse(std::string p_strInfo, JsonParser::IJsonPtr p_pJson);

	public:
		std::string m_strDeptCode;
		std::string m_strDeptName;
		std::string m_strUserName;
		std::string m_strSeatNo;
		std::string m_strBuyIdle; //idle:ÏÐ£¬busy:Ã¦
	};


	class CTransportQueue
	{
	public:
	public:
		CTransportQueue();
		~CTransportQueue();

	public:
		std::string ToJson(JsonParser::IJsonFactoryPtr p_pJson);
		bool Parse(std::string p_strInfo, JsonParser::IJsonFactoryPtr p_pJson);

	public:
		std::string		strAlarmID;
		std::string     strTimerName;
		std::string		strTransportType;
		std::map<std::string, std::string>		m_mapRelateID;
		PROTOCOL::CTransPortAlarmRequest		oSourceInfo;
		PROTOCOL::CAlarmTransferCallRespond		oTransferCallInfo;
	};
}