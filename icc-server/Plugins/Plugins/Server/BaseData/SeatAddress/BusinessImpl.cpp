#include "Boost.h"
#include "BusinessImpl.h"
#include <thread>

#define LOAD_WAITTIME 1000

#define SEAT_ADDRESS ("SeatAddress")

namespace SET_TYPE
{
	const std::string ADD_TYPE = "1";   //新增
	const std::string UPDATE_TYPE = "2";  //更新
	const std::string DELETE_TYPE = "3";  //删除
}

CBusinessImpl::CBusinessImpl()
{
}

CBusinessImpl::~CBusinessImpl()
{
}
void CBusinessImpl::OnInit()
{
	printf("OnInit enter! plugin = %s", MODULE_NAME);

	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(BASEDATA_OBSERVER_CENTER);
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pIDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pJsonFac = ICCGetIJsonFactory();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();

	printf("OnInit complete! plugin = %s", MODULE_NAME);
}

void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s", MODULE_NAME);	

	while (!_LoadSeatAddressFromDB())
	{
		ICC_LOG_ERROR(m_pLog, "plugin basedata.SeatAddress failed to load SeatAddress info");
		std::this_thread::sleep_for(std::chrono::milliseconds(LOAD_WAITTIME));
	}

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_seat_address_request", OnCNotifSetSeatAddressRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_seat_address_request", OnCNotifGetSeatAddressRequest);	

	ICC_LOG_INFO(m_pLog, "plugin basedata. seataddress start success");

	printf("OnStart complete! plugin = %s", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_INFO(m_pLog, "keyword stop success");
}

void CBusinessImpl::OnDestroy()
{
}


bool CBusinessImpl::_LoadSeatAddressFromDB()
{
	std::vector<ICC::PROTOCOL::CSeatAddressInfo> tmp_vecSeatAddress;
	if (!_QueryAllSeatAddress(tmp_vecSeatAddress))
	{
		ICC_LOG_ERROR(m_pLog, "_QueryAll Seat Address failed!!!");
		return false;
	}

	if (tmp_vecSeatAddress.empty())
	{
		ICC_LOG_INFO(m_pLog, "Seat Address size is 0!!!");
		return true;
	}

	std::map<std::string, std::string> tmp_mapSeatAddress;
	int iCount = tmp_vecSeatAddress.size();
	for (int i = 0; i < iCount; ++i)
	{
		tmp_mapSeatAddress.insert(std::make_pair(tmp_vecSeatAddress[i].m_strSeatNo, tmp_vecSeatAddress[i].ToString(m_pJsonFac->CreateJson())));
	}

	m_pRedisClient->Del(SEAT_ADDRESS);

	return m_pRedisClient->HMSet(SEAT_ADDRESS, tmp_mapSeatAddress);
}

bool CBusinessImpl::_ExistSeatAddressBySeatno(const std::string& strSeatNo)
{
	DataBase::SQLRequest l_oSeatAddrReq;
	l_oSeatAddrReq.sql_id = "query_icc_t_seat_address_by_seatno";
	l_oSeatAddrReq.param["seat_no"] = strSeatNo;
	DataBase::IResultSetPtr l_pRSet = m_pIDBConn->Exec(l_oSeatAddrReq, false);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());

	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error Error Message :[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}

	int l_iSize = l_pRSet->RecordSize();
	if (l_iSize > 0)
	{
		return true;
	}
	return false;
}

bool CBusinessImpl::_ValidSetSeatAddress(const PROTOCOL::CSeatAddressSetRequest& in_oRequest, std::string& out_strErrMsg)
{
	//设置类型为空
	if (in_oRequest.m_oBody.m_strSetType.empty())
	{
		out_strErrMsg = "set type is empty";
		return false;
	}

	//如果是删除类型，不用较难后续的参数
	if (SET_TYPE::DELETE_TYPE == in_oRequest.m_oBody.m_strSetType)
	{
		return true;
	}

	if (in_oRequest.m_oBody.m_strAddress.empty())
	{
		out_strErrMsg = "address is empty";
		return false;
	}

	return true;
}

