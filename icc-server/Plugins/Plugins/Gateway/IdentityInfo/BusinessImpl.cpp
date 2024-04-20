#include "Boost.h"
#include "BusinessImpl.h"

#include <sstream>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>

using namespace boost::archive::iterators;

const std::string CODE_SUCCESS = "200";
const std::string CODE_SFXX_SUCCESS = "0";

void CBusinessImpl::OnInit()
{
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(GATEWAY_IDENTIFYINFO_OBSERVER_CENTER);
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();

	//m_pString = ICCGetIStringFactory()->CreateString();
	//m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();

	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);

	//m_pHttpClient = ICCGetIHttpClientFactory()->CreateHttpClient();

	m_pJsonFty = ICCGetIJsonFactory();
}

void CBusinessImpl::OnStart()
{		

	std::string l_strIsUsing = m_pConfig->GetValue("ICC/Plugin/IdentityInfo/IsUsing", "1");
	if (l_strIsUsing != "1")
	{
		ICC_LOG_DEBUG(m_pLog, "there is no need to load the plugin");
		return;
	}	

	m_strServerIP = m_pConfig->GetValue("ICC/Plugin/IdentityInfo/serverip", "");  //服务IP
	m_strServerPort = m_pConfig->GetValue("ICC/Plugin/IdentityInfo/serverport", "80");  //服务端口
	
	m_strToken = m_pConfig->GetValue("ICC/Plugin/IdentityInfo/token", "");  //授权码
	
	m_strSmulateFlag = m_pConfig->GetValue("ICC/Plugin/IdentityInfo/SmulateFlag", "");  //是否模拟

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_bjr_sfzh", OnNotifGetBJRSFZH);//1
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_bjr_sfxx", OnNotifGetBJRSFXX);//4

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


//获取报警人身份证号
void CBusinessImpl::OnNotifGetBJRSFZH(ObserverPattern::INotificationPtr p_pNotify)
{
	if (NULL == p_pNotify.get())
	{
		ICC_LOG_ERROR(m_pLog, "input notify msg is null");
		return;
	}

	std::string tmp_strMessage(p_pNotify->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "receive message: %s", tmp_strMessage.c_str());

	ICC::PROTOCOL::CGetBJRSFZHRequest tmp_oRequest;

	if (!tmp_oRequest.ParseString(tmp_strMessage, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	ICC::PROTOCOL::CGetBJRSFZHRespond tmp_oRespond;
	tmp_oRespond.m_oHeader = tmp_oRequest.m_oHeader;

	do
	{
		if (!_ValidGetBJRSFZHReqMsg(tmp_oRequest.m_oBody, tmp_oRespond.m_oHeader.m_strMsg))
		{
			tmp_oRespond.m_oHeader.m_strResult = "1";
			break;
		}

		if (m_strServerIP.empty())
		{
			tmp_oRespond.m_oHeader.m_strResult = "2";
			tmp_oRespond.m_oHeader.m_strMsg = "not config identityinfo server";
			break;
		}

		std::string tmp_strTarget("/zllcbk/police/zllcbk/ryxxcx/cxsjda");
		std::map<std::string, std::string> tmp_mapHeaders;

		std::string strErrorMessage;

		tmp_mapHeaders.insert(std::make_pair("Content-Type", "application/json"));
		tmp_mapHeaders["PTOKEN"] = m_strToken;
		 
		std::string tmp_strAuthInfo(tmp_oRequest.m_oBody.m_oAuthInfo.ToString(m_pJsonFty->CreateJson()));
		
		if (!_Base64Encode(tmp_strAuthInfo, &(tmp_mapHeaders["INFO"]))) //需要base64的鉴权消息
		{
			tmp_oRespond.m_oHeader.m_strResult = "3";
			tmp_oRespond.m_oHeader.m_strMsg = "encode base64 failed";
			break;
		}

		ICC_LOG_DEBUG(m_pLog, "auth: %s, base64 info message: %s", tmp_strAuthInfo.c_str(),  tmp_mapHeaders["INFO"].c_str());

		
		std::string tmp_strResult;

		//如果配置模拟
		if ("1" == m_strSmulateFlag)
		{
			tmp_strResult = _SmulateSFZH(tmp_oRequest.m_oBody.m_strPhoneNo);
		}
		else
		{

			JsonParser::IJsonPtr tmp_spJson = m_pJsonFty->CreateJson();

			if (NULL == tmp_spJson.get())
			{
				tmp_oRespond.m_oHeader.m_strResult = "4";
				tmp_oRespond.m_oHeader.m_strMsg = "json is null";
				break;
			}

			tmp_spJson->SetNodeValue("/sjhm", tmp_oRequest.m_oBody.m_strPhoneNo);

			std::string tmp_strContent(tmp_spJson->ToString());

			IHttpClientPtr tmp_pHttpClient = ICCGetIHttpClientFactory()->CreateHttpClient();

			if (NULL == tmp_pHttpClient.get())
			{
				tmp_oRespond.m_oHeader.m_strResult = "8";
				tmp_oRespond.m_oHeader.m_strMsg = "http client is null";
				break;
			}

			tmp_strResult = tmp_pHttpClient->PostEx(m_strServerIP, m_strServerPort, tmp_strTarget, tmp_mapHeaders, tmp_strContent, strErrorMessage);
		}
		

		if (tmp_strResult.empty())
		{
			tmp_oRespond.m_oHeader.m_strResult = "5";
			tmp_oRespond.m_oHeader.m_strMsg = strErrorMessage;
			ICC_LOG_ERROR(m_pLog, "%s receive nothing!!!!err[%s]", tmp_strTarget.c_str(), strErrorMessage.c_str());
			break;
		}

		ICC_LOG_DEBUG(m_pLog, "%s receive [%s]", tmp_strTarget.c_str(), tmp_strResult.c_str());

		if (!tmp_oRespond.ParseString(tmp_strResult, m_pJsonFty->CreateJson()))
		{
			tmp_oRespond.m_oHeader.m_strResult = "6";
			tmp_oRespond.m_oHeader.m_strMsg = "parese receive message failed";
			break;
		}

		if (CODE_SUCCESS != tmp_oRespond.m_oBody.m_strCode)
		{
			tmp_oRespond.m_oHeader.m_strResult = "7";
			tmp_oRespond.m_oHeader.m_strMsg = tmp_oRespond.m_oBody.m_strMessage;
			break;
		}

		_InsertSFZXX(tmp_oRequest.m_oBody, tmp_oRespond.m_oBody);

	} while (0);

	tmp_strMessage = tmp_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(tmp_strMessage);

	ICC_LOG_INFO(m_pLog, "respond msg: %s", tmp_strMessage.c_str());
	
}

//获取报警人身份信息
void CBusinessImpl::OnNotifGetBJRSFXX(ObserverPattern::INotificationPtr p_pNotify)
{
	if (NULL == p_pNotify.get())
	{
		ICC_LOG_ERROR(m_pLog, "input notify msg is null");
		return;
	}

	std::string tmp_strMessage(p_pNotify->GetMessages());
	ICC_LOG_DEBUG(m_pLog, "receive message: %s", tmp_strMessage.c_str());

	ICC::PROTOCOL::CGetBJRSFXXRequest tmp_oRequest;

	if (!tmp_oRequest.ParseString(tmp_strMessage, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	ICC::PROTOCOL::CGetBJRSFXXRespond tmp_oRespond;
	tmp_oRespond.m_oHeader = tmp_oRequest.m_oHeader;

	do
	{
		if (!_ValidGetBJRSFXXReqMsg(tmp_oRequest.m_oBody, tmp_oRespond.m_oHeader.m_strMsg))
		{
			tmp_oRespond.m_oHeader.m_strResult = "1";
			break;
		}

		if (m_strServerIP.empty())
		{
			tmp_oRespond.m_oHeader.m_strResult = "2";
			tmp_oRespond.m_oHeader.m_strMsg = "not config identityinfo server";
			break;
		}

		std::string tmp_strTarget("/zllcbk/police/zllcbk/ryxxcx/ryjcxx");
		std::map<std::string, std::string> tmp_mapHeaders;
		
		std::string strErrorMessage;
		
		tmp_mapHeaders.insert(std::make_pair("Content-Type", "application/json"));
		tmp_mapHeaders["PTOKEN"] = m_strToken;
		
		std::string tmp_strAuthInfo(tmp_oRequest.m_oBody.m_oAuthInfo.ToString(m_pJsonFty->CreateJson()));

		if (!_Base64Encode(tmp_strAuthInfo, &(tmp_mapHeaders["INFO"]))) //需要base64的鉴权消息
		{
			tmp_oRespond.m_oHeader.m_strResult = "3";
			tmp_oRespond.m_oHeader.m_strMsg = "encode base64 failed";
			break;
		}

		ICC_LOG_DEBUG(m_pLog, "info auth: %s, base64 message: %s", tmp_strAuthInfo.c_str(), tmp_mapHeaders["INFO"].c_str());

		std::string tmp_strResult;

		//如果配置模拟
		if ("1" == m_strSmulateFlag)
		{
			
			tmp_strResult = _SmulateSFXX(tmp_oRequest.m_oBody.m_strBJRSFZH);
		}
		else
		{
			JsonParser::IJsonPtr tmp_spJson = m_pJsonFty->CreateJson();

			if (NULL == tmp_spJson.get())
			{
				tmp_oRespond.m_oHeader.m_strResult = "4";
				tmp_oRespond.m_oHeader.m_strMsg = "json is null";
				break;
			}

			tmp_spJson->SetNodeValue("/zjhm", tmp_oRequest.m_oBody.m_strBJRSFZH);
			tmp_spJson->SetNodeValue("/type", "01");
			std::string tmp_strContent(tmp_spJson->ToString());

			IHttpClientPtr tmp_pHttpClient = ICCGetIHttpClientFactory()->CreateHttpClient();

			if (NULL == tmp_pHttpClient.get())
			{
				tmp_oRespond.m_oHeader.m_strResult = "8";
				tmp_oRespond.m_oHeader.m_strMsg = "http client is null";
				break;
			}

			tmp_strResult = tmp_pHttpClient->PostEx(m_strServerIP, m_strServerPort, tmp_strTarget, tmp_mapHeaders, tmp_strContent, strErrorMessage);
		}
		
		if (tmp_strResult.empty())
		{
			tmp_oRespond.m_oHeader.m_strResult = "5";
			tmp_oRespond.m_oHeader.m_strMsg = strErrorMessage;
			ICC_LOG_ERROR(m_pLog, "%s receive nothing!!!!err[%s]", tmp_strTarget.c_str(), strErrorMessage.c_str());
			break;
		}
		
		ICC_LOG_DEBUG(m_pLog, "%s receive [%s]", tmp_strTarget.c_str(), tmp_strResult.c_str());
		
		if (!tmp_oRespond.ParseString(tmp_strResult, m_pJsonFty->CreateJson()))
		{
			tmp_oRespond.m_oHeader.m_strResult = "6";
			tmp_oRespond.m_oHeader.m_strMsg = "parese receive message failed";
			break;
		}

		//返回是否成功
		if (CODE_SFXX_SUCCESS != tmp_oRespond.m_oBody.m_strCode)
		{
			tmp_oRespond.m_oHeader.m_strResult = "7";
			tmp_oRespond.m_oHeader.m_strMsg = tmp_oRespond.m_oBody.m_strMessage;
			break;
		}

		for (unsigned int i = 0; i < tmp_oRespond.m_oBody.m_vecData.size(); ++i)
		{
			_InsertRYXX(tmp_oRespond.m_oBody.m_vecData[i]);
		}
		

	} while (0);

	tmp_strMessage = tmp_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(tmp_strMessage);

	ICC_LOG_INFO(m_pLog, "respond msg: %s", tmp_strMessage.c_str());
}


bool CBusinessImpl::_ValidAuthInfo(const ICC::PROTOCOL::CAuthInfo &in_oAuthInfo, std::string& out_strErrMsg)  //较验认证信息
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

std::string CBusinessImpl::_SmulateSFZH(const std::string& in_strPhoneNo)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_sfzh_record";
	l_SqlRequest.param["phone_no"] = in_strPhoneNo;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return "";
	}
	if (!l_pResult->Next())
	{
		ICC_LOG_DEBUG(m_pLog, "result is null, select_icc_t_sfzh_record have no data");
		return "";
	}

	return l_pResult->GetValue("sfzh");
}

std::string CBusinessImpl::_SmulateSFXX(const std::string& in_strSFZH)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_sfxx_record";
	l_SqlRequest.param["card_id"] = in_strSFZH;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return "";
	}
	if (!l_pResult->Next())
	{
		ICC_LOG_DEBUG(m_pLog, "result is null, select_icc_t_sfxx_record have no data");
		return "";
	}

	return l_pResult->GetValue("sfxx");
}

//较验获取报警人身份证号请求消息
bool CBusinessImpl::_ValidGetBJRSFZHReqMsg(const ICC::PROTOCOL::CGetBJRSFZHRequest::CBody &in_oReqest, std::string& out_strErrMsg)
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
bool CBusinessImpl::_ValidGetBJRSFXXReqMsg(const ICC::PROTOCOL::CGetBJRSFXXRequest::CBody & in_oReqest, std::string& out_strErrMsg)
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

void CBusinessImpl::_InsertSFZXX(const ICC::PROTOCOL::CGetBJRSFZHRequest::CBody& sfzRequestInfo, const ICC::PROTOCOL::CGetBJRSFZHRespond::CBody& sfzResponseInfo)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "insert_icc_t_alarm_sfzxx";
	l_SqlRequest.param["sjhm"] = sfzRequestInfo.m_strPhoneNo;
	l_SqlRequest.param["xm"] = sfzResponseInfo.m_strXM;
	l_SqlRequest.param["zjhm"] = sfzResponseInfo.m_strSFZH;
	l_SqlRequest.param["lys"] = sfzResponseInfo.m_strLYS;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());		
	}
	
}

