#include "Boost.h"
#include "MessageCenterImpl.h"
#define CLIENT_REGISTER_INFO	"ClientRegisterInfo"
#define PARAM_INFO				"ParamInfo"

CMessageCenterImpl::CMessageCenterImpl(IResourceManagerPtr p_pIResourceManager) : 
	m_pResourceManager(p_pIResourceManager)
{
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_LogPtr = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pHttpClient = ICCGetIHttpClientFactory()->CreateHttpClient();
	m_JsonFactoryPtr = ICCGetIJsonFactory();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();

	m_workThreadStop = false;
	m_workThreadStart = false;

	Start();
}

CMessageCenterImpl::~CMessageCenterImpl()
{
	m_workThreadStop = true;
	m_cvMsg.notify_all();
	l_oThreadWorkList->join();
}

bool CMessageCenterImpl::Start()
{
	if (m_workThreadStart == false)
	{
		l_oThreadWorkList.reset(new boost::thread(boost::bind(&CMessageCenterImpl::WorkThread, this, this)));
		m_workThreadStart = true;
	}

	return true;
}

bool CMessageCenterImpl::Stop()
{
	m_workThreadStop = true;
	m_workThreadStart = false;
	m_cvMsg.notify_all();
	l_oThreadWorkList->join();

	return true;
}

ICC::IResourceManagerPtr CMessageCenterImpl::GetResourceManager()
{
	return m_pResourceManager;
}

bool CMessageCenterImpl::GetMsgCenterService(std::string& p_strIp, std::string& p_strPort)
{
	std::string m_strNacosServerIp = this->m_strNacosServerIp;
	std::string m_strNacosServerPort = this->m_strNacosServerPort;
	std::string m_strNacosNamespace = m_strNacosServerNamespace;
	std::string m_strMsgCenterServiceName = m_pConfig->GetValue("ICC/Component/MessageCenter/servicename", "commandcenter-umc-service");
	std::string m_strNacosGroupName = m_strNacosServerGroupName;
	std::string m_strNacosQueryUrl = m_pConfig->GetValue("ICC/Component/HttpServer/queryurl", "/nacos/v1/ns/instance/list");
	std::string m_strMsgCenterServiceHealthyFlag = m_pConfig->GetValue("ICC/Component/MessageCenter/servicehealthyflag", "1");

	std::string strTarget = m_pString->Format("%s?namespaceId=%s&serviceName=%s@@%s", m_strNacosQueryUrl.c_str(), m_strNacosNamespace.c_str(),
		m_strNacosGroupName.c_str(), m_strMsgCenterServiceName.c_str());
	std::string strContent;
	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	std::string strErrorMessage;

	std::string strReceive = m_pHttpClient->GetEx(m_strNacosServerIp, m_strNacosServerPort, strTarget, mapHeaders, strContent, strErrorMessage);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_LogPtr, "not receive nacos server response.NacosServerIp=%s:%s,strTarget=%s", m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str(), strTarget.c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_LogPtr, "receive nacos response : [%s]. ", strReceive.c_str());

	JsonParser::IJsonPtr pJson = m_JsonFactoryPtr->CreateJson();
	if (!pJson->LoadJson(strReceive))
	{
		ICC_LOG_ERROR(m_LogPtr, "analyze nacos response failed.[%s] ", strReceive.c_str());
		return false;
	}

	int iCount = pJson->GetCount("/hosts");
	for (int i = 0; i < iCount; i++)
	{
		std::string l_strPrefixPath("/hosts/" + std::to_string(i) + "/");
		std::string strHealthy = pJson->GetNodeValue(l_strPrefixPath + "healthy", "");
		if (strHealthy == m_strMsgCenterServiceHealthyFlag)
		{
			p_strIp = pJson->GetNodeValue(l_strPrefixPath + "ip", "");
			p_strPort = pJson->GetNodeValue(l_strPrefixPath + "port", "");
			ICC_LOG_DEBUG(m_LogPtr, "find healthy msg center service : [%s:%s]. ", p_strIp.c_str(), p_strPort.c_str());
			return true;
		}
	}

	ICC_LOG_DEBUG(m_LogPtr, "not find healthy msg center service,iCount=%d,m_strNacosServerIp=%s:%s!!!", iCount, m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str());
	return false;
}

