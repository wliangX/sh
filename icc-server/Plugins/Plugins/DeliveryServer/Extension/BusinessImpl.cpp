#include "Boost.h"
#include "BusinessImpl.h"
#include <thread>

#define MODULE_NAME		"Extension"
#define TALK_STATE		"talkstate"
#define PARAM_INFO		"ParamInfo"
#define ADD_ALARM		"add_alarm"
#define ERROR			"500"
#define SUCCESS			"200"
#define SUCCESS_MSG		"success"
#define FPS				"0"
#define FMS				"1"
const unsigned long THREADID_RECEIVENOTIFY = 10000;

CBusinessImpl::CBusinessImpl()
{
}

CBusinessImpl::~CBusinessImpl()
{
}
void CBusinessImpl::OnInit()
{
	printf("OnInit enter! plugin = %s\n", MODULE_NAME);

	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(OBSERVER_CENTER_DELIVBERYSERVER_EXTENSION);
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pIDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pJsonFac = ICCGetIJsonFactory();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_pMsgCenter = ICCGetIMessageCenterFactory()->CreateMessageCenter();
	m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();
	m_pHttpClient = ICCGetIHttpClientFactory()->CreateHttpClient();
	_CreateThreads();

	_InitProcNotifys();

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::_InitProcNotifys()
{
	m_mapFuncs.insert(std::make_pair("syn_nacos_params", &CBusinessImpl::OnReceiveSynNacosParams));
	m_mapFuncs.insert(std::make_pair("device_state_sync", &CBusinessImpl::OnReceiveMandatoryList));
	m_mapFuncs.insert(std::make_pair("listM3u8PlayUrl", &CBusinessImpl::OnReceivePlayRecord));
	m_mapFuncs.insert(std::make_pair("listAudioUrl", &CBusinessImpl::OnReceiveDownloadRecord));
}

void CBusinessImpl::OnStart()
{
	m_strIsDetectionACD = m_pConfig->GetValue("ICC/Plugin/Extension/IsDetectionAcd","1");
	m_strIsUsing = m_pConfig->GetValue("ICC/Plugin/Extension/IsUsing", "1");
	if (m_strIsUsing != "1")
	{
		ICC_LOG_DEBUG(m_pLog, "Extension plugin no need to load!!!");
		return;
	}
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);

	m_strDetectionTime = m_pConfig->GetValue("ICC/Plugin/Extension/DetectionTime", "3");
	m_strNacosQueryUrl = m_pConfig->GetValue("ICC/Component/HttpServer/queryurl", "/nacos/v1/ns/instance/list");
	m_strServiceHealthyFlag = m_pConfig->GetValue("ICC/Plugin/Extension/ServiceHealthyFlag", "1");
	m_strSendOverTime= m_pConfig->GetValue("ICC/Plugin/Extension/SendOverTime", "20");
	m_strFpsServiceName = m_pConfig->GetValue("ICC/Plugin/Extension/FpsServiceName", "commandcenter-fps-service");
	m_strFpsPalyTarget = m_pConfig->GetValue("ICC/Plugin/Extension/FpsPlayTarget", "/file/v2/listM3u8PlayUrl");
	m_strFpsDownloadTarget = m_pConfig->GetValue("ICC/Plugin/Extension/FpsDownloadTarget", "/file/v2/listAudioUrl");
	m_strFmsServiceName = m_pConfig->GetValue("ICC/Plugin/Extension/FmsServiceName", "commandcenter-file-service");
	m_strFmsTarget= m_pConfig->GetValue("ICC/Plugin/Extension/FmsTarget", "/file/v2/getFileUrl?fileId=");
	m_pMsgCenter->Start();

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "device_state_sync", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "syn_nacos_params", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "listM3u8PlayUrl", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "listAudioUrl", _OnReceiveNotify);

	ICC_LOG_INFO(m_pLog, "plugin DeliveryServer.Extensionn start success");

	printf("OnStart complete! plugin = %s\n", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_INFO(m_pLog, "Extension stop success");
}

void CBusinessImpl::OnDestroy()
{
}


void CBusinessImpl::OnReceiveMandatoryList(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Receive device state:[%s]", l_strRequestMsg.c_str());
	PROTOCOL::CDeviceStateEvent l_DeviceStateEvent;
	JsonParser::IJsonPtr l_pJson = m_pJsonFac->CreateJson();
	if (!l_DeviceStateEvent.ParseString(l_strRequestMsg, l_pJson))
	{
		ICC_LOG_DEBUG(m_pLog, "Parse Device state Message Failed!!!");
		return;
	}

	PROTOCOL::CDeviceInfo l_DeviceInfo;
	ExchangeData(l_DeviceInfo, l_DeviceStateEvent);
	std::string strTimerId = CreateTimerId();
	if ((l_DeviceStateEvent.m_oBody.m_strState == TALK_STATE))
	{
		if((!l_DeviceStateEvent.m_oBody.m_strACD.empty())||(l_DeviceStateEvent.m_oBody.m_strACD.empty()&& m_strIsDetectionACD == "0"))
		{
			std::lock_guard<std::mutex> guard(m_DeviceMutex);
			m_DeviceDataList[strTimerId] = l_DeviceInfo;
		}

		m_pTimer.AddTimer(this, strTimerId, 0, m_pString->ToUInt(m_strDetectionTime) * 1000);
	}
	ICC_LOG_DEBUG(m_pLog, "MandatoryList Finished!!!");
}

bool CBusinessImpl::IsExitsAlarmId(std::string &strSourceId,std::string &strAlarmId)
{
	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "select_icc_t_jjdb";
	l_SQLRequest.param["source_id"] = strSourceId;

	std::string strTime = m_pDateTime->GetCallRefIdTime(strSourceId);
	if (strTime != "")
	{
		l_SQLRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 2 * 86400);
		l_SQLRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 2 * 86400);
	}

	DataBase::IResultSetPtr l_pResult = m_pIDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "Is Exits AlarmId  failed:[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	if (l_pResult->Next())
	{
		strAlarmId = l_pResult->GetValue("id");
	}
	return true;
}