void CBusinessImpl::_InsertRYXX(const ICC::PROTOCOL::CGetBJRSFXXRespond::CData& bjrInfo)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "insert_icc_t_alarm_bjrxx";
	l_SqlRequest.param["sfzhm"] = bjrInfo.m_strSFZHM;
	l_SqlRequest.param["biaoqian"] = bjrInfo.m_strBiaoQian;
	l_SqlRequest.param["zjlx"] = bjrInfo.m_strZJLX;
	l_SqlRequest.param["cph"] = bjrInfo.m_strCPH;
	l_SqlRequest.param["rylb"] = bjrInfo.m_strRYLB;
	l_SqlRequest.param["ryxl"] = bjrInfo.m_strRYXL;
	l_SqlRequest.param["xm"] = bjrInfo.m_strXM;
	l_SqlRequest.param["xb"] = bjrInfo.m_strXB;
	l_SqlRequest.param["csrq"] = bjrInfo.m_strCSRQ;
	l_SqlRequest.param["mz"] = bjrInfo.m_strMZ;
	l_SqlRequest.param["jgssxdm"] = bjrInfo.m_strJGSSXDM;
	l_SqlRequest.param["jggjdqdm"] = bjrInfo.m_strJGGJDQDM;
	l_SqlRequest.param["hjdqh"] = bjrInfo.m_strHJDQH;
	l_SqlRequest.param["hjdqhdm"] = bjrInfo.m_strHJDQHDM;
	l_SqlRequest.param["hjdz"] = bjrInfo.m_strHJDZ;
	l_SqlRequest.param["hjdgajgdm"] = bjrInfo.m_strHJDGAJGDM;
	l_SqlRequest.param["hjdgajg"] = bjrInfo.m_strHJDGAJG;
	l_SqlRequest.param["xzdgajg"] = bjrInfo.m_strXZDGAJG;
	l_SqlRequest.param["xzdgajgdm"] = bjrInfo.m_strXZDGAJGDM;
	l_SqlRequest.param["xzdqh"] = bjrInfo.m_strXZDQH;
	l_SqlRequest.param["xzdqhdm"] = bjrInfo.m_strXZDQHDM;
	l_SqlRequest.param["xzddz"] = bjrInfo.m_strXZDDZ;
	l_SqlRequest.param["sjhm"] = bjrInfo.m_strSJHM;
	l_SqlRequest.param["hujibiangeng"] = bjrInfo.m_strHJBG;
	l_SqlRequest.param["zp"] = bjrInfo.m_strZP;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, true);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
	}
}