bool CMessageCenterImpl::GetSmpService(std::string& p_strIp, std::string& p_strPort, std::string l_serverName)
{
	std::string m_strNacosServerIp = this->m_strNacosServerIp;
	std::string m_strNacosServerPort = this->m_strNacosServerPort;
	std::string m_strNacosNamespace = m_strNacosServerNamespace;
	std::string m_strMsgCenterServiceName = l_serverName;
	std::string m_strNacosGroupName = m_strNacosServerGroupName;
	std::string m_strNacosQueryUrl = m_pConfig->GetValue("ICC/Component/HttpServer/queryurl", "/nacos/v1/ns/instance/list");
	std::string m_strMsgCenterServiceHealthyFlag = "1";

	std::string strTarget = m_pString->Format("%s?namespaceId=%s&serviceName=%s@@%s", m_strNacosQueryUrl.c_str(), m_strNacosNamespace.c_str(),
		m_strNacosGroupName.c_str(), m_strMsgCenterServiceName.c_str());
	std::string strContent;
	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	std::string strErrorMessage;

	std::string strReceive = m_pHttpClient->GetWithTimeout(m_strNacosServerIp, m_strNacosServerPort, strTarget, mapHeaders, strContent, strErrorMessage,2);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_LogPtr, "not receive nacos server response.NacosServerIp=%s:%s,strTarget=%s", m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str(), strTarget.c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_LogPtr, "receive nacos response : [%s]. ", strReceive.c_str());

	JsonParser::IJsonPtr pJson = m_JsonFactoryPtr->CreateJson();
	if (!pJson->LoadJson(strReceive))
	{
		ICC_LOG_ERROR(m_LogPtr, "analyze nacos response failed.[%s] ", strReceive.c_str());
		return false;
	}

	int iCount = pJson->GetCount("/hosts");
	for (int i = 0; i < iCount; i++)
	{
		std::string l_strPrefixPath("/hosts/" + std::to_string(i) + "/");
		std::string strHealthy = pJson->GetNodeValue(l_strPrefixPath + "healthy", "");
		if (strHealthy == m_strMsgCenterServiceHealthyFlag)
		{
			p_strIp = pJson->GetNodeValue(l_strPrefixPath + "ip", "");
			p_strPort = pJson->GetNodeValue(l_strPrefixPath + "port", "");
			ICC_LOG_DEBUG(m_LogPtr, "find healthy msg center service : [%s:%s]. ", p_strIp.c_str(), p_strPort.c_str());
			return true;
		}
	}

	ICC_LOG_DEBUG(m_LogPtr, "not find healthy msg center service,iCount=%d,m_strNacosServerIp=%s:%s!!!", iCount, m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str());
	return false;
}

bool CMessageCenterImpl::BuildSendMsg(std::string& l_buildMsg, MessageSendDTOData l_sendData)
{
	JsonParser::IJsonPtr pJson = m_JsonFactoryPtr->CreateJson();

	pJson->SetNodeValue("/appCode", l_sendData.appCode);
	pJson->SetNodeValue("/businessCode", l_sendData.businessCode);
	pJson->SetNodeValue("/compensateType", l_sendData.compensateType);
	pJson->SetNodeValue("/delayDuration", l_sendData.delayDuration);
	pJson->SetNodeValue("/delayType", l_sendData.compensateType);
	pJson->SetNodeValue("/moduleCode", l_sendData.moduleCode);
	pJson->SetNodeValue("/needStorage", l_sendData.needStorage);
	pJson->SetNodeValue("/sendType", l_sendData.sendType);
	pJson->SetNodeValue("/sync", l_sendData.sync);
	pJson->SetNodeValue("/title", l_sendData.title);
	pJson->SetNodeValue("/message", l_sendData.message);

	unsigned int l_curIndex = 0;
	for (std::vector<ReceiveObject>::iterator it = l_sendData.receiveObjects.begin(); it != l_sendData.receiveObjects.end(); it++)
	{
		JsonParser::IJsonPtr l_pIJson = m_JsonFactoryPtr->CreateJson();
		unsigned int l_iIndex = 0;
		for (auto it_notice = it->noticeList.cbegin(); it_notice != it->noticeList.cend(); it_notice++)
		{
			pJson->SetNodeValue("/receiveObjects/" + std::to_string(l_curIndex) + "/noticeList/" + std::to_string(l_iIndex), *it_notice);
			l_iIndex++;
		}

		l_iIndex = 0;
		for (auto it_notice = it->syncList.cbegin(); it_notice != it->syncList.cend(); it_notice++)
		{
			pJson->SetNodeValue("/receiveObjects/" + std::to_string(l_curIndex) + "/syncList/" + std::to_string(l_iIndex), *it_notice);
			l_iIndex++;
		}

		l_iIndex = 0;
		for (auto it_notice = it->orgRule.cbegin(); it_notice != it->orgRule.cend(); it_notice++)
		{
			pJson->SetNodeValue("/receiveObjects/" + std::to_string(l_curIndex) + "/orgRule/" + std::to_string(l_iIndex), *it_notice);
			l_iIndex++;
		}

		pJson->SetNodeValue("/receiveObjects/" + std::to_string(l_curIndex) + "/type", it->type);
		l_curIndex++;
	}

	l_buildMsg = pJson->ToString();

	return true;
}

bool CMessageCenterImpl::Send(std::string l_sendMsg, const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName)
{
	ICC_LOG_DEBUG(m_LogPtr, "add msg to msg center : [%s]. ", l_sendMsg.c_str());
	SetNacosParams(strNacosIp, strNacosPort, strNameSpace, strGroupName);
	AddWorkMsg(l_sendMsg);

	return true;
}

