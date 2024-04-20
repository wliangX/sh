#include "Boost.h"
#include "BusinessImpl.h"

#define TIMER_CMD_NAME "mprs_timer"
#define TIMER_CHECK_HEART "check_heart_timer"
#define CLIENT_REGISTER_INFO "ClientRegisterInfo"
const unsigned long THREADID_REBINDUDP = 10011;
const unsigned long THREADID_BINDUDPSUCCESS = 10012;

CBusinessImpl::CBusinessImpl()
{
	m_SendTimes = 3;        //发送次数
	m_IntervalTime = 3;     //定时器间隔
	m_UDPPort = 0;			//UDP服务端口
}

void CBusinessImpl::OnInit()
{	
	m_bUDPIsStart = false;

	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pStaticConfig = ICCGetIConfigFactory()->CreateStaticConfig();
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(GATEWAY_MRPS_OBSERVER_CENTER);
	m_pLockFty = ICCGetILockFactory();
	m_pJsonFty = ICCGetIJsonFactory();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pString = ICCGetIStringFactory()->CreateString();
	//m_pConsumer = ICCGetAmqClient()->CreateAsyncConsumer();
	m_pXmlFty = ICCGetIXmlFactory();
	m_ptimerMgr = ICCGetITimerFactory()->CreateTimerManager();	
	m_pHelpTool = ICCGetHelpToolFactory()->CreateHelpTool();
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
}

void CBusinessImpl::OnStart()
{	
	std::string l_strIsUsing = m_pConfig->GetValue("ICC/Plugin/MRPS/IsUsing", "0");
	if (l_strIsUsing != "1")
	{
		ICC_LOG_DEBUG(m_pLog, "there is no need to load the plugin");
		return;
	}

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "acd_call_state_sync", OnCtiCallInfoNotice);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "device_state_sync", OnCtiDeviceInfoNotice);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "call_over_sync", OnCtiCallOverSyncNotice);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "callRefId_sync_Mrcc", OnCtiCallRefIdSyncNotice);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_sync", OnCtiAlarmSyncNotice);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "check_user_respond", OnAuthResponse);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TIMER_CMD_NAME, OnTimer);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TIMER_CHECK_HEART, CheckClientHeart);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "sync_server_lock", OnNotifiDoubleServerSync);

	m_SendTimes = m_pString->ToUInt(m_pConfig->GetValue("ICC/Plugin/MRPS/SendTimes", STRING_EMPTY));
	m_IntervalTime = m_pString->ToUInt(m_pConfig->GetValue("ICC/Plugin/MRPS/TimeInterval", STRING_EMPTY));
	m_UDPPort = m_pString->ToUInt(m_pConfig->GetValue("ICC/Plugin/MRPS/UDPPort", "11111"));
	m_strServiceID = m_pStaticConfig->GetValue("ICC/Component/AmqClient/ClientID", "");
	m_CheckHeart = m_pString->ToUInt(m_pConfig->GetValue("ICC/Plugin/MRPS/CheckClientHeart", "45"));
	ICC_LOG_DEBUG(m_pLog, "MRPS m_CheckHeart start :[%d]", m_CheckHeart);
	int iFailedNum = m_pString->ToUInt(m_pConfig->GetValue("ICC/Plugin/MRPS/RebindSendFailedCount", "10"));
	bool bIsRebind = m_pConfig->GetValue("ICC/Plugin/MRPS/IsRebindSendFailed", "0") == "1" ? true : false;

	ICC_LOG_DEBUG(m_pLog, "RebindSendFailedCount:[%d], IsRebindSendFailed:[%s]", iFailedNum, bIsRebind ? "true" : "false");

	//m_pMessageStruct = boost::make_shared<CMessageStruct>(m_pLog, m_pXmlFty, m_pDateTime, m_pJsonFty, m_pString);
	m_UdpHelper = boost::make_shared<CUdpHelper>(m_pLog, m_pHelpTool);
	m_UdpHelper->SetDateTimePtr(m_pDateTime);
	m_UdpHelper->SetStringUtilPtr(m_pString);
	m_UdpHelper->SetXmlFactoryPtr(m_pXmlFty);
	m_UdpHelper->SetJsonFactoryPtr(m_pJsonFty);
	m_UdpHelper->SetObserverCenterPtr(m_pObserverCenter);
	m_UdpHelper->SetLockFtyPtr(m_pLockFty);
	m_UdpHelper->SetSendTimes(m_SendTimes, m_IntervalTime);
	m_UdpHelper->SetBusinessImp(this);
	m_UdpHelper->SetRebindFailedFlag(bIsRebind);
	m_UdpHelper->SetFailedNum(iFailedNum);
	m_UdpHelper->OnStart();

	// 接收udp消息
	m_ptimerMgr->AddTimer(TIMER_CMD_NAME, 1, 1);
	m_ptimerMgr->AddTimer(TIMER_CHECK_HEART, m_CheckHeart, 1);

	m_sendThread.AddMessage(this, THREADID_REBINDUDP);

	//m_pUdp->Bind(m_UDPPort, m_UdpHelper);

	ICC_LOG_DEBUG(m_pLog, "%s plugin started.", MODULE_NAME);
}