std::string CBusinessImpl::CreateTimerId()
{
	std::string l_strCurTime = m_pDateTime->CurrentDateTimeStr();

	if (m_timerIdCount >= 10000)
	{
		std::lock_guard<std::mutex> guard(m_timerIdCountMutex);
		m_timerIdCount = 1000;
	}

	for (std::string::iterator l_curIndex = l_strCurTime.begin(); l_curIndex != l_strCurTime.end();)
	{
		if (*l_curIndex < '0' || *l_curIndex > '9')
		{
			l_curIndex = l_strCurTime.erase(l_curIndex);
		}
		else
		{
			l_curIndex++;
		}
	}

	return l_strCurTime + std::to_string(m_timerIdCount++);
}

void CBusinessImpl::OnTimer(std::string uMsgId)
{
	std::string l_curTimerId = uMsgId;
	std::string strAlarmId;
	PROTOCOL::CDeviceInfo l_DeviceInfo;
	{
		std::lock_guard<std::mutex> guard(m_DeviceMutex);
		l_DeviceInfo = m_DeviceDataList[l_curTimerId];
	}

	IsExitsAlarmId(l_DeviceInfo.m_strCallRefId, strAlarmId);
	std::string m_strDeviceInfo = l_DeviceInfo.ToJson(m_pJsonFac->CreateJson());

	if (strAlarmId.empty())
	{

		ICC_LOG_DEBUG(m_pLog, "NacosIP: [%s] , NacosPort[%s], NacosServerNamespace : [%s] , NacosServerGroupName : [%s] !!!", m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str(), m_strNacosServerNamespace.c_str(), m_strNacosServerGroupName.c_str());
		if (!m_pMsgCenter->Send(ADD_ALARM, false, true, m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName, m_strDeviceInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "Send umc Message Failed,NacosIP:[%s],NacosPort[%s],NacosServerNamespace:[%s],NacosServerGroupName:[%s]!!!", m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str(), m_strNacosServerNamespace.c_str(), m_strNacosServerGroupName.c_str());
			m_pTimer.RemoveTimer(l_curTimerId);
			return;
		}
	}

	{
		std::lock_guard<std::mutex> guard(m_DeviceMutex);
		m_DeviceDataList.erase(l_curTimerId);
	}
	m_pTimer.RemoveTimer(l_curTimerId);
	return;
}

void CBusinessImpl::ExchangeData(PROTOCOL::CDeviceInfo &l_DeviceInfo, PROTOCOL::CDeviceStateEvent &l_DeviceStateEvent)
{
	l_DeviceInfo.m_strACD = l_DeviceStateEvent.m_oBody.m_strACD;
	l_DeviceInfo.m_strCallDirection = l_DeviceStateEvent.m_oBody.m_strCallDirection;
	l_DeviceInfo.m_strCalledId = l_DeviceStateEvent.m_oBody.m_strCalledId;
	l_DeviceInfo.m_strCallerId = l_DeviceStateEvent.m_oBody.m_strCallerId;
	l_DeviceInfo.m_strCallRefId = l_DeviceStateEvent.m_oBody.m_strCallRefId;
	l_DeviceInfo.m_strCSTACallRefId = l_DeviceStateEvent.m_oBody.m_strCSTACallRefId;
	l_DeviceInfo.m_strDeptCode = l_DeviceStateEvent.m_oBody.m_strDeptCode;
	l_DeviceInfo.m_strDeptName= l_DeviceStateEvent.m_oBody.m_strDeptName;
	l_DeviceInfo.m_strDevice= l_DeviceStateEvent.m_oBody.m_strDevice;
	l_DeviceInfo.m_strDeviceType= l_DeviceStateEvent.m_oBody.m_strDeviceType;
	l_DeviceInfo.m_strOriginalCalledId = l_DeviceStateEvent.m_oBody.m_strOriginalCalledId;
	l_DeviceInfo.m_strOriginalCallerId = l_DeviceStateEvent.m_oBody.m_strOriginalCallerId;
	l_DeviceInfo.m_strState= l_DeviceStateEvent.m_oBody.m_strState;
	l_DeviceInfo.m_strTime = l_DeviceStateEvent.m_oBody.m_strTime;
}

void CBusinessImpl::OnReceiveSynNacosParams(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog , "Server nacos params Sync: %s", l_strRequestMsg.c_str());

	PROTOCOL::CSyncNacosParams syn;
	if (!syn.ParseString(l_strRequestMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "l_strRequestMsg parse json failed");
		return;
	}

	SetNacosParams(syn.m_oBody.m_strNacosServerIp, syn.m_oBody.m_strNacosServerPort,
		syn.m_oBody.m_strNacosNamespace, syn.m_oBody.m_strNacosGroupName);
}

void CBusinessImpl::SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName)
{
	boost::lock_guard<boost::mutex> lock(m_mutexNacosParams);
	m_strNacosServerIp = strNacosIp;
	m_strNacosServerPort = strNacosPort;
	m_strNacosServerNamespace = strNameSpace;
	m_strNacosServerGroupName = strGroupName;
}