bool CMessageCenterImpl::Send(std::string l_sendMsg, std::string l_alarmId, bool send_to_icc_bz, bool send_to_alarmer, const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName)
{
	if (!send_to_icc_bz && !send_to_alarmer)
	{
		ICC_LOG_DEBUG(m_LogPtr, "No one pays attention to the current alarm list");
		return true;
	}

	MsgCenter::MessageSendDTOData l_CDTOData;
	std::string l_curSendMsg;
	l_CDTOData.appCode = "icc";
	l_CDTOData.businessCode = "alarminfo";
	l_CDTOData.compensateType = 0;
	l_CDTOData.delayDuration = "";
	l_CDTOData.delayType = 0;
	l_CDTOData.title = l_sendMsg;
	l_CDTOData.moduleCode = "alarminfo";
	l_CDTOData.needStorage = "true";
	l_CDTOData.sendType = "0";
	l_CDTOData.sync = "true";
	l_CDTOData.message = l_alarmId;

	std::string l_deptCode;
	std::string l_deptGuid;
	std::string l_alarmerCode;

	if (send_to_icc_bz)
	{
		if (GetDeptCode(l_deptCode, l_alarmerCode, l_alarmId))
		{
			l_deptGuid = DeptCodeToDeptGuid(l_deptCode);
		}
		std::vector<std::string> l_curNoticeList = GetGuidbByDept(l_deptGuid, "RECEIVE-MONITOR", strNacosIp, strNacosPort, strNameSpace, strGroupName);

		if (l_curNoticeList.empty())
		{
			MsgCenter::ReceiveObject l_recObj;
			l_recObj.type = "role";
			l_recObj.noticeList.push_back(m_pConfig->GetValue("ICC/Plugin/MonitorSeat/MonitorCode", "ICC_BZ"));
			l_recObj.syncList.push_back(m_pConfig->GetValue("ICC/Plugin/MonitorSeat/MonitorCode", "ICC_BZ"));

			l_CDTOData.receiveObjects.push_back(l_recObj);
		}
		else
		{ 
			MsgCenter::ReceiveObject l_recObj;
			l_recObj.type = "user";
			for (int index = 0; index < l_curNoticeList.size(); index++)
			{
				if (send_to_alarmer && l_alarmerCode == l_curNoticeList[index])
				{
					continue;
				}
				l_recObj.noticeList.push_back(l_curNoticeList[index]);
				l_recObj.syncList.push_back(l_curNoticeList[index]);
			}

			l_CDTOData.receiveObjects.push_back(l_recObj);
		}
	}


	if (send_to_alarmer)
	{
		MsgCenter::ReceiveObject l_recObj;
		l_recObj.type = "user";

		//添加警单接警员推送
		l_recObj.noticeList.push_back(l_alarmerCode);
		l_recObj.syncList.push_back(l_alarmerCode);

		l_CDTOData.receiveObjects.push_back(l_recObj);
	}

	if (l_CDTOData.receiveObjects[0].noticeList.size() == 0)
	{
		ICC_LOG_DEBUG(m_LogPtr, "No one pays attention to the current alarm list: [%s]", l_curSendMsg.c_str());
		return false;
	}

	BuildSendMsg(l_curSendMsg, l_CDTOData);
	Send(l_curSendMsg, m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName);

	return true;
}


void CMessageCenterImpl::WorkThread(void* l_curMessageCenter)
{
	CMessageCenterImpl* l_pMsgCenter = (CMessageCenterImpl*)l_curMessageCenter;

	while (true)
	{
		if (l_pMsgCenter->m_workThreadStop)
			break;

		//l_pMsgCenter->m_mtxMsg.lock();
		if (l_pMsgCenter->m_msgWorkList.empty())
		{
			boost::unique_lock<boost::mutex> lock(l_pMsgCenter->m_mutexWait);
			l_pMsgCenter->m_cvMsg.wait(lock);
		}

		std::vector<std::string> tmpMsgWorkList;
		{
			boost::lock_guard<boost::mutex> lock(l_pMsgCenter->m_mtxMsg);
			tmpMsgWorkList = l_pMsgCenter->m_msgWorkList;
			l_pMsgCenter->m_msgWorkList.clear();
		}

		for (auto l_ita = tmpMsgWorkList.begin(); l_ita != tmpMsgWorkList.end(); ++l_ita)
		{
			SendMsgTo12345(*l_ita);			
		}

		//l_pMsgCenter->m_mtxMsg.unlock();
	}
}

void CMessageCenterImpl::AddWorkMsg(std::string l_strMsg)
{
	m_mtxMsg.lock();
	m_msgWorkList.push_back(l_strMsg);
	m_mtxMsg.unlock();

	m_cvMsg.notify_all();
}

