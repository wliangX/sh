#include "Boost.h"
#include "BusinessImpl.h"

#include <sstream>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>

#define TIMER_CMD_NAME	"get_login_token"
using namespace boost::archive::iterators;

const std::string CODE_SUCCESS = "200";
const std::string CODE_SFXX_SUCCESS = "0";

void CBusinessImpl::OnInit()
{
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(GATEWAY_PERSONINFO_OBSERVER_CENTER);
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();

	m_pString = ICCGetIStringFactory()->CreateString();
	//m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();

	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);

	m_pHttpClient = ICCGetIHttpClientFactory()->CreateHttpClient();

	m_pJsonFty = ICCGetIJsonFactory();

	m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();
}

void CBusinessImpl::OnStart()
{

	std::string l_strIsUsing = m_pConfig->GetValue("ICC/Plugin/PersonInfo/IsUsing", "0");
	if (l_strIsUsing != "1")
	{
		ICC_LOG_DEBUG(m_pLog, "there is no need to load the plugin");
		return;
	}

	m_strServerIP = m_pConfig->GetValue("ICC/Plugin/PersonInfo/serverip", "");  //服务IP
	m_strServerPort = m_pConfig->GetValue("ICC/Plugin/PersonInfo/serverport", "80");  //服务端口

	m_strToken = m_pConfig->GetValue("ICC/Plugin/PersonInfo/token", "");  //授权码

	m_strUsername = m_pConfig->GetValue("ICC/Plugin/PersonInfo/Username", "");  //用户名
	m_strPassword = m_pConfig->GetValue("ICC/Plugin/PersonInfo/Password", "");  //密码

	m_strSmulateFlag = m_pConfig->GetValue("ICC/Plugin/PersonInfo/SmulateFlag", "0");  //是否模拟

	m_intTimeOut = m_pString->ToInt(m_pConfig->GetValue("ICC/Plugin/PersonInfo/TimeOut", "5"));

	//get_bjr_sfzh
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_bjr_sfxx", OnNotifGetPersonInfoByIdCard);
	//get_bjr_sfxx
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_bjr_sfzh", OnNotifGetPersonInfoByPhone);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TIMER_CMD_NAME, OnNotifGetLoginToken);

	//添加定时器 3000s调用一次
	m_pTimerMgr->AddTimer(TIMER_CMD_NAME, 3000, 1);
	ICC_LOG_DEBUG(m_pLog, "%s plugin started.", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_DEBUG(m_pLog, "%s plugin stop.", MODULE_NAME);
}

void CBusinessImpl::OnDestroy()
{

}

bool CBusinessImpl::_Base64Encode(const std::string& input, std::string* output)
{
	typedef base64_from_binary<transform_width<std::string::const_iterator, 6, 8>> Base64EncodeIterator;
	std::stringstream result;
	try {
		copy(Base64EncodeIterator(input.begin()), Base64EncodeIterator(input.end()), std::ostream_iterator<char>(result));
	}
	catch (...) {
		return false;
	}
	size_t equal_count = (3 - input.length() % 3) % 3;
	for (size_t i = 0; i < equal_count; i++)
	{
		result.put('=');
	}
	*output = result.str();
	return output->empty() == false;
}

void CBusinessImpl::OnNotifGetLoginToken(ObserverPattern::INotificationPtr p_pNotify)
{
	//登录 定时调用
	ICC_LOG_DEBUG(m_pLog, "OnNotifGetLoginToken start");
	if (!Login())
	{
		ICC_LOG_DEBUG(m_pLog, "Login failed");
		return;
	}
}

bool CBusinessImpl::Login()
{
	ICC_LOG_DEBUG(m_pLog, "PersonInfo Login start");

	std::string strContent;
	std::string strErrorMessage;
	std::string strLogin;
	std::string strTarget("/api/zhzxapi/login");
	JsonParser::IJsonPtr pJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CGetBJRSFZHLoginPespond m_oLoginRespond;

	m_oLoginRespond.m_oBody.m_strUsername = m_strUsername;
	m_oLoginRespond.m_oBody.m_strPassword = m_strPassword;
	
	ICC_LOG_DEBUG(m_pLog, "PersonInfo Login Username[%s], m_strPassword[%s]", m_oLoginRespond.m_oBody.m_strUsername.c_str(), m_oLoginRespond.m_oBody.m_strPassword.c_str());
	
	strContent = m_oLoginRespond.ToString(m_pJsonFty->CreateJson());
	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", "application/json"));

	ICC_LOG_DEBUG(m_pLog, "PersonInfo Login strContent[%s]", strContent.c_str());

	strLogin = m_pHttpClient->PostEx(m_strServerIP, m_strServerPort, strTarget, mapHeaders, strContent, strErrorMessage);
	
	//strLogin = LoginContent();
	ICC_LOG_DEBUG(m_pLog, "PersonInfo Login strLogin[%s]", strLogin.c_str());
	if (strLogin.empty())
	{
		ICC_LOG_ERROR(m_pLog, "PersonInfo post Login error!!!!err[%s]", strErrorMessage.c_str());
		return false;
	}
	
	//解析  Parse
	PROTOCOL::CGetBJRSFZHLoginRequest m_oLoginRequest;

	if (!m_oLoginRequest.ParseString(strLogin, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "PersonInfo LoginRequestt[%s] ParseString Failed", strLogin.c_str());
		return false;
	}

	ICC_LOG_ERROR(m_pLog, "PersonInfo LoginRequest Parse [%s]", strLogin.c_str());

	if (0 != m_oLoginRequest.m_oBody.m_code.compare("0"))
	{
		ICC_LOG_ERROR(m_pLog, "PersonInfo LoginRequest code Parse[%s]", m_oLoginRequest.m_oBody.m_code.c_str());
		return false;
	}
	m_strToken = m_oLoginRequest.m_oBody.m_data;
	ICC_LOG_DEBUG(m_pLog, "PersonInfo Login end");
	return true;
}

void CBusinessImpl::OnNotifGetPersonInfoByIdCard(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "PersonInfo GetPersonInfoByIdCard receive message:[%s]", p_pNotify->GetMessages().c_str());

	//解析请求消息
	PROTOCOL::CGetBJRSFXXRequest tmp_oRequest;

	if (!tmp_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	std::string strContent;
	std::string strErrorMessage;
	std::string strIdCardRespond;
	//如果配置模拟
	if ("1" == m_strSmulateFlag)
	{
		strIdCardRespond = GetPostTextByContent("get_bjr_sfxx");
	}
	else
	{
		std::string strTarget("/api/zhzxapi/getPersonInfoByIdCard");

		PROTOCOL::CGetBJRSFZHByIdCardPespond m_oIdCardRespond;
		m_oIdCardRespond.m_oBody.m_strGmsfhm = tmp_oRequest.m_oBody.m_strBJRSFZH;
		m_oIdCardRespond.m_oBody.m_strDwellType = "2";
		strContent = m_oIdCardRespond.ToString(m_pJsonFty->CreateJson());
		std::map<std::string, std::string> mapHeaders;
		mapHeaders.insert(std::make_pair("Content-Type", "application/json"));
		mapHeaders.insert(std::make_pair("token", m_strToken));

		ICC_LOG_DEBUG(m_pLog, "PersonInfo GetPersonInfoByIdCard strContent[%s]", strContent.c_str());
		//strIdCardRespond = m_pHttpClient->PostEx(m_strServerIP, m_strServerPort, strTarget, mapHeaders, strContent, strErrorMessage);
		strIdCardRespond = m_pHttpClient->PostWithTimeout(m_strServerIP, m_strServerPort, strTarget, mapHeaders, strContent, strErrorMessage, m_intTimeOut);

		ICC_LOG_DEBUG(m_pLog, "PersonInfo GetPersonInfoByIdCar strIdCardRespond[%s]", strIdCardRespond.c_str());
	}
	
	//解析
	PROTOCOL::CGetBJRSFZHIdentiyInfoRequest m_oInfoRequest;
	m_oInfoRequest.m_oHeader = tmp_oRequest.m_oHeader;
	do
	{
		if (strIdCardRespond.empty())
		{
			ICC_LOG_ERROR(m_pLog, "PersonInfo post getPersonInfoByIdCard error!!!!err[%s]", strErrorMessage.c_str());
			m_oInfoRequest.m_oBody.m_code = "500";
			m_oInfoRequest.m_oBody.m_msg = "Failed";
			break;
		}
		std::string strReceive = Subreplace(strIdCardRespond, "\"null\"", "\"\"");

		if (!m_oInfoRequest.ParseString(strReceive, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "PersonInfo GetPersonInfoByIdCar Request[%s] ParseString Failed", strReceive.c_str());
			m_oInfoRequest.m_oBody.m_code = "501";
			m_oInfoRequest.m_oBody.m_msg = "ParseString Failed";
			break;
		}
	} while (0);

	//入库 insert_icc_t_alarm_bjrxx
	if ((0 == m_oInfoRequest.m_oBody.m_code.compare("200")) && (m_oInfoRequest.m_oBody.m_vecData.size() > 0))
	{
		InsertIdentiyInfo(m_oInfoRequest);
	}
	
	//组装信息给前端页面 
	BuildBJRSFXXRRespond(m_oInfoRequest, p_pNotify);

}

void CBusinessImpl::OnNotifGetPersonInfoByPhone(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "PersonInfo GetPersonInfoByPhone receive message:[%s]", p_pNotify->GetMessages().c_str());
	//解析请求消息
	PROTOCOL::CGetBJRSFZHRequest tmp_oRequest;

	if (!tmp_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}
	std::string strContent;
	std::string strErrorMessage;
	std::string strPhoneRespond;

	//如果配置模拟
	if ("1" == m_strSmulateFlag)
	{
		strPhoneRespond = GetPostTextByContent("get_bjr_sfzh");
	}
	else
	{
		std::string strTarget("/api/zhzxapi/getPersonInfoByPhone");
		PROTOCOL::CGetBJRSFZHByPhonePespond m_oPhoneRespond;

		m_oPhoneRespond.m_oBody.m_strlxdh = tmp_oRequest.m_oBody.m_strPhoneNo;

		strContent = m_oPhoneRespond.ToString(m_pJsonFty->CreateJson());
		std::map<std::string, std::string> mapHeaders;
		mapHeaders.insert(std::make_pair("Content-Type", "application/json"));
		mapHeaders.insert(std::make_pair("token", m_strToken));
		ICC_LOG_DEBUG(m_pLog, "PersonInfo GetPersonInfoByPhone strContent[%s]", strContent.c_str());

		//strPhoneRespond = m_pHttpClient->PostEx(m_strServerIP, m_strServerPort, strTarget, mapHeaders, strContent, strErrorMessage);
		strPhoneRespond = m_pHttpClient->PostWithTimeout(m_strServerIP, m_strServerPort, strTarget, mapHeaders, strContent, strErrorMessage, m_intTimeOut);
		
		ICC_LOG_DEBUG(m_pLog, "PersonInfo GetPersonInfoByPhone strPhoneRespond[%s]", strPhoneRespond.c_str());
	}
	//解析
	PROTOCOL::CGetBJRSFZHIdentiyInfoRequest m_oInfoRequest;

	do
	{
		if (strPhoneRespond.empty())
		{
			ICC_LOG_ERROR(m_pLog, "post getPersonInfoByPhone error!!!!err[%s]", strErrorMessage.c_str());
			m_oInfoRequest.m_oBody.m_code = "500";
			m_oInfoRequest.m_oBody.m_msg = "ParseString Failed";
			break;
		}
		std::string strReceive = Subreplace(strPhoneRespond, "\"null\"", "\"\"");
		if (!m_oInfoRequest.ParseString(strReceive, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "getPersonInfoByPhone[%s] ParseString Failed", strReceive.c_str());
			m_oInfoRequest.m_oBody.m_code = "501";
			m_oInfoRequest.m_oBody.m_msg = "ParseString Failed";
			break;
		}

	} while (0);
	//入库 insert_icc_t_alarm_bjrxx
	//InsertIdentiyInfo(m_oInfoRequest);
	
	//组装信息给前端页面 
	BuildBJRSFHMRRespond(m_oInfoRequest, p_pNotify);
}

