#include "Boost.h"
#include "BusinessImpl.h"
#include "ResultSetCounter.h"

#define READFOUNDERTIMESTAMP "ReadFounderTimeStamp"
#define READICCTIMESTAMP     "ReadICCTimestamp"
#define SOURCE_FOUNDER "Founder"
#define MODE_NAME "mode_name"

CBusinessImpl* g_pBusinessImpl = nullptr;

CBusinessImpl::CBusinessImpl()
{
	g_pBusinessImpl = this;
	m_pReadDataThread = nullptr;
	m_llCheckInterval = 1000;
}

CBusinessImpl::~CBusinessImpl()
{
}

void CBusinessImpl::OnInit()
{
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(GATEWAY_FOUNDER_OBSERVER_CENTER);
	m_pJsonFty = ICCGetIJsonFactory();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pString = ICCGetIStringFactory()->CreateString();	
	m_pHelpTool = ICCGetHelpToolFactory()->CreateHelpTool();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
	m_pSQLBD = ICCGetISqlBuilderFactory()->CreateSqlBuilder();
	m_pSqlReqeustFactory = ICCGetISqlRequestFactory();

	m_pOracleClient = std::make_shared<COracleClient>();
	
}

void CBusinessImpl::OnStart()
{
	ICC_LOG_DEBUG(m_pLog, "%s plugin start begin.", MODULE_NAME);

	m_strInvalidAlarm = m_pConfig->GetValue("ICC/Plugin/Founder/InvalidAlarm", "1");  //无效警情是否推送给方正
	m_strSelfDispose= m_pConfig->GetValue("ICC/Plugin/Founder/SelfDispose", "1");	//已自处警情是否推送给方正
	m_strFpsIp = m_pConfig->GetValue("ICC/Plugin/Founder/FpsIp", "80.75.112.92");     //FPS的IP地址
	m_strFpsPort = m_pConfig->GetValue("ICC/Plugin/Founder/FpsPort", "28080");		//FPS的端口号
	m_strDelayTime= m_pConfig->GetValue("ICC/Plugin/Founder/DelayTime", "-3");
	ICC_LOG_DEBUG(m_pLog, "Read Founder config InvalidAlarm:[%s],SelfDispose:[%s],FpsIp:[%s],Fpsport:[%s]", m_strInvalidAlarm.c_str(), m_strSelfDispose.c_str(), m_strFpsIp.c_str(), m_strFpsPort.c_str());
	std::string l_strIsUsing = m_pConfig->GetValue("ICC/Plugin/Founder/IsUsing", "0");
	if (l_strIsUsing != "1")
	{
		ICC_LOG_DEBUG(m_pLog, "there is no need to load the plugin");
		return;
	}

	std::string strCheckInterval = m_pConfig->GetValue("ICC/Plugin/Founder/checkinterval", "3");
	if (strCheckInterval.empty())
	{
		strCheckInterval = "3";
	}
	int iCheckInterval = std::stoi(strCheckInterval);
	if (iCheckInterval < 1 || iCheckInterval > 3600 * 100)
	{
		iCheckInterval = 3;
	}

	m_llCheckInterval = iCheckInterval * 1000;

	m_strTableModeName = m_pConfig->GetValue("ICC/Plugin/Founder/tablemodename", "TEST01");

	 if (m_pOracleClient == nullptr)
	 {
		 ICC_LOG_DEBUG(m_pLog, "malloc oracle client failed!!!!");
		 return;
	 }

	if (!m_pOracleClient->Init(m_pLog, m_pString, m_pConfig, m_pHelpTool, m_pSQLBD, m_pSqlReqeustFactory))
	{
		ICC_LOG_DEBUG(m_pLog, "init oracle client failed!!!!");
		return;
	}	

	//_Test();

	m_pReadDataThread = boost::make_shared<boost::thread>(boost::bind(&CBusinessImpl::_ReadDataThread, this));
	if (m_pReadDataThread == nullptr)
	{
		ICC_LOG_DEBUG(m_pLog, "create thread failed!!!!");
		return;
	}
	//_Test();	

	// 注册观察者
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_sync", OnReceiveAlarmSyncNotify);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_process_sync", OnReceiveProcessSyncNotify);

	ICC_LOG_DEBUG(m_pLog, "%s plugin start complete.", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	if (m_pOracleClient)
	{
		m_pOracleClient->UnInit();
	}

	ICC_LOG_DEBUG(m_pLog, "%s plugin stoped.", MODULE_NAME);
}

void CBusinessImpl::OnDestroy()
{

}

/*****************************************************************
 * 
 ****************************************************************/