bool CMessageCenterImpl::SendMsgTo12345(std::string l_strMsg)
{
	std::string strTarget = "/msg/send";
	std::string strErrorMessage;
	std::map<std::string, std::string> mapHeaders;

	std::string m_strMsgCenterServerIp;
	std::string m_strMsgCenterServerPort;

	JsonParser::IJsonPtr pJson = m_JsonFactoryPtr->CreateJson();
	mapHeaders.insert(std::make_pair("Content-Type", "application/json"));

	GetMsgCenterService(m_strMsgCenterServerIp, m_strMsgCenterServerPort);

	ICC_LOG_DEBUG(m_LogPtr, "send message to msg center: %s", l_strMsg.c_str());

	std::string strReceive = m_pHttpClient->PostEx(m_strMsgCenterServerIp, m_strMsgCenterServerPort, strTarget, mapHeaders, l_strMsg, strErrorMessage);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_LogPtr, "post msg center error!!!!err[%s]", strErrorMessage.c_str());
		return false;
	}
	else
	{
		ICC_LOG_DEBUG(m_LogPtr, "get message from msg center: %s", strReceive.c_str());
	}

	return true;
}

void CMessageCenterImpl::SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName)
{
	boost::lock_guard<boost::mutex> lock(m_mutexNacosParams);
	m_strNacosServerIp = strNacosIp;
	m_strNacosServerPort = strNacosPort;
	m_strNacosServerNamespace = strNameSpace;
	m_strNacosServerGroupName = strGroupName;
}

std::vector<std::string> CMessageCenterImpl::GetGuidbByDept(std::string l_orgGuid, std::string l_roleAttr, const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName)
{
	SetNacosParams(strNacosIp, strNacosPort, strNameSpace, strGroupName);
	std::vector<std::string> l_resDatas;
	JsonParser::IJsonPtr pJson = m_JsonFactoryPtr->CreateJson();
	std::string l_sendMsg;

	pJson->SetNodeValue("/orgGuid", l_orgGuid);
	pJson->SetNodeValue("/roleAttr", l_roleAttr);
	pJson->SetNodeValue("/orgType", "belong");

	l_sendMsg = pJson->ToString();

	std::string strTarget = "/basic/staff/queryGuidByOrgGuidAndRoleType?orgGuid=" + l_orgGuid + "&roleAttr=" + l_roleAttr + "&orgType=belong";
	std::string strErrorMessage;
	std::map<std::string, std::string> mapHeaders;

	std::string m_strSMPServerIp;
	std::string m_strSMPServerPort;

	mapHeaders.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));

	GetSmpService(m_strSMPServerIp, m_strSMPServerPort, "center-smp-northface-service");

	ICC_LOG_DEBUG(m_LogPtr, "%s, %s, send message to smp service: %s", l_orgGuid.c_str(), l_roleAttr.c_str(), l_sendMsg.c_str());

	std::string strReceive = m_pHttpClient->GetWithTimeout(m_strSMPServerIp, m_strSMPServerPort, strTarget, mapHeaders, l_sendMsg, strErrorMessage,2);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_LogPtr, "post smp service error!!!!err[%s]", strErrorMessage.c_str());
		return l_resDatas;
	}
	else
	{
		ICC_LOG_DEBUG(m_LogPtr, "get message from smp service: %s", strReceive.c_str());

		JsonParser::IJsonPtr l_pJson = m_JsonFactoryPtr->CreateJson();
		l_pJson->LoadJson(strReceive);

		int iCount = l_pJson->GetCount("/data");

		for (int i = 0; i < iCount; i++)
		{
			std::string l_strPrefixPath("/data/" + std::to_string(i));

			l_resDatas.push_back(l_pJson->GetNodeValue(l_strPrefixPath, ""));

			ICC_LOG_LOWDEBUG(m_LogPtr, "get message: %s", l_pJson->GetNodeValue(l_strPrefixPath, "").c_str());
		}

		return l_resDatas;
	}
}