void CBusinessImpl::OnReceivePlayRecord(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	//录音播放都是单条话务进行播放
	std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive play record msg: %s", l_strRequestMsg.c_str());

	PROTOCOL::CReceivePlayRecordRequest l_PlayRecordRequest;
	if (!l_PlayRecordRequest.ParseString(l_strRequestMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse json failed:[%s]", l_strRequestMsg.c_str());
		return;
	}
	std::string strRecordType;
	PROTOCOL::CReceiveDownloadRecordRespond l_tmpPlayRecordRespond;
	PROTOCOL::CReceiveDownloadRecordRespond l_oPlayRecordRespond;
	//录音播放都是单条播放，可以修改为只取第0条数据
	for (int i = 0; i < l_PlayRecordRequest.m_oBody.m_vecTrafficId.size(); i++)
	{
		if(!_GetCallMsgFromDB(l_PlayRecordRequest.m_oBody.m_vecTrafficId[i], l_tmpPlayRecordRespond, strRecordType))
		{
			ICC_LOG_ERROR(m_pLog, "get call msg from database failed,callref_id:[%s]", l_PlayRecordRequest.m_oBody.m_vecTrafficId[i].c_str());
			_CommonRecordRespond(p_pNotifiRequest, l_oPlayRecordRespond, ERROR, "get call msg from database failed", l_PlayRecordRequest.m_oHeader.m_strMsgId);
			return;
		}
	}

	std::map<std::string, std::string> mapHeaders;
	std::string strErrorMessage;
	std::string strReceive;
	mapHeaders.insert(std::make_pair("Content-Type", "application/json"));
	if (strRecordType == FPS)
	{
		std::string strFpsIp;
		std::string strFpsPort;
		if (!GetFileService(strFpsIp, strFpsPort, m_strFpsServiceName))
		{
			ICC_LOG_ERROR(m_pLog, "get fps service ip and port failed");
			_CommonRecordRespond(p_pNotifiRequest, l_oPlayRecordRespond, ERROR, "get fps service ip and port failed", l_PlayRecordRequest.m_oHeader.m_strMsgId);
			return;
		}
		ICC_LOG_DEBUG(m_pLog, "post msg:[%s]", l_strRequestMsg.c_str());
		strReceive = m_pHttpClient->PostWithTimeout(strFpsIp, strFpsPort, m_strFpsPalyTarget, mapHeaders,
			l_strRequestMsg, strErrorMessage, m_pString->ToInt(m_strSendOverTime));
		JsonParser::IJsonPtr pJson = ICCGetIJsonFactory()->CreateJson();
		if (strReceive.empty())
		{
			ICC_LOG_ERROR(m_pLog, "post msg to fps error,errormsg:[%s]", strErrorMessage.c_str());
			_CommonRecordRespond(p_pNotifiRequest, l_oPlayRecordRespond, ERROR, strErrorMessage, l_PlayRecordRequest.m_oHeader.m_strMsgId);
			return;
		}
		else if (!pJson->LoadJson(strReceive))
		{
			ICC_LOG_ERROR(m_pLog, "parser fps failed message:[%s]", strReceive.c_str());
			_CommonRecordRespond(p_pNotifiRequest, l_oPlayRecordRespond, ERROR, strErrorMessage, l_PlayRecordRequest.m_oHeader.m_strMsgId);
			return;
		}
		else
		{
			p_pNotifiRequest->Response(strReceive);
			ICC_LOG_DEBUG(m_pLog, "response:[%s]", strReceive.c_str());
		}
	}
	else
	{
		std::string strFmsIp;
		std::string strFmsPort;
		if (!GetFileService(strFmsIp, strFmsPort, m_strFmsServiceName))
		{
			ICC_LOG_ERROR(m_pLog, "get fms service ip and port failed");
			_CommonRecordRespond(p_pNotifiRequest, l_oPlayRecordRespond, ERROR, "get fps service ip and port failed", l_PlayRecordRequest.m_oHeader.m_strMsgId);
			return;
		}
		//录音播放都是单条播放，可以修改为只取第0条数据
		PROTOCOL::CRecordIDToDBRequest l_oRecordIDToDBRequest;
		for (int i = 0; i < l_tmpPlayRecordRespond.m_oBody.m_vecData.size(); i++)
		{
			if (!l_tmpPlayRecordRespond.m_oBody.m_vecData[i].m_strRecordId.empty())
			{
				if (!l_oRecordIDToDBRequest.ParseString(l_tmpPlayRecordRespond.m_oBody.m_vecData[i].m_strRecordId, m_pJsonFac->CreateJson()))
				{
					ICC_LOG_DEBUG(m_pLog, "parse record id failed,record_id:[%s]", l_tmpPlayRecordRespond.m_oBody.m_vecData[i].m_strRecordId.c_str());
					continue;
				}
			}
			l_oRecordIDToDBRequest.m_oBody.m_vecData[i].m_strCalled = l_tmpPlayRecordRespond.m_oBody.m_vecData[i].m_strCalled;
			l_oRecordIDToDBRequest.m_oBody.m_vecData[i].m_strCaller = l_tmpPlayRecordRespond.m_oBody.m_vecData[i].m_strCaller;
			l_oRecordIDToDBRequest.m_oBody.m_vecData[i].m_strCallRefId = l_tmpPlayRecordRespond.m_oBody.m_vecData[i].m_strTrafficId;
		}
		
		for (int i = 0; i < l_oRecordIDToDBRequest.m_oBody.m_vecData.size(); i++)
		{
			PROTOCOL::PostFmsRecord l_PostFmsRecord;
			l_PostFmsRecord.m_oBody.m_strFileId = l_oRecordIDToDBRequest.m_oBody.m_vecData[i].m_strFileID;
			std::string strPostMsg = l_PostFmsRecord.ToString(m_pJsonFac->CreateJson());
			std::string strFmsTarget = m_strFmsTarget;
			strFmsTarget = strFmsTarget.append(l_PostFmsRecord.m_oBody.m_strFileId);
			std::string strLocalIp;
			strReceive = m_pHttpClient->GetExWithTimeout(strFmsIp, strFmsPort, strFmsTarget, mapHeaders,
				strPostMsg, strErrorMessage, strLocalIp, m_pString->ToInt(m_strSendOverTime));
			ICC_LOG_DEBUG(m_pLog, "receive msg from fms:[%s]", strReceive.c_str());
			PROTOCOL::CGetFmsRecordFile l_oGetFmsRecordFile;
			if (!l_oGetFmsRecordFile.ParseString(strReceive, m_pJsonFac->CreateJson()))
			{
				ICC_LOG_DEBUG(m_pLog,"parse fms record result failed,msg:[%s]", strReceive.c_str());
			}
			PROTOCOL::CReceiveDownloadRecordRespond::CBody::CData l_TempPlayRecordData;
			l_TempPlayRecordData.m_strHttpsUrl = l_oGetFmsRecordFile.m_oBody.l_oCData.m_strHttpsUrl;
			l_TempPlayRecordData.m_strHttpUrl = l_oGetFmsRecordFile.m_oBody.l_oCData.m_strHttpUrl;
			l_TempPlayRecordData.m_strCalled = l_oRecordIDToDBRequest.m_oBody.m_vecData[i].m_strCalled;
			l_TempPlayRecordData.m_strCaller = l_oRecordIDToDBRequest.m_oBody.m_vecData[i].m_strCaller;
			l_TempPlayRecordData.m_strTrafficId = l_oRecordIDToDBRequest.m_oBody.m_vecData[i].m_strCallRefId;
			l_oPlayRecordRespond.m_oBody.m_vecData.push_back(l_TempPlayRecordData);
		}
		l_oPlayRecordRespond.m_oBody.m_strCode = SUCCESS;
		l_oPlayRecordRespond.m_oBody.m_strMessage = SUCCESS_MSG;
		l_oPlayRecordRespond.m_oBody.m_strMsgId = l_PlayRecordRequest.m_oHeader.m_strMsgId;
		std::string strSendMsg = l_oPlayRecordRespond.ToString(m_pJsonFac->CreateJson());
		p_pNotifiRequest->Response(strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "response:[%s]", strSendMsg.c_str());
	}
}

void CBusinessImpl::OnReceiveDownloadRecord(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	//录音下载可能存在录音批量下载
	std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "receive play record msg: %s", l_strRequestMsg.c_str());

	PROTOCOL::CReceiveDownloadRecordRequest l_oDownloadRecordRequest;
	if (!l_oDownloadRecordRequest.ParseString(l_strRequestMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse json failed:[%s]", l_strRequestMsg.c_str());
		return;
	}
	std::string strRecordType;
	bool bIsSame = true;
	bool bJdugeFail = false;
	PROTOCOL::CReceiveDownloadRecordRespond l_oDownloadRecordRespond;
	PROTOCOL::CReceiveDownloadRecordRespond l_oTempDownloadRecordRespond;
	if (!_JudgeIsSameRecordType(l_oDownloadRecordRequest.m_oBody.m_vecTrafficId, l_oTempDownloadRecordRespond, strRecordType, bIsSame))
	{
		ICC_LOG_DEBUG(m_pLog, "jduge is same record type failed");
	}
	std::map<std::string, std::string> mapHeaders;
	std::string strErrorMessage;
	std::string strReceive;
	mapHeaders.insert(std::make_pair("Content-Type", "application/json"));
	//批量下载的录音都是存储在一个文件服务器上
	if (bIsSame == true)
	{
		//查询的录音全部是FPS上的录音
		if (strRecordType == FPS)
		{
			std::string strFpsIp;
			std::string strFpsPort;
			if (!GetFileService(strFpsIp, strFpsPort, m_strFpsServiceName))
			{
				ICC_LOG_ERROR(m_pLog, "get fps service ip and port failed");
				_CommonRecordRespond(p_pNotifiRequest, l_oDownloadRecordRespond, ERROR, "get fps service ip and port failed", l_oDownloadRecordRequest.m_oHeader.m_strMsgId);
				return;
			}
			ICC_LOG_DEBUG(m_pLog, "post msg:[%s]", l_strRequestMsg.c_str());
			strReceive = m_pHttpClient->PostWithTimeout(strFpsIp, strFpsPort,m_strFpsDownloadTarget, mapHeaders,
				l_strRequestMsg, strErrorMessage, m_pString->ToInt(m_strSendOverTime));
			ICC_LOG_DEBUG(m_pLog, "receive msg from fps:[%s]", strReceive.c_str());
			JsonParser::IJsonPtr pJson = ICCGetIJsonFactory()->CreateJson();
			if (strReceive.empty())
			{
				ICC_LOG_ERROR(m_pLog, "post msg to fps error,errormsg:[%s]", strErrorMessage.c_str());
				_CommonRecordRespond(p_pNotifiRequest, l_oDownloadRecordRespond, ERROR, strErrorMessage, l_oDownloadRecordRequest.m_oHeader.m_strMsgId);
				return;
			}
			else if (!pJson->LoadJson(strReceive))
			{
				ICC_LOG_ERROR(m_pLog, "parse fps failed message:[%s]", strReceive.c_str());
				_CommonRecordRespond(p_pNotifiRequest, l_oDownloadRecordRespond, ERROR, strErrorMessage, l_oDownloadRecordRequest.m_oHeader.m_strMsgId);
				return;
			}
			else
			{
				p_pNotifiRequest->Response(strReceive);
				ICC_LOG_DEBUG(m_pLog, "response:[%s]", strReceive.c_str());
			}
		}
		//查询的录音都是Fms上的录音
		else
		{
			std::string strFmsIp;
			std::string strFmsPort;
			if (!GetFileService(strFmsIp, strFmsPort, m_strFmsServiceName))
			{
				ICC_LOG_ERROR(m_pLog, "get fms service ip and port failed");
				_CommonRecordRespond(p_pNotifiRequest, l_oTempDownloadRecordRespond, ERROR, "get fms service ip and port failed", l_oDownloadRecordRequest.m_oHeader.m_strMsgId);
				return;
			}
			PROTOCOL::CRecordIDToDBRequest l_oRecordIDToDBRequest;
			for (int i = 0; i < l_oTempDownloadRecordRespond.m_oBody.m_vecData.size(); i++)
			{
				_BulidRecordContent(l_oTempDownloadRecordRespond, l_oRecordIDToDBRequest);
			}

			for (int i = 0; i < l_oRecordIDToDBRequest.m_oBody.m_vecData.size(); i++)
			{
				_GetFmsRecord(l_oRecordIDToDBRequest, l_oDownloadRecordRespond, strFmsIp, strFmsPort, i);
			}
			_CommonRecordRespond(p_pNotifiRequest, l_oDownloadRecordRespond, SUCCESS, SUCCESS_MSG, l_oDownloadRecordRequest.m_oHeader.m_strMsgId);
			return;
		}
	}
	else if(bIsSame==false||bJdugeFail==true)
	{
		PROTOCOL::CRecordIDToDBRequest l_oRecordIDToDBRequest;
		std::string strFpsIp;
		std::string strFpsPort;
		if (!GetFileService(strFpsIp, strFpsPort, m_strFpsServiceName))
		{
			ICC_LOG_ERROR(m_pLog, "get fps service ip and port failed");
			_CommonRecordRespond(p_pNotifiRequest, l_oDownloadRecordRespond, ERROR, "get fps service ip and port failed", l_oDownloadRecordRequest.m_oHeader.m_strMsgId);
			return;
		}
		std::string strFmsIp;
		std::string strFmsPort;
		if (!GetFileService(strFmsIp, strFmsPort, m_strFmsServiceName))
		{
			ICC_LOG_ERROR(m_pLog, "get fms service ip and port failed");
			_CommonRecordRespond(p_pNotifiRequest, l_oDownloadRecordRespond, ERROR, "get fms service ip and port failed", l_oDownloadRecordRequest.m_oHeader.m_strMsgId);
			return;
		}
		for (int i = 0; i < l_oTempDownloadRecordRespond.m_oBody.m_vecData.size(); i++)
		{
			if (!l_oTempDownloadRecordRespond.m_oBody.m_vecData[i].m_strRecordId.empty())
			{
				PROTOCOL::CRecordIDToDBRequest l_oTempRecordIDToDBRequest;
				if (!l_oTempRecordIDToDBRequest.ParseString(l_oTempDownloadRecordRespond.m_oBody.m_vecData[i].m_strRecordId, m_pJsonFac->CreateJson()))
				{
					ICC_LOG_DEBUG(m_pLog, "parse record id failed,record_id:[%s]", l_oDownloadRecordRespond.m_oBody.m_vecData[i].m_strRecordId.c_str());
					continue;
				}
				for (int j = 0; j < l_oTempRecordIDToDBRequest.m_oBody.m_vecData.size(); j++)
				{
					PROTOCOL::CRecordIDToDBRequest::CData l_oRecordData;
					l_oRecordData.m_strRecordType = l_oTempDownloadRecordRespond.m_oBody.m_vecData[i].m_strRecord_type;
					l_oRecordData.m_strCallRefId = l_oTempDownloadRecordRespond.m_oBody.m_vecData[i].m_strTrafficId;
					l_oRecordData.m_strCaller = l_oTempDownloadRecordRespond.m_oBody.m_vecData[i].m_strCaller;
					l_oRecordData.m_strCalled = l_oTempDownloadRecordRespond.m_oBody.m_vecData[i].m_strCalled;
					l_oRecordData.m_strFileID = l_oTempRecordIDToDBRequest.m_oBody.m_vecData[j].m_strFileID;
					l_oRecordIDToDBRequest.m_oBody.m_vecData.push_back(l_oRecordData);
				}
			}
			else
			{
				PROTOCOL::CRecordIDToDBRequest::CData l_oRecordData;
				l_oRecordData.m_strRecordType = l_oTempDownloadRecordRespond.m_oBody.m_vecData[i].m_strRecord_type;
				l_oRecordData.m_strCallRefId = l_oTempDownloadRecordRespond.m_oBody.m_vecData[i].m_strTrafficId;
				l_oRecordIDToDBRequest.m_oBody.m_vecData.push_back(l_oRecordData);
			}
		}
		for (int i = 0; i < l_oRecordIDToDBRequest.m_oBody.m_vecData.size(); i++)
		{
			if (l_oRecordIDToDBRequest.m_oBody.m_vecData[i].m_strRecordType == FPS)
			{
				_GetSingleFpsRecordToLoad(l_oRecordIDToDBRequest, l_oDownloadRecordRespond, strFpsIp, strFpsPort, i);
			}
			if (l_oRecordIDToDBRequest.m_oBody.m_vecData[i].m_strRecordType == FMS)
			{
				_GetFmsRecord(l_oRecordIDToDBRequest, l_oDownloadRecordRespond, strFmsIp, strFmsPort, i);
			}
		}
		_CommonRecordRespond(p_pNotifiRequest, l_oDownloadRecordRespond, SUCCESS, SUCCESS_MSG, l_oDownloadRecordRequest.m_oHeader.m_strMsgId);
		return;
	}
}

void CBusinessImpl::_DestoryThreads()
{
	for (size_t i = 0; i < m_uProcThreadCount; ++i)
	{
		m_vecProcThreads[i]->Stop(0);
	}
	m_vecProcThreads.clear();
}


CommonWorkThreadPtr CBusinessImpl::_GetThread()
{
	int iIndex = 0;
	if (m_strDispatchMode == "1")
	{
		std::lock_guard<std::mutex> lock(m_mutexThread);
		if (m_uCurrentThreadIndex >= m_uProcThreadCount)
		{
			m_uCurrentThreadIndex = 0;
		}
		iIndex = m_uCurrentThreadIndex;
		m_uCurrentThreadIndex++;
	}
	else
	{
		std::lock_guard<std::mutex> lock(m_mutexThread);
		int iMessageCount = 0;
		for (size_t i = 0; i < m_uProcThreadCount; ++i)
		{
			int iTmpMessageCount = m_vecProcThreads[i]->GetCWorkThreadListCount();
			if (iTmpMessageCount == 0)
			{
				iIndex = i;
				break;
			}

			if (iMessageCount == 0)
			{
				iIndex = i;
				iMessageCount = iTmpMessageCount;
			}
			else if (iMessageCount > iTmpMessageCount)
			{
				iIndex = i;
				iMessageCount = iTmpMessageCount;
			}
		}
	}

	return m_vecProcThreads[iIndex];
}

void CBusinessImpl::_OnReceiveNotify(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	CommonWorkThreadPtr pThread = _GetThread();
	if (pThread)
	{
		pThread->AddMessage(this, THREADID_RECEIVENOTIFY, 0, 0, 0, p_pNotifiReqeust->GetCmdName(), "", p_pNotifiReqeust);
	}
}

void CBusinessImpl::_DispatchNotify(ObserverPattern::INotificationPtr p_pNotifiReqeust, const std::string& strCmdName)
{
	std::map<std::string, ProcNotify>::const_iterator itr;
	itr = m_mapFuncs.find(strCmdName);
	if (itr != m_mapFuncs.end())
	{
		(this->*itr->second)(p_pNotifiReqeust);
	}
}

void CBusinessImpl::ProcMessage(CommonThread_data msg_data)
{
	switch (msg_data.msg_id)
	{
	case THREADID_RECEIVENOTIFY:
	{
		try
		{
			ICC_LOG_DEBUG(m_pLog, "THREADID_RECEIVENOTIFY begin!");
			_DispatchNotify(msg_data.pTask, msg_data.str_msg);
			ICC_LOG_DEBUG(m_pLog, "THREADID_RECEIVENOTIFY end!");
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "THREADID_RECEIVENOTIFY exception!");
		}
	}
	break;

	default:
		break;
	}
}

