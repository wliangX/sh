#include "Boost.h"
#include "HttpAddress.h"
#include "RecordPluginImpl.h"


using namespace std;



AddressInteraction::AddressInteraction()
{

}

AddressInteraction::~AddressInteraction()
{

}




void AddressInteraction::Init(boost::shared_ptr<PluginResource> theRES)
{
	this->m_pRES = theRES;

	this->nacosAddress			= m_pRES->m_pConfig->GetValue("ICC/Plugin/Record/NacosServerIp", "");
	this->nacosPort				= m_pRES->m_pConfig->GetValue("ICC/Plugin/Record/NacosServerPort", "");
	this->nacosNameSpaceID		= m_pRES->m_pConfig->GetValue("ICC/Plugin/Record/NacosNamespace", "dev");

	this->nacosAccessAPI		= m_pRES->m_pConfig->GetValue("ICC/Plugin/Record/NacosAccessAPI", "/nacos/v1/ns/instance/list");
	this->nacosGroupName        = m_pRES->m_pConfig->GetValue("ICC/Plugin/Record/NacosGroupName", "global");
	
}

bool AddressInteraction::GetHealthyServiceFromNacos(const string& servName, string& servAddr, string& servPort)
{
	if (this->nacosAddress.length() < 7 || this->nacosPort.length() < 3)
	{
		ICC_LOG_ERROR(m_pRES->m_pLog, "%s", "Nacos address init info invalid !!");
		return false;
	}

	string getHttpStr = "http://";
	getHttpStr += this->nacosAddress;
	getHttpStr += ":";
	getHttpStr += this->nacosPort;
	getHttpStr += this->nacosAccessAPI;
	getHttpStr += "?serviceName=";
	getHttpStr += servName;
	getHttpStr += "&groupName=";
	getHttpStr += this->nacosGroupName;
	getHttpStr += "&namespaceId=";
	getHttpStr += this->nacosNameSpaceID;

	ICC_LOG_INFO(m_pRES->m_pLog, "Get Nacos healthy URL:%s", getHttpStr.c_str());

	string responseString = this->m_pRES->m_pHttpClient->Get(getHttpStr, "");

	//printf("%s", responseString.c_str());

	ICC_LOG_INFO(m_pRES->m_pLog, "Nacos response json:%s", responseString.c_str());

	return ParseNacosResponse(responseString, servAddr, servPort);
}


bool AddressInteraction::ParseNacosResponse(const std::string& theResponseString, string& servAddr, string& servPort)
{
	int pos = theResponseString.find('{');
	int lastPos = theResponseString.rfind('}');

	if (pos == string::npos || lastPos == string::npos)
	{// can't parse invalid json string
		ICC_LOG_ERROR(m_pRES->m_pLog, "%s", "response from Nacos is invalid!!");
		return false;
	}

	std::string midString = theResponseString.substr(pos, lastPos-pos+1);

	JsonParser::IJsonPtr pJson = this->m_pRES->m_pJsonFty->CreateJson();

	if (!pJson->LoadJson(midString))
	{
		ICC_LOG_ERROR(m_pRES->m_pLog, "%s", "parse Nacos response failed!!");
		return false;
	}


	for (int hostIdx = 0; hostIdx < 10; hostIdx++)
	{
		string tmpHead = "/hosts/";
		tmpHead += (hostIdx + '0');

		string newServiceAddress = pJson->GetNodeValue(tmpHead + "/ip", "");
		string newServicePort = pJson->GetNodeValue(tmpHead + "/port", "");

		string healStr = pJson->GetNodeValue(tmpHead + "/healthy", "");

		if (newServiceAddress.length() > 6 && newServicePort.length() > 2
			&& (healStr == "1" || healStr == "true") )
		{// get a healthy service.
			servAddr = newServiceAddress;
			servPort = newServicePort;
			return true;
		}

		if (newServiceAddress.length() <= 1 || newServicePort.length() <= 1)
		{// haven't valid address or port
			ICC_LOG_ERROR(m_pRES->m_pLog, "%s", "Can't pick health service 01 !!");
			return false;
		}

		if (healStr != "1" && healStr != "true")
		{// haven't valid address or port
			ICC_LOG_ERROR(m_pRES->m_pLog, "%s", "Can't pick health service 02 !!");
			continue;
		}
	}

	return false;
}


boost::shared_ptr<AddressInteraction> GetAddressInteractionGlobalInstance()
{
	static boost::shared_ptr<AddressInteraction> S_pAI_Inst = NULL;
	if (!S_pAI_Inst)
	{
		S_pAI_Inst = boost::make_shared<AddressInteraction>();
	}
	return S_pAI_Inst;
}