void CBusinessImpl::BuildBJRSFHMRRespond(PROTOCOL::CGetBJRSFZHIdentiyInfoRequest& m_oInfoRequest, ObserverPattern::INotificationPtr p_pNotify)
{
	ICC::PROTOCOL::CGetBJRSFZHRespond l_oRespond;
	l_oRespond.m_oBody.m_strCode = m_oInfoRequest.m_oBody.m_code;
	l_oRespond.m_oBody.m_strMessage = m_oInfoRequest.m_oBody.m_msg;

	if ((0 == m_oInfoRequest.m_oBody.m_code.compare("200")) && (!m_oInfoRequest.m_oBody.m_vecData.empty()))
	{
		l_oRespond.m_oBody.m_strSFZH = m_oInfoRequest.m_oBody.m_vecData[0].m_strgmsfhm;
		l_oRespond.m_oBody.m_strXM = m_oInfoRequest.m_oBody.m_vecData[0].m_strxm;
	}

	std::string l_strMessage = l_oRespond.ToString(ICCGetIJsonFactory()->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:%s", l_strMessage.c_str());
}

void CBusinessImpl::BuildBJRSFXXRRespond(PROTOCOL::CGetBJRSFZHIdentiyInfoRequest& m_oInfoRequest, ObserverPattern::INotificationPtr p_pNotify)
{
	//组装信息给前端页面 
	PROTOCOL::CGetBJRSFXXRespond l_oRespond;
	l_oRespond.m_oBody.m_strCode = m_oInfoRequest.m_oBody.m_code;
	l_oRespond.m_oBody.m_strMessage = m_oInfoRequest.m_oBody.m_msg;

	if ((0 == m_oInfoRequest.m_oBody.m_code.compare("200")) && (m_oInfoRequest.m_oBody.m_vecData.size() > 0))
	{
		l_oRespond.m_oBody.m_strXM = m_oInfoRequest.m_oBody.m_vecData[0].m_strxm;
		l_oRespond.m_oBody.m_strSFZH = m_oInfoRequest.m_oBody.m_vecData[0].m_strgmsfhm;
		JsonParser::IJsonPtr tmp_spJson = m_pJsonFty->CreateJson();

		PROTOCOL::CGetBJRSFXXRespond::CData data;
		data.m_strSFZHM = m_oInfoRequest.m_oBody.m_vecData[0].m_strgmsfhm;
		//data.m_strCPH = m_oInfoRequest.m_oBody.m_vecData[0].m_strgmsfhm;
		//data.m_strRYLB = l_Result->GetValue("dept_name");
		//data.m_strRYXL = l_Result->GetValue("remark");
		data.m_strXM = m_oInfoRequest.m_oBody.m_vecData[0].m_strxm;
		data.m_strXB = m_oInfoRequest.m_oBody.m_vecData[0].m_strxb;

		data.m_strCSRQ = m_oInfoRequest.m_oBody.m_vecData[0].m_strcsrq;
		data.m_strMZ = m_oInfoRequest.m_oBody.m_vecData[0].m_strmz;
		//data.m_strJGSSXDM = m_oInfoRequest.m_oBody.m_vecData[0].m_strhjdzBzdzbm;
		//data.m_strJGGJDQDM = m_oInfoRequest.m_oBody.m_vecData[0].m_strhjdzPcsdm;
		data.m_strHJDQH = m_oInfoRequest.m_oBody.m_vecData[0].m_strhjdzQhnxxdz;
		if (m_oInfoRequest.m_oBody.m_vecData[0].m_strhjdzBzdzbm.empty())
		{
			data.m_strHJDQHDM = m_oInfoRequest.m_oBody.m_vecData[0].m_strAddrDO.m_strquId;
		}
		else
		{
			data.m_strHJDQHDM = m_oInfoRequest.m_oBody.m_vecData[0].m_strhjdzBzdzbm;
		}
		data.m_strHJDZ = m_oInfoRequest.m_oBody.m_vecData[0].m_strhjdzDz;
		data.m_strHJDGAJGDM = m_oInfoRequest.m_oBody.m_vecData[0].m_strhjdzPcsdm;
		data.m_strHJDGAJG = m_oInfoRequest.m_oBody.m_vecData[0].m_strhjdzPcs;
		//data.m_strXZDGAJG = l_Result->GetValue("remark");
		//data.m_strXZDGAJGDM = m_oInfoRequest.m_oBody.m_vecData[0].m_strbm;
		//data.m_strXZDQH = l_Result->GetValue("dept_name_path");

		data.m_strXZDQHDM = m_oInfoRequest.m_oBody.m_vecData[0].m_strAddrDO.m_strquId;
		data.m_strXZDDZ = m_oInfoRequest.m_oBody.m_vecData[0].m_strxzzDz;
		data.m_strSJHM = m_oInfoRequest.m_oBody.m_vecData[0].m_strlxdh;
		//data.m_strHJBG = l_Result->GetValue("remark");
		//data.m_vecHJBG = m_oInfoRequest.m_oBody.m_vecData[0].m_strbm;
		data.m_strZP = m_oInfoRequest.m_oBody.m_vecData[0].m_strzpUrl;

		l_oRespond.m_oBody.m_vecData.push_back(data);
	}
	std::string l_strMessage = l_oRespond.ToString(ICCGetIJsonFactory()->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:%s", l_strMessage.c_str());
}

bool CBusinessImpl::_ValidAuthInfo(const ICC::PROTOCOL::CAuthInfo& in_oAuthInfo, std::string& out_strErrMsg)  //较验认证信息
{
	//较难姓名是否为空
	if (in_oAuthInfo.m_strXm.empty())
	{
		out_strErrMsg = "xm is empty.";
		return false;
	}

	//较难身份证号是否为空
	if (in_oAuthInfo.m_stSFZH.empty())
	{
		out_strErrMsg = "sczh is empty.";
		return false;
	}

	//较难单位代码是否为空
	if (in_oAuthInfo.m_strDWDM.empty())
	{
		out_strErrMsg = "dwdm is empty.";
		return false;
	}

	//较难单位名称是否为空
	if (in_oAuthInfo.m_strDWMC.empty())
	{
		out_strErrMsg = "dwmc is empty.";
		return false;
	}

	return true;
}

//较验获取报警人身份证号请求消息
bool CBusinessImpl::_ValidGetBJRSFZHReqMsg(const ICC::PROTOCOL::CGetBJRSFZHRequest::CBody& in_oReqest, std::string& out_strErrMsg)
{

	if (!_ValidAuthInfo(in_oReqest.m_oAuthInfo, out_strErrMsg))
	{
		return false;
	}

	if (in_oReqest.m_strPhoneNo.empty())
	{
		out_strErrMsg = "phone_no is empty";
		return false;
	}

	return true;
}

//较验获取报警人身份信息请求消息
bool CBusinessImpl::_ValidGetBJRSFXXReqMsg(const ICC::PROTOCOL::CGetBJRSFXXRequest::CBody& in_oReqest, std::string& out_strErrMsg)
{
	if (!_ValidAuthInfo(in_oReqest.m_oAuthInfo, out_strErrMsg))
	{
		return false;
	}

	if (in_oReqest.m_strBJRSFZH.empty())
	{
		out_strErrMsg = "bjrsfzh is empty";
		return false;
	}

	return true;
}

void CBusinessImpl::InsertIdentiyInfo(PROTOCOL::CGetBJRSFZHIdentiyInfoRequest& bjrInfo)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "insert_icc_t_alarm_bjrxx";
	if ((0 == bjrInfo.m_oBody.m_code.compare("200")) && (bjrInfo.m_oBody.m_vecData.size() > 0))
	{
		l_SqlRequest.param["sfzhm"] = bjrInfo.m_oBody.m_vecData[0].m_strgmsfhm;
		l_SqlRequest.param["xm"] = bjrInfo.m_oBody.m_vecData[0].m_strxm;
		l_SqlRequest.param["xb"] = bjrInfo.m_oBody.m_vecData[0].m_strxb;
		l_SqlRequest.param["csrq"] = bjrInfo.m_oBody.m_vecData[0].m_strcsrq;
		l_SqlRequest.param["mz"] = bjrInfo.m_oBody.m_vecData[0].m_strmz;
		l_SqlRequest.param["jgssxdm"] = bjrInfo.m_oBody.m_vecData[0].m_strhjdzBzdzbm;
		l_SqlRequest.param["hjdqh"] = bjrInfo.m_oBody.m_vecData[0].m_strhjdzQhnxxdz;
		//l_SqlRequest.param["hjdqhdm"] = bjrInfo.m_oBody.m_vecData[0].m_strhjdzBzdzbm;
		if (bjrInfo.m_oBody.m_vecData[0].m_strhjdzBzdzbm.empty())
		{
			l_SqlRequest.param["hjdqhdm"] = bjrInfo.m_oBody.m_vecData[0].m_strAddrDO.m_strquId;
		}
		else
		{
			l_SqlRequest.param["hjdqhdm"] = bjrInfo.m_oBody.m_vecData[0].m_strhjdzBzdzbm;
		}
		l_SqlRequest.param["hjdz"] = bjrInfo.m_oBody.m_vecData[0].m_strhjdzDz;
		l_SqlRequest.param["hjdgajgdm"] = bjrInfo.m_oBody.m_vecData[0].m_strhjdzPcsdm;
		l_SqlRequest.param["hjdgajg"] = bjrInfo.m_oBody.m_vecData[0].m_strhjdzPcs;
		l_SqlRequest.param["xzdqhdm"] = bjrInfo.m_oBody.m_vecData[0].m_strAddrDO.m_strquId;
		l_SqlRequest.param["xzddz"] = bjrInfo.m_oBody.m_vecData[0].m_strxzzDz;
		l_SqlRequest.param["sjhm"] = bjrInfo.m_oBody.m_vecData[0].m_strlxdh;
		l_SqlRequest.param["zp"] = bjrInfo.m_oBody.m_vecData[0].m_strzpUrl;
		l_SqlRequest.param["bzxx"] = bjrInfo.ToString(m_pJsonFty->CreateJson());
	}
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
	}
}

std::string CBusinessImpl::Subreplace(const std::string souceStr, const std::string subStr, const std::string newStr)
{
	std::string desStr = souceStr;
	size_t pos = 0;
	while ((pos = desStr.find(subStr)) != std::string::npos) {
		desStr.replace(pos, subStr.length(), newStr);
	}
	return desStr;
}

std::string CBusinessImpl::GetPostTextByContent(const std::string strSearch)
{
	std::string strResult = "";
	std::string strTemp = "select value from public.icc_t_param where name = 'SearchStrId';;;";
	std::string strSql = Subreplace(strTemp, "SearchStrId", strSearch);
	ICC_LOG_DEBUG(m_pLog, "sql = %s\n", strSql.c_str());
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(strSql);
	do
	{
		ICC_LOG_DEBUG(m_pLog, "GetPostTextByContent , sql = [%s]", l_result->GetSQL().c_str());
		if (!l_result->IsValid())
		{
			ICC_LOG_DEBUG(m_pLog, "ExecQuery Error ,Error Message = [%s]", l_result->GetErrorMsg().c_str());
			break;
		}

		if (l_result->Next())
		{
			strResult = l_result->GetValue("value");
		}
	} while (false);

	return strResult;
}