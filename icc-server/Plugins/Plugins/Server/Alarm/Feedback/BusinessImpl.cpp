#include "Boost.h"
#include "BusinessImpl.h"
#define STAFF_ID_NO 	"1"
#define STAFF_CODE	    "2"

void CBusinessImpl::OnInit()
{
    m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(ALARM_OBSERVER_CENTER);
    m_LockFacPtr = ICCGetILockFactory();  
    m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
    m_pString = ICCGetIStringFactory()->CreateString();
    m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
    m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
    m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
}

void CBusinessImpl::OnStart()
{	
    ICC_LOG_DEBUG(m_pLog,"OnStart enter! plugin = %s\n", MODULE_NAME);

    m_strAssistantPolice = m_pConfig->GetValue("ICC/Plugin/Synthetical/AssistantPolice", "");
    m_strPolice = m_pConfig->GetValue("ICC/Plugin/Synthetical/Police", "");
    m_strCodeMode = m_pConfig->GetValue("ICC/Plugin/Synthetical/CodeMode", "1");
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_all_info_request", OnCNotifiGetFeedbackRequest);
    ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_or_update_feedback_request", OnCNotifiSetFeedbackRequest);
    ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_feedback_by_process_request", OnCNotifiGetFeedbackByProcessRequest);
    ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_log_feedback_request", OnCGetAlarmLogFeedbackRequest);

	ICC_LOG_DEBUG(m_pLog,"OnStart cpmplete! plugin = %s,AssistantPolice=%s,Police=%s,CodeMode:[%s]\n", MODULE_NAME, m_strAssistantPolice.c_str(), m_strPolice.c_str(), m_strCodeMode.c_str());
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_DEBUG(m_pLog, "feedback stop success");
}

void CBusinessImpl::OnDestroy()
{

}

void CBusinessImpl::OnCNotifiGetFeedbackRequest(ObserverPattern::INotificationPtr p_pNotify)
{	
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

    JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
    PROTOCOL::CGetFeedBackRequest l_oGetFeedBackRequest;
	if (!l_oGetFeedBackRequest.ParseString(p_pNotify->GetMessages(), l_pIJson))
    {
		ICC_LOG_ERROR(m_pLog, "request is not json");
        return;
    }

    if (l_oGetFeedBackRequest.m_oBody.m_strAlarmID.empty() && l_oGetFeedBackRequest.m_oBody.m_strCallRefID.empty())
    {
        return;
    }
	std::string l_strAlarmId = l_oGetFeedBackRequest.m_oBody.m_strAlarmID;
    std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();
    std::string l_strGuid = m_pString->CreateGuid();
    std::string l_strCallRefId = l_oGetFeedBackRequest.m_oBody.m_strCallRefID;

	ICC_LOG_DEBUG(m_pLog, "get feedback by alarmid, [%s]", l_strAlarmId.c_str());

    PROTOCOL::CGetFeedBackRespond l_oGetFeedBackRespond;

    if (!l_oGetFeedBackRequest.m_oBody.m_strAlarmID.empty() && l_oGetFeedBackRequest.m_oBody.m_strAlarmID.size() < 15) {
        l_oGetFeedBackRespond.m_oHeader.m_strResult = "1";
        l_oGetFeedBackRespond.m_oHeader.m_strMsgId = l_oGetFeedBackRequest.m_oHeader.m_strMsgId;
        l_oGetFeedBackRespond.m_oHeader.m_strMsg = "m_strAlarmID is error ";
        JsonParser::IJsonPtr l_pIJsonRespond = ICCGetIJsonFactory()->CreateJson();
        std::string l_strMessage = l_oGetFeedBackRespond.ToString(l_pIJsonRespond);
        p_pNotify->Response(l_strMessage);
        return;
    }

    /*l_oGetFeedBackRespond.m_oHeader.m_strSystemID = "ICC";
    l_oGetFeedBackRespond.m_oHeader.m_strSubsystemID = "ICC-ApplicationServer";
    l_oGetFeedBackRespond.m_oHeader.m_strMsgid = l_strGuid;
    l_oGetFeedBackRespond.m_oHeader.m_strRelatedID = l_oGetFeedBackRequest.m_oHeader.m_strRelatedID;
    l_oGetFeedBackRespond.m_oHeader.m_strSendTime = l_strCurrentTime;
    l_oGetFeedBackRespond.m_oHeader.m_strCmd = "get_alarm_all_info_respond";
    l_oGetFeedBackRespond.m_oHeader.m_strRequest = l_oGetFeedBackRequest.m_oHeader.m_strRequest;
    l_oGetFeedBackRespond.m_oHeader.m_strRequestType = l_oGetFeedBackRequest.m_oHeader.m_strRequestType;
    l_oGetFeedBackRespond.m_oHeader.m_strResponse = "";
    l_oGetFeedBackRespond.m_oHeader.m_strResponseType = "";
    l_oGetFeedBackRespond.m_oHeader.m_strMsgId = l_oGetFeedBackRequest.m_oHeader.s*/
    l_oGetFeedBackRespond.m_oHeader = l_oGetFeedBackRequest.m_oHeader;

    std::vector<FeedbackData> l_vecFeedback;
    if (!l_strAlarmId.empty())
    {
        QueryFeedbackByAlarmID(l_strAlarmId, l_vecFeedback);
        ICC_LOG_DEBUG(m_pLog, "get feedback by alarm[%s] size[%d]", l_strAlarmId.c_str(), l_vecFeedback.size());
    }

    AlarmData l_tAlarm;
    GetAlarm(l_strAlarmId, l_tAlarm, l_strCallRefId);
    l_strAlarmId = l_tAlarm.m_strID;
    
    if (0 != l_oGetFeedBackRequest.m_oBody.m_strIgnorePrivacy.compare("true"))
    {
        if (0 == l_tAlarm.m_strPrivacy.compare("1"))
        {
            l_tAlarm.m_strCallerNo = "******";
            l_tAlarm.m_strCallerName = "******";
            l_tAlarm.m_strCallerAddr = "******";
            l_tAlarm.m_strCallerID = "******";
            l_tAlarm.m_strCallerIDType = "******";
            l_tAlarm.m_strCallerGender = "******";
            //l_tAlarm.m_strCallerAge = "******";
            //l_tAlarm.m_strCallerBirthday = "******";
            l_tAlarm.m_strContactNo = "******";
            // l_tAlarm.m_strContactName = "******";
            // l_tAlarm.m_strContactAddr = "******";
            // l_tAlarm.m_strContactID = "******";
            // l_tAlarm.m_strContactIDType = "******";
            // l_tAlarm.m_strContactGender = "******";
             //l_tAlarm.m_strContactAge = "******";
             //l_tAlarm.m_strContactBirthday = "******";
        }
    }


    std::vector<ProcessData> l_vecProcess;
    if (!l_strAlarmId.empty())
    {
        GetAlarmProcessByAlarmID(l_strAlarmId, l_vecProcess);
        ICC_LOG_DEBUG(m_pLog, "get process by alarm[%s] size[%d]", l_strAlarmId.c_str(), l_vecProcess.size());

        if (l_vecFeedback.size() == 0)
        {
            for (size_t i = 0; i < l_vecProcess.size(); i++)
            {
                //已退单的,已取消的 反馈数据则不返回 [4/16/2019 w16314]
                if (PROCESS_STATUS_RETURN == l_vecProcess[i].m_strState || PROCESS_STATUS_CANCEL == l_vecProcess[i].m_strState)
                {
                    ICC_LOG_DEBUG(m_pLog, "process is cancelled, no need to get[%s]", l_vecProcess[i].m_strID.c_str());
                    continue;
                }

                PROTOCOL::CGetFeedBackRespond::CFeedbackData l_oFeedback;
                l_oFeedback.m_strAlarmID = l_tAlarm.m_strID;
                //l_oFeedback.m_strActualOccurTime = l_tAlarm.m_strActualOccurTime;
                l_oFeedback.m_strActualOccurAddr = l_tAlarm.m_strAddr;
                l_oFeedback.m_strAlarmCalledNoType = l_tAlarm.m_strCalledNoType;
                l_oFeedback.m_strAlarmFirstType = l_tAlarm.m_strFirstType;
                l_oFeedback.m_strAlarmSecondType = l_tAlarm.m_strSecondType;
                l_oFeedback.m_strAlarmThirdType = l_tAlarm.m_strThirdType;
                l_oFeedback.m_strAlarmFourthType = l_tAlarm.m_strFourthType;
                l_oFeedback.m_strAlarmAddrDeptName = l_tAlarm.m_strAdminDeptName;
                l_oFeedback.m_strAlarmLatitude = l_tAlarm.m_strLatitude;
                l_oFeedback.m_strAlarmLongitude = l_tAlarm.m_strLongitude;
                l_oFeedback.m_strAlarmLongitude = l_tAlarm.m_strLongitude;
                //l_oFeedback.m_strFireBuildingFirstType = l_tAlarm.m_strFireBuildingType;
                l_oFeedback.m_strTrafficVehicleNo = l_tAlarm.m_strVehicleNo;
                l_oFeedback.m_strTrafficVehicleType = l_tAlarm.m_strVehicleType;
                //l_oFeedback.m_strEventType = l_tAlarm.m_strEventType;

                l_oFeedback.m_strProcessID = l_vecProcess[i].m_strID;
                l_oFeedback.m_strProcessDeptCode = l_vecProcess[i].m_strProcessDeptCode;
                l_oFeedback.m_strProcessDeptName = l_vecProcess[i].m_strProcessDeptName;
                l_oFeedback.m_strProcessCode = l_vecProcess[i].m_strProcessCode;
                l_oFeedback.m_strProcessName = l_vecProcess[i].m_strProcessName;
                //l_oFeedback.m_strProcessLeaderCode = l_vecProcess[i].m_strProcessLeaderCode;
                //l_oFeedback.m_strProcessLeaderName = l_vecProcess[i].m_strProcessLeaderName;
                //l_oFeedback.m_strProcessDeptDistrictCode = l_vecProcess[i].m_strProcessDeptDistrictCode;
                l_oFeedback.m_strDispatchDeptCode = l_vecProcess[i].m_strDispatchDeptCode;
                l_oFeedback.m_strDispatchDeptName = l_vecProcess[i].m_strDispatchDeptName;
                l_oFeedback.m_strDispatchCode = l_vecProcess[i].m_strDispatchCode;
                l_oFeedback.m_strDispatchName = l_vecProcess[i].m_strDispatchName;
                //l_oFeedback.m_strDispatchLeaderCode = l_vecProcess[i].m_strDispatchLeaderCode;
                //l_oFeedback.m_strDispatchLeaderName = l_vecProcess[i].m_strDispatchLeaderName;
                l_oFeedback.m_strDispatchDeptDistrictCode = l_vecProcess[i].m_strDispatchDeptDistrictCode;
                //反馈单位行政区划默认为处警单位行政区划
                //l_oFeedback.m_strFeedbackDeptDistrictCode = l_vecProcess[i].m_strProcessDeptDistrictCode;

                l_vecFeedback.push_back(l_oFeedback);
            }
        }
        else
        {
            if (l_vecFeedback.size() > l_vecProcess.size())
            {
                ICC_LOG_ERROR(m_pLog, "db feedback is wrong !!! one process size < feedback size");

                l_oGetFeedBackRespond.m_oHeader.m_strResult = "0";
                l_oGetFeedBackRespond.m_oHeader.m_strMsg = "process size < feedback size";
                JsonParser::IJsonPtr l_pIJsonRespond = ICCGetIJsonFactory()->CreateJson();
                std::string l_strMessage = l_oGetFeedBackRespond.ToString(l_pIJsonRespond);

                p_pNotify->Response(l_strMessage);
                return;
            }

            for (size_t i = 0; i < l_vecProcess.size(); i++)
            {
                // 已退单的反馈数据则不返回
                if (PROCESS_STATUS_RETURN == l_vecProcess[i].m_strState)
                {
                    ICC_LOG_DEBUG(m_pLog, "process is cancelled, no need to get[%s]", l_vecProcess[i].m_strID.c_str());
                    continue;
                }

                //这里把处警单对应的反馈信息找到，正常情况应该要能找到，如果找不到下面会补一个
                PROTOCOL::CGetFeedBackRespond::CFeedbackData l_oFeedback;
                for (size_t j = 0; j < l_vecFeedback.size(); j++)
                {
                    if (l_vecProcess.at(i).m_strID == l_vecFeedback.at(j).m_strProcessID)
                    {
                        l_oFeedback = l_vecFeedback.at(j);
                        if (l_vecProcess[i].m_strState == PROCESS_STATUS_CANCEL)
                        {
                            l_vecFeedback.erase(l_vecFeedback.begin() + j);
                        }
                        break;
                    }
                }

                if (l_oFeedback.m_strProcessID.empty())
                {
                    // 处警单为空这里逻辑有点疑问，什么情况会为空，但是如果存在的话，还是把接警，处警信息补全 [4/17/2019 w16314]
                    ICC_LOG_DEBUG(m_pLog, "process info is empty, complete all info[%s]", l_tAlarm.m_strID.c_str());

                    l_oFeedback.m_strAlarmID = l_tAlarm.m_strID;
                    // l_oFeedback.m_strActualOccurTime = l_tAlarm.m_strActualOccurTime;
                    l_oFeedback.m_strActualOccurAddr = l_tAlarm.m_strAddr;
                    l_oFeedback.m_strAlarmCalledNoType = l_tAlarm.m_strCalledNoType;
                    l_oFeedback.m_strAlarmFirstType = l_tAlarm.m_strFirstType;
                    l_oFeedback.m_strAlarmSecondType = l_tAlarm.m_strSecondType;
                    l_oFeedback.m_strAlarmThirdType = l_tAlarm.m_strThirdType;
                    l_oFeedback.m_strAlarmFourthType = l_tAlarm.m_strFourthType;
                    l_oFeedback.m_strAlarmAddrDeptName = l_tAlarm.m_strAdminDeptName;
                    l_oFeedback.m_strAlarmLatitude = l_tAlarm.m_strLatitude;
                    l_oFeedback.m_strAlarmLongitude = l_tAlarm.m_strLongitude;
                    l_oFeedback.m_strAlarmLongitude = l_tAlarm.m_strLongitude;
                    // l_oFeedback.m_strFireBuildingFirstType = l_tAlarm.m_strFireBuildingType;
                    l_oFeedback.m_strTrafficVehicleNo = l_tAlarm.m_strVehicleNo;
                    l_oFeedback.m_strTrafficVehicleType = l_tAlarm.m_strVehicleType;
                    // l_oFeedback.m_strEventType = l_tAlarm.m_strEventType;

                    l_oFeedback.m_strProcessID = l_vecProcess[i].m_strID;
                    l_oFeedback.m_strProcessDeptCode = l_vecProcess[i].m_strProcessDeptCode;
                    l_oFeedback.m_strProcessDeptName = l_vecProcess[i].m_strProcessDeptName;
                    l_oFeedback.m_strProcessCode = l_vecProcess[i].m_strProcessCode;
                    l_oFeedback.m_strProcessName = l_vecProcess[i].m_strProcessName;
                    //l_oFeedback.m_strProcessLeaderCode = l_vecProcess[i].m_strProcessLeaderCode;
                    //l_oFeedback.m_strProcessLeaderName = l_vecProcess[i].m_strProcessLeaderName;
                    //l_oFeedback.m_strProcessDeptDistrictCode = l_vecProcess[i].m_strProcessDeptDistrictCode;
                    l_oFeedback.m_strDispatchDeptCode = l_vecProcess[i].m_strDispatchDeptCode;
                    l_oFeedback.m_strDispatchDeptName = l_vecProcess[i].m_strDispatchDeptName;
                    l_oFeedback.m_strDispatchCode = l_vecProcess[i].m_strDispatchCode;
                    l_oFeedback.m_strDispatchName = l_vecProcess[i].m_strDispatchName;
                    //l_oFeedback.m_strDispatchLeaderCode = l_vecProcess[i].m_strDispatchLeaderCode;
                    //l_oFeedback.m_strDispatchLeaderName = l_vecProcess[i].m_strDispatchLeaderName;
                    l_oFeedback.m_strDispatchDeptDistrictCode = l_vecProcess[i].m_strDispatchDeptDistrictCode;
                    l_vecFeedback.push_back(l_oFeedback);
                }
            }
        }
    }
	
    l_oGetFeedBackRespond.m_oBody.m_oAlarm = l_tAlarm;

	ICC_LOG_DEBUG(m_pLog, "get alarm feedback, [%s][%d]", l_tAlarm.m_strID.c_str(), l_vecFeedback.size());
	
    if (!l_strAlarmId.empty())
    {
        for (size_t i = 0; i < l_vecFeedback.size(); i++)
        {
            //ICC客户端查询
            if (l_oGetFeedBackRequest.m_oBody.m_strProcessID.empty())
            {
                l_oGetFeedBackRespond.m_oBody.m_vecFeedbackData.push_back(l_vecFeedback.at(i));
            }
            else
            {
                //APP查询时，只查询特定处警部门的反馈单
                if (l_oGetFeedBackRequest.m_oBody.m_strProcessID == l_vecFeedback.at(i).m_strProcessID)
                {
                    l_oGetFeedBackRespond.m_oBody.m_vecFeedbackData.push_back(l_vecFeedback.at(i));
                }
            }
        }

        for (size_t i = 0; i < l_vecProcess.size(); i++)
        {
            l_oGetFeedBackRespond.m_oBody.m_vecProcessData.push_back(l_vecProcess.at(i));
        }

        std::vector<LinkedData> l_vecLinked;
        if (!GetLinkedDispatchByAlarmID(l_strAlarmId, l_vecLinked))
        {
            ICC_LOG_DEBUG(m_pLog, "get LinkedData failed by alarm[%s] size[%d]", l_strAlarmId.c_str(), l_vecLinked.size());
        }
        ICC_LOG_DEBUG(m_pLog, "get LinkedData by alarm[%s] size[%d]", l_strAlarmId.c_str(), l_vecLinked.size());
        for (size_t i = 0; i < l_vecLinked.size(); i++)
        {
            l_oGetFeedBackRespond.m_oBody.m_vectLinkeData.push_back(l_vecLinked.at(i));
        }
    }

	ICC_LOG_DEBUG(m_pLog, "get alarm-process-feedback, [%s][%d][%d]", l_tAlarm.m_strID.c_str(), l_oGetFeedBackRespond.m_oBody.m_vecProcessData.size(), l_oGetFeedBackRespond.m_oBody.m_vecFeedbackData.size());

    l_oGetFeedBackRespond.m_oHeader.m_strResult = "0";

    JsonParser::IJsonPtr l_pIJsonRespond = ICCGetIJsonFactory()->CreateJson();
    std::string l_strMessage = l_oGetFeedBackRespond.ToString(l_pIJsonRespond);

	p_pNotify->Response(l_strMessage);
    ICC_LOG_DEBUG(m_pLog, "get OnCNotifiGetFeedbackRequest l_strMessage : [%s]", l_strMessage.c_str());

}

