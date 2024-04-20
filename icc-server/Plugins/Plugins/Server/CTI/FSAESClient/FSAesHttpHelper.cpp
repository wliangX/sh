#include "Boost.h"
#include "FSAesHttpHelper.h"
#include "FSAesSwitchManager.h"
#include "CTIFSAesDefine.h"
//////////////////////////////////////////////////////////////////////////////
//

class CHCPSyncResponse
{
public:
	virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
	{
		p_pJson->SetNodeValue("/body/code", m_strCode);
		p_pJson->SetNodeValue("/body/msgid", m_strMsgID);
		return p_pJson->ToString();
	}
public:
	std::string m_strCode;
	std::string m_strMsgID;
};

//////////////////////////////////////////////////////////////////////////////
//
class CFSAesHttpCallback : public IHttpCallback
{
public:
	CFSAesHttpCallback(CFSAesHttpHelper* p_HttpServerHelper);
	virtual ~CFSAesHttpCallback();
public:
	virtual void OnMessage(const std::string& p_strSrcIP, const std::string& p_strGuid, const std::string& p_strTarget, const std::string& p_strBody);

private:
	CFSAesHttpHelper* m_HttpHelper;
};

CFSAesHttpCallback::CFSAesHttpCallback(CFSAesHttpHelper* p_HttpHelper)
	: m_HttpHelper(p_HttpHelper)
{
	//
}

CFSAesHttpCallback::~CFSAesHttpCallback()
{
	//
}

void CFSAesHttpCallback::OnMessage(const std::string& p_strSrcIP, const std::string& p_strGuid, const std::string& p_strTarget, const std::string& p_strBody)
{
	if (m_HttpHelper)
	{
		m_HttpHelper->OnHttpMessage(p_strSrcIP, p_strGuid, p_strTarget, p_strBody);
	}
}


////////////////////////////////////////////////////////////////////////////
//
CFSAesHttpHelper::CFSAesHttpHelper(Log::ILogPtr	p_pLogPtr, 
	IResourceManagerPtr p_pResourceManager)
	:m_pResourceManager(p_pResourceManager),
	m_pLog(p_pLogPtr),
	m_pHttpServer(nullptr),
	m_bStart(false),
	m_pHttpRqstClient(nullptr),
	m_strCTIClientID("")
{
	m_strNodeName = "hcp";
	m_pDateTimePtr = ICCGetResourceEx(DateTime::IDateTimeFactory, ICCIDateTimeFactoryResourceName, p_pResourceManager)->CreateDateTime();
}

CFSAesHttpHelper::~CFSAesHttpHelper()
{
	
}


bool CFSAesHttpHelper::Start(const std::string& p_strHttpLocalAddress, const std::string& p_strHttpLocalPort, 
	const std::string& p_strFSAesServerIP, const std::string& p_strFSAesServerPort)
{
	if (m_bStart)
	{
		return true;
	}

	if (p_strHttpLocalAddress.empty() || p_strHttpLocalPort.empty() || p_strFSAesServerIP.empty() || p_strFSAesServerPort.empty())
	{
		return false;
	}

	if (m_pHttpServer == nullptr)
	{
		m_pHttpServer = ICCGetIHttpServerFactory()->CreateHttpServer();
	}

	if (m_pHttpServer)
	{
		m_pHttpServer->StartHttp(boost::make_shared<CFSAesHttpCallback>(this), p_strHttpLocalAddress, std::atoi(p_strHttpLocalPort.c_str()));
	}

	if (m_pHttpRqstClient == nullptr)
	{
		m_pHttpRqstClient = ICCGetIHttpClientFactory()->CreateHttpClient();
	}

	m_strASEServerIP = p_strFSAesServerIP;
	m_strAESServerPort = p_strFSAesServerPort;
	ICC_LOG_DEBUG(m_pLog, "LocalAddr:[%s:%s],ASEServerIP:[%s],AESServerPort[%s] ", p_strHttpLocalAddress.c_str(), p_strHttpLocalPort.c_str(),m_strASEServerIP.c_str(), m_strAESServerPort.c_str());
	m_bStart = true;

	return m_bStart;
}

void CFSAesHttpHelper::Stop()
{
	if (m_pHttpServer)
	{
		m_pHttpServer->StopHttp();
	}
	m_bStart = false;
}

void CFSAesHttpHelper::UpdateAESServerIp(const std::string& p_strAESServerIp)
{
	if (p_strAESServerIp.empty())
	{
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "set aes server ip,oldIp:[%s],newIp:[%s]", m_strASEServerIP.c_str(), p_strAESServerIp.c_str());

	m_strASEServerIP = p_strAESServerIp;
}

