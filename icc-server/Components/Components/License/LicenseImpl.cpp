#include "Boost.h"
#include "LicenseImpl.h"

#define CHECK_INTERVAL 3600

void CResCallImpl::OnResponse(std::string p_strMessage)
{
	m_pLicense->OnResponse(p_strMessage);
}

CLicenseImpl::CLicenseImpl(IResourceManagerPtr p_ResourceManagerPtr)
	: m_pResourceManager(p_ResourceManagerPtr), 
	m_Permanence(m_IoService),
	m_Timer(m_IoService)
{
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pJsonFty = ICCGetIJsonFactory();
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pStaticConfig = ICCGetIConfigFactory()->CreateStaticConfig();
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pConsumer = ICCGetAmqClient()->CreateAsyncConsumer();
	m_pProducer = ICCGetAmqClient()->CreateProducer();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pHelpTool = ICCGetHelpToolFactory()->CreateHelpTool();

	m_strbrokerURI = m_pConfig->GetValue("ICC/Component/AmqClient/BrokerURI", "failover:(tcp://127.0.0.1:61616)");
	m_strClientID = m_pStaticConfig->GetValue("ICC/Component/AmqClient/ClientID", "") + ".License";
	m_strClientID += "." + m_pString->CreateGuid();
	m_strUserName = m_pConfig->GetValue("ICC/Component/AmqClient/UserName", "admin");
	m_strPassword = m_pConfig->GetValue("ICC/Component/AmqClient/Password", "admin");

	ICC_LOG_DEBUG(m_pLog, "Prepare connect AMQ , brokerURI[%s], strClientID[%s], strUserName[%s]",
		m_strbrokerURI.c_str(), m_strClientID.c_str(), m_strUserName.c_str());

	if (!ConnectMQ())
	{
		ICC_LOG_DEBUG(m_pLog, "message connect to amq failed.");
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "connect to amq success.");

	GetLicenseInfo();

	std::string l_strGuid = m_pString->CreateGuid();

	m_Timer.async_wait([this, l_strGuid](const boost::system::error_code &){
		this->OnTimer(l_strGuid);
	});

	m_Timer.expires_from_now(boost::posix_time::seconds(CHECK_INTERVAL));
}

CLicenseImpl::~CLicenseImpl()
{

}


//定时器响应
void CLicenseImpl::OnTimer(std::string p_strTimerName)
{
	GetLicenseInfo();

	//激活下一次定时器
	try
	{
		m_Timer.expires_from_now(boost::posix_time::seconds(CHECK_INTERVAL));

		m_Timer.async_wait([this, p_strTimerName](const boost::system::error_code &){
			this->OnTimer(p_strTimerName);
		});
	}
	catch (...)
	{
		
	}
}


void CLicenseImpl::GetLicenseInfo()
{
	PROTOCOL::CSmpSyncRequest l_oCSmpRequest;
	l_oCSmpRequest.m_oHeader.m_strSystemID = "ICC";
	l_oCSmpRequest.m_oHeader.m_strSubsystemID = "";
	l_oCSmpRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oCSmpRequest.m_oHeader.m_strCmd = "get_license_infor";
	l_oCSmpRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oCSmpRequest.m_oHeader.m_strRequest = "queue_smp_sharedata";
	l_oCSmpRequest.m_oHeader.m_strRequestType = "0";
	l_oCSmpRequest.m_oBody.m_strSystemNo = "ICC";
	l_oCSmpRequest.m_oBody.m_strVersion = m_strLicenseVersion;
	l_oCSmpRequest.m_strType = TYPE_SYSTEM;

	std::string l_strMessage = l_oCSmpRequest.ToString(m_pJsonFty->CreateJson());

	boost::lock_guard<boost::mutex> guard(m_Mutex);
	{
		if (!(m_pProducer && m_pProducer->Request(l_strMessage, "queue_smp_sharedata", boost::make_shared<CResCallImpl>(this), nullptr)))  //请求应答模式
		{
			ICC_LOG_ERROR(m_pLog, "send request message to smp failed: [%s]", l_strMessage.c_str());
			return;
		}
	
		ICC_LOG_DEBUG(m_pLog, "send request message to smp.[%s]", l_strMessage.c_str());
	}
}