bool CBusinessImpl::GetFileService(std::string& p_strIp, std::string& p_strPort, const std::string& strFileServiceName)
{
	ICC_LOG_DEBUG(m_pLog, "file service name:[%s]", strFileServiceName.c_str());
	std::string strNacosQueryUrl = m_strNacosQueryUrl;
	std::string strNacosServerIp = m_strNacosServerIp;
	std::string strNacosServerPort = m_strNacosServerPort;
	std::string strNacosNamespace = m_strNacosServerNamespace;
	std::string strFpsServiceName = strFileServiceName;
	std::string strTarget = m_pString->Format("%s?namespaceId=%s&serviceName=%s@@%s", strNacosQueryUrl.c_str(), strNacosNamespace.c_str(),
		m_strNacosServerGroupName.c_str(), strFpsServiceName.c_str());
	std::string strContent;
	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	std::string strErrorMessage;

	std::string strReceive = m_pHttpClient->GetWithTimeout(strNacosServerIp, strNacosServerPort, strTarget, mapHeaders, strContent, strErrorMessage, 2);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_pLog, "not receive nacos server response.NacosServerIp=%s:%s,strTarget=%s", m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str(), strTarget.c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "receive nacos response : [%s]. ", strReceive.c_str());

	JsonParser::IJsonPtr pJson = ICCGetIJsonFactory()->CreateJson();
	if (!pJson->LoadJson(strReceive))
	{
		ICC_LOG_ERROR(m_pLog, "analyze nacos response failed.[%s] ", strReceive.c_str());
		return false;
	}

	int iCount = pJson->GetCount("/hosts");
	for (int i = 0; i < iCount; i++)
	{
		std::string l_strPrefixPath("/hosts/" + std::to_string(i) + "/");
		std::string strHealthy = pJson->GetNodeValue(l_strPrefixPath + "healthy", "");
		if (strHealthy == m_strServiceHealthyFlag)
		{
			p_strIp = pJson->GetNodeValue(l_strPrefixPath + "ip", "");
			p_strPort = pJson->GetNodeValue(l_strPrefixPath + "port", "");
			ICC_LOG_DEBUG(m_pLog, "find healthy file service:[%s] : [%s:%s] ", strFileServiceName.c_str(),p_strIp.c_str(), p_strPort.c_str());
			return true;
		}
	}

	ICC_LOG_DEBUG(m_pLog, "not find healthy file service,iCount=%d,NacosServerIp=%s:NacosServerPort:[%s]!!!", iCount, m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str());
	return false;
}