bool CMessageCenterImpl::Send(std::string l_sendMsg, std::string strSeatNo, std::string strDeptCode, const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName)
{
	MsgCenter::MessageSendDTOData l_CDTOData;
	std::string l_curSendMsg;
	l_CDTOData.appCode = "icc";
	l_CDTOData.businessCode = "violation";
	l_CDTOData.compensateType = 0;
	l_CDTOData.delayDuration = "";
	l_CDTOData.delayType = 0;
	l_CDTOData.title = l_sendMsg;
	l_CDTOData.moduleCode = "violation";
	l_CDTOData.needStorage = "true";
	l_CDTOData.sendType = "0";
	l_CDTOData.sync = "true";
	l_CDTOData.message = strSeatNo;
	std::string l_deptGuid = DeptCodeToDeptGuid(strDeptCode);
	std::vector<std::string> l_curNoticeList = GetGuidbByDept(l_deptGuid, "RECEIVE-MONITOR", strNacosIp, strNacosPort, strNameSpace, strGroupName);
	if (l_curNoticeList.empty())
	{
		MsgCenter::ReceiveObject l_recObj;
		l_recObj.type = "role";
		l_recObj.noticeList.push_back(m_pConfig->GetValue("ICC/Plugin/MonitorSeat/MonitorCode", "ICC_BZ"));
		l_recObj.syncList.push_back(m_pConfig->GetValue("ICC/Plugin/MonitorSeat/MonitorCode", "ICC_BZ"));

		l_CDTOData.receiveObjects.push_back(l_recObj);
	}
	else
	{
		MsgCenter::ReceiveObject l_recObj;
		l_recObj.type = "user";
		for (int index = 0; index < l_curNoticeList.size(); index++)
		{
			l_recObj.noticeList.push_back(l_curNoticeList[index]);
			l_recObj.syncList.push_back(l_curNoticeList[index]);
		}

		l_CDTOData.receiveObjects.push_back(l_recObj);
	}
	if (l_CDTOData.receiveObjects[0].noticeList.size() == 0)
	{
		ICC_LOG_DEBUG(m_LogPtr, "No one pays attention to the current alarm list: [%s]", l_curSendMsg.c_str());
		return false;
	}

	BuildSendMsg(l_curSendMsg, l_CDTOData);
	Send(l_curSendMsg, m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName);

	return true;
}


bool CMessageCenterImpl::SendMsgToCenter(std::string l_sendMsg, std::string l_sendType, std::string strSeatNo, std::string strDeptCode, const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName)
{
	MsgCenter::MessageSendDTOData l_CDTOData;
	std::string l_curSendMsg;
	l_CDTOData.appCode = "icc";
	l_CDTOData.businessCode = l_sendType;
	l_CDTOData.compensateType = 0;
	l_CDTOData.delayDuration = "";
	l_CDTOData.delayType = 0;
	l_CDTOData.title = l_sendMsg;
	l_CDTOData.moduleCode = l_sendType;
	l_CDTOData.needStorage = "true";
	l_CDTOData.sendType = "0";
	l_CDTOData.sync = "true";
	l_CDTOData.message = strSeatNo;
	std::string l_deptGuid = DeptCodeToDeptGuid(strDeptCode);
	std::vector<std::string> l_curNoticeList = GetGuidbByDept(l_deptGuid, "RECEIVE-MONITOR", strNacosIp, strNacosPort, strNameSpace, strGroupName);
	if (l_curNoticeList.empty())
	{
		MsgCenter::ReceiveObject l_recObj;
		l_recObj.type = "role";
		l_recObj.noticeList.push_back(m_pConfig->GetValue("ICC/Plugin/MonitorSeat/MonitorCode", "ICC_BZ"));
		l_recObj.syncList.push_back(m_pConfig->GetValue("ICC/Plugin/MonitorSeat/MonitorCode", "ICC_BZ"));

		l_CDTOData.receiveObjects.push_back(l_recObj);
	}
	else
	{
		MsgCenter::ReceiveObject l_recObj;
		l_recObj.type = "user";
		for (int index = 0; index < l_curNoticeList.size(); index++)
		{
			l_recObj.noticeList.push_back(l_curNoticeList[index]);
			l_recObj.syncList.push_back(l_curNoticeList[index]);
		}

		l_CDTOData.receiveObjects.push_back(l_recObj);
	}
	if (l_CDTOData.receiveObjects[0].noticeList.size() == 0)
	{
		ICC_LOG_DEBUG(m_LogPtr, "No one pays attention to the current alarm list: [%s]", l_curSendMsg.c_str());
		return false;
	}

	BuildSendMsg(l_curSendMsg, l_CDTOData);
	Send(l_curSendMsg, m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName);

	return true;
}