void CLicenseImpl::OnResponse(std::string p_strMessage)
{
	boost::lock_guard<boost::mutex> guard(m_Mutex);
	
	ICC_LOG_DEBUG(m_pLog, "smp respond license success.[%s]", p_strMessage.c_str());

	PROTOCOL::CGetLicenseRequest p_pLicenseRequest;
	if (!p_pLicenseRequest.ParseString(p_strMessage, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "license parsestring error");
		return;
	}

	if (p_pLicenseRequest.m_Body.m_strCount == "0" || p_pLicenseRequest.m_Body.m_vecData.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "respond license info is null");
		return;
	}

	for (auto var : p_pLicenseRequest.m_Body.m_vecData)
	{
		if (var.m_strNodeDesp == "ICC")
		{
			m_licenseInfo.m_strEnableTimeLimit = var.m_strEnableTimeLimit;
			m_licenseInfo.m_strEnableSuperdogCode = var.m_strEnableSuperdogCode;
			m_licenseInfo.m_strNodeDesp = var.m_strNodeDesp;
			m_licenseInfo.m_strNodeName = var.m_strNodeName;
			m_licenseInfo.m_strSuperdogCode = var.m_strSuperdogCode;
			m_licenseInfo.m_strMachineCode = var.m_strMachineCode;
			m_licenseInfo.m_strGenDate = var.m_strGenDate;
			m_licenseInfo.m_strDefaultNum = var.m_strDefaultNum;
			m_licenseInfo.m_strValidDate = var.m_strValidDate;
			m_licenseInfo.m_strVenderCode = var.m_strVenderCode;
			m_licenseInfo.m_strEnableMachineCode = var.m_strEnableMachineCode;
			m_licenseInfo.m_strLicenseDesp = var.m_strLicenseDesp;
			m_licenseInfo.m_strValidDays = var.m_strValidDays;
			m_licenseInfo.m_strStatus = var.m_strStatus;
			
			// 第一个是秘钥生成的时间
			// 第二个是截止日期，第三个是有效天数，第二个第三个条件 哪个先到期就按哪个算
			// 这个工具是puc给开发的

			DateTime::CDateTime l_start = m_pDateTime->FromString(m_licenseInfo.m_strGenDate);
			DateTime::CDateTime l_end = m_pDateTime->FromString(m_licenseInfo.m_strValidDate);
			int l_iDays = m_pDateTime->DaysDifference(l_end, l_start);
			int l_iValidDays = m_pString->ToUInt(m_licenseInfo.m_strValidDays);

			if (l_iDays > l_iValidDays)
			{
				DateTime::CDateTime l_end2 = m_pDateTime->AddDays(l_start, l_iValidDays);
				m_licenseInfo.m_strValidDate = m_pDateTime->ToString(l_end2);
			}
			else
			{
				m_licenseInfo.m_strValidDays = m_pString->Number(l_iDays);
			}
			
			ICC_LOG_DEBUG(m_pLog, "license info: enabletimelimit[%s] enablesuperdogcode[%s] enablemachinecode[%s] systemcode[%s] systemname[%s] superdogcode[%s] machinecode[%s] gendate[%s] vendercode[%s] validdays[%d] status[%s] clientnum[%d] validdate[%s], licensedesp[%s]",
			IsEnableSuperdogCode() ? "true" : "false",
			IsEnableSuperdogCode() ? "true" : "false",
			IsEnableMachineCode() ? "true" : "false",
			GetSystemCode().c_str(),
			GetSystemName().c_str(),
			GetSuperdogCode().c_str(),
			GetMachineCode().c_str(),
			GetGenDate().c_str(),
			GetVenderCode().c_str(),
			GetValidDays(),
			GetStatus().c_str(),
			GetClientNum(),
			GetValidDate().c_str(),
			GetLicenseDesp().c_str());

			break;
		}
	}
}