void CBusinessImpl::_CommonRecordRespond(ObserverPattern::INotificationPtr p_pNotifiRequest, PROTOCOL::CReceiveDownloadRecordRespond& l_DownloadRespond, const std::string& strCode, const std::string& strMsg, const std::string& strMsgId)
{
	l_DownloadRespond.m_oBody.m_strCode = strCode;
	l_DownloadRespond.m_oBody.m_strMessage = strMsg;
	if (!strMsgId.empty())
	{
		l_DownloadRespond.m_oBody.m_strMsgId = strMsgId;
	}
	else
	{
		l_DownloadRespond.m_oBody.m_strMsgId = m_pString->CreateGuid();
	}
	std::string strSendMsg = l_DownloadRespond.ToString(m_pJsonFac->CreateJson());
	p_pNotifiRequest->Response(strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "response:[%s]", strSendMsg.c_str());
}

bool CBusinessImpl::_GetCallMsgFromDB(const std::string& strCallRefId, PROTOCOL::CReceiveDownloadRecordRespond& l_oDownloadRespond, std::string& strRecordType)
{
	DataBase::SQLRequest l_tSQLRequest;
	l_tSQLRequest.sql_id = "select_icc_t_callevent";
	l_tSQLRequest.param["callref_id"] = strCallRefId;
	DataBase::IResultSetPtr l_pResult = m_pIDBConn->Exec(l_tSQLRequest, true);
	ICC_LOG_DEBUG(m_pLog, "exec sql:[%s]", l_pResult->GetSQL().c_str());
	//std::string strRecordId;
	std::string strSwitchApiType;
	PROTOCOL::CReceiveDownloadRecordRespond::CBody::CData l_oRecordData;
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "exec sql failed,sql:[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (l_pResult->Next())
	{
		//暂时用交换机类型来判断录音，后续根据使用需求更改
		strSwitchApiType = l_pResult->GetValue("switch_type");
		//strRecordId = l_pResult->GetValue("record_file_id");
	}
	//if (strRecordId.empty())
	//{
	//	strRecordType = FPS;
	//}
	if (strSwitchApiType == "0")
	{
		strRecordType = FPS;
		l_oRecordData.m_strRecord_type = FPS;
	}
	else if(strSwitchApiType=="3")
	{
		strRecordType = FMS;
		l_oRecordData.m_strRecord_type = FMS;
		l_oRecordData.m_strCalled = l_pResult->GetValue("called_id");
		l_oRecordData.m_strCaller = l_pResult->GetValue("caller_id");
		//l_oRecordData.m_strEndTime = l_pResult->GetValue("hangup_time");
		//l_oRecordData.m_strStartTime = l_pResult->GetValue("");
		l_oRecordData.m_strGuid = m_pString->CreateGuid();
		l_oRecordData.m_strTrafficId = strCallRefId;
		l_oRecordData.m_strRecordId = l_pResult->GetValue("record_file_id");
	}
	l_oDownloadRespond.m_oBody.m_vecData.push_back(l_oRecordData);
	return true;
}