void CBusinessImpl::OnCNotifiGetFeedbackByProcessRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

    JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
    PROTOCOL::CGetFeedBackByProcessRequest l_oCGetFeedBackByProcessRequest;
	if (!l_oCGetFeedBackByProcessRequest.ParseString(p_pNotify->GetMessages(), l_pIJson))
    {
		ICC_LOG_ERROR(m_pLog, "request is not json");
        return;
    }

    std::string l_strProcessId = l_oCGetFeedBackByProcessRequest.m_oBody.m_strProcessID;
    std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();
    std::string l_strGuid = m_pString->CreateGuid();

	ICC_LOG_ERROR(m_pLog, "get feedback by process ,[%s]", l_strProcessId.c_str());


    PROTOCOL::CGetFeedBackByProcessRespond l_oGetFeedBackByProcessRespond;
    l_oGetFeedBackByProcessRespond.m_oHeader.m_strSystemID = "ICC";
    l_oGetFeedBackByProcessRespond.m_oHeader.m_strSubsystemID = "ICC-ApplicationServer";
    l_oGetFeedBackByProcessRespond.m_oHeader.m_strMsgid = l_strGuid;
    l_oGetFeedBackByProcessRespond.m_oHeader.m_strRelatedID = l_oCGetFeedBackByProcessRequest.m_oHeader.m_strMsgid;
    l_oGetFeedBackByProcessRespond.m_oHeader.m_strSendTime = l_strCurrentTime;
    l_oGetFeedBackByProcessRespond.m_oHeader.m_strCmd = "get_feedback_by_process_respond";
    l_oGetFeedBackByProcessRespond.m_oHeader.m_strRequest = l_oCGetFeedBackByProcessRequest.m_oHeader.m_strRequest;
    l_oGetFeedBackByProcessRespond.m_oHeader.m_strRequestType = l_oCGetFeedBackByProcessRequest.m_oHeader.m_strRequestType;
    l_oGetFeedBackByProcessRespond.m_oHeader.m_strResponse = "";
    l_oGetFeedBackByProcessRespond.m_oHeader.m_strResponseType = "";

    std::vector<FeedbackData> l_vecFeedBack;
    QueryFeedbackByProcessID(l_strProcessId,l_vecFeedBack);

    if (l_vecFeedBack.size() == 0)
    {
        ICC_LOG_ERROR(m_pLog, "have no feedback history ,init feedback");
        ProcessData l_tProcess;
        if (!GetAlarmProcess(l_strProcessId, l_tProcess))
        {
            return;
        }
        
        AlarmData l_tAlarm;
        if (!GetAlarm(l_tProcess.m_strAlarmID, l_tAlarm))
        {
            return;
        }

        if (0 == l_tAlarm.m_strPrivacy.compare("1"))
        {
            l_tAlarm.m_strCallerNo = "******";
            l_tAlarm.m_strCallerName = "******";
            l_tAlarm.m_strCallerAddr = "******";
            l_tAlarm.m_strCallerID = "******";
            l_tAlarm.m_strCallerIDType = "******";
            l_tAlarm.m_strCallerGender = "******";
           // l_tAlarm.m_strCallerAge = "******";
            //l_tAlarm.m_strCallerBirthday = "******";
            l_tAlarm.m_strContactNo = "******";
           // l_tAlarm.m_strContactName = "******";
           // l_tAlarm.m_strContactAddr = "******";
           // l_tAlarm.m_strContactID = "******";
            //l_tAlarm.m_strContactIDType = "******";
            //l_tAlarm.m_strContactGender = "******";
           // l_tAlarm.m_strContactAge = "******";
            //l_tAlarm.m_strContactBirthday = "******";
        }

        PROTOCOL::CGetFeedBackByProcessRespond::CBody l_oFeedback;
        l_oFeedback.m_strAlarmID = l_tAlarm.m_strID;
       // l_oFeedback.m_strActualOccurTime = l_tAlarm.m_strActualOccurTime;
        l_oFeedback.m_strActualOccurAddr = l_tAlarm.m_strAddr;
        l_oFeedback.m_strAlarmCalledNoType = l_tAlarm.m_strCalledNoType;
        l_oFeedback.m_strAlarmFirstType = l_tAlarm.m_strFirstType;
        l_oFeedback.m_strAlarmSecondType = l_tAlarm.m_strSecondType;
        l_oFeedback.m_strAlarmThirdType = l_tAlarm.m_strThirdType;
        l_oFeedback.m_strAlarmFourthType = l_tAlarm.m_strFourthType;
        l_oFeedback.m_strAlarmAddrDeptName = l_tAlarm.m_strAdminDeptName;
        l_oFeedback.m_strAlarmLatitude = l_tAlarm.m_strLatitude;
        l_oFeedback.m_strAlarmLongitude = l_tAlarm.m_strLongitude;
        l_oFeedback.m_strAlarmLongitude = l_tAlarm.m_strLongitude;
        //l_oFeedback.m_strFireBuildingFirstType = l_tAlarm.m_strFireBuildingType;
        l_oFeedback.m_strTrafficVehicleNo = l_tAlarm.m_strVehicleNo;
        l_oFeedback.m_strTrafficVehicleType = l_tAlarm.m_strVehicleType;
       // l_oFeedback.m_strEventType = l_tAlarm.m_strEventType;
    

        l_oFeedback.m_strProcessID = l_tProcess.m_strID;
        l_oFeedback.m_strProcessDeptCode = l_tProcess.m_strProcessDeptCode;
        l_oFeedback.m_strProcessDeptName = l_tProcess.m_strProcessDeptName;
        l_oFeedback.m_strProcessCode = l_tProcess.m_strProcessCode;
        l_oFeedback.m_strProcessName = l_tProcess.m_strProcessName;
        //l_oFeedback.m_strProcessLeaderCode = l_tProcess.m_strProcessLeaderCode;  tzx ??
        //l_oFeedback.m_strProcessLeaderName = l_tProcess.m_strProcessLeaderName; tzx ??
        //l_oFeedback.m_strProcessDeptDistrictCode = l_tProcess.m_strProcessDeptDistrictCode; tzx ??
        l_oFeedback.m_strDispatchDeptCode = l_tProcess.m_strDispatchDeptCode;
        l_oFeedback.m_strDispatchDeptName = l_tProcess.m_strDispatchDeptName;
        l_oFeedback.m_strDispatchCode = l_tProcess.m_strDispatchCode;
        l_oFeedback.m_strDispatchName = l_tProcess.m_strDispatchName;
        //l_oFeedback.m_strDispatchLeaderCode = l_tProcess.m_strDispatchLeaderCode; tzx ??
        //l_oFeedback.m_strDispatchLeaderName = l_tProcess.m_strDispatchLeaderName; tzx ??
        l_oFeedback.m_strDispatchDeptDistrictCode = l_tProcess.m_strDispatchDeptDistrictCode;


        l_oGetFeedBackByProcessRespond.m_oBody = l_oFeedback;

        JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
        std::string l_strMessage = l_oGetFeedBackByProcessRespond.ToString(l_pIJson);

		p_pNotify->Response(l_strMessage);
		ICC_LOG_DEBUG(m_pLog, "send message :[%s]", l_strMessage.c_str());
    }
    else
    {
        for (size_t i = 0; i < l_vecFeedBack.size();++i)
        {
            PROTOCOL::CGetFeedBackByProcessRespond::CBody l_oBody;
            l_oBody.m_strID = l_vecFeedBack.at(i).m_strID;
            l_oBody.m_strAlarmID = l_vecFeedBack.at(i).m_strAlarmID;
            l_oBody.m_strProcessID = l_vecFeedBack.at(i).m_strProcessID;
            l_oBody.m_strResultType = l_vecFeedBack.at(i).m_strResultType;
            l_oBody.m_strResultContent = l_vecFeedBack.at(i).m_strResultContent;
            l_oBody.m_strLeaderInstruction = l_vecFeedBack.at(i).m_strLeaderInstruction;
            l_oBody.m_strState = l_vecFeedBack.at(i).m_strState;
            l_oBody.m_strTimeEdit = l_vecFeedBack.at(i).m_strTimeEdit;
            l_oBody.m_strTimeSubmit = l_vecFeedBack.at(i).m_strTimeSubmit;
            l_oBody.m_strTimeArrived = l_vecFeedBack.at(i).m_strTimeArrived;
            l_oBody.m_strTimeSigned = l_vecFeedBack.at(i).m_strTimeSigned;
            l_oBody.m_strTimePoliceDispatch = l_vecFeedBack.at(i).m_strTimePoliceDispatch;
            l_oBody.m_strTimePoliceArrived = l_vecFeedBack.at(i).m_strTimePoliceArrived;
            l_oBody.m_strActualOccurTime = l_vecFeedBack.at(i).m_strActualOccurTime;
            l_oBody.m_strActualOccurAddr = l_vecFeedBack.at(i).m_strActualOccurAddr;
            l_oBody.m_strFeedbackDeptDistrictCode = l_vecFeedBack.at(i).m_strFeedbackDeptDistrictCode;
            l_oBody.m_strFeedbackDeptCode = l_vecFeedBack.at(i).m_strFeedbackDeptCode;
            l_oBody.m_strFeedbackDeptName = l_vecFeedBack.at(i).m_strFeedbackDeptName;
            l_oBody.m_strFeedbackCode = l_vecFeedBack.at(i).m_strFeedbackCode;
            l_oBody.m_strFeedbackName = l_vecFeedBack.at(i).m_strFeedbackName;
            l_oBody.m_strFeedbackLeaderCode = l_vecFeedBack.at(i).m_strFeedbackLeaderCode;
            l_oBody.m_strFeedbackLeaderName = l_vecFeedBack.at(i).m_strFeedbackLeaderName;
            l_oBody.m_strProcessDeptDistrictCode = l_vecFeedBack.at(i).m_strProcessDeptDistrictCode;
            l_oBody.m_strProcessDeptCode = l_vecFeedBack.at(i).m_strProcessDeptCode;
            l_oBody.m_strProcessDeptName = l_vecFeedBack.at(i).m_strProcessDeptName;
            l_oBody.m_strProcessCode = l_vecFeedBack.at(i).m_strProcessCode;
            l_oBody.m_strProcessName = l_vecFeedBack.at(i).m_strProcessName;
            l_oBody.m_strProcessLeaderCode = l_vecFeedBack.at(i).m_strProcessLeaderCode;
            l_oBody.m_strProcessLeaderName = l_vecFeedBack.at(i).m_strProcessLeaderName;
            l_oBody.m_strDispatchDeptDistrictCode = l_vecFeedBack.at(i).m_strDispatchDeptDistrictCode;
            l_oBody.m_strDispatchDeptCode = l_vecFeedBack.at(i).m_strDispatchDeptCode;
            l_oBody.m_strDispatchDeptName = l_vecFeedBack.at(i).m_strDispatchDeptName;
            l_oBody.m_strDispatchCode = l_vecFeedBack.at(i).m_strDispatchCode;
            l_oBody.m_strDispatchName = l_vecFeedBack.at(i).m_strDispatchName;
            l_oBody.m_strDispatchLeaderCode = l_vecFeedBack.at(i).m_strDispatchLeaderCode;
            l_oBody.m_strDispatchLeaderName = l_vecFeedBack.at(i).m_strDispatchLeaderName;
            l_oBody.m_strPersonId = l_vecFeedBack.at(i).m_strPersonId;
            l_oBody.m_strPersonIdType = l_vecFeedBack.at(i).m_strPersonIdType;
            l_oBody.m_strPersonNationality = l_vecFeedBack.at(i).m_strPersonNationality;
            l_oBody.m_strPersonName = l_vecFeedBack.at(i).m_strPersonName;
            l_oBody.m_strPersonSlaveId = l_vecFeedBack.at(i).m_strPersonSlaveId;
            l_oBody.m_strPersonSlaveIdType = l_vecFeedBack.at(i).m_strPersonSlaveIdType;
            l_oBody.m_strPersonSlaveNationality = l_vecFeedBack.at(i).m_strPersonSlaveNationality;
            l_oBody.m_strPersonSlaveName = l_vecFeedBack.at(i).m_strPersonSlaveName;
            l_oBody.m_strAlarmCalledNoType = l_vecFeedBack.at(i).m_strAlarmCalledNoType;
            l_oBody.m_strAlarmFirstType = l_vecFeedBack.at(i).m_strAlarmFirstType;
            l_oBody.m_strAlarmSecondType = l_vecFeedBack.at(i).m_strAlarmSecondType;
            l_oBody.m_strAlarmThirdType = l_vecFeedBack.at(i).m_strAlarmThirdType;
            l_oBody.m_strAlarmFourthType = l_vecFeedBack.at(i).m_strAlarmFourthType;
            l_oBody.m_strAlarmAddrDeptName = l_vecFeedBack.at(i).m_strAlarmAddrDeptName;
            l_oBody.m_strAlarmAddrFirstType = l_vecFeedBack.at(i).m_strAlarmAddrFirstType;
            l_oBody.m_strAlarmAddrSecondType = l_vecFeedBack.at(i).m_strAlarmAddrSecondType;
            l_oBody.m_strAlarmAddrThirdType = l_vecFeedBack.at(i).m_strAlarmAddrThirdType;
            l_oBody.m_strAlarmLongitude = l_vecFeedBack.at(i).m_strAlarmLongitude;
            l_oBody.m_strAlarmLatitude = l_vecFeedBack.at(i).m_strAlarmLatitude;
            l_oBody.m_strAlarmRegionType = l_vecFeedBack.at(i).m_strAlarmRegionType;
            l_oBody.m_strAlarmLocationType = l_vecFeedBack.at(i).m_strAlarmLocationType;
            l_oBody.m_strPeopleNumCapture = l_vecFeedBack.at(i).m_strPeopleNumCapture;
            l_oBody.m_strPeopleNumRescue = l_vecFeedBack.at(i).m_strPeopleNumRescue;
            l_oBody.m_strPeopleNumSlightInjury = l_vecFeedBack.at(i).m_strPeopleNumSlightInjury;
            l_oBody.m_strPeopleNumSeriousInjury = l_vecFeedBack.at(i).m_strPeopleNumSeriousInjury;
            l_oBody.m_strPeopleNumDeath = l_vecFeedBack.at(i).m_strPeopleNumDeath;
            l_oBody.m_strPoliceNumDispatch = l_vecFeedBack.at(i).m_strPoliceNumDispatch;
            l_oBody.m_strPoliceCarNumDispatch = l_vecFeedBack.at(i).m_strPoliceCarNumDispatch;
            l_oBody.m_strEconomyLoss = l_vecFeedBack.at(i).m_strEconomyLoss;
            l_oBody.m_strRetrieveEconomyLoss = l_vecFeedBack.at(i).m_strRetrieveEconomyLoss;
            l_oBody.m_strFirePutOutTime = l_vecFeedBack.at(i).m_strFirePutOutTime;
            l_oBody.m_strFireBuildingFirstType = l_vecFeedBack.at(i).m_strFireBuildingFirstType;
            l_oBody.m_strFireBuildingSecondType = l_vecFeedBack.at(i).m_strFireBuildingSecondType;
            l_oBody.m_strFireBuildingThirdType = l_vecFeedBack.at(i).m_strFireBuildingThirdType;
            l_oBody.m_strFireSourceType = l_vecFeedBack.at(i).m_strFireSourceType;
            l_oBody.m_strFireRegionType = l_vecFeedBack.at(i).m_strFireRegionType;
            l_oBody.m_strFireCauseFirstType = l_vecFeedBack.at(i).m_strFireCauseFirstType;
            l_oBody.m_strFireCauseSecondType = l_vecFeedBack.at(i).m_strFireCauseSecondType;
            l_oBody.m_strFireCauseThirdType = l_vecFeedBack.at(i).m_strFireCauseThirdType;
            l_oBody.m_strFireArea = l_vecFeedBack.at(i).m_strFireArea;
            l_oBody.m_strTrafficRoadLevel = l_vecFeedBack.at(i).m_strTrafficRoadLevel;
            l_oBody.m_strTrafficAccidentLevel = l_vecFeedBack.at(i).m_strTrafficAccidentLevel;
            l_oBody.m_strTrafficVehicleNo = l_vecFeedBack.at(i).m_strTrafficVehicleNo;
            l_oBody.m_strTrafficVehicleType = l_vecFeedBack.at(i).m_strTrafficVehicleType;
            l_oBody.m_strTrafficSlaveVehicleNo = l_vecFeedBack.at(i).m_strTrafficSlaveVehicleNo;
            l_oBody.m_strTrafficSlaveVehicleType = l_vecFeedBack.at(i).m_strTrafficSlaveVehicleType;
            l_oBody.m_strEventType = l_vecFeedBack.at(i).m_strEventType;
         
            l_oBody.m_strCreateUser = l_vecFeedBack.at(i).m_strCreateUser;
            l_oBody.m_strCreateTime = l_vecFeedBack.at(i).m_strCreateTime;
            l_oBody.m_strUpdateUser = l_vecFeedBack.at(i).m_strUpdateUser;
            l_oBody.m_strUpdateTime = l_vecFeedBack.at(i).m_strUpdateTime;
            l_oGetFeedBackByProcessRespond.m_oBody = l_oBody;

            JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
            std::string l_strMessage = l_oGetFeedBackByProcessRespond.ToString(l_pIJson);

			p_pNotify->Response(l_strMessage);
			ICC_LOG_DEBUG(m_pLog, "send message :[%s]", l_strMessage.c_str());
        }
    }
}