void CBusinessImpl::OnStop()
{
	if (m_pUdp)
	{
		m_pUdp->Close();
	}
	
	m_UdpHelper->OnStop();

	ICC_LOG_DEBUG(m_pLog, "%s plugin stop.", MODULE_NAME);
}

void CBusinessImpl::OnDestroy()
{

}

void CBusinessImpl::OnNotifiDoubleServerSync(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	if (p_pNotifiRequest)
	{
		std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
		ICC_LOG_DEBUG(m_pLog, "Server Role Sync: %s", l_strRequestMsg.c_str());

		PROTOCOL::CSyncServerLock l_oSyncObj;
		if (!l_oSyncObj.ParseString(l_strRequestMsg, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "l_strRequestMsg parse json failed");

			return;
		}

		std::string strServerId(m_pConfig->ServerFlag());
		if (l_oSyncObj.m_oBody.m_strServiceID != m_strServiceID)
		{
			return;
		}
		ICC_LOG_DEBUG(m_pLog, "Server Role Sync,Config ServerFalg:%s, clientid:%s", strServerId.c_str(), l_oSyncObj.m_oBody.m_strClientID.c_str());
		if (l_oSyncObj.m_oBody.m_strServiceState.compare("master") == 0)
		{
			if (l_oSyncObj.m_oBody.m_strClientID == strServerId)
			{
				m_UdpHelper->SetMasterSlaveFlag(true);				
			}
			else
			{
				m_UdpHelper->SetMasterSlaveFlag(false);				
			}
		}
		else
		{
			if (l_oSyncObj.m_oBody.m_strClientID == strServerId)
			{
				m_UdpHelper->SetMasterSlaveFlag(false);				
			}
		}
	}
}