bool CBusinessImpl::_JudgeIsSameRecordType(std::vector<std::string> m_vecTrafficId, PROTOCOL::CReceiveDownloadRecordRespond& l_oDownloadRecordRespond, std::string& strSumRecordType,bool& bIsSame)
{
	std::string strFirstType;
	for (int i = 0; i < m_vecTrafficId.size(); i++)
	{
		std::string strRecordType;
		if (!_JudgeSingleRecordType(m_vecTrafficId[i], l_oDownloadRecordRespond, strRecordType))
		{
			ICC_LOG_DEBUG(m_pLog, "judge single record type failed,callref_id:[%s]", m_vecTrafficId[i].c_str());
			return false;
		}
		if (i == 0)
		{
			strFirstType = strRecordType;
			strSumRecordType = strRecordType;
			bIsSame = true;
		}
		else
		{
			if (strRecordType != strFirstType)
			{
				bIsSame = false;
			}
		}
	}
	return true;
}

bool CBusinessImpl::_JudgeSingleRecordType(const std::string& strCallRefId, PROTOCOL::CReceiveDownloadRecordRespond& l_oDownloadRecordRespond, std::string& strRecordType)
{
	DataBase::SQLRequest l_tSQLRequest;
	l_tSQLRequest.sql_id = "select_icc_t_callevent";
	l_tSQLRequest.param["callref_id"] = strCallRefId;
	DataBase::IResultSetPtr l_pResult = m_pIDBConn->Exec(l_tSQLRequest, true);
	ICC_LOG_DEBUG(m_pLog, "exec sql:[%s]", l_pResult->GetSQL().c_str());
	//std::string strRecordId;
	std::string strSwitchApiType;
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "exec sql failed,sql:[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (l_pResult->Next())
	{
		//按兵爷需求，暂时按照排队机类型获取录音
		strSwitchApiType= l_pResult->GetValue("switch_type");
		//strRecordId = l_pResult->GetValue("record_file_id");
	}
	//if (strRecordId.empty())
	//{
	//	strRecordType = FPS;
	//}
	//else
	//{
	//	strRecordType = FMS;
	//}
	if (strSwitchApiType == "0")
	{
		PROTOCOL::CReceiveDownloadRecordRespond::CBody::CData l_oDownloadRecordData;
		strRecordType = FPS;
		l_oDownloadRecordData.m_strRecord_type = FPS;
		l_oDownloadRecordData.m_strTrafficId = strCallRefId;
		l_oDownloadRecordRespond.m_oBody.m_vecData.push_back(l_oDownloadRecordData);
	}
	else if (strSwitchApiType == "3")
	{
		PROTOCOL::CReceiveDownloadRecordRespond::CBody::CData l_oDownloadRecordData;
		strRecordType = FMS;
		l_oDownloadRecordData.m_strRecord_type = FMS;
		l_oDownloadRecordData.m_strCalled = l_pResult->GetValue("called_id");
		l_oDownloadRecordData.m_strCaller = l_pResult->GetValue("caller_id");
		//l_oDownloadRecordData.m_strEndTime = l_pResult->GetValue("hangup_time");
		//l_oDownloadRecordData.m_strStartTime = l_pResult->GetValue("");
		l_oDownloadRecordData.m_strGuid = m_pString->CreateGuid();
		l_oDownloadRecordData.m_strTrafficId = strCallRefId;
		l_oDownloadRecordData.m_strRecordId = l_pResult->GetValue("record_file_id");
		l_oDownloadRecordRespond.m_oBody.m_vecData.push_back(l_oDownloadRecordData);
	}
	return true;
}

