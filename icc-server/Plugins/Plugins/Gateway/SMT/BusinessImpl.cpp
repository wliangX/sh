#include "Boost.h"
#include "BusinessImpl.h"
#include "AlarmDefine.h"


#define MQCONNECT_INFO "MQConnectInfo"
#define TIMER_CMD_NAME "smt_timer"

CBusinessImpl* g_pBusinessImpl = nullptr;

CBusinessImpl::CBusinessImpl()
{
	g_pBusinessImpl = this;
}

CBusinessImpl::~CBusinessImpl()
{
}

void CBusinessImpl::OnInit()
{
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(GATEWAY_SMT_OBSERVER_CENTER);
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pStaticConfig = ICCGetIConfigFactory()->CreateStaticConfig();
	m_pJsonFty = ICCGetIJsonFactory();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pHelpTool = ICCGetHelpToolFactory()->CreateHelpTool();
	m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();
	m_pHttpClient = ICCGetIHttpClientFactory()->CreateHttpClient();
}

void CBusinessImpl::OnStart()
{
	std::string l_strIsUsing = m_pConfig->GetValue("ICC/Plugin/SMT/IsUsing", "0");

	if (l_strIsUsing != "1")
	{
		ICC_LOG_DEBUG(m_pLog, "there is no need to load the plugin");
		return;
	}

	m_strServiceID = m_pStaticConfig->GetValue("ICC/Component/AmqClient/ClientID", "");
	m_strUrl = m_pConfig->GetValue("ICC/Plugin/SMT/SMTUrl", "http://127.0.0.1:26410/smt-agent/systemMonitorToolAgentService/monitor");
	std::string l_strInterval = m_pConfig->GetValue("ICC/Plugin/SMT/Interval", "60");
	m_strSystemName = m_pConfig->GetValue("ICC/Plugin/SMT/SystemName", "ICC");
	m_strSystemCode = m_pConfig->GetValue("ICC/Plugin/SMT/SystemCode", "ICC");

	std::map<std::string, std::string> l_ParamsMap;
	l_ParamsMap["Content-Type"] = "application/json; charset = utf-8";
	l_ParamsMap["Accept"] = "application/json";
	l_ParamsMap["Connection"] = "close";

	m_pHttpClient->SetHeaders(l_ParamsMap);

	// 注册观察者
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "ConnectionInfo", OnCNotifiConnect);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "RemoveInfo", OnCNotifiDisConnect);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "sync_server_lock", OnNotifiDoubleServerSync);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TIMER_CMD_NAME, OnTimer);
	m_pTimerMgr->AddTimer(TIMER_CMD_NAME, m_pString->ToUInt(l_strInterval), 5);
    ICC_LOG_INFO(m_pLog, "smt plugin start success!");

}

void CBusinessImpl::OnStop()
{

}

void CBusinessImpl::OnDestroy()
{

}

void CBusinessImpl::OnTimer(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	
}

void CBusinessImpl::OnNotifiDoubleServerSync(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Server Role Sync: %s", l_strRequestMsg.c_str());

		PROTOCOL::CSyncServerLock l_oSyncObj;
		if (!l_oSyncObj.ParseString(l_strRequestMsg, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "l_strRequestMsg parse json failed");

			return;
		}

		std::string l_strMQClientInfo;
		for (auto info : m_mapMQConnectInfo)
		{
			if (m_pString->Contains(info.second, l_oSyncObj.m_oBody.m_strClientID))
			{
				l_strMQClientInfo = info.second;
				break;
			}
		}
		if (l_strMQClientInfo.empty())
		{
			ICC_LOG_ERROR(m_pLog, "mqclientinfo not find");
			return;
		}

		PROTOCOL::CConnect l_oConnect;
		if (!l_oConnect.ParseString(l_strMQClientInfo, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "mqclientinfo parse json failed");
			return;
		}

		std::string l_strContent = BuildBody2(l_oConnect.m_strClientIP, l_oSyncObj.m_oBody.m_strServiceState, l_oSyncObj.m_oBody.m_strServiceID);

		if (!l_strContent.empty())
		{
			std::string l_strRespond = m_pHttpClient->Post(m_strUrl, l_strContent);
			ICC_LOG_DEBUG(m_pLog, "send alarm: [%s][%s]", m_strUrl.c_str(), l_strContent.c_str());
			if (l_strRespond.empty())
			{
				ICC_LOG_ERROR(m_pLog, "smt http post all respond empty when ServerSync");
			}
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "connect event no need to send: [%s]", l_oConnect.m_strClientID.c_str());
		}
	}
}