bool CLicenseImpl::ConnectMQ()
{
	std::string strConsumerClientId = m_strClientID;
	if (!m_pConsumer->Connect(m_strbrokerURI, strConsumerClientId, m_strUserName, m_strPassword))
	{
		ICC_LOG_ERROR(m_pLog, "connect AMQ failed, brokerURI[%s], strClientID[%s], strUserName[%s], strPassword[%s]",
			m_strbrokerURI.c_str(), strConsumerClientId.c_str(), m_strUserName.c_str(), m_strPassword.c_str());
		return false;
	}
	ICC_LOG_DEBUG(m_pLog, "consumer connect AMQ success, brokerURI[%s], strClientID[%s], strUserName[%s]",
		m_strbrokerURI.c_str(), m_strClientID.c_str(), m_strUserName.c_str());

	std::string strProducerClientId = m_strClientID;
	if (!m_pProducer->Connect(m_strbrokerURI, strProducerClientId, m_strUserName, m_strPassword))
	{
		ICC_LOG_ERROR(m_pLog, "connect AMQ failed, brokerURI[%s], strClientID[%s], strUserName[%s], strPassword[%s]",
			m_strbrokerURI.c_str(), strProducerClientId.c_str(), m_strUserName.c_str(), m_strPassword.c_str());
		return false;
	}
	ICC_LOG_DEBUG(m_pLog, "producer connect AMQ success, brokerURI[%s], strClientID[%s], strUserName[%s]",
		m_strbrokerURI.c_str(), m_strClientID.c_str(), m_strUserName.c_str());

	return true;
}

//是否启用限制时间控制
bool CLicenseImpl::IsEnableTimeLimit()
{
	return m_licenseInfo.m_strEnableTimeLimit == "1" ? true : false;
}

//是否启用超级狗控制
bool CLicenseImpl::IsEnableSuperdogCode()
{
	return m_licenseInfo.m_strEnableSuperdogCode == "1" ? true : false;
}

//是否启用机器码控制
bool CLicenseImpl::IsEnableMachineCode()
{
	return m_licenseInfo.m_strEnableMachineCode == "1" ? true : false;
}

//获取系统编码
std::string CLicenseImpl::GetSystemCode()
{
	return m_licenseInfo.m_strNodeDesp;
}

//获取系统名称
std::string CLicenseImpl::GetSystemName()
{
	return m_licenseInfo.m_strNodeName;
}

//获取超级狗编码
std::string CLicenseImpl::GetSuperdogCode()
{
	return m_licenseInfo.m_strSuperdogCode;
}

//获取机器码
std::string CLicenseImpl::GetMachineCode()
{
	return m_licenseInfo.m_strMachineCode;
}

//获取License key生成日期
std::string CLicenseImpl::GetGenDate()
{
	return m_licenseInfo.m_strGenDate;
}

//获取厂商代码
std::string CLicenseImpl::GetVenderCode()
{
	return m_licenseInfo.m_strVenderCode;
}

//获取截止日期
std::string CLicenseImpl::GetValidDate()
{
	return m_licenseInfo.m_strValidDate;
}

//License状态
std::string CLicenseImpl::GetStatus()
{
	return m_licenseInfo.m_strStatus;
}

//获取客户端授权数量
unsigned int CLicenseImpl::GetClientNum()
{
	return m_pString->ToUInt(m_licenseInfo.m_strDefaultNum);
}

//获取有效天数
unsigned int CLicenseImpl::GetValidDays()
{
	return m_pString->ToUInt(m_licenseInfo.m_strValidDays);
}

//LicenseDesp
std::string CLicenseImpl::GetLicenseDesp()
{
	return m_licenseInfo.m_strLicenseDesp;
}

IResourceManagerPtr License::CLicenseImpl::GetResourceManager()
{
	return m_pResourceManager;
}