void CBusinessImpl::_CreateThreads()
{
	Config::IConfigPtr l_pCfgReader = ICCGetIConfigFactory()->CreateConfig();
	if (nullptr == l_pCfgReader)
	{
		ICC_LOG_ERROR(m_pLog, "dbagent create config failed!!!");
		return;
	}
	m_strDispatchMode = l_pCfgReader->GetValue("ICC/Plugin/Extension/procthreaddispatchmode", "1");
	m_uProcThreadCount = m_pString->ToUInt(l_pCfgReader->GetValue("ICC/Plugin/Extension/procthreadcount", "4"));
	boost::thread h1;
	unsigned int uConCurrency = h1.hardware_concurrency();
	if (m_uProcThreadCount > uConCurrency)
	{
		m_uProcThreadCount = uConCurrency;
	}

	ICC_LOG_INFO(m_pLog, "dbagent proc thread count:%d", m_uProcThreadCount);

	for (int i = 0; i < m_uProcThreadCount; ++i)
	{
		CommonWorkThreadPtr pThread = boost::make_shared<CCommonWorkThread>();
		if (pThread)
		{
			m_vecProcThreads.push_back(pThread);
		}
	}

	ICC_LOG_INFO(m_pLog, "dbagent real proc thread count:%d", m_vecProcThreads.size());
}