void CBusinessImpl::OnCtiCallInfoNotice(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CCallStateEvent request;
	std::string l_strRecvMsg(p_pNotifiRequest->GetMessages());
	if(!request.ParseString(l_strRecvMsg, l_pJson))
	{
		ICC_LOG_ERROR(m_pLog, "[OnCtiCallInfoNotice]Parse request error.[%s]", l_strRecvMsg.c_str());
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "recveice massage[%s].", l_strRecvMsg.c_str());

	if(request.m_oBody.m_strState.compare(CTI_ASSIGN) != 0 || request.m_oBody.m_strAgent.empty())
	{//--By z13060 2018/05/31 14:52 其他状态未分配分机，交换机不录音
		return;
	}

	std::string l_strKey = request.m_oBody.m_strCallRefId + request.m_oBody.m_strAgent;
	CallInfo& l_CallInfo = m_CallInfoMap[l_strKey];
	l_CallInfo.realcallrefid = request.m_oBody.m_strCSTACallRefId;
	l_CallInfo.device = request.m_oBody.m_strAgent;
	l_CallInfo.callrefid = request.m_oBody.m_strCallRefId;
	l_CallInfo.caller = request.m_oBody.m_strCallerId;
	l_CallInfo.called = request.m_oBody.m_strCalledId;
	ICC_LOG_DEBUG(m_pLog, "Add call info, key[%s], realcallrefid[%s], device[%s], callrefid[%s], caller[%s], called[%s].",
		l_strKey.c_str(), l_CallInfo.realcallrefid.c_str(), l_CallInfo.device.c_str(), l_CallInfo.callrefid.c_str(), l_CallInfo.caller.c_str(), l_CallInfo.called.c_str());

	CheckCallInfoMapLength();
}