void CBusinessImpl::OnCNotifiConnect(ObserverPattern::INotificationPtr p_pNotifiConnect)
{
	std::string l_strMessage = p_pNotifiConnect->GetMessages();
	PROTOCOL::CConnect l_oConnect;
	if (!l_oConnect.ParseString(l_strMessage, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Monitor Error ParseString Error:[%s]", l_strMessage.c_str());
		return;
	}

	//这里只关注服务的连接状态
	bool bMatch = m_pString->RegexMatch(l_oConnect.m_strClientID, "ICC\\.(Server|Gateway)\\.(Alarm|CTI){0,1}(\\.){0,1}([a-zA-Z0-9-])+\\.Producer");
	if (bMatch)
	{
		m_mapMQConnectInfo[l_oConnect.m_strConnectID] = l_oConnect.ToString(m_pJsonFty->CreateJson());

		ICC_LOG_DEBUG(m_pLog, "connect [%s][%s]", l_oConnect.m_strClientID.c_str(), l_oConnect.m_strConnectID.c_str());

		std::string l_strContent = BuildBody(l_oConnect, "connect");

		if (!l_strContent.empty())
		{
			std::string l_strRespond = m_pHttpClient->Post(m_strUrl, l_strContent);
			ICC_LOG_DEBUG(m_pLog, "send alarm: [%s][%s]", m_strUrl.c_str(), l_strContent.c_str());
			if (l_strRespond.empty())
			{
				ICC_LOG_ERROR(m_pLog, "smt http post all respond empty when connect");
			}
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "connect event no need to send: [%s]", l_oConnect.m_strClientID.c_str());
		}
	}
}

void CBusinessImpl::OnCNotifiDisConnect(ObserverPattern::INotificationPtr p_pNotifiDisConnect)
{
	std::string l_strMessage = p_pNotifiDisConnect->GetMessages();
	PROTOCOL::CDisConnect l_oDisConnect;
	if (!l_oDisConnect.ParseString(l_strMessage, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "monitor error parsestring error:[%s]", l_strMessage.c_str());
		return;
	}

	//根据connectId查找clientId	
	auto l_iter = m_mapMQConnectInfo.find(l_oDisConnect.m_strConnectID);
	if (l_iter == m_mapMQConnectInfo.end())
	{
		ICC_LOG_DEBUG(m_pLog, "disconnect id[%s], can not find mq client info:", l_oDisConnect.m_strConnectID.c_str());
		return;
	}

	std::string l_strMQClientInfo = l_iter->second;
	m_mapMQConnectInfo.erase(l_iter);
	ICC_LOG_DEBUG(m_pLog, "disconnect [%s], get mq client info: %s", l_oDisConnect.m_strConnectID.c_str(), l_strMQClientInfo.c_str());

	PROTOCOL::CConnect l_oConnect;
	if (!l_oConnect.ParseString(l_strMQClientInfo, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "mqclientinfo parse json failed");
		return;
	}

	//这里只关注服务的连接状态
	bool bMatch = m_pString->RegexMatch(l_oConnect.m_strClientID, "ICC\\.(Server|Gateway)\\.([a-zA-Z0-9])+\\.([a-zA-Z0-9-])+\\.Producer");
	if (bMatch)
	{
		ICC_LOG_DEBUG(m_pLog, "disconnect, [%s][%s]", l_oConnect.m_strClientID.c_str(), l_oConnect.m_strConnectID.c_str());

		std::string l_strContent = BuildBody(l_oConnect, "disconnect");
		if (!l_strContent.empty())
		{
			std::string l_strRespond = m_pHttpClient->Post(m_strUrl, l_strContent);
			ICC_LOG_DEBUG(m_pLog, "send alarm: [%s][%s]", m_strUrl.c_str(), l_strContent.c_str());
			if (l_strRespond.empty())
			{
				ICC_LOG_ERROR(m_pLog, "smt http post all respond empty when disconnect");
			}
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "disconnect event no need to send: [%s]", l_oConnect.m_strClientID.c_str());
		}
	}
}