void CBusinessImpl::_BulidRecordContent(const PROTOCOL::CReceiveDownloadRecordRespond& l_oDownloadRecordRespond, PROTOCOL::CRecordIDToDBRequest& l_oRecordIDToDBRequest)
{
	for (int i = 0; i < l_oDownloadRecordRespond.m_oBody.m_vecData.size(); i++)
	{
		if (!l_oDownloadRecordRespond.m_oBody.m_vecData[i].m_strRecordId.empty())
		{
			PROTOCOL::CRecordIDToDBRequest l_oTempRecordIDToDBRequest;
			if (!l_oTempRecordIDToDBRequest.ParseString(l_oDownloadRecordRespond.m_oBody.m_vecData[i].m_strRecordId, m_pJsonFac->CreateJson()))
			{
				ICC_LOG_DEBUG(m_pLog, "parse record id failed,record_id:[%s]", l_oDownloadRecordRespond.m_oBody.m_vecData[i].m_strRecordId.c_str());
				continue;
			}
			for (int j = 0; j < l_oTempRecordIDToDBRequest.m_oBody.m_vecData.size(); j++)
			{
				PROTOCOL::CRecordIDToDBRequest::CData l_oRecordIdData;
				l_oRecordIdData.m_strFileID = l_oTempRecordIDToDBRequest.m_oBody.m_vecData[j].m_strFileID;
				l_oRecordIdData.m_strCallRefId = l_oDownloadRecordRespond.m_oBody.m_vecData[i].m_strTrafficId;
				l_oRecordIdData.m_strCaller = l_oDownloadRecordRespond.m_oBody.m_vecData[i].m_strCaller;
				l_oRecordIdData.m_strCalled = l_oDownloadRecordRespond.m_oBody.m_vecData[i].m_strCalled;
				l_oRecordIDToDBRequest.m_oBody.m_vecData.push_back(l_oRecordIdData);
			}
		}
	}
}

void CBusinessImpl::_GetFmsRecord(const PROTOCOL::CRecordIDToDBRequest& l_oRecordIDToDBRequest, PROTOCOL::CReceiveDownloadRecordRespond& l_oDownloadRecordRespond, const std::string& strFmsIp, const std::string& strFmsPort,int i)
{
	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", "application/json"));
	std::string strErrorMessage;
	PROTOCOL::PostFmsRecord l_PostFmsRecord;
	l_PostFmsRecord.m_oBody.m_strFileId = l_oRecordIDToDBRequest.m_oBody.m_vecData[i].m_strFileID;
	std::string strPostMsg = l_PostFmsRecord.ToString(m_pJsonFac->CreateJson());
	std::string strFmsTarget = m_strFmsTarget;
	strFmsTarget = strFmsTarget.append(l_PostFmsRecord.m_oBody.m_strFileId);
	std::string strLocalIp;
	std::string strReceive = m_pHttpClient->GetExWithTimeout(strFmsIp, strFmsPort, strFmsTarget, mapHeaders,
		strPostMsg, strErrorMessage, strLocalIp, m_pString->ToInt(m_strSendOverTime));
	ICC_LOG_DEBUG(m_pLog, "receive msg from fms:[%s]", strReceive.c_str());
	PROTOCOL::CGetFmsRecordFile l_oGetFmsRecordFile;
	if (!l_oGetFmsRecordFile.ParseString(strReceive, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "pase fms record result failed,msg:[%s],errormsg:[%s]", strReceive.c_str(), strErrorMessage.c_str());
	}
	PROTOCOL::CReceiveDownloadRecordRespond::CBody::CData l_TempDownloadRecordData;
	//一个文件ID对应一个录音，所以取第0个
	l_TempDownloadRecordData.m_strHttpsUrl = l_oGetFmsRecordFile.m_oBody.l_oCData.m_strHttpsUrl;
	l_TempDownloadRecordData.m_strHttpUrl = l_oGetFmsRecordFile.m_oBody.l_oCData.m_strHttpUrl;
	l_TempDownloadRecordData.m_strCalled = l_oRecordIDToDBRequest.m_oBody.m_vecData[i].m_strCalled;
	l_TempDownloadRecordData.m_strCaller = l_oRecordIDToDBRequest.m_oBody.m_vecData[i].m_strCaller;
	l_TempDownloadRecordData.m_strTrafficId = l_oRecordIDToDBRequest.m_oBody.m_vecData[i].m_strCallRefId;
	l_TempDownloadRecordData.m_strGuid = m_pString->CreateGuid();
	l_oDownloadRecordRespond.m_oBody.m_vecData.push_back(l_TempDownloadRecordData);
}

void CBusinessImpl::_GetSingleFpsRecordToLoad(const PROTOCOL::CRecordIDToDBRequest& l_oRecordIDToDBRequest, PROTOCOL::CReceiveDownloadRecordRespond& l_oDownloadRecordRespond, const std::string& strFpsIp, const std::string& strFpsPort, int i)
{
	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", "application/json"));
	PROTOCOL::CDownloadRecordRequest l_oDownloadRecord;
	std::string strTarrficId = l_oRecordIDToDBRequest.m_oBody.m_vecData[i].m_strCallRefId;
	l_oDownloadRecord.m_oBody.m_vecTrafficId.push_back(strTarrficId);
	std::string l_strRequestMsg=l_oDownloadRecord.ToString(m_pJsonFac->CreateJson());
	ICC_LOG_DEBUG(m_pLog, "post msg:[%s]", l_strRequestMsg.c_str());
	std::string strErrorMessage;
	std::string strReceive = m_pHttpClient->PostWithTimeout(strFpsIp, strFpsPort, m_strFpsDownloadTarget, mapHeaders,
		l_strRequestMsg, strErrorMessage, m_pString->ToInt(m_strSendOverTime));
	ICC_LOG_DEBUG(m_pLog, "receive msg from fps:[%s]", strReceive.c_str());
	PROTOCOL::CReceiveDownloadRecordRespond l_oTempDownloadRecord;
	if (strReceive.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "get fps msg is empty");
		return;
	}
	JsonParser::IJsonPtr pJson = ICCGetIJsonFactory()->CreateJson();
	if (!pJson->LoadJson(strReceive))
	{
		ICC_LOG_ERROR(m_pLog, "load json failed,msg:[%s]", strReceive.c_str());
		return;
	}
	if (!l_oTempDownloadRecord.ParseString(strReceive, m_pJsonFac->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "parse fps msg failed,msg:[%s]", strReceive.c_str());
		return;
	}
	if (l_oTempDownloadRecord.m_oBody.m_vecData.size() > 0)
	{
		PROTOCOL::CReceiveDownloadRecordRespond::CBody::CData l_oTempDownloadRecordData = l_oTempDownloadRecord.m_oBody.m_vecData[0];
		l_oDownloadRecordRespond.m_oBody.m_vecData.push_back(l_oTempDownloadRecordData);
	}
}