void CBusinessImpl::OnCtiDeviceInfoNotice(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CDeviceStateEvent request;
	std::string l_strRecvMsg(p_pNotifiRequest->GetMessages());
	if(!request.ParseString(l_strRecvMsg, l_pJson))
	{
		ICC_LOG_ERROR(m_pLog, "[OnCtiDeviceInfoNotice]Parse request error.[%s]", l_strRecvMsg.c_str());
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "recveice massage[%s].", l_strRecvMsg.c_str());

	if (request.m_oBody.m_strDeviceType.compare("ipphone") != 0
		|| (request.m_oBody.m_strState.compare(CTI_RINGSTATE) != 0
		&& request.m_oBody.m_strState.compare(CTI_RINGBACKSTATE) != 0
		&& request.m_oBody.m_strState.compare(CTI_DIALSTATE) != 0
		&& request.m_oBody.m_strState.compare(CTI_HANGUPSTATE) != 0
		&& request.m_oBody.m_strState.compare(CTI_WAITHANGUPSTATE) != 0
		&& request.m_oBody.m_strState.compare(CTI_TALKSTATE) != 0))
	{//不处理非话机的消息，仅处理上面状态的消息。
		return;
	}

	if(request.m_oBody.m_strDevice.empty())
	{//--By z13060 2018/05/31 14:52 其他状态未分配分机，交换机不录音
		request.m_oBody.m_strDevice = (request.m_oBody.m_strCallDirection.compare("in") == 0) ? request.m_oBody.m_strCalledId : request.m_oBody.m_strCallerId;
	}
	
	std::string l_key = request.m_oBody.m_strCallRefId + request.m_oBody.m_strDevice;
	bool b_TalkState = false;
	auto l_it = m_CallInfoMap.find(l_key);
	if (l_it == m_CallInfoMap.end() && request.m_oBody.m_strState.compare(CTI_TALKSTATE) == 0) {
		b_TalkState = true;
	}
	CallInfo& l_CallInfo = m_CallInfoMap[l_key]; 
	if(request.m_oBody.m_strState.compare(CTI_HANGUPSTATE) != 0
		&& request.m_oBody.m_strState.compare(CTI_WAITHANGUPSTATE) != 0
		&& request.m_oBody.m_strState.compare(CTI_FREESTATE) != 0)
	{
		std::string l_strCalledId = request.m_oBody.m_strCalledId;
		if (l_strCalledId.empty())
		{
			// 添加极速110 会议成员，被叫号码可能为空
			l_strCalledId = request.m_oBody.m_strOriginalCalledId;
		}

		if (!request.m_oBody.m_strTime.empty())
		{
			l_CallInfo.callstart = m_pString->ToUInt64(request.m_oBody.m_strTime);
		}
		else
		{
			l_CallInfo.callstart = m_pString->ToUInt64(m_pDateTime->CurrentDateTimeStr());
		}
		l_CallInfo.realcallrefid = request.m_oBody.m_strCSTACallRefId;
		l_CallInfo.device = request.m_oBody.m_strDevice;
		l_CallInfo.callrefid = request.m_oBody.m_strCallRefId;
		l_CallInfo.caller = request.m_oBody.m_strCallerId;
		l_CallInfo.called = l_strCalledId;
		ICC_LOG_DEBUG(m_pLog, "Add call info, key[%s], realcallrefid[%s], device[%s], callrefid[%s], caller[%s], called[%s].",
			l_key.c_str(), l_CallInfo.realcallrefid.c_str(), l_CallInfo.device.c_str(), l_CallInfo.callrefid.c_str(), l_CallInfo.caller.c_str(), l_strCalledId.c_str());

		CheckCallInfoMapLength();
	}

	unsigned int l_phonetype = 1;	
	unsigned int l_type = (request.m_oBody.m_strDevice.compare(l_CallInfo.caller) == 0) ? 1 : 2;	// TODO ZengW 2018/05/29 18:16 暂时不处理中途加入，中途退出的情况。

	/*freestate（空闲态）ringstate（振铃态）ringbackstate（回铃态）dialstate（拨号态）talkstate（通话态）
	conferencestate（会议态）holdstate（保留态）hangupstate（挂机态）waithangupstate（等待挂机态）*/
	if(request.m_oBody.m_strState.compare(CTI_RINGSTATE) == 0 
		|| request.m_oBody.m_strState.compare(CTI_RINGBACKSTATE) == 0
		|| request.m_oBody.m_strState.compare(CTI_DIALSTATE) == 0
		|| (request.m_oBody.m_strState.compare(CTI_TALKSTATE) == 0 && b_TalkState))
	{
		l_CallInfo.callstart = m_UdpHelper->UDPCurrentTime();

		std::string l_guid = m_pString->CreateGuid();
		std::string l_str = m_pString->Format(CTI_CALLMEMBER_NOTICE, l_guid.data(),
			l_CallInfo.callrefid.data(), l_CallInfo.realcallrefid.data(), request.m_oBody.m_strDevice.data(), l_phonetype, l_type, m_UdpHelper->UDPCurrentTime());
		m_UdpHelper->NoticeXml(l_guid, l_str);
		//m_UdpHelper->SendCallMember(l_it->second, l_phone, l_phonetype, l_type);
	}
	else if(request.m_oBody.m_strState.compare(CTI_HANGUPSTATE) == 0 
		|| request.m_oBody.m_strState.compare(CTI_WAITHANGUPSTATE) == 0
		|| request.m_oBody.m_strState.compare(CTI_HOLDSTATE) == 0)
	{
		l_CallInfo.callstop = m_UdpHelper->UDPCurrentTime();

		std::string l_strCallRefId = l_CallInfo.callrefid;
		std::string l_strRealCallRefId = l_CallInfo.realcallrefid;
		std::string l_strCaller = l_CallInfo.caller;
		std::string l_strCalled = l_CallInfo.called;
		uint64 l_unCallStart = l_CallInfo.callstart;
		uint64 l_unCallStop = l_CallInfo.callstop;

		for(auto l_it = m_CallInfoMap.begin(); l_it != m_CallInfoMap.end();)
		{//特殊处理的情况，会议转移代答等
			// ICC_LOG_DEBUG(m_pLog, "m_CallInfoMap , key[%s].[%s-%s-%s]", l_it->first.c_str(), l_it->second.callrefid.c_str(), l_it->second.device.c_str(), l_it->second.realcallrefid.c_str());
			if(l_it->second.callrefid.compare(request.m_oBody.m_strCallRefId) == 0 && l_it->second.realcallrefid.compare(request.m_oBody.m_strCSTACallRefId) != 0
				&& l_it->second.device.compare(request.m_oBody.m_strDevice) == 0)
			{//处理话务ID相同，但是设备话务编码不同的情况。,保持与交换机逻辑一致,交换机逻辑为：仅保留后一个编码

				ICC_LOG_ERROR(m_pLog, "call info merge old[%s],new[%s].", l_it->first.data(), l_key.data());
				l_CallInfo.caller = l_it->second.caller;
				l_CallInfo.called = l_it->second.called;
				l_CallInfo.callstart = l_it->second.callstart;

				ICC_LOG_DEBUG(m_pLog, "CSTACallRefId changed, delete call info, key[%s].", l_it->first.c_str());
				l_it = m_CallInfoMap.erase(l_it);
			}
			else
			{
				l_it++;
			}
		}
		if (l_strCaller.empty()) {
			l_strCaller = request.m_oBody.m_strCallerId;
		}
		if (l_strCalled.empty()) {
			l_strCalled = request.m_oBody.m_strCalledId;
		}
		if (l_strCallRefId.empty()) {
			l_strCallRefId = request.m_oBody.m_strCallRefId;
		}
		if (l_strRealCallRefId.empty()) {
			l_strRealCallRefId = request.m_oBody.m_strCSTACallRefId;
		}
		if (request.m_oBody.m_strState.compare(CTI_HANGUPSTATE) == 0 && !l_strCaller.empty() && !l_strCalled.empty()) {
			calrCalledNum(request.m_oBody.m_strDevice, l_strCaller, l_strCalled);
		}
		std::string l_guid = m_pString->CreateGuid();
		/*std::string l_str = m_pString->Format(CTI_CALLINFO_NOTICE, l_guid.data(),
			l_CallInfo.callrefid.data(), l_CallInfo.realcallrefid.data(), request.m_oBody.m_strDevice.data(),
			l_CallInfo.caller.data(), l_CallInfo.called.data(), l_CallInfo.callstart, l_CallInfo.callstop, m_UdpHelper->UDPCurrentTime());*/
		std::string l_str = m_pString->Format(CTI_CALLINFO_NOTICE, l_guid.c_str(),
			l_strCallRefId.c_str(), l_strRealCallRefId.c_str(), request.m_oBody.m_strDevice.c_str(),
			l_strCaller.c_str(), l_strCalled.c_str(), l_unCallStart, l_unCallStop, m_UdpHelper->UDPCurrentTime());
		m_UdpHelper->NoticeXml(l_guid, l_str);
		//m_UdpHelper->SendCallFinish(l_it->second, l_phone);
		//m_CallInfoMap.erase(l_it); By z13060 2018/05/31 14:26 需要继续保存在内存中，因为内部话机呼叫时会受到不同话机的挂机时间，在定时器中删除。

		m_CallInfoMap.erase(l_key);
		ICC_LOG_DEBUG(m_pLog, "Device hangup, delete call info, key[%s].", l_key.c_str());
	}
	
 }

 void CBusinessImpl::calrCalledNum(const std::string& p_strPhone, std::string& p_strCallerId, std::string& p_strCalledId)
 {
	 if (p_strPhone.compare(p_strCallerId) != 0 && p_strPhone.compare(p_strCalledId) != 0) {
		 if (m_pRedisClient->HExists(CLIENT_REGISTER_INFO, p_strCalledId)) {
			 p_strCalledId = p_strPhone;
		 } else if (m_pRedisClient->HExists(CLIENT_REGISTER_INFO, p_strCallerId)) {
			 p_strCalledId = p_strPhone;
		 }
		 else {
			 ICC_LOG_DEBUG(m_pLog, "Device hangup,  call info, num [%s].", p_strPhone.c_str());
		 }
	 }
 }