void CMessageCenterImpl::BuildManualAcceptConfig(std::string l_manualAcceptType, std::string& l_value)
{
	std::string strParamInfo;
	if (!m_pRedisClient->HGet(PARAM_INFO, l_manualAcceptType, strParamInfo))
	{
		ICC_LOG_DEBUG(m_LogPtr, "Hget ParamInfo Failed!!!");
		return;
	}
	PROTOCOL::CParamInfo l_ParamInfo;
	if (!l_ParamInfo.Parse(strParamInfo, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_DEBUG(m_LogPtr, "Json Parse ParamInfo Failed!!!");
	}
	l_value = l_ParamInfo.m_strValue;
}

bool CMessageCenterImpl::GetUserGuid(std::string& l_strUserGuid, std::string& l_strUserDeptGuid, std::string l_strUserCode, std::string l_strUserName)
{
	DataBase::SQLRequest l_strDeptSql;

	l_strDeptSql.sql_id = "select_icc_t_staff_all";
	l_strDeptSql.param["code"] = l_strUserCode;
	l_strDeptSql.param["name"] = l_strUserName;
	DataBase::IResultSetPtr l_sqlResult = m_pDBConn->Exec(l_strDeptSql);

	if (!l_sqlResult->IsValid())
	{
		std::string l_strSQL = l_sqlResult->GetSQL();
		ICC_LOG_ERROR(m_LogPtr, "%s table failed, SQL: [%s], Error: [%s]",
			l_strDeptSql.sql_id.c_str(),
			l_strSQL.c_str(),
			l_sqlResult->GetErrorMsg().c_str());

		return "";
	}
	else if (l_sqlResult->Next())
	{
		ICC_LOG_DEBUG(m_LogPtr, "sql success,SQL: [%s]",
			l_sqlResult->GetSQL().c_str());

		l_strUserGuid = l_sqlResult->GetValue("guid");
		l_strUserDeptGuid = l_sqlResult->GetValue("dept_guid");
		return true;
	}

	return false;
}

std::string CMessageCenterImpl::DeptCodeToDeptGuid(std::string l_deptCode)
{
	DataBase::SQLRequest l_strDeptSql;

	l_strDeptSql.sql_id = "select_icc_t_dept_by_code";
	l_strDeptSql.param["code"] = l_deptCode;
	DataBase::IResultSetPtr l_sqlResult = m_pDBConn->Exec(l_strDeptSql);

	if (!l_sqlResult->IsValid())
	{
		std::string l_strSQL = l_sqlResult->GetSQL();
		ICC_LOG_ERROR(m_LogPtr, "%s table failed, SQL: [%s], Error: [%s]",
			l_strDeptSql.sql_id.c_str(),
			l_strSQL.c_str(),
			l_sqlResult->GetErrorMsg().c_str());

		return "";
	}
	else if (l_sqlResult->Next())
	{
		ICC_LOG_DEBUG(m_LogPtr, "sql success,SQL: [%s]",
			l_sqlResult->GetSQL().c_str());

		return l_sqlResult->GetValue("guid");
	}

	return "";
}

bool CMessageCenterImpl::GetDeptCode(std::string& l_strDeptCode, std::string& l_strAlarmerCode, std::string l_strAlarmId)
{
	DataBase::SQLRequest l_strDeptSql;

	l_strDeptSql.sql_id = "select_icc_t_jjdb_by_alarm_id";
	l_strDeptSql.param["id"] = l_strAlarmId;
	std::string strTime = m_pDateTime->GetAlarmIdTime(l_strAlarmId);
	if (strTime != "")
	{
		l_strDeptSql.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime);
		l_strDeptSql.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}
	DataBase::IResultSetPtr l_sqlResult = m_pDBConn->Exec(l_strDeptSql);
	if (!l_sqlResult->IsValid())
	{
		std::string l_strSQL = l_sqlResult->GetSQL();
		ICC_LOG_ERROR(m_LogPtr, "%s table failed, SQL: [%s], Error: [%s]",
			l_strDeptSql.sql_id.c_str(),
			l_strSQL.c_str(),
			l_sqlResult->GetErrorMsg().c_str());

		return false;
	}
	else if (l_sqlResult->Next())
	{
		ICC_LOG_DEBUG(m_LogPtr, "sql success, value is %s ,SQL: [%s]",
			l_sqlResult->GetValue("value").c_str(),
			l_sqlResult->GetSQL().c_str());

		l_strDeptCode = l_sqlResult->GetValue("jjdwdm");
		l_strAlarmerCode = l_sqlResult->GetValue("alarmer_code");

		return true;
	}

	return false;
}

bool CMessageCenterImpl::Send(std::string l_sendMsg, std::string l_deptCode, bool send_to_icc_bz, const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName, std::string business_code)
{
	if (!send_to_icc_bz)
	{
		ICC_LOG_DEBUG(m_LogPtr, "No one pays attention to the current alarm list");
		return true;
	}

	MsgCenter::MessageSendDTOData l_CDTOData;
	std::string l_curSendMsg;
	l_CDTOData.appCode = "icc";
	l_CDTOData.businessCode = business_code;
	l_CDTOData.compensateType = 0;
	l_CDTOData.delayDuration = "";
	l_CDTOData.delayType = 0;
	l_CDTOData.title = l_sendMsg;
	l_CDTOData.moduleCode = business_code;
	l_CDTOData.needStorage = "true";
	l_CDTOData.sendType = "0";
	l_CDTOData.sync = "true";
	l_CDTOData.message = l_deptCode;

	std::string l_deptGuid;

	if (send_to_icc_bz)
	{
		l_deptGuid = DeptCodeToDeptGuid(l_deptCode);

		std::vector<std::string> l_curNoticeList = GetGuidbByDept(l_deptGuid, "RECEIVE-MONITOR", strNacosIp, strNacosPort, strNameSpace, strGroupName);

		if (l_curNoticeList.empty())
		{
			MsgCenter::ReceiveObject l_recObj;
			l_recObj.type = "role";
			l_recObj.noticeList.push_back(m_pConfig->GetValue("ICC/Plugin/MonitorSeat/MonitorCode", "ICC_BZ"));
			l_recObj.syncList.push_back(m_pConfig->GetValue("ICC/Plugin/MonitorSeat/MonitorCode", "ICC_BZ"));

			l_CDTOData.receiveObjects.push_back(l_recObj);
		}
		else
		{
			MsgCenter::ReceiveObject l_recObj;
			l_recObj.type = "user";
			for (int index = 0; index < l_curNoticeList.size(); index++)
			{
				l_recObj.noticeList.push_back(l_curNoticeList[index]);
				l_recObj.syncList.push_back(l_curNoticeList[index]);
			}

			l_CDTOData.receiveObjects.push_back(l_recObj);
		}
	}

	if (l_CDTOData.receiveObjects[0].noticeList.size() == 0)
	{
		ICC_LOG_DEBUG(m_LogPtr, "No one pays attention to the current alarm list: [%s]", l_curSendMsg.c_str());
		return false;
	}

	BuildSendMsg(l_curSendMsg, l_CDTOData);
	Send(l_curSendMsg, m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName);

	return true;
}