void CBusinessImpl::OnCNotifiSetFeedbackRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	JsonParser::IJsonFactoryPtr l_pJsonFty = ICCGetIJsonFactory();
	PROTOCOL::CSetFeedBackRequest l_oSetFeedbackRequest;
	if (!l_oSetFeedbackRequest.ParseString(p_pNotify->GetMessages(), l_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "request is not json");
		return;
	}
	
	ICC_LOG_DEBUG(m_pLog, "set feedback, get process [%s]", l_oSetFeedbackRequest.m_oBody.m_strProcessID.c_str());

	std::string l_strCurrentTime(m_pDateTime->CurrentDateTimeStr());
	

	if (l_oSetFeedbackRequest.m_oBody.m_strUpdateTime.empty())
	{
		l_oSetFeedbackRequest.m_oBody.m_strUpdateTime = l_strCurrentTime;
	}

	if (l_oSetFeedbackRequest.m_oBody.m_strUpdateUser.empty())
	{
		l_oSetFeedbackRequest.m_oBody.m_strUpdateUser = l_oSetFeedbackRequest.m_oBody.m_strFeedbackCode;
	}	

	ProcessData l_oProcessData;
	if (GetAlarmProcess(l_oSetFeedbackRequest.m_oBody.m_strProcessID, l_oProcessData))
	{
		// 如果对指挥中心已取消，或者派出所已退单警单进行反馈操作，则返回失败 //  [4/16/2019 w16314]
		if (l_oProcessData.m_strState == PROCESS_STATUS_CANCEL || l_oProcessData.m_strState == PROCESS_STATUS_RETURN)
		{			
			// 发送响应消息
			PROTOCOL::CSetFeedBackRespond l_oSetFeedbackResp;
			std::string l_strGuid = m_pString->CreateGuid();
			l_oSetFeedbackResp.m_oHeader.m_strSystemID = "ICC";
			l_oSetFeedbackResp.m_oHeader.m_strSubsystemID = "ICC-ApplicationServer";
			l_oSetFeedbackResp.m_oHeader.m_strMsgid = l_strGuid;
			l_oSetFeedbackResp.m_oHeader.m_strRelatedID = l_oSetFeedbackRequest.m_oHeader.m_strMsgid;
			l_oSetFeedbackResp.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
			l_oSetFeedbackResp.m_oHeader.m_strCmd = "add_or_update_feedback_respond";
			l_oSetFeedbackResp.m_oHeader.m_strRequest = l_oSetFeedbackRequest.m_oHeader.m_strResponse;
			l_oSetFeedbackResp.m_oHeader.m_strRequestType = l_oSetFeedbackRequest.m_oHeader.m_strResponseType;
			l_oSetFeedbackResp.m_oHeader.m_strResponse = "";
			l_oSetFeedbackResp.m_oHeader.m_strResponseType = "";

			l_oSetFeedbackResp.m_oBody.m_strResult = "1";//失败

			std::string l_strMessage(l_oSetFeedbackResp.ToString(l_pJsonFty->CreateJson()));
			p_pNotify->Response(l_strMessage);

			ICC_LOG_DEBUG(m_pLog, "alarm's process is cancelled or returned, [%s][%s]", l_oSetFeedbackRequest.m_oBody.m_strAlarmID.c_str(), l_oSetFeedbackRequest.m_oBody.m_strProcessID.c_str());
			return;
		}
	}

	std::string l_strTime = m_pDateTime->CurrentDateTimeStr();
	l_oSetFeedbackRequest.m_oBody.m_strTimeEdit = l_strTime;
	l_oSetFeedbackRequest.m_oBody.m_strTimeSubmit = l_strTime;
	l_oSetFeedbackRequest.m_oBody.m_strTimeArrived = l_strTime;
	l_oSetFeedbackRequest.m_oBody.m_strTimeSigned = l_strTime;

	//记录总数
	DataBase::SQLRequest l_tSQLReqCnt;
	l_tSQLReqCnt.sql_id = SELECT_FEEDBACK_COUNT;
	l_tSQLReqCnt.param["alarm_id"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmID;
	l_tSQLReqCnt.param["process_id"] = l_oSetFeedbackRequest.m_oBody.m_strProcessID;
	//执行查询记录总数sql
	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_tSQLReqCnt, true);
	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "get feedback count failed, [%s]", l_result->GetErrorMsg().c_str());
		return;
	}
	std::string strNum = l_result->GetValue(0, "num");
	ICC_LOG_DEBUG(m_pLog, "get alarm feedback count: [%s][%s]", l_oSetFeedbackRequest.m_oBody.m_strAlarmID.c_str(), strNum.c_str());

	FeedbackData l_oFeedBack;
	std::string l_strFeedbackID = l_oSetFeedbackRequest.m_oBody.m_strID;
	if (!l_strFeedbackID.empty() && strNum != "0")
	{
		//查询备份信息，后会写流水表和book表
		QueryFeedbackByFeedBackID(l_strFeedbackID, l_oFeedBack);
	}

	std::string l_strSyncType;
	DataBase::SQLRequest l_oSetFeedbackSQLReq;
	

	FeedbackData l_oFeedBackBook;
	if (!l_strFeedbackID.empty() && strNum != "0")
	{		
		if (CheckIsEdit(l_oSetFeedbackRequest.m_oBody, l_oFeedBack))
		{
			ICC_LOG_DEBUG(m_pLog, "feedback is not changed, [%s]", l_strFeedbackID.c_str());
			// 发送响应消息
			PROTOCOL::CSetFeedBackRespond l_oSetFeedbackResp;
			std::string l_strGuid = m_pString->CreateGuid();
			l_oSetFeedbackResp.m_oHeader.m_strSystemID = "ICC";
			l_oSetFeedbackResp.m_oHeader.m_strSubsystemID = "ICC-ApplicationServer";
			l_oSetFeedbackResp.m_oHeader.m_strMsgid = l_strGuid;
			l_oSetFeedbackResp.m_oHeader.m_strRelatedID = l_oSetFeedbackRequest.m_oHeader.m_strMsgid;
			l_oSetFeedbackResp.m_oHeader.m_strSendTime = l_strCurrentTime;
			l_oSetFeedbackResp.m_oHeader.m_strCmd = "add_or_update_feedback_respond";
			l_oSetFeedbackResp.m_oHeader.m_strRequest = l_oSetFeedbackRequest.m_oHeader.m_strResponse;
			l_oSetFeedbackResp.m_oHeader.m_strRequestType = l_oSetFeedbackRequest.m_oHeader.m_strResponseType;
			l_oSetFeedbackResp.m_oHeader.m_strResponse = "";
			l_oSetFeedbackResp.m_oHeader.m_strResponseType = "";

			l_oSetFeedbackResp.m_oBody.m_strResult = "0";//成功

			std::string l_strMessage(l_oSetFeedbackResp.ToString(l_pJsonFty->CreateJson()));
			p_pNotify->Response(l_strMessage);
			return;
		}

		l_oFeedBackBook = l_oFeedBack;

		l_strSyncType = "2";
		l_oSetFeedbackSQLReq.sql_id = "update_icc_t_alarm_feedback";
		l_oSetFeedbackSQLReq.set["id"] = l_oSetFeedbackRequest.m_oBody.m_strID;
		l_oSetFeedbackSQLReq.set["alarm_id"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmID;
		l_oSetFeedbackSQLReq.set["process_id"] = l_oSetFeedbackRequest.m_oBody.m_strProcessID;
		l_oSetFeedbackSQLReq.set["result_type"] = l_oSetFeedbackRequest.m_oBody.m_strResultType;
		l_oSetFeedbackSQLReq.set["result_content"] = l_oSetFeedbackRequest.m_oBody.m_strResultContent;
		l_oSetFeedbackSQLReq.set["leader_instruction"] = l_oSetFeedbackRequest.m_oBody.m_strLeaderInstruction;
		l_oSetFeedbackSQLReq.set["state"] = DIC021020;
		l_oSetFeedbackSQLReq.set["time_edit"] = l_oSetFeedbackRequest.m_oBody.m_strTimeEdit;
		l_oSetFeedbackSQLReq.set["time_submit"] = l_oSetFeedbackRequest.m_oBody.m_strTimeSubmit;
		l_oSetFeedbackSQLReq.set["time_arrived"] = l_oSetFeedbackRequest.m_oBody.m_strTimeArrived;
		l_oSetFeedbackSQLReq.set["time_signed"] = l_oSetFeedbackRequest.m_oBody.m_strTimeSigned;
		l_oSetFeedbackSQLReq.set["time_police_dispatch"] = l_oSetFeedbackRequest.m_oBody.m_strTimePoliceDispatch;
		l_oSetFeedbackSQLReq.set["time_police_arrived"] = l_oSetFeedbackRequest.m_oBody.m_strTimePoliceArrived;
		l_oSetFeedbackSQLReq.set["actual_occur_time"] = l_oSetFeedbackRequest.m_oBody.m_strActualOccurTime;
		l_oSetFeedbackSQLReq.set["actual_occur_addr"] = l_oSetFeedbackRequest.m_oBody.m_strActualOccurAddr;
		l_oSetFeedbackSQLReq.set["feedback_dept_district_code"] = l_oSetFeedbackRequest.m_oBody.m_strFeedbackDeptDistrictCode;
		l_oSetFeedbackSQLReq.set["feedback_dept_code"] = l_oSetFeedbackRequest.m_oBody.m_strFeedbackDeptCode;
		l_oSetFeedbackSQLReq.set["feedback_dept_name"] = l_oSetFeedbackRequest.m_oBody.m_strFeedbackDeptName;
		l_oSetFeedbackSQLReq.set["feedback_code"] = l_oSetFeedbackRequest.m_oBody.m_strFeedbackCode;
		l_oSetFeedbackSQLReq.set["feedback_name"] = l_oSetFeedbackRequest.m_oBody.m_strFeedbackName;
		l_oSetFeedbackSQLReq.set["feedback_leader_code"] = l_oSetFeedbackRequest.m_oBody.m_strFeedbackLeaderCode;
		l_oSetFeedbackSQLReq.set["feedback_leader_name"] = l_oSetFeedbackRequest.m_oBody.m_strFeedbackLeaderName;
		l_oSetFeedbackSQLReq.set["process_dept_district_code"] = l_oSetFeedbackRequest.m_oBody.m_strProcessDeptDistrictCode;
		l_oSetFeedbackSQLReq.set["process_dept_code"] = l_oSetFeedbackRequest.m_oBody.m_strProcessDeptCode;
		l_oSetFeedbackSQLReq.set["process_dept_name"] = l_oSetFeedbackRequest.m_oBody.m_strProcessDeptName;
		l_oSetFeedbackSQLReq.set["process_code"] = l_oSetFeedbackRequest.m_oBody.m_strProcessCode;
		l_oSetFeedbackSQLReq.set["process_name"] = l_oSetFeedbackRequest.m_oBody.m_strProcessName;
		l_oSetFeedbackSQLReq.set["process_leader_code"] = l_oSetFeedbackRequest.m_oBody.m_strProcessLeaderCode;
		l_oSetFeedbackSQLReq.set["process_leader_name"] = l_oSetFeedbackRequest.m_oBody.m_strProcessLeaderName;
		l_oSetFeedbackSQLReq.set["dispatch_dept_district_code"] = l_oSetFeedbackRequest.m_oBody.m_strDispatchDeptDistrictCode;
		l_oSetFeedbackSQLReq.set["dispatch_dept_code"] = l_oSetFeedbackRequest.m_oBody.m_strDispatchDeptCode;
		l_oSetFeedbackSQLReq.set["dispatch_dept_name"] = l_oSetFeedbackRequest.m_oBody.m_strDispatchDeptName;
		l_oSetFeedbackSQLReq.set["dispatch_code"] = l_oSetFeedbackRequest.m_oBody.m_strDispatchCode;
		l_oSetFeedbackSQLReq.set["dispatch_name"] = l_oSetFeedbackRequest.m_oBody.m_strDispatchName;
		l_oSetFeedbackSQLReq.set["dispatch_leader_code"] = l_oSetFeedbackRequest.m_oBody.m_strDispatchLeaderCode;
		l_oSetFeedbackSQLReq.set["dispatch_leader_name"] = l_oSetFeedbackRequest.m_oBody.m_strDispatchLeaderName;
		l_oSetFeedbackSQLReq.set["person_id"] = l_oSetFeedbackRequest.m_oBody.m_strPersonId;
		l_oSetFeedbackSQLReq.set["person_id_type"] = l_oSetFeedbackRequest.m_oBody.m_strPersonIdType;
		l_oSetFeedbackSQLReq.set["person_nationality"] = l_oSetFeedbackRequest.m_oBody.m_strPersonNationality;
		l_oSetFeedbackSQLReq.set["person_name"] = l_oSetFeedbackRequest.m_oBody.m_strPersonName;
		l_oSetFeedbackSQLReq.set["person_slave_id"] = l_oSetFeedbackRequest.m_oBody.m_strPersonSlaveId;
		l_oSetFeedbackSQLReq.set["person_slave_id_type"] = l_oSetFeedbackRequest.m_oBody.m_strPersonSlaveIdType;
		l_oSetFeedbackSQLReq.set["person_slave_nationality"] = l_oSetFeedbackRequest.m_oBody.m_strPersonSlaveNationality;
		l_oSetFeedbackSQLReq.set["person_slave_name"] = l_oSetFeedbackRequest.m_oBody.m_strPersonSlaveName;
		l_oSetFeedbackSQLReq.set["alarm_first_type"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmFirstType;
		l_oSetFeedbackSQLReq.set["alarm_second_type"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmSecondType;
		l_oSetFeedbackSQLReq.set["alarm_third_type"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmThirdType;
		l_oSetFeedbackSQLReq.set["alarm_fourth_type"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmFourthType;
		l_oSetFeedbackSQLReq.set["alarm_addr_dept_name"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmAddrDeptName;
		l_oSetFeedbackSQLReq.set["alarm_addr_first_type"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmAddrFirstType;
		l_oSetFeedbackSQLReq.set["alarm_addr_second_type"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmAddrSecondType;
		l_oSetFeedbackSQLReq.set["alarm_addr_third_type"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmAddrThirdType;
		l_oSetFeedbackSQLReq.set["alarm_longitude"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmLongitude;
		l_oSetFeedbackSQLReq.set["alarm_latitude"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmLatitude;
		l_oSetFeedbackSQLReq.set["alarm_region_type"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmRegionType;
		l_oSetFeedbackSQLReq.set["alarm_location_type"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmLocationType;
		l_oSetFeedbackSQLReq.set["people_num_capture"] = l_oSetFeedbackRequest.m_oBody.m_strPeopleNumCapture;
		l_oSetFeedbackSQLReq.set["people_num_rescue"] = l_oSetFeedbackRequest.m_oBody.m_strPeopleNumRescue;
		l_oSetFeedbackSQLReq.set["people_num_slight_injury"] = l_oSetFeedbackRequest.m_oBody.m_strPeopleNumSlightInjury;
		l_oSetFeedbackSQLReq.set["people_num_serious_injury"] = l_oSetFeedbackRequest.m_oBody.m_strPeopleNumSeriousInjury;
		l_oSetFeedbackSQLReq.set["people_num_death"] = l_oSetFeedbackRequest.m_oBody.m_strPeopleNumDeath;
		l_oSetFeedbackSQLReq.set["police_num_dispatch"] = l_oSetFeedbackRequest.m_oBody.m_strPoliceNumDispatch;
		l_oSetFeedbackSQLReq.set["police_car_num_dispatch"] = l_oSetFeedbackRequest.m_oBody.m_strPoliceCarNumDispatch;
		l_oSetFeedbackSQLReq.set["economy_loss"] = l_oSetFeedbackRequest.m_oBody.m_strEconomyLoss;
		l_oSetFeedbackSQLReq.set["retrieve_economy_loss"] = l_oSetFeedbackRequest.m_oBody.m_strRetrieveEconomyLoss;
		l_oSetFeedbackSQLReq.set["fire_put_out_time"] = l_oSetFeedbackRequest.m_oBody.m_strFirePutOutTime;
		l_oSetFeedbackSQLReq.set["fire_building_first_type"] = l_oSetFeedbackRequest.m_oBody.m_strFireBuildingFirstType;
		l_oSetFeedbackSQLReq.set["fire_building_second_type"] = l_oSetFeedbackRequest.m_oBody.m_strFireBuildingSecondType;
		l_oSetFeedbackSQLReq.set["fire_building_third_type"] = l_oSetFeedbackRequest.m_oBody.m_strFireBuildingThirdType;
		l_oSetFeedbackSQLReq.set["fire_source_type"] = l_oSetFeedbackRequest.m_oBody.m_strFireSourceType;
		l_oSetFeedbackSQLReq.set["fire_region_type"] = l_oSetFeedbackRequest.m_oBody.m_strFireRegionType;
		l_oSetFeedbackSQLReq.set["fire_cause_first_type"] = l_oSetFeedbackRequest.m_oBody.m_strFireCauseFirstType;
		l_oSetFeedbackSQLReq.set["fire_cause_second_type"] = l_oSetFeedbackRequest.m_oBody.m_strFireCauseSecondType;
		l_oSetFeedbackSQLReq.set["fire_cause_third_type"] = l_oSetFeedbackRequest.m_oBody.m_strFireCauseThirdType;
		l_oSetFeedbackSQLReq.set["fire_area"] = l_oSetFeedbackRequest.m_oBody.m_strFireArea;
		l_oSetFeedbackSQLReq.set["traffic_road_level"] = l_oSetFeedbackRequest.m_oBody.m_strTrafficRoadLevel;
		l_oSetFeedbackSQLReq.set["traffic_accident_level"] = l_oSetFeedbackRequest.m_oBody.m_strTrafficAccidentLevel;
		l_oSetFeedbackSQLReq.set["traffic_vehicle_no"] = l_oSetFeedbackRequest.m_oBody.m_strTrafficVehicleNo;
		l_oSetFeedbackSQLReq.set["traffic_vehicle_type"] = l_oSetFeedbackRequest.m_oBody.m_strTrafficVehicleType;
		l_oSetFeedbackSQLReq.set["traffic_slave_vehicle_no"] = l_oSetFeedbackRequest.m_oBody.m_strTrafficSlaveVehicleNo;
		l_oSetFeedbackSQLReq.set["traffic_slave_vehicle_type"] = l_oSetFeedbackRequest.m_oBody.m_strTrafficSlaveVehicleType;
		l_oSetFeedbackSQLReq.set["event_type"] = l_oSetFeedbackRequest.m_oBody.m_strEventType;;
		l_oSetFeedbackSQLReq.set["alarm_called_no_type"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmCalledNoType;
		l_oSetFeedbackSQLReq.set["update_user"] = l_oSetFeedbackRequest.m_oBody.m_strUpdateUser;
		l_oSetFeedbackSQLReq.set["update_time"] = l_oSetFeedbackRequest.m_oBody.m_strUpdateTime;
		l_oSetFeedbackSQLReq.param["id"] = l_oSetFeedbackRequest.m_oBody.m_strID;
	}
	else
	{
		if (l_oSetFeedbackRequest.m_oBody.m_strCreateTime.empty())
		{
			l_oSetFeedbackRequest.m_oBody.m_strCreateTime = l_strCurrentTime;
		}

		if (l_oSetFeedbackRequest.m_oBody.m_strCreateUser.empty())
		{
			l_oSetFeedbackRequest.m_oBody.m_strCreateUser = l_oSetFeedbackRequest.m_oBody.m_strFeedbackCode;
		}

		//缓存中不存在所请求的GUID单位信息，则将数据插入数据库
		l_oSetFeedbackSQLReq.sql_id = "insert_icc_t_alarm_feedback";
		if (l_strFeedbackID.empty())
		{
			l_strFeedbackID = m_pDateTime->CreateSerial();
		}

		l_strSyncType = "1";
		l_oSetFeedbackSQLReq.param["id"] = l_strFeedbackID;
		l_oSetFeedbackSQLReq.param["alarm_id"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmID;
		l_oSetFeedbackSQLReq.param["process_id"] = l_oSetFeedbackRequest.m_oBody.m_strProcessID;
		l_oSetFeedbackSQLReq.param["result_type"] = l_oSetFeedbackRequest.m_oBody.m_strResultType;
		l_oSetFeedbackSQLReq.param["result_content"] = l_oSetFeedbackRequest.m_oBody.m_strResultContent;
		l_oSetFeedbackSQLReq.param["leader_instruction"] = l_oSetFeedbackRequest.m_oBody.m_strLeaderInstruction;
		l_oSetFeedbackSQLReq.param["state"] = DIC021020;
		l_oSetFeedbackSQLReq.param["time_edit"] = l_oSetFeedbackRequest.m_oBody.m_strTimeEdit;
		l_oSetFeedbackSQLReq.param["time_submit"] = l_oSetFeedbackRequest.m_oBody.m_strTimeSubmit;
		l_oSetFeedbackSQLReq.param["time_arrived"] = l_oSetFeedbackRequest.m_oBody.m_strTimeArrived;
		l_oSetFeedbackSQLReq.param["time_signed"] = l_oSetFeedbackRequest.m_oBody.m_strTimeSigned;
		l_oSetFeedbackSQLReq.param["time_police_dispatch"] = l_oSetFeedbackRequest.m_oBody.m_strTimePoliceDispatch;
		l_oSetFeedbackSQLReq.param["time_police_arrived"] = l_oSetFeedbackRequest.m_oBody.m_strTimePoliceArrived;
		l_oSetFeedbackSQLReq.param["actual_occur_time"] = l_oSetFeedbackRequest.m_oBody.m_strActualOccurTime;
		l_oSetFeedbackSQLReq.param["actual_occur_addr"] = l_oSetFeedbackRequest.m_oBody.m_strActualOccurAddr;
		l_oSetFeedbackSQLReq.param["feedback_dept_district_code"] = l_oSetFeedbackRequest.m_oBody.m_strFeedbackDeptDistrictCode;
		l_oSetFeedbackSQLReq.param["feedback_dept_code"] = l_oSetFeedbackRequest.m_oBody.m_strFeedbackDeptCode;
		l_oSetFeedbackSQLReq.param["feedback_dept_name"] = l_oSetFeedbackRequest.m_oBody.m_strFeedbackDeptName;
		l_oSetFeedbackSQLReq.param["feedback_code"] = l_oSetFeedbackRequest.m_oBody.m_strFeedbackCode;
		l_oSetFeedbackSQLReq.param["feedback_name"] = l_oSetFeedbackRequest.m_oBody.m_strFeedbackName;
		l_oSetFeedbackSQLReq.param["feedback_leader_code"] = l_oSetFeedbackRequest.m_oBody.m_strFeedbackLeaderCode;
		l_oSetFeedbackSQLReq.param["feedback_leader_name"] = l_oSetFeedbackRequest.m_oBody.m_strFeedbackLeaderName;
		l_oSetFeedbackSQLReq.param["process_dept_district_code"] = l_oSetFeedbackRequest.m_oBody.m_strProcessDeptDistrictCode;
		l_oSetFeedbackSQLReq.param["process_dept_code"] = l_oSetFeedbackRequest.m_oBody.m_strProcessDeptCode;
		l_oSetFeedbackSQLReq.param["process_dept_name"] = l_oSetFeedbackRequest.m_oBody.m_strProcessDeptName;
		l_oSetFeedbackSQLReq.param["process_code"] = l_oSetFeedbackRequest.m_oBody.m_strProcessCode;
		l_oSetFeedbackSQLReq.param["process_name"] = l_oSetFeedbackRequest.m_oBody.m_strProcessName;
		l_oSetFeedbackSQLReq.param["process_leader_code"] = l_oSetFeedbackRequest.m_oBody.m_strProcessLeaderCode;
		l_oSetFeedbackSQLReq.param["process_leader_name"] = l_oSetFeedbackRequest.m_oBody.m_strProcessLeaderName;
		l_oSetFeedbackSQLReq.param["dispatch_dept_district_code"] = l_oSetFeedbackRequest.m_oBody.m_strDispatchDeptDistrictCode;
		l_oSetFeedbackSQLReq.param["dispatch_dept_code"] = l_oSetFeedbackRequest.m_oBody.m_strDispatchDeptCode;
		l_oSetFeedbackSQLReq.param["dispatch_dept_name"] = l_oSetFeedbackRequest.m_oBody.m_strDispatchDeptName;
		l_oSetFeedbackSQLReq.param["dispatch_code"] = l_oSetFeedbackRequest.m_oBody.m_strDispatchCode;
		l_oSetFeedbackSQLReq.param["dispatch_name"] = l_oSetFeedbackRequest.m_oBody.m_strDispatchName;
		l_oSetFeedbackSQLReq.param["dispatch_leader_code"] = l_oSetFeedbackRequest.m_oBody.m_strDispatchLeaderCode;
		l_oSetFeedbackSQLReq.param["dispatch_leader_name"] = l_oSetFeedbackRequest.m_oBody.m_strDispatchLeaderName;
		l_oSetFeedbackSQLReq.param["person_id"] = l_oSetFeedbackRequest.m_oBody.m_strPersonId;
		l_oSetFeedbackSQLReq.param["person_id_type"] = l_oSetFeedbackRequest.m_oBody.m_strPersonIdType;
		l_oSetFeedbackSQLReq.param["person_nationality"] = l_oSetFeedbackRequest.m_oBody.m_strPersonNationality;
		l_oSetFeedbackSQLReq.param["person_name"] = l_oSetFeedbackRequest.m_oBody.m_strPersonName;
		l_oSetFeedbackSQLReq.param["person_slave_id"] = l_oSetFeedbackRequest.m_oBody.m_strPersonSlaveId;
		l_oSetFeedbackSQLReq.param["person_slave_id_type"] = l_oSetFeedbackRequest.m_oBody.m_strPersonSlaveIdType;
		l_oSetFeedbackSQLReq.param["person_slave_nationality"] = l_oSetFeedbackRequest.m_oBody.m_strPersonSlaveNationality;
		l_oSetFeedbackSQLReq.param["person_slave_name"] = l_oSetFeedbackRequest.m_oBody.m_strPersonSlaveName;
		l_oSetFeedbackSQLReq.param["alarm_first_type"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmFirstType;
		l_oSetFeedbackSQLReq.param["alarm_second_type"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmSecondType;
		l_oSetFeedbackSQLReq.param["alarm_third_type"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmThirdType;
		l_oSetFeedbackSQLReq.param["alarm_fourth_type"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmFourthType;
		l_oSetFeedbackSQLReq.param["alarm_addr_dept_name"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmAddrDeptName;
		l_oSetFeedbackSQLReq.param["alarm_addr_first_type"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmAddrFirstType;
		l_oSetFeedbackSQLReq.param["alarm_addr_second_type"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmAddrSecondType;
		l_oSetFeedbackSQLReq.param["alarm_addr_third_type"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmAddrThirdType;
		l_oSetFeedbackSQLReq.param["alarm_longitude"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmLongitude;
		l_oSetFeedbackSQLReq.param["alarm_latitude"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmLatitude;
		l_oSetFeedbackSQLReq.param["alarm_region_type"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmRegionType;
		l_oSetFeedbackSQLReq.param["alarm_location_type"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmLocationType;
		l_oSetFeedbackSQLReq.param["people_num_capture"] = l_oSetFeedbackRequest.m_oBody.m_strPeopleNumCapture;
		l_oSetFeedbackSQLReq.param["people_num_rescue"] = l_oSetFeedbackRequest.m_oBody.m_strPeopleNumRescue;
		l_oSetFeedbackSQLReq.param["people_num_slight_injury"] = l_oSetFeedbackRequest.m_oBody.m_strPeopleNumSlightInjury;
		l_oSetFeedbackSQLReq.param["people_num_serious_injury"] = l_oSetFeedbackRequest.m_oBody.m_strPeopleNumSeriousInjury;
		l_oSetFeedbackSQLReq.param["people_num_death"] = l_oSetFeedbackRequest.m_oBody.m_strPeopleNumDeath;
		l_oSetFeedbackSQLReq.param["police_num_dispatch"] = l_oSetFeedbackRequest.m_oBody.m_strPoliceNumDispatch;
		l_oSetFeedbackSQLReq.param["police_car_num_dispatch"] = l_oSetFeedbackRequest.m_oBody.m_strPoliceCarNumDispatch;
		l_oSetFeedbackSQLReq.param["economy_loss"] = l_oSetFeedbackRequest.m_oBody.m_strEconomyLoss;
		l_oSetFeedbackSQLReq.param["retrieve_economy_loss"] = l_oSetFeedbackRequest.m_oBody.m_strRetrieveEconomyLoss;
		l_oSetFeedbackSQLReq.param["fire_put_out_time"] = l_oSetFeedbackRequest.m_oBody.m_strFirePutOutTime;
		l_oSetFeedbackSQLReq.param["fire_building_first_type"] = l_oSetFeedbackRequest.m_oBody.m_strFireBuildingFirstType;
		l_oSetFeedbackSQLReq.param["fire_building_second_type"] = l_oSetFeedbackRequest.m_oBody.m_strFireBuildingSecondType;
		l_oSetFeedbackSQLReq.param["fire_building_third_type"] = l_oSetFeedbackRequest.m_oBody.m_strFireBuildingThirdType;
		l_oSetFeedbackSQLReq.param["fire_source_type"] = l_oSetFeedbackRequest.m_oBody.m_strFireSourceType;
		l_oSetFeedbackSQLReq.param["fire_region_type"] = l_oSetFeedbackRequest.m_oBody.m_strFireRegionType;
		l_oSetFeedbackSQLReq.param["fire_cause_first_type"] = l_oSetFeedbackRequest.m_oBody.m_strFireCauseFirstType;
		l_oSetFeedbackSQLReq.param["fire_cause_second_type"] = l_oSetFeedbackRequest.m_oBody.m_strFireCauseSecondType;
		l_oSetFeedbackSQLReq.param["fire_cause_third_type"] = l_oSetFeedbackRequest.m_oBody.m_strFireCauseThirdType;
		l_oSetFeedbackSQLReq.param["fire_area"] = l_oSetFeedbackRequest.m_oBody.m_strFireArea;
		l_oSetFeedbackSQLReq.param["traffic_road_level"] = l_oSetFeedbackRequest.m_oBody.m_strTrafficRoadLevel;
		l_oSetFeedbackSQLReq.param["traffic_accident_level"] = l_oSetFeedbackRequest.m_oBody.m_strTrafficAccidentLevel;
		l_oSetFeedbackSQLReq.param["traffic_vehicle_no"] = l_oSetFeedbackRequest.m_oBody.m_strTrafficVehicleNo;
		l_oSetFeedbackSQLReq.param["traffic_vehicle_type"] = l_oSetFeedbackRequest.m_oBody.m_strTrafficVehicleType;
		l_oSetFeedbackSQLReq.param["traffic_slave_vehicle_no"] = l_oSetFeedbackRequest.m_oBody.m_strTrafficSlaveVehicleNo;
		l_oSetFeedbackSQLReq.param["traffic_slave_vehicle_type"] = l_oSetFeedbackRequest.m_oBody.m_strTrafficSlaveVehicleType;
		l_oSetFeedbackSQLReq.param["event_type"] = l_oSetFeedbackRequest.m_oBody.m_strEventType;
		l_oSetFeedbackSQLReq.param["alarm_called_no_type"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmCalledNoType;
		l_oSetFeedbackSQLReq.param["create_user"] = l_oSetFeedbackRequest.m_oBody.m_strCreateUser;
		l_oSetFeedbackSQLReq.param["create_time"] = l_oSetFeedbackRequest.m_oBody.m_strCreateTime;
		l_oSetFeedbackSQLReq.param["update_user"] = l_oSetFeedbackRequest.m_oBody.m_strUpdateUser;
		l_oSetFeedbackSQLReq.param["update_time"] = l_oSetFeedbackRequest.m_oBody.m_strUpdateTime;

	}
	//执行数据库操作                

	DataBase::IResultSetPtr l_pRSet;
	if (!m_pDBConn)
	{
		ICC_LOG_FATAL(m_pLog, "db connect is null!!!");
		return;
	}

	l_pRSet = m_pDBConn->Exec(l_oSetFeedbackSQLReq);

	/*根据数据库操作结果构造响应消息*/
	PROTOCOL::CSetFeedBackRespond l_oSetFeedbackResp;
	std::string l_strGuid = m_pString->CreateGuid();
	l_oSetFeedbackResp.m_oHeader.m_strSystemID = "ICC";
	l_oSetFeedbackResp.m_oHeader.m_strSubsystemID = "ICC-ApplicationServer";
	l_oSetFeedbackResp.m_oHeader.m_strMsgid = l_strGuid;
	l_oSetFeedbackResp.m_oHeader.m_strRelatedID = l_oSetFeedbackRequest.m_oHeader.m_strMsgid;
	l_oSetFeedbackResp.m_oHeader.m_strSendTime = l_strCurrentTime;
	l_oSetFeedbackResp.m_oHeader.m_strCmd = "add_or_update_feedback_respond";
	l_oSetFeedbackResp.m_oHeader.m_strRequest = l_oSetFeedbackRequest.m_oHeader.m_strResponse;
	l_oSetFeedbackResp.m_oHeader.m_strRequestType = l_oSetFeedbackRequest.m_oHeader.m_strResponseType;
	l_oSetFeedbackResp.m_oHeader.m_strResponse = "";
	l_oSetFeedbackResp.m_oHeader.m_strResponseType = "";

	if (!l_pRSet->IsValid())
	{
		l_oSetFeedbackResp.m_oBody.m_strResult = "1";//失败
		ICC_LOG_ERROR(m_pLog, "set feedback failed:[%s]", l_pRSet->GetErrorMsg().c_str());
	}
	else
	{
		l_oSetFeedbackResp.m_oBody.m_strResult = "0";//成功
		ICC_LOG_DEBUG(m_pLog, "set feedback to db success, [%s]", l_strFeedbackID.c_str());
		{ //发送同步
			FeedbackData l_oData;
			{
				l_oData.m_strMsgSource = l_oSetFeedbackRequest.m_oBody.m_strMsgSource;
				l_oData.m_strID = l_strFeedbackID;
				l_oData.m_strAlarmID = l_oSetFeedbackRequest.m_oBody.m_strAlarmID;
				l_oData.m_strProcessID = l_oSetFeedbackRequest.m_oBody.m_strProcessID;
				l_oData.m_strResultType = l_oSetFeedbackRequest.m_oBody.m_strResultType;
				l_oData.m_strResultContent = l_oSetFeedbackRequest.m_oBody.m_strResultContent;
				l_oData.m_strLeaderInstruction = l_oSetFeedbackRequest.m_oBody.m_strLeaderInstruction;
				l_oData.m_strState = DIC021020;
				l_oData.m_strTimeEdit = l_oSetFeedbackRequest.m_oBody.m_strTimeEdit;
				l_oData.m_strTimeSubmit = l_oSetFeedbackRequest.m_oBody.m_strTimeSubmit;
				l_oData.m_strTimeArrived = l_oSetFeedbackRequest.m_oBody.m_strTimeArrived;
				l_oData.m_strTimeSigned = l_oSetFeedbackRequest.m_oBody.m_strTimeSigned;
				l_oData.m_strTimePoliceDispatch = l_oSetFeedbackRequest.m_oBody.m_strTimePoliceDispatch;
				l_oData.m_strTimePoliceArrived = l_oSetFeedbackRequest.m_oBody.m_strTimePoliceArrived;
				l_oData.m_strActualOccurTime = l_oSetFeedbackRequest.m_oBody.m_strActualOccurTime;
				l_oData.m_strActualOccurAddr = l_oSetFeedbackRequest.m_oBody.m_strActualOccurAddr;
				l_oData.m_strFeedbackDeptDistrictCode = l_oSetFeedbackRequest.m_oBody.m_strFeedbackDeptDistrictCode;
				l_oData.m_strFeedbackDeptCode = l_oSetFeedbackRequest.m_oBody.m_strFeedbackDeptCode;
				l_oData.m_strFeedbackDeptName = l_oSetFeedbackRequest.m_oBody.m_strFeedbackDeptName;
				l_oData.m_strFeedbackCode = l_oSetFeedbackRequest.m_oBody.m_strFeedbackCode;
				l_oData.m_strFeedbackName = l_oSetFeedbackRequest.m_oBody.m_strFeedbackName;
				l_oData.m_strFeedbackLeaderCode = l_oSetFeedbackRequest.m_oBody.m_strFeedbackLeaderCode;
				l_oData.m_strFeedbackLeaderName = l_oSetFeedbackRequest.m_oBody.m_strFeedbackLeaderName;
				l_oData.m_strProcessDeptDistrictCode = l_oSetFeedbackRequest.m_oBody.m_strProcessDeptDistrictCode;
				l_oData.m_strProcessDeptCode = l_oSetFeedbackRequest.m_oBody.m_strProcessDeptCode;
				l_oData.m_strProcessDeptName = l_oSetFeedbackRequest.m_oBody.m_strProcessDeptName;
				l_oData.m_strProcessCode = l_oSetFeedbackRequest.m_oBody.m_strProcessCode;
				l_oData.m_strProcessName = l_oSetFeedbackRequest.m_oBody.m_strProcessName;
				l_oData.m_strProcessLeaderCode = l_oSetFeedbackRequest.m_oBody.m_strProcessLeaderCode;
				l_oData.m_strProcessLeaderName = l_oSetFeedbackRequest.m_oBody.m_strProcessLeaderName;
				l_oData.m_strDispatchDeptDistrictCode = l_oSetFeedbackRequest.m_oBody.m_strDispatchDeptDistrictCode;
				l_oData.m_strDispatchDeptCode = l_oSetFeedbackRequest.m_oBody.m_strDispatchDeptCode;
				l_oData.m_strDispatchDeptName = l_oSetFeedbackRequest.m_oBody.m_strDispatchDeptName;
				l_oData.m_strDispatchCode = l_oSetFeedbackRequest.m_oBody.m_strDispatchCode;
				l_oData.m_strDispatchName = l_oSetFeedbackRequest.m_oBody.m_strDispatchName;
				l_oData.m_strDispatchLeaderCode = l_oSetFeedbackRequest.m_oBody.m_strDispatchLeaderCode;
				l_oData.m_strDispatchLeaderName = l_oSetFeedbackRequest.m_oBody.m_strDispatchLeaderName;
				l_oData.m_strPersonId = l_oSetFeedbackRequest.m_oBody.m_strPersonId;
				l_oData.m_strPersonIdType = l_oSetFeedbackRequest.m_oBody.m_strPersonIdType;
				l_oData.m_strPersonNationality = l_oSetFeedbackRequest.m_oBody.m_strPersonNationality;
				l_oData.m_strPersonName = l_oSetFeedbackRequest.m_oBody.m_strPersonName;
				l_oData.m_strPersonSlaveId = l_oSetFeedbackRequest.m_oBody.m_strPersonSlaveId;
				l_oData.m_strPersonSlaveIdType = l_oSetFeedbackRequest.m_oBody.m_strPersonSlaveIdType;
				l_oData.m_strPersonSlaveNationality = l_oSetFeedbackRequest.m_oBody.m_strPersonSlaveNationality;
				l_oData.m_strPersonSlaveName = l_oSetFeedbackRequest.m_oBody.m_strPersonSlaveName;
				l_oData.m_strAlarmFirstType = l_oSetFeedbackRequest.m_oBody.m_strAlarmFirstType;
				l_oData.m_strAlarmSecondType = l_oSetFeedbackRequest.m_oBody.m_strAlarmSecondType;
				l_oData.m_strAlarmThirdType = l_oSetFeedbackRequest.m_oBody.m_strAlarmThirdType;
				l_oData.m_strAlarmFourthType = l_oSetFeedbackRequest.m_oBody.m_strAlarmFourthType;
				l_oData.m_strAlarmAddrDeptName = l_oSetFeedbackRequest.m_oBody.m_strAlarmAddrDeptName;
				l_oData.m_strAlarmAddrFirstType = l_oSetFeedbackRequest.m_oBody.m_strAlarmAddrFirstType;
				l_oData.m_strAlarmAddrSecondType = l_oSetFeedbackRequest.m_oBody.m_strAlarmAddrSecondType;
				l_oData.m_strAlarmAddrThirdType = l_oSetFeedbackRequest.m_oBody.m_strAlarmAddrThirdType;
				l_oData.m_strAlarmLongitude = l_oSetFeedbackRequest.m_oBody.m_strAlarmLongitude;
				l_oData.m_strAlarmLatitude = l_oSetFeedbackRequest.m_oBody.m_strAlarmLatitude;
				l_oData.m_strAlarmRegionType = l_oSetFeedbackRequest.m_oBody.m_strAlarmRegionType;
				l_oData.m_strAlarmLocationType = l_oSetFeedbackRequest.m_oBody.m_strAlarmLocationType;
				l_oData.m_strPeopleNumCapture = l_oSetFeedbackRequest.m_oBody.m_strPeopleNumCapture;
				l_oData.m_strPeopleNumRescue = l_oSetFeedbackRequest.m_oBody.m_strPeopleNumRescue;
				l_oData.m_strPeopleNumSlightInjury = l_oSetFeedbackRequest.m_oBody.m_strPeopleNumSlightInjury;
				l_oData.m_strPeopleNumSeriousInjury = l_oSetFeedbackRequest.m_oBody.m_strPeopleNumSeriousInjury;
				l_oData.m_strPeopleNumDeath = l_oSetFeedbackRequest.m_oBody.m_strPeopleNumDeath;
				l_oData.m_strPoliceNumDispatch = l_oSetFeedbackRequest.m_oBody.m_strPoliceNumDispatch;
				l_oData.m_strPoliceCarNumDispatch = l_oSetFeedbackRequest.m_oBody.m_strPoliceCarNumDispatch;
				l_oData.m_strEconomyLoss = l_oSetFeedbackRequest.m_oBody.m_strEconomyLoss;
				l_oData.m_strRetrieveEconomyLoss = l_oSetFeedbackRequest.m_oBody.m_strRetrieveEconomyLoss;
				l_oData.m_strFirePutOutTime = l_oSetFeedbackRequest.m_oBody.m_strFirePutOutTime;
				l_oData.m_strFireBuildingFirstType = l_oSetFeedbackRequest.m_oBody.m_strFireBuildingFirstType;
				l_oData.m_strFireBuildingSecondType = l_oSetFeedbackRequest.m_oBody.m_strFireBuildingSecondType;
				l_oData.m_strFireBuildingThirdType = l_oSetFeedbackRequest.m_oBody.m_strFireBuildingThirdType;
				l_oData.m_strFireSourceType = l_oSetFeedbackRequest.m_oBody.m_strFireSourceType;
				l_oData.m_strFireRegionType = l_oSetFeedbackRequest.m_oBody.m_strFireRegionType;
				l_oData.m_strFireCauseFirstType = l_oSetFeedbackRequest.m_oBody.m_strFireCauseFirstType;
				l_oData.m_strFireCauseSecondType = l_oSetFeedbackRequest.m_oBody.m_strFireCauseSecondType;
				l_oData.m_strFireCauseThirdType = l_oSetFeedbackRequest.m_oBody.m_strFireCauseThirdType;
				l_oData.m_strFireArea = l_oSetFeedbackRequest.m_oBody.m_strFireArea;
				l_oData.m_strTrafficRoadLevel = l_oSetFeedbackRequest.m_oBody.m_strTrafficRoadLevel;
				l_oData.m_strTrafficAccidentLevel = l_oSetFeedbackRequest.m_oBody.m_strTrafficAccidentLevel;
				l_oData.m_strTrafficVehicleNo = l_oSetFeedbackRequest.m_oBody.m_strTrafficVehicleNo;
				l_oData.m_strTrafficVehicleType = l_oSetFeedbackRequest.m_oBody.m_strTrafficVehicleType;
				l_oData.m_strTrafficSlaveVehicleNo = l_oSetFeedbackRequest.m_oBody.m_strTrafficSlaveVehicleNo;
				l_oData.m_strTrafficSlaveVehicleType = l_oSetFeedbackRequest.m_oBody.m_strTrafficSlaveVehicleType;
				l_oData.m_strEventType = l_oSetFeedbackRequest.m_oBody.m_strEventType;
				l_oData.m_strAlarmCalledNoType = l_oSetFeedbackRequest.m_oBody.m_strAlarmCalledNoType;
				l_oData.m_strCreateTime = l_oSetFeedbackRequest.m_oBody.m_strCreateTime;
				l_oData.m_strCreateUser =  l_oSetFeedbackRequest.m_oBody.m_strCreateUser;
				l_oData.m_strUpdateTime = l_oSetFeedbackRequest.m_oBody.m_strUpdateTime;
				l_oData.m_strUpdateUser = l_oSetFeedbackRequest.m_oBody.m_strUpdateUser;
				SyncFeedBackInfo(l_oData, l_strSyncType, l_oSetFeedbackRequest.m_oHeader.m_strMsgid);
			}
			std::string l_strFeedBackBookGuid;
			std::string l_strResourceType;
			std::string l_strCode = BS001003005;
			if (l_strSyncType == "2")
			{
				l_strFeedBackBookGuid = m_pDateTime->CreateSerial();
				l_strResourceType = "3";
				AddFeedbackBook(l_oFeedBackBook, l_strFeedBackBookGuid, "befor");
				AddFeedbackBook(l_oData, l_strFeedBackBookGuid, "after");
				l_strCode = BS001003006;
			}

			l_strGuid = m_pString->CreateGuid();

			//TODO::记录警情流水_受理接警单	
			//向数据库添加接警流水并发送通知

			DataBase::SQLRequest l_tSQL;
			l_tSQL.sql_id = "select_icc_t_staff_dept";
			l_tSQL.param["user_code"] = l_oSetFeedbackRequest.m_oBody.m_strUpdateUser;
			DataBase::IResultSetPtr l_pRSetStaff = m_pDBConn->Exec(l_tSQL, true);
			
			if (!l_pRSetStaff->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "get user info by usercode[%s] failed, record logs failed", l_oSetFeedbackRequest.m_oBody.m_strUpdateUser.c_str());
				return;
			}			

			std::string l_strUserName;
			std::string l_strDeptName;
			std::string l_strDeptCode;
			if (l_pRSetStaff->Next())
			{
				l_strUserName = l_pRSetStaff->GetValue("name");
				l_strDeptCode = l_pRSetStaff->GetValue("deptcode");
				l_strDeptName = l_pRSetStaff->GetValue("deptname");
			}

			ICC_LOG_DEBUG(m_pLog, "get user info by usercode [%s][%s][%s][%s]", l_oSetFeedbackRequest.m_oBody.m_strUpdateUser.c_str(), 
				l_strUserName.c_str(), l_strDeptCode.c_str(), l_strDeptName.c_str());

			PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
			std::vector<std::string> l_vecParamList;

            Data::CStaffInfo l_oStaffInfo;
            if (!_GetStaffInfo(l_oSetFeedbackRequest.m_oBody.m_strUpdateUser, l_oStaffInfo))
            {
                ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
            }
            std::string l_strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_strUserName);
            l_vecParamList.push_back(l_strStaffName);
			l_vecParamList.push_back(l_oSetFeedbackRequest.m_oBody.m_strUpdateUser);
			l_vecParamList.push_back(l_strDeptName);
			l_oAlarmLogInfo.m_strAlarmID = l_oSetFeedbackRequest.m_oBody.m_strAlarmID;
            l_oAlarmLogInfo.m_strOperateAttachDesc = GenAlarmLogAttach(l_strResourceType, l_strFeedBackBookGuid);
			l_oAlarmLogInfo.m_strOperateContent = GenAlarmLogContent(l_vecParamList);
			l_oAlarmLogInfo.m_strCreateUser = l_oSetFeedbackRequest.m_oBody.m_strFeedbackName;
			l_oAlarmLogInfo.m_strCreateTime = m_pDateTime->CurrentDateTimeStr();
			l_oAlarmLogInfo.m_strOperate = l_strCode;
			l_oAlarmLogInfo.m_strID = m_pString->CreateGuid();
            l_oAlarmLogInfo.m_strSourceName = "icc";

            std::string l_strCurTime(m_pDateTime->ToString(m_pDateTime->CurrentDateTime(), DateTime::DEFAULT_DATETIME_STRING_FORMAT));
            //TODO::select_icc_t_jjdb_jjsj 查询jjsj
            std::string strTime = m_pDateTime->GetAlarmIdTime(l_oAlarmLogInfo.m_strAlarmID);
            
            if (strTime != "")
            {
                DataBase::SQLRequest l_sqlReqeust;
                l_sqlReqeust.sql_id = "select_icc_t_jjdb_jjsj";
                l_sqlReqeust.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
                l_sqlReqeust.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
                l_sqlReqeust.param["jjdbh"] = l_oAlarmLogInfo.m_strAlarmID;
                DataBase::IResultSetPtr l_pRSetPtr = m_pDBConn->Exec(l_sqlReqeust);
                if (!l_pRSetPtr->IsValid())
                {
                    ICC_LOG_ERROR(m_pLog, "select_icc_t_jjdb_jjsj failed, error msg:[%s]", l_pRSetPtr->GetErrorMsg().c_str());
                }
                if (l_pRSetPtr->Next())
                {
                    l_oAlarmLogInfo.m_strReceivedTime = l_pRSetPtr->GetValue("jjsj");
                }
            }

            if (l_oAlarmLogInfo.m_strReceivedTime.empty())
            {
                l_oAlarmLogInfo.m_strReceivedTime = l_strCurTime;
            }

			//TODO::记录警情流水日志
			DataBase::SQLRequest l_tSQLReqInsertAlarm;
            if (PROTOCOL::CAlarmLogSync::SetLogInsertSql(l_oAlarmLogInfo, l_tSQLReqInsertAlarm))
            {
               
                l_tSQLReqInsertAlarm.param["create_time"] = l_strCurTime;// l_oAlarmLogInfo.m_strCreateTime;

                DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertAlarm);
                if (!l_pRSet->IsValid())
                {
                    ICC_LOG_ERROR(m_pLog, "insert alarm log info failed, error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
                }

                SyncAlarmLogInfo(l_oAlarmLogInfo);
            }

			l_tSQLReqInsertAlarm.sql_id = "insert_icc_t_alarm_log";
					
		}

		{
			//修改接警单处警单状态
			DataBase::SQLRequest l_tSQLUpdateAlarmState;
			l_tSQLUpdateAlarmState.sql_id = "update_icc_t_alarm";
			l_tSQLUpdateAlarmState.param["id"] = l_oSetFeedbackRequest.m_oBody.m_strAlarmID;
			l_tSQLUpdateAlarmState.set["state"] = DIC019100;//"DIC019090";收到反馈后，改为已终结
			l_tSQLUpdateAlarmState.set["update_user"] = "FeedBack";
			l_tSQLUpdateAlarmState.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
			if ("1" == l_strSyncType)
			{
				l_tSQLUpdateAlarmState.set["is_feedback"] = "1";
			}

			DataBase::IResultSetPtr l_pRSetAlarm = m_pDBConn->Exec(l_tSQLUpdateAlarmState);
			if (!l_pRSetAlarm->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "update alarm state failed,error msg:[%s]", l_pRSetAlarm->GetErrorMsg().c_str());
				return;
			}

			ICC_LOG_DEBUG(m_pLog, "update alarm state [%s][%s]", l_oSetFeedbackRequest.m_oBody.m_strAlarmID.c_str(), "05");

			AlarmData l_oAlarm;
			if (GetAlarm(l_oSetFeedbackRequest.m_oBody.m_strAlarmID, l_oAlarm))
			{
				SyncAlarmInfo(l_oAlarm, "2", l_oSetFeedbackRequest.m_oHeader.m_strMsgid);
			}

			DataBase::SQLRequest l_tSQLUpdateProcessState;
			l_tSQLUpdateProcessState.sql_id = "update_icc_t_pjdb";
			l_tSQLUpdateProcessState.param["id"] = l_oSetFeedbackRequest.m_oBody.m_strProcessID;
            std::string strTime = m_pDateTime->GetDispatchIdTime(l_oSetFeedbackRequest.m_oBody.m_strProcessID);
            if (strTime != "")
            {
                l_tSQLUpdateProcessState.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 86400 * 15);
                l_tSQLUpdateProcessState.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
            }

			l_tSQLUpdateProcessState.set["state"] = "DIC020060";
			l_tSQLUpdateProcessState.set["update_user"] = "FeedBack";
			l_tSQLUpdateProcessState.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
			DataBase::IResultSetPtr l_pRSetProcess = m_pDBConn->Exec(l_tSQLUpdateProcessState);
			

			if (!l_pRSetProcess->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "update process state failed,error msg:[%s]", l_pRSetProcess->GetErrorMsg().c_str());
				return;
			}

			ICC_LOG_DEBUG(m_pLog, "update process state [%s][%s]", l_oSetFeedbackRequest.m_oBody.m_strProcessID.c_str(), "DIC020060");

			ProcessData l_oProcess;
			if (GetAlarmProcess(l_oSetFeedbackRequest.m_oBody.m_strProcessID, l_oProcess))
			{
				SyncProcessInfo(l_oProcess, "2", l_oSetFeedbackRequest.m_oHeader.m_strMsgid);
			}
		}
	}
	//返回SetFeedbackRequest的结果信息
	std::string l_strMessage(l_oSetFeedbackResp.ToString(l_pJsonFty->CreateJson()));
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::OnCGetAlarmLogFeedbackRequest(ObserverPattern::INotificationPtr p_pNotify)
{	
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

    JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
    PROTOCOL::CGetAlarmLogFeedBackResourceRequest l_oGetAlarmLogFeedBackResourceRequest;
	if (!l_oGetAlarmLogFeedBackResourceRequest.ParseString(p_pNotify->GetMessages(), l_pIJson))
    {
		ICC_LOG_ERROR(m_pLog, "request is not json");
        return;
    }    
    
	ICC_LOG_DEBUG(m_pLog, "get feedback records by resourceid[%s]", l_oGetAlarmLogFeedBackResourceRequest.m_oBody.m_strResourceId.c_str());

	std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();
    std::string l_strGuid = m_pString->CreateGuid();

    PROTOCOL::CGetAlarmLogFeedBackResourceRespond l_oGetAlarmLogFeedBackResourceRespond;
    l_oGetAlarmLogFeedBackResourceRespond.m_oHeader.m_strSystemID = "ICC";
    l_oGetAlarmLogFeedBackResourceRespond.m_oHeader.m_strSubsystemID = "ICC-ApplicationServer";
    l_oGetAlarmLogFeedBackResourceRespond.m_oHeader.m_strMsgid = l_strGuid;
    l_oGetAlarmLogFeedBackResourceRespond.m_oHeader.m_strRelatedID = l_oGetAlarmLogFeedBackResourceRequest.m_oHeader.m_strMsgid;
    l_oGetAlarmLogFeedBackResourceRespond.m_oHeader.m_strSendTime = l_strCurrentTime;
    l_oGetAlarmLogFeedBackResourceRespond.m_oHeader.m_strCmd = "get_alarm_log_feedback_respond";
    l_oGetAlarmLogFeedBackResourceRespond.m_oHeader.m_strRequest = l_oGetAlarmLogFeedBackResourceRequest.m_oHeader.m_strRequest;
    l_oGetAlarmLogFeedBackResourceRespond.m_oHeader.m_strRequestType = l_oGetAlarmLogFeedBackResourceRequest.m_oHeader.m_strRequestType;
    l_oGetAlarmLogFeedBackResourceRespond.m_oHeader.m_strResponse = "";
    l_oGetAlarmLogFeedBackResourceRespond.m_oHeader.m_strResponseType = "";

    GetFeedBackBook(l_oGetAlarmLogFeedBackResourceRequest.m_oBody.m_strResourceId, l_oGetAlarmLogFeedBackResourceRespond.m_oBody.m_vecData);

    JsonParser::IJsonPtr l_pIJsonRespond = ICCGetIJsonFactory()->CreateJson();
    std::string l_strMessage = l_oGetAlarmLogFeedBackResourceRespond.ToString(l_pIJsonRespond);

	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "send message :[%s]", l_strMessage.c_str());
}

void CBusinessImpl::QueryFeedbackByAlarmID(std::string p_strAlarmID, std::vector<FeedbackData>& p_vecFeedBack)
{
    if (!m_pDBConn)
    {
        ICC_LOG_ERROR(m_pLog, "db connect is null!!!");
        return;
    }

    DataBase::SQLRequest l_SqlRequest;
    l_SqlRequest.sql_id = "select_icc_t_alarm_feedback";
	l_SqlRequest.param["alarm_id"] = p_strAlarmID;

    DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest, true);
	
    if (!l_result->IsValid())
    {
        ICC_LOG_ERROR(m_pLog, "get feedback by alarmid failed, [%s]", l_result->GetErrorMsg().c_str());
        return;
    }

	ICC_LOG_DEBUG(m_pLog, "get feedback by alarmid success, [%s]", p_strAlarmID.c_str());
    while (l_result->Next())
    {
        PROTOCOL::CGetFeedBackRespond::CFeedbackData l_oData;
        l_oData.m_strID = l_result->GetValue("id");
        l_oData.m_strAlarmID = l_result->GetValue("alarm_id");
        l_oData.m_strProcessID = l_result->GetValue("process_id");
        l_oData.m_strResultType = l_result->GetValue("result_type");
        l_oData.m_strResultContent = l_result->GetValue("result_content");
        l_oData.m_strLeaderInstruction = l_result->GetValue("leader_instruction");
        l_oData.m_strState = l_result->GetValue("state");
        l_oData.m_strTimeEdit = l_result->GetValue("time_edit");
        l_oData.m_strTimeSubmit = l_result->GetValue("time_submit");
        l_oData.m_strTimeArrived = l_result->GetValue("time_arrived");
        l_oData.m_strTimeSigned = l_result->GetValue("time_signed");
        l_oData.m_strTimePoliceDispatch = l_result->GetValue("time_police_dispatch");
        l_oData.m_strTimePoliceArrived = l_result->GetValue("time_police_arrived");
        l_oData.m_strActualOccurTime = l_result->GetValue("actual_occur_time");
        l_oData.m_strActualOccurAddr = l_result->GetValue("actual_occur_addr");
        l_oData.m_strFeedbackDeptDistrictCode = l_result->GetValue("feedback_dept_district_code");
        l_oData.m_strFeedbackDeptCode = l_result->GetValue("feedback_dept_code");
        l_oData.m_strFeedbackDeptName = l_result->GetValue("feedback_dept_name");
        l_oData.m_strFeedbackCode = l_result->GetValue("feedback_code");
        l_oData.m_strFeedbackName = l_result->GetValue("feedback_name");
        l_oData.m_strFeedbackLeaderCode = l_result->GetValue("feedback_leader_code");
        l_oData.m_strFeedbackLeaderName = l_result->GetValue("feedback_leader_name");
        l_oData.m_strProcessDeptDistrictCode = l_result->GetValue("process_dept_district_code");
        l_oData.m_strProcessDeptCode = l_result->GetValue("process_dept_code");
        l_oData.m_strProcessDeptName = l_result->GetValue("process_dept_name");
        l_oData.m_strProcessCode = l_result->GetValue("process_code");
        l_oData.m_strProcessName = l_result->GetValue("process_name");
        l_oData.m_strProcessLeaderCode = l_result->GetValue("process_leader_code");
        l_oData.m_strProcessLeaderName = l_result->GetValue("process_leader_name");
        l_oData.m_strDispatchDeptDistrictCode = l_result->GetValue("dispatch_dept_district_code");
        l_oData.m_strDispatchDeptCode = l_result->GetValue("dispatch_dept_code");
        l_oData.m_strDispatchDeptName = l_result->GetValue("dispatch_dept_name");
        l_oData.m_strDispatchCode = l_result->GetValue("dispatch_code");
        l_oData.m_strDispatchName = l_result->GetValue("dispatch_name");
        l_oData.m_strDispatchLeaderCode = l_result->GetValue("dispatch_leader_code");
        l_oData.m_strDispatchLeaderName = l_result->GetValue("dispatch_leader_name");
        l_oData.m_strPersonId = l_result->GetValue("person_id");
        l_oData.m_strPersonIdType = l_result->GetValue("person_id_type");
        l_oData.m_strPersonNationality = l_result->GetValue("person_nationality");
        l_oData.m_strPersonName = l_result->GetValue("person_name");
        l_oData.m_strPersonSlaveId = l_result->GetValue("person_slave_id");
        l_oData.m_strPersonSlaveIdType = l_result->GetValue("person_slave_id_type");
        l_oData.m_strPersonSlaveNationality = l_result->GetValue("person_slave_nationality");
        l_oData.m_strPersonSlaveName = l_result->GetValue("person_slave_name");
        l_oData.m_strAlarmFirstType = l_result->GetValue("alarm_first_type");
        l_oData.m_strAlarmSecondType = l_result->GetValue("alarm_second_type");
        l_oData.m_strAlarmThirdType = l_result->GetValue("alarm_third_type");
        l_oData.m_strAlarmFourthType = l_result->GetValue("alarm_fourth_type");
        l_oData.m_strAlarmAddrDeptName = l_result->GetValue("alarm_addr_dept_name");
        l_oData.m_strAlarmAddrFirstType = l_result->GetValue("alarm_addr_first_type");
        l_oData.m_strAlarmAddrSecondType = l_result->GetValue("alarm_addr_second_type");
        l_oData.m_strAlarmAddrThirdType = l_result->GetValue("alarm_addr_third_type");
        l_oData.m_strAlarmLongitude = l_result->GetValue("alarm_longitude");
        l_oData.m_strAlarmLatitude = l_result->GetValue("alarm_latitude");
        l_oData.m_strAlarmRegionType = l_result->GetValue("alarm_region_type");
        l_oData.m_strAlarmLocationType = l_result->GetValue("alarm_location_type");
        l_oData.m_strPeopleNumCapture = l_result->GetValue("people_num_capture");
        l_oData.m_strPeopleNumRescue = l_result->GetValue("people_num_rescue");
        l_oData.m_strPeopleNumSlightInjury = l_result->GetValue("people_num_slight_injury");
        l_oData.m_strPeopleNumSeriousInjury = l_result->GetValue("people_num_serious_injury");
        l_oData.m_strPeopleNumDeath = l_result->GetValue("people_num_death");
        l_oData.m_strPoliceNumDispatch = l_result->GetValue("police_num_dispatch");
        l_oData.m_strPoliceCarNumDispatch = l_result->GetValue("police_car_num_dispatch");
        l_oData.m_strEconomyLoss = l_result->GetValue("economy_loss");
        l_oData.m_strRetrieveEconomyLoss = l_result->GetValue("retrieve_economy_loss");
        l_oData.m_strFirePutOutTime = l_result->GetValue("fire_put_out_time");
        l_oData.m_strFireBuildingFirstType = l_result->GetValue("fire_building_first_type");
        l_oData.m_strFireBuildingSecondType = l_result->GetValue("fire_building_second_type");
        l_oData.m_strFireBuildingThirdType = l_result->GetValue("fire_building_third_type");
        l_oData.m_strFireSourceType = l_result->GetValue("fire_source_type");
        l_oData.m_strFireRegionType = l_result->GetValue("fire_region_type");
        l_oData.m_strFireCauseFirstType = l_result->GetValue("fire_cause_first_type");
        l_oData.m_strFireCauseSecondType = l_result->GetValue("fire_cause_second_type");
        l_oData.m_strFireCauseThirdType = l_result->GetValue("fire_cause_third_type");
        l_oData.m_strFireArea = l_result->GetValue("fire_area");
        l_oData.m_strTrafficRoadLevel = l_result->GetValue("traffic_road_level");
        l_oData.m_strTrafficAccidentLevel = l_result->GetValue("traffic_accident_level");
        l_oData.m_strTrafficVehicleNo = l_result->GetValue("traffic_vehicle_no");
        l_oData.m_strTrafficVehicleType = l_result->GetValue("traffic_vehicle_type");
        l_oData.m_strTrafficSlaveVehicleNo = l_result->GetValue("traffic_slave_vehicle_no");
        l_oData.m_strTrafficSlaveVehicleType = l_result->GetValue("traffic_slave_vehicle_type");
        l_oData.m_strEventType = l_result->GetValue("event_type");
        l_oData.m_strAlarmCalledNoType = l_result->GetValue("alarm_called_no_type");      
        l_oData.m_strCreateTime = l_result->GetValue("create_time");
        l_oData.m_strCreateUser = l_result->GetValue("create_user");
        l_oData.m_strUpdateTime = l_result->GetValue("update_time");
        l_oData.m_strUpdateUser = l_result->GetValue("update_user");
        p_vecFeedBack.push_back(l_oData);
    }
}

void CBusinessImpl::QueryFeedbackByProcessID(std::string p_strProcessID, std::vector<FeedbackData>& p_vecFeedBack)
{
    if (!m_pDBConn)
    {
        ICC_LOG_ERROR(m_pLog, "DB Connect Is Null!!!");
        return;
    }

    DataBase::SQLRequest l_SqlRequest;
    l_SqlRequest.sql_id = "select_icc_t_alarm_feedback";
    l_SqlRequest.param["process_id"] = p_strProcessID;

    DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest, true);
	
    if (!l_result->IsValid())
    {
        ICC_LOG_ERROR(m_pLog, "get feedback by processid failed, [%s]", l_result->GetErrorMsg().c_str());
        return;
    }
	ICC_LOG_DEBUG(m_pLog, "get feedback by processid success, [%s]", p_strProcessID.c_str());
    while (l_result->Next())
    {
        PROTOCOL::CGetFeedBackRespond::CFeedbackData l_oData;
        l_oData.m_strID = l_result->GetValue("id");
        l_oData.m_strAlarmID = l_result->GetValue("alarm_id");
        l_oData.m_strProcessID = l_result->GetValue("process_id");
        l_oData.m_strResultType = l_result->GetValue("result_type");
        l_oData.m_strResultContent = l_result->GetValue("result_content");
        l_oData.m_strLeaderInstruction = l_result->GetValue("leader_instruction");
        l_oData.m_strState = l_result->GetValue("state");
        l_oData.m_strTimeEdit = l_result->GetValue("time_edit");
        l_oData.m_strTimeSubmit = l_result->GetValue("time_submit");
        l_oData.m_strTimeArrived = l_result->GetValue("time_arrived");
        l_oData.m_strTimeSigned = l_result->GetValue("time_signed");
        l_oData.m_strTimePoliceDispatch = l_result->GetValue("time_police_dispatch");
        l_oData.m_strTimePoliceArrived = l_result->GetValue("time_police_arrived");
        l_oData.m_strActualOccurTime = l_result->GetValue("actual_occur_time");
        l_oData.m_strActualOccurAddr = l_result->GetValue("actual_occur_addr");
        l_oData.m_strFeedbackDeptDistrictCode = l_result->GetValue("feedback_dept_district_code");
        l_oData.m_strFeedbackDeptCode = l_result->GetValue("feedback_dept_code");
        l_oData.m_strFeedbackDeptName = l_result->GetValue("feedback_dept_name");
        l_oData.m_strFeedbackCode = l_result->GetValue("feedback_code");
        l_oData.m_strFeedbackName = l_result->GetValue("feedback_name");
        l_oData.m_strFeedbackLeaderCode = l_result->GetValue("feedback_leader_code");
        l_oData.m_strFeedbackLeaderName = l_result->GetValue("feedback_leader_name");
        l_oData.m_strProcessDeptDistrictCode = l_result->GetValue("process_dept_district_code");
        l_oData.m_strProcessDeptCode = l_result->GetValue("process_dept_code");
        l_oData.m_strProcessDeptName = l_result->GetValue("process_dept_name");
        l_oData.m_strProcessCode = l_result->GetValue("process_code");
        l_oData.m_strProcessName = l_result->GetValue("process_name");
        l_oData.m_strProcessLeaderCode = l_result->GetValue("process_leader_code");
        l_oData.m_strProcessLeaderName = l_result->GetValue("process_leader_name");
        l_oData.m_strDispatchDeptDistrictCode = l_result->GetValue("dispatch_dept_district_code");
        l_oData.m_strDispatchDeptCode = l_result->GetValue("dispatch_dept_code");
        l_oData.m_strDispatchDeptName = l_result->GetValue("dispatch_dept_name");
        l_oData.m_strDispatchCode = l_result->GetValue("dispatch_code");
        l_oData.m_strDispatchName = l_result->GetValue("dispatch_name");
        l_oData.m_strDispatchLeaderCode = l_result->GetValue("dispatch_leader_code");
        l_oData.m_strDispatchLeaderName = l_result->GetValue("dispatch_leader_name");
        l_oData.m_strPersonId = l_result->GetValue("person_id");
        l_oData.m_strPersonIdType = l_result->GetValue("person_id_type");
        l_oData.m_strPersonNationality = l_result->GetValue("person_nationality");
        l_oData.m_strPersonName = l_result->GetValue("person_name");
        l_oData.m_strPersonSlaveId = l_result->GetValue("person_slave_id");
        l_oData.m_strPersonSlaveIdType = l_result->GetValue("person_slave_id_type");
        l_oData.m_strPersonSlaveNationality = l_result->GetValue("person_slave_nationality");
        l_oData.m_strPersonSlaveName = l_result->GetValue("person_slave_name");
        l_oData.m_strAlarmFirstType = l_result->GetValue("alarm_first_type");
        l_oData.m_strAlarmSecondType = l_result->GetValue("alarm_second_type");
        l_oData.m_strAlarmThirdType = l_result->GetValue("alarm_third_type");
        l_oData.m_strAlarmFourthType = l_result->GetValue("alarm_fourth_type");
        l_oData.m_strAlarmAddrDeptName = l_result->GetValue("alarm_addr_dept_name");
        l_oData.m_strAlarmAddrFirstType = l_result->GetValue("alarm_addr_first_type");
        l_oData.m_strAlarmAddrSecondType = l_result->GetValue("alarm_addr_second_type");
        l_oData.m_strAlarmAddrThirdType = l_result->GetValue("alarm_addr_third_type");
        l_oData.m_strAlarmLongitude = l_result->GetValue("alarm_longitude");
        l_oData.m_strAlarmLatitude = l_result->GetValue("alarm_latitude");
        l_oData.m_strAlarmRegionType = l_result->GetValue("alarm_region_type");
        l_oData.m_strAlarmLocationType = l_result->GetValue("alarm_location_type");
        l_oData.m_strPeopleNumCapture = l_result->GetValue("people_num_capture");
        l_oData.m_strPeopleNumRescue = l_result->GetValue("people_num_rescue");
        l_oData.m_strPeopleNumSlightInjury = l_result->GetValue("people_num_slight_injury");
        l_oData.m_strPeopleNumSeriousInjury = l_result->GetValue("people_num_serious_injury");
        l_oData.m_strPeopleNumDeath = l_result->GetValue("people_num_death");
        l_oData.m_strPoliceNumDispatch = l_result->GetValue("police_num_dispatch");
        l_oData.m_strPoliceCarNumDispatch = l_result->GetValue("police_car_num_dispatch");
        l_oData.m_strEconomyLoss = l_result->GetValue("economy_loss");
        l_oData.m_strRetrieveEconomyLoss = l_result->GetValue("retrieve_economy_loss");
        l_oData.m_strFirePutOutTime = l_result->GetValue("fire_put_out_time");
        l_oData.m_strFireBuildingFirstType = l_result->GetValue("fire_building_first_type");
        l_oData.m_strFireBuildingSecondType = l_result->GetValue("fire_building_second_type");
        l_oData.m_strFireBuildingThirdType = l_result->GetValue("fire_building_third_type");
        l_oData.m_strFireSourceType = l_result->GetValue("fire_source_type");
        l_oData.m_strFireRegionType = l_result->GetValue("fire_region_type");
        l_oData.m_strFireCauseFirstType = l_result->GetValue("fire_cause_first_type");
        l_oData.m_strFireCauseSecondType = l_result->GetValue("fire_cause_second_type");
        l_oData.m_strFireCauseThirdType = l_result->GetValue("fire_cause_third_type");
        l_oData.m_strFireArea = l_result->GetValue("fire_area");
        l_oData.m_strTrafficRoadLevel = l_result->GetValue("traffic_road_level");
        l_oData.m_strTrafficAccidentLevel = l_result->GetValue("traffic_accident_level");
        l_oData.m_strTrafficVehicleNo = l_result->GetValue("traffic_vehicle_no");
        l_oData.m_strTrafficVehicleType = l_result->GetValue("traffic_vehicle_type");
        l_oData.m_strTrafficSlaveVehicleNo = l_result->GetValue("traffic_slave_vehicle_no");
        l_oData.m_strTrafficSlaveVehicleType = l_result->GetValue("traffic_slave_vehicle_type");
        l_oData.m_strEventType = l_result->GetValue("event_type");
        l_oData.m_strAlarmCalledNoType = l_result->GetValue("alarm_called_no_type");
        l_oData.m_strCreateTime = l_result->GetValue("create_time");
        l_oData.m_strCreateUser = l_result->GetValue("create_user");
        l_oData.m_strUpdateTime = l_result->GetValue("update_time");
        l_oData.m_strUpdateUser = l_result->GetValue("update_user");
        p_vecFeedBack.push_back(l_oData);
    }
}

bool CBusinessImpl::QueryFeedbackByFeedBackID(std::string p_strID, FeedbackData& p_oFeedBack)
{
    if (!m_pDBConn)
    {
        ICC_LOG_ERROR(m_pLog, "db connect is null!!!");
        return false;
    }

    DataBase::SQLRequest l_SqlRequest;
    l_SqlRequest.sql_id = "select_icc_t_alarm_feedback";
    l_SqlRequest.param["id"] = p_strID;

    DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest, true);	
    if (!l_result->IsValid())
    {
        ICC_LOG_ERROR(m_pLog, "get feedback by feedbackid failed, [%s]", l_result->GetErrorMsg().c_str());
        return false;
    }

	ICC_LOG_DEBUG(m_pLog, "get feedback by feedbackid success, [%s]", p_strID.c_str());
    while (l_result->Next())
    {
        p_oFeedBack.m_strID = l_result->GetValue("id");
        p_oFeedBack.m_strAlarmID = l_result->GetValue("alarm_id");
        p_oFeedBack.m_strProcessID = l_result->GetValue("process_id");
        p_oFeedBack.m_strResultType = l_result->GetValue("result_type");
        p_oFeedBack.m_strResultContent = l_result->GetValue("result_content");
        p_oFeedBack.m_strLeaderInstruction = l_result->GetValue("leader_instruction");
        p_oFeedBack.m_strState = l_result->GetValue("state");
        p_oFeedBack.m_strTimeEdit = l_result->GetValue("time_edit");
        p_oFeedBack.m_strTimeSubmit = l_result->GetValue("time_submit");
        p_oFeedBack.m_strTimeArrived = l_result->GetValue("time_arrived");
        p_oFeedBack.m_strTimeSigned = l_result->GetValue("time_signed");
        p_oFeedBack.m_strTimePoliceDispatch = l_result->GetValue("time_police_dispatch");
        p_oFeedBack.m_strTimePoliceArrived = l_result->GetValue("time_police_arrived");
        p_oFeedBack.m_strActualOccurTime = l_result->GetValue("actual_occur_time");
        p_oFeedBack.m_strActualOccurAddr = l_result->GetValue("actual_occur_addr");
        p_oFeedBack.m_strFeedbackDeptDistrictCode = l_result->GetValue("feedback_dept_district_code");
        p_oFeedBack.m_strFeedbackDeptCode = l_result->GetValue("feedback_dept_code");
        p_oFeedBack.m_strFeedbackDeptName = l_result->GetValue("feedback_dept_name");
        p_oFeedBack.m_strFeedbackCode = l_result->GetValue("feedback_code");
        p_oFeedBack.m_strFeedbackName = l_result->GetValue("feedback_name");
        p_oFeedBack.m_strFeedbackLeaderCode = l_result->GetValue("feedback_leader_code");
        p_oFeedBack.m_strFeedbackLeaderName = l_result->GetValue("feedback_leader_name");
        p_oFeedBack.m_strProcessDeptDistrictCode = l_result->GetValue("process_dept_district_code");
        p_oFeedBack.m_strProcessDeptCode = l_result->GetValue("process_dept_code");
        p_oFeedBack.m_strProcessDeptName = l_result->GetValue("process_dept_name");
        p_oFeedBack.m_strProcessCode = l_result->GetValue("process_code");
        p_oFeedBack.m_strProcessName = l_result->GetValue("process_name");
        p_oFeedBack.m_strProcessLeaderCode = l_result->GetValue("process_leader_code");
        p_oFeedBack.m_strProcessLeaderName = l_result->GetValue("process_leader_name");
        p_oFeedBack.m_strDispatchDeptDistrictCode = l_result->GetValue("dispatch_dept_district_code");
        p_oFeedBack.m_strDispatchDeptCode = l_result->GetValue("dispatch_dept_code");
        p_oFeedBack.m_strDispatchDeptName = l_result->GetValue("dispatch_dept_name");
        p_oFeedBack.m_strDispatchCode = l_result->GetValue("dispatch_code");
        p_oFeedBack.m_strDispatchName = l_result->GetValue("dispatch_name");
        p_oFeedBack.m_strDispatchLeaderCode = l_result->GetValue("dispatch_leader_code");
        p_oFeedBack.m_strDispatchLeaderName = l_result->GetValue("dispatch_leader_name");
        p_oFeedBack.m_strPersonId = l_result->GetValue("person_id");
        p_oFeedBack.m_strPersonIdType = l_result->GetValue("person_id_type");
        p_oFeedBack.m_strPersonNationality = l_result->GetValue("person_nationality");
        p_oFeedBack.m_strPersonName = l_result->GetValue("person_name");
        p_oFeedBack.m_strPersonSlaveId = l_result->GetValue("person_slave_id");
        p_oFeedBack.m_strPersonSlaveIdType = l_result->GetValue("person_slave_id_type");
        p_oFeedBack.m_strPersonSlaveNationality = l_result->GetValue("person_slave_nationality");
        p_oFeedBack.m_strPersonSlaveName = l_result->GetValue("person_slave_name");
        p_oFeedBack.m_strAlarmFirstType = l_result->GetValue("alarm_first_type");
        p_oFeedBack.m_strAlarmSecondType = l_result->GetValue("alarm_second_type");
        p_oFeedBack.m_strAlarmThirdType = l_result->GetValue("alarm_third_type");
        p_oFeedBack.m_strAlarmFourthType = l_result->GetValue("alarm_fourth_type");
        p_oFeedBack.m_strAlarmAddrDeptName = l_result->GetValue("alarm_addr_dept_name");
        p_oFeedBack.m_strAlarmAddrFirstType = l_result->GetValue("alarm_addr_first_type");
        p_oFeedBack.m_strAlarmAddrSecondType = l_result->GetValue("alarm_addr_second_type");
        p_oFeedBack.m_strAlarmAddrThirdType = l_result->GetValue("alarm_addr_third_type");
        p_oFeedBack.m_strAlarmLongitude = l_result->GetValue("alarm_longitude");
        p_oFeedBack.m_strAlarmLatitude = l_result->GetValue("alarm_latitude");
        p_oFeedBack.m_strAlarmRegionType = l_result->GetValue("alarm_region_type");
        p_oFeedBack.m_strAlarmLocationType = l_result->GetValue("alarm_location_type");
        p_oFeedBack.m_strPeopleNumCapture = l_result->GetValue("people_num_capture");
        p_oFeedBack.m_strPeopleNumRescue = l_result->GetValue("people_num_rescue");
        p_oFeedBack.m_strPeopleNumSlightInjury = l_result->GetValue("people_num_slight_injury");
        p_oFeedBack.m_strPeopleNumSeriousInjury = l_result->GetValue("people_num_serious_injury");
        p_oFeedBack.m_strPeopleNumDeath = l_result->GetValue("people_num_death");
        p_oFeedBack.m_strPoliceNumDispatch = l_result->GetValue("police_num_dispatch");
        p_oFeedBack.m_strPoliceCarNumDispatch = l_result->GetValue("police_car_num_dispatch");
        p_oFeedBack.m_strEconomyLoss = l_result->GetValue("economy_loss");
        p_oFeedBack.m_strRetrieveEconomyLoss = l_result->GetValue("retrieve_economy_loss");
        p_oFeedBack.m_strFirePutOutTime = l_result->GetValue("fire_put_out_time");
        p_oFeedBack.m_strFireBuildingFirstType = l_result->GetValue("fire_building_first_type");
        p_oFeedBack.m_strFireBuildingSecondType = l_result->GetValue("fire_building_second_type");
        p_oFeedBack.m_strFireBuildingThirdType = l_result->GetValue("fire_building_third_type");
        p_oFeedBack.m_strFireSourceType = l_result->GetValue("fire_source_type");
        p_oFeedBack.m_strFireRegionType = l_result->GetValue("fire_region_type");
        p_oFeedBack.m_strFireCauseFirstType = l_result->GetValue("fire_cause_first_type");
        p_oFeedBack.m_strFireCauseSecondType = l_result->GetValue("fire_cause_second_type");
        p_oFeedBack.m_strFireCauseThirdType = l_result->GetValue("fire_cause_third_type");
        p_oFeedBack.m_strFireArea = l_result->GetValue("fire_area");
        p_oFeedBack.m_strTrafficRoadLevel = l_result->GetValue("traffic_road_level");
        p_oFeedBack.m_strTrafficAccidentLevel = l_result->GetValue("traffic_accident_level");
        p_oFeedBack.m_strTrafficVehicleNo = l_result->GetValue("traffic_vehicle_no");
        p_oFeedBack.m_strTrafficVehicleType = l_result->GetValue("traffic_vehicle_type");
        p_oFeedBack.m_strTrafficSlaveVehicleNo = l_result->GetValue("traffic_slave_vehicle_no");
        p_oFeedBack.m_strTrafficSlaveVehicleType = l_result->GetValue("traffic_slave_vehicle_type");
        p_oFeedBack.m_strEventType = l_result->GetValue("event_type");
        p_oFeedBack.m_strAlarmCalledNoType = l_result->GetValue("alarm_called_no_type");
        p_oFeedBack.m_strCreateTime = l_result->GetValue("create_time");
        p_oFeedBack.m_strCreateUser = l_result->GetValue("create_user");
        p_oFeedBack.m_strUpdateTime = l_result->GetValue("update_time");
        p_oFeedBack.m_strUpdateUser = l_result->GetValue("update_user");
    }
    return !p_oFeedBack.m_strID.empty();
}

bool CBusinessImpl::CheckIsEdit(const PROTOCOL::CSetFeedBackRequest::CBody& p_oFeedBackRequest, const FeedbackData& p_oFeedBack)
{
	if (p_oFeedBackRequest.m_strID == p_oFeedBack.m_strID&&
		p_oFeedBackRequest.m_strAlarmID == p_oFeedBack.m_strAlarmID&&
		p_oFeedBackRequest.m_strProcessID == p_oFeedBack.m_strProcessID&&
		p_oFeedBackRequest.m_strResultType == p_oFeedBack.m_strResultType&&
		p_oFeedBackRequest.m_strResultContent == p_oFeedBack.m_strResultContent&&
		p_oFeedBackRequest.m_strLeaderInstruction == p_oFeedBack.m_strLeaderInstruction&&
		p_oFeedBackRequest.m_strState == p_oFeedBack.m_strState&&
		p_oFeedBackRequest.m_strTimePoliceDispatch == p_oFeedBack.m_strTimePoliceDispatch&&
		p_oFeedBackRequest.m_strTimePoliceArrived == p_oFeedBack.m_strTimePoliceArrived&&
		p_oFeedBackRequest.m_strActualOccurTime == p_oFeedBack.m_strActualOccurTime&&
		p_oFeedBackRequest.m_strActualOccurAddr == p_oFeedBack.m_strActualOccurAddr&&
		p_oFeedBackRequest.m_strFeedbackDeptDistrictCode == p_oFeedBack.m_strFeedbackDeptDistrictCode&&
		p_oFeedBackRequest.m_strFeedbackDeptCode == p_oFeedBack.m_strFeedbackDeptCode&&
		p_oFeedBackRequest.m_strFeedbackDeptName == p_oFeedBack.m_strFeedbackDeptName&&
		p_oFeedBackRequest.m_strFeedbackCode == p_oFeedBack.m_strFeedbackCode&&
		p_oFeedBackRequest.m_strFeedbackName == p_oFeedBack.m_strFeedbackName&&
		p_oFeedBackRequest.m_strFeedbackLeaderCode == p_oFeedBack.m_strFeedbackLeaderCode&&
		p_oFeedBackRequest.m_strFeedbackLeaderName == p_oFeedBack.m_strFeedbackLeaderName&&
		p_oFeedBackRequest.m_strProcessDeptDistrictCode == p_oFeedBack.m_strProcessDeptDistrictCode&&
		p_oFeedBackRequest.m_strProcessDeptCode == p_oFeedBack.m_strProcessDeptCode&&
		p_oFeedBackRequest.m_strProcessDeptName == p_oFeedBack.m_strProcessDeptName&&
		p_oFeedBackRequest.m_strProcessCode == p_oFeedBack.m_strProcessCode&&
		p_oFeedBackRequest.m_strProcessName == p_oFeedBack.m_strProcessName&&
		p_oFeedBackRequest.m_strProcessLeaderCode == p_oFeedBack.m_strProcessLeaderCode&&
		p_oFeedBackRequest.m_strProcessLeaderName == p_oFeedBack.m_strProcessLeaderName&&
		p_oFeedBackRequest.m_strDispatchDeptDistrictCode == p_oFeedBack.m_strDispatchDeptDistrictCode&&
		p_oFeedBackRequest.m_strDispatchDeptCode == p_oFeedBack.m_strDispatchDeptCode&&
		p_oFeedBackRequest.m_strDispatchDeptName == p_oFeedBack.m_strDispatchDeptName&&
		p_oFeedBackRequest.m_strDispatchCode == p_oFeedBack.m_strDispatchCode&&
		p_oFeedBackRequest.m_strDispatchName == p_oFeedBack.m_strDispatchName&&
		p_oFeedBackRequest.m_strDispatchLeaderCode == p_oFeedBack.m_strDispatchLeaderCode&&
		p_oFeedBackRequest.m_strDispatchLeaderName == p_oFeedBack.m_strDispatchLeaderName&&
		p_oFeedBackRequest.m_strPersonId == p_oFeedBack.m_strPersonId&&
		p_oFeedBackRequest.m_strPersonIdType == p_oFeedBack.m_strPersonIdType&&
		p_oFeedBackRequest.m_strPersonNationality == p_oFeedBack.m_strPersonNationality&&
		p_oFeedBackRequest.m_strPersonName == p_oFeedBack.m_strPersonName&&
		p_oFeedBackRequest.m_strPersonSlaveId == p_oFeedBack.m_strPersonSlaveId&&
		p_oFeedBackRequest.m_strPersonSlaveIdType == p_oFeedBack.m_strPersonSlaveIdType&&
		p_oFeedBackRequest.m_strPersonSlaveNationality == p_oFeedBack.m_strPersonSlaveNationality&&
		p_oFeedBackRequest.m_strPersonSlaveName == p_oFeedBack.m_strPersonSlaveName&&
		p_oFeedBackRequest.m_strAlarmCalledNoType == p_oFeedBack.m_strAlarmCalledNoType&&
		p_oFeedBackRequest.m_strAlarmFirstType == p_oFeedBack.m_strAlarmFirstType&&
		p_oFeedBackRequest.m_strAlarmSecondType == p_oFeedBack.m_strAlarmSecondType&&
		p_oFeedBackRequest.m_strAlarmThirdType == p_oFeedBack.m_strAlarmThirdType&&
		p_oFeedBackRequest.m_strAlarmFourthType == p_oFeedBack.m_strAlarmFourthType&&
		p_oFeedBackRequest.m_strAlarmAddrDeptName == p_oFeedBack.m_strAlarmAddrDeptName&&
		p_oFeedBackRequest.m_strAlarmAddrFirstType == p_oFeedBack.m_strAlarmAddrFirstType&&
		p_oFeedBackRequest.m_strAlarmAddrSecondType == p_oFeedBack.m_strAlarmAddrSecondType&&
		p_oFeedBackRequest.m_strAlarmAddrThirdType == p_oFeedBack.m_strAlarmAddrThirdType&&
		p_oFeedBackRequest.m_strAlarmLongitude == p_oFeedBack.m_strAlarmLongitude&&
		p_oFeedBackRequest.m_strAlarmLatitude == p_oFeedBack.m_strAlarmLatitude&&
		p_oFeedBackRequest.m_strAlarmRegionType == p_oFeedBack.m_strAlarmRegionType&&
		p_oFeedBackRequest.m_strAlarmLocationType == p_oFeedBack.m_strAlarmLocationType&&
		p_oFeedBackRequest.m_strPeopleNumCapture == p_oFeedBack.m_strPeopleNumCapture&&
		p_oFeedBackRequest.m_strPeopleNumRescue == p_oFeedBack.m_strPeopleNumRescue&&
		p_oFeedBackRequest.m_strPeopleNumSlightInjury == p_oFeedBack.m_strPeopleNumSlightInjury&&
		p_oFeedBackRequest.m_strPeopleNumSeriousInjury == p_oFeedBack.m_strPeopleNumSeriousInjury&&
		p_oFeedBackRequest.m_strPeopleNumDeath == p_oFeedBack.m_strPeopleNumDeath&&
		p_oFeedBackRequest.m_strPoliceNumDispatch == p_oFeedBack.m_strPoliceNumDispatch&&
		p_oFeedBackRequest.m_strPoliceCarNumDispatch == p_oFeedBack.m_strPoliceCarNumDispatch&&
		p_oFeedBackRequest.m_strEconomyLoss == p_oFeedBack.m_strEconomyLoss&&
		p_oFeedBackRequest.m_strRetrieveEconomyLoss == p_oFeedBack.m_strRetrieveEconomyLoss&&
		p_oFeedBackRequest.m_strFirePutOutTime == p_oFeedBack.m_strFirePutOutTime&&
		p_oFeedBackRequest.m_strFireBuildingFirstType == p_oFeedBack.m_strFireBuildingFirstType&&
		p_oFeedBackRequest.m_strFireBuildingSecondType == p_oFeedBack.m_strFireBuildingSecondType&&
		p_oFeedBackRequest.m_strFireBuildingThirdType == p_oFeedBack.m_strFireBuildingThirdType&&
		p_oFeedBackRequest.m_strFireSourceType == p_oFeedBack.m_strFireSourceType&&
		p_oFeedBackRequest.m_strFireRegionType == p_oFeedBack.m_strFireRegionType&&
		p_oFeedBackRequest.m_strFireCauseFirstType == p_oFeedBack.m_strFireCauseFirstType&&
		p_oFeedBackRequest.m_strFireCauseSecondType == p_oFeedBack.m_strFireCauseSecondType&&
		p_oFeedBackRequest.m_strFireCauseThirdType == p_oFeedBack.m_strFireCauseThirdType&&
		p_oFeedBackRequest.m_strFireArea == p_oFeedBack.m_strFireArea&&
		p_oFeedBackRequest.m_strTrafficRoadLevel == p_oFeedBack.m_strTrafficRoadLevel&&
		p_oFeedBackRequest.m_strTrafficAccidentLevel == p_oFeedBack.m_strTrafficAccidentLevel&&
		p_oFeedBackRequest.m_strTrafficVehicleNo == p_oFeedBack.m_strTrafficVehicleNo&&
		p_oFeedBackRequest.m_strTrafficVehicleType == p_oFeedBack.m_strTrafficVehicleType&&
		p_oFeedBackRequest.m_strTrafficSlaveVehicleNo == p_oFeedBack.m_strTrafficSlaveVehicleNo&&
		p_oFeedBackRequest.m_strEventType == p_oFeedBack.m_strEventType&&
		p_oFeedBackRequest.m_strTrafficSlaveVehicleType == p_oFeedBack.m_strTrafficSlaveVehicleType		
		)
	{
		return true;
	}
	return false;
}

std::string CBusinessImpl::GenAlarmLogContent(std::vector<std::string> p_vecParamList)
{
    JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
    unsigned int l_iIndex = 0;
    for (auto it = p_vecParamList.cbegin(); it != p_vecParamList.cend(); it++)
    {
        l_pIJson->SetNodeValue("/param/" + std::to_string(l_iIndex), *it);
        l_iIndex++;
    }
    return l_pIJson->ToString();
}

std::string CBusinessImpl::GenAlarmLogAttach(std::string& type, std::string& id)
{
    JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();

    std::string l_strPath = "/";
    std::string l_strNum = std::to_string(0);

    if (0 == type.compare("7"))
    {
        l_pIJson->SetNodeValue(l_strPath + l_strNum + "/type", "jjlyh");
    }
    else
    {
        l_pIJson->SetNodeValue(l_strPath + l_strNum + "/type", type);
    }

    l_pIJson->SetNodeValue(l_strPath + l_strNum + "/id", id);
    l_pIJson->SetNodeValue(l_strPath + l_strNum + "/name", "");
    l_pIJson->SetNodeValue(l_strPath + l_strNum + "/path", "");

    return l_pIJson->ToString();
}

bool CBusinessImpl::AddFeedbackBook(FeedbackData& p_oFeedBack, std::string p_strGuid, std::string p_strFlag)
{
	DataBase::SQLRequest l_oSetFeedbackSQLReq;
	l_oSetFeedbackSQLReq.sql_id = "insert_icc_t_alarm_feedback_book";	
	std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();

    l_oSetFeedbackSQLReq.param["guid"] = p_strGuid;
    l_oSetFeedbackSQLReq.param["id"] = p_oFeedBack.m_strID;
	l_oSetFeedbackSQLReq.param["alarm_id"] = p_oFeedBack.m_strAlarmID;
	l_oSetFeedbackSQLReq.param["process_id"] = p_oFeedBack.m_strProcessID;
	l_oSetFeedbackSQLReq.param["result_type"] = p_oFeedBack.m_strResultType;
	l_oSetFeedbackSQLReq.param["result_content"] = p_oFeedBack.m_strResultContent;
	l_oSetFeedbackSQLReq.param["leader_instruction"] = p_oFeedBack.m_strLeaderInstruction;
	l_oSetFeedbackSQLReq.param["state"] = p_oFeedBack.m_strState;
	l_oSetFeedbackSQLReq.param["time_edit"] = p_oFeedBack.m_strTimeEdit;
	l_oSetFeedbackSQLReq.param["time_submit"] = p_oFeedBack.m_strTimeSubmit;
	l_oSetFeedbackSQLReq.param["time_arrived"] = p_oFeedBack.m_strTimeArrived;
	l_oSetFeedbackSQLReq.param["time_signed"] = p_oFeedBack.m_strTimeSigned;
	l_oSetFeedbackSQLReq.param["time_police_dispatch"] = p_oFeedBack.m_strTimePoliceDispatch;
	l_oSetFeedbackSQLReq.param["time_police_arrived"] = p_oFeedBack.m_strTimePoliceArrived;
	l_oSetFeedbackSQLReq.param["actual_occur_time"] = p_oFeedBack.m_strActualOccurTime;
	l_oSetFeedbackSQLReq.param["actual_occur_addr"] = p_oFeedBack.m_strActualOccurAddr;
	l_oSetFeedbackSQLReq.param["feedback_dept_district_code"] = p_oFeedBack.m_strFeedbackDeptDistrictCode;
	l_oSetFeedbackSQLReq.param["feedback_dept_code"] = p_oFeedBack.m_strFeedbackDeptCode;
	l_oSetFeedbackSQLReq.param["feedback_dept_name"] = p_oFeedBack.m_strFeedbackDeptName;
	l_oSetFeedbackSQLReq.param["feedback_code"] = p_oFeedBack.m_strFeedbackCode;
	l_oSetFeedbackSQLReq.param["feedback_name"] = p_oFeedBack.m_strFeedbackName;
	l_oSetFeedbackSQLReq.param["feedback_leader_code"] = p_oFeedBack.m_strFeedbackLeaderCode;
	l_oSetFeedbackSQLReq.param["feedback_leader_name"] = p_oFeedBack.m_strFeedbackLeaderName;
	l_oSetFeedbackSQLReq.param["process_dept_district_code"] = p_oFeedBack.m_strProcessDeptDistrictCode;
	l_oSetFeedbackSQLReq.param["process_dept_code"] = p_oFeedBack.m_strProcessDeptCode;
	l_oSetFeedbackSQLReq.param["process_dept_name"] = p_oFeedBack.m_strProcessDeptName;
	l_oSetFeedbackSQLReq.param["process_code"] = p_oFeedBack.m_strProcessCode;
	l_oSetFeedbackSQLReq.param["process_name"] = p_oFeedBack.m_strProcessName;
	l_oSetFeedbackSQLReq.param["process_leader_code"] = p_oFeedBack.m_strProcessLeaderCode;
	l_oSetFeedbackSQLReq.param["process_leader_name"] = p_oFeedBack.m_strProcessLeaderName;
	l_oSetFeedbackSQLReq.param["dispatch_dept_district_code"] = p_oFeedBack.m_strDispatchDeptDistrictCode;
	l_oSetFeedbackSQLReq.param["dispatch_dept_code"] = p_oFeedBack.m_strDispatchDeptCode;
	l_oSetFeedbackSQLReq.param["dispatch_dept_name"] = p_oFeedBack.m_strDispatchDeptName;
	l_oSetFeedbackSQLReq.param["dispatch_code"] = p_oFeedBack.m_strDispatchCode;
	l_oSetFeedbackSQLReq.param["dispatch_name"] = p_oFeedBack.m_strDispatchName;
	l_oSetFeedbackSQLReq.param["dispatch_leader_code"] = p_oFeedBack.m_strDispatchLeaderCode;
	l_oSetFeedbackSQLReq.param["dispatch_leader_name"] = p_oFeedBack.m_strDispatchLeaderName;
	l_oSetFeedbackSQLReq.param["person_id"] = p_oFeedBack.m_strPersonId;
	l_oSetFeedbackSQLReq.param["person_id_type"] = p_oFeedBack.m_strPersonIdType;
	l_oSetFeedbackSQLReq.param["person_nationality"] = p_oFeedBack.m_strPersonNationality;
	l_oSetFeedbackSQLReq.param["person_name"] = p_oFeedBack.m_strPersonName;
	l_oSetFeedbackSQLReq.param["person_slave_id"] = p_oFeedBack.m_strPersonSlaveId;
	l_oSetFeedbackSQLReq.param["person_slave_id_type"] = p_oFeedBack.m_strPersonSlaveIdType;
	l_oSetFeedbackSQLReq.param["person_slave_nationality"] = p_oFeedBack.m_strPersonSlaveNationality;
	l_oSetFeedbackSQLReq.param["person_slave_name"] = p_oFeedBack.m_strPersonSlaveName;
	l_oSetFeedbackSQLReq.param["alarm_first_type"] = p_oFeedBack.m_strAlarmFirstType;
	l_oSetFeedbackSQLReq.param["alarm_second_type"] = p_oFeedBack.m_strAlarmSecondType;
	l_oSetFeedbackSQLReq.param["alarm_third_type"] = p_oFeedBack.m_strAlarmThirdType;
	l_oSetFeedbackSQLReq.param["alarm_fourth_type"] = p_oFeedBack.m_strAlarmFourthType;
	l_oSetFeedbackSQLReq.param["alarm_addr_dept_name"] = p_oFeedBack.m_strAlarmAddrDeptName;
	l_oSetFeedbackSQLReq.param["alarm_addr_first_type"] = p_oFeedBack.m_strAlarmAddrFirstType;
	l_oSetFeedbackSQLReq.param["alarm_addr_second_type"] = p_oFeedBack.m_strAlarmAddrSecondType;
	l_oSetFeedbackSQLReq.param["alarm_addr_third_type"] = p_oFeedBack.m_strAlarmAddrThirdType;
	l_oSetFeedbackSQLReq.param["alarm_longitude"] = p_oFeedBack.m_strAlarmLongitude;
	l_oSetFeedbackSQLReq.param["alarm_latitude"] = p_oFeedBack.m_strAlarmLatitude;
	l_oSetFeedbackSQLReq.param["alarm_region_type"] = p_oFeedBack.m_strAlarmRegionType;
	l_oSetFeedbackSQLReq.param["alarm_location_type"] = p_oFeedBack.m_strAlarmLocationType;
	l_oSetFeedbackSQLReq.param["people_num_capture"] = p_oFeedBack.m_strPeopleNumCapture;
	l_oSetFeedbackSQLReq.param["people_num_rescue"] = p_oFeedBack.m_strPeopleNumRescue;
	l_oSetFeedbackSQLReq.param["people_num_slight_injury"] = p_oFeedBack.m_strPeopleNumSlightInjury;
	l_oSetFeedbackSQLReq.param["people_num_serious_injury"] = p_oFeedBack.m_strPeopleNumSeriousInjury;
	l_oSetFeedbackSQLReq.param["people_num_death"] = p_oFeedBack.m_strPeopleNumDeath;
	l_oSetFeedbackSQLReq.param["police_num_dispatch"] = p_oFeedBack.m_strPoliceNumDispatch;
	l_oSetFeedbackSQLReq.param["police_car_num_dispatch"] = p_oFeedBack.m_strPoliceCarNumDispatch;
	l_oSetFeedbackSQLReq.param["economy_loss"] = p_oFeedBack.m_strEconomyLoss;
	l_oSetFeedbackSQLReq.param["retrieve_economy_loss"] = p_oFeedBack.m_strRetrieveEconomyLoss;
	l_oSetFeedbackSQLReq.param["fire_put_out_time"] = p_oFeedBack.m_strFirePutOutTime;
	l_oSetFeedbackSQLReq.param["fire_building_first_type"] = p_oFeedBack.m_strFireBuildingFirstType;
	l_oSetFeedbackSQLReq.param["fire_building_second_type"] = p_oFeedBack.m_strFireBuildingSecondType;
	l_oSetFeedbackSQLReq.param["fire_building_third_type"] = p_oFeedBack.m_strFireBuildingThirdType;
	l_oSetFeedbackSQLReq.param["fire_source_type"] = p_oFeedBack.m_strFireSourceType;
	l_oSetFeedbackSQLReq.param["fire_region_type"] = p_oFeedBack.m_strFireRegionType;
	l_oSetFeedbackSQLReq.param["fire_cause_first_type"] = p_oFeedBack.m_strFireCauseFirstType;
	l_oSetFeedbackSQLReq.param["fire_cause_second_type"] = p_oFeedBack.m_strFireCauseSecondType;
	l_oSetFeedbackSQLReq.param["fire_cause_third_type"] = p_oFeedBack.m_strFireCauseThirdType;
	l_oSetFeedbackSQLReq.param["fire_area"] = p_oFeedBack.m_strFireArea;
	l_oSetFeedbackSQLReq.param["traffic_road_level"] = p_oFeedBack.m_strTrafficRoadLevel;
	l_oSetFeedbackSQLReq.param["traffic_accident_level"] = p_oFeedBack.m_strTrafficAccidentLevel;
	l_oSetFeedbackSQLReq.param["traffic_vehicle_no"] = p_oFeedBack.m_strTrafficVehicleNo;
	l_oSetFeedbackSQLReq.param["traffic_vehicle_type"] = p_oFeedBack.m_strTrafficVehicleType;
	l_oSetFeedbackSQLReq.param["traffic_slave_vehicle_no"] = p_oFeedBack.m_strTrafficSlaveVehicleNo;
	l_oSetFeedbackSQLReq.param["traffic_slave_vehicle_type"] = p_oFeedBack.m_strTrafficSlaveVehicleType;
	l_oSetFeedbackSQLReq.param["event_type"] = p_oFeedBack.m_strEventType;
    l_oSetFeedbackSQLReq.param["flag"] = p_strFlag;
	l_oSetFeedbackSQLReq.param["alarm_called_no_type"] = p_oFeedBack.m_strAlarmCalledNoType;
	l_oSetFeedbackSQLReq.param["create_user"] = p_oFeedBack.m_strFeedbackCode;
	l_oSetFeedbackSQLReq.param["create_time"] = l_strCurrentTime;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSetFeedbackSQLReq);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "add feedback book info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

bool CBusinessImpl::GetAlarmProcess(std::string l_strID,ProcessData& l_oProcess)
{
    DataBase::SQLRequest l_oSeleteProcessSQLReq;
    l_oSeleteProcessSQLReq.sql_id = "select_icc_t_pjdb";
    l_oSeleteProcessSQLReq.param["id"] = l_strID;

    std::string strTime = m_pDateTime->GetDispatchIdTime(l_strID);
    if (strTime != "")
    {
        l_oSeleteProcessSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 86400 * 15);
        l_oSeleteProcessSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
    }

    if (!m_pDBConn)
    {
        ICC_LOG_ERROR(m_pLog, "db connect is null!!!");
        return false;
    }
    DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oSeleteProcessSQLReq, true);	
    if (!l_pResult->IsValid())
    {
        ICC_LOG_ERROR(m_pLog, "get process failed, [%s]", l_pResult->GetErrorMsg().c_str());
        return false;
    }

	ICC_LOG_DEBUG(m_pLog, "get process success, [%s]", l_strID.c_str());

    while (l_pResult->Next())
    {
        l_oProcess.m_strDispatchDeptDistrictCode = l_pResult->GetValue("dispatch_dept_district_code");
        l_oProcess.m_strID = l_pResult->GetValue("id");
        l_oProcess.m_strAlarmID = l_pResult->GetValue("alarm_id");
        l_oProcess.m_strDispatchDeptCode = l_pResult->GetValue("dispatch_dept_code");
        l_oProcess.m_strDispatchCode = l_pResult->GetValue("dispatch_code");
        l_oProcess.m_strDispatchName = l_pResult->GetValue("dispatch_name");
        l_oProcess.m_strRecordID = l_pResult->GetValue("record_id");
        l_oProcess.m_strDispatchSuggestion = l_pResult->GetValue("dispatch_suggestion");
        l_oProcess.m_strProcessDeptCode = l_pResult->GetValue("process_dept_code");
        l_oProcess.m_strTimeSubmit = l_pResult->GetValue("time_submit");
        l_oProcess.m_strTimeArrived = l_pResult->GetValue("time_arrived");
        l_oProcess.m_strTimeSigned = l_pResult->GetValue("time_signed");
        l_oProcess.m_strProcessName = l_pResult->GetValue("process_name");
        l_oProcess.m_strProcessCode = l_pResult->GetValue("process_code");
        l_oProcess.m_strDispatchPersonnel = l_pResult->GetValue("dispatch_personnel");
        l_oProcess.m_strDispatchVehicles = l_pResult->GetValue("dispatch_vehicles");
        l_oProcess.m_strDispatchBoats = l_pResult->GetValue("dispatch_boats");
        l_oProcess.m_strState = l_pResult->GetValue("state");
        l_oProcess.m_strCreateTime = l_pResult->GetValue("create_time");
        l_oProcess.m_strUpdateTime = l_pResult->GetValue("update_time");
        l_oProcess.m_strDispatchDeptName = l_pResult->GetValue("dispatch_dept_name");
        l_oProcess.m_strDispatchDeptOrgCode = l_pResult->GetValue("dispatch_dept_org_code");
        l_oProcess.m_strProcessDeptName = l_pResult->GetValue("process_dept_name");
        l_oProcess.m_strProcessDeptOrgCode = l_pResult->GetValue("process_dept_org_code");
        l_oProcess.m_strProcessObjectType = l_pResult->GetValue("process_object_type");
        l_oProcess.m_strProcessObjectName = l_pResult->GetValue("process_object_name");
        l_oProcess.m_strProcessObjectCode = l_pResult->GetValue("process_object_code");
        l_oProcess.m_strBusinessStatus = l_pResult->GetValue("business_status");
        l_oProcess.m_strSeatCode = l_pResult->GetValue("seat_code");
        l_oProcess.m_strCancelTime = l_pResult->GetValue("cancel_time");
        l_oProcess.m_strCancelReason = l_pResult->GetValue("cancel_reason");
        l_oProcess.m_strIsAutoAssignJob = l_pResult->GetValue("is_auto_assign_job","0");
        l_oProcess.m_strCreateUser = l_pResult->GetValue("create_user");
        l_oProcess.m_strUpdateUser = l_pResult->GetValue("update_user");
        l_oProcess.m_strOvertimeState = l_pResult->GetValue("overtime_state");

        l_oProcess.m_strProcessObjectID = l_pResult->GetValue("process_object_id");
        l_oProcess.m_strTransfDeptOrjCode = l_pResult->GetValue("transfers_dept_org_code");
        l_oProcess.m_strIsOver = l_pResult->GetValue("is_over","0");
        l_oProcess.m_strOverRemark = l_pResult->GetValue("over_remark");
        l_oProcess.m_strParentID = l_pResult->GetValue("parent_id");
        l_oProcess.m_strGZLDM = l_pResult->GetValue("flow_code");

        l_oProcess.m_strCentreProcessDeptCode = l_pResult->GetValue("centre_process_dept_code");
        l_oProcess.m_strCentreAlarmDeptCode = l_pResult->GetValue("centre_alarm_dept_code");
        l_oProcess.m_strDispatchDeptDistrictName = l_pResult->GetValue("dispatch_dept_district_name");
        l_oProcess.m_strLinkedDispatchCode = l_pResult->GetValue("linked_dispatch_code");
        l_oProcess.m_strOverTime = l_pResult->GetValue("over_time");
        l_oProcess.m_strFinishedTimeoutState = l_pResult->GetValue("finished_timeout_state");
        l_oProcess.m_strPoliceType = l_pResult->GetValue("police_type");
        l_oProcess.m_strProcessDeptShortName = l_pResult->GetValue("process_dept_short_name");
        l_oProcess.m_strDispatchDeptShortName = l_pResult->GetValue("dispatch_dept_short_name");
        l_oProcess.m_strCreateTeminal = l_pResult->GetValue("createteminal");
        l_oProcess.m_strUpdateTeminal = l_pResult->GetValue("updateteminal");
    }
    return true;
}

bool CBusinessImpl::GetLinkedDispatchByAlarmID(std::string p_strAlarmID, std::vector<LinkedData>& p_vecProcess)
{
    if (p_strAlarmID.empty()) {
        ICC_LOG_ERROR(m_pLog, " p_strAlarmID Is Null!!!");
        return false;
    }

    DataBase::SQLRequest l_oSeleteLinkedSQLReq;
    l_oSeleteLinkedSQLReq.sql_id = "postgres_select_icc_t_linked_dispatch";
    l_oSeleteLinkedSQLReq.param["alarm_id"] = p_strAlarmID;
    DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oSeleteLinkedSQLReq, true);
    ICC_LOG_DEBUG(m_pLog, "postgres_select_icc_t_linked_dispatch sql:[%s]", l_pResult->GetSQL().c_str());
    if (!l_pResult->IsValid())
    {
        ICC_LOG_ERROR(m_pLog, "get linked by alarm failed :[%s]", l_pResult->GetErrorMsg().c_str());
        return false;
    }

    ICC_LOG_DEBUG(m_pLog, "get linked by alarm success, [%s]", p_strAlarmID.c_str());

    while (l_pResult->Next())
    {
        LinkedData l_oProcess;
        l_oProcess.m_bIsNewProcess = false;
        l_oProcess.m_strID = l_pResult->GetValue("id");
        l_oProcess.m_strAlarmID = l_pResult->GetValue("alarm_id");
        l_oProcess.m_strState = l_pResult->GetValue("state");
        l_oProcess.m_strLinkedOrgCode = l_pResult->GetValue("linked_org_code");
        l_oProcess.m_strLinkedOrgName = l_pResult->GetValue("linked_org_name");
        l_oProcess.m_strLinkedOrgType = l_pResult->GetValue("linked_org_type");
        l_oProcess.m_strDispatchCode = l_pResult->GetValue("dispatch_code");
        l_oProcess.m_strDispatchName = l_pResult->GetValue("dispatch_name");
        l_oProcess.m_strCreateTime = l_pResult->GetValue("create_time");
        l_oProcess.m_strUpdateTime = l_pResult->GetValue("update_time");
        l_oProcess.m_strCreateUser = l_pResult->GetValue("create_user");
        l_oProcess.m_strUpdateUser = l_pResult->GetValue("update_user");
        l_oProcess.m_strResult = l_pResult->GetValue("result");
        p_vecProcess.push_back(l_oProcess);
    }
    return true;
}

bool CBusinessImpl::GetAlarmProcessByAlarmID(std::string p_strAlarmID, std::vector<ProcessData>& p_vecProcess)
{
    DataBase::SQLRequest l_oSeleteProcessSQLReq;
    l_oSeleteProcessSQLReq.sql_id = "select_icc_t_pjdb";
    l_oSeleteProcessSQLReq.param["alarm_id"] = p_strAlarmID;

    std::string strTime = m_pDateTime->GetAlarmIdTime(p_strAlarmID);
    if (strTime != "")
    {
        l_oSeleteProcessSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime);
        l_oSeleteProcessSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
    }

    if (!m_pDBConn)
    {
        ICC_LOG_ERROR(m_pLog, "DB Connect Is Null!!!");
        return false;
    }
    DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oSeleteProcessSQLReq, true);
	
    if (!l_pResult->IsValid())
    {
        ICC_LOG_ERROR(m_pLog, "get process by alarm failed :[%s]", l_pResult->GetErrorMsg().c_str());
        return false;
    }

	ICC_LOG_DEBUG(m_pLog, "get process by alarm success, [%s]", p_strAlarmID.c_str());

    while (l_pResult->Next())
    {
        ProcessData l_oProcess;
        l_oProcess.m_bIsNewProcess = false;
        l_oProcess.m_strDispatchDeptDistrictCode = l_pResult->GetValue("dispatch_dept_district_code");
        l_oProcess.m_strID = l_pResult->GetValue("id");
        l_oProcess.m_strAlarmID = l_pResult->GetValue("alarm_id");
        l_oProcess.m_strDispatchDeptCode = l_pResult->GetValue("dispatch_dept_code");
        l_oProcess.m_strDispatchCode = l_pResult->GetValue("dispatch_code");
        l_oProcess.m_strDispatchName = l_pResult->GetValue("dispatch_name");
        l_oProcess.m_strRecordID = l_pResult->GetValue("record_id");
        l_oProcess.m_strDispatchSuggestion = l_pResult->GetValue("dispatch_suggestion");
        l_oProcess.m_strProcessDeptCode = l_pResult->GetValue("process_dept_code");
        l_oProcess.m_strTimeSubmit = l_pResult->GetValue("time_submit");
        l_oProcess.m_strTimeArrived = l_pResult->GetValue("time_arrived");
        l_oProcess.m_strTimeSigned = l_pResult->GetValue("time_signed");
        l_oProcess.m_strProcessName = l_pResult->GetValue("process_name");
        l_oProcess.m_strProcessCode = l_pResult->GetValue("process_code");
        l_oProcess.m_strDispatchPersonnel = l_pResult->GetValue("dispatch_personnel");
        l_oProcess.m_strDispatchVehicles = l_pResult->GetValue("dispatch_vehicles");
        l_oProcess.m_strDispatchBoats = l_pResult->GetValue("dispatch_boats");
        l_oProcess.m_strState = l_pResult->GetValue("state");
        l_oProcess.m_strCreateTime = l_pResult->GetValue("create_time");
        l_oProcess.m_strUpdateTime = l_pResult->GetValue("update_time");
        l_oProcess.m_strDispatchDeptName = l_pResult->GetValue("dispatch_dept_name");
        l_oProcess.m_strDispatchDeptOrgCode = l_pResult->GetValue("dispatch_dept_org_code");
        l_oProcess.m_strProcessDeptName = l_pResult->GetValue("process_dept_name");
        l_oProcess.m_strProcessDeptOrgCode = l_pResult->GetValue("process_dept_org_code");
        l_oProcess.m_strProcessObjectType = l_pResult->GetValue("process_object_type");
        l_oProcess.m_strProcessObjectName = l_pResult->GetValue("process_object_name");
        l_oProcess.m_strProcessObjectCode = l_pResult->GetValue("process_object_code");
        l_oProcess.m_strBusinessStatus = l_pResult->GetValue("business_status");
        l_oProcess.m_strSeatCode = l_pResult->GetValue("seat_code");
        l_oProcess.m_strCancelTime = l_pResult->GetValue("cancel_time");
        l_oProcess.m_strCancelReason = l_pResult->GetValue("cancel_reason");
        l_oProcess.m_strIsAutoAssignJob = l_pResult->GetValue("is_auto_assign_job","0");
        l_oProcess.m_strCreateUser = l_pResult->GetValue("create_user");
        l_oProcess.m_strUpdateUser = l_pResult->GetValue("update_user");
        l_oProcess.m_strOvertimeState = l_pResult->GetValue("overtime_state");

        l_oProcess.m_strProcessObjectID = l_pResult->GetValue("process_object_id");
        l_oProcess.m_strTransfDeptOrjCode = l_pResult->GetValue("transfers_dept_org_code");
        l_oProcess.m_strIsOver = l_pResult->GetValue("is_over","0");
        l_oProcess.m_strOverRemark = l_pResult->GetValue("over_remark");
        l_oProcess.m_strParentID = l_pResult->GetValue("parent_id");
        l_oProcess.m_strGZLDM = l_pResult->GetValue("flow_code");

        l_oProcess.m_strCentreProcessDeptCode = l_pResult->GetValue("centre_process_dept_code");
        l_oProcess.m_strCentreAlarmDeptCode = l_pResult->GetValue("centre_alarm_dept_code");
        l_oProcess.m_strDispatchDeptDistrictName = l_pResult->GetValue("dispatch_dept_district_name");
        l_oProcess.m_strLinkedDispatchCode = l_pResult->GetValue("linked_dispatch_code");
        l_oProcess.m_strOverTime = l_pResult->GetValue("over_time");
        l_oProcess.m_strFinishedTimeoutState = l_pResult->GetValue("finished_timeout_state");
        l_oProcess.m_strPoliceType = l_pResult->GetValue("police_type");
        l_oProcess.m_strDispatchDeptShortName = l_pResult->GetValue("dispatch_dept_short_name");
        l_oProcess.m_strProcessDeptShortName = l_pResult->GetValue("process_dept_short_name");
        l_oProcess.m_strCreateTeminal = l_pResult->GetValue("createteminal");
        l_oProcess.m_strUpdateTeminal = l_pResult->GetValue("updateteminal");
		if (l_oProcess.m_strState != PROCESS_STATUS_CANCEL)
		{
			p_vecProcess.push_back(l_oProcess);
		}
    }
    return true;
}

bool CBusinessImpl::GetAlarm(std::string l_strID,AlarmData& l_oAlarm,std::string l_strCallRefID)
{
    DataBase::SQLRequest l_oSeleteAlarmSQLReq;
    l_oSeleteAlarmSQLReq.sql_id = "select_icc_t_jjdb";
    if (!l_strID.empty())
    {
        l_oSeleteAlarmSQLReq.param["id"] = l_strID;
        std::string strTime = m_pDateTime->GetAlarmIdTime(l_strID);
        if (strTime != "")
        {
            l_oSeleteAlarmSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
            l_oSeleteAlarmSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
        }
    }
    if (!l_strCallRefID.empty())
    {
        std::string strTime;
        l_oSeleteAlarmSQLReq.param["source_id"] = l_strCallRefID;

        strTime = m_pDateTime->GetCallRefIdTime(l_strCallRefID);
        if (strTime != "")
        {
            l_oSeleteAlarmSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 2 * 86400);
            l_oSeleteAlarmSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 2 * 86400);
        }
    }

    if (!m_pDBConn)
    {
        ICC_LOG_ERROR(m_pLog, "db connect is null!!!");
        return false;
    }
    DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oSeleteAlarmSQLReq, true);
	
    if (!l_pResult->IsValid())
    {
        ICC_LOG_ERROR(m_pLog, "get alarm failed, [%s]", l_pResult->GetErrorMsg().c_str());
        return false;
    }

	ICC_LOG_DEBUG(m_pLog, "get alarm success, [%s]", l_strID.c_str());
    
 
    if (l_pResult->Next())
    {
  //      l_oAlarm.m_strID = l_pResult->GetValue("id");
		//l_oAlarm.m_strMergeID = l_pResult->GetValue("merge_id");
		////l_oAlarm.m_strReceiptSeatno = l_pResult->GetValue("receipt_seatno");
  //      l_oAlarm.m_strTitle = l_pResult->GetValue("title");
  //      l_oAlarm.m_strContent = l_pResult->GetValue("content");
  //      l_oAlarm.m_strTime = l_pResult->GetValue("time");
  //      //l_oAlarm.m_strActualOccurTime = l_pResult->GetValue("actual_occur_time");
  //      l_oAlarm.m_strAddr = l_pResult->GetValue("addr");
  //      l_oAlarm.m_strLongitude = l_pResult->GetValue("longitude");
  //      l_oAlarm.m_strLatitude = l_pResult->GetValue("latitude");
  //      l_oAlarm.m_strState = l_pResult->GetValue("state");
  //      l_oAlarm.m_strLevel = l_pResult->GetValue("level");
  //      l_oAlarm.m_strSourceType = l_pResult->GetValue("source_type");
  //      l_oAlarm.m_strSourceID = l_pResult->GetValue("source_id");
  //      l_oAlarm.m_strHandleType = l_pResult->GetValue("handle_type");
  //      l_oAlarm.m_strFirstType = l_pResult->GetValue("first_type");
  //      l_oAlarm.m_strSecondType = l_pResult->GetValue("second_type");
  //      l_oAlarm.m_strThirdType = l_pResult->GetValue("third_type");
  //      l_oAlarm.m_strFourthType = l_pResult->GetValue("fourth_type");
  //      l_oAlarm.m_strVehicleNo = l_pResult->GetValue("vehicle_no");
  //      l_oAlarm.m_strVehicleType = l_pResult->GetValue("vehicle_type");
  //      l_oAlarm.m_strSymbolCode = l_pResult->GetValue("symbol_code");
  //     // l_oAlarm.m_strSymbolAddr = l_pResult->GetValue("symbol_addr");
  //     // l_oAlarm.m_strFireBuildingType = l_pResult->GetValue("fire_building_type");
  //     // l_oAlarm.m_strEventType = l_pResult->GetValue("event_type");
  //      l_oAlarm.m_strCalledNoType = l_pResult->GetValue("called_no_type");
  //     // l_oAlarm.m_strActualCalledNoType = l_pResult->GetValue("actual_called_no_type");
  //      l_oAlarm.m_strCallerNo = l_pResult->GetValue("caller_no");
  //      l_oAlarm.m_strCallerName = l_pResult->GetValue("caller_name");
  //      l_oAlarm.m_strCallerAddr = l_pResult->GetValue("caller_addr");
  //      l_oAlarm.m_strCallerID = l_pResult->GetValue("caller_id");
  //      l_oAlarm.m_strCallerIDType = l_pResult->GetValue("caller_id_type");
  //      l_oAlarm.m_strCallerGender = l_pResult->GetValue("caller_gender");
  //      //l_oAlarm.m_strCallerAge = l_pResult->GetValue("caller_age");
  //      //l_oAlarm.m_strCallerBirthday = l_pResult->GetValue("caller_birthday");
  //      l_oAlarm.m_strContactNo = l_pResult->GetValue("contact_no");
  //     // l_oAlarm.m_strContactName = l_pResult->GetValue("contact_name");
  //     // l_oAlarm.m_strContactAddr = l_pResult->GetValue("contact_addr");
  //     // l_oAlarm.m_strContactID = l_pResult->GetValue("contact_id");
  //     // l_oAlarm.m_strContactIDType = l_pResult->GetValue("contact_id_type");
  //     // l_oAlarm.m_strContactGender = l_pResult->GetValue("contact_gender");
  //     // l_oAlarm.m_strContactAge = l_pResult->GetValue("contact_age");
  //     // l_oAlarm.m_strContactBirthday = l_pResult->GetValue("contact_birthday");
  //     // l_oAlarm.m_strAdminDeptDistrictCode = l_pResult->GetValue("admin_dept_district_code");
  //      l_oAlarm.m_strAdminDeptCode = l_pResult->GetValue("admin_dept_code");
  //      l_oAlarm.m_strAdminDeptName = l_pResult->GetValue("admin_dept_name");
  //      l_oAlarm.m_strReceiptDeptDistrictCode = l_pResult->GetValue("receipt_dept_district_code");
  //      l_oAlarm.m_strReceiptDeptCode = l_pResult->GetValue("receipt_dept_code");
  //      l_oAlarm.m_strReceiptDeptName = l_pResult->GetValue("receipt_dept_name");
  //      //l_oAlarm.m_strLeaderCode = l_pResult->GetValue("leader_code");
  //      //l_oAlarm.m_strLeaderName = l_pResult->GetValue("leader_name");
  //      l_oAlarm.m_strReceiptCode = l_pResult->GetValue("receipt_code");
  //      l_oAlarm.m_strReceiptName = l_pResult->GetValue("receipt_name");
		////l_oAlarm.m_strDispatchSuggestion = l_pResult->GetValue("dispatch_suggestion");
		//l_oAlarm.m_strIsMerge = l_pResult->GetValue("is_merge");
		//l_oAlarm.m_strCreateUser = l_pResult->GetValue("create_user");
		//l_oAlarm.m_strCreateTime = l_pResult->GetValue("create_time");
		//l_oAlarm.m_strUpdateUser = l_pResult->GetValue("update_user");
		//l_oAlarm.m_strUpdateTime = l_pResult->GetValue("update_time");
  //      //l_oAlarm.m_strCityCode = l_pResult->GetValue("city_code");
		//l_oAlarm.m_strPrivacy = l_pResult->GetValue("is_privacy");
		//l_oAlarm.m_strRemark = l_pResult->GetValue("remark");

        if (!l_oAlarm.ParseAlarmRecord(l_pResult))
        {
            ICC_LOG_ERROR(m_pLog, "Parse record failed.");
        }

    }
    if (!SearchRecordFileID(l_oAlarm))
    {
        ICC_LOG_DEBUG(m_pLog, "Search Record File ID error");
        return false;
    }
    return true;
}

bool CBusinessImpl::GetFeedBackBook(std::string l_strID, std::vector<FeedBackBook>& p_oFeedBackBook)
{
    DataBase::SQLRequest l_SqlRequest;
    l_SqlRequest.sql_id = "select_icc_t_alarm_feedback_book";
    l_SqlRequest.param["guid"] = l_strID;

    DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_SqlRequest, true);
	
    if (!l_result->IsValid())
    {
        ICC_LOG_ERROR(m_pLog, "get feedback failed, [%s]", l_result->GetErrorMsg().c_str());
        return false;
    }

	ICC_LOG_DEBUG(m_pLog, "get feedback success, [%s]", l_strID.c_str());
    while (l_result->Next())
    {
        FeedBackBook l_oFeedBackBook;
        l_oFeedBackBook.m_strGUID = l_result->GetValue("guid");
        l_oFeedBackBook.m_strID = l_result->GetValue("id");
        l_oFeedBackBook.m_strAlarmID = l_result->GetValue("alarm_id");
        l_oFeedBackBook.m_strProcessID = l_result->GetValue("process_id");
        l_oFeedBackBook.m_strResultType = l_result->GetValue("result_type");
        l_oFeedBackBook.m_strResultContent = l_result->GetValue("result_content");
        l_oFeedBackBook.m_strLeaderInstruction = l_result->GetValue("leader_instruction");
        l_oFeedBackBook.m_strState = l_result->GetValue("state");
        l_oFeedBackBook.m_strTimeEdit = l_result->GetValue("time_edit");
        l_oFeedBackBook.m_strTimeSubmit = l_result->GetValue("time_submit");
        l_oFeedBackBook.m_strTimeArrived = l_result->GetValue("time_arrived");
        l_oFeedBackBook.m_strTimeSigned = l_result->GetValue("time_signed");
        l_oFeedBackBook.m_strTimePoliceDispatch = l_result->GetValue("time_police_dispatch");
        l_oFeedBackBook.m_strTimePoliceArrived = l_result->GetValue("time_police_arrived");
        l_oFeedBackBook.m_strActualOccurTime = l_result->GetValue("actual_occur_time");
        l_oFeedBackBook.m_strActualOccurAddr = l_result->GetValue("actual_occur_addr");
        l_oFeedBackBook.m_strFeedbackDeptDistrictCode = l_result->GetValue("feedback_dept_district_code");
        l_oFeedBackBook.m_strFeedbackDeptCode = l_result->GetValue("feedback_dept_code");
        l_oFeedBackBook.m_strFeedbackDeptName = l_result->GetValue("feedback_dept_name");
        l_oFeedBackBook.m_strFeedbackCode = l_result->GetValue("feedback_code");
        l_oFeedBackBook.m_strFeedbackName = l_result->GetValue("feedback_name");
        l_oFeedBackBook.m_strFeedbackLeaderCode = l_result->GetValue("feedback_leader_code");
        l_oFeedBackBook.m_strFeedbackLeaderName = l_result->GetValue("feedback_leader_name");
        l_oFeedBackBook.m_strProcessDeptDistrictCode = l_result->GetValue("process_dept_district_code");
        l_oFeedBackBook.m_strProcessDeptCode = l_result->GetValue("process_dept_code");
        l_oFeedBackBook.m_strProcessDeptName = l_result->GetValue("process_dept_name");
        l_oFeedBackBook.m_strProcessCode = l_result->GetValue("process_code");
        l_oFeedBackBook.m_strProcessName = l_result->GetValue("process_name");
        l_oFeedBackBook.m_strProcessLeaderCode = l_result->GetValue("process_leader_code");
        l_oFeedBackBook.m_strProcessLeaderName = l_result->GetValue("process_leader_name");
        l_oFeedBackBook.m_strDispatchDeptDistrictCode = l_result->GetValue("dispatch_dept_district_code");
        l_oFeedBackBook.m_strDispatchDeptCode = l_result->GetValue("dispatch_dept_code");
        l_oFeedBackBook.m_strDispatchDeptName = l_result->GetValue("dispatch_dept_name");
        l_oFeedBackBook.m_strDispatchCode = l_result->GetValue("dispatch_code");
        l_oFeedBackBook.m_strDispatchName = l_result->GetValue("dispatch_name");
        l_oFeedBackBook.m_strDispatchLeaderCode = l_result->GetValue("dispatch_leader_code");
        l_oFeedBackBook.m_strDispatchLeaderName = l_result->GetValue("dispatch_leader_name");
        l_oFeedBackBook.m_strPersonId = l_result->GetValue("person_id");
        l_oFeedBackBook.m_strPersonIdType = l_result->GetValue("person_id_type");
        l_oFeedBackBook.m_strPersonNationality = l_result->GetValue("person_nationality");
        l_oFeedBackBook.m_strPersonName = l_result->GetValue("person_name");
        l_oFeedBackBook.m_strPersonSlaveId = l_result->GetValue("person_slave_id");
        l_oFeedBackBook.m_strPersonSlaveIdType = l_result->GetValue("person_slave_id_type");
        l_oFeedBackBook.m_strPersonSlaveNationality = l_result->GetValue("person_slave_nationality");
        l_oFeedBackBook.m_strPersonSlaveName = l_result->GetValue("person_slave_name");
        l_oFeedBackBook.m_strAlarmFirstType = l_result->GetValue("alarm_first_type");
        l_oFeedBackBook.m_strAlarmSecondType = l_result->GetValue("alarm_second_type");
        l_oFeedBackBook.m_strAlarmThirdType = l_result->GetValue("alarm_third_type");
        l_oFeedBackBook.m_strAlarmFourthType = l_result->GetValue("alarm_fourth_type");
        l_oFeedBackBook.m_strAlarmAddrDeptName = l_result->GetValue("alarm_addr_dept_name");
        l_oFeedBackBook.m_strAlarmAddrFirstType = l_result->GetValue("alarm_addr_first_type");
        l_oFeedBackBook.m_strAlarmAddrSecondType = l_result->GetValue("alarm_addr_second_type");
        l_oFeedBackBook.m_strAlarmAddrThirdType = l_result->GetValue("alarm_addr_third_type");
        l_oFeedBackBook.m_strAlarmLongitude = l_result->GetValue("alarm_longitude");
        l_oFeedBackBook.m_strAlarmLatitude = l_result->GetValue("alarm_latitude");
        l_oFeedBackBook.m_strAlarmRegionType = l_result->GetValue("alarm_region_type");
        l_oFeedBackBook.m_strAlarmLocationType = l_result->GetValue("alarm_location_type");
        l_oFeedBackBook.m_strPeopleNumCapture = l_result->GetValue("people_num_capture");
        l_oFeedBackBook.m_strPeopleNumRescue = l_result->GetValue("people_num_rescue");
        l_oFeedBackBook.m_strPeopleNumSlightInjury = l_result->GetValue("people_num_slight_injury");
        l_oFeedBackBook.m_strPeopleNumSeriousInjury = l_result->GetValue("people_num_serious_injury");
        l_oFeedBackBook.m_strPeopleNumDeath = l_result->GetValue("people_num_death");
        l_oFeedBackBook.m_strPoliceNumDispatch = l_result->GetValue("police_num_dispatch");
        l_oFeedBackBook.m_strPoliceCarNumDispatch = l_result->GetValue("police_car_num_dispatch");
        l_oFeedBackBook.m_strEconomyLoss = l_result->GetValue("economy_loss");
        l_oFeedBackBook.m_strRetrieveEconomyLoss = l_result->GetValue("retrieve_economy_loss");
        l_oFeedBackBook.m_strFirePutOutTime = l_result->GetValue("fire_put_out_time");
        l_oFeedBackBook.m_strFireBuildingFirstType = l_result->GetValue("fire_building_first_type");
        l_oFeedBackBook.m_strFireBuildingSecondType = l_result->GetValue("fire_building_second_type");
        l_oFeedBackBook.m_strFireBuildingThirdType = l_result->GetValue("fire_building_third_type");
        l_oFeedBackBook.m_strFireSourceType = l_result->GetValue("fire_source_type");
        l_oFeedBackBook.m_strFireRegionType = l_result->GetValue("fire_region_type");
        l_oFeedBackBook.m_strFireCauseFirstType = l_result->GetValue("fire_cause_first_type");
        l_oFeedBackBook.m_strFireCauseSecondType = l_result->GetValue("fire_cause_second_type");
        l_oFeedBackBook.m_strFireCauseThirdType = l_result->GetValue("fire_cause_third_type");
        l_oFeedBackBook.m_strFireArea = l_result->GetValue("fire_area");
        l_oFeedBackBook.m_strTrafficRoadLevel = l_result->GetValue("traffic_road_level");
        l_oFeedBackBook.m_strTrafficAccidentLevel = l_result->GetValue("traffic_accident_level");
        l_oFeedBackBook.m_strTrafficVehicleNo = l_result->GetValue("traffic_vehicle_no");
        l_oFeedBackBook.m_strTrafficVehicleType = l_result->GetValue("traffic_vehicle_type");
        l_oFeedBackBook.m_strTrafficSlaveVehicleNo = l_result->GetValue("traffic_slave_vehicle_no");
        l_oFeedBackBook.m_strTrafficSlaveVehicleType = l_result->GetValue("traffic_slave_vehicle_type");
        l_oFeedBackBook.m_strEventType = l_result->GetValue("event_type");
        l_oFeedBackBook.m_strFlag = l_result->GetValue("flag");
        l_oFeedBackBook.m_strAlarmCalledNoType = l_result->GetValue("alarm_called_no_type");
        l_oFeedBackBook.m_strCreateTime = l_result->GetValue("create_time");
        l_oFeedBackBook.m_strCreateUser = l_result->GetValue("create_user");
        l_oFeedBackBook.m_strUpdateTime = l_result->GetValue("update_time");
        l_oFeedBackBook.m_strUpdateUser = l_result->GetValue("update_uesr");
        p_oFeedBackBook.push_back(l_oFeedBackBook);
    }
    return true;
}

void CBusinessImpl::SyncAlarmInfo(const AlarmData& p_roAlarmSync, std::string l_strType, std::string p_strReleatedID)
{
    std::string l_strGuid = m_pString->CreateGuid();
    PROTOCOL::CAlarmSync l_oAlarmSync;
    l_oAlarmSync.m_oHeader.m_strSystemID = "ICC";
    l_oAlarmSync.m_oHeader.m_strSubsystemID = "ICC-ApplicationServer";
    l_oAlarmSync.m_oHeader.m_strMsgid = l_strGuid;
	l_oAlarmSync.m_oHeader.m_strRelatedID = p_strReleatedID;
    l_oAlarmSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
    l_oAlarmSync.m_oHeader.m_strCmd = "alarm_sync";
    l_oAlarmSync.m_oHeader.m_strRequest = "topic_alarm_sync";
    l_oAlarmSync.m_oHeader.m_strRequestType = "1";
    l_oAlarmSync.m_oHeader.m_strResponse = "";
    l_oAlarmSync.m_oHeader.m_strResponseType = "";

    l_oAlarmSync.m_oBody.m_strSyncType = l_strType;
    l_oAlarmSync.m_oBody.m_oAlarmInfo = p_roAlarmSync;
    /*l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strID = p_roAlarmSync.m_strID;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strMergeID = p_roAlarmSync.m_strMergeID;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strTitle = p_roAlarmSync.m_strTitle;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strContent = p_roAlarmSync.m_strContent;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strTime = p_roAlarmSync.m_strTime;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strActualOccurTime = p_roAlarmSync.m_strActualOccurTime;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strAddr = p_roAlarmSync.m_strAddr;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strLongitude = p_roAlarmSync.m_strLongitude;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strLatitude = p_roAlarmSync.m_strLatitude;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strState = p_roAlarmSync.m_strState.empty() ? "02" : p_roAlarmSync.m_strState;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strLevel = p_roAlarmSync.m_strLevel;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strSourceType = p_roAlarmSync.m_strSourceType;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strSourceID = p_roAlarmSync.m_strSourceID;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strHandleType = p_roAlarmSync.m_strHandleType;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strFirstType = p_roAlarmSync.m_strFirstType;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strSecondType = p_roAlarmSync.m_strSecondType;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strThirdType = p_roAlarmSync.m_strThirdType;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strFourthType = p_roAlarmSync.m_strFourthType;

    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strVehicleNo = p_roAlarmSync.m_strVehicleNo;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strVehicleType = p_roAlarmSync.m_strVehicleType;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strSymbolCode = p_roAlarmSync.m_strSymbolCode;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strSymbolAddr = p_roAlarmSync.m_strSymbolAddr;

    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strFireBuildingType = p_roAlarmSync.m_strFireBuildingType;

    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strEventType = p_roAlarmSync.m_strEventType;


    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strCalledNoType = p_roAlarmSync.m_strCalledNoType;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strActualCalledNoType = p_roAlarmSync.m_strActualCalledNoType;

    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strCallerNo = p_roAlarmSync.m_strCallerNo;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strCallerName = p_roAlarmSync.m_strCallerName;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strCallerAddr = p_roAlarmSync.m_strCallerAddr;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strCallerID = p_roAlarmSync.m_strCallerID;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strCallerIDType = p_roAlarmSync.m_strCallerIDType;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strCallerGender = p_roAlarmSync.m_strCallerGender;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strCallerAge = p_roAlarmSync.m_strCallerAge;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strCallerBirthday = p_roAlarmSync.m_strCallerBirthday;

    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strContactNo = p_roAlarmSync.m_strContactNo;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strContactName = p_roAlarmSync.m_strContactName;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strContactAddr = p_roAlarmSync.m_strContactAddr;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strContactID = p_roAlarmSync.m_strContactID;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strContactIDType = p_roAlarmSync.m_strContactIDType;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strContactGender = p_roAlarmSync.m_strContactGender;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strContactAge = p_roAlarmSync.m_strContactAge;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strContactBirthday = p_roAlarmSync.m_strContactBirthday;

    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strAdminDeptDistrictCode = p_roAlarmSync.m_strAdminDeptDistrictCode;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strAdminDeptCode = p_roAlarmSync.m_strAdminDeptCode;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strAdminDeptName = p_roAlarmSync.m_strAdminDeptName;

    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strReceiptDeptDistrictCode = p_roAlarmSync.m_strReceiptDeptDistrictCode;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strReceiptDeptCode = p_roAlarmSync.m_strReceiptDeptCode;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strReceiptDeptName = p_roAlarmSync.m_strReceiptDeptName;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strLeaderCode = p_roAlarmSync.m_strLeaderCode;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strLeaderName = p_roAlarmSync.m_strLeaderName;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strReceiptCode = p_roAlarmSync.m_strReceiptCode;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strReceiptName = p_roAlarmSync.m_strReceiptName;
	l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strSeatNo = p_roAlarmSync.m_strReceiptSeatno;
	l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strIsMerge = p_roAlarmSync.m_strIsMerge;

    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strDispatchSuggestion = p_roAlarmSync.m_strDispatchSuggestion;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strCreateUser = p_roAlarmSync.m_strCreateUser;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strCreateTime = p_roAlarmSync.m_strCreateTime;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strUpdateUser = p_roAlarmSync.m_strUpdateUser;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strUpdateTime = p_roAlarmSync.m_strUpdateTime;

    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strPrivacy = p_roAlarmSync.m_strPrivacy;
    l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strRemark = p_roAlarmSync.m_strRemark;*/

    JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
    std::string l_strMessage = l_oAlarmSync.ToString(l_pIJson);
    m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
    ICC_LOG_DEBUG(m_pLog, "sync alarm msg[\n%s\n]", l_strMessage.c_str());
}

void CBusinessImpl::SyncProcessInfo(const ProcessData& p_rProcessToSync, std::string l_strType, std::string p_strReleatedID)
{
    PROTOCOL::CAlarmProcessSync l_oAlarmProcessSync;
    l_oAlarmProcessSync.m_oHeader.m_strSystemID = "ICC";
    l_oAlarmProcessSync.m_oHeader.m_strSubsystemID = "ICC-ApplicationServer";
	l_oAlarmProcessSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oAlarmProcessSync.m_oHeader.m_strRelatedID = p_strReleatedID;
    l_oAlarmProcessSync.m_oHeader.m_strCmd = "alarm_process_sync";
    l_oAlarmProcessSync.m_oHeader.m_strRequest = "topic_alarm_sync";
    l_oAlarmProcessSync.m_oHeader.m_strRequestType = "1";//主题
    l_oAlarmProcessSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();

    //l_oAlarmProcessSync.m_oBody.m_ProcessData = p_rProcessToSync;
    l_oAlarmProcessSync.m_oBody.m_strSyncType = l_strType;


    //同步消息
    JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
    std::string l_strMsg(l_oAlarmProcessSync.ToString(l_pIJson));
    m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
    ICC_LOG_DEBUG(m_pLog, "sync alarm process msg[\n%s\n]", l_strMsg.c_str());
}

void CBusinessImpl::SyncFeedBackInfo(const FeedbackData& p_rFeedBackToSync, std::string l_strType, std::string p_strReleatedID)
{
    std::string l_strGuid = m_pString->CreateGuid();

    PROTOCOL::CFeedBackSync l_CFeedbackSync;
    l_CFeedbackSync.m_oHeader.m_strSystemID = "ICC";
    l_CFeedbackSync.m_oHeader.m_strSubsystemID = "ICC-ApplicationServer";
    l_CFeedbackSync.m_oHeader.m_strMsgid = l_strGuid;
	l_CFeedbackSync.m_oHeader.m_strRelatedID = p_strReleatedID;
    l_CFeedbackSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
    l_CFeedbackSync.m_oHeader.m_strCmd = "alarm_feedback_sync";
    l_CFeedbackSync.m_oHeader.m_strRequest = "topic_alarm_sync";
    l_CFeedbackSync.m_oHeader.m_strRequestType = "1";
    l_CFeedbackSync.m_oHeader.m_strResponse = "";
    l_CFeedbackSync.m_oHeader.m_strResponseType = "";
	if (!p_rFeedBackToSync.m_strMsgSource.compare("vcs"))
	{
		l_CFeedbackSync.m_oHeader.m_strCMSProperty = MSG_SOURCE_VCS;
	}
	else
	{
		l_CFeedbackSync.m_oHeader.m_strCMSProperty = MSG_SOURCE;
	}

    l_CFeedbackSync.m_oBody.m_strSyncType = l_strType;
    l_CFeedbackSync.m_oBody.m_strID = p_rFeedBackToSync.m_strID;
    l_CFeedbackSync.m_oBody.m_strAlarmID = p_rFeedBackToSync.m_strAlarmID;
    l_CFeedbackSync.m_oBody.m_strProcessID = p_rFeedBackToSync.m_strProcessID;
    l_CFeedbackSync.m_oBody.m_strResultType = p_rFeedBackToSync.m_strResultType;
    l_CFeedbackSync.m_oBody.m_strResultContent = p_rFeedBackToSync.m_strResultContent;
    l_CFeedbackSync.m_oBody.m_strLeaderInstruction = p_rFeedBackToSync.m_strLeaderInstruction;
    l_CFeedbackSync.m_oBody.m_strState = p_rFeedBackToSync.m_strState;
    l_CFeedbackSync.m_oBody.m_strTimeEdit = p_rFeedBackToSync.m_strTimeEdit;
    l_CFeedbackSync.m_oBody.m_strTimeSubmit = p_rFeedBackToSync.m_strTimeSubmit;
    l_CFeedbackSync.m_oBody.m_strTimeArrived = p_rFeedBackToSync.m_strTimeArrived;
    l_CFeedbackSync.m_oBody.m_strTimeSigned = p_rFeedBackToSync.m_strTimeSigned;
    l_CFeedbackSync.m_oBody.m_strTimePoliceDispatch = p_rFeedBackToSync.m_strTimePoliceDispatch;
    l_CFeedbackSync.m_oBody.m_strTimePoliceArrived = p_rFeedBackToSync.m_strTimePoliceArrived;
    l_CFeedbackSync.m_oBody.m_strActualOccurTime = p_rFeedBackToSync.m_strActualOccurTime;
    l_CFeedbackSync.m_oBody.m_strActualOccurAddr = p_rFeedBackToSync.m_strActualOccurAddr;
    l_CFeedbackSync.m_oBody.m_strFeedbackDeptDistrictCode = p_rFeedBackToSync.m_strFeedbackDeptDistrictCode;
    l_CFeedbackSync.m_oBody.m_strFeedbackDeptCode = p_rFeedBackToSync.m_strFeedbackDeptCode;
    l_CFeedbackSync.m_oBody.m_strFeedbackDeptName = p_rFeedBackToSync.m_strFeedbackDeptName;
    l_CFeedbackSync.m_oBody.m_strFeedbackCode = p_rFeedBackToSync.m_strFeedbackCode;
    l_CFeedbackSync.m_oBody.m_strFeedbackName = p_rFeedBackToSync.m_strFeedbackName;
    l_CFeedbackSync.m_oBody.m_strFeedbackLeaderCode = p_rFeedBackToSync.m_strFeedbackLeaderCode;
    l_CFeedbackSync.m_oBody.m_strFeedbackLeaderName = p_rFeedBackToSync.m_strFeedbackLeaderName;
    l_CFeedbackSync.m_oBody.m_strProcessDeptDistrictCode = p_rFeedBackToSync.m_strProcessDeptDistrictCode;
    l_CFeedbackSync.m_oBody.m_strProcessDeptCode = p_rFeedBackToSync.m_strProcessDeptCode;
    l_CFeedbackSync.m_oBody.m_strProcessDeptName = p_rFeedBackToSync.m_strProcessDeptName;
    l_CFeedbackSync.m_oBody.m_strProcessCode = p_rFeedBackToSync.m_strProcessCode;
    l_CFeedbackSync.m_oBody.m_strProcessName = p_rFeedBackToSync.m_strProcessName;
    l_CFeedbackSync.m_oBody.m_strProcessLeaderCode = p_rFeedBackToSync.m_strProcessLeaderCode;
    l_CFeedbackSync.m_oBody.m_strProcessLeaderName = p_rFeedBackToSync.m_strProcessLeaderName;
    l_CFeedbackSync.m_oBody.m_strDispatchDeptDistrictCode = p_rFeedBackToSync.m_strDispatchDeptDistrictCode;
    l_CFeedbackSync.m_oBody.m_strDispatchDeptCode = p_rFeedBackToSync.m_strDispatchDeptCode;
    l_CFeedbackSync.m_oBody.m_strDispatchDeptName = p_rFeedBackToSync.m_strDispatchDeptName;
    l_CFeedbackSync.m_oBody.m_strDispatchCode = p_rFeedBackToSync.m_strDispatchCode;
    l_CFeedbackSync.m_oBody.m_strDispatchName = p_rFeedBackToSync.m_strDispatchName;
    l_CFeedbackSync.m_oBody.m_strDispatchLeaderCode = p_rFeedBackToSync.m_strDispatchLeaderCode;
    l_CFeedbackSync.m_oBody.m_strDispatchLeaderName = p_rFeedBackToSync.m_strDispatchLeaderName;
    l_CFeedbackSync.m_oBody.m_strPersonId = p_rFeedBackToSync.m_strPersonId;
    l_CFeedbackSync.m_oBody.m_strPersonIdType = p_rFeedBackToSync.m_strPersonIdType;
    l_CFeedbackSync.m_oBody.m_strPersonNationality = p_rFeedBackToSync.m_strPersonNationality;
    l_CFeedbackSync.m_oBody.m_strPersonName = p_rFeedBackToSync.m_strPersonName;
    l_CFeedbackSync.m_oBody.m_strPersonSlaveId = p_rFeedBackToSync.m_strPersonSlaveId;
    l_CFeedbackSync.m_oBody.m_strPersonSlaveIdType = p_rFeedBackToSync.m_strPersonSlaveIdType;
    l_CFeedbackSync.m_oBody.m_strPersonSlaveNationality = p_rFeedBackToSync.m_strPersonSlaveNationality;
    l_CFeedbackSync.m_oBody.m_strPersonSlaveName = p_rFeedBackToSync.m_strPersonSlaveName;
    l_CFeedbackSync.m_oBody.m_strAlarmFirstType = p_rFeedBackToSync.m_strAlarmFirstType;
    l_CFeedbackSync.m_oBody.m_strAlarmSecondType = p_rFeedBackToSync.m_strAlarmSecondType;
    l_CFeedbackSync.m_oBody.m_strAlarmThirdType = p_rFeedBackToSync.m_strAlarmThirdType;
    l_CFeedbackSync.m_oBody.m_strAlarmFourthType = p_rFeedBackToSync.m_strAlarmFourthType;
    l_CFeedbackSync.m_oBody.m_strAlarmAddrDeptName = p_rFeedBackToSync.m_strAlarmAddrDeptName;
    l_CFeedbackSync.m_oBody.m_strAlarmAddrFirstType = p_rFeedBackToSync.m_strAlarmAddrFirstType;
    l_CFeedbackSync.m_oBody.m_strAlarmAddrSecondType = p_rFeedBackToSync.m_strAlarmAddrSecondType;
    l_CFeedbackSync.m_oBody.m_strAlarmAddrThirdType = p_rFeedBackToSync.m_strAlarmAddrThirdType;
    l_CFeedbackSync.m_oBody.m_strAlarmLongitude = p_rFeedBackToSync.m_strAlarmLongitude;
    l_CFeedbackSync.m_oBody.m_strAlarmLatitude = p_rFeedBackToSync.m_strAlarmLatitude;
    l_CFeedbackSync.m_oBody.m_strAlarmRegionType = p_rFeedBackToSync.m_strAlarmRegionType;
    l_CFeedbackSync.m_oBody.m_strAlarmLocationType = p_rFeedBackToSync.m_strAlarmLocationType;
    l_CFeedbackSync.m_oBody.m_strPeopleNumCapture = p_rFeedBackToSync.m_strPeopleNumCapture;
    l_CFeedbackSync.m_oBody.m_strPeopleNumRescue = p_rFeedBackToSync.m_strPeopleNumRescue;
    l_CFeedbackSync.m_oBody.m_strPeopleNumSlightInjury = p_rFeedBackToSync.m_strPeopleNumSlightInjury;
    l_CFeedbackSync.m_oBody.m_strPeopleNumSeriousInjury = p_rFeedBackToSync.m_strPeopleNumSeriousInjury;
    l_CFeedbackSync.m_oBody.m_strPeopleNumDeath = p_rFeedBackToSync.m_strPeopleNumDeath;
    l_CFeedbackSync.m_oBody.m_strPoliceNumDispatch = p_rFeedBackToSync.m_strPoliceNumDispatch;
    l_CFeedbackSync.m_oBody.m_strPoliceCarNumDispatch = p_rFeedBackToSync.m_strPoliceCarNumDispatch;
    l_CFeedbackSync.m_oBody.m_strEconomyLoss = p_rFeedBackToSync.m_strEconomyLoss;
    l_CFeedbackSync.m_oBody.m_strRetrieveEconomyLoss = p_rFeedBackToSync.m_strRetrieveEconomyLoss;
    l_CFeedbackSync.m_oBody.m_strFirePutOutTime = p_rFeedBackToSync.m_strFirePutOutTime;
    l_CFeedbackSync.m_oBody.m_strFireBuildingFirstType = p_rFeedBackToSync.m_strFireBuildingFirstType;
    l_CFeedbackSync.m_oBody.m_strFireBuildingSecondType = p_rFeedBackToSync.m_strFireBuildingSecondType;
    l_CFeedbackSync.m_oBody.m_strFireBuildingThirdType = p_rFeedBackToSync.m_strFireBuildingThirdType;
    l_CFeedbackSync.m_oBody.m_strFireSourceType = p_rFeedBackToSync.m_strFireSourceType;
    l_CFeedbackSync.m_oBody.m_strFireRegionType = p_rFeedBackToSync.m_strFireRegionType;
    l_CFeedbackSync.m_oBody.m_strFireCauseFirstType = p_rFeedBackToSync.m_strFireCauseFirstType;
    l_CFeedbackSync.m_oBody.m_strFireCauseSecondType = p_rFeedBackToSync.m_strFireCauseSecondType;
    l_CFeedbackSync.m_oBody.m_strFireCauseThirdType = p_rFeedBackToSync.m_strFireCauseThirdType;
    l_CFeedbackSync.m_oBody.m_strFireArea = p_rFeedBackToSync.m_strFireArea;
    l_CFeedbackSync.m_oBody.m_strTrafficRoadLevel = p_rFeedBackToSync.m_strTrafficRoadLevel;
    l_CFeedbackSync.m_oBody.m_strTrafficAccidentLevel = p_rFeedBackToSync.m_strTrafficAccidentLevel;
    l_CFeedbackSync.m_oBody.m_strTrafficVehicleNo = p_rFeedBackToSync.m_strTrafficVehicleNo;
    l_CFeedbackSync.m_oBody.m_strTrafficVehicleType = p_rFeedBackToSync.m_strTrafficVehicleType;
    l_CFeedbackSync.m_oBody.m_strTrafficSlaveVehicleNo = p_rFeedBackToSync.m_strTrafficSlaveVehicleNo;
    l_CFeedbackSync.m_oBody.m_strTrafficSlaveVehicleType = p_rFeedBackToSync.m_strTrafficSlaveVehicleType;
    l_CFeedbackSync.m_oBody.m_strEventType = p_rFeedBackToSync.m_strEventType;
    l_CFeedbackSync.m_oBody.m_strAlarmCalledNoType = p_rFeedBackToSync.m_strAlarmCalledNoType;

	l_CFeedbackSync.m_oBody.m_strCreateUser = p_rFeedBackToSync.m_strCreateUser;
	l_CFeedbackSync.m_oBody.m_strCreateTime = p_rFeedBackToSync.m_strCreateTime;
	l_CFeedbackSync.m_oBody.m_strUpdateUser = p_rFeedBackToSync.m_strUpdateUser;
	l_CFeedbackSync.m_oBody.m_strUpdateTime = p_rFeedBackToSync.m_strUpdateTime;


    JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
    std::string l_strMessage = l_CFeedbackSync.ToString(l_pIJson);
    m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send message :[%s]", l_strMessage.c_str());
}

void CBusinessImpl::SyncAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_rAlarmLogToSync)
{
    std::string l_strGuid = m_pString->CreateGuid();

    PROTOCOL::CAlarmLogSync l_oAlarmLogSync;
    l_oAlarmLogSync.m_oHeader.m_strSystemID = "ICC";
    l_oAlarmLogSync.m_oHeader.m_strSubsystemID = "ICC-ApplicationServer";
    l_oAlarmLogSync.m_oHeader.m_strMsgid = l_strGuid;
    l_oAlarmLogSync.m_oHeader.m_strRelatedID = "";
    l_oAlarmLogSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
    l_oAlarmLogSync.m_oHeader.m_strCmd = "alarm_log_sync";
    l_oAlarmLogSync.m_oHeader.m_strRequest = "topic_alarm";
    l_oAlarmLogSync.m_oHeader.m_strRequestType = "1";

    l_oAlarmLogSync.m_oBody.m_strAlarmID = p_rAlarmLogToSync.m_strAlarmID;
    l_oAlarmLogSync.m_oBody.m_strProcessID = p_rAlarmLogToSync.m_strProcessID;
    l_oAlarmLogSync.m_oBody.m_strFeedbackID = p_rAlarmLogToSync.m_strFeedbackID;
    l_oAlarmLogSync.m_oBody.m_strSeatNo = p_rAlarmLogToSync.m_strSeatNo;
    l_oAlarmLogSync.m_oBody.m_strOperate = p_rAlarmLogToSync.m_strOperate;
    l_oAlarmLogSync.m_oBody.m_strOperateContent = p_rAlarmLogToSync.m_strOperateContent;
    l_oAlarmLogSync.m_oBody.m_strFromType = p_rAlarmLogToSync.m_strFromType;
    l_oAlarmLogSync.m_oBody.m_strFromObject = p_rAlarmLogToSync.m_strFromObject;
    l_oAlarmLogSync.m_oBody.m_strFromObjectName = p_rAlarmLogToSync.m_strFromObjectName;
    l_oAlarmLogSync.m_oBody.m_strFromObjectOrgName = p_rAlarmLogToSync.m_strFromObjectOrgName;
    l_oAlarmLogSync.m_oBody.m_strFromObjectOrgCode = p_rAlarmLogToSync.m_strFromObjectOrgCode;
    l_oAlarmLogSync.m_oBody.m_strToType = p_rAlarmLogToSync.m_strToType;
    l_oAlarmLogSync.m_oBody.m_strToObject = p_rAlarmLogToSync.m_strToObject;
    l_oAlarmLogSync.m_oBody.m_strToObjectName = p_rAlarmLogToSync.m_strToObjectName;
    l_oAlarmLogSync.m_oBody.m_strToObjectOrgName = p_rAlarmLogToSync.m_strToObjectOrgName;
    l_oAlarmLogSync.m_oBody.m_strToObjectOrgCode = p_rAlarmLogToSync.m_strToObjectOrgCode;
    l_oAlarmLogSync.m_oBody.m_strCreateUser = p_rAlarmLogToSync.m_strCreateUser;
    l_oAlarmLogSync.m_oBody.m_strCreateTime = p_rAlarmLogToSync.m_strCreateTime;
    l_oAlarmLogSync.m_oBody.m_strDeptOrgCode = p_rAlarmLogToSync.m_strDeptOrgCode;
    l_oAlarmLogSync.m_oBody.m_strSourceName = p_rAlarmLogToSync.m_strSourceName;
    l_oAlarmLogSync.m_oBody.m_strOperateAttachDesc = p_rAlarmLogToSync.m_strOperateAttachDesc;
    l_oAlarmLogSync.m_oBody.m_strReceivedTime = p_rAlarmLogToSync.m_strReceivedTime;
    JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
    std::string l_strMessage = l_oAlarmLogSync.ToString(l_pIJson, ICCGetIJsonFactory()->CreateJson());
    m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
    ICC_LOG_DEBUG(m_pLog, " alarm log sync:[%s]", l_strMessage.c_str());
}

bool CBusinessImpl::SearchRecordFileID(AlarmData& l_oAlarm)
{
    DataBase::SQLRequest l_oSeleteRecordFileIDSQLReq;
    l_oSeleteRecordFileIDSQLReq.sql_id = "select_icc_t_callevent_for_record";
    std::string strSourceID= l_oAlarm.m_strSourceID;
    if (!strSourceID.empty())
    {
        l_oSeleteRecordFileIDSQLReq.param["callref_id"] = strSourceID;
       std::string strCallRefIDTime= m_pDateTime->GetCallRefIdTime(l_oAlarm.m_strSourceID);
        if (!strCallRefIDTime.empty())
        {
            l_oSeleteRecordFileIDSQLReq.param["create_time_begin"] = m_pDateTime->GetFrontTime(strCallRefIDTime, 2 * 86400);
            l_oSeleteRecordFileIDSQLReq.param["create_time_end"] = m_pDateTime->GetAfterTime(strCallRefIDTime, 2 * 86400);
        }
        DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oSeleteRecordFileIDSQLReq, true);
        ICC_LOG_DEBUG(m_pLog, "exec sql=%s", l_pResult->GetSQL().c_str());

        if (!l_pResult->IsValid())
        {
            ICC_LOG_ERROR(m_pLog, "get record failed, [%s]", l_pResult->GetErrorMsg().c_str());
            return false;
        }
        if (l_pResult->Next())
        {
            l_oAlarm.m_strRecordFileID = l_pResult->GetValue("record_file_id");
        }
    }
    return true;
}

bool CBusinessImpl::_GetStaffInfo(const std::string& strStaffCode, Data::CStaffInfo& l_oStaffInfo)
{
    std::string strStaffInfo;
    if (m_strCodeMode == STAFF_ID_NO)
    {
        if (!m_pRedisClient->HGet("StaffIdInfo", strStaffCode, strStaffInfo))
        {
            ICC_LOG_DEBUG(m_pLog, "Hget StaffIdInfo failed,staff_id_no:[%s]", strStaffCode.c_str());
            return false;
        }
    }
    else if (m_strCodeMode == STAFF_CODE)
    {
        if (!m_pRedisClient->HGet("StaffInfoMap", strStaffCode, strStaffInfo))
        {
            ICC_LOG_DEBUG(m_pLog, "Hget StaffInfoMap failed,staff_code:[%s]", strStaffCode.c_str());
            return false;
        }
    }
    if (!l_oStaffInfo.Parse(strStaffInfo, m_pJsonFty->CreateJson()))
    {
        ICC_LOG_DEBUG(m_pLog, "parse staff info failed!!!");
        return false;
    }
    return true;
}

std::string	CBusinessImpl::_GetPoliceTypeName(const std::string& strStaffType, const std::string& strStaffName)
{
    std::string strEndStaffName;
    if (strStaffType == "JZLX101")
    {
        strEndStaffName = m_pString->Format("%s%s", m_strAssistantPolice.c_str(), strStaffName.c_str());
    }
    else
    {
        strEndStaffName = m_pString->Format("%s%s", m_strPolice.c_str(), strStaffName.c_str());
    }
    return strEndStaffName;
}