void CBusinessImpl::OnCtiCallOverSyncNotice(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CCallOverEvent request;
	std::string l_strRecvMsg(p_pNotifiRequest->GetMessages());
	if(!request.ParseString(l_strRecvMsg, l_pJson))
	{
		ICC_LOG_ERROR(m_pLog, "[OnCtiCallOverSyncNotice]Parse request error.[%s]", l_strRecvMsg.c_str());
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "recveice massage[%s].", l_strRecvMsg.c_str());
	
	std::string l_strKey = request.m_oBody.m_strCallRefId + request.m_oBody.m_strCalledId;
	m_CallInfoMap.erase(l_strKey);
	ICC_LOG_DEBUG(m_pLog, "Call over, delete call info, key[%s].", l_strKey.c_str());
}

void CBusinessImpl::OnCtiCallRefIdSyncNotice(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CCallMrccEvent request;
	std::string l_strRecvMsg(p_pNotifiRequest->GetMessages());
	if (!request.ParseString(l_strRecvMsg, l_pJson))
	{
		ICC_LOG_ERROR(m_pLog, "[OnCtiCallOverSyncNotice]Parse request error.[%s]", l_strRecvMsg.c_str());
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "recveice massage[%s].", l_strRecvMsg.c_str());

	std::string l_guid = m_pString->CreateGuid();

	std::string l_str = m_pString->Format(CTI_CALLREFID_NOTICE, l_guid.data(),
		request.m_oBody.m_strCallRefId.data(), request.m_oBody.m_strOldCallRefId.data(), m_UdpHelper->UDPCurrentTime());
	m_UdpHelper->NoticeXml(l_guid, l_str);

}