std::string CBusinessImpl::BuildBody(const PROTOCOL::CConnect& p_connect, std::string p_strOpt)
{
	JsonParser::IJsonPtr l_JsonPtr = m_pJsonFty->CreateJson();
	if (!l_JsonPtr)
	{
		return "";
	}

	std::string l_strIp, l_strPort;
	if (!ParseUrl(p_connect.m_strClientIP, l_strIp, l_strPort))
	{
		return "";
	}
	
	//指挥中心信息
	l_JsonPtr->SetNodeValue("/meta/command", "alarmMonitor");
	l_JsonPtr->SetNodeValue("/meta/controlCenter/code", m_strSystemCode);
	l_JsonPtr->SetNodeValue("/meta/controlCenter/name", m_strSystemName);
	//产品信息
	l_JsonPtr->SetNodeValue("/meta/production/name", "02");
	l_JsonPtr->SetNodeValue("/meta/production/ip", l_strIp);
	

	//告警监控列表-告警网元信息	
	l_JsonPtr->SetNodeValue("/interfaces/alarmMonitor/0/networkElement/ip", l_strIp);
	l_JsonPtr->SetNodeValue("/interfaces/alarmMonitor/0/networkElement/networkElementType", "02_00");

	std::string l_stralarmCode; //告警编码
	std::string l_stralarmLevel;//告警级别
	std::string l_stralarmType; //告警类型
	std::string l_stralarmCause; //告警原因	
	std::string p_strAlarmGUID;  //告警guid

	std::string l_stralarmSolution; //告警解决办法  不需要填
	std::string l_stralarmName;     //告警名称      不需要填

	if (m_pString->StartsWith(p_connect.m_strClientID, g_smt_ServerName[icc_server_alarm]))
	{
		l_JsonPtr->SetNodeValue("/meta/production/port", g_smt_ServerPort[icc_server_alarm]);
		l_JsonPtr->SetNodeValue("/interfaces/alarmMonitor/0/networkElement/port", g_smt_ServerPort[icc_server_alarm]);

		if (p_strOpt == "connect")
		{
			//Alarm连接
			l_stralarmCode = g_smt_AlarmCode[icc_business_connection_alarm];
			l_stralarmLevel = g_smt_AlarmLevel[warning];			

			//查找AlarmGuid
			auto l_iter = m_mapAlarmGuid.find(g_smt_ServerName[icc_server_alarm]);
			if (l_iter != m_mapAlarmGuid.end())
			{
				p_strAlarmGUID = l_iter->second;
				m_mapAlarmGuid.erase(l_iter);
			}
		}
		else
		{
			//Alarm断开
			l_stralarmCode = g_smt_AlarmCode[icc_business_disconnection_alarm];
			l_stralarmLevel = g_smt_AlarmLevel[critical];			
			p_strAlarmGUID = m_pString->CreateGuid();
			m_mapAlarmGuid[g_smt_ServerName[icc_server_alarm]] = p_strAlarmGUID;
		}
		
		l_stralarmCause = std::string(g_smt_AlarmCause[icc_server_alarm]) + "//" + l_strIp + ":" + g_smt_ServerPort[icc_server_alarm];
		l_stralarmType = g_smt_AlarmType[business_alarm];
	}
	else if (m_pString->StartsWith(p_connect.m_strClientID, g_smt_ServerName[icc_server_cti]))
	{	
		l_JsonPtr->SetNodeValue("/interfaces/alarmMonitor/0/networkElement/networkElementType", "02_04");
		l_JsonPtr->SetNodeValue("/meta/production/port", g_smt_ServerPort[icc_server_cti]);
		l_JsonPtr->SetNodeValue("/interfaces/alarmMonitor/0/networkElement/port", g_smt_ServerPort[icc_server_cti]);

		if (p_strOpt == "connect")
		{
			//CTI连接
			l_stralarmCode = g_smt_AlarmCode[icc_cti_connection_alarm];
			l_stralarmLevel = g_smt_AlarmLevel[warning];			

			//查找AlarmGuid
			auto l_iter = m_mapAlarmGuid.find(g_smt_ServerName[icc_server_cti]);
			if (l_iter != m_mapAlarmGuid.end())
			{
				p_strAlarmGUID = l_iter->second;
				m_mapAlarmGuid.erase(l_iter);
			}			
		}
		else
		{
			//CTI断开
			l_stralarmCode = g_smt_AlarmCode[icc_cti_disconnection_alarm];
			l_stralarmLevel = g_smt_AlarmLevel[critical];
			p_strAlarmGUID = m_pString->CreateGuid();
			m_mapAlarmGuid[g_smt_ServerName[icc_server_cti]] = p_strAlarmGUID;
		}

		l_stralarmCause = std::string(g_smt_AlarmCause[icc_server_cti]) + "//" + l_strIp + ":" + g_smt_ServerPort[icc_server_cti];
		l_stralarmType = g_smt_AlarmType[business_alarm];
	}
	else
	{
		//不关心的连接状态数据
		return "";
	}

	if (p_strAlarmGUID.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "[%s] alarm guid is empty, no need to send", p_connect.m_strClientID.c_str());
		return "";
	}

	//告警监控列表-告警信息
	l_JsonPtr->SetNodeValue("/interfaces/alarmMonitor/0/alarm/alarmUUID", p_strAlarmGUID);// 告警UUID
	l_JsonPtr->SetNodeValue("/interfaces/alarmMonitor/0/alarm/alarmCode", l_stralarmCode);// 告警编码
	l_JsonPtr->SetNodeValue("/interfaces/alarmMonitor/0/alarm/alarmName", l_stralarmName);// 告警名称
	l_JsonPtr->SetNodeValue("/interfaces/alarmMonitor/0/alarm/alarmLevel", l_stralarmLevel);// 告警级别
	l_JsonPtr->SetNodeValue("/interfaces/alarmMonitor/0/alarm/alarmType", l_stralarmType);// 告警类型
	l_JsonPtr->SetNodeValue("/interfaces/alarmMonitor/0/alarm/alarmCause", l_stralarmCause);// 告警发生原因
	l_JsonPtr->SetNodeValue("/interfaces/alarmMonitor/0/alarm/alarmSolution", l_stralarmSolution);// 告警解决方法
	l_JsonPtr->SetNodeValue("/interfaces/alarmMonitor/0/alarm/firstOccurrenceTime", m_pDateTime->CurrentDateTimeStr());// 告警首次发生时间

	return l_JsonPtr->ToString();
}