bool CMessageCenterImpl::Send(std::string l_sendMsg, std::string l_userName, std::string l_userCode, bool send_to_icc_bz, const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName, std::string business_code)
{
	MsgCenter::MessageSendDTOData l_CDTOData;
	std::string l_curSendMsg;
	l_CDTOData.appCode = "icc";
	l_CDTOData.businessCode = business_code;
	l_CDTOData.compensateType = 0;
	l_CDTOData.delayDuration = "";
	l_CDTOData.delayType = 0;
	l_CDTOData.title = l_sendMsg;
	l_CDTOData.moduleCode = business_code;
	l_CDTOData.needStorage = "true";
	l_CDTOData.sendType = "0";
	l_CDTOData.sync = "true";
	l_CDTOData.message = l_userName;

	std::string l_UserGuid;
	std::string l_deptGuid;

	if (!GetUserGuid(l_UserGuid, l_deptGuid, l_userCode, l_userName))
	{
		ICC_LOG_DEBUG(m_LogPtr, "No find UserGuid or DeptGuid", l_curSendMsg.c_str());
		return false;
	}
	std::vector<std::string> l_curNoticeList = GetGuidbByDept(l_deptGuid, "RECEIVE-MONITOR", strNacosIp, strNacosPort, strNameSpace, strGroupName);

	if (l_curNoticeList.empty())
	{
		MsgCenter::ReceiveObject l_recObj;
		l_recObj.type = "role";
		l_recObj.noticeList.push_back(m_pConfig->GetValue("ICC/Plugin/MonitorSeat/MonitorCode", "ICC_BZ"));
		l_recObj.syncList.push_back(m_pConfig->GetValue("ICC/Plugin/MonitorSeat/MonitorCode", "ICC_BZ"));

		l_CDTOData.receiveObjects.push_back(l_recObj);
	}
	else
	{
		MsgCenter::ReceiveObject l_recObj;
		l_recObj.type = "user";
		for (int index = 0; index < l_curNoticeList.size(); index++)
		{
			l_recObj.noticeList.push_back(l_curNoticeList[index]);
			l_recObj.syncList.push_back(l_curNoticeList[index]);
		}

		l_recObj.noticeList.push_back(l_UserGuid);
		l_recObj.syncList.push_back(l_UserGuid);

		l_CDTOData.receiveObjects.push_back(l_recObj);
	}

	if (l_CDTOData.receiveObjects[0].noticeList.size() == 0)
	{
		ICC_LOG_DEBUG(m_LogPtr, "No one pays attention to the current alarm list: [%s]", l_curSendMsg.c_str());
		return false;
	}

	BuildSendMsg(l_curSendMsg, l_CDTOData);
	Send(l_curSendMsg, m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName);

	return true;
}