void CBusinessImpl::OnCtiAlarmSyncNotice(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	PROTOCOL::CAlarmSync request;
	std::string l_strRecvMsg(p_pNotifiRequest->GetMessages());
	if(!request.ParseString(l_strRecvMsg, l_pJson))
	{
		ICC_LOG_ERROR(m_pLog, "[OnCtiAlarmSyncNotice]Parse request error.[%s]", l_strRecvMsg.c_str());
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "recveice massage[%s].", l_strRecvMsg.c_str());

	if(request.m_oBody.m_oAlarmInfo.m_strSourceType.compare(TELEPHONECALL) == 0)
	{// 电话报警
		std::string l_strCallRefId = request.m_oBody.m_oAlarmInfo.m_strSourceID;
		if(!l_strCallRefId.empty())
		{
			std::vector<std::string> l_strRealCallrefVec;
			//同一话务ID在m_CallInfoMap中可能存在多条，需要同时更新
			for(auto l_it = m_CallInfoMap.begin(); l_it != m_CallInfoMap.end(); l_it++)
			{
				if(l_it->second.callrefid.compare(l_strCallRefId) == 0)
				{
					if(!l_it->second.alarmid.empty())
					{
						ICC_LOG_DEBUG(m_pLog, "Call Already Send, call[%s],case[%s]", l_strCallRefId.data(), request.m_oBody.m_oAlarmInfo.m_strID.data());
						return;
					}

					l_it->second.alarmid = request.m_oBody.m_oAlarmInfo.m_strID;
					l_strRealCallrefVec.push_back(l_it->second.realcallrefid);
				}
			}
			if(!l_strRealCallrefVec.empty())
			{// ZengW 2018/05/29 13:30 发送案件信息到MRPS系统,默认发送第一个RealCallrefID，有需要再调整
				std::string l_guid = m_pString->CreateGuid();
				std::string l_str = m_pString->Format(CTI_CASEINFO_NOTICE, l_guid.data(),
					request.m_oBody.m_oAlarmInfo.m_strID.data(), l_strCallRefId.data(), l_strRealCallrefVec.begin()->data(), m_UdpHelper->UDPCurrentTime());
				m_UdpHelper->NoticeXml(l_guid, l_str);
				//m_UdpHelper->SendCaseInfo(l_strCaseId, l_strCallRefId, *l_strRealCallrefVec.begin());
			}
		}
	}
}

void CBusinessImpl::OnTimer(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	m_UdpHelper->OnTimer();
}

