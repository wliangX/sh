#include "Boost.h"
#include "PriorityLevelDef.h"
#include "RecordPluginImpl.h"
#include "RecordBusinessImpl.h"
#include "HttpAddress.h"
#include "UploadFiles.h"

//#include "RecordBusinessImpl.h"


unsigned int RecordPluginImpl::GetPriorityLevel()
{
	return PRIORITY_LEVEL_RECORD;
}

void RecordPluginImpl::OnStart()
{
	//CCommonLogger::Instance().InitLogger(GetResourceManager());

	//WRITE_DEBUG_LOG("CPluginImpl::OnStart() begin.");

	//this->m_pResourceManager = GetResourceManager();

	//WRITE_DEBUG_LOG("CPluginImpl::OnStart() complete.");


	m_pRES->m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	if (!m_pRES->m_pLog)
	{// log create error, can't continue
		return;
	}

	m_pRES->m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	if (!m_pRES->m_pConfig)
	{
		ICC_LOG_ERROR(m_pRES->m_pLog, "%s", "Create config failed !");
	}


/*
	m_pRedis = ICCGetIRedisClientFactory()->CreateRedisClient();
	if (!m_pRedis)
	{// xml parser create error, can't continue
		ICC_LOG_ERROR(this->m_pLog, "%s", "init redis client failed!");
		return;
	}
	*/

	m_pRES->m_pJsonFty = ICCGetIJsonFactory();
	if (!m_pRES->m_pJsonFty)
	{// create json factory failed
		ICC_LOG_ERROR(m_pRES->m_pLog, "%s", "create json factory failed!");
		return;
	}

	m_pRES->m_pString = ICCGetIStringFactory()->CreateString();
	if (!m_pRES->m_pString)
	{
		ICC_LOG_ERROR(m_pRES->m_pLog, "%s", "Create string failed!");
	}

	m_pRES->m_pHttpClient = ICCGetIHttpClientFactory()->CreateHttpClient();
	if (!m_pRES->m_pHttpClient)
	{// create http client
		ICC_LOG_ERROR(m_pRES->m_pLog, "%s", "init http client failed!");
		return;
	}

	GetUploadManagerGlobalInstance()->Init(m_pRES);
	GetAddressInteractionGlobalInstance()->Init(m_pRES);


#if 0 // test code
	string fileID;
	string retCode = GetUploadManagerGlobalInstance()->UploadFile("D:\\Proj\\Build.zip", fileID);

	string addr, port;
	string addr2, port2;
	string addr3, port3;
	GetUploadManagerGlobalInstance()->GetFileServiceAddress(addr, port);

	GetAddressInteractionGlobalInstance()->GetHealthyServiceFromNacos("commandcenter-file-service", addr, port);

	GetAddressInteractionGlobalInstance()->GetHealthyServiceFromNacos("commandcenter-file-service", addr2, port2);
	GetAddressInteractionGlobalInstance()->GetHealthyServiceFromNacos("commandcenter-file-service", addr3, port3);

	printf("code[%s], fileID[%s]", retCode.c_str(), fileID.c_str());
#endif


	//
	m_pBusiness = boost::make_shared<RecordBusinessImpl>();
	if (m_pBusiness != nullptr)
	{
		m_pBusiness->Init(GetResourceManager());
		m_pBusiness->Start();
	}

}

void RecordPluginImpl::OnStop()
{
	//WRITE_DEBUG_LOG("CPluginImpl::OnStop() begin.");
	if (m_pBusiness != nullptr)
	{
		m_pBusiness->Stop();
		m_pBusiness->Destroy();
	}

	//WRITE_DEBUG_LOG("CPluginImpl::OnStop() complete.");

	//CCommonLogger::Instance().UninitLogger();
}



RecordPluginImpl::RecordPluginImpl()
{
	m_pRES = boost::make_shared<PluginResource>();
}

RecordPluginImpl::~RecordPluginImpl()
{

}