bool CBusinessImpl::_ExistAlarmData(const std::string& strAlarmId, bool& bExist)
{
	bExist = false;
	DataBase::SQLRequest tmp_oSQLRequst;
	tmp_oSQLRequst.sql_id = "oracle_judge_alarm_exist";
	tmp_oSQLRequst.param["mode_name"] = m_strTableModeName;
	tmp_oSQLRequst.param["jjdbh"] = strAlarmId;	
	{
		ResultSetPtr pSet = m_pOracleClient->Exec(tmp_oSQLRequst);
		if (!pSet->IsValid())
		{
			ICC_LOG_WARNING(m_pLog, "resultset is invalid! sql:%s", pSet->GetSQL().c_str());
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", pSet->GetSQL().c_str());

		std::vector<std::string> vecFiledNames = pSet->GetFieldNames();

		int iCounter = CResultSetCounter::Instance()->GetCounter();
		ICC_LOG_DEBUG(m_pLog, "enter resultset counter is %d", iCounter);

		do
		{
			if (pSet->Next())
			{
				std::string strCount = pSet->GetValue("COUNT");
				if (strCount.empty())
				{
					ICC_LOG_ERROR(m_pLog, "not find field:COUNT");
					return false;
				}

				if (std::stoi(strCount) > 0)
				{
					bExist = true;
					break;
				}
			}
		} while (false);
			
		
	}
	return true;
}

bool CBusinessImpl::_WriteAlarmDataToFounder(std::vector<std::map<std::string, std::string>>& datas)
{
	unsigned int iCount = datas.size();
	std::string strState;
	std::string carNumber;
	std::string carType;
	DataBase::SQLRequest tmp_oSQLRequst;
	for (unsigned int i = 0; i < iCount; ++i)
	{
		//VCS自接警以及门市报警不推中间库，根据行政区划代码过滤
		if (datas[i]["xzqhdm"] != "510500" && datas[i]["xzqhdm"] != "510521" && datas[i]["xzqhdm"] != "510522" && datas[i]["xzqhdm"] != "510524" && datas[i]["xzqhdm"] != "510525"&&datas[i]["jjfs"]=="1")
		{
			continue;
		}

		//VCS自接警不推中间库，根据接警方式字段过滤
		if(datas[i]["jjfs"]!="3"&&datas[i]["jjfs"]!="icc")
		{
			continue;
		}

		//方正涉案车辆只能有一辆，故推送第一条满足接警单编号的车辆信息即可
		tmp_oSQLRequst.sql_id = "query_icc_t_jqclb_by_alarm_id_only_one_car";
		tmp_oSQLRequst.param["alarm_id"] = datas[i]["jjdbh"];
		DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(tmp_oSQLRequst);
		if (!l_pResult->IsValid())
		{
			ICC_LOG_WARNING(m_pLog, "resultset is invalid! sql:%s", l_pResult->GetSQL().c_str());
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", l_pResult->GetSQL().c_str());
		if (l_pResult->Next())
		{
			carNumber = l_pResult->GetValue("cph");
			carType = l_pResult->GetValue("zldm");
		}
		std::map<std::string, std::string>::const_iterator itr;
		itr = datas[i].find("jjdbh");
		if (itr == datas[i].end())
		{
			return false;
		}
		std::string strAlarmId = itr->second;
		if (strAlarmId.empty())
		{
			return false;
		}

		bool bExist = false;
		if (!_ExistAlarmData(strAlarmId, bExist))
		{
			return false;
		}

		if (!bExist)
		{
			tmp_oSQLRequst.sql_id = "oracle_insert_founder_t_jjdb";
		}
		else
		{
			tmp_oSQLRequst.sql_id = "oracle_update_founder_t_jjdb";
		}

		tmp_oSQLRequst.param[MODE_NAME] = m_strTableModeName;
		for (itr = datas[i].begin(); itr != datas[i].end(); ++itr)
		{
			std::string strTmp;
			if (itr->first == "bjsj" || itr->first == "jjsj" || itr->first == "jjwcsj" || itr->first == "rksj" || itr->first == "gxsj")
			{
				if (strTmp.empty() && itr->first == "rksj")
				{
					strTmp = datas[i]["jjsj"];
				}
				strTmp = _StandardDateTime(itr->second);
				std::string Tmp = FromUTCToLocal(strTmp);
				tmp_oSQLRequst.param[itr->first] = Tmp;
			}
			else
			{
				tmp_oSQLRequst.param[itr->first] = itr->second;
			}
		}
		////关联警单子警单不推方正
		//if (!datas[i]["glzjjdbh"].empty())
		//{
		//	ICC_LOG_DEBUG(m_pLog, "the relation of alarm don't send to founder");
		//	continue;
		//}

		////12345的警情暂时不推方正
		//if (datas[i]["sfbm"]!="1" && datas[i]["sfbm"]!="0")
		//{
		//	ICC_LOG_DEBUG(m_pLog, "alarm come from 12345 don't send to founder，sfbm:[%s]",datas[i]["sfbm"].c_str());
		//	continue;
		//}

		//无效警情推送配置项为0时，无效警情不推送
		if (m_strInvalidAlarm == "0" && datas[i]["wxbs"] == "1")
		{
			ICC_LOG_DEBUG(m_pLog, "the Invalid alarm don't send to founder");
			continue;
		}

		//已自处警情推送配置项为0时，已自处警情不推送
		if (m_strSelfDispose == "0" && datas[i]["jqclztdm"] == "04")
		{
			ICC_LOG_DEBUG(m_pLog, "the Self Dispose of alarm don't send to founder");
			continue;
		}
		tmp_oSQLRequst.param["sjchpzldm"] = carType;
		tmp_oSQLRequst.param["sjcph"] = carNumber;

		//转换为方正的证件代码写入中间库
		if (datas[i]["bjrzjdm"] == "01")
		{
			tmp_oSQLRequst.param["bjrzjdm"] = "111";
		}	 
		if (datas[i]["bjrzjdm"] == "02")
		{	 
			tmp_oSQLRequst.param["bjrzjdm"] = "114";
		}	 
		if (datas[i]["bjrzjdm"] == "03")
		{	 
			tmp_oSQLRequst.param["bjrzjdm"] = "133";
		}	 
		if (datas[i]["bjrzjdm"] == "04")
		{	 
			tmp_oSQLRequst.param["bjrzjdm"] ="335";
		}	 
		if (datas[i]["bjrzjdm"] == "05")
		{	 
			tmp_oSQLRequst.param["bjrzjdm"] = "414";
		}	 
		if (datas[i]["bjrzjdm"] == "06")
		{
			tmp_oSQLRequst.param["bjrzjdm"] == "513";
		}
		ICC_LOG_DEBUG(m_pLog, "sjchpzldm:[%s],sjchp:[%s],bjrzjdm:[%s]", tmp_oSQLRequst.param["sjchpzldm"].c_str(), tmp_oSQLRequst.param["sjchp"].c_str(), tmp_oSQLRequst.param["bjrzjdm"].c_str());
		std::string strRecordId = datas[i]["jjlyh"];
		std::string strRecordUrl=m_pString->Format("https://%s:%s/commandcenter-fps-service/fps-proxy/download/v2/audio/%s", m_strFpsIp.c_str(),m_strFpsPort.c_str(),strRecordId.c_str());
		tmp_oSQLRequst.param["jjlyh"] = strRecordUrl;
		ICC_LOG_DEBUG(m_pLog, strRecordUrl.c_str());
		ResultSetPtr pSet = m_pOracleClient->Exec(tmp_oSQLRequst);
		ICC_LOG_DEBUG(m_pLog, "exec sql! sql:%s", pSet->GetSQL().c_str());
		if (!pSet->IsValid())
		{
			ICC_LOG_DEBUG(m_pLog, "result is invalid");
			return false;
		}		
	}
	return true;
}

bool CBusinessImpl::_ReadAlarmDataFromICC(const std::string& strBeginTime, const std::string& strEndTime, std::vector<std::map<std::string, std::string>>& datas)
{
	std::string BeginTime= FromLocalToUTC(strBeginTime);
	std::string EndTime = FromLocalToUTC(strEndTime);
	DataBase::SQLRequest tmp_oSQLRequst;
	tmp_oSQLRequst.sql_id = "founder_select_icc_t_jjdb_by_updatetime";
	tmp_oSQLRequst.param["begin_time"] = BeginTime;
	tmp_oSQLRequst.param["end_time"] = EndTime;
	tmp_oSQLRequst.param["jjsj_end"] = EndTime;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(tmp_oSQLRequst);
	if (!l_pResult->IsValid())
	{
		ICC_LOG_WARNING(m_pLog, "resultset is invalid! sql:%s", l_pResult->GetSQL().c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", l_pResult->GetSQL().c_str());

	std::vector<std::string> vecFiledNames = l_pResult->GetFieldNames();
	while (l_pResult->Next())
	{
		std::map<std::string, std::string> mapTmps;
		unsigned int iCount = vecFiledNames.size();
		for (unsigned int i = 0; i < iCount; ++i)
		{
			mapTmps.insert(std::make_pair(vecFiledNames[i], l_pResult->GetValue(vecFiledNames[i])));
		}
		datas.push_back(mapTmps);
	}

	return true;
}

bool CBusinessImpl::_SyncAlarmDataFromICCToFounder(const std::string& strBeginTime, const std::string& strEndTime)
{
	ICC_LOG_DEBUG(m_pLog, "read icc alarm data begin!! begintime:%s, endtime:%s", strBeginTime.c_str(), strEndTime.c_str());
	std::vector<std::map<std::string, std::string>> datas;
	if (!_ReadAlarmDataFromICC(strBeginTime, strEndTime, datas))
	{
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "read alarm data from icc complete !! begintime:%s, endtime:%s", strBeginTime.c_str(), strEndTime.c_str());

	if (!_WriteAlarmDataToFounder(datas))
	{
		return false;
	}

	return true;
	
}


/*****************************************************************
 * 
 ****************************************************************/

bool CBusinessImpl::_ReadProcessDataFromICC(const std::string& strBeginTime, const std::string& strEndTime, std::vector<std::map<std::string, std::string>>& datas)
{
	std::string BeginTime = FromLocalToUTC(strBeginTime);
	std::string EndTime = FromLocalToUTC(strEndTime);
	ICC_LOG_DEBUG(m_pLog,"UtcBeginTime:[%s],UtcEndTime:[%s]", BeginTime.c_str(), EndTime.c_str());
	DataBase::SQLRequest tmp_oSQLRequst;
	tmp_oSQLRequst.sql_id = "founder_select_icc_t_pjdb_by_updatetime";
	tmp_oSQLRequst.param["begin_time"] = BeginTime;
	tmp_oSQLRequst.param["end_time"] = EndTime;
	tmp_oSQLRequst.param["jjsj_end"] = EndTime;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(tmp_oSQLRequst);
	if (!l_pResult->IsValid())
	{
		ICC_LOG_WARNING(m_pLog, "resultset is invalid! sql:%s", l_pResult->GetSQL().c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", l_pResult->GetSQL().c_str());

	std::vector<std::string> vecFiledNames = l_pResult->GetFieldNames();
	while (l_pResult->Next())
	{
		std::map<std::string, std::string> mapTmps;
		unsigned int iCount = vecFiledNames.size();
		for (unsigned int i = 0; i < iCount; ++i)
		{
			mapTmps.insert(std::make_pair(vecFiledNames[i], l_pResult->GetValue(vecFiledNames[i])));
		}
		datas.push_back(mapTmps);
	}

	return true;	
}

bool CBusinessImpl::_WriteProcessDataToFounder(std::vector<std::map<std::string, std::string>>& datas)
{
	unsigned int iCount = datas.size();
	for (unsigned int i = 0; i < iCount; ++i)
	{
		////VCS自接警以及门市报警不推中间库,现场bug派警单没有管辖单位，为了调试暂时不过滤
		//if (datas[i]["xzqhdm"] != "510500" && datas[i]["xzqhdm"] != "510521" && datas[i]["xzqhdm"] != "510522" && datas[i]["xzqhdm"] != "510524" && datas[i]["xzqhdm"] != "510525")
		//{
		//	continue;
		//}
		
		//防止方正过来的数据再次回推
		if (datas[i]["pjdwdm"] != "510500000000" && datas[i]["pjdwdm"] != "510521000000" && datas[i]["pjdwdm"] != "510522000000" && datas[i]["pjdwdm"] != "510524000000" && datas[i]["pjdwdm"] != "510525000000")
		{
			continue;
		}
		std::map<std::string, std::string>::const_iterator itr;
		itr = datas[i].find("pjdbh");
		if (itr == datas[i].end())
		{
			ICC_LOG_ERROR(m_pLog, "not find pjdbh!!");
			return false;
		}
		std::string strId = itr->second;
		if (strId.empty())
		{
			ICC_LOG_ERROR(m_pLog, "pjdbh is null!!");
			return false;
		}

		DataBase::SQLRequest tmp_oSQLRequst;
		bool bExist = false;
		if (!_ExistProccessData(strId, bExist))
		{
			ICC_LOG_ERROR(m_pLog, "judge process exist failed!! pjdid:%s", strId.c_str());
			return false;
		}

		if (!bExist)
		{
			tmp_oSQLRequst.sql_id = "oracle_insert_founder_t_pjdb";
		}
		else
		{
			tmp_oSQLRequst.sql_id = "oracle_update_founder_t_pjdb";
		}

		tmp_oSQLRequst.param[MODE_NAME] = m_strTableModeName;
		std::string strTmp;
		std::string strState;
		for (itr = datas[i].begin(); itr != datas[i].end(); ++itr)
		{
			std::string strTmp;
			if (itr->first == "pjsj" || itr->first == "xtzdddsj" || itr->first == "pdqssj" || itr->first == "rksj" || itr->first == "gxsj")
			{
				if (strTmp.empty() && itr->first == "rksj")
				{
					strTmp = datas[i]["pjsj"];
				}
				//icc初始的派警单gxsj不赋值，方正需要
				else if (strTmp.empty() && itr->first == "gxsj")
				{
					strTmp = m_pDateTime->CurrentDateTimeStr();
				}
				else
				{
					strTmp = _StandardDateTime(itr->second);
				}
				std::string Tmp = FromUTCToLocal(strTmp);
				tmp_oSQLRequst.param[itr->first] = Tmp;
			}

			//将icc的01-07状态转换为方正内部所需状态（5、10、15这种）
			else if (itr->first == "jqclztdm")
			{
				if (itr->second == "01")
				{
					strState = "5";
				}
				if (itr->second == "02")
				{
					strState = "10";
				}
				if (itr->second == "05")
				{
					strState = "15";
				}
				if (itr->second == "06")
				{
					strState = "20";
				}
				if (itr->second == "07")
				{
					strState = "30";
				}
				tmp_oSQLRequst.param["CJDZT"] = strState;
				tmp_oSQLRequst.param[itr->first] = itr->second;
				
			}
			else
			{
				tmp_oSQLRequst.param[itr->first] = itr->second;
			}

		}
		//icc内部撤销的派警单状态为17，方正用tbzt字段来表示
		if (datas[i]["jqclztdm"] == "17")
		{
			tmp_oSQLRequst.param["TBZT"] = "1";
		}
		else
		{
			tmp_oSQLRequst.param["TBZT"] = "0";
		}
		//首次派单签收人员为空
		if (datas[i]["qsryxm"].empty())
		{
			tmp_oSQLRequst.param["qsryxm"] = datas[i]["pjyxm"];
		}
		ResultSetPtr pSet = m_pOracleClient->Exec(tmp_oSQLRequst);		
		if (!pSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "exec sql! error:%s sql:%s", pSet->GetErrorMsg().c_str(), pSet->GetSQL().c_str());
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", pSet->GetSQL().c_str());
	}

	return true;
}

bool CBusinessImpl::_SyncProcessDataFromICCToFounder(const std::string& strBeginTime, const std::string& strEndTime)
{
	ICC_LOG_DEBUG(m_pLog, "read icc process data begin!! begintime:%s, endtime:%s", strBeginTime.c_str(), strEndTime.c_str());
	std::vector<std::map<std::string, std::string>> datas;
	if (!_ReadProcessDataFromICC(strBeginTime, strEndTime, datas))
	{
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "read icc process data from icc complete !! count:%d, begintime:%s, endtime:%s", datas.size(), strBeginTime.c_str(), strEndTime.c_str());

	if (!_WriteProcessDataToFounder(datas))
	{
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "write process data to founder from icc complete !! begintime:%s, endtime:%s", strBeginTime.c_str(), strEndTime.c_str());

	return true;
}

bool CBusinessImpl::_SyncDataFromICCToFounder(const std::string& strEndTime)
{
	std::string strBeginTime;

	if (!_GetPreTimestamp(READICCTIMESTAMP, strBeginTime))
	{
		ICC_LOG_DEBUG(m_pLog, "read icc pre timestamp failed!");
		return false;
	}
	//DateTime::CDateTime m_oBeginTime = m_pDateTime->FromString(strBeginstrTime);
	//DateTime::CDateTime l_oBeginTime = m_pDateTime->AddSeconds(m_oBeginTime, -5);
	//std::string strBeginTime = m_pDateTime->ToString(l_oBeginTime);
	ICC_LOG_DEBUG(m_pLog, "sync icc data to founder begin!! begintime:%s, endtime:%s", strBeginTime.c_str(), strEndTime.c_str());
	if (!_SyncAlarmDataFromICCToFounder(strBeginTime, strEndTime))
	{
		return false;
	}	
	
	ICC_LOG_DEBUG(m_pLog, "sync icc alarm data to founder complete !! begintime:%s, endtime:%s", strBeginTime.c_str(), strEndTime.c_str());

	if (!_SyncProcessDataFromICCToFounder(strBeginTime, strEndTime))
	{
		return false;
	}
	
	ICC_LOG_DEBUG(m_pLog, "sync icc process data to founder complete !! begintime:%s, endtime:%s", strBeginTime.c_str(), strEndTime.c_str());
	
	//12345的反馈单数据暂时不推方正
	//if (!_SyncFeedbackDataFromICCToFounder(strBeginTime, strEndTime))
	//{
	//		return false;
	//}

	ICC_LOG_DEBUG(m_pLog, "sync icc feedback data to founder complete !! begintime:%s, endtime:%s", strBeginTime.c_str(), strEndTime.c_str());

	_WriteTimestamp(READICCTIMESTAMP, strEndTime);
	ICC_LOG_DEBUG(m_pLog, "sync icc data end!! begintime:%s, endtime:%s", strBeginTime.c_str(), strEndTime.c_str());	

	return true;
}


/*****************************************************************
 * 
 ****************************************************************/
bool CBusinessImpl::_ExistFeedbackDataInICC(const std::string& strId, const std::string& strUpdateTime, bool& bExist, bool& bUpdate)
{
	bExist = false;
	bUpdate = false;
	std::string UpdateTime=FromLocalToUTC(strUpdateTime);
	DataBase::SQLRequest tmp_oSQLRequst;
	tmp_oSQLRequst.sql_id = "founder_filter_feedback";	
	tmp_oSQLRequst.param["fkdbh"] = strId;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(tmp_oSQLRequst);
	if (!l_pResult->IsValid())
	{
		ICC_LOG_WARNING(m_pLog, "resultset is invalid! error:%s sql:%s", l_pResult->GetErrorMsg().c_str(), l_pResult->GetSQL().c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", l_pResult->GetSQL().c_str());

	do
	{
		if (l_pResult->Next())
		{
			bExist = true;

			std::string  curTime = l_pResult->GetValue("gxsj");
			if (UpdateTime.empty())
			{
				ICC_LOG_DEBUG(m_pLog, "gxsj is empty!!! sql:%s", l_pResult->GetSQL().c_str());
				return false;
			}
			DateTime::CDateTime strPgTime = m_pDateTime->FromString(curTime);
			//防止更新时间出现误差
			m_pDateTime->AddSeconds(strPgTime, -10);
			std::string strTime = m_pDateTime->ToString(strPgTime);
			std::string strTmpPgTime = _StandardDateTime(strTime);
			std::string strTmpOraTime = _StandardDateTime(UpdateTime);
			

			if (strTmpOraTime > strTmpPgTime)
			{
				bUpdate = true;
				break;
			}
		}
		else
		{
			bExist = false;
		}
	} while (false);

	return true;
}

bool CBusinessImpl::_WriteFeedbackDataToICC(std::vector<std::map<std::string, std::string>>& datas)
{
	std::string strProcessId;
	std::string strDispatchDeptName;
	std::string strProcessDeptName;
	unsigned int iCount = datas.size();
	for (unsigned int i = 0; i < iCount; ++i)
	{
		DataBase::SQLRequest tmp_oSQLRequst;
		std::map<std::string, std::string>::const_iterator itr;
		itr = datas[i].find("FKDBH");
		if (itr == datas[i].end())
		{
			return false;
		}

		std::string strId = itr->second;
		if (strId.empty())
		{
			return false;
		}
		
		itr = datas[i].find("GXSJ");
		if (itr == datas[i].end())
		{
			return false;
		}

		std::string strUpdateTime = itr->second;
		bool bExist = false;
		bool bUpdate = false;
		if (!_ExistFeedbackDataInICC(strId, strUpdateTime, bExist, bUpdate))
		{
			return false;
		}

		if (!bExist)
		{
			tmp_oSQLRequst.sql_id = "founder_insert_icc_t_fkdb";
		}
		else if (bUpdate)
		{
			tmp_oSQLRequst.sql_id = "founder_update_icc_t_fkdb";
		}
		else
		{
			continue;
		}
		for (itr = datas[i].begin(); itr != datas[i].end(); ++itr)
		{
			std::string strTmp;
			if (itr->first == "FKSJ" || itr->first == "CJSJ01" || itr->first == "DDXCSJ" || itr->first == "XCCLWBSJ" || itr->first == "JQFSSJ" || itr->first == "rkSJ" || itr->first == "GXSJ")
			{
				strTmp = _StandardDateTime(itr->second);
				std::string Tmp = FromLocalToUTC(strTmp);
				if (!bExist)
				{
					tmp_oSQLRequst.sql_id = "founder_insert_icc_t_fkdb";
					tmp_oSQLRequst.param[m_pString->Lower(itr->first)] = Tmp;
				}
				else if (bUpdate)
				{
					tmp_oSQLRequst.set[m_pString->Lower(itr->first)] = Tmp;
				}
			}
			else
			{
				if (!bExist)
				{
					tmp_oSQLRequst.param[m_pString->Lower(itr->first)] = itr->second;
				}
				else if (bUpdate)
				{
					if (itr->first == "FKDBH")
					{
						tmp_oSQLRequst.param[m_pString->Lower(itr->first)] = itr->second;
					}
					tmp_oSQLRequst.set[m_pString->Lower(itr->first)] = itr->second;
				}
			}
		}
		//tmp_oSQLRequst.param["pjdbh"] = strProcessId;
		DataBase::IResultSetPtr pSet = m_pDBConn->Exec(tmp_oSQLRequst);
		if (!pSet->IsValid())
		{
			ICC_LOG_DEBUG(m_pLog, "exec sql failed! error:%s, sql:%s", pSet->GetErrorMsg().c_str(), pSet->GetSQL().c_str());
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", pSet->GetSQL().c_str());
	}

	return true;
}

bool CBusinessImpl::_ReadFeedbackDataFromOracle(const std::string& strBeginTime, const std::string& strEndTime, std::vector<std::map<std::string, std::string>>& datas)
{
	DataBase::SQLRequest tmp_oSQLRequst;
	tmp_oSQLRequst.sql_id = "oracle_query_feedback";
	tmp_oSQLRequst.param["mode_name"] = m_strTableModeName;
	tmp_oSQLRequst.param["begin_time"] = strBeginTime;
	tmp_oSQLRequst.param["end_time"] = strEndTime;
	{
		ResultSetPtr pSet = m_pOracleClient->Exec(tmp_oSQLRequst);
		if (!pSet->IsValid())
		{
			ICC_LOG_WARNING(m_pLog, "resultset is invalid! error:%s sql:%s", pSet->GetErrorMsg().c_str(), pSet->GetSQL().c_str());
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", pSet->GetSQL().c_str());

		std::vector<std::string> vecFiledNames = pSet->GetFieldNames();

		int iCounter = CResultSetCounter::Instance()->GetCounter();
		ICC_LOG_DEBUG(m_pLog, "enter resultset counter is %d", iCounter);

		while (pSet->Next())
		{			
			std::map<std::string, std::string> mapTmps;
			for (unsigned int index = 0; index < vecFiledNames.size(); ++index)
			{
				mapTmps.insert(std::make_pair(vecFiledNames[index], pSet->GetValue(vecFiledNames[index])));
			}			
			datas.push_back(mapTmps);
		}
	}

	return true;
}

bool CBusinessImpl::_ProcFeedbackData(const std::string& strBeginTimes, const std::string& strEndTime)
{
	ICC_LOG_DEBUG(m_pLog, "proc feedback data begin: begintime:[%s], endtime:[%s]", strBeginTimes.c_str(), strEndTime.c_str());
	std::string strProcessId;
	std::string strProcessDeptName;
	std::string strDispatchDeptName;
	std::vector<std::map<std::string, std::string>> datas;
	if (!_ReadFeedbackDataFromOracle(strBeginTimes, strEndTime, datas))
	{
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "read feedback data from oracle complete. count:%d, begintime:[%s], endtime:[%s]", datas.size(), strBeginTimes.c_str(), strEndTime.c_str());

	if (!_WriteFeedbackDataToICC(datas))
	{
		return false;
	}
	ICC_LOG_DEBUG(m_pLog, "write feedback data complete! begintime:[%s], endtime:[%s],ProcessId:[%s]", strBeginTimes.c_str(), strEndTime.c_str(),strProcessId.c_str());

	std::vector< PROTOCOL::CAddOrUpdateProcessRequest::CProcessData> vecProcessDatas;
	
	if (!_ReadProcessDataByProcessID(datas,vecProcessDatas))
	{
		ICC_LOG_DEBUG(m_pLog, "Read Pjdbh By Jjdbh failed");
		return false;
	}
	ICC_LOG_DEBUG(m_pLog, "read process data from feedback from oracle complete. begintime:[%s], endtime:[%s]", strBeginTimes.c_str(), strEndTime.c_str());

	/*if (!_FilterProcessData_FKDB(vecProcessDatas))
	{
		ICC_LOG_DEBUG(m_pLog, "Filter Process Data By Fkdb failed");
		return false;
	}
	ICC_LOG_DEBUG(m_pLog, "filter process by feedback data complete!! begintime:%s, endtime:%s", strBeginTimes.c_str(), strEndTime.c_str());*/

	//if (!_SendFeedbackData(vecProcessDatas))
	//{
	//	ICC_LOG_DEBUG(m_pLog, "Send Process Data failed");
	//	return false;
	//}
	//ICC_LOG_DEBUG(m_pLog, "send process data by feedback complete!! begintime:%s, endtime:%s", strBeginTimes.c_str(), strEndTime.c_str());
	if (!UpdateOperateFeedbackLog(vecProcessDatas))
	{
		ICC_LOG_DEBUG(m_pLog, "update OperateLog failed");
		return false;
	}
	ICC_LOG_DEBUG(m_pLog, "update OperateLog complete!!!");

	return true;
}

bool CBusinessImpl::_GetPreTimestamp(const std::string& strKey, std::string& strPreTimestamp)
{	
	strPreTimestamp = "";
	if (!m_pRedisClient->Get(strKey, strPreTimestamp))
	{
		ICC_LOG_WARNING(m_pLog, "get founder timestamp data from redis failed!");
		return false;
	}

	if (strPreTimestamp.empty())
	{
		strPreTimestamp = "2000-01-01 00:00:00";
	}

	return true;
}

void CBusinessImpl::_WriteTimestamp(const std::string& strKey, const std::string& strTimestamp)
{
	if (!m_pRedisClient->Set(strKey, strTimestamp))
	{
		ICC_LOG_ERROR(m_pLog, "write founder timestamp data to redis failed! key:[%s], value:%s",strKey.c_str(), strTimestamp.c_str());
		return ;
	}
}

bool CBusinessImpl::_ReadProcessData(const std::string& strBeginTimes, const std::string& strEndTime, std::vector<PROTOCOL::CAddOrUpdateProcessRequest::CProcessData>& vecProcessDatas)
{
	DataBase::SQLRequest tmp_oSQLRequst;
	tmp_oSQLRequst.sql_id = "oracle_query_process";
	tmp_oSQLRequst.param["mode_name"] = m_strTableModeName;
	tmp_oSQLRequst.param["begin_time"] = strBeginTimes;
	tmp_oSQLRequst.param["end_time"] = strEndTime;
	{
		ResultSetPtr pSet = m_pOracleClient->Exec(tmp_oSQLRequst);
		if (!pSet->IsValid())
		{
			ICC_LOG_WARNING(m_pLog, "resultset is invalid! error:%s, sql:%s", pSet->GetErrorMsg().c_str(), pSet->GetSQL().c_str());
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", pSet->GetSQL().c_str());

		int iCounter = CResultSetCounter::Instance()->GetCounter();
		ICC_LOG_DEBUG(m_pLog, "enter resultset counter is %d", iCounter);

		while (pSet->Next())
		{
			PROTOCOL::CAddOrUpdateProcessRequest::CProcessData processData;

			processData.m_strMsgSource = SOURCE_FOUNDER;
			processData.m_strID = pSet->GetValue("PJDBH");
			processData.m_strDispatchDeptDistrictCode = pSet->GetValue("XZQHDM");
			processData.m_strAlarmID = pSet->GetValue("JJDBH");
			processData.m_strDispatchDeptCode = pSet->GetValue("PJDWDM");
			processData.m_strDispatchCode = pSet->GetValue("PJYBH");
			processData.m_strDispatchName = pSet->GetValue("PJYXM");
			processData.m_strRecordID = pSet->GetValue("PJLYH");
			processData.m_strDispatchSuggestion = pSet->GetValue("PJYJ");
			processData.m_strProcessDeptCode = pSet->GetValue("CJDWDM");
			std::string strPjsj= pSet->GetValue("PJSJ");
			std::string strUtcPjsj = FromLocalToUTC(strPjsj);
			processData.m_strTimeSubmit = strUtcPjsj;
			std::string strXtzdddsj= pSet->GetValue("XTZDDDSJ");
			std::string strUtcXtzddsj = FromLocalToUTC(strXtzdddsj);
			processData.m_strTimeArrived = strUtcXtzddsj;
			std::string strPdqssj= pSet->GetValue("PDQSSJ");
			std::string strUtcPdqssj = FromLocalToUTC(strPdqssj);
			processData.m_strTimeSigned = strUtcPdqssj;
			processData.m_strProcessName = pSet->GetValue("QSRYXM");
			processData.m_strProcessCode = pSet->GetValue("QSRYBM");
			processData.m_strDispatchPersonnel = pSet->GetValue("CDRY");
			processData.m_strDispatchVehicles = pSet->GetValue("CDCL");
			processData.m_strDispatchBoats = pSet->GetValue("CDCT");
			processData.m_strState = pSet->GetValue("JQCLZTDM");
			std::string strCjsj= pSet->GetValue("rksj");
			std::string strUtcCjsj = FromLocalToUTC(strCjsj);
			processData.m_strCreateTime = strUtcCjsj;
			std::string strGxsj= pSet->GetValue("GXSJ");
			std::string strUtcGxsj = FromLocalToUTC(strGxsj);
			processData.m_strUpdateTime = strUtcGxsj;
			processData.m_strCancelReason = pSet->GetValue("CXYY");
			std::string strTmpState = pSet->GetValue("TBZT");
			if (strTmpState == "1")
			{
				processData.m_strState = "17";
			}
			processData.m_strProcessObjectCode = processData.m_strProcessCode;
			processData.m_strProcessObjectName = processData.m_strProcessName;
			processData.m_strProcessDeptName = pSet->GetValue("CJDWMC");
			processData.m_strCreateUser = pSet->GetValue("PJYXM");
			processData.m_strDispatchDeptName = pSet->GetValue("PJDWMC");
			processData.m_strProcessObjectType = pSet->GetValue("SJYLX");
			vecProcessDatas.push_back(processData);
		}
	}

	return true;
}

bool CBusinessImpl::_IsNeedUpdate(const std::string& strProcessId, bool& bNeedUpdate)
{
	bNeedUpdate = false;

	DataBase::SQLRequest tmp_oSQLRequst;
	tmp_oSQLRequst.sql_id = "oracle_filter_process";
	tmp_oSQLRequst.param["PJDBH"] = strProcessId;

	std::string strTime = m_pDateTime->GetDispatchIdTime(strProcessId);
	if (strTime != "")
	{
		tmp_oSQLRequst.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 86400 * 15);
		tmp_oSQLRequst.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(tmp_oSQLRequst);
	if (!l_pResult->IsValid())
	{
		ICC_LOG_WARNING(m_pLog, "resultset is invalid! error:%s sql:%s", l_pResult->GetErrorMsg().c_str(), l_pResult->GetSQL().c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", l_pResult->GetSQL().c_str());

	do 
	{
		if (l_pResult->Next())
		{
			std::string strCount = l_pResult->GetValue("count");
			if (std::stoi(strCount) > 0)
			{
				bNeedUpdate = true;
				break;
			}
			else
			{
				bNeedUpdate = false;
				break;
			}
			//	std::string curTime = l_pResult->GetValue("gxsj");
			//	if (strUpdateTime.empty())
			//	{	
			//		bNeedUpdate = true;
			//		break;
			//	}

			//	DateTime::CDateTime strPgTime = m_pDateTime->FromString(curTime);
			//	//防止更新时间出现误差
			//	DateTime::CDateTime m_oUpdateTime=m_pDateTime->AddSeconds(strPgTime, -10);
			//	std::string strTime = m_pDateTime->ToString(strPgTime);
			//	std::string strTmpPgTime = _StandardDateTime(strTime);
			//	std::string strTmpOraTime = _StandardDateTime(strUpdateTime);
			//	ICC_LOG_DEBUG(m_pLog, "database time:[%s],updatetime:[%s]", strTmpPgTime.c_str(), strTmpOraTime.c_str());

			//	if(strTmpOraTime > strTmpPgTime)
			//	{ 
			//		bNeedUpdate = true;
			//		break;
			//	}
			//	 //防止数据库里有了数据，但是更新时间相同，造成阻塞
			//	else if(strTmpOraTime == strTmpPgTime)
			//	{
			//		 bNeedUpdate = false;
			//		 bContinue = true;
			//		 break;
			//	}
			//}
			//else
			//{
			//	bNeedUpdate = false;
			//}
		}
	} while (false);
	
	return true;
}


bool CBusinessImpl::_FilterProcessData_FKDB(std::vector<PROTOCOL::CAddOrUpdateProcessRequest::CProcessData>& vecProcessDatas)
{
	std::vector<PROTOCOL::CAddOrUpdateProcessRequest::CProcessData> vecTempDatas = vecProcessDatas;
	vecProcessDatas.clear();

	unsigned int iCount = vecTempDatas.size();
	for (unsigned int i = 0; i < iCount; ++i)
	{
		bool bUpdate = true;
		if (!_IsNeedUpdate(vecProcessDatas[i].m_strID, bUpdate))
		{
			return false;
		}

		if (!bUpdate)
		{
			continue;
		}
		vecProcessDatas.push_back(vecTempDatas[i]);
	}

	return true;
}

bool CBusinessImpl::_SendProcessData(std::vector<PROTOCOL::CAddOrUpdateProcessRequest::CProcessData>& vecProcessDatas)
{
	unsigned int iCount = vecProcessDatas.size();
	DataBase::SQLRequest l_SQLRequest;
	DataBase::IResultSetPtr lSet;
	std::string strProId;
	std::vector<PROTOCOL::CAlarmInfo> m_vecData;
	PROTOCOL::CAddOrUpdateProcessRequest::CProcessData l_ProcessData;
	for (unsigned int i = 0; i < iCount; ++i)
	{
		//中间库传过来的数据如果是派警到个人的，就不存到ICC的库并且不发同步更新警情的消息，只写流水，受警类型为01的标识方正派警员的派警单
		if (vecProcessDatas[i].m_strState!= "01" && vecProcessDatas[i].m_strProcessObjectType != "01")
		{
			l_ProcessData.m_strMsgSource = SOURCE_FOUNDER;
			PROTOCOL::CAddOrUpdateAlarmWithProcessRequest l_CAddOrUpdateProcessRequest;
			l_CAddOrUpdateProcessRequest.m_oHeader.m_strCmd = "add_or_update_alarm_and_process_request";
			l_CAddOrUpdateProcessRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
			l_CAddOrUpdateProcessRequest.m_oHeader.m_strRequest = "queue_alarm";
			l_CAddOrUpdateProcessRequest.m_oHeader.m_strRequestType = "0";
			l_CAddOrUpdateProcessRequest.m_oHeader.m_strRequestFlag = "MQ";
			l_CAddOrUpdateProcessRequest.m_oHeader.m_strMsgId = m_pString->CreateGuid();
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strState = vecProcessDatas[i].m_strState;
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strID = vecProcessDatas[i].m_strAlarmID;
			l_SQLRequest.sql_id = "select_max_state_in_process_table_by_alarm_id";
			l_SQLRequest.param["alarm_id"] = vecProcessDatas[i].m_strAlarmID;
			l_SQLRequest.param["mode_name"] = m_strTableModeName;
			ResultSetPtr pSet = m_pOracleClient->Exec(l_SQLRequest);
			if (!pSet->IsValid())
			{
				ICC_LOG_WARNING(m_pLog, "resultset is invalid! error:%s sql:%s", pSet->GetErrorMsg().c_str(), pSet->GetSQL().c_str());
				return false;
			}

			ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", pSet->GetSQL().c_str());

			int iCounter = CResultSetCounter::Instance()->GetCounter();
			ICC_LOG_DEBUG(m_pLog, "enter resultset counter is %d", iCounter);

			ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", pSet->GetSQL().c_str());
			if (pSet->Next())
			{
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strState = pSet->GetValue("JQCLZTDM");
			}
	/*		strProId = _ReadProcessIDByAlarmID(vecProcessDatas[i].m_strAlarmID);
			ICC_LOG_DEBUG(m_pLog, "Process Id[%s]", strProId.c_str());*/
			l_CAddOrUpdateProcessRequest.m_oBody.m_vecProcessData.push_back(vecProcessDatas[i]);
			l_SQLRequest.sql_id = "select_jjdb_in_postgres_by_alarm_id";
			l_SQLRequest.param["alarm_id"] = vecProcessDatas[i].m_strAlarmID;
			std::string strTime = m_pDateTime->GetAlarmIdTime(vecProcessDatas[i].m_strAlarmID);
			if (strTime != "")
			{
				l_SQLRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
				l_SQLRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
			}
			lSet = m_pDBConn->Exec(l_SQLRequest);
			if (!lSet->IsValid())
			{
				ICC_LOG_WARNING(m_pLog, "resultset is invalid! error:%s, sql:%s", lSet->GetErrorMsg().c_str(), lSet->GetSQL().c_str());
				return false;
			}

			ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", lSet->GetSQL().c_str());
			if (lSet->Next())
			{
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strAdminDeptCode = lSet->GetValue("gxdwdm");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strAdminDeptName = lSet->GetValue("gxdwmc");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strAdminDeptOrgCode = lSet->GetValue("gxdwdmbs");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strAlarmAddr = lSet->GetValue("bjdz");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strBusinessState = lSet->GetValue("dqywzt");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strCalledNoType = lSet->GetValue("jjlx");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strCallerAddr = lSet->GetValue("jqdz");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strCallerGender = lSet->GetValue("bjrxbdm");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strCallerID = lSet->GetValue("bjrzjhm");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strCallerIDType = lSet->GetValue("bjrzjdm");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strCallerName = lSet->GetValue("bjrmc");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strCallerNo = lSet->GetValue("bjdh");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strCallerUserName = lSet->GetValue("bjdhyhm");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strContent = lSet->GetValue("bjnr");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strContactNo = lSet->GetValue("lxdh");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strCreateTime = lSet->GetValue("rksj");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strCreateUser = lSet->GetValue("cjry");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strDeleteFlag = lSet->GetValue("scbs");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strDescOfDead = lSet->GetValue("swryqksm");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strDescOfInjured = lSet->GetValue("ssryqksm");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strDescOfTrapped = lSet->GetValue("bkryqksm");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strEmergencyRescueLevel = lSet->GetValue("yjjydjdm");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strErpetratorsNumber = lSet->GetValue("zars");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strFirstSubmitTime = lSet->GetValue("jjwcsj");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strFirstType = lSet->GetValue("jqlbdm");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strFourthType = lSet->GetValue("jqzldm");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strID = lSet->GetValue("jjdbh");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strIsArmed = lSet->GetValue("ywcwq");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strIsExplosionOrLeakage = lSet->GetValue("ywbzxl");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strIsFeedBack = lSet->GetValue("fkbs");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strIsForeignLanguage = lSet->GetValue("sfswybj");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strIsHazardousSubstances = lSet->GetValue("ywwxwz");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strIsInvalid = lSet->GetValue("wxbs");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strIsMerge = lSet->GetValue("hbbs");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strIsOver = lSet->GetValue("sfja");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strIsProcessFlagSynchronized = lSet->GetValue("tbcjbs");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strIsSameForBackground = lSet->GetValue("ybjxxyz");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strIsVisitor = lSet->GetValue("hfbs");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strJurisdictionalOrgcode = lSet->GetValue("tzhgxdwdm");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strJurisdictionalOrgidentifier = lSet->GetValue("tzhgxdwdmbs");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strJurisdictionalOrgname = lSet->GetValue("tzhgxdwmc");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strLatitude = lSet->GetValue("bjryzb");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strLevel = lSet->GetValue("jqdjdm");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strLongitude = lSet->GetValue("bjrxzb");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strManualLatitude = lSet->GetValue("fxdwwd");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strManualLongitude = lSet->GetValue("fxdwjd");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strMergeID = lSet->GetValue("glzjjdbh");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strMergeType = lSet->GetValue("gllx");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strMsgSource = lSet->GetValue("lhlx");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strPrivacy = lSet->GetValue("smbm");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strReceiptCode = lSet->GetValue("jjybh");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strReceiptDeptCode = lSet->GetValue("jjdwdm");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode = lSet->GetValue("xzqhdm");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strReceiptDeptName = lSet->GetValue("jjdwmc");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strReceiptSrvName = lSet->GetValue("jjfs");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strReceiptName = lSet->GetValue("jjyxm");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strReceivedTime = lSet->GetValue("jjsj");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strRemark = lSet->GetValue("bcjjnr");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strSeatNo = lSet->GetValue("jjxwh");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strSecondType = lSet->GetValue("jqlxdm");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strSourceType = lSet->GetValue("jqlyfs");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strState = vecProcessDatas[i].m_strState;
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strSymbolCode = lSet->GetValue("tzdbh");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strThirdType = lSet->GetValue("jqxldm");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strTime = lSet->GetValue("bjsj");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strTitle = lSet->GetValue("jqbq");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strUpdateTime = m_pDateTime->CurrentDateTimeStr();
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strUpdateUser = vecProcessDatas[i].m_strDispatchCode;
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strUpdateUserDeptCode = vecProcessDatas[i].m_strDispatchDeptCode;
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strUpdateUserDeptName = vecProcessDatas[i].m_strDispatchDeptName;
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strUpdateUserName = vecProcessDatas[i].m_strDispatchName;
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strVehicleNo = lSet->GetValue("sjcph");
				l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strVehicleType = lSet->GetValue("sjchpzldm");
			}
			std::string l_strMessage = l_CAddOrUpdateProcessRequest.ToString(ICCGetIJsonFactory()->CreateJson());
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
		}
	}

	return true;
}

bool CBusinessImpl::_ProcProcessData(const std::string& strBeginTime, const std::string& strEndTime)
{
	ICC_LOG_DEBUG(m_pLog, "read process data begin!! begintime:%s, endtime:%s", strBeginTime.c_str(), strEndTime.c_str());

	PROTOCOL::CAlarmInfo CAlarm;
	std::vector< PROTOCOL::CAddOrUpdateProcessRequest::CProcessData> vecProcessDatas;
	bool bRead = _ReadProcessData(strBeginTime, strEndTime, vecProcessDatas);
	if (!bRead)
	{
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "read process data complete!! count:%d, begintime:%s, endtime:%s", vecProcessDatas.size(), strBeginTime.c_str(), strEndTime.c_str());
	
	bool bFilter = _WriteProcessDataToICC(vecProcessDatas);
	if (!bFilter)
	{
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "write process data complete!! begintime:%s, endtime:%s", strBeginTime.c_str(), strEndTime.c_str());


	bool bSend = _SendProcessData(vecProcessDatas);
	if (!bSend)
	{
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "send process data complete!! begintime:%s, endtime:%s", strBeginTime.c_str(), strEndTime.c_str());

	bool bUpdateLog = UpdateOperateProcessLog(vecProcessDatas);
	if (!bUpdateLog)
	{
		ICC_LOG_DEBUG(m_pLog, "update OperateLog  failed!!!");
		return false;
	}
	ICC_LOG_DEBUG(m_pLog, "update OperateLog  complete!!!");

	return true;
}

bool CBusinessImpl::_SyncDataFromFounderToICC(const std::string& strEndTime)
{
	std::string strBeginstrTime;
	if (!_GetPreTimestamp(READFOUNDERTIMESTAMP, strBeginstrTime))
	{
		ICC_LOG_DEBUG(m_pLog, "get read oracle pre time failed!");
		return false;
	}
	//每三秒读一次，每次前八秒的数据，防止因为时钟同步问题漏读数据
	//DateTime::CDateTime m_oBeginTime = m_pDateTime->FromString(strBeginstrTime);
	//DateTime::CDateTime l_oBeginTime=m_pDateTime->AddSeconds(m_oBeginTime, -5);
	//std::string strBeginTime = m_pDateTime->ToString(l_oBeginTime);
	ICC_LOG_DEBUG(m_pLog, "sync founder data to icc begin!! begintime:%s, endtime:%s", strBeginstrTime.c_str(), strEndTime.c_str());

	//自接警暂定由VCS自接警，取消读取方正的接警单数据。
	if (!_ProcAlarmData(strBeginstrTime, strEndTime))
	{
		ICC_LOG_DEBUG(m_pLog, "process alarm data failed!");
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "sync founder alarm data to icc complete !! begintime:%s, endtime:%s", strBeginstrTime.c_str(), strEndTime.c_str());
	if (!_ProcProcessData(strBeginstrTime, strEndTime))
	{
		ICC_LOG_DEBUG(m_pLog, "process process data failed!");
		return false;
	}
	ICC_LOG_DEBUG(m_pLog, "sync founder process data to icc complete !! begintime:%s, endtime:%s", strBeginstrTime.c_str(), strEndTime.c_str());
	if (!_ProcFeedbackData(strBeginstrTime, strEndTime))
	{
		ICC_LOG_DEBUG(m_pLog, "process feedback data failed!");
		return false;
	}
	ICC_LOG_DEBUG(m_pLog, "sync founder feedback data to icc complete !! begintime:%s, endtime:%s", strBeginstrTime.c_str(), strEndTime.c_str());

	_WriteTimestamp(READFOUNDERTIMESTAMP, strEndTime);
	ICC_LOG_DEBUG(m_pLog, "sync founder data to icc end!! begintime:%s, endtime:%s", strBeginstrTime.c_str(), strEndTime.c_str());

	return true;
}

void CBusinessImpl::_ReadDataThread()
{
	ICC_LOG_DEBUG(m_pLog, "_ReadDataThread enter!");
	while (true)
	{
		std::string strCurrentTime = m_pDateTime->CurrentDateTimeStr();
		std::string strEndTime = FromUTCToLocal(strCurrentTime);
	
		ICC_LOG_DEBUG(m_pLog, "founder sync data begin:%s,interval:%lld", strEndTime.c_str(), m_llCheckInterval);

		int iCounter = CResultSetCounter::Instance()->GetCounter();
		ICC_LOG_DEBUG(m_pLog, "enter resultset counter is %d", iCounter);

		do 
		{			
			if (!_SyncDataFromICCToFounder(strEndTime))
			{
				break;
			}
			DateTime::CDateTime m_oEndTime = m_pDateTime->FromString(strCurrentTime);
			unsigned int m_DelayTime = m_pString->ToUInt(m_strDelayTime);
			DateTime::CDateTime l_oEndTime = m_pDateTime->AddSeconds(m_oEndTime, m_DelayTime);
			std::string EndTime = m_pDateTime->ToString(l_oEndTime);
			strEndTime = FromUTCToLocal(EndTime);
			if (!_SyncDataFromFounderToICC(strEndTime))
			{
				break;
			}			

		} while (false);

		ICC_LOG_DEBUG(m_pLog, "founder sync data end:%s", strEndTime.c_str());	
		
		boost::this_thread::sleep(boost::posix_time::millisec(m_llCheckInterval));
	}
	ICC_LOG_DEBUG(m_pLog, "_ReadDataThread exit!");
}


std::string CBusinessImpl::_StandardDateTime(const std::string& strTimeStamp)
{
	int iIndex = strTimeStamp.find(".");
	if (iIndex == -1)
	{
		return strTimeStamp;
	}

	return strTimeStamp.substr(0, iIndex);	
}




















/*****************************************************************
 *                       保留代码
 ****************************************************************/


void CBusinessImpl::_InsertAlarmData(const PROTOCOL::CAlarmInfo alarmData)
{
	DataBase::SQLRequest tmp_oSQLRequst;
	tmp_oSQLRequst.sql_id = "";
	_AssignAlarmSql(alarmData, tmp_oSQLRequst);
}

void CBusinessImpl::_UpdateAlarmData(const PROTOCOL::CAlarmInfo alarmData)
{
	DataBase::SQLRequest tmp_oSQLRequst;
	tmp_oSQLRequst.sql_id = "";
	_AssignAlarmSql(alarmData, tmp_oSQLRequst);
}

void CBusinessImpl::_AssignAlarmSql(const PROTOCOL::CAlarmInfo alarmData, DataBase::SQLRequest& sqlRequest)
{
	sqlRequest.param["id"] = alarmData.m_strID;
	sqlRequest.param["merge_id"] = alarmData.m_strMergeID;
	sqlRequest.param["merge_type"] = alarmData.m_strMergeType;
	sqlRequest.param["receipt_seatno"] = alarmData.m_strSeatNo;
	sqlRequest.param["label"] = alarmData.m_strTitle;
	sqlRequest.param["content"] = alarmData.m_strContent;
	sqlRequest.param["receiving_time"] = alarmData.m_strTime;
	sqlRequest.param["received_time"] = alarmData.m_strReceivedTime;
	sqlRequest.param["addr"] = alarmData.m_strAddr;
	sqlRequest.param["longitude"] = alarmData.m_strLongitude;
	sqlRequest.param["latitude"] = alarmData.m_strLatitude;
	sqlRequest.param["state"] = alarmData.m_strState;
	sqlRequest.param["level"] = alarmData.m_strLevel;
	sqlRequest.param["source_type"] = alarmData.m_strSourceType;
	sqlRequest.param["source_id"] = alarmData.m_strSourceID;
	sqlRequest.param["handle_type"] = alarmData.m_strHandleType;
	sqlRequest.param["first_type"] = alarmData.m_strFirstType;
	sqlRequest.param["second_type"] = alarmData.m_strSecondType;
	sqlRequest.param["third_type"] = alarmData.m_strThirdType;
	sqlRequest.param["fourth_type"] = alarmData.m_strFourthType;
	sqlRequest.param["vehicle_no"] = alarmData.m_strVehicleNo;
	sqlRequest.param["vehicle_type"] = alarmData.m_strVehicleType;
	sqlRequest.param["symbol_code"] = alarmData.m_strSymbolCode;

	sqlRequest.param["called_no_type"] = alarmData.m_strCalledNoType;

	sqlRequest.param["caller_no"] = alarmData.m_strCallerNo;
	sqlRequest.param["caller_name"] = alarmData.m_strCallerName;
	sqlRequest.param["caller_addr"] = alarmData.m_strCallerAddr;
	sqlRequest.param["caller_id"] = alarmData.m_strCallerID;
	sqlRequest.param["caller_id_type"] = alarmData.m_strCallerIDType;
	sqlRequest.param["caller_gender"] = alarmData.m_strCallerGender;


	sqlRequest.param["contact_no"] = alarmData.m_strContactNo;

	sqlRequest.param["admin_dept_code"] = alarmData.m_strAdminDeptCode;
	sqlRequest.param["admin_dept_name"] = alarmData.m_strAdminDeptName;
	sqlRequest.param["receipt_dept_district_code"] = alarmData.m_strReceiptDeptDistrictCode;
	sqlRequest.param["receipt_dept_code"] = alarmData.m_strReceiptDeptCode;
	sqlRequest.param["receipt_dept_name"] = alarmData.m_strReceiptDeptName;
	sqlRequest.param["receipt_code"] = alarmData.m_strReceiptCode;
	sqlRequest.param["receipt_name"] = alarmData.m_strReceiptName;

	sqlRequest.param["create_time"] = alarmData.m_strCreateTime;
	sqlRequest.param["create_user"] = alarmData.m_strCreateUser;

	sqlRequest.param["is_privacy"] = alarmData.m_strPrivacy;
	sqlRequest.param["remark"] = alarmData.m_strRemark;

	//新增
	sqlRequest.param["alarm_addr"] = alarmData.m_strAlarmAddr;
	sqlRequest.param["caller_user_name"] = alarmData.m_strCallerUserName;
	sqlRequest.param["erpetrators_number"] = alarmData.m_strErpetratorsNumber;
	sqlRequest.param["is_armed"] = alarmData.m_strIsArmed;
	sqlRequest.param["is_hazardous_substances"] = alarmData.m_strIsHazardousSubstances;
	sqlRequest.param["is_explosion_or_leakage"] = alarmData.m_strIsExplosionOrLeakage;
	sqlRequest.param["desc_of_trapped"] = alarmData.m_strDescOfTrapped;
	sqlRequest.param["desc_of_injured"] = alarmData.m_strDescOfInjured;
	sqlRequest.param["desc_of_dead"] = alarmData.m_strDescOfDead;
	sqlRequest.param["is_foreign_language"] = alarmData.m_strIsForeignLanguage;
	sqlRequest.param["manual_longitude"] = alarmData.m_strManualLongitude;
	sqlRequest.param["manual_latitude"] = alarmData.m_strManualLatitude;
	sqlRequest.param["emergency_rescue_level"] = alarmData.m_strEmergencyRescueLevel;
	sqlRequest.param["is_hazardous_vehicle"] = alarmData.m_strIsHazardousVehicle;
	sqlRequest.param["receipt_srv_name"] = alarmData.m_strReceiptSrvName;
	sqlRequest.param["admin_dept_org_code"] = alarmData.m_strAdminDeptOrgCode;
	sqlRequest.param["receipt_dept_org_code"] = alarmData.m_strReceiptDeptOrgCode;
	sqlRequest.param["is_invalid"] = alarmData.m_strIsInvalid;
	sqlRequest.param["business_status"] = alarmData.m_strBusinessState;

	sqlRequest.param["is_delete"] = alarmData.m_strDeleteFlag; //先写死 2022/3/17
	sqlRequest.param["is_feedback"] = alarmData.m_strIsFeedBack;
	sqlRequest.param["is_visitor"] = alarmData.m_strIsVisitor;

	sqlRequest.param["jurisdictional_orgcode"] = alarmData.m_strJurisdictionalOrgcode;
	sqlRequest.param["jurisdictional_orgname"] = alarmData.m_strJurisdictionalOrgname;
	sqlRequest.param["jurisdictional_orgidentifier"] = alarmData.m_strJurisdictionalOrgidentifier;
	sqlRequest.param["had_push"] = alarmData.m_strHadPush;
	sqlRequest.param["is_merge"] = alarmData.m_strIsMerge;
	sqlRequest.param["is_over"] = alarmData.m_strIsOver;

	sqlRequest.param["createTeminal"] = alarmData.m_strCreateTeminal;
	sqlRequest.param["updateTeminal"] = alarmData.m_strUpdateTeminal;
}

void CBusinessImpl::OnReceiveAlarmSyncNotify(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string p_strMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "received alarm sync message : [%s]", p_strMsg.c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CAlarmSync l_CAlarmSync;
	if (!l_CAlarmSync.ParseString(p_strMsg, l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "analyze alarm sync message failed!! : [%s]", p_strMsg.c_str());
		return;
	}

	//查询表数据是否存在，不存在，插入；存在，更新	
	/*if (!_ExistAlarmData(l_CAlarmSync.m_oBody.m_oAlarmInfo.m_strID))
	{
		_InsertAlarmData(l_CAlarmSync.m_oBody.m_oAlarmInfo);
	}
	else
	{
		_UpdateAlarmData(l_CAlarmSync.m_oBody.m_oAlarmInfo);
	}*/
}

bool CBusinessImpl::_ExistProccessData(const std::string& strProcessId, bool& bExist)
{
	bExist = false;
	DataBase::SQLRequest tmp_oSQLRequst;
	tmp_oSQLRequst.sql_id = "oracle_judge_process_exist";
	tmp_oSQLRequst.param["mode_name"] = m_strTableModeName;
	tmp_oSQLRequst.param["pjdbh"] = strProcessId;
	{
		ResultSetPtr pSet = m_pOracleClient->Exec(tmp_oSQLRequst);
		if (!pSet->IsValid())
		{
			ICC_LOG_WARNING(m_pLog, "resultset is invalid! sql:%s", pSet->GetSQL().c_str());
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", pSet->GetSQL().c_str());

		do
		{
			if (pSet->Next())
			{
				std::string strCount = pSet->GetValue("COUNT");
				if (strCount.empty())
				{
					ICC_LOG_ERROR(m_pLog, "not find field:COUNT");
					return false;
				}

				if (std::stoi(strCount) > 0)
				{
					bExist = true;
					break;
				}
			}
		} while (false);


	}
	return true;
}

bool CBusinessImpl::_WriteProcessDataToICC(std::vector<PROTOCOL::CAddOrUpdateProcessRequest::CProcessData>& vecProcessDatas)
{
	std::vector<PROTOCOL::CAddOrUpdateProcessRequest::CProcessData> vecTempDatas = vecProcessDatas;
	vecProcessDatas.clear();

	
	DataBase::SQLRequest l_SQLRequest;
	unsigned int iCount = vecTempDatas.size();
	std::string strState;
	for (unsigned int i = 0; i < iCount; ++i)
	{
		//中间库传过来的数据如果是派警到个人的，就不存到ICC的库，受警类型为01的标识方正派警员的派警单
		bool bUpdate = true;
		bool bContinue = false;
		//if (vecProcessDatas[i].m_strDispatchDeptCode != vecProcessDatas[i].m_strProcessDeptCode)
		//{
		//	vecTempDatas[i].m_strID = _ReadProcessIDByAlarmID(vecProcessDatas[i].m_strAlarmID);
		//}
		if (!_IsNeedUpdate(vecTempDatas[i].m_strID, bUpdate))
		{
			return false;
		}
		ICC_LOG_DEBUG(m_pLog, "is Need Update:[%d],is Need continue[%d]", bUpdate, bContinue);
		if (bContinue)
		{
			continue;
		}
		if (vecProcessDatas[i].m_strProcessObjectType == "01")
		{
			strState = "17";
		}
		else
		{
			strState = vecTempDatas[i].m_strState;
		}
		if (!bUpdate)
		{
			l_SQLRequest.sql_id = "postgres_insert_icc_t_pjdb";
			l_SQLRequest.param["xzqhdm"] = vecTempDatas[i].m_strDispatchDeptDistrictCode;
			l_SQLRequest.param["pjdbh"] = vecTempDatas[i].m_strID;
			std::string strTime = m_pDateTime->GetAlarmIdTime(vecTempDatas[i].m_strID);
			if (strTime != "")
			{
				l_SQLRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime);
				l_SQLRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
			}
			l_SQLRequest.param["jjdbh"] = vecTempDatas[i].m_strAlarmID;
			l_SQLRequest.param["pjdwdm"] = vecTempDatas[i].m_strDispatchDeptCode;
			l_SQLRequest.param["pjybh"] = vecTempDatas[i].m_strDispatchCode;
			l_SQLRequest.param["pjyxm"] = vecTempDatas[i].m_strDispatchName;
			l_SQLRequest.param["pjlyh"] = vecTempDatas[i].m_strRecordID;
			l_SQLRequest.param["pjyj"] = vecTempDatas[i].m_strDispatchSuggestion;
			l_SQLRequest.param["cjdwdm"] = vecTempDatas[i].m_strProcessDeptCode;
			l_SQLRequest.param["pjsj"] = vecTempDatas[i].m_strTimeSubmit;
			l_SQLRequest.param["xtzdddsj"] = vecTempDatas[i].m_strTimeArrived;
			l_SQLRequest.param["pdqssj"] = vecTempDatas[i].m_strTimeSigned;
			l_SQLRequest.param["qsryxm"] = vecTempDatas[i].m_strProcessName;
			l_SQLRequest.param["qsrybm"] = vecTempDatas[i].m_strProcessCode;
			l_SQLRequest.param["cdry"] = vecTempDatas[i].m_strDispatchPersonnel;
			l_SQLRequest.param["cdcl"] = vecTempDatas[i].m_strDispatchVehicles;
			l_SQLRequest.param["cdct"] = vecTempDatas[i].m_strDispatchBoats;
			l_SQLRequest.param["jqclztdm"] = strState;
			l_SQLRequest.param["rksj"] = vecTempDatas[i].m_strCreateTime;
			l_SQLRequest.param["gxsj"] = vecTempDatas[i].m_strUpdateTime;
			l_SQLRequest.param["cjdzt"] = strState;
			l_SQLRequest.param["cjdwmc"] = vecTempDatas[i].m_strProcessDeptName;
			l_SQLRequest.param["pjdwmc"] = vecTempDatas[i].m_strDispatchDeptName;
			l_SQLRequest.param["cxyy"] = vecTempDatas[i].m_strCancelReason;
		}
		else
		{
			l_SQLRequest.sql_id = "postgres_update_icc_t_pjdb";
			l_SQLRequest.set["xzqhdm"] = vecTempDatas[i].m_strDispatchDeptDistrictCode;
			l_SQLRequest.param["pjdbh"] = vecTempDatas[i].m_strID;
			std::string strTime = m_pDateTime->GetDispatchIdTime(vecTempDatas[i].m_strID);
			if (strTime != "")
			{
				l_SQLRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 86400 * 15);
				l_SQLRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
			}
			l_SQLRequest.set["jjdbh"] = vecTempDatas[i].m_strAlarmID;
			l_SQLRequest.set["pjdwdm"] = vecTempDatas[i].m_strDispatchDeptCode;
			l_SQLRequest.set["pjybh"] = vecTempDatas[i].m_strDispatchCode;
			l_SQLRequest.set["pjyxm"] = vecTempDatas[i].m_strDispatchName;
			l_SQLRequest.set["pjlyh"] = vecTempDatas[i].m_strRecordID;
			l_SQLRequest.set["pjyj"] = vecTempDatas[i].m_strDispatchSuggestion;
			l_SQLRequest.set["cjdwdm"] = vecTempDatas[i].m_strProcessDeptCode;
			l_SQLRequest.set["pjsj"] = vecTempDatas[i].m_strTimeSubmit;
			l_SQLRequest.set["qsryxm"] = vecTempDatas[i].m_strProcessName;
			l_SQLRequest.set["qsrybm"] = vecTempDatas[i].m_strProcessCode;
			l_SQLRequest.set["cdry"] = vecTempDatas[i].m_strDispatchPersonnel;
			l_SQLRequest.set["cdcl"] = vecTempDatas[i].m_strDispatchVehicles;
			l_SQLRequest.set["cdct"] = vecTempDatas[i].m_strDispatchBoats;
			l_SQLRequest.set["jqclztdm"] = strState;
			l_SQLRequest.param["gxsj"] = vecTempDatas[i].m_strUpdateTime;
			l_SQLRequest.set["cjdzt"] = strState;
			l_SQLRequest.set["cjdwmc"] = vecTempDatas[i].m_strProcessDeptName;
			l_SQLRequest.set["pjdwmc"] = vecTempDatas[i].m_strDispatchDeptName;
			l_SQLRequest.set["cxyy"] = vecTempDatas[i].m_strCancelReason;
			if (vecTempDatas[i].m_strTimeArrived.empty())
			{
				l_SQLRequest.param["xtzdddsj"] = "NULL";
			}
			else
			{
				l_SQLRequest.param["xtzdddsj"] = m_pString->Format("\'%s\'", vecTempDatas[i].m_strTimeArrived.c_str());
			}
			if (vecTempDatas[i].m_strTimeSigned.empty())
			{
				l_SQLRequest.param["pdqssj"] = "NULL";
			}
			else
			{
				l_SQLRequest.param["pdqssj"] = m_pString->Format("\'%s\'", vecTempDatas[i].m_strTimeSigned.c_str());
			}
			if (vecTempDatas[i].m_strCreateTime.empty())
			{
				l_SQLRequest.param["rksj"] = "NULL";
			}
			else
			{
				l_SQLRequest.param["rksj"] = m_pString->Format("\'%s\'", vecTempDatas[i].m_strCreateTime.c_str());
			}
		}
		DataBase::IResultSetPtr pSet = m_pDBConn->Exec(l_SQLRequest);
		if (!pSet->IsValid())
		{
			ICC_LOG_DEBUG(m_pLog, "exec sql failed! error:%s, sql:%s", pSet->GetErrorMsg().c_str(), pSet->GetSQL().c_str());
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", pSet->GetSQL().c_str());
		vecProcessDatas.push_back(vecTempDatas[i]);

	}

	return true;

}

bool ICC::CBusinessImpl::_SyncFeedbackDataFromICCToFounder(const std::string& strBeginTime, const std::string& strEndTime)
{
	ICC_LOG_DEBUG(m_pLog, "read icc feedback data begin!! begintime:%s, endtime:%s", strBeginTime.c_str(), strEndTime.c_str());
	std::vector<std::map<std::string, std::string>> datas;
	if (!_ReadFeedbackDataFromICC(strBeginTime, strEndTime, datas))
	{
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "read alarm feedback from icc complete !! begintime:%s, endtime:%s", strBeginTime.c_str(), strEndTime.c_str());

	if (!_WriteFeedbackDataToFounder(datas))
	{
		return false;
	}

	return true;
}

bool ICC::CBusinessImpl::_ReadFeedbackDataFromICC(const std::string& strBeginTime, const std::string& strEndTime, std::vector<std::map<std::string, std::string>>& datas)
{
	std::string BeginTime = FromLocalToUTC(strBeginTime);
	std::string EndTime = FromLocalToUTC(strEndTime);
	DataBase::SQLRequest tmp_oSQLRequst;
	tmp_oSQLRequst.sql_id = "founder_select_icc_t_fkdb_by_updatetime";
	tmp_oSQLRequst.param["begin_time"] = BeginTime;
	tmp_oSQLRequst.param["end_time"] = EndTime;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(tmp_oSQLRequst);
	if (!l_pResult->IsValid())
	{
		ICC_LOG_WARNING(m_pLog, "resultset is invalid! sql:%s", l_pResult->GetSQL().c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", l_pResult->GetSQL().c_str());

	std::vector<std::string> vecFiledNames = l_pResult->GetFieldNames();
	while (l_pResult->Next())
	{
		std::map<std::string, std::string> mapTmps;
		unsigned int iCount = vecFiledNames.size();
		for (unsigned int i = 0; i < iCount; ++i)
		{
			mapTmps.insert(std::make_pair(vecFiledNames[i], l_pResult->GetValue(vecFiledNames[i])));
		}
		datas.push_back(mapTmps);
	}

	return true;
}

bool ICC::CBusinessImpl::_WriteFeedbackDataToFounder(std::vector<std::map<std::string, std::string>>& datas)
{
	unsigned int iCount = datas.size();
	for (unsigned int i = 0; i < iCount; ++i)
	{
		std::map<std::string, std::string>::const_iterator itr;
		itr = datas[i].find("fkdbh");
		if (itr == datas[i].end())
		{
			return false;
		}
		std::string strFeedbakId = itr->second;
		if (strFeedbakId.empty())
		{
			return false;
		}

		DataBase::SQLRequest tmp_oSQLRequst;
		bool bExist = false;
		if (!_ExistFeedbackData(strFeedbakId, bExist))
		{
			return false;
		}

		if (!bExist)
		{
			tmp_oSQLRequst.sql_id = "oracle_insert_founder_t_fkdb";
		}
		else
		{
			tmp_oSQLRequst.sql_id = "oracle_update_founder_t_fkdb";
		}

		tmp_oSQLRequst.param[MODE_NAME] = m_strTableModeName;
		for (itr = datas[i].begin(); itr != datas[i].end(); ++itr)
		{
			std::string strTmp;
			if (itr->first == "fksj" || itr->first == "cjsj01" || itr->first == "ddxcsj" || itr->first == "xcclwbsj" || itr->first == "jqfssj"|| itr->first == "rksj"||itr->first == "gxsj")
			{
				strTmp = _StandardDateTime(itr->second);
				std::string Tmp = FromUTCToLocal(strTmp);
				tmp_oSQLRequst.param[itr->first] = Tmp;
			}
			else if (itr->first == "jqzldm" && itr->second.empty())
			{
				tmp_oSQLRequst.param["jqzldm"] = datas[i]["jqxldm"];
			}
			else
			{
				tmp_oSQLRequst.param[itr->first] = itr->second;
			}
		}
		ResultSetPtr pSet = m_pOracleClient->Exec(tmp_oSQLRequst);
		ICC_LOG_DEBUG(m_pLog, "exec sql! sql:%s", pSet->GetSQL().c_str());
		if (!pSet->IsValid())
		{
			ICC_LOG_DEBUG(m_pLog, "result is invalid");
			return false;
		}
	}
	return true;
}

bool ICC::CBusinessImpl::_ExistFeedbackData(const std::string &strFeedbackId, bool& bExist)
{
	bExist = false;
	DataBase::SQLRequest tmp_oSQLRequst;
	tmp_oSQLRequst.sql_id = "oracle_judge_feedbackid_exist";
	tmp_oSQLRequst.param["mode_name"] = m_strTableModeName;
	tmp_oSQLRequst.param["fkdbh"] = strFeedbackId;
	{
		ResultSetPtr pSet = m_pOracleClient->Exec(tmp_oSQLRequst);
		if (!pSet->IsValid())
		{
			ICC_LOG_WARNING(m_pLog, "resultset is invalid! sql:%s", pSet->GetSQL().c_str());
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", pSet->GetSQL().c_str());

		std::vector<std::string> vecFiledNames = pSet->GetFieldNames();

		int iCounter = CResultSetCounter::Instance()->GetCounter();
		ICC_LOG_DEBUG(m_pLog, "enter resultset counter is %d", iCounter);

		do
		{
			if (pSet->Next())
			{
				std::string strCount = pSet->GetValue("COUNT");
				if (strCount.empty())
				{
					ICC_LOG_ERROR(m_pLog, "not find field:COUNT");
					return false;
				}

				if (std::stoi(strCount) > 0)
				{
					bExist = true;
					break;
				}
			}
		} while (false);


	}
	return true;
}

void CBusinessImpl::_InsertProcessData(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& processData)
{
	DataBase::SQLRequest tmp_oSQLRequst;
	tmp_oSQLRequst.sql_id = "";
	_AssignProcessSql(processData, tmp_oSQLRequst);
}

void CBusinessImpl::_UpdateProcessData(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& processData)
{
	DataBase::SQLRequest tmp_oSQLRequst;
	tmp_oSQLRequst.sql_id = "";
	_AssignProcessSql(processData, tmp_oSQLRequst);
}

void CBusinessImpl::_AssignProcessSql(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& processData, DataBase::SQLRequest& sqlRequest)
{
	sqlRequest.param["XZQHDM "] = processData.m_strDispatchDeptDistrictCode;
	sqlRequest.param["PJDBH "] = processData.m_strID;
	sqlRequest.param["JJDBH "] = processData.m_strAlarmID;
	sqlRequest.param["PJDWDM "] = processData.m_strDispatchDeptCode;
	sqlRequest.param["PJYBH "] = processData.m_strDispatchCode;
	sqlRequest.param["PJYXM "] = processData.m_strDispatchName;
	sqlRequest.param["PJLYH "] = processData.m_strRecordID;
	sqlRequest.param["PJYJ "] = processData.m_strDispatchSuggestion;
	sqlRequest.param["CJDWDM "] = processData.m_strProcessDeptCode;
	sqlRequest.param["PJSJ "] = processData.m_strTimeSubmit;
	sqlRequest.param["XTZDDDSJ "] = processData.m_strTimeArrived;
	sqlRequest.param["PDQSSJ "] = processData.m_strTimeSigned;
	sqlRequest.param["QSRYXM "] = processData.m_strProcessName;
	sqlRequest.param["QSRYBM "] = processData.m_strProcessCode;
	sqlRequest.param["CDRY "] = processData.m_strDispatchPersonnel;
	sqlRequest.param["CDCL "] = processData.m_strDispatchVehicles;
	sqlRequest.param["CDCT "] = processData.m_strDispatchBoats;
	sqlRequest.param["JQCLZTDM "] = processData.m_strState;
	sqlRequest.param["rksj"] = processData.m_strCreateTime;
	sqlRequest.param["GXSJ "] = processData.m_strUpdateTime;
}

void CBusinessImpl::OnReceiveProcessSyncNotify(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string p_strMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "received process sync message : [%s]", p_strMsg.c_str());
	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CAlarmProcessSync l_CAlarmProcessSync;
	if (!l_CAlarmProcessSync.ParseString(p_strMsg, l_pIJson))
	{
		ICC_LOG_ERROR(m_pLog, "analyze process sync message failed!! : [%s]", p_strMsg.c_str());
		return;
	}

	//查询表是否存在，不存在，插入；存在，更新
	/*if (!_ExistProccessData(l_CAlarmProcessSync.m_oBody.m_ProcessData.m_strID))
	{
		_InsertProcessData(l_CAlarmProcessSync.m_oBody.m_ProcessData);
	}
	else
	{
		_UpdateProcessData(l_CAlarmProcessSync.m_oBody.m_ProcessData);
	}*/
}


void CBusinessImpl::_Test()
{
	std::string strSql = "SELECT * FROM TEST01.ICC_T_FKDB";
	
	{
		ResultSetPtr pSet = m_pOracleClient->ExecQuery(strSql);
		if (!pSet->IsValid())
		{
			ICC_LOG_DEBUG(m_pLog, "exec sql failed! error:%s sql:%s", pSet->GetErrorMsg().c_str(), pSet->GetSQL().c_str());
			return;
		}

		ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", pSet->GetSQL().c_str());
		
		while (pSet->Next())
		{			
			std::string strid = pSet->GetValue("FKDBH");
			std::string strtest = pSet->GetValue("FKYXM");
			//std::string strbjdhyhm = pSet->GetValue("BJDHYHM");			

			//ICC_LOG_DEBUG(m_pLog, "jjyxm : %s;  bjdhyhm:%s", strjjyxm.c_str(), strbjdhyhm.c_str());
			ICC_LOG_DEBUG(m_pLog, "test : %s;", strtest.c_str());



		}
	}



	/*DataBase::SQLRequest tmp_oSQLRequst;
	tmp_oSQLRequst.sql_id = "oracle_query_test";
	{
		ResultSetPtr pSet = m_pOracleClient->Exec(tmp_oSQLRequst);
		if (!pSet->IsValid())
		{
			ICC_LOG_DEBUG(m_pLog, "exec sql failed! error:%s sql:%s", pSet->GetErrorMsg().c_str(), pSet->GetSQL().c_str());
			return;
		}

		ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", pSet->GetSQL().c_str());

		int iCounter = CResultSetCounter::Instance()->GetCounter();
		ICC_LOG_DEBUG(m_pLog, "enter resultset counter is %d", iCounter);

		int i = 0;
		while (pSet->Next())
		{
			i++;
			std::string strID = pSet->GetValue("ID");
			std::string strUser = pSet->GetValue("NAME");
			std::string strAge = pSet->GetValue("AGE");

			ICC_LOG_DEBUG(m_pLog, "the %d data: ID=%s,USER=%s,AGE=%s", i, strID.c_str(), strUser.c_str(), strAge.c_str());
		}
	}*/


	/*std::string strSql = "SELECT * FROM TEST.\"myuser\"";
	{
		ResultSetPtr pSet = m_pOracleClient->ExecQuery(strSql);
		if (!pSet->IsValid())
		{
			return;
		}

		int i = 0;
		while (pSet->Next())
		{
			i++;
			std::string strID = pSet->GetValue("ID");
			std::string strUser = pSet->GetValue("NAME");
			std::string strAge = pSet->GetValue("AGE");

			ICC_LOG_DEBUG(m_pLog, "the %d data: ID=%s,USER=%s,AGE=%s", i, strID.c_str(), strUser.c_str(), strAge.c_str());
		}
	}*/

	int iCounter = CResultSetCounter::Instance()->GetCounter();
	ICC_LOG_DEBUG(m_pLog, "resultset counter is %d", iCounter);

	m_pOracleClient->UnInit();
}

bool CBusinessImpl::_ReadFeedbackData(const std::string& strBeginTimes, const std::string& strEndTime, std::vector<PROTOCOL::CFeedBackRequest>& vecFeedbackDatas)
{
	DataBase::SQLRequest tmp_oSQLRequst;
	tmp_oSQLRequst.sql_id = "oracle_query_feedback";
	tmp_oSQLRequst.param["mode_name"] = m_strTableModeName;
	tmp_oSQLRequst.param["begin_time"] = strBeginTimes;
	tmp_oSQLRequst.param["end_time"] = strEndTime;
	{
		ResultSetPtr pSet = m_pOracleClient->Exec(tmp_oSQLRequst);
		if (!pSet->IsValid())
		{
			ICC_LOG_WARNING(m_pLog, "resultset is invalid! error:%s sql:%s", pSet->GetErrorMsg().c_str(), pSet->GetSQL().c_str());
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", pSet->GetSQL().c_str());

		std::vector<std::string> vecFiledNames = pSet->GetFieldNames();

		int iCounter = CResultSetCounter::Instance()->GetCounter();
		ICC_LOG_DEBUG(m_pLog, "enter resultset counter is %d", iCounter);

		while (pSet->Next())
		{
			PROTOCOL::CFeedBackRequest requestData;
			for (unsigned int index = 0; index < vecFiledNames.size(); ++index)
			{
				requestData.m_oBody.m_mapDatas.insert(std::make_pair(vecFiledNames[index], pSet->GetValue(vecFiledNames[index])));
			}
			requestData.m_oBody.m_mapDatas.insert(std::make_pair("msg_source", SOURCE_FOUNDER));
			vecFeedbackDatas.push_back(requestData);
		}
	}

	return true;
}

//bool CBusinessImpl::_SendFeedbackData(std::vector<PROTOCOL::CFeedBackRequest>& vecFeedbackDatas)
//{
//	for (unsigned int i = 0; i < vecFeedbackDatas.size(); ++i)
//	{
//		vecFeedbackDatas[i].m_oHeader.m_strSystemID = "ICC";
//		vecFeedbackDatas[i].m_oHeader.m_strMsgid = m_pString->CreateGuid();
//		vecFeedbackDatas[i].m_oHeader.m_strCmd = "add_or_update_feedback_request";
//		vecFeedbackDatas[i].m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
//		vecFeedbackDatas[i].m_oHeader.m_strRequest = "queue_alarm";
//		vecFeedbackDatas[i].m_oHeader.m_strRequestType = "0";
//
//		std::string strMessage = vecFeedbackDatas[i].ToString(m_pJsonFty->CreateJson());
//		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(strMessage));
//		ICC_LOG_DEBUG(m_pLog, "send message complete: [%s]", strMessage.c_str());
//	}
//
//	return true;
//}

std::string CBusinessImpl::FromLocalToUTC(const std::string& strLocalDateTime)
{
	DateTime::CDateTime curTime = m_pDateTime->FromString(strLocalDateTime);
	if (curTime == ERROR_DATE_TIME)
	{
		return "";
	}

	DateTime::CDateTime realTime = m_pDateTime->AddHours(curTime, -8);

	return m_pDateTime->ToString(realTime);
}

std::string CBusinessImpl::FromUTCToLocal(const std::string& strUTCDateTime)
{
	DateTime::CDateTime curTime = m_pDateTime->FromString(strUTCDateTime);
	if (curTime == ERROR_DATE_TIME)
	{
		return "";
	}

	DateTime::CDateTime realTime = m_pDateTime->AddHours(curTime, 8);

	return m_pDateTime->ToString(realTime);
}

bool CBusinessImpl::_ReadProcessDataByProcessID(std::vector<std::map<std::string, std::string>>& datas,std::vector<PROTOCOL::CAddOrUpdateProcessRequest::CProcessData>& vecProcessDatas)
{
	unsigned int iCount = datas.size();
	for (unsigned int i = 0; i < iCount; ++i)
	{
		/*DataBase::SQLRequest l_oSQLRequst;
		l_oSQLRequst.sql_id = "oracle_query_processdata_by_pjdbh";
		l_oSQLRequst.param["mode_name"] = m_strTableModeName;
		std::map<std::string, std::string>::const_iterator itr;
		for (itr = datas[i].begin(); itr != datas[i].end(); ++itr)
		{
			if (itr->first == "PJDBH")
			{
				l_oSQLRequst.param["PJDBH"] = itr->second;
				ICC_LOG_DEBUG(m_pLog, "PJDBH:[%s]", itr->second.c_str());
			}
		}
		int iCounter = CResultSetCounter::Instance()->GetCounter();
		ResultSetPtr pSet = m_pOracleClient->Exec(l_oSQLRequst);
		if (!pSet->IsValid())
		{
			ICC_LOG_WARNING(m_pLog, "resultset is invalid! error:%s sql:%s", pSet->GetErrorMsg().c_str(), pSet->GetSQL().c_str());
			return false;
		}
		ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:[%s],size:[%d]", pSet->GetSQL().c_str(), pSet->RecordSize());
		
		ICC_LOG_DEBUG(m_pLog, "enter resultset counter is %d", iCounter);*/

		PROTOCOL::CAddOrUpdateProcessRequest::CProcessData processData;
		//while(pSet->Next())
		//{
		processData.m_strMsgSource = SOURCE_FOUNDER;
		processData.m_strDispatchDeptDistrictCode = datas[i]["XZQHDM"];
		processData.m_strAlarmID = datas[i]["JJDBH"];
		//processData.m_strDispatchDeptCode = pSet->GetValue("PJDWDM");
		//processData.m_strDispatchDeptName = pSet->GetValue("PJDWMC");
		//processData.m_strDispatchCode = pSet->GetValue("PJYBH");
		//processData.m_strDispatchName = pSet->GetValue("PJYXM");
		processData.m_strRecordID = datas[i]["FKLYH"];
		processData.m_strDispatchSuggestion = datas[i]["CJCZQK"];
		processData.m_strProcessDeptCode = datas[i]["FKDWDM"];
		processData.m_strProcessDeptName = datas[i]["FKDWMC"];
		//std::string strPjsj = pSet->GetValue("PJSJ");
		//std::string strUtcPjsj = FromLocalToUTC(strPjsj);
		//processData.m_strTimeSubmit = strUtcPjsj;
		//std::string strXtzdddsj = pSet->GetValue("XTZDDDSJ");
		//std::string strUtcXtzddsj = FromLocalToUTC(strXtzdddsj);
		//processData.m_strTimeArrived = strUtcXtzddsj;
		//std::string strPdqssj = pSet->GetValue("PDQSSJ");;
		//std::string strUtcPdqssj = FromLocalToUTC(strPdqssj);
		//processData.m_strTimeSigned = strUtcPdqssj;
		processData.m_strProcessName = datas[i]["FKYXM"];
		processData.m_strProcessCode = datas[i]["FKYBH"];
		processData.m_strDispatchPersonnel = datas[i]["CDRC"];
		processData.m_strDispatchVehicles = datas[i]["CDCC"];
		processData.m_strDispatchBoats = datas[i]["CDCT"];
		processData.m_strState = datas[i]["JQCLZTDM"];
		processData.m_strProcessObjectName = datas[i]["FKYXM"];
		processData.m_strProcessObjectCode = datas[i]["FKYBH"];
		std::string strCjsj = datas[i]["rksj"];
		std::string strUtcCjsj = FromLocalToUTC(strCjsj);
		processData.m_strCreateTime = strUtcCjsj;
		std::string strGxsj = datas[i]["GXSJ"];
		std::string strUtcGxsj = FromLocalToUTC(strGxsj);
		processData.m_strUpdateTime = strUtcGxsj;
		std::string strTmpState = datas[i]["TBZT"];
		if (strTmpState == "1")
		{
			processData.m_strState = "17";
		}
		processData.m_strOverRemark = datas[i]["CJCZQK"];
		vecProcessDatas.push_back(processData);
		//}
	}
	return true;
}

std::string CBusinessImpl::_ReadProcessIDByAlarmID(std::string &strAlarmId)
{
	std::string strProcessId;
	DataBase::SQLRequest tmp_oSQLRequst;
	tmp_oSQLRequst.sql_id = "select_icc_t_pjdb_process_id_by_alarm_id_and_create_time_mininum";
	tmp_oSQLRequst.param["alarm_id"] = strAlarmId;
	std::string strTime = m_pDateTime->GetAlarmIdTime(strAlarmId);
	if (strTime != "")
	{
		tmp_oSQLRequst.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime);
		tmp_oSQLRequst.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(tmp_oSQLRequst);
	if (!l_pResult->IsValid())
	{
		ICC_LOG_WARNING(m_pLog, "resultset is invalid! sql:%s", l_pResult->GetSQL().c_str())
	}

	ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", l_pResult->GetSQL().c_str());
	if(l_pResult->Next())
	{
		strProcessId = l_pResult->GetValue("pjdbh");
	}
	return strProcessId;
}

//bool CBusinessImpl::_ReadProcessIDByAlarmIDandCode(std::string& strAlarmId, std::string& strProcessCode, std::string& strProcessId, std::string& strDispathcDeptName, std::string& strProcessDeptName)
//{
//	DataBase::SQLRequest tmp_oSQLRequst;
//	tmp_oSQLRequst.sql_id = "postgres_query_processdata_by_jjdbh_and_cjdwdm";
//	tmp_oSQLRequst.param["jjdbh"] = strAlarmId;
//	tmp_oSQLRequst.param["cjdwdm"] = strProcessCode;
//
//	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(tmp_oSQLRequst);
//	if (!l_pResult->IsValid())
//	{
//		ICC_LOG_WARNING(m_pLog, "resultset is invalid! sql:%s", l_pResult->GetSQL().c_str());
//		return false;
//	}
//
//	ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", l_pResult->GetSQL().c_str());
//	if (l_pResult->Next())
//	{
//		strProcessId = l_pResult->GetValue("pjdbh");
//		strDispathcDeptName = l_pResult->GetValue("pjdwmc");
//		strProcessDeptName = l_pResult->GetValue("cjdwmc");
//	}
//	ICC_LOG_DEBUG(m_pLog, "%s,%s,%s", strProcessId.c_str(), strDispathcDeptName.c_str(), strProcessDeptName.c_str());
//	return true;
//}



bool CBusinessImpl::_ProcAlarmData(const std::string& strBeginTimes, const std::string& strEndTime)
{
	ICC_LOG_DEBUG(m_pLog, "process alarm data begin: begintime:[%s], endtime:[%s]", strBeginTimes.c_str(), strEndTime.c_str());

	std::vector<std::map<std::string, std::string>> datas;
	if (!_ReadAlarmDataFromOracle(strBeginTimes, strEndTime, datas))
	{
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "read alarm data from oracle complete. count:%d, begintime:[%s], endtime:[%s]", datas.size(), strBeginTimes.c_str(), strEndTime.c_str());

	if (!_WriteAlarmDataToICC(datas))
	{
		return false;
	}
	return true;
	ICC_LOG_DEBUG(m_pLog, "write alarm data complete! begintime:[%s], endtime:[%s]", strBeginTimes.c_str(), strEndTime.c_str());
}

bool CBusinessImpl::_ReadAlarmDataFromOracle(const std::string& strBeginTime, const std::string& strEndTime, std::vector<std::map<std::string, std::string>>& datas)
{
	DataBase::SQLRequest tmp_oSQLRequst;
	tmp_oSQLRequst.sql_id = "oracle_query_alarm_by_updatetime";
	tmp_oSQLRequst.param["mode_name"] = m_strTableModeName;
	tmp_oSQLRequst.param["begin_time"] = strBeginTime;
	tmp_oSQLRequst.param["end_time"] = strEndTime;
	{
		ResultSetPtr pSet = m_pOracleClient->Exec(tmp_oSQLRequst);
		if (!pSet->IsValid())
		{
			ICC_LOG_WARNING(m_pLog, "resultset is invalid! error:%s sql:%s", pSet->GetErrorMsg().c_str(), pSet->GetSQL().c_str());
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", pSet->GetSQL().c_str());

		std::vector<std::string> vecFiledNames = pSet->GetFieldNames();

		int iCounter = CResultSetCounter::Instance()->GetCounter();
		ICC_LOG_DEBUG(m_pLog, "enter resultset counter is %d", iCounter);

		while (pSet->Next())
		{
			std::map<std::string, std::string> mapTmps;
			for (unsigned int index = 0; index < vecFiledNames.size(); ++index)
			{
				mapTmps.insert(std::make_pair(vecFiledNames[index], pSet->GetValue(vecFiledNames[index])));
			}
			datas.push_back(mapTmps);
		}
	}

	return true;
}

bool ICC::CBusinessImpl::_WriteAlarmDataToICC(std::vector<std::map<std::string, std::string>>& datas)
{
	unsigned int iCount = datas.size();
	for (unsigned int i = 0; i < iCount; ++i)
	{
		std::map<std::string, std::string>::const_iterator itr;
		itr = datas[i].find("JJDBH");
		if (itr == datas[i].end())
		{
			return false;
		}
		std::string strAlarmId = itr->second;
		if (strAlarmId.empty())
		{
			return false;
		}

		DataBase::SQLRequest tmp_oSQLRequst;
		DataBase::IResultSetPtr pSet;
		bool bExist = false;
		if (!_ExistAlarmDataByPostgres(strAlarmId, bExist))
		{
			return false;
		}

		if (!bExist)
		{
			tmp_oSQLRequst.sql_id = "postgres_insert_icc_t_jjdb";
			for (itr = datas[i].begin(); itr != datas[i].end(); ++itr)
			{
				std::string strTmp;
				if (itr->first == "BJSJ" || itr->first == "JJSJ" || itr->first == "JJWCSJ" || itr->first == "rksj" || itr->first == "GXSJ")
				{
					if (strTmp.empty() && itr->first == "rksj")
					{
						strTmp = datas[i]["JJSJ"];
					}
					else if (strTmp.empty() && itr->first == "GXSJ")
					{
						strTmp = datas[i]["JJWCSJ"];
					}

					strTmp = _StandardDateTime(itr->second);
					std::string Tmp = FromLocalToUTC(strTmp);
					tmp_oSQLRequst.param[itr->first] = Tmp;
				}
				else
				{
					tmp_oSQLRequst.param[itr->first] = itr->second;
				}
			}
			//区分是方正的自接警
			tmp_oSQLRequst.param["JJFS"] = "4";

			//对方正的证件代码转为ICC内部的
			if(datas[i]["BJRZJDM"] == "111"|| datas[i]["BJRZJDM"] == "114"|| datas[i]["BJRZJDM"] == "133"||datas[i]["BJRZJDM"] == "335"|| datas[i]["BJRZJDM"] == "414"|| datas[i]["BJRZJDM"] == "513")
			{
				if (datas[i]["BJRZJDM"] == "111")
				{
					tmp_oSQLRequst.param["BJRZJDM"] = "01";
				}
				if (datas[i]["BJRZJDM"] == "114")
				{
					tmp_oSQLRequst.param["BJRZJDM"] = "02";
				}
				if (datas[i]["BJRZJDM"] == "133")
				{
					tmp_oSQLRequst.param["BJRZJDM"] = "03";
				}
				if (datas[i]["BJRZJDM"] == "335")
				{
					tmp_oSQLRequst.param["BJRZJDM"] = "04";
				}
				if (datas[i]["BJRZJDM"] == "414")
				{
					tmp_oSQLRequst.param["BJRZJDM"] = "05";
				}
				if (datas[i]["BJRZJDM"] == "513")
				{
					tmp_oSQLRequst.param["BJRZJDM"] = "06";
				}
			}
			else
			{
				tmp_oSQLRequst.param["BJRZJDM"] = "07";
			}
			ICC_LOG_DEBUG(m_pLog,"BJRZJDM:[%s]", tmp_oSQLRequst.param["BJRZJDM"].c_str())
			pSet = m_pDBConn->Exec(tmp_oSQLRequst);
			ICC_LOG_DEBUG(m_pLog, "exec sql! sql:%s", pSet->GetSQL().c_str());
			if (!pSet->IsValid())
			{
				return false;
			}
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "no alarm data need update");
		}
		if (!datas[i]["SJCPH"].empty())
		{
			tmp_oSQLRequst.sql_id = "insert_icc_t_jqclb";
			tmp_oSQLRequst.param["alarm_id"] = datas[i]["JJDBH"];
			tmp_oSQLRequst.param["car_number"] = datas[i]["SJCPH"];
			tmp_oSQLRequst.param["car_type"] = datas[i]["SJCHPZLDM"];
			tmp_oSQLRequst.param["id"] = m_pString->CreateGuid();
			tmp_oSQLRequst.param["car_user_name"] = "";
			tmp_oSQLRequst.param["car_user_phone"] = "";
			tmp_oSQLRequst.param["car_user_number"] = "";
			tmp_oSQLRequst.param["is_hazardous_vehicle"] = "0";
			tmp_oSQLRequst.param["create_time"] = "";
			tmp_oSQLRequst.param["update_time"] = m_pDateTime->CurrentDateTimeStr();
			tmp_oSQLRequst.param["deleted"] = "0";
			tmp_oSQLRequst.param["createTeminal"] = "";
			tmp_oSQLRequst.param["updateTeminal"] = "";
			pSet = m_pDBConn->Exec(tmp_oSQLRequst);
			ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", pSet->GetSQL().c_str());
			if (!pSet->IsValid())
			{
				ICC_LOG_DEBUG(m_pLog, "exec sql failed! sql:%s", pSet->GetErrorMsg().c_str());
				return false;
			}
		}
		
	}
	return true;
}

bool ICC::CBusinessImpl::_ExistAlarmDataByPostgres(const std::string& strAlarmId, bool& bExist)
{
	bExist = false;
	DataBase::SQLRequest tmp_oSQLRequst;
	tmp_oSQLRequst.sql_id = "postgres_judge_alarm_exist";
	tmp_oSQLRequst.param["jjdbh"] = strAlarmId;
	std::string strTime = m_pDateTime->GetAlarmIdTime(strAlarmId);
	if (strTime != "")
	{
		tmp_oSQLRequst.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		tmp_oSQLRequst.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}
	{
		DataBase::IResultSetPtr pSet = m_pDBConn->Exec(tmp_oSQLRequst);
		if (!pSet->IsValid())
		{
			ICC_LOG_WARNING(m_pLog, "resultset is invalid! sql:%s", pSet->GetSQL().c_str());
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", pSet->GetSQL().c_str());

		std::vector<std::string> vecFiledNames = pSet->GetFieldNames();

		int iCounter = CResultSetCounter::Instance()->GetCounter();
		ICC_LOG_DEBUG(m_pLog, "enter resultset counter is %d", iCounter);

		do
		{
			if (pSet->Next())
			{
				std::string strCount = pSet->GetValue("count");
				if (strCount.empty())
				{
					ICC_LOG_ERROR(m_pLog, "not find field:COUNT");
					return false;
				}

				if (std::stoi(strCount) > 0)
				{
					bExist = true;
					break;
				}
			}
		} while (false);


	}
	return true;
}
bool CBusinessImpl::_SendFeedbackData(std::vector<PROTOCOL::CAddOrUpdateProcessRequest::CProcessData>& vecProcessDatas)
{
	unsigned int iCount = vecProcessDatas.size();
	DataBase::SQLRequest l_SQLRequest;
	DataBase::IResultSetPtr lSet;
	std::string strProcessId; 
	std::string strDispatchDeptName;
	std::string strProcessDeptName;

	std::vector<PROTOCOL::CAlarmInfo> m_vecData;
	PROTOCOL::CAddOrUpdateProcessRequest::CProcessData l_ProcessData;
	for (unsigned int i = 0; i < iCount; ++i)
	{
		l_ProcessData.m_strMsgSource = SOURCE_FOUNDER;
		PROTOCOL::CAddOrUpdateAlarmWithProcessRequest l_CAddOrUpdateProcessRequest;
		l_CAddOrUpdateProcessRequest.m_oHeader.m_strCmd = "add_or_update_alarm_and_process_request";
		l_CAddOrUpdateProcessRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		l_CAddOrUpdateProcessRequest.m_oHeader.m_strRequest = "queue_alarm";
		l_CAddOrUpdateProcessRequest.m_oHeader.m_strRequestType = "0";
		l_CAddOrUpdateProcessRequest.m_oHeader.m_strRequestFlag = "MQ";
		l_CAddOrUpdateProcessRequest.m_oHeader.m_strMsgId = m_pString->CreateGuid();
		l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strID = vecProcessDatas[i].m_strAlarmID;
		l_SQLRequest.sql_id = "select_max_state_in_feedback_table_by_alarm_id";
		l_SQLRequest.param["alarm_id"] = vecProcessDatas[i].m_strAlarmID;
		l_SQLRequest.param["mode_name"] = m_strTableModeName;
		ResultSetPtr pSet = m_pOracleClient->Exec(l_SQLRequest);
		if (!pSet->IsValid())
		{
			ICC_LOG_WARNING(m_pLog, "resultset is invalid! error:%s sql:%s", pSet->GetErrorMsg().c_str(), pSet->GetSQL().c_str());
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", pSet->GetSQL().c_str());

		int iCounter = CResultSetCounter::Instance()->GetCounter();
		ICC_LOG_DEBUG(m_pLog, "enter resultset counter is %d", iCounter);

		ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", pSet->GetSQL().c_str());
		if (pSet->Next())
		{
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strState = pSet->GetValue("JQCLZTDM");
		}
		l_SQLRequest.sql_id = "postgres_query_processdata_by_jjdbh_and_cjdwdm";
		l_SQLRequest.param["jjdbh"] = vecProcessDatas[i].m_strAlarmID;
		std::string strTime = m_pDateTime->GetAlarmIdTime(vecProcessDatas[i].m_strAlarmID);
		if (strTime != "")
		{
			l_SQLRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime);
			l_SQLRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
		}

		l_SQLRequest.param["cjdwdm"] = vecProcessDatas[i].m_strProcessDeptCode;

		lSet = m_pDBConn->Exec(l_SQLRequest);
		if (!lSet->IsValid())
		{
			ICC_LOG_WARNING(m_pLog, "resultset is invalid! sql:%s", lSet->GetSQL().c_str());
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", lSet->GetSQL().c_str());
		if (lSet->Next())
		{
			strDispatchDeptName = lSet->GetValue("pjdwmc");
			strProcessDeptName = lSet->GetValue("cjdwmc");
		}
		ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", lSet->GetSQL().c_str());
		vecProcessDatas[i].m_strDispatchDeptName = strDispatchDeptName;
		vecProcessDatas[i].m_strProcessDeptName = strProcessDeptName;
		l_CAddOrUpdateProcessRequest.m_oBody.m_vecProcessData.push_back(vecProcessDatas[i]);
		l_SQLRequest.sql_id = "select_jjdb_in_postgres_by_alarm_id";
		l_SQLRequest.param["alarm_id"] = vecProcessDatas[i].m_strAlarmID;

		strTime = m_pDateTime->GetAlarmIdTime(vecProcessDatas[i].m_strAlarmID);
		if (strTime != "")
		{
			l_SQLRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
			l_SQLRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
		}

		lSet = m_pDBConn->Exec(l_SQLRequest);
		if (!lSet->IsValid())
		{
			ICC_LOG_WARNING(m_pLog, "resultset is invalid! error:%s, sql:%s", lSet->GetErrorMsg().c_str(), lSet->GetSQL().c_str());
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "exec sql success! sql:%s", lSet->GetSQL().c_str());
		if (lSet->Next())
		{
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strAdminDeptCode = lSet->GetValue("gxdwdm");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strAdminDeptName = lSet->GetValue("gxdwmc");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strAdminDeptOrgCode = lSet->GetValue("gxdwdmbs");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strAlarmAddr = lSet->GetValue("bjdz");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strBusinessState = lSet->GetValue("dqywzt");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strCalledNoType = lSet->GetValue("jjlx");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strCallerAddr = lSet->GetValue("jqdz");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strCallerGender = lSet->GetValue("bjrxbdm");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strCallerID = lSet->GetValue("bjrzjhm");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strCallerIDType = lSet->GetValue("bjrzjdm");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strCallerName = lSet->GetValue("bjrmc");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strCallerNo = lSet->GetValue("bjdh");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strCallerUserName = lSet->GetValue("bjdhyhm");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strContent = lSet->GetValue("bjnr");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strContactNo = lSet->GetValue("lxdh");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strCreateTime = lSet->GetValue("rksj");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strCreateUser = lSet->GetValue("cjry");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strDeleteFlag = lSet->GetValue("scbs");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strDescOfDead = lSet->GetValue("swryqksm");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strDescOfInjured = lSet->GetValue("ssryqksm");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strDescOfTrapped = lSet->GetValue("bkryqksm");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strEmergencyRescueLevel = lSet->GetValue("yjjydjdm");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strErpetratorsNumber = lSet->GetValue("zars");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strFirstSubmitTime = lSet->GetValue("jjwcsj");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strFirstType = lSet->GetValue("jqlbdm");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strFourthType = lSet->GetValue("jqzldm");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strID = lSet->GetValue("jjdbh");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strIsArmed = lSet->GetValue("ywcwq");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strIsExplosionOrLeakage = lSet->GetValue("ywbzxl");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strIsFeedBack = lSet->GetValue("fkbs");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strIsForeignLanguage = lSet->GetValue("sfswybj");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strIsHazardousSubstances = lSet->GetValue("ywwxwz");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strIsInvalid = lSet->GetValue("wxbs");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strIsMerge = lSet->GetValue("hbbs");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strIsOver = lSet->GetValue("sfja");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strIsProcessFlagSynchronized = lSet->GetValue("tbcjbs");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strIsSameForBackground = lSet->GetValue("ybjxxyz");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strIsVisitor = lSet->GetValue("hfbs");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strJurisdictionalOrgcode = lSet->GetValue("tzhgxdwdm");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strJurisdictionalOrgidentifier = lSet->GetValue("tzhgxdwdmbs");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strJurisdictionalOrgname = lSet->GetValue("tzhgxdwmc");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strLatitude = lSet->GetValue("bjryzb");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strLevel = lSet->GetValue("jqdjdm");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strLongitude = lSet->GetValue("bjrxzb");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strManualLatitude = lSet->GetValue("fxdwwd");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strManualLongitude = lSet->GetValue("fxdwjd");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strMergeID = lSet->GetValue("glzjjdbh");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strMergeType = lSet->GetValue("gllx");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strMsgSource = lSet->GetValue("lhlx");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strPrivacy = lSet->GetValue("smbm");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strReceiptCode = lSet->GetValue("jjybh");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strReceiptDeptCode = lSet->GetValue("jjdwdm");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode = lSet->GetValue("xzqhdm");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strReceiptDeptName = lSet->GetValue("jjdwmc");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strReceiptSrvName = lSet->GetValue("jjfs");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strReceiptName = lSet->GetValue("jjyxm");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strReceivedTime = lSet->GetValue("jjsj");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strRemark = lSet->GetValue("bcjjnr");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strSeatNo = lSet->GetValue("jjxwh");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strSecondType = lSet->GetValue("jqlxdm");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strSourceType = lSet->GetValue("jqlyfs");
			//l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strState = vecProcessDatas[i].m_strState;
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strSymbolCode = lSet->GetValue("tzdbh");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strThirdType = lSet->GetValue("jqxldm");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strTime = lSet->GetValue("bjsj");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strTitle = lSet->GetValue("jqbq");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strUpdateTime = m_pDateTime->CurrentDateTimeStr();
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strUpdateUser = vecProcessDatas[i].m_strProcessName;
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strUpdateUserDeptCode = vecProcessDatas[i].m_strProcessDeptCode;
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strUpdateUserDeptName = vecProcessDatas[i].m_strProcessDeptName;
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strUpdateUserName = vecProcessDatas[i].m_strProcessObjectName;
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strVehicleNo = lSet->GetValue("sjcph");
			l_CAddOrUpdateProcessRequest.m_oBody.m_oAlarm.m_strVehicleType = lSet->GetValue("sjchpzldm");
		}
		std::string l_strMessage = l_CAddOrUpdateProcessRequest.ToString(ICCGetIJsonFactory()->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
	}

	return true;
}

//bool CBusinessImpl::SendAlarmUpdateRequest(std::vector<PROTOCOL::CAlarmInfo>& vecProcessDatas)
//{
//	PROTOCOL::CAddOrUpdateAlarmWithProcessRequest l_oRequest;
//	std::vector< PROTOCOL::CAlarmInfo> vecAlarmDatas;
//	l_oRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();
//	l_oRequest.m_oHeader.m_strCmd = "add_or_update_alarm_and_process_request";
//	l_oRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
//	l_oRequest.m_oHeader.m_strRequest = "queue_alarm";
//	l_oRequest.m_oHeader.m_strRequestType = "0";
//	l_oRequest.m_oHeader.m_strRequestFlag = "MQ";
//
//	l_oRequest.m_oBody.m_oAlarm.m_strMsgSource = "FOUNDER";
//
//
//	std::string l_strMessage = l_oRequest.ToString(ICCGetIJsonFactory()->CreateJson());
//	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
//	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
//}

bool CBusinessImpl::UpdateOperateProcessLog(std::vector< PROTOCOL::CAddOrUpdateProcessRequest::CProcessData>& vecProcessDatas)
{
	unsigned int iCount = vecProcessDatas.size();
	std::string strOperate = "";
	std::vector<std::string> l_vecParamList;
	PROTOCOL::CAlarmLogSync l_oAlarmLogSync;
	for (unsigned int i = 0; i < iCount; i++)
	{
		l_vecParamList.clear();
		l_oAlarmLogSync.m_oBody.m_strID = m_pString->CreateGuid();
		l_oAlarmLogSync.m_oBody.m_strAlarmID = vecProcessDatas[i].m_strAlarmID;
		l_oAlarmLogSync.m_oBody.m_strProcessID = vecProcessDatas[i].m_strID;
		l_oAlarmLogSync.m_oBody.m_strSeatNo = vecProcessDatas[i].m_strSeatCode;
		//方正推送我们的警单更新状态为02时不写流水
		if (vecProcessDatas[i].m_strState == "02" && vecProcessDatas[i].m_strDispatchDeptCode == "510500000000")
		{
			continue;
		}
		////方正自接警写01的流水
		//if (vecProcessDatas[i].m_strState == "01")
		//{
		//	strOperate = "BS001001001";
		//	l_vecParamList.push_back(vecProcessDatas[i].m_strDispatchName);
		//	l_vecParamList.push_back(vecProcessDatas[i].m_strDispatchCode);
		//	l_vecParamList.push_back(vecProcessDatas[i].m_strDispatchDeptName);
		//}
		
		//ICC过去的派警单更新状态为02不写流水
		if (vecProcessDatas[i].m_strState == "02"&&vecProcessDatas[i].m_strDispatchDeptCode!="510500000000")
		{
			strOperate = "Dispatch_Send";
			l_vecParamList.push_back(vecProcessDatas[i].m_strDispatchDeptName);
			l_vecParamList.push_back(vecProcessDatas[i].m_strDispatchName);
			l_vecParamList.push_back(vecProcessDatas[i].m_strProcessDeptName);
			l_vecParamList.push_back(vecProcessDatas[i].m_strProcessObjectName);
			l_vecParamList.push_back(vecProcessDatas[i].m_strDispatchSuggestion);
		}
		/*if (vecProcessDatas[i].m_strState == "05")
		{
			strOperate = "BS001003007";
			l_vecParamList.push_back(vecProcessDatas[i].m_strProcessObjectName);
			l_vecParamList.push_back(vecProcessDatas[i].m_strProcessObjectCode);
			l_vecParamList.push_back(vecProcessDatas[i].m_strProcessDeptName);
		}
		if (vecProcessDatas[i].m_strState == "06")
		{
			strOperate = "BS001003008";
			l_vecParamList.push_back(vecProcessDatas[i].m_strProcessObjectName);
			l_vecParamList.push_back(vecProcessDatas[i].m_strProcessObjectCode);
			l_vecParamList.push_back(vecProcessDatas[i].m_strProcessDeptName);
		}
		if (vecProcessDatas[i].m_strState == "07")
		{
			strOperate = "BS001003009";
			l_vecParamList.push_back(vecProcessDatas[i].m_strProcessObjectName);
			l_vecParamList.push_back(vecProcessDatas[i].m_strProcessObjectCode);
			l_vecParamList.push_back(vecProcessDatas[i].m_strProcessDeptName);
			l_vecParamList.push_back(vecProcessDatas[i].m_strOverRemark);
		}*/
		if (vecProcessDatas[i].m_strState == "17" && vecProcessDatas[i].m_strProcessObjectType != "01")
		{
			strOperate = "BS001003011";
			//l_vecParamList.push_back(vecProcessDatas[i].m_strProcessObjectName);
			//l_vecParamList.push_back(vecProcessDatas[i].m_strProcessObjectCode);
			l_vecParamList.push_back(vecProcessDatas[i].m_strProcessDeptName);
		}
		l_oAlarmLogSync.m_oBody.m_strOperate = strOperate;
		l_oAlarmLogSync.m_oBody.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
		//流水内容为空时，不向前端发送流水同步,防止时间轴错乱
		if (l_oAlarmLogSync.m_oBody.m_strOperateContent.empty()||l_oAlarmLogSync.m_oBody.m_strOperate.empty())
		{
			continue;
		}
		l_oAlarmLogSync.m_oBody.m_strFromType = "User";
		l_oAlarmLogSync.m_oBody.m_strFromObject = vecProcessDatas[i].m_strDispatchCode;
		l_oAlarmLogSync.m_oBody.m_strFromObjectOrgName = vecProcessDatas[i].m_strDispatchDeptName;
		l_oAlarmLogSync.m_oBody.m_strFromObjectOrgCode = vecProcessDatas[i].m_strDispatchDeptOrgCode;
		l_oAlarmLogSync.m_oBody.m_strToType = vecProcessDatas[i].m_strProcessObjectType;
		l_oAlarmLogSync.m_oBody.m_strToObject = vecProcessDatas[i].m_strProcessObjectCode;
		l_oAlarmLogSync.m_oBody.m_strToObjectName = vecProcessDatas[i].m_strProcessObjectName;
		l_oAlarmLogSync.m_oBody.m_strToObjectOrgName = vecProcessDatas[i].m_strProcessObjectName;
		l_oAlarmLogSync.m_oBody.m_strToObjectOrgCode = vecProcessDatas[i].m_strProcessDeptOrgCode;
		l_oAlarmLogSync.m_oBody.m_strCreateUser = vecProcessDatas[i].m_strCreateUser;
		l_oAlarmLogSync.m_oBody.m_strCreateTime = vecProcessDatas[i].m_strCreateTime;
		l_oAlarmLogSync.m_oBody.m_strSourceName = "Founder";

		l_oAlarmLogSync.m_oBody.m_strFromOrgIdentifier = l_oAlarmLogSync.m_oBody.m_strFromObjectOrgCode;
		l_oAlarmLogSync.m_oBody.m_strToObjectOrgIdentifier = l_oAlarmLogSync.m_oBody.m_strToObjectOrgCode;
		l_oAlarmLogSync.m_oBody.m_strDescription = vecProcessDatas[i].m_strDispatchSuggestion;
		l_oAlarmLogSync.m_oBody.m_strCreateOrg = vecProcessDatas[i].m_strDispatchDeptOrgCode;

		//TODO::记录警情流水日志
		{
			
			std::string l_strCurTime(m_pDateTime->ToString(m_pDateTime->CurrentDateTime(), DateTime::DEFAULT_DATETIME_STRING_FORMAT));
			l_oAlarmLogSync.m_oBody.m_strCreateTime = l_strCurTime;

			InsertDBAlarmLogInfo(l_oAlarmLogSync.m_oBody);
		}
		//发同步
		{
			std::string l_strGuid = m_pString->CreateGuid();
			l_oAlarmLogSync.m_oHeader.m_strSystemID = "ICC";
			l_oAlarmLogSync.m_oHeader.m_strSubsystemID = "ICC";// SUBSYSTEMID;
			l_oAlarmLogSync.m_oHeader.m_strMsgid = l_strGuid;
			l_oAlarmLogSync.m_oHeader.m_strRelatedID = "";
			l_oAlarmLogSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
			l_oAlarmLogSync.m_oHeader.m_strCmd = "alarm_log_sync";
			l_oAlarmLogSync.m_oHeader.m_strRequest = "topic_alarm";//?topic_alarm_sync
			l_oAlarmLogSync.m_oHeader.m_strRequestType = "1";
			l_oAlarmLogSync.m_oHeader.m_strResponse = "VCS";
			l_oAlarmLogSync.m_oHeader.m_strResponseType = "";

			JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
			std::string l_strMessage = l_oAlarmLogSync.ToString(l_pIJson, ICCGetIJsonFactory()->CreateJson());

			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
			ICC_LOG_DEBUG(m_pLog, "send message:[%s],m_strContent:%s", l_strMessage.c_str(), l_oAlarmLogSync.m_oBody.m_strOperateContent.c_str());
		}
	}
	return true;
}

bool CBusinessImpl::InsertDBAlarmLogInfo(PROTOCOL::CAlarmLogSync::CBody& p_AlarmLogInfo)
{
	//TODO::记录警情流水日志
	DataBase::SQLRequest l_tSQLReqInsertAlarm;
	//使用带毫秒的流水
	std::string l_strCurTime(m_pDateTime->ToString(m_pDateTime->CurrentDateTime(), DateTime::DEFAULT_DATETIME_STRING_FORMAT));
	if (p_AlarmLogInfo.m_strReceivedTime.empty())
	{
		//TODO::select_icc_t_jjdb_jjsj 查询jjsj
		std::string strTime = m_pDateTime->GetAlarmIdTime(p_AlarmLogInfo.m_strAlarmID);

		if (strTime != "")
		{
			DataBase::SQLRequest l_sqlReqeust;
			l_sqlReqeust.sql_id = "select_icc_t_jjdb_jjsj";
			l_sqlReqeust.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
			l_sqlReqeust.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
			l_sqlReqeust.param["jjdbh"] = p_AlarmLogInfo.m_strAlarmID;
			DataBase::IResultSetPtr l_pRSetPtr = m_pDBConn->Exec(l_sqlReqeust);
			if (!l_pRSetPtr->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "select_icc_t_jjdb_jjsj failed, error msg:[%s]", l_pRSetPtr->GetErrorMsg().c_str());
			}
			if (l_pRSetPtr->Next())
			{
				p_AlarmLogInfo.m_strReceivedTime = l_pRSetPtr->GetValue("jjsj");
			}
		}
	}

	if (p_AlarmLogInfo.m_strReceivedTime.empty())
	{
		p_AlarmLogInfo.m_strReceivedTime = l_strCurTime;
	}

	if (!PROTOCOL::CAlarmLogSync::SetLogInsertSql(p_AlarmLogInfo, l_tSQLReqInsertAlarm))
	{
		return false;
	}

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertAlarm, false);
	ICC_LOG_DEBUG(m_pLog, "sql icc_t_alarm_log:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert alarm log info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	// 数据库写流水太快问题、导致流水时间相同排序混乱，若有其他更合理方法，可修改
	m_pHelpTool->Sleep(1);
	return true;
}

std::string CBusinessImpl::BuildAlarmLogContent(std::vector<std::string> &p_vecParamList)
{
	JsonParser::IJsonPtr l_pIJson = m_pJsonFty->CreateJson();
	unsigned int l_iIndex = 0;
	for (auto it = p_vecParamList.cbegin(); it != p_vecParamList.cend(); it++)
	{
		l_pIJson->SetNodeValue("/param/" + std::to_string(l_iIndex), *it);
		l_iIndex++;
	}
	return l_pIJson->ToString();
}

bool CBusinessImpl::UpdateOperateFeedbackLog(std::vector< PROTOCOL::CAddOrUpdateProcessRequest::CProcessData>& vecProcessDatas)
{
	ICC_LOG_DEBUG(m_pLog, "Update Operate Feedback Log begin!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	unsigned int iCount = vecProcessDatas.size();
	std::string strOperate = "";
	std::vector<std::string> l_vecParamList;
	PROTOCOL::CAlarmLogSync l_oAlarmLogSync;
	for (unsigned int i = 0; i < iCount; i++)
	{
		ICC_LOG_DEBUG(m_pLog, "state[%s]", vecProcessDatas[i].m_strState.c_str());
		l_oAlarmLogSync.m_oBody.m_strID = m_pString->CreateGuid();
		l_oAlarmLogSync.m_oBody.m_strAlarmID = vecProcessDatas[i].m_strAlarmID;
		l_oAlarmLogSync.m_oBody.m_strProcessID = vecProcessDatas[i].m_strID;
		l_oAlarmLogSync.m_oBody.m_strSeatNo = vecProcessDatas[i].m_strSeatCode;
		////方正自接警写01的流水
		//if (vecProcessDatas[i].m_strState == "01")
		//{
		//	strOperate = "BS001001001";
		//	l_vecParamList.push_back(vecProcessDatas[i].m_strDispatchName);
		//	l_vecParamList.push_back(vecProcessDatas[i].m_strDispatchCode);
		//	l_vecParamList.push_back(vecProcessDatas[i].m_strDispatchDeptName);
		//}

		//ICC过去的派警单更新状态为02不写流水
		/*if (vecProcessDatas[i].m_strState == "02" && vecProcessDatas[i].m_strDispatchName != vecProcessDatas[i].m_strProcessName)
		{
			strOperate = "Dispatch_Send";
			l_vecParamList.push_back(vecProcessDatas[i].m_strDispatchDeptName);
			l_vecParamList.push_back(vecProcessDatas[i].m_strDispatchName);
			l_vecParamList.push_back(vecProcessDatas[i].m_strProcessDeptName);
			l_vecParamList.push_back(vecProcessDatas[i].m_strProcessObjectName);
			l_vecParamList.push_back(vecProcessDatas[i].m_strDispatchSuggestion);
		}*/
		if (vecProcessDatas[i].m_strState != "02" && vecProcessDatas[i].m_strState != "01")
		{
			l_vecParamList.clear();
			if (vecProcessDatas[i].m_strState == "03")
			{
				strOperate = "BS001003007";
				l_vecParamList.push_back(vecProcessDatas[i].m_strProcessObjectName);
				l_vecParamList.push_back(vecProcessDatas[i].m_strProcessObjectCode);
				l_vecParamList.push_back(vecProcessDatas[i].m_strProcessDeptName);
			}
			if (vecProcessDatas[i].m_strState == "05")
			{
				strOperate = "BS001003008";
				l_vecParamList.push_back(vecProcessDatas[i].m_strProcessObjectName);
				l_vecParamList.push_back(vecProcessDatas[i].m_strProcessObjectCode);
				l_vecParamList.push_back(vecProcessDatas[i].m_strProcessDeptName);
			}
			if (vecProcessDatas[i].m_strState == "06")
			{
				strOperate = "BS001003009";
				l_vecParamList.push_back(vecProcessDatas[i].m_strProcessObjectName);
				l_vecParamList.push_back(vecProcessDatas[i].m_strProcessObjectCode);
				l_vecParamList.push_back(vecProcessDatas[i].m_strProcessDeptName);
				l_vecParamList.push_back(vecProcessDatas[i].m_strOverRemark);
			}
			if (vecProcessDatas[i].m_strState == "07")
			{
				strOperate = "BS001003010";
				l_vecParamList.push_back(vecProcessDatas[i].m_strProcessObjectName);
				l_vecParamList.push_back(vecProcessDatas[i].m_strProcessObjectCode);
				l_vecParamList.push_back(vecProcessDatas[i].m_strProcessDeptName);
				l_vecParamList.push_back(vecProcessDatas[i].m_strOverRemark);
			}

			//退单是在派警单来进行退单
			/*if (vecProcessDatas[i].m_strState == "17")
			{
				strOperate = "BS001002028";
				l_vecParamList.push_back(vecProcessDatas[i].m_strProcessObjectName);
				l_vecParamList.push_back(vecProcessDatas[i].m_strProcessObjectCode);
				l_vecParamList.push_back(vecProcessDatas[i].m_strProcessDeptName);
			}*/
			l_oAlarmLogSync.m_oBody.m_strOperate = strOperate;
			l_oAlarmLogSync.m_oBody.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
			//流水内容为空时，不向前端发送流水同步,防止时间轴错乱
			if (l_oAlarmLogSync.m_oBody.m_strOperateContent.empty() || l_oAlarmLogSync.m_oBody.m_strOperate.empty())
			{
				continue;
			}
			l_oAlarmLogSync.m_oBody.m_strFromType = "User";
			l_oAlarmLogSync.m_oBody.m_strFromObject = vecProcessDatas[i].m_strDispatchCode;
			l_oAlarmLogSync.m_oBody.m_strFromObjectOrgName = vecProcessDatas[i].m_strDispatchDeptName;
			l_oAlarmLogSync.m_oBody.m_strFromObjectOrgCode = vecProcessDatas[i].m_strDispatchDeptOrgCode;
			l_oAlarmLogSync.m_oBody.m_strToType = vecProcessDatas[i].m_strProcessObjectType;
			l_oAlarmLogSync.m_oBody.m_strToObject = vecProcessDatas[i].m_strProcessObjectCode;
			l_oAlarmLogSync.m_oBody.m_strToObjectName = vecProcessDatas[i].m_strProcessObjectName;
			l_oAlarmLogSync.m_oBody.m_strToObjectOrgName = vecProcessDatas[i].m_strProcessObjectName;
			l_oAlarmLogSync.m_oBody.m_strToObjectOrgCode = vecProcessDatas[i].m_strProcessDeptOrgCode;
			l_oAlarmLogSync.m_oBody.m_strCreateUser = vecProcessDatas[i].m_strCreateUser;
			l_oAlarmLogSync.m_oBody.m_strCreateTime = vecProcessDatas[i].m_strCreateTime;
			l_oAlarmLogSync.m_oBody.m_strSourceName = "Founder";

			l_oAlarmLogSync.m_oBody.m_strFromOrgIdentifier = l_oAlarmLogSync.m_oBody.m_strFromObjectOrgCode;
			l_oAlarmLogSync.m_oBody.m_strToObjectOrgIdentifier = l_oAlarmLogSync.m_oBody.m_strToObjectOrgCode;
			l_oAlarmLogSync.m_oBody.m_strDescription = vecProcessDatas[i].m_strDispatchSuggestion;
			l_oAlarmLogSync.m_oBody.m_strCreateOrg = vecProcessDatas[i].m_strDispatchDeptOrgCode;

			//TODO::记录警情流水日志
			{

				std::string l_strCurTime(m_pDateTime->ToString(m_pDateTime->CurrentDateTime(), DateTime::DEFAULT_DATETIME_STRING_FORMAT));
				l_oAlarmLogSync.m_oBody.m_strCreateTime = l_strCurTime;

				InsertDBAlarmLogInfo(l_oAlarmLogSync.m_oBody);
			}
			//发同步
			{
				std::string l_strGuid = m_pString->CreateGuid();
				l_oAlarmLogSync.m_oHeader.m_strSystemID = "ICC";
				l_oAlarmLogSync.m_oHeader.m_strSubsystemID = "ICC";// SUBSYSTEMID;
				l_oAlarmLogSync.m_oHeader.m_strMsgid = l_strGuid;
				l_oAlarmLogSync.m_oHeader.m_strRelatedID = "";
				l_oAlarmLogSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
				l_oAlarmLogSync.m_oHeader.m_strCmd = "alarm_log_sync";
				l_oAlarmLogSync.m_oHeader.m_strRequest = "topic_alarm";//?topic_alarm_sync
				l_oAlarmLogSync.m_oHeader.m_strRequestType = "1";
				l_oAlarmLogSync.m_oHeader.m_strResponse = "VCS";
				l_oAlarmLogSync.m_oHeader.m_strResponseType = "";

				JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
				std::string l_strMessage = l_oAlarmLogSync.ToString(l_pIJson, ICCGetIJsonFactory()->CreateJson());

				m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
				ICC_LOG_DEBUG(m_pLog, "send message:[%s],m_strContent:%s", l_strMessage.c_str(), l_oAlarmLogSync.m_oBody.m_strOperateContent.c_str());
			}
		}
	}
	return true;
}