void CBusinessImpl::OnCNotifSetSeatAddressRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string strMessage = p_pNotify->GetMessages();

	/*std::string strMessage = "{\
		\"body\": {\
		\"set_type\": \"1\",\
			\"address\" : \"55555\"\
	},\
		\"header\" : {\
			\"seat_no\": \"12138\",\
				\"request_id\" : \"2de6fa3b-09e4-419f-8635-2263e0455d3b\"\
		}\
}\
		";*/

	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", strMessage.c_str());

	PROTOCOL::CSeatAddressSetRequest l_oSetRequest;
	JsonParser::IJsonPtr l_IJson = m_pJsonFac->CreateJson();
	if (!l_oSetRequest.ParseString(strMessage, l_IJson))
	{
		ICC_LOG_ERROR(m_pLog, "parse set_key_word_info_request failed:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	PROTOCOL::CHeaderEx l_oSetRespond;
	
	l_oSetRespond = l_oSetRequest.m_oHeader;

	std::string l_strGuid;
	bool tmp_bSuccess = false;
	do
	{
		//较验参数失败，不做后续处理，进入返回处理
		if (!_ValidSetSeatAddress(l_oSetRequest, l_oSetRespond.m_strMsg))
		{
			l_oSetRespond.m_strResult = "1"; //返回失败
			break;
		}

		DataBase::SQLRequest l_oSeatAddrReq;
		l_oSeatAddrReq.sql_id = "delete_icc_t_seat_address";

		l_oSeatAddrReq.param["seat_no"] = l_oSetRequest.m_oHeader.m_strSeatNo;

		PROTOCOL::CSeatAddressInfo tmp_oInfo;
		//加入地址库
		if (SET_TYPE::ADD_TYPE == l_oSetRequest.m_oBody.m_strSetType)
		{
			tmp_oInfo.m_strGuid =  m_pString->CreateGuid();

			l_oSeatAddrReq.sql_id = "insert_icc_t_seat_address";
			l_oSeatAddrReq.param["guid"] = tmp_oInfo.m_strGuid;
			l_oSeatAddrReq.param["address"] = l_oSetRequest.m_oBody.m_strAddress;
			l_oSeatAddrReq.param["create_time"] = m_pDateTime->CurrentDateTimeStr();

		}
		else if (SET_TYPE::UPDATE_TYPE == l_oSetRequest.m_oBody.m_strSetType)
		{
			std::string tmp_strValue;
			if (!m_pRedisClient->HGet(SEAT_ADDRESS, l_oSetRequest.m_oHeader.m_strSeatNo, tmp_strValue))
			{
				ICC_LOG_ERROR(m_pLog, "get from redis failed.");
				l_oSetRespond.m_strResult = "3"; //返回失败
				l_oSetRespond.m_strMsg = "operate redis value failed";
				break;
			}

			ICC_LOG_DEBUG(m_pLog, "older seat value: %s", tmp_strValue.c_str());

			if (!tmp_oInfo.ParseString(tmp_strValue, m_pJsonFac->CreateJson()))
			{
				ICC_LOG_ERROR(m_pLog, "parse redis value failed, value: %s", tmp_strValue.c_str());
				l_oSetRespond.m_strResult = "4"; //返回失败
				l_oSetRespond.m_strMsg = "parse redis value failed";
				break;
			}

			l_oSeatAddrReq.sql_id = "update_icc_t_seat_address";
			l_oSeatAddrReq.param["address"] = l_oSetRequest.m_oBody.m_strAddress;
			l_oSeatAddrReq.param["update_time"] = m_pDateTime->CurrentDateTimeStr();
		}
		
		l_strGuid = m_pIDBConn->BeginTransaction();
		
		if (!_ExecSql(l_oSeatAddrReq, l_strGuid))
		{
			if (SET_TYPE::ADD_TYPE == l_oSetRequest.m_oBody.m_strSetType)
			{
				if (_ExistSeatAddressBySeatno(l_oSetRequest.m_oHeader.m_strSeatNo))
				{
					m_pIDBConn->Commit(l_strGuid);
					l_strGuid = m_pIDBConn->BeginTransaction();

					DataBase::SQLRequest l_oSeatAddrReqEx;
					l_oSeatAddrReqEx.sql_id = "update_icc_t_seat_address";
					l_oSeatAddrReqEx.param["seat_no"] = l_oSetRequest.m_oHeader.m_strSeatNo;
					l_oSeatAddrReqEx.param["address"] = l_oSetRequest.m_oBody.m_strAddress;
					l_oSeatAddrReqEx.param["update_time"] = m_pDateTime->CurrentDateTimeStr();
					if (!_ExecSql(l_oSeatAddrReqEx, l_strGuid))
					{
						l_oSetRespond.m_strResult = "2"; //返回失败
						l_oSetRespond.m_strMsg = "execute " + l_oSeatAddrReqEx.sql_id + " failed";
						break;
					}
				}
				else
				{
					l_oSetRespond.m_strResult = "2"; //返回失败
					l_oSetRespond.m_strMsg = "execute " + l_oSeatAddrReq.sql_id + " failed";
					break;
				}
			}
			else
			{
				l_oSetRespond.m_strResult = "2"; //返回失败
				l_oSetRespond.m_strMsg = "execute " + l_oSeatAddrReq.sql_id + " failed";
				break;
			}			
		}

		//如果不为删除
		if (SET_TYPE::DELETE_TYPE == l_oSetRequest.m_oBody.m_strSetType)
		{
			tmp_bSuccess = m_pRedisClient->HDel(SEAT_ADDRESS, l_oSetRequest.m_oHeader.m_strSeatNo);
		}
		else
		{
			tmp_oInfo.m_strSeatNo = l_oSetRequest.m_oHeader.m_strSeatNo;
			tmp_oInfo.m_strAddress = l_oSetRequest.m_oBody.m_strAddress;
			
			std::string tmp_strSeatAddress(tmp_oInfo.ToString(m_pJsonFac->CreateJson()));

			if (!tmp_strSeatAddress.empty())
			{
				tmp_bSuccess = m_pRedisClient->HSet(SEAT_ADDRESS, l_oSetRequest.m_oHeader.m_strSeatNo, tmp_strSeatAddress);
			}
			else
			{
				l_oSetRespond.m_strResult = "5"; //返回失败
				l_oSetRespond.m_strMsg = "save redis value is empty";
				break;
			}
		}
		
		if (!tmp_bSuccess)
		{
			l_oSetRespond.m_strResult = "3"; //返回失败
			l_oSetRespond.m_strMsg = "operate redis failed";
		}
		
		m_pIDBConn->Commit(l_strGuid);
		
	} while (0);


	if ((!tmp_bSuccess) && (!l_strGuid.empty()))
	{
		m_pIDBConn->Rollback(l_strGuid);
	}
	
	JsonParser::IJsonPtr tmp_spJson = m_pJsonFac->CreateJson();

	if (NULL == tmp_spJson.get())
	{
		ICC_LOG_ERROR(m_pLog, "Create json failed.");
		return;
	}

	l_oSetRespond.SaveTo(tmp_spJson);
	std::string l_strMessage(tmp_spJson->ToString());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send:[%s]", l_strMessage.c_str());

}

void CBusinessImpl::OnCNotifGetSeatAddressRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CHeaderEx l_oRequest;
	JsonParser::IJsonPtr l_IJson = m_pJsonFac->CreateJson();
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), l_IJson))
	{
		ICC_LOG_ERROR(m_pLog, "parse query keyword failed:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	PROTOCOL::CSeatAddressGetRespond l_oRespond;

	l_oRespond.m_oHeader = l_oRequest;
	
	do
	{
		//是否存在
		if (!m_pRedisClient->HExists(SEAT_ADDRESS, l_oRequest.m_strSeatNo))
		{
			if (_ExistSeatAddressBySeatno(l_oRequest.m_strSeatNo))
			{
				if (!_LoadSeatAddressFromDB())
				{
					ICC_LOG_DEBUG(m_pLog, "seat no: %s not set address", l_oRequest.m_strSeatNo.c_str());
					break;
				}
				
				if (!m_pRedisClient->HExists(SEAT_ADDRESS, l_oRequest.m_strSeatNo))
				{
					ICC_LOG_DEBUG(m_pLog, "seat no: %s not set address", l_oRequest.m_strSeatNo.c_str());
					break;
				}
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "seat no: %s not set address", l_oRequest.m_strSeatNo.c_str());
				break;
			}
			
		}

		//获取对应的值
		std::string tmp_strValue;
		if (!m_pRedisClient->HGet(SEAT_ADDRESS, l_oRequest.m_strSeatNo, tmp_strValue))
		{
			ICC_LOG_ERROR(m_pLog, "get from redis failed.");
			l_oRespond.m_oHeader.m_strResult = "3"; //返回失败
			l_oRespond.m_oHeader.m_strMsg = "operate redis value failed";
			break;
		}

		//如果为空不需要后面的处理
		if (tmp_strValue.empty())
		{
			ICC_LOG_DEBUG(m_pLog, "seat no: %s not set address", l_oRequest.m_strSeatNo.c_str());
			break;
		}

		ICC_LOG_DEBUG(m_pLog, "seat address: %s", tmp_strValue.c_str());
		
		if (!l_oRespond.m_oBody.ParseString(tmp_strValue, m_pJsonFac->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "parse redis value failed, value: %s", tmp_strValue.c_str());
			l_oRespond.m_oHeader.m_strResult = "4"; //返回失败
			l_oRespond.m_oHeader.m_strMsg = "parse redis value failed";
			break;
		}

	} while (0);

	std::string l_strMessage = l_oRespond.ToString(m_pJsonFac->CreateJson());
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

bool CBusinessImpl::_ExecSql(DataBase::SQLRequest in_oSQLReq, const std::string& in_strTransGuid)
{
	DataBase::IResultSetPtr l_pRSet = m_pIDBConn->Exec(in_oSQLReq, false, in_strTransGuid);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
	
	if (!l_pRSet)
	{
		ICC_LOG_ERROR(m_pLog, "Error:[%s]", "l_pRSet is null");
		return false;
	}

	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "Error:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}

	return true;
}

bool CBusinessImpl::_QueryAllSeatAddress(std::vector<ICC::PROTOCOL::CSeatAddressInfo>& vecAllSeatAddress)
{
	DataBase::SQLRequest l_oKeyDeptSQLReq;
	l_oKeyDeptSQLReq.sql_id = "select_icc_t_seat_address";
	ICC_LOG_DEBUG(m_pLog, "_LoadSeatAddressFromDB Begin");
	DataBase::IResultSetPtr l_result = m_pIDBConn->Exec(l_oKeyDeptSQLReq);
	ICC_LOG_INFO(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
	
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error Error Message :[%s]", l_result->GetErrorMsg().c_str());
		return false;
	}

	int l_iSize = 0;
	while (l_result->Next())
	{
		PROTOCOL::CSeatAddressInfo tmp_oSeatAddr;

		tmp_oSeatAddr.m_strGuid = l_result->GetValue("guid");
		tmp_oSeatAddr.m_strSeatNo = l_result->GetValue("seat_no");
		tmp_oSeatAddr.m_strAddress = l_result->GetValue("address");

		vecAllSeatAddress.push_back(tmp_oSeatAddr);
	}
	ICC_LOG_DEBUG(m_pLog, "_LoadKeyWordFromDB Success size[%d]", vecAllSeatAddress.size());

	return true;
}