bool CMessageCenterImpl::Send(std::string strBusinessCode, bool send_to_icc_bz, bool send_to_alarmer, const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName,std::string m_strDeviceInfo)
{
	if (!send_to_icc_bz && !send_to_alarmer)
	{
		ICC_LOG_DEBUG(m_LogPtr, "No one pays attention to the current alarm list");
		return true;
	}
	MsgCenter::MessageSendDTOData l_CDTOData;
	std::string l_curSendMsg;
	l_CDTOData.appCode = "icc";
	l_CDTOData.businessCode = strBusinessCode;
	l_CDTOData.compensateType = 0;
	l_CDTOData.delayDuration = "";
	l_CDTOData.delayType = 0;
	l_CDTOData.title = strBusinessCode;
	l_CDTOData.moduleCode = strBusinessCode;
	l_CDTOData.needStorage = "false";
	l_CDTOData.sendType = "0";
	l_CDTOData.sync = "true";
	l_CDTOData.message = m_strDeviceInfo;

	PROTOCOL::CDeviceInfo l_DeviceInfo;
	if (!l_DeviceInfo.Parse(m_strDeviceInfo, m_JsonFactoryPtr->CreateJson()))
	{
		ICC_LOG_DEBUG(m_LogPtr, "Parse Device Info Failed!!!");
		return false;
	}
	std::string l_deptGuid;
	std::string l_alarmerCode;
	if (!GetAlarmerGuidBySeat(l_DeviceInfo.m_strCalledId, l_alarmerCode))
	{
		ICC_LOG_DEBUG(m_LogPtr, "Get Alarmer Guid Failed!!!");
		return false;
	}
	if (send_to_icc_bz)
	{
		l_deptGuid = DeptCodeToDeptGuid(l_DeviceInfo.m_strDeptCode);
		std::vector<std::string> l_curNoticeList = GetGuidbByDept(l_deptGuid, "RECEIVE-MONITOR", strNacosIp, strNacosPort, strNameSpace, strGroupName);

		if (l_curNoticeList.empty())
		{
			MsgCenter::ReceiveObject l_recObj;
			l_recObj.type = "role";
			l_recObj.noticeList.push_back(m_pConfig->GetValue("ICC/Plugin/MonitorSeat/MonitorCode", "ICC_BZ"));
			l_recObj.syncList.push_back(m_pConfig->GetValue("ICC/Plugin/MonitorSeat/MonitorCode", "ICC_BZ"));

			l_CDTOData.receiveObjects.push_back(l_recObj);
		}
		else
		{
			MsgCenter::ReceiveObject l_recObj;
			l_recObj.type = "user";
			for (int index = 0; index < l_curNoticeList.size(); index++)
			{
				if (send_to_alarmer && l_alarmerCode == l_curNoticeList[index])
				{
					continue;
				}
				l_recObj.noticeList.push_back(l_curNoticeList[index]);
				l_recObj.syncList.push_back(l_curNoticeList[index]);
			}

			l_CDTOData.receiveObjects.push_back(l_recObj);
		}
	}

	if (send_to_alarmer)
	{
		MsgCenter::ReceiveObject l_recObj;
		l_recObj.type = "user";

		//添加警单接警员推送
		l_recObj.noticeList.push_back(l_alarmerCode);
		l_recObj.syncList.push_back(l_alarmerCode);

		l_CDTOData.receiveObjects.push_back(l_recObj);
	}

	if (l_CDTOData.receiveObjects[0].noticeList.size() == 0)
	{
		ICC_LOG_DEBUG(m_LogPtr, "No one pays attention to the current alarm list: [%s]", l_curSendMsg.c_str());
		return false;
	}

	BuildSendMsg(l_curSendMsg, l_CDTOData);
	Send(l_curSendMsg, strNacosIp, strNacosPort, strNameSpace, strGroupName);

	return true;
}

bool CMessageCenterImpl::AlarmerCodeToGuid(std::string& strCode,std::string &strGuid)
{
	DataBase::SQLRequest l_strDeptSql;

	l_strDeptSql.sql_id = "select_icc_t_staff_by_alarmer_id";
	l_strDeptSql.param["code"] = strCode;
	DataBase::IResultSetPtr l_sqlResult = m_pDBConn->Exec(l_strDeptSql);
	if (!l_sqlResult->IsValid())
	{
		std::string l_strSQL = l_sqlResult->GetSQL();
		ICC_LOG_ERROR(m_LogPtr, "%s table failed, SQL: [%s], Error: [%s]",
			l_strDeptSql.sql_id.c_str(),
			l_strSQL.c_str(),
			l_sqlResult->GetErrorMsg().c_str());

		return false;
	}
	else if (l_sqlResult->Next())
	{
		ICC_LOG_DEBUG(m_LogPtr, "sql success, value is %s ,SQL: [%s]",
			l_sqlResult->GetValue("value").c_str(),
			l_sqlResult->GetSQL().c_str());

		strGuid = l_sqlResult->GetValue("guid");

		return true;
	}

	return true;
}

bool CMessageCenterImpl::GetAlarmerGuidBySeat(std::string& l_strSeat, std::string& l_alarmerCode)
{
	std::string m_strClientRegisterInfo;
	if (!m_pRedisClient->HGet(CLIENT_REGISTER_INFO, l_strSeat, m_strClientRegisterInfo))
	{
		ICC_LOG_DEBUG(m_LogPtr, "Hget Client Register Info Failed!!!");
		return false;
	}
	CRegisterInfo l_RegisterInfo;
	JsonParser::IJsonPtr l_pJson = m_JsonFactoryPtr->CreateJson();
	if (!l_RegisterInfo.Parse(m_strClientRegisterInfo, l_pJson))
	{
		ICC_LOG_DEBUG(m_LogPtr, "Parse Client Register Info Failed!!!");
		return false;
	}
	if (!AlarmerCodeToGuid(l_RegisterInfo.m_strStaffCode, l_alarmerCode))
	{
		ICC_LOG_DEBUG(m_LogPtr, "Alarmer Code To Guid Failed!!!");
		return false;
	}
	return true;
}