bool CFSAesHttpHelper::GetLocalServerIp(std::string& p_strLocalServerIp)
{
	std::string l_strErrorMessage;
	if (!m_pHttpRqstClient->GetLocalIP(m_strASEServerIP, m_strAESServerPort, l_strErrorMessage, p_strLocalServerIp))
	{
		ICC_LOG_ERROR(m_pLog, "Get http LocalIP failed, error msg:%s", l_strErrorMessage.c_str());

		return false;
	}
	return true;
}

void CFSAesHttpHelper::SetNodeName(const std::string& p_strNodeName)
{
	if (!p_strNodeName.empty())
	{
		m_strNodeName = p_strNodeName;
	}
}

void CFSAesHttpHelper::OnHttpMessage(const std::string& p_strSrcIP, const std::string& p_strGuid, const std::string& p_strTarget, const std::string& p_strBody)
{
	if (m_pHttpServer)
	{
		CHCPSyncResponse l_SyncResponse;
		l_SyncResponse.m_strCode = HTTP_SUCCESS_CODE;
		l_SyncResponse.m_strMsgID = p_strGuid;
		m_pHttpServer->OnResponse(p_strGuid, l_SyncResponse.ToString(ICCGetIJsonFactory()->CreateJson()));

		CFSAesSwitchManager::Instance()->OnRecviceFSAesMessage(p_strSrcIP, p_strGuid, p_strTarget, p_strBody);
	}
}


int CFSAesHttpHelper::SendRequestToFSAes(const std::string& p_strMsgId, IFSAesCmdRequestPtr p_pAesCmdRequest,std::string &p_strResponse)
{
	std::string l_strAesRequestBody = _CreateFSAesHttpRequestBody(p_strMsgId, p_pAesCmdRequest);
	if (l_strAesRequestBody.empty())
	{
		return -1;
	}
	
	std::string l_strRequestName = CFSAesSwitchManager::Instance()->GetRequestCmdString(p_pAesCmdRequest->GetRequestCmdType());
	if (p_pAesCmdRequest->GetRequestCmdType() != REQUEST_AES_HEART_BEAT_CMD)
	{
		ICC_LOG_DEBUG(m_pLog, "send request to aes server,cmd:%s,body:[%s]", l_strRequestName.c_str(), l_strAesRequestBody.c_str());
	}
	
	/*
	if (p_pAesCmdRequest->GetRequestCmdType() != REQUEST_AES_LOGIN_CMD && m_strCTIClientID.empty())
	{
		return -1;
	}
	*/
	if (m_pHttpRqstClient)
	{
		std::string strTarget = "/" + m_strNodeName + "/" + l_strRequestName;
		std::string strErrorMessage;
		std::map<std::string, std::string> mapHeaders;
		//JsonParser::IJsonPtr pJson = ICCGetIJsonFactory()->CreateJson();
		mapHeaders.insert(std::make_pair("Content-Type", "application/json"));

		unsigned int l_ulTimeOutVale = 5;
		if (p_pAesCmdRequest->GetRequestCmdType() == REQUEST_AES_LOGIN_CMD)
		{
			l_ulTimeOutVale = 3;
		}
		p_strResponse = m_pHttpRqstClient->PostWithTimeout(m_strASEServerIP, m_strAESServerPort, strTarget, mapHeaders, l_strAesRequestBody, strErrorMessage, l_ulTimeOutVale);
		if (p_strResponse.empty())
		{
			ICC_LOG_ERROR(m_pLog, "http post error,url:[%s:%s %s],data:[%s] ", m_strASEServerIP.c_str(), m_strAESServerPort.c_str(), strTarget.c_str(), l_strAesRequestBody.c_str());
		}

		return RESULT_SUCCESS;
		
	}
	return -1;
}


std::string CFSAesHttpHelper::_CreateFSAesHttpRequestBody(const std::string& p_strMsgId, IFSAesCmdRequestPtr p_pAesCmdRequest)
{
	JsonParser::IJsonPtr pJson = ICCGetIJsonFactory()->CreateJson();
	if (pJson)
	{
		pJson->SetNodeValue("/body/clientid", m_strCTIClientID);
		pJson->SetNodeValue("/body/msgid", p_strMsgId);

		std::map<std::string, std::string> l_mapRequestParams;
		p_pAesCmdRequest->GetRequestParam(l_mapRequestParams);

		std::map<std::string, std::string>::const_iterator itr_const;
		std::string l_strPre;
		for (itr_const = l_mapRequestParams.begin(); itr_const != l_mapRequestParams.end(); ++itr_const)
		{
			if (itr_const->first != CTI_PARAM_cti_conferenceid)
			{
				l_strPre = "/body/" + itr_const->first;
				pJson->SetNodeValue(l_strPre, itr_const->second);
			}
		}
		pJson->SetNodeValue("/body/time", m_pDateTimePtr->CurrentDateTimeStr());

		return pJson->ToString();
	}

	return "";
}