void CBusinessImpl::CheckClientHeart(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	ICC_LOG_DEBUG(m_pLog, "CheckClientHeart m_CheckHeart start :[%d]", m_CheckHeart);
	m_UdpHelper->CheckClientHeart();
}

void CBusinessImpl::OnAuthResponse(ObserverPattern::INotificationPtr p_pAuthRespond)
{
	std::string l_strMsg = p_pAuthRespond->GetMessages();
	JsonParser::IJsonPtr l_pJason = m_pJsonFty->CreateJson();

	if (!l_pJason->LoadJson(l_strMsg))
	{
		ICC_LOG_ERROR(m_pLog, "load json object failed.");
		return;
	}
	ICC_LOG_DEBUG(m_pLog, "Handling Auth Response, %s", l_strMsg.data());

	//0：表示成功，1：表示失败
	std::string l_strRes = l_pJason->GetNodeValue("/body/result", STRING_EMPTY);
	if(!l_strRes.empty())
	{
		m_UdpHelper->SendLoginResult(m_pString->ToInt(l_strRes));
	}
	else
	{//9 为未知错误
		m_UdpHelper->SendLoginResult(LOGIN_RESULT_UNKNOW);
	}
}

bool CBusinessImpl::CheckCallInfoMapLength()
{
	if(m_CallInfoMap.size() > 240)
	{//认为系统同时呼叫路数不超过240路
		uint64 l_time = m_UdpHelper->UDPCurrentTime();
		std::string l_delid;
		for(auto l_it1 = m_CallInfoMap.begin(); l_it1 != m_CallInfoMap.end(); l_it1++)
		{
			if(l_it1->second.callstart < l_time)
			{//清除通话开始时间最早的话务
				l_time = l_it1->second.callstart;
				l_delid = l_it1->first;
			}
		}
		m_CallInfoMap.erase(l_delid);
		ICC_LOG_DEBUG(m_pLog, "m_CallInfoMap size > 240, delete key[%s].", l_delid.data());
		return true;
	}
	return false;
}

void CBusinessImpl::OnReport(int iCode, const std::string& strMessage)
{
	if (iCode == Udp::UDP_RESULT_SUCCESS)
	{		
		ICC_LOG_DEBUG(m_pLog, "bind udp success!!port:[%d]", m_UDPPort);
		m_UdpHelper->SetUdpPtr(m_pUdp);
		m_bUDPIsStart = true;

		//m_sendThread.AddMessage(this, THREADID_BINDUDPSUCCESS);
	}
	else
	{
		m_bUDPIsStart = false;
		m_UdpHelper->SetUdpPtr(nullptr);
		m_sendThread.AddMessage(this, THREADID_REBINDUDP);
	}
}

void CBusinessImpl::_BindUdp()
{
	ICC_LOG_DEBUG(m_pLog, "will bind udp,udpport:%d", m_UDPPort);	
	
	if (m_pUdp)
	{
		m_pUdp->Close();	
		m_pUdp = nullptr;
	}		

	m_pUdp = ICCGetIUdpFactory()->CreateUdp();
	
	m_pHelpTool->Sleep(2000);
	m_pUdp->Bind(m_UDPPort, m_UdpHelper);		

	ICC_LOG_DEBUG(m_pLog, "bind udp complete,udpport,udpport:%d", m_UDPPort);	
}

int iCount = 0;

void CBusinessImpl::ProcMessage(CommonThread_data msg_data)
{
	switch (msg_data.msg_id)
	{
	case THREADID_REBINDUDP:
	{
		try
		{			
			_BindUdp();
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "%s", "unknown exception!");
		}
	}
	break;	
	case THREADID_BINDUDPSUCCESS:
	{
		try
		{
			if (iCount < 5)
			{
				m_pHelpTool->Sleep(30000);
				_BindUdp();
				iCount++;
			}			
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "%s", "unknown exception!");
		}
	}
	break;
	default:
		break;
	}
}