std::string CBusinessImpl::BuildBody2(std::string p_strClientIP, std::string p_strSvcState, std::string p_strSvcID)
{
	JsonParser::IJsonPtr l_JsonPtr = m_pJsonFty->CreateJson();
	if (!l_JsonPtr)
	{
		return "";
	}

	std::string l_strIp, l_strPort;
	if (!ParseUrl(p_strClientIP, l_strIp, l_strPort))
	{
		return "";
	}

	//指挥中心信息
	l_JsonPtr->SetNodeValue("/meta/command", "alarmMonitor");
	l_JsonPtr->SetNodeValue("/meta/controlCenter/code", m_strSystemCode);
	l_JsonPtr->SetNodeValue("/meta/controlCenter/name", m_strSystemName);
	//产品信息
	l_JsonPtr->SetNodeValue("/meta/production/name", "02");
	l_JsonPtr->SetNodeValue("/meta/production/ip", l_strIp);


	//告警监控列表-告警网元信息	
	std::string l_strNetInfo;
	if (m_pString->StartsWith(p_strSvcID, g_smt_ServerName[icc_server_alarm]))
	{
		l_strNetInfo = "02_00";
	}
	else if (m_pString->StartsWith(p_strSvcID, g_smt_ServerName[icc_server_cti]))
	{
		l_strNetInfo = "02_04";
	}
	else
	{
		l_strNetInfo = "02_01";
	}

	l_JsonPtr->SetNodeValue("/interfaces/alarmMonitor/0/networkElement/ip", l_strIp);
	l_JsonPtr->SetNodeValue("/interfaces/alarmMonitor/0/networkElement/networkElementType", l_strNetInfo);

	std::string l_stralarmCode; //告警编码
	std::string l_stralarmLevel;//告警级别
	std::string l_stralarmType; //告警类型
	std::string l_stralarmCause; //告警原因	
	std::string p_strAlarmGUID;  //告警guid

	std::string l_stralarmSolution; //告警解决办法  不需要填
	std::string l_stralarmName;     //告警名称      不需要填

	l_JsonPtr->SetNodeValue("/meta/production/port", l_strPort);
	l_JsonPtr->SetNodeValue("/interfaces/alarmMonitor/0/networkElement/port", l_strPort);

	if (p_strSvcState == "master")
	{
		l_stralarmCode = g_smt_AlarmCode[icc_hostmachine_switch_alarm];
		l_stralarmLevel = g_smt_AlarmLevel[major];

		p_strAlarmGUID = m_pString->CreateGuid();
	}
	else
	{
	}
	
	l_stralarmCause = p_strClientIP + "[" + p_strSvcID + "]";
	l_stralarmType = g_smt_AlarmType[business_alarm];
	
	if (p_strAlarmGUID.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "[%s] slave, no need to send", l_stralarmCause.c_str());
		return "";
	}

	//告警监控列表-告警信息
	l_JsonPtr->SetNodeValue("/interfaces/alarmMonitor/0/alarm/alarmUUID", p_strAlarmGUID);// 告警UUID
	l_JsonPtr->SetNodeValue("/interfaces/alarmMonitor/0/alarm/alarmCode", l_stralarmCode);// 告警编码
	l_JsonPtr->SetNodeValue("/interfaces/alarmMonitor/0/alarm/alarmName", l_stralarmName);// 告警名称
	l_JsonPtr->SetNodeValue("/interfaces/alarmMonitor/0/alarm/alarmLevel", l_stralarmLevel);// 告警级别
	l_JsonPtr->SetNodeValue("/interfaces/alarmMonitor/0/alarm/alarmType", l_stralarmType);// 告警类型
	l_JsonPtr->SetNodeValue("/interfaces/alarmMonitor/0/alarm/alarmCause", l_stralarmCause);// 告警发生原因
	l_JsonPtr->SetNodeValue("/interfaces/alarmMonitor/0/alarm/alarmSolution", l_stralarmSolution);// 告警解决方法
	l_JsonPtr->SetNodeValue("/interfaces/alarmMonitor/0/alarm/firstOccurrenceTime", m_pDateTime->CurrentDateTimeStr());// 告警首次发生时间

	return l_JsonPtr->ToString();
}

bool CBusinessImpl::ParseUrl(std::string p_strUrl, std::string& p_strIP, std::string& p_strPort)
{
	std::string l_strHttpTag = "tcp://";
	std::string l_strTemp = p_strUrl;
	// 截断协议头
	if (l_strTemp.find(l_strHttpTag) == 0)
	{
		l_strTemp = l_strTemp.substr(l_strHttpTag.length());
	}
	else
	{
		return false;
	}

	// 解析域名
	std::size_t l_idex = l_strTemp.find('/');
	// 如果有路径，只取ip:port部分
	if (std::string::npos == l_idex)
	{
		l_idex = l_strTemp.size();
	}

	// 解析ip
	p_strIP = l_strTemp.substr(0, l_idex);

	// 解析port
	l_idex = p_strIP.find(':');
	if (std::string::npos == l_idex)
	{
		return false;
	}

	p_strPort = p_strIP.substr(l_idex + 1);
	p_strIP = p_strIP.substr(0, l_idex);

	return true;
}