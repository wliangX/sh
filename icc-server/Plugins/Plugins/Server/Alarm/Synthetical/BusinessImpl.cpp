#include "Boost.h"
#include "BusinessImpl.h"

const unsigned long THREADID_RECEIVECOMMONNOTIFY = 10000;
const unsigned long THREADID_RECEIVEALARMNOTIFY = 10001;
const unsigned long ThreadId_Violation = 10002;

const unsigned long long TIMOUT_INTERVAL = 300 * 1000;
const std::string DEFAULT_DATETIME_STRING_FORMAT = "%04d%02d%02d%02d%02d%02d%06d";
#define PARAM_INFO	"ParamInfo"
#define STAFF_ID_NO	"1"
#define STAFF_CODE	"2"


enum EnModifyFeild
{
	EN_NULL = 0,
	EN_MODIFY_CONTENT = 1,
    EN_MODIFY_STATE = 2
};

void CBusinessImpl::OnInit()
{
	m_nAlarmViolationTimeout = 0;
	printf("OnInit enter! plugin = %s\n", MODULE_NAME);
	m_iZoneSecond = 0;
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(OBSERVER_CENTER_ALARM_SYNTHETICAL);
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pJsonFty = ICCGetIJsonFactory();
	m_pHelpTool = ICCGetHelpToolFactory()->CreateHelpTool();
	m_pDBConn = ICCGetIDBConnFactory()->CreateDBConn(DataBase::PostgreSQL);
	m_pRedisClient = ICCGetIRedisClientFactory()->CreateRedisClient();
	m_pHttpClient = ICCGetIHttpClientFactory()->CreateHttpClient();
	m_pMsgCenter = ICCGetIMessageCenterFactory()->CreateMessageCenter();
	m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();
	InitHandleStateOrder();	

	m_uProcThreadCount = 0;
	m_uCurrentThreadIndex = 0;

	m_uProcAlarmThreadCount = 0;
	m_uCurrentAlarmThreadIndex = 0;

	m_bViolationIsRunning = false;

	_InitProcNotifys();

	_CreateThreads();

	printf("OnInit complete! plugin = %s\n", MODULE_NAME);
}

void ICC::CBusinessImpl::InitHandleStateOrder()
{
	// 初始化处警状态顺序
	m_mapProcessStateOrder[PROCESS_STATUS_HANDLING] = 0;	
	m_mapProcessStateOrder[PROCESS_STATUS_TAKEN] = 1;	
	m_mapProcessStateOrder[PROCESS_STATUS_ASSGIN] = 2;	//已下达	
	m_mapProcessStateOrder[PROCESS_STATUS_SIGIN] = 3;	//已接收
	m_mapProcessStateOrder[PROCESS_STATUS_ARRIVED] = 4;	
	m_mapProcessStateOrder[PROCESS_STATUS_CLOSED] = 5;	
	m_mapProcessStateOrder[PROCESS_STATUS_ARCHIVED] = 6;	
	m_mapProcessStateOrder[PROCESS_STATUS_CANCEL] = 7;	
	m_mapProcessStateOrder[PROCESS_STATUS_RETURN] = 8;	
	

	//m_mapProcessStateOrder["02"] = 1;	//已下达
	//m_mapProcessStateOrder["DIC020020"] = 2;	//已到达
	//m_mapProcessStateOrder["DIC020030"] = 3;	//已接收
	//m_mapProcessStateOrder["DIC020040"] = 4;	//已出动
	//m_mapProcessStateOrder["DIC020050"] = 5;	//已到场
	//m_mapProcessStateOrder["DIC020060"] = 6;	//已反馈
	//m_mapProcessStateOrder["DIC020061"] = 7;	//无需反馈
	//m_mapProcessStateOrder["DIC020070"] = 8;	//已作废
	//m_mapProcessStateOrder["DIC020071"] = 9;	//已取消
	//m_mapProcessStateOrder["DIC020072"] = 10;	//退单审核
	//m_mapProcessStateOrder["DIC020073"] = 11;	//已退单
}

void CBusinessImpl::_InitProcNotifys()
{
	m_mapAlarmFuncs.insert(std::make_pair("add_alarm_request", &CBusinessImpl::_ProcessAddAlarmRequest));
	m_mapAlarmFuncs.insert(std::make_pair("add_or_update_alarm_and_process_request", &CBusinessImpl::_ProcessAddOrUpdateAlarmReqeust));

	m_mapFuncs.insert(std::make_pair("add_or_update_process_request", &CBusinessImpl::OnCNotifiAddOrUpdateProcessRequest));
	m_mapFuncs.insert(std::make_pair("add_wechat_alarm_request", &CBusinessImpl::OnCNotifiAddWeChatAlarmRequest));
	m_mapFuncs.insert(std::make_pair("add_alarm_sign_request", &CBusinessImpl::OnCNotifiAddAlarmSignRequest));
	m_mapFuncs.insert(std::make_pair("get_alarm_sign_request", &CBusinessImpl::OnCNotifiGetAlarmSignRequest));

	m_mapFuncs.insert(std::make_pair("add_alarm_remark_request", &CBusinessImpl::OnCNotifiAddAlarmRemarkRequest));
	m_mapFuncs.insert(std::make_pair("get_alarm_remark_request", &CBusinessImpl::OnCNotifiGetAlarmRemarkRequest));
	m_mapFuncs.insert(std::make_pair("get_alarm_all_remark_request", &CBusinessImpl::OnCNotifiGetAlarmAllRemarkRequest));

	m_mapFuncs.insert(std::make_pair("get_alarm_log_request", &CBusinessImpl::OnCNotifiGetAlarmLogRequest));
	m_mapFuncs.insert(std::make_pair("get_alarm_log_alarm_request", &CBusinessImpl::OnCNotifiGetAlarmLogAlarmRequest));
	m_mapFuncs.insert(std::make_pair("get_alarm_log_process_request", &CBusinessImpl::OnCNotifiGetAlarmLogProcessRequest));
	m_mapFuncs.insert(std::make_pair("get_alarm_log_callref_request", &CBusinessImpl::OnCNotifiGetAlarmLogCallrefRequest));

	m_mapFuncs.insert(std::make_pair("get_merge_alarm_request", &CBusinessImpl::OnCNotifiGetMergeAlarmRequest));
	m_mapFuncs.insert(std::make_pair("merge_alarm_request", &CBusinessImpl::OnCNotifiMergeAlarmRequest));
	m_mapFuncs.insert(std::make_pair("unmerge_alarm_request", &CBusinessImpl::OnCNotifiUnMergeRequest));
	m_mapFuncs.insert(std::make_pair("get_alarmid_request", &CBusinessImpl::OnCNotifiGetAlarmBysourceIDRequest));

	m_mapFuncs.insert(std::make_pair("alarm_visit_request", &CBusinessImpl::OnCNotifiAlarmVisitRequest));
	m_mapFuncs.insert(std::make_pair("search_alarm_visit_request", &CBusinessImpl::OnCNotifiSearchAlarmVisitRequest));
	m_mapFuncs.insert(std::make_pair("get_alarm_visit_by_alarmid_request", &CBusinessImpl::OnCNotifiSearchAlarmVisitRequest));
	m_mapFuncs.insert(std::make_pair("get_alarm_visit_by_id_request", &CBusinessImpl::OnCNotifiSearchAlarmVisitRequest));
	m_mapFuncs.insert(std::make_pair("search_visit_statistic_info_request", &CBusinessImpl::OnCNotifiSearchAlarmVisitStatisticRequest));

	m_mapFuncs.insert(std::make_pair("update_callevent_flag_request", &CBusinessImpl::OnCNotifiUpdateCalleventFlagRequest));

	//获取敏感警情信息
	m_mapFuncs.insert(std::make_pair("get_all_key_word_alarm_info_request", &CBusinessImpl::OnCNotifiGetAllKeyWordAlarmRequest));

	m_mapFuncs.insert(std::make_pair("add_jqsacl_request", &CBusinessImpl::OnAlarmRelatedCarsAddExRequest));
	m_mapFuncs.insert(std::make_pair("query_jqsacl_request", &CBusinessImpl::OnAlarmRelatedCarsQueryExRequest));
	m_mapFuncs.insert(std::make_pair("delete_jqsacl_request", &CBusinessImpl::OnAlarmRelatedCarsDeleteExRequest));
	m_mapFuncs.insert(std::make_pair("update_jqsacl_request", &CBusinessImpl::OnAlarmRelatedCarsUpdateExRequest));
	//新增查询
	m_mapFuncs.insert(std::make_pair("search_jqsacl_request", &CBusinessImpl::OnAlarmRelatedCarsSearchExRequest));

	m_mapFuncs.insert(std::make_pair("add_jqdsr_request", &CBusinessImpl::OnAlarmRelatedPersonsAddExRequest));
	m_mapFuncs.insert(std::make_pair("query_jqdsr_request", &CBusinessImpl::OnAlarmRelatedPersonsQueryExRequest));
	m_mapFuncs.insert(std::make_pair("delete_jqdsr_request", &CBusinessImpl::OnAlarmRelatedPersonsDeleteExRequest));
	m_mapFuncs.insert(std::make_pair("update_jqdsr_request", &CBusinessImpl::OnAlarmRelatedPersonsUpdateExRequest));
	// m_mapFuncs.insert(std::make_pair("search_jqdsr_request", &CBusinessImpl::OnAlarmRelatedPersonsSearchExRequest));

	//m_mapFuncs.insert(std::make_pair("alarm_violation_checktimer", &CBusinessImpl::OnAlarmViolationCheckTimer));

	//警情取消   //2022/3/17 资阳项目使用
	m_mapFuncs.insert(std::make_pair("delete_alarm_request", &CBusinessImpl::OnAlarmDeleteRequest));

	m_mapFuncs.insert(std::make_pair("get_alarmid_request", &CBusinessImpl::OnGetAlarmIDRequest));

	//复活警情，将无效变有效
	m_mapFuncs.insert(std::make_pair("revive_alarm_request", &CBusinessImpl::OnReviveAlarmRequest));

	//关联录音
	m_mapFuncs.insert(std::make_pair("set_alarm_callref_request",  &CBusinessImpl::OnSetAlarmCallrefRequest));

	//取消关联录音
	m_mapFuncs.insert(std::make_pair("delete_alarm_callref_request", &CBusinessImpl::OnCNotifiDeleteAlarmCallrefRequest));

	//更新报警人位置信息
	m_mapFuncs.insert(std::make_pair("update_alarm_gps", &CBusinessImpl::OnUpdateAlarmGpsRequest));

	//新增12345接口
	m_mapFuncs.insert(std::make_pair("syn_nacos_params", &CBusinessImpl::OnReceiveSynNacosParams));

	//新增暂存警单(打开/关闭)接口
	m_mapFuncs.insert(std::make_pair("open_temporary_alarm", &CBusinessImpl::OnOpenTemporaryAlarm));
	m_mapFuncs.insert(std::make_pair("close_temporary_alarm", &CBusinessImpl::OnCloseTemporaryAlarm));

	// 新增 短信发送 接口
	m_mapFuncs.insert(std::make_pair("post_sms_send", &CBusinessImpl::OnNotifiPostSmsSend));

	// 获取流水更改详细信息
	m_mapFuncs.insert(std::make_pair("get_alarm_log_info_request", &CBusinessImpl::OnNotifiGetAlarmLogInfo));
	// 联动调派单位 ICC
	m_mapFuncs.insert(std::make_pair("add_or_update_linked_dispatch_info", &CBusinessImpl::ModifyAddOrUpdateLinkedDisPatchRequest));
	// 联动调派单位 VCS
	m_mapFuncs.insert(std::make_pair("add_or_update_linked_dispatch", &CBusinessImpl::OnCNotifiAddOrUpdateLinkedDisPatchRequest));

	// 新增警情附件
	m_mapFuncs.insert(std::make_pair("add_alarm_attach_request", &CBusinessImpl::OnNotifiAddAlarmAttachRequest));
	// 查询警情附件
	m_mapFuncs.insert(std::make_pair("search_alarm_attach_request", &CBusinessImpl::OnNotifiSearchAlarmAttachRequest));
	// 删除警情附件
	m_mapFuncs.insert(std::make_pair("delete_alarm_attach_request", &CBusinessImpl::OnNotifiDeleteAlarmAttachRequest));
	// 警情附件变更
	m_mapFuncs.insert(std::make_pair("alarm_attach_change_request", &CBusinessImpl::OnNotifiAlarmAttachChangeRequest));
	// 聊天记录变更
	m_mapFuncs.insert(std::make_pair("alarm_chatinfo_change_request", &CBusinessImpl::OnNotifiAlarmChatInfoChangeRequest));
	// 黑名单信息发送
	m_mapFuncs.insert(std::make_pair("black_info_send", &CBusinessImpl::OnCNotifuGetBlackMessageSendRequest));

	m_mapFuncs.insert(std::make_pair("call_over_sync", &CBusinessImpl::OnReceiveCallOver));
}

void CBusinessImpl::OnStart()
{
	printf("OnStart enter! plugin = %s\n", MODULE_NAME);
	m_strIdGenRule = m_pConfig->GetValue("ICC/Plugin/Message/idgenrule", "1");
	m_strAlarmIdGenRule = m_pConfig->GetValue("ICC/Plugin/Message/alarmidgenrule", "4");
	std::string tmp_strZone(m_pConfig->GetValue("ICC/Plugin/Message/zone", "0")); //所处的时区

	m_iZoneSecond = m_pString->ToInt(tmp_strZone) *60*60; //时区转秒数
	m_nMsgCenterExecTime = 300;//失败重传等待时间
	m_nMsgCenterExecCount = 6;//失败重传次数

	m_strUrgeCount = m_pConfig->GetValue("ICC/Plugin/Synthetical/UrgeCount", "1");
	m_strUrgeInterval = m_pConfig->GetValue("ICC/Plugin/Synthetical/UrgeInterval", "60");
	m_strUrgeVoiceId = m_pConfig->GetValue("ICC/Plugin/Synthetical/UrgeVoiceId", "100");
	m_strAssistantPolice = m_pConfig->GetValue("ICC/Plugin/Synthetical/AssistantPolice", "");
	m_strPolice = m_pConfig->GetValue("ICC/Plugin/Synthetical/Police", "");
	m_strCodeMode = m_pConfig->GetValue("ICC/Plugin/Synthetical/CodeMode", "1");

	ICC_LOG_DEBUG(m_pLog, "synthetical start begin, idgenrule=%s，AssistantPolice=%s,Police=%s,CodeMode:[%s]", m_strIdGenRule.c_str(), m_strAssistantPolice.c_str(), m_strPolice.c_str(), m_strCodeMode.c_str());

	m_strSmsServiceName = m_pConfig->GetValue("ICC/Plugin/Sms/SmsServiceName", "rvs");
	m_strWechatServiceName = m_pConfig->GetValue("ICC/Plugin/Synthetical/WechatServiceName", "commandcenter-icc-octg");
	m_strWechatfileServiceName = m_pConfig->GetValue("ICC/Plugin/Synthetical/WechatfileServiceName", "commandcenter-file-service");


	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_alarm_request", OnCNotifiAddAlarmRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_or_update_alarm_and_process_request", OnCNotifiAddOrUpdateAlarmAndProcessRequest);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_or_update_process_request", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_wechat_alarm_request", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_alarm_sign_request", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_sign_request", _OnReceiveNotify);
	
    ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_alarm_remark_request", _OnReceiveNotify);
    ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_remark_request", _OnReceiveNotify);
    ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_all_remark_request", _OnReceiveNotify);
	
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_log_request", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_log_alarm_request", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_log_process_request", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_log_callref_request", _OnReceiveNotify);
	
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_merge_alarm_request", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "merge_alarm_request", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "unmerge_alarm_request", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarmid_request", _OnReceiveNotify);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_visit_request", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_alarm_visit_request", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_visit_by_alarmid_request", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_visit_by_id_request", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_visit_statistic_info_request", _OnReceiveNotify);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "update_callevent_flag_request", _OnReceiveNotify);

	//获取敏感警情信息
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_all_key_word_alarm_info_request", _OnReceiveNotify);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_jqsacl_request", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_jqsacl_request", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_jqsacl_request", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "update_jqsacl_request", _OnReceiveNotify);
	//新增查询
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_jqsacl_request", _OnReceiveNotify);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_jqdsr_request", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_jqdsr_request", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_jqdsr_request", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "update_jqdsr_request", _OnReceiveNotify);
	// ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_jqdsr_request", _OnReceiveNotify);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_violation_checktimer",OnAlarmViolationCheckTimer);

	//警情取消   //2022/3/17 资阳项目使用
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_alarm_request", _OnReceiveNotify);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarmid_request", _OnReceiveNotify);

	//复活警情，将无效变有效
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "revive_alarm_request", _OnReceiveNotify);

	// 关联警情
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_alarm_callref_request", _OnReceiveNotify);
	// 取消关联警情
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_alarm_callref_request", _OnReceiveNotify);

	//更新报警人位置信息
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "update_alarm_gps", _OnReceiveNotify);

	//新增12345接口
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "syn_nacos_params", _OnReceiveNotify);

	//新增暂存警单(打开/关闭)接口
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "open_temporary_alarm", _OnReceiveNotify);
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "close_temporary_alarm", _OnReceiveNotify);

	// 新增 短信发送 接口
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "post_sms_send", _OnReceiveNotify);

	// 获取流水更改详细信息
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_log_info_request", _OnReceiveNotify);
	// 新增或更新联动单位 VCS
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_or_update_linked_dispatch", _OnReceiveNotify);

	// 新增警情附件
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_alarm_attach_request", _OnReceiveNotify);
	// 查询警情附件
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_alarm_attach_request", _OnReceiveNotify);
	// 删除警情附件
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_alarm_attach_request", _OnReceiveNotify);

	// 警情附件变更
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_attach_change_request", _OnReceiveNotify);

	// 聊天记录变更
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_chatinfo_change_request", _OnReceiveNotify);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "black_info_send", _OnReceiveNotify);

	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "call_over_sync", _OnReceiveNotify);

	m_pMsgCenter->Start();

	printf("OnStart complete! plugin = %s, zone second: %d\n", MODULE_NAME, m_iZoneSecond);

	m_nAlarmViolationTimeout = 300;
	std::string l_strAlarmTimeout;
	if (m_pRedisClient->HGet("Violation_TimeoutValue", "AlarmTimeout", l_strAlarmTimeout))
	{
		if (!l_strAlarmTimeout.empty())
		{
			m_nAlarmViolationTimeout = std::atoi(l_strAlarmTimeout.c_str());
		}
	}
	else
	{
		std::string l_strAlarmViolationTimeOut = m_pConfig->GetValue("ICC/Plugin/CTI/ViolationAlarmTimeOut", "");
		if (!l_strAlarmViolationTimeOut.empty())
		{
			m_nAlarmViolationTimeout = std::atoi(l_strAlarmViolationTimeOut.c_str());
			m_pRedisClient->HSet("Violation_TimeoutValue", "AlarmTimeout", l_strAlarmViolationTimeOut);
		}
	}
	
	if (m_pTimerMgr && m_nAlarmViolationTimeout > 0)
	{
		//启动1s的违规检测定时器
		m_pTimerMgr->AddTimer("alarm_violation_checktimer", 1, 10);
	}

	//读取是否启用自动催办配置
	m_strIsUseAutourge = m_pConfig->GetValue("ICC/Plugin/Synthetical/isUseAutourge", "1");
	m_strAutourgeType = m_pConfig->GetValue("ICC/Plugin/Synthetical/autourgeMode", "2");
	m_strLang_code = m_pConfig->GetValue("ICC/Plugin/Synthetical/langCode", "zh-CN");
	m_strAutourgeSmsContent = m_pConfig->GetValue("ICC/Plugin/Synthetical/UrgeSmsContent", "");

	ICC_LOG_DEBUG(m_pLog, "synthetical start complete ViolationTimeOut=%d", m_nAlarmViolationTimeout);


	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_alarm_request", OnCNotifiAddAlarmRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_or_update_alarm_and_process_request", OnCNotifiAddOrUpdateAlarmAndProcessRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_or_update_process_request", OnCNotifiAddOrUpdateProcessRequest);
 //   
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_alarm_sign_request", OnCNotifiAddAlarmSignRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_sign_request", OnCNotifiGetAlarmSignRequest);
	//
 //   ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_alarm_remark_request", OnCNotifiAddAlarmRemarkRequest);
 //   ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_remark_request", OnCNotifiGetAlarmRemarkRequest);
 //   ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_all_remark_request", OnCNotifiGetAlarmAllRemarkRequest);
	//
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_log_request", OnCNotifiGetAlarmLogRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_log_alarm_request", OnCNotifiGetAlarmLogAlarmRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_log_process_request", OnCNotifiGetAlarmLogProcessRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_log_callref_request", OnCNotifiGetAlarmLogCallrefRequest);
	//
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_merge_alarm_request", OnCNotifiGetMergeAlarmRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "merge_alarm_request", OnCNotifiMergeAlarmRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "unmerge_alarm_request", OnCNotifiUnMergeRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarmid_request", OnCNotifiGetAlarmBysourceIDRequest);

	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_visit_request", OnCNotifiAlarmVisitRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_alarm_visit_request", OnCNotifiSearchAlarmVisitRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_visit_by_alarmid_request", OnCNotifiSearchAlarmVisitRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_visit_by_id_request", OnCNotifiSearchAlarmVisitRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_visit_statistic_info_request", OnCNotifiSearchAlarmVisitStatisticRequest);

	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "update_callevent_flag_request", OnCNotifiUpdateCalleventFlagRequest);

	////获取敏感警情信息
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_all_key_word_alarm_info_request", OnCNotifiGetAllKeyWordAlarmRequest);

	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_jqsacl_request", OnAlarmRelatedCarsAddExRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_jqsacl_request", OnAlarmRelatedCarsQueryExRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_jqsacl_request", OnAlarmRelatedCarsDeleteExRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "update_jqsacl_request", OnAlarmRelatedCarsUpdateExRequest);
	////新增查询
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_jqsacl_request", OnAlarmRelatedCarsSearchExRequest);

	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "add_jqdsr_request", OnAlarmRelatedPersonsAddExRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "query_jqdsr_request", OnAlarmRelatedPersonsQueryExRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_jqdsr_request", OnAlarmRelatedPersonsDeleteExRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "update_jqdsr_request", OnAlarmRelatedPersonsUpdateExRequest);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "search_jqdsr_request", OnAlarmRelatedPersonsSearchExRequest);

	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "alarm_violation_checktimer", OnAlarmViolationCheckTimer);

	////警情取消   //2022/3/17 资阳项目使用
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "delete_alarm_request", OnAlarmDeleteRequest);

	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarmid_request", OnGetAlarmIDRequest);

	////复活警情，将无效变有效
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "revive_alarm_request", OnReviveAlarmRequest);

	////复活警情，将无效变有效
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "set_alarm_callref_request", OnSetAlarmCallrefRequest);

	////更新报警人位置信息
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "update_alarm_gps", OnUpdateAlarmGpsRequest);

	////新增12345接口
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "push_alarm_to12345", OnPushAlarmTo12345);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "repulse_alarm_to12345", OnRepulseAlarmTo12345); 
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "syn_nacos_params", OnReceiveSynNacosParams);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "msgcenter_send_failed_retransmission", OnMsgCenterRetransmission);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "feedback_sync", OnPushRevisitTo12345);//接收同步信息后向12345推送反馈单信息

	////新增暂存警单(打开/关闭)接口
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "open_temporary_alarm", OnOpenTemporaryAlarm);
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "close_temporary_alarm", OnCloseTemporaryAlarm);

	//// 新增 短信发送 接口
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "post_sms_send", OnNotifiPostSmsSend);

	//// 获取流水更改详细信息
	//ADDOBSERVER(m_pObserverCenter, CBusinessImpl, "get_alarm_log_info_request", OnNotifiGetAlarmLogInfo);
}

void CBusinessImpl::OnStop()
{
	ICC_LOG_DEBUG(m_pLog, "alarmreceipt stop success");
}

void CBusinessImpl::OnDestroy()
{
}

void CBusinessImpl::OnUpdateAlarmGpsRequest(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::string strMessage = p_pNotifiRequest->GetMessages();	
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", strMessage.c_str());

	PROTOCOL::CUpdateBJRGpsRequest l_oRequest;
	if (!l_oRequest.ParseString(strMessage, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", strMessage.c_str());
		return;
	}

	DataBase::SQLRequest l_SQLRequest;

	l_SQLRequest.sql_id = "select_icc_t_jjdb";
	l_SQLRequest.param["id"] = l_oRequest.m_oBody.m_alarm_id;
	DataBase::IResultSetPtr l_Result = m_pDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "Select Alarm sql:[%s]", l_Result->GetSQL().c_str());

	std::string strLongitude;
	std::string strLatitude;
	if (!l_Result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select alarm  info failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
		return;
	}
	if (l_Result->Next())
	{
		if (l_Result->GetValue("bjrxzb").empty()&&!l_oRequest.m_oBody.m_bjr_gps_x.empty())
		{
			strLongitude = l_oRequest.m_oBody.m_bjr_gps_x;
		}
		if (l_Result->GetValue("bjryzb").empty()&&!l_oRequest.m_oBody.m_bjr_gps_y.empty())
		{
			strLatitude = l_oRequest.m_oBody.m_bjr_gps_y;
		}
	}
	ICC_LOG_LOWDEBUG(m_pLog, "Longitude:[%s],Latitude:[%s]", strLongitude.c_str(), strLatitude.c_str());
	l_SQLRequest.param.clear();
	l_SQLRequest.sql_id = "update_icc_t_jjdb";
	l_SQLRequest.param["id"] = l_oRequest.m_oBody.m_alarm_id;
	l_SQLRequest.set["alarm_addr"] = l_oRequest.m_oBody.m_bjr_dz;
	l_SQLRequest.set["addr"] = l_oRequest.m_oBody.m_bjr_dz;

	std::string strTime = m_pDateTime->GetAlarmIdTime(l_oRequest.m_oBody.m_alarm_id);
	if (strTime != "")
	{
		l_SQLRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_SQLRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}

	//2023.1.4修改将原本longitude,latitude修改为manual_longitude,manual_latitude
	//l_SQLRequest.set["longitude"] = l_oRequest.m_oBody.m_bjr_gps_x;
	//l_SQLRequest.set["latitude"] = l_oRequest.m_oBody.m_bjr_gps_y;

	l_SQLRequest.set["manual_longitude"] = l_oRequest.m_oBody.m_bjr_gps_x;
	l_SQLRequest.set["manual_latitude"] = l_oRequest.m_oBody.m_bjr_gps_y;
	if (!strLongitude.empty())
	{
		l_SQLRequest.set["longitude"] = strLongitude;
	}
	if (!strLatitude.empty())
	{
		l_SQLRequest.set["latitude"] = strLatitude;
	}

	l_Result = m_pDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	bool bExecSuccess = false;

	PROTOCOL::CHeaderEx l_oRespond = l_oRequest.m_oHeader;
	if (!l_Result->IsValid())
	{
		l_oRespond.m_strResult = "1";
		l_oRespond.m_strMsg = "execute update_icc_t_jjdb failed";		
		ICC_LOG_ERROR(m_pLog, "update_icc_t_jjdb failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());		
	}
	else
	{
		l_oRespond.m_strResult = "0";
		bExecSuccess = true;
	}

	//回复消息
	JsonParser::IJsonPtr tmp_spJson = m_pJsonFty->CreateJson();
	l_oRespond.SaveTo(tmp_spJson);
	std::string tmp_strMsg(tmp_spJson->ToString());
	p_pNotifiRequest->Response(tmp_strMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", tmp_strMsg.c_str());

	if (bExecSuccess)
	{
		SyncReceiptDate(l_oRequest.m_oBody.m_alarm_id);
	}
}

void CBusinessImpl::OnSetAlarmCallrefRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CSetAlarmCallrefRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	bool tmp_bSuccess = _SetAlarmCallref(l_oRequest.m_oBody.m_strAlarmID, l_oRequest.m_oBody.m_strCallrefID);

	PROTOCOL::CHeaderEx l_oRespond = l_oRequest.m_oHeader;
	if (tmp_bSuccess)
	{
		l_oRespond.m_strResult = "0";
	}
	else
	{
		l_oRespond.m_strResult = "1";
	}
	//回复消息
	JsonParser::IJsonPtr tmp_spJson = m_pJsonFty->CreateJson();
	l_oRespond.SaveTo(tmp_spJson);
	std::string tmp_strMsg(tmp_spJson->ToString());
	p_pNotify->Response(tmp_strMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", tmp_strMsg.c_str());

	if (tmp_bSuccess)
	{
		//发同步消息
		PROTOCOL::CSetAlarmCallrefSync l_oSetAlarmCallrefSync;
		l_oSetAlarmCallrefSync.m_oBody.m_strAlarmID = l_oRequest.m_oBody.m_strAlarmID;
		l_oSetAlarmCallrefSync.m_oBody.m_strCallrefID = l_oRequest.m_oBody.m_strCallrefID;

		l_oSetAlarmCallrefSync.m_oHeader.m_strSystemID = SYSTEMID;
		l_oSetAlarmCallrefSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
		l_oSetAlarmCallrefSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_oSetAlarmCallrefSync.m_oHeader.m_strRelatedID = l_oRequest.m_oHeader.m_strMsgId;
		l_oSetAlarmCallrefSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		l_oSetAlarmCallrefSync.m_oHeader.m_strCmd = "topic_set_alarm_callref_sync";
		l_oSetAlarmCallrefSync.m_oHeader.m_strRequest = "topic_set_alarm_callref_sync";
		l_oSetAlarmCallrefSync.m_oHeader.m_strRequestType = "1";
		l_oSetAlarmCallrefSync.m_oHeader.m_strResponse = "";
		l_oSetAlarmCallrefSync.m_oHeader.m_strResponseType = "";
		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMessage = l_oSetAlarmCallrefSync.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());

		PROTOCOL::CAddOrUpdateAlarmWithProcessRequest l_oAlarmRequest;
		// 同步 vcs 
		if (GetAlarm(l_oRequest.m_oBody.m_strAlarmID, l_oAlarmRequest))
		{
			l_oAlarmRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
			l_oAlarmRequest.m_oBody.m_oAlarm.m_strUpdateTime = m_pDateTime->CurrentDateTimeStr();
			ICC_LOG_DEBUG(m_pLog, "send m_strAlarmID:[%s] - [%s][%s][%s][%s]", l_oRequest.m_oBody.m_strAlarmID.c_str(), l_oRequest.m_oBody.m_strUpdateUserName.c_str(),
				l_oRequest.m_oBody.m_strUpdateUserDeptName.c_str(), l_oRequest.m_oBody.m_strUpdateUserDeptCode.c_str(), l_oRequest.m_oBody.m_strUpdateUser.c_str());
			SyncSyntInfo(l_oAlarmRequest, EDIT);
		}
		if (!l_oRequest.m_oBody.m_strAlarmID.empty() && l_oRequest.m_oBody.m_strNeedLog == "1")
		{
			// 写流水
			std::string l_strUserName = l_oRequest.m_oBody.m_strUpdateUserName;
			std::string l_strDeptName = l_oRequest.m_oBody.m_strUpdateUserDeptName;
			std::string l_strDeptCode = l_oRequest.m_oBody.m_strUpdateUserDeptCode;
			std::string l_strUserCode = l_oRequest.m_oBody.m_strUpdateUser;
			if (l_strUserCode.empty())
			{
				ICC_LOG_ERROR(m_pLog, "merge user code is empty!!! will not wriete log!");
			}

			if (l_strUserName.empty() || l_strDeptCode.empty())
			{
				GetStaffDept(l_strUserCode, l_strUserName, l_strDeptName, l_strDeptCode);
			}

			PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
			l_oAlarmLogInfo.m_strAlarmID = l_oRequest.m_oBody.m_strAlarmID;
			std::vector<std::string> l_vecParamList;
			Data::CStaffInfo l_oStaffInfo;
			if (!_GetStaffInfo(l_strUserCode, l_oStaffInfo))
			{
				ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
			}
			std::string l_strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_strUserName);	  //警员姓名

			l_vecParamList.push_back(l_strStaffName);
			l_vecParamList.push_back(l_strUserCode);
			l_vecParamList.push_back(l_strDeptCode);
			l_vecParamList.push_back(l_strDeptName);
			l_vecParamList.push_back(l_oRequest.m_oBody.m_strCallrefID);

			l_oAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
			l_oAlarmLogInfo.m_strCreateUser = l_strUserCode;
			l_oAlarmLogInfo.m_strOperate = LOG_ALARM_CALLREF;
			l_oAlarmLogInfo.m_strDeptOrgCode = l_strDeptCode;
			l_oAlarmLogInfo.m_strSourceName = "3";

			std::string type = RESOURCETYPE_CALLREF;
			l_oAlarmLogInfo.m_strOperateAttachDesc = BuildAlarmLogAttach(type, l_oRequest.m_oBody.m_strCallrefID);

			AddAlarmLogInfo(l_oAlarmLogInfo, "");
		}
	}
}

void CBusinessImpl::OnCNotifiDeleteAlarmCallrefRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CSetAlarmCallrefRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	bool tmp_bSuccess = _DeleteAlarmCallref(l_oRequest.m_oBody.m_strAlarmID, l_oRequest.m_oBody.m_strCallrefID);
	PROTOCOL::CHeaderEx l_oRespond = l_oRequest.m_oHeader;
	if (tmp_bSuccess)
	{
		l_oRespond.m_strResult = "0";
	}
	else
	{
		l_oRespond.m_strResult = "1";
	}
	//回复消息
	JsonParser::IJsonPtr tmp_spJson = m_pJsonFty->CreateJson();
	l_oRespond.SaveTo(tmp_spJson);
	std::string tmp_strMsg(tmp_spJson->ToString());
	p_pNotify->Response(tmp_strMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", tmp_strMsg.c_str());

	if (tmp_bSuccess)
	{
		//发同步消息
		PROTOCOL::CSetAlarmCallrefSync l_oSetAlarmCallrefSync;
		l_oSetAlarmCallrefSync.m_oBody.m_strAlarmID = l_oRequest.m_oBody.m_strAlarmID;
		l_oSetAlarmCallrefSync.m_oBody.m_strCallrefID = l_oRequest.m_oBody.m_strCallrefID;

		l_oSetAlarmCallrefSync.m_oHeader.m_strSystemID = SYSTEMID;
		l_oSetAlarmCallrefSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
		l_oSetAlarmCallrefSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_oSetAlarmCallrefSync.m_oHeader.m_strRelatedID = l_oRequest.m_oHeader.m_strMsgId;
		l_oSetAlarmCallrefSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		l_oSetAlarmCallrefSync.m_oHeader.m_strCmd = "topic_set_alarm_callref_sync";
		l_oSetAlarmCallrefSync.m_oHeader.m_strRequest = "topic_set_alarm_callref_sync";
		l_oSetAlarmCallrefSync.m_oHeader.m_strRequestType = "1";
		l_oSetAlarmCallrefSync.m_oHeader.m_strResponse = "";
		l_oSetAlarmCallrefSync.m_oHeader.m_strResponseType = "";
		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMessage = l_oSetAlarmCallrefSync.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());

		PROTOCOL::CAddOrUpdateAlarmWithProcessRequest l_oAlarmRequest;
		// 同步 vcs 
		if (GetAlarm(l_oRequest.m_oBody.m_strAlarmID, l_oAlarmRequest))
		{
			l_oAlarmRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
			l_oAlarmRequest.m_oBody.m_oAlarm.m_strUpdateTime = m_pDateTime->CurrentDateTimeStr();

			ICC_LOG_DEBUG(m_pLog, "send m_strAlarmID:[%s]", l_oRequest.m_oBody.m_strAlarmID.c_str());
			SyncSyntInfo(l_oAlarmRequest, EDIT);
		}
		if (!l_oRequest.m_oBody.m_strAlarmID.empty())
		{
			// 写流水
			std::string l_strUserName = l_oRequest.m_oBody.m_strUpdateUserName;
			std::string l_strDeptName = l_oRequest.m_oBody.m_strUpdateUserDeptName;
			std::string l_strDeptCode = l_oRequest.m_oBody.m_strUpdateUserDeptCode;
			std::string l_strUserCode = l_oRequest.m_oBody.m_strUpdateUser;
			if (l_strUserCode.empty())
			{
				ICC_LOG_ERROR(m_pLog, "merge user code is empty!!! will not wriete log!");
			}

			if (l_strUserName.empty() || l_strDeptCode.empty())
			{
				GetStaffDept(l_strUserCode, l_strUserName, l_strDeptName, l_strDeptCode);
			}

			PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
			l_oAlarmLogInfo.m_strAlarmID = l_oRequest.m_oBody.m_strAlarmID;
			std::vector<std::string> l_vecParamList;
			Data::CStaffInfo l_oStaffInfo;
			if (!_GetStaffInfo(l_strUserCode, l_oStaffInfo))
			{
				ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
			}
			std::string l_strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_strUserName);

			l_vecParamList.push_back(l_strStaffName);
			l_vecParamList.push_back(l_strUserCode);
			l_vecParamList.push_back(l_strDeptCode);
			l_vecParamList.push_back(l_strDeptName);
			l_vecParamList.push_back(l_oRequest.m_oBody.m_strCallrefID);

			l_oAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
			l_oAlarmLogInfo.m_strCreateUser = l_strUserCode;

			l_oAlarmLogInfo.m_strOperate = LOG_DELETE_ALARM_CALLREF;
			l_oAlarmLogInfo.m_strDeptOrgCode = l_strDeptCode;
			l_oAlarmLogInfo.m_strSourceName = "3";

			std::string type = RESOURCETYPE_CALLREF;
			l_oAlarmLogInfo.m_strOperateAttachDesc = BuildAlarmLogAttach(type, l_oRequest.m_oBody.m_strCallrefID);
			AddAlarmLogInfo(l_oAlarmLogInfo, "");
		}
	}
}


void CBusinessImpl::OnReviveAlarmRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CReviveAlarmRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	PROTOCOL::CHeaderEx l_oRespond = l_oRequest.m_oHeader;

	//处理复活
	std::string tmp_strTransID;
	bool tmp_bSuccess = _UpdateReviveInfo(l_oRequest.m_oBody, tmp_strTransID, l_oRespond.m_strMsg);
	
	if (tmp_bSuccess)
	{
		m_pDBConn->Commit(tmp_strTransID);
	}
	else
	{
		l_oRespond.m_strResult = "1";
		m_pDBConn->Rollback(tmp_strTransID);
	}

	//回复消息
	JsonParser::IJsonPtr tmp_spJson = m_pJsonFty->CreateJson();
	l_oRespond.SaveTo(tmp_spJson);

	std::string tmp_strMsg(tmp_spJson->ToString());
	p_pNotify->Response(tmp_strMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", tmp_strMsg.c_str());
}


//////////////////////////////////////////////////////////////////////////
// 业务处理入口

void CBusinessImpl::_ProcessAddAlarmRequest(ICC::PROTOCOL::AddOrUpdateAlarmWithProcessRequestPtr pAlarmRequestPtr, ObserverPattern::INotificationPtr p_pNotify)
{
	if (pAlarmRequestPtr == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "add alarm request is null");
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "_ProcessAddAlarmRequest enter, alarm id: %s", pAlarmRequestPtr->m_oBody.m_oAlarm.m_strID.c_str());

	PROTOCOL::CAddOrUpdateAlarmWithProcessRequest &l_oRequest = *pAlarmRequestPtr;

	// 构造回复
	PROTOCOL::CAddOrUpdateAlarmWithProcessRespond l_oRespond;
	BuildRespondHeader("add_alarm_respond", l_oRequest.m_oHeader, l_oRespond.m_oHeader);

	//防止新建警单的时候，一个话务前端多次请求,谈多次单
	if(!l_oRequest.m_oBody.m_oAlarm.m_strSourceID.empty()&& l_oRequest.m_oBody.m_strSaveFlag!="1")
	{
		std::lock_guard<std::mutex> guard(m_mutexCallMap);
		auto it = m_mapCall.find(l_oRequest.m_oBody.m_oAlarm.m_strSourceID);
		if (it != m_mapCall.end())
		{
			ICC_LOG_DEBUG(m_pLog, "callref id already exists:%s", l_oRequest.m_oBody.m_oAlarm.m_strSourceID.c_str());
			l_oRespond.m_oHeader.m_strResult = "1";
			l_oRespond.m_oHeader.m_strMsg = "Insert alarm failed";
			l_oRespond.m_oHeader.m_strMsg = "Source ID Already Create Alarm";
			std::string l_strSendMsg = l_oRespond.ToString4AddAlarm(m_pJsonFty->CreateJson());
			p_pNotify->Response(l_strSendMsg);
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
			return;
		}
		else
		{
			m_mapCall[l_oRequest.m_oBody.m_oAlarm.m_strSourceID] = m_pString->CreateGuid();
		}
	}


	if (l_oRequest.m_oBody.m_strSaveFlag == "1")
	{
		ICC_LOG_DEBUG(m_pLog, "timing save . [%s]", l_oRequest.m_oBody.m_oAlarm.m_strID.c_str());

		do
		{
			ICC::PROTOCOL::CAlarmInfo info;
			if (!_QueryAlarmInfo(l_oRequest.m_oBody.m_oAlarm.m_strID, info))
			{
				ICC_LOG_ERROR(m_pLog, "query alarm failed. id : %s", l_oRequest.m_oBody.m_oAlarm.m_strID.c_str());
				l_oRespond.m_oHeader.m_strResult = "1";
				l_oRespond.m_oHeader.m_strMsg = "not find alarm when timing save!!";
				break;
			}

			ICC_LOG_DEBUG(m_pLog, "timing save . [%s], state:%s", l_oRequest.m_oBody.m_oAlarm.m_strID.c_str(), info.m_strState.c_str());

			if ("00" != info.m_strState)
			{
				ICC_LOG_INFO(m_pLog, "new alarm state is less, not timing save. id : %s", l_oRequest.m_oBody.m_oAlarm.m_strID.c_str());
				l_oRespond.m_oHeader.m_strResult = "0";
				l_oRespond.m_oHeader.m_strMsg = "new alarm state is less, not timing save!!";
				break;
			}

			if (!UpdateAlarmInfo(l_oRequest.m_oBody, ""))
			{
				ICC_LOG_ERROR(m_pLog, "UpdateAlarmInfo failed.");
				l_oRespond.m_oHeader.m_strResult = "1";
				l_oRespond.m_oHeader.m_strMsg = "update alarm failed";
			}

		} while (false);
		if (l_oRequest.m_oBody.m_oAlarm.m_strReceiptDeptCode.empty() || l_oRequest.m_oBody.m_oAlarm.m_strReceiptCode.empty()) {
			l_oRespond.m_oHeader.m_strResult = "1";
			l_oRespond.m_oHeader.m_strMsg = "ReceiptDeptCode or ReceiptCode is empty";
		}
		std::string l_strSendMsg = l_oRespond.ToString4AddAlarm(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	}
	else
	{
		bool l_bRes = BuildAddAlarmRespond(l_oRespond, l_oRequest);

		if (!l_bRes)
		{
			l_oRespond.m_oHeader.m_strResult = "1";
			l_oRespond.m_oHeader.m_strMsg = "Insert alarm failed";
			if (l_oRequest.m_oBody.m_oAlarm.m_strReceiptDeptCode.empty() || l_oRequest.m_oBody.m_oAlarm.m_strReceiptCode.empty()) {
				l_oRespond.m_oHeader.m_strMsg = "ReceiptDeptCode or ReceiptCode is empty";
			}
		}

		std::string l_strSendMsg = l_oRespond.ToString4AddAlarm(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

		if (l_bRes)
		{// 发送同步
			SyncAlarmInfo(l_oRequest.m_oBody.m_oAlarm, ESyncType::ADD);


			_SyncKeyWordAlarmInfo(l_oRequest.m_oBody.m_oAlarm, false);

		}
		m_mapCall.erase(l_oRequest.m_oBody.m_oAlarm.m_strSourceID);
	}
	if (l_oRequest.m_oBody.m_oAlarm.m_strState == "00")
	{
		ICC_LOG_DEBUG(m_pLog, "add Alarm Violation Check,id=%s", l_oRequest.m_oBody.m_oAlarm.m_strID.c_str());
		_AddAlarmViolationCheck(l_oRequest.m_oBody.m_oAlarm.m_strID, l_oRequest.m_oBody.m_oAlarm.m_strCreateTime);
	}
}
void CBusinessImpl::OnCNotifiAddAlarmRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	
	//--By z13060 2018/08/25 12:00 修改创警逻辑，所有失败节点需要返回。
	// 解析请求消息
	PROTOCOL::AddOrUpdateAlarmWithProcessRequestPtr l_oRequest = std::make_shared<PROTOCOL::CAddOrUpdateAlarmWithProcessRequest>();
	if (!l_oRequest->ParseString4AddAlarm(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	int iAlarmId = _GetAlarmId(l_oRequest->m_oBody.m_oAlarm.m_strID);
	CommonWorkThreadPtr pThread = _GetThreadEx(iAlarmId);
	if (pThread)
	{
		pThread->AddMessage(this, THREADID_RECEIVEALARMNOTIFY, 0, 0, 0, "add_alarm_request", "", p_pNotify, nullptr, nullptr, l_oRequest);
	}
	
	ICC_LOG_DEBUG(m_pLog, "add alarm message: alarm id: [%s:%d], ", l_oRequest->m_oBody.m_oAlarm.m_strID.c_str(), iAlarmId);
}

bool CBusinessImpl::_QueryAlarmInfoBySourceId(const std::string& strSourceId, PROTOCOL::CAlarmInfo& alarmInfo)
{
	DataBase::SQLRequest l_tSQLRequest;
	l_tSQLRequest.sql_id = "select_icc_t_jjdb_by_source_id";	
	l_tSQLRequest.param["call_id"] = strSourceId;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLRequest);
	ICC_LOG_DEBUG(m_pLog, "_QueryAlarmInfoBySourceId sql complete:[%s]", l_pRSet->GetSQL().c_str());

	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update alarm jjwc time failed,error msg:[%s], callid:[%s]", l_pRSet->GetErrorMsg().c_str(), strSourceId.c_str());
		return false;
	}

	if (l_pRSet->Next())
	{
		alarmInfo.ParseAlarmRecordEx(l_pRSet);
		return true;
	}

	ICC_LOG_WARNING(m_pLog, "_QueryAlarmInfoBySourceId record is null");

	return false;
}

bool CBusinessImpl::_UpdateJJWCTime(const std::string& strCallId, const std::string& strHangupTime)
{
	DataBase::SQLRequest l_tSQLRequest;
	l_tSQLRequest.sql_id = "update_icc_t_jjdb_jjwcsj_by_source_id";
	l_tSQLRequest.param["hangup_time"] = strHangupTime;
	l_tSQLRequest.param["update_time"] = m_pDateTime->CurrentDateTimeStr();
	l_tSQLRequest.param["call_id"] = strCallId;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLRequest);
	ICC_LOG_DEBUG(m_pLog, "_UpdateJJWCTime sql complete:[%s]", l_pRSet->GetSQL().c_str());

	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update alarm jjwc time failed,error msg:[%s], callid:[%s], hanguptime:[%s]", l_pRSet->GetErrorMsg().c_str(), strCallId.c_str(), strHangupTime.c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "update alarm jjwc time success,callid:[%s], hanguptime:[%s]", strCallId.c_str(), strHangupTime.c_str());

	return true;
}

void CBusinessImpl::OnReceiveCallOver(ObserverPattern::INotificationPtr p_pNotify)
{
	std::string l_strRequestMsg = p_pNotify->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "OnReceiveCallOver receive call over event: %s", l_strRequestMsg.c_str());

	PROTOCOL::CCallOverEvent l_oRequestObj;
	if (!l_oRequestObj.ParseString(l_strRequestMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "Invalid call over request protocol: [%s]", l_strRequestMsg.c_str());
		return;
	}

	PROTOCOL::CAlarmInfo alarmInfo;
	if (!_QueryAlarmInfoBySourceId(l_oRequestObj.m_oBody.m_strCallRefId, alarmInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "OnReceiveCallOver sync alarm info get alarm failed: [%s]", l_oRequestObj.m_oBody.m_strCallRefId.c_str());
		return;
	}

	_UpdateJJWCTime(l_oRequestObj.m_oBody.m_strCallRefId, l_oRequestObj.m_oBody.m_strHangupTime);

	ICC_LOG_DEBUG(m_pLog, "OnReceiveCallOver update jjwcsj complete: [%s]", l_oRequestObj.m_oBody.m_strCallRefId.c_str());

	if (!alarmInfo.m_strFirstSubmitTime.empty())
	{
		alarmInfo.m_strFirstSubmitTime = l_oRequestObj.m_oBody.m_strHangupTime;
		SyncAlarmInfo(alarmInfo, ESyncType::EDIT);

		ICC_LOG_DEBUG(m_pLog, "OnReceiveCallOver sync alarm info complete: [%s]", l_oRequestObj.m_oBody.m_strCallRefId.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "OnReceiveCallOver alarm is not submit,will not sync: [%s:%s]", l_oRequestObj.m_oBody.m_strCallRefId.c_str(),
			alarmInfo.m_strID.c_str());
	}
}

void CBusinessImpl::_ProcessAddOrUpdateAlarmReqeust(ICC::PROTOCOL::AddOrUpdateAlarmWithProcessRequestPtr pAlarmRequestPtr, ObserverPattern::INotificationPtr p_pNotify)
{
	if (pAlarmRequestPtr == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "add or update alarm request is null");
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "_ProcessAddOrUpdateAlarmReqeust enter， alarm id: %s", pAlarmRequestPtr->m_oBody.m_oAlarm.m_strID.c_str());

	PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& l_oRequest = *pAlarmRequestPtr;

	std::map<std::string, std::map<std::string, std::string>> l_AlarmNewData, l_AlarmSourceValue;
	std::string l_strGuidID = m_pString->CreateGuid();
	// 更新之前先根据警情id记录一下表单内容
	if (l_oRequest.m_oBody.m_strIsEdit == "1" && !l_oRequest.m_oBody.m_oAlarm.m_strID.empty())
	{
		l_AlarmSourceValue = GetAlarmDataByAlarmID(l_oRequest.m_oBody.m_oAlarm.m_strID);
	}

	if (l_oRequest.m_oBody.m_oAlarm.m_strState == "00")
	{
		ICC_LOG_DEBUG(m_pLog, "add Alarm Violation Check,id=%s", l_oRequest.m_oBody.m_oAlarm.m_strID.c_str());
		_AddAlarmViolationCheck(l_oRequest.m_oBody.m_oAlarm.m_strID, l_oRequest.m_oBody.m_oAlarm.m_strCreateTime);
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "delete Alarm Violation Check,id=%s", l_oRequest.m_oBody.m_oAlarm.m_strID.c_str());
		_DeleteAlarmWiolationCheck(l_oRequest.m_oBody.m_oAlarm.m_strID);
	}

	// 新增或更新警单
	bool l_bIsSuccess(false);
	bool bUpdateVisit = true;

	std::string tmp_strMsg("success");
	ESyncType l_AlarmState = EDIT;
	int tmp_iModifyContent = 0;

	if (!IsSyntInfoValid(l_oRequest.m_oBody))
	{// 数据不合理
		// to do something

		ICC_LOG_DEBUG(m_pLog, "[%s]xxxx AddOrUpdateAlarmAndProcessRequest 1.1.", l_oRequest.m_oBody.m_oAlarm.m_strID.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "[%s]xxxx AddOrUpdateAlarmAndProcessRequest 2.0.", l_oRequest.m_oBody.m_oAlarm.m_strID.c_str());

		std::string l_strTransactionGuid = m_pDBConn->BeginTransaction();
		if (l_oRequest.m_oBody.m_oAlarm.m_strID.empty())
		{// id为空，新增
			ICC_LOG_DEBUG(m_pLog, "[%s]xxxx AddOrUpdateAlarmAndProcessRequest 2.1.", l_oRequest.m_oBody.m_oAlarm.m_strID.c_str());
			l_AlarmState = ADD;
			ModifyAddSyntRequest(l_oRequest);
			if (l_oRequest.m_oBody.m_oAlarm.m_strReceiptCode.empty() || l_oRequest.m_oBody.m_oAlarm.m_strReceiptDeptCode.empty()) 
			{
				l_bIsSuccess = false;
				tmp_strMsg = "ReceiptDeptCode or ReceiptCode is empty";
			}
			else
			{
				l_oRequest.m_oBody.m_oAlarm.m_strInitialFirstType = l_oRequest.m_oBody.m_oAlarm.m_strFirstType;					// 初始警情类别代码
				l_oRequest.m_oBody.m_oAlarm.m_strInitialSecondType = l_oRequest.m_oBody.m_oAlarm.m_strSecondType;				// 初始警情类型代码
				l_oRequest.m_oBody.m_oAlarm.m_strInitialThirdType = l_oRequest.m_oBody.m_oAlarm.m_strThirdType;					// 初始警情细类代码
				l_oRequest.m_oBody.m_oAlarm.m_strInitialFourthType = l_oRequest.m_oBody.m_oAlarm.m_strFourthType;			    // 初始警情子类代码
				l_oRequest.m_oBody.m_oAlarm.m_strInitialAdminDeptCode = l_oRequest.m_oBody.m_oAlarm.m_strAdminDeptCode;			// 初始管辖单位代码
				l_oRequest.m_oBody.m_oAlarm.m_strInitialAdminDeptName = l_oRequest.m_oBody.m_oAlarm.m_strAdminDeptName;			// 初始管辖单位名称
				l_oRequest.m_oBody.m_oAlarm.m_strInitialAdminDeptOrgCode = l_oRequest.m_oBody.m_oAlarm.m_strAdminDeptOrgCode;   // 初始管辖单位短码
				l_oRequest.m_oBody.m_oAlarm.m_strInitialAddr = l_oRequest.m_oBody.m_oAlarm.m_strAddr;						    // 初始警情地址
				
				if (l_oRequest.m_oBody.m_oAlarm.m_strFirstSubmitTime.empty())
				{
					l_oRequest.m_oBody.m_oAlarm.m_strFirstSubmitTime = m_pDateTime->CurrentDateTimeStr();
				}

				l_bIsSuccess = AddSyntInfo(l_oRequest, l_strTransactionGuid);
				if (!l_bIsSuccess)
				{
					tmp_strMsg = "add synt info failed";
				}
			}	
		}
		else
		{// id 非空，更新
			ICC_LOG_DEBUG(m_pLog, "[%s]xxxx AddOrUpdateAlarmAndProcessRequest 3.0.", l_oRequest.m_oBody.m_oAlarm.m_strID.c_str());
			// 如果更新时间为空或者有新增的处警单，则将状态改为提交
			// 从而刷新对应单位的iccweb警情列表
			if (!IsUpdateTimeNotEmpty(l_oRequest.m_oBody.m_oAlarm.m_strID, l_strTransactionGuid)
				|| IsHaveNewProcess(l_oRequest))
			{
				l_AlarmState = SUBMIT;
			}
			else
			{
				l_AlarmState = EDIT;
			}

			ICC_LOG_DEBUG(m_pLog, "[%s]xxxx AddOrUpdateAlarmAndProcessRequest 3.1, AlarmState[%d]", l_oRequest.m_oBody.m_oAlarm.m_strID.c_str(), l_AlarmState);

			if (l_oRequest.m_oBody.m_strIsEdit == "1" || l_oRequest.m_oBody.m_oAlarm.m_strMsgSource == "vcs_relocated")
			{
				//警单类型变更
				SendMessageToMsgCenterOnAlarmTypeChange(l_oRequest, p_pNotify->GetMessages());
				//更新警单提醒关注警单的警员
				SendMessageToMsgCenterOnAlarmChange(l_oRequest, p_pNotify->GetMessages());
			}

			ModifyUpdateSyntRequest(l_oRequest, l_strTransactionGuid);
			if (l_oRequest.m_oBody.m_oAlarm.m_strReceiptCode.empty() || l_oRequest.m_oBody.m_oAlarm.m_strReceiptDeptCode.empty()) 
			{
				l_bIsSuccess = false;
				tmp_strMsg = "ReceiptDeptCode or ReceiptCode is empty";
			}
			else
			{
				if (_SelectAlarmIdToFirstType(l_oRequest.m_oBody.m_oAlarm.m_strID))
				{
					l_oRequest.m_oBody.m_oAlarm.m_strInitialFirstType = l_oRequest.m_oBody.m_oAlarm.m_strFirstType;					// 初始警情类别代码
					l_oRequest.m_oBody.m_oAlarm.m_strInitialSecondType = l_oRequest.m_oBody.m_oAlarm.m_strSecondType;				// 初始警情类型代码
					l_oRequest.m_oBody.m_oAlarm.m_strInitialThirdType = l_oRequest.m_oBody.m_oAlarm.m_strThirdType;					// 初始警情细类代码
					l_oRequest.m_oBody.m_oAlarm.m_strInitialFourthType = l_oRequest.m_oBody.m_oAlarm.m_strFourthType;			    // 初始警情子类代码
					l_oRequest.m_oBody.m_oAlarm.m_strInitialAdminDeptCode = l_oRequest.m_oBody.m_oAlarm.m_strAdminDeptCode;			// 初始管辖单位代码
					l_oRequest.m_oBody.m_oAlarm.m_strInitialAdminDeptName = l_oRequest.m_oBody.m_oAlarm.m_strAdminDeptName;			// 初始管辖单位名称
					l_oRequest.m_oBody.m_oAlarm.m_strInitialAdminDeptOrgCode = l_oRequest.m_oBody.m_oAlarm.m_strAdminDeptOrgCode;   // 初始管辖单位短码
					l_oRequest.m_oBody.m_oAlarm.m_strInitialAddr = l_oRequest.m_oBody.m_oAlarm.m_strAddr;						    // 初始警情地址
				}
				if(l_oRequest.m_oBody.m_oAlarm.m_strFirstSubmitTime.empty())
				{
					l_oRequest.m_oBody.m_oAlarm.m_strFirstSubmitTime = m_pDateTime->CurrentDateTimeStr();
				}

				l_bIsSuccess = UpdateSyntInfo(l_oRequest, l_strTransactionGuid, tmp_iModifyContent, l_strGuidID);

				if (!l_bIsSuccess)
				{
					tmp_strMsg = "update synt info failed";
				}
			}
			if (l_oRequest.m_oBody.m_oAlarm.m_strMsgSource != "vcs_relocated")
			{
				bUpdateVisit = _UpdateVisitCallType(l_oRequest.m_oBody.m_oAlarm.m_strID, l_oRequest.m_oBody.m_oAlarm.m_strCalledNoType, l_strTransactionGuid);
			}
		}

		//当警情调派存在单位时,根据配置文件选择是否进行自动催办
		if (m_strIsUseAutourge == "1" && !l_oRequest.m_oBody.m_vecProcessData.empty() && l_oRequest.m_oBody.m_oAlarm.m_strMsgSource != "vcs_relocated")
		{
			ICC_LOG_DEBUG(m_pLog, "[%s]Action Autourge", l_oRequest.m_oBody.m_oAlarm.m_strID.c_str());
			AutoUrgeOnUpdateAlarm(l_oRequest.m_oBody);
		}

		//接警完成时间由前端传过来，后端不做处理了，仅没传的时候取当前时间
		//_UpdateAlarmSubmitTime(l_oRequest.m_oBody.m_oAlarm.m_strID, l_strTransactionGuid);

		(l_bIsSuccess && bUpdateVisit) ? (m_pDBConn->Commit(l_strTransactionGuid)) : (m_pDBConn->Rollback(l_strTransactionGuid));
	}

	ICC_LOG_DEBUG(m_pLog, "[%s]xxxx AddOrUpdateAlarmAndProcessRequest 4.0.", l_oRequest.m_oBody.m_oAlarm.m_strID.c_str());
	// 发送回复消息
	PROTOCOL::CAddOrUpdateAlarmWithProcessRespond l_oRespond;
	BuildRespondHeader("add_or_update_alarm_and_process_respond", l_oRequest.m_oHeader, l_oRespond.m_oHeader);
	//l_oRespond.m_oBody.m_strResult = l_bIsSuccess ? "0" : "1";//新增警情成功回复0,否则回复1
	l_oRespond.m_oHeader.m_strResult = l_bIsSuccess ? "0" : "1";//新增警情成功回复0,否则回复1
	l_oRespond.m_oHeader.m_strMsg = tmp_strMsg;
	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());

	if (l_oRequest.m_oBody.m_strIsEdit == "0")
	{
		SendMessageToMsgCenterOnMajorAlarm(l_oRequest, p_pNotify->GetMessages());
	}

	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	// 发送同步消息
	if (l_bIsSuccess)
	{
		SyncSyntInfo(l_oRequest, l_AlarmState);

		//是否需要更新敏感警情状态
		if (_SyncKeyWordAlarmInfo(l_oRequest.m_oBody.m_oAlarm, EN_MODIFY_CONTENT == tmp_iModifyContent))
		{
			_UpdateKeyWordAlarmState(l_oRequest.m_oBody.m_oAlarm.m_strID, l_oRequest.m_oBody.m_oAlarm.m_strState);
		}
	}
	// 更新成功了话，记录一下更新的内容
	if (l_oRequest.m_oBody.m_strIsEdit == "1")
	{
		l_AlarmNewData = GetAlarmDataByAlarmID(l_oRequest.m_oBody.m_oAlarm.m_strID);
		UpdateAlarmLog(l_AlarmSourceValue, l_AlarmNewData, l_strGuidID);
	}
}

void CBusinessImpl::OnCNotifiAddWeChatAlarmRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive OnCNotifiAddWeChatAlarmRequest message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::AddOrUpdateAlarmWithProcessRequestPtr l_oRequest = std::make_shared<PROTOCOL::CAddOrUpdateAlarmWithProcessRequest>();
	if (!l_oRequest->ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}
	std::string l_strWeChatAlarmID = "";

	bool l_bAddWeChat = false;
	l_oRequest->m_oBody.m_oAlarm.m_strSourceID = l_oRequest->m_oBody.m_oAlarm.m_strID;
	l_oRequest->m_oBody.m_oAlarm.m_strAlarmSourceType = "04";
	l_oRequest->m_oBody.m_oAlarm.m_strAlarmSourceId = l_oRequest->m_oBody.m_oAlarm.m_strID;
	if (l_oRequest->m_oBody.m_oAlarm.m_strUpdateUser.empty()) 
	{
		l_oRequest->m_oBody.m_oAlarm.m_strUpdateUser = l_oRequest->m_oBody.m_oAlarm.m_strReceiptCode;
	}
	if (l_oRequest->m_oBody.m_oAlarm.m_strID.empty() || (IsCreateWeChatAlarm(l_oRequest->m_oBody.m_oAlarm.m_strID, l_strWeChatAlarmID) && l_strWeChatAlarmID.empty())) 
	{
		if (!l_oRequest->m_oBody.m_oAlarm.m_strReceiptDeptCode.empty() && l_oRequest->m_oBody.m_oAlarm.m_strReceiptDeptCode.size() > 5)
		{
			l_oRequest->m_oBody.m_oAlarm.m_strID = _GenAlarmID(l_oRequest->m_oBody.m_oAlarm.m_strReceiptDeptCode.substr(0, 6).c_str(), l_oRequest->m_oBody.m_oAlarm.m_strSeatNo, l_oRequest->m_oBody.m_oAlarm.m_strReceiptDeptCode);
			BuildWeChatDataInfo(l_oRequest);
			l_bAddWeChat = true;
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "m_strID or m_strReceiptDeptCode is empty not create alarmid");
			// 发送回复消息
			PROTOCOL::CAddOrUpdateAlarmWithProcessRespond l_oRespond;
			BuildRespondHeader("add_or_update_alarm_and_process_respond", l_oRequest->m_oHeader, l_oRespond.m_oHeader);
			l_oRespond.m_oHeader.m_strResult = "1";
			l_oRespond.m_oHeader.m_strMsg = "m_strID or m_strReceiptDeptCode is empty not create alarmid";
			std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
			p_pNotify->Response(l_strSendMsg);
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
			return;
		}
	}
	else
	{
		
		l_oRequest->m_oBody.m_oAlarm.m_strID = l_strWeChatAlarmID;
	}

	// handle_type_code 为 1表示 无效  2 表示 自处
	if (l_oRequest->m_oBody.m_oAlarm.m_strHandleTypeCode == "1")
	{
		l_oRequest->m_oBody.m_oAlarm.m_strIsInvalid = "1";
		l_oRequest->m_oBody.m_oAlarm.m_strHandleType = "10";
	}
	else if (l_oRequest->m_oBody.m_oAlarm.m_strHandleTypeCode == "2")
	{
		l_oRequest->m_oBody.m_oAlarm.m_strHandleType = "05";
		l_oRequest->m_oBody.m_oAlarm.m_strState = "04";
	}
	else {
		ICC_LOG_ERROR(m_pLog, "we chat HandleTypeCode  is %s", l_oRequest->m_oBody.m_oAlarm.m_strHandleTypeCode.c_str());
		// 发送回复消息
		PROTOCOL::CAddOrUpdateAlarmWithProcessRespond l_oRespond;
		BuildRespondHeader("add_or_update_alarm_and_process_respond", l_oRequest->m_oHeader, l_oRespond.m_oHeader);
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "WeChatAlarm HandleTypeCode error";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}
	l_oRequest->m_oBody.m_oAlarm.m_strCalledNoType = "99";
	l_oRequest->m_oBody.m_oAlarm.m_strSourceType = "06";
	// 车辆的信息需要放在车辆信息表里面
	if (!SyncVehicleNoInfo(l_oRequest)) {
		ICC_LOG_ERROR(m_pLog, "SyncVehicleNoInfo failed message ");
	}

	_ProcessAddOrUpdateAlarmReqeust(l_oRequest, p_pNotify);

	if (l_oRequest->m_oBody.m_oAlarm.m_strHandleTypeCode == "1" || l_oRequest->m_oBody.m_oAlarm.m_strHandleTypeCode == "2")
	{
		boost::thread l_oThreadQueryAlarmCount(boost::bind(&CBusinessImpl::WechatAlarmAttach, this, l_oRequest->m_oBody.m_oAlarm.m_strID, l_bAddWeChat, false));
		//WechatAlarmAttach(l_oRequest->m_oBody.m_oAlarm.m_strID, l_bAddWeChat);
	}
}

bool CBusinessImpl::IsCreateWeChatAlarm(const std::string& strAlarmId, std::string& l_strWeChatAlarmID)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_jjdb";
	l_SqlRequest.param["alarm_source_id"] = strAlarmId;
	std::string strStartTime = m_pDateTime->CurrentDateTimeStr();
	l_SqlRequest.param["jjsj_end"] = strStartTime;
	l_SqlRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strStartTime, 15 * 24 * 60 * 60);
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "IsCreateWeChatAlarm sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (l_pResult->Next())
	{
		l_strWeChatAlarmID = l_pResult->GetValue("id");
	}
	return true;
}

bool CBusinessImpl::SyncVehicleNoInfo(PROTOCOL::AddOrUpdateAlarmWithProcessRequestPtr l_oRequest)
{
	DataBase::SQLRequest l_SqlRequest;
	
	if (l_oRequest->m_oBody.m_oAlarm.m_vecVehicleNo.empty()) {
		return true;
	}
	l_SqlRequest.sql_id = "insert_icc_t_jqclb";
	l_SqlRequest.param["id"] = m_pString->CreateGuid();  // 由于添加警单的车辆信息入库是用不同的接口的 这里的GUID可能不一样
	l_SqlRequest.param["alarm_id"] = l_oRequest->m_oBody.m_oAlarm.m_strID;
	
	
	l_SqlRequest.param["car_type"] = "";
	l_SqlRequest.param["car_user_name"] = l_oRequest->m_oBody.m_oAlarm.m_strCallerName;
	l_SqlRequest.param["car_user_phone"] = l_oRequest->m_oBody.m_oAlarm.m_strCallerNo;
	l_SqlRequest.param["car_user_number"] = l_oRequest->m_oBody.m_oAlarm.m_strCallerID;
	
	l_SqlRequest.param["is_hazardous_vehicle"] = "0";

	l_SqlRequest.param["create_time"] = l_oRequest->m_oBody.m_oAlarm.m_strCreateTime.empty() ? m_pDateTime->CurrentDateTimeStr() : l_oRequest->m_oBody.m_oAlarm.m_strCreateTime;
	l_SqlRequest.param["update_time"] = l_oRequest->m_oBody.m_oAlarm.m_strUpdateTime.empty() ? m_pDateTime->CurrentDateTimeStr() : l_oRequest->m_oBody.m_oAlarm.m_strUpdateTime;
	l_SqlRequest.param["deleted"] = "0";
	l_SqlRequest.param["createTeminal"] = l_oRequest->m_oBody.m_oAlarm.m_strCreateTeminal;
	l_SqlRequest.param["updateTeminal"] = l_oRequest->m_oBody.m_oAlarm.m_strUpdateTeminal;
	l_SqlRequest.param["car_number"] = l_oRequest->m_oBody.m_oAlarm.m_vecVehicleNo[0];
	
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "BuildWeChatDataInfo sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "BuildWeChatDataInfo exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	int size = l_oRequest->m_oBody.m_oAlarm.m_vecVehicleNo.size();
	for (int i = 1; i < size; i++) {
		l_SqlRequest.param["id"] = m_pString->CreateGuid();
		l_SqlRequest.param["car_number"] = l_oRequest->m_oBody.m_oAlarm.m_vecVehicleNo[i];
		l_pResult = m_pDBConn->Exec(l_SqlRequest);
		ICC_LOG_DEBUG(m_pLog, "BuildWeChatDataInfo sql:[%s]", l_pResult->GetSQL().c_str());
		if (!l_pResult->IsValid())
		{
			ICC_LOG_DEBUG(m_pLog, "BuildWeChatDataInfo exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		}
	}
	return true;

}


std::string CBusinessImpl::SearchReceiptDeptDistrictCodeByCode(const std::string& m_strReceiptDeptCode)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_jjdb";
	/*l_SqlRequest.param["source_id"] = strAlarmId;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "IsCreateWeChatAlarm sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (l_pResult->Next())
	{
		l_strWeChatAlarmID = l_pResult->GetValue("id");
	}*/
	return m_strReceiptDeptCode;
}

void CBusinessImpl::BuildWeChatDataInfo(PROTOCOL::AddOrUpdateAlarmWithProcessRequestPtr l_oRequest)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "insert_icc_t_jjdb";
	l_SqlRequest.param["receipt_dept_code"] = l_oRequest->m_oBody.m_oAlarm.m_strReceiptDeptCode;
	l_SqlRequest.param["id"] = l_oRequest->m_oBody.m_oAlarm.m_strID;
	l_SqlRequest.param["source_id"] = l_oRequest->m_oBody.m_oAlarm.m_strSourceID;
	l_SqlRequest.param["called_no_type"] = "99"; // l_oRequest->m_oBody.m_oAlarm.m_strCalledNoType;  // 99
	l_SqlRequest.param["source_type"] = "06";	// l_oRequest->m_oBody.m_oAlarm.m_strSourceType;		// 06
	l_SqlRequest.param["receipt_code"] = l_oRequest->m_oBody.m_oAlarm.m_strReceiptCode;
	l_SqlRequest.param["receipt_name"] = l_oRequest->m_oBody.m_oAlarm.m_strReceiptName;
	l_SqlRequest.param["create_time"] = l_oRequest->m_oBody.m_oAlarm.m_strCreateTime.empty() ? m_pDateTime->CurrentDateTimeStr() : l_oRequest->m_oBody.m_oAlarm.m_strCreateTime;
	l_SqlRequest.param["update_time"] = l_oRequest->m_oBody.m_oAlarm.m_strUpdateTime.empty() ? m_pDateTime->CurrentDateTimeStr() : l_oRequest->m_oBody.m_oAlarm.m_strUpdateTime;
	l_SqlRequest.param["receiving_time"] = l_oRequest->m_oBody.m_oAlarm.m_strTime;
	l_SqlRequest.param["received_time"] = l_oRequest->m_oBody.m_oAlarm.m_strReceivedTime;
	l_SqlRequest.param["caller_name"] = l_oRequest->m_oBody.m_oAlarm.m_strCallerName;
	l_SqlRequest.param["contact_no"] = l_oRequest->m_oBody.m_oAlarm.m_strContactNo;
	l_SqlRequest.param["caller_no"] = l_oRequest->m_oBody.m_oAlarm.m_strCallerNo;
	l_SqlRequest.param["addr"] = l_oRequest->m_oBody.m_oAlarm.m_strAddr;
	l_SqlRequest.param["content"] = l_oRequest->m_oBody.m_oAlarm.m_strContent;
	l_SqlRequest.param["first_type"] = l_oRequest->m_oBody.m_oAlarm.m_strFirstType;
	l_SqlRequest.param["second_type"] = l_oRequest->m_oBody.m_oAlarm.m_strSecondType;
	l_SqlRequest.param["third_type"] = l_oRequest->m_oBody.m_oAlarm.m_strThirdType;
	l_SqlRequest.param["fourth_type"] = l_oRequest->m_oBody.m_oAlarm.m_strFourthType;
	l_SqlRequest.param["longitude"] = l_oRequest->m_oBody.m_oAlarm.m_strLongitude;
	l_SqlRequest.param["latitude"] = l_oRequest->m_oBody.m_oAlarm.m_strLatitude;
	l_SqlRequest.param["vehicle_no"] = l_oRequest->m_oBody.m_oAlarm.m_strVehicleNo;
	l_SqlRequest.param["is_privacy"] = l_oRequest->m_oBody.m_oAlarm.m_strPrivacy;
	l_SqlRequest.param["caller_gender"] = l_oRequest->m_oBody.m_oAlarm.m_strCallerGender;
	l_SqlRequest.param["caller_id_type"] = l_oRequest->m_oBody.m_oAlarm.m_strCallerIDType;
	l_SqlRequest.param["caller_id"] = l_oRequest->m_oBody.m_oAlarm.m_strCallerID;
	l_SqlRequest.param["handle_type"] = l_oRequest->m_oBody.m_oAlarm.m_strHandleType;
	l_SqlRequest.param["level"] = l_oRequest->m_oBody.m_oAlarm.m_strLevel;
	l_SqlRequest.param["is_visitor"] = "0";
	l_SqlRequest.param["alarm_source_type"] = l_oRequest->m_oBody.m_oAlarm.m_strAlarmSourceType;
	l_SqlRequest.param["alarm_source_id"] = l_oRequest->m_oBody.m_oAlarm.m_strAlarmSourceId;
	l_SqlRequest.param["update_user"] = l_oRequest->m_oBody.m_oAlarm.m_strUpdateUser;
	l_SqlRequest.param["is_delete"] = "0";
	l_SqlRequest.param["is_feedback"] = "0";
	l_SqlRequest.param["is_invalid"] = "0";
	l_SqlRequest.param["had_push"] = "0";
	l_SqlRequest.param["is_merge"] = "0";
	l_SqlRequest.param["is_over"] = "0";
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "BuildWeChatDataInfo sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "BuildWeChatDataInfo exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
	}
}

void CBusinessImpl::OnCNotifiAddOrUpdateAlarmAndProcessRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::AddOrUpdateAlarmWithProcessRequestPtr l_oRequest = std::make_shared<PROTOCOL::CAddOrUpdateAlarmWithProcessRequest>();
	if (!l_oRequest->ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}
	int size = l_oRequest->m_oBody.m_vecProcessData.size();
	for (int i = 0; i < size; i++)
	{
		if (l_oRequest->m_oBody.m_vecProcessData[i].m_strUpdateTime.empty())
		{
			l_oRequest->m_oBody.m_vecProcessData[i].m_strUpdateTime = m_pDateTime->CurrentDateTimeStr();
		}
	}
	int iAlarmId = _GetAlarmId(l_oRequest->m_oBody.m_oAlarm.m_strID);
	CommonWorkThreadPtr pThread = _GetThreadEx(iAlarmId);
	if (pThread)
	{
		pThread->AddMessage(this, THREADID_RECEIVEALARMNOTIFY, 0, 0, 0, "add_or_update_alarm_and_process_request", "", p_pNotify, nullptr, nullptr, l_oRequest);
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "add or update alarm  message failed: alarm id: [%s:%d:%s], ", l_oRequest->m_oBody.m_oAlarm.m_strID.c_str(), iAlarmId, p_pNotify->GetCmdGuid().c_str());
	}

	ICC_LOG_DEBUG(m_pLog, "add or update alarm  message: alarm id: [%s:%d], ", l_oRequest->m_oBody.m_oAlarm.m_strID.c_str(), iAlarmId);
}

void CBusinessImpl::OnGetAlarmIDRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CGetAlarmIDRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	PROTOCOL::CAddOrUpdateAlarmWithProcessRespond l_oRespond;
	l_oRespond.m_oHeader = l_oRequest.m_oHeader;
	l_oRespond.m_oBody.m_strID = _GenAlarmID(l_oRequest.m_oBody.m_strReceiptDeptDistrictCode, l_oRequest.m_oHeader.m_strSeatNo);
	l_oRespond.m_oBody.m_strTime = m_pDateTime->CurrentDateTimeStr();

	std::string tmp_strMsg(l_oRespond.ToString4AddAlarm(m_pJsonFty->CreateJson()));
	p_pNotify->Response(tmp_strMsg);

	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", tmp_strMsg.c_str());
}


void CBusinessImpl::OnAlarmDeleteRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	PROTOCOL::CDeleteAlarmRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}


	PROTOCOL::CHeaderEx l_oRespond;

	l_oRespond = l_oRequest.m_oHeader;

	std::string tmp_strTransGuid;

	bool tmp_bSuccess = false;

	PROTOCOL::CAddOrUpdateAlarmWithProcessRequest l_oAlarmRequest;
	do
	{
		//警情ID不能为空
		if (l_oRequest.m_oBody.m_strAlarmID.empty())
		{
			l_oRespond.m_strResult = "1";
			l_oRespond.m_strMsg = "alarmid is empty";
			break;
		}

		//找到告警
		if (!GetAlarm(l_oRequest.m_oBody.m_strAlarmID, l_oAlarmRequest))
		{
			l_oRespond.m_strResult = "2";
			l_oRespond.m_strMsg = "not find alarmid[" + l_oRequest.m_oBody.m_strAlarmID +"]";
			break;
		}

	    //如果状态不对，不允许删, callednotype 03自接警，警单状态为自接接
		if (l_oAlarmRequest.m_oBody.m_oAlarm.m_strCalledNoType != "03" && l_oAlarmRequest.m_oBody.m_oAlarm.m_strState != "00")
		{
			l_oRespond.m_strResult = "3";
			l_oRespond.m_strMsg = "alarm can not cancel, callnotype[" + l_oAlarmRequest.m_oBody.m_oAlarm.m_strCalledNoType + "] "+"state["+ l_oAlarmRequest.m_oBody.m_oAlarm.m_strState+"]";
			break;
		}

		l_oAlarmRequest.m_oBody.m_oAlarm.m_strDeleteFlag = "1"; //软删除

		tmp_strTransGuid = m_pDBConn->BeginTransaction();

		std::string l_strReceiptResourceID = m_pString->CreateGuid();
		int tmp_iModifield = 0;
		if (InsertAlarmBookBeforeUpdate(l_oAlarmRequest.m_oBody.m_oAlarm, l_strReceiptResourceID, tmp_strTransGuid, tmp_iModifield))
		{//更新该接警数据

			if (!UpdateAlarmInfo(l_oAlarmRequest.m_oBody, tmp_strTransGuid))
			{
				ICC_LOG_ERROR(m_pLog, "UpdateAlarmInfo failed.");
				l_oRespond.m_strResult = "4";
				l_oRespond.m_strMsg = "update alarm failed";
				break;
			}
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "backup failed.");
			l_oRespond.m_strResult = "5";
			l_oRespond.m_strMsg = "insert alarmbook failed";
			break;
		}

		tmp_bSuccess = true;

	} while (0);

	if (tmp_bSuccess)
	{
		m_pDBConn->Commit(tmp_strTransGuid);
	}
	else if (!tmp_strTransGuid.empty())
	{
		m_pDBConn->Rollback(tmp_strTransGuid);
	}


	//回复消息
	JsonParser::IJsonPtr tmp_spJson = m_pJsonFty->CreateJson();
	l_oRespond.SaveTo(tmp_spJson);

	std::string tmp_strMsg(tmp_spJson->ToString());
	p_pNotify->Response(tmp_strMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", tmp_strMsg.c_str());

	// 发送同步消息
	if (tmp_bSuccess)
	{
		SyncSyntInfo(l_oAlarmRequest, DEL);
	}
}

void CBusinessImpl::OnAlarmViolationCheckTimer(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	//_AlarmViolationCheckProcess();
	if (!m_bViolationIsRunning)
	{
		m_threadViolation.AddMessage(this, ThreadId_Violation);
		ICC_LOG_INFO(m_pLog, "violation is not running, add!!!");
	}
	else
	{
		ICC_LOG_INFO(m_pLog, "violation is running, not add!!!");
	}
}

void CBusinessImpl::_AddAlarmViolationCheck(const std::string& strID, std::string strCreateTime)
{
	std::string l_strAlarmTimeout;
	if (m_pRedisClient->HGet("Violation_TimeoutValue", "AlarmTimeout", l_strAlarmTimeout))
	{
		if (!l_strAlarmTimeout.empty())
		{
			m_nAlarmViolationTimeout = std::atoi(l_strAlarmTimeout.c_str());
		}
	}

	if (m_nAlarmViolationTimeout <= 0)
	{
		return;
	}
	if (strID.empty())
	{
		return;
	}
	ICC::DateTime::CDateTime l_startTime = m_pDateTime->FromString(strCreateTime);
	unsigned int l_strTimeCount = m_pDateTime->SecondsDifference(l_startTime, m_pDateTime->CurrentDateTime());
	std::lock_guard<std::mutex> guard(m_violationCheckMutex);
	std::map<std::string, int>::iterator it = m_mAlarmViolationCheckList.find(strID);
	if (it != m_mAlarmViolationCheckList.end() || l_strTimeCount >= m_nAlarmViolationTimeout)
	{
		ICC_LOG_DEBUG(m_pLog, "AlarmViolation Check already exists:[%s], cur time count: %u", strID.c_str(), l_strTimeCount);
		return;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "add AlarmViolation Check strID:[%s],AlarmViolationTimeout=%d, cur time count: %u", strID.c_str(), m_nAlarmViolationTimeout, l_strTimeCount);
	}
	m_mAlarmViolationCheckList[strID] = 0;
}

void CBusinessImpl::_DeleteAlarmWiolationCheck(const std::string& strID)
{
	std::lock_guard<std::mutex> guard(m_violationCheckMutex);
	std::map<std::string, int>::iterator it = m_mAlarmViolationCheckList.find(strID);
	if (it != m_mAlarmViolationCheckList.end())
	{
		m_mAlarmViolationCheckList.erase(it);
	}
}

void CBusinessImpl::_AlarmViolationCheckProcess()
{
	std::vector<std::string> CurrViolationList;
	CurrViolationList.clear();
	{
		std::lock_guard<std::mutex> guard(m_violationCheckMutex);
		auto it = m_mAlarmViolationCheckList.begin();
		while (it != m_mAlarmViolationCheckList.end())
		{
			it->second++;
			if (it->second > m_nAlarmViolationTimeout)
			{
				std::string strKey = it->first;
				CurrViolationList.push_back(strKey);
				it = m_mAlarmViolationCheckList.erase(it);
				break;
			}
			++it;
		}
	}
	

	for (int i = 0; i < CurrViolationList.size(); i++)
	{
		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = "select_icc_t_jjdb";
		l_SqlRequest.param["id"] = CurrViolationList[i];

		std::string strTime = m_pDateTime->GetAlarmIdTime(CurrViolationList[i]);
		if (strTime != "")
		{
			l_SqlRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
			l_SqlRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
		}

		DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
		if (!l_pResult->IsValid())
		{
			ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		}
		else
		{
			if (l_pResult->Next())
			{
				std::string l_strState = l_pResult->GetValue("state");
				std::string l_strSeatNo = l_pResult->GetValue("receipt_seatno");
				std::string l_strReceiptName = l_pResult->GetValue("receipt_name");
				std::string l_strReceiptDeptCode = l_pResult->GetValue("receipt_dept_code");
				if (l_strState == "00")
				{
					ReportAlarmViolation(CurrViolationList[i], l_strSeatNo, l_strReceiptName, l_strReceiptDeptCode);
				}
			}
		}
	}
}

void CBusinessImpl::ReportAlarmViolation(const std::string &strID,const std::string & p_strSeatNo, const std::string& p_strReceiptName, const std::string& p_strReceiptDeptCode)
{
	DataBase::SQLRequest l_strViolationSql;
	std::string l_strGUID = m_pString->CreateGuid();
	std::string l_strCurrentTime = m_pDateTime->CurrentDateTimeStr();
	std::string l_strTimeOutValue = m_pString->Format("%d", m_nAlarmViolationTimeout);
	std::string l_strVoilationType = "DIC050004";
	l_strViolationSql.sql_id = "insert_icc_t_violation";
	l_strViolationSql.param.insert(std::pair<std::string, std::string>("guid", l_strGUID));
	l_strViolationSql.param.insert(std::pair<std::string, std::string>("create_time", l_strCurrentTime));
	l_strViolationSql.param.insert(std::pair<std::string, std::string>("timeout_value", l_strTimeOutValue));
	l_strViolationSql.param.insert(std::pair<std::string, std::string>("violation_type", l_strVoilationType));
	//l_strViolationSql.param.insert(std::pair<std::string, std::string>("violation_target", p_strSeatNo));  //violation_target对象填警单号
	l_strViolationSql.param.insert(std::pair<std::string, std::string>("violation_target", strID));
	l_strViolationSql.param.insert(std::pair<std::string, std::string>("violation_user", p_strReceiptName));
	l_strViolationSql.param.insert(std::pair<std::string, std::string>("dept_code", p_strReceiptDeptCode));

	l_strViolationSql.param.insert(std::pair<std::string, std::string>("violation_id", l_strGUID));
	//l_strViolationSql.param.insert(std::pair<std::string, std::string>("update_time", p_strCurrentTime));
	l_strViolationSql.param.insert(std::pair<std::string, std::string>("violation_time", l_strCurrentTime));

	DataBase::IResultSetPtr l_pSqlResult;
	l_pSqlResult = m_pDBConn->Exec(l_strViolationSql);
	if (!l_pSqlResult->IsValid())
	{
		std::string l_strSQL = l_pSqlResult->GetSQL();
		ICC_LOG_DEBUG(m_pLog, "%s table failed, SQL: [%s], Error: [%s]",
			l_strViolationSql.sql_id.c_str(),
			l_strSQL.c_str(),
			l_pSqlResult->GetErrorMsg().c_str());

		return;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "%s table success, SQL: [%s]",l_strViolationSql.sql_id.c_str(),l_pSqlResult->GetSQL().c_str());
		SendMessageToMsgCenterOnAlarmOverTime(p_strSeatNo, strID, p_strReceiptName, p_strReceiptDeptCode);
	}
	//发同步消息
	PROTOCOL::CViolationSync l_CViolationSync;
	l_CViolationSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_CViolationSync.m_oHeader.m_strCmd = "topic_violation_sync";
	l_CViolationSync.m_oHeader.m_strRequest = "topic_violation";
	l_CViolationSync.m_oHeader.m_strRequestType = "1";//主题
	l_CViolationSync.m_oHeader.m_strSendTime = l_strCurrentTime;

	l_CViolationSync.m_oBody.m_strGUID = l_strGUID;
	l_CViolationSync.m_oBody.m_strTimeOutValue = l_strTimeOutValue;
	l_CViolationSync.m_oBody.m_strVoilationType = l_strVoilationType;
	//l_CViolationSync.m_oBody.m_strViolationTarget = p_strSeatNo; //violation_target对象填警单号
	l_CViolationSync.m_oBody.m_strViolationTarget = strID;
	l_CViolationSync.m_oBody.m_strViolationUser = p_strReceiptName;
	l_CViolationSync.m_oBody.m_strViolationTime = l_strCurrentTime;
	l_CViolationSync.m_oBody.m_strDeptCode = p_strReceiptDeptCode;
	l_CViolationSync.m_oBody.m_strViolationID = l_strGUID;
	l_CViolationSync.m_oBody.m_strSyncType = "1";

	std::string l_strSyncMessage = l_CViolationSync.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));

	ICC_LOG_DEBUG(m_pLog, "Send Violation Sync: [%s]", l_strSyncMessage.c_str());
}
void CBusinessImpl::SendMessageToMsgCenterOnAlarmChange(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest l_oRequest, std::string l_strMsg)
{
	//更新警单提醒关注警单的警员

	std::string strParamInfo;
	if (!m_pRedisClient->HGet(PARAM_INFO, "AlarmChange", strParamInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "Hget ParamInfo Failed!!!");
		return;
	}
	PROTOCOL::CParamInfo l_ParamInfo;
	if (!l_ParamInfo.Parse(strParamInfo, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "Json Parse ParamInfo Failed!!!");
		return;
	}
	std::string strValue = l_ParamInfo.m_strValue;

	MsgCenter::MessageSendDTOData l_CDTOData;
	std::string l_curSendMsg;
	l_CDTOData.appCode = "icc";
	l_CDTOData.businessCode = "alarminfo";
	l_CDTOData.compensateType = 0;
	l_CDTOData.delayDuration = "";
	l_CDTOData.delayType = 0;
	l_CDTOData.title = m_pString->ReplaceFirst(strValue, "$", l_oRequest.m_oBody.m_oAlarm.m_strID);
	l_CDTOData.moduleCode = "alarminfo";
	l_CDTOData.needStorage = "true";
	l_CDTOData.sendType = "0";
	l_CDTOData.sync = "true";
	l_CDTOData.message = l_oRequest.m_oBody.m_oAlarm.m_strID;

	DataBase::SQLRequest l_strDeptSql;
	l_strDeptSql.sql_id = "select_icc_t_user_all";
	l_strDeptSql.param["alarm_id"] = l_oRequest.m_oBody.m_oAlarm.m_strID;
	DataBase::IResultSetPtr l_sqlResult = m_pDBConn->Exec(l_strDeptSql);
	if (!l_sqlResult->IsValid())
	{
		std::string l_strSQL = l_sqlResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "%s table failed, SQL: [%s], Error: [%s]",
			l_strDeptSql.sql_id.c_str(),
			l_strSQL.c_str(),
			l_sqlResult->GetErrorMsg().c_str());

		return;
	}

	MsgCenter::ReceiveObject l_recObj;
	l_recObj.type = "user";

	while (l_sqlResult->Next())
	{
		ICC_LOG_ERROR(m_pLog, "select value from icc_t_user_all [%s]",
			l_sqlResult->GetValue("guid").c_str());

		l_recObj.noticeList.push_back(l_sqlResult->GetValue("guid"));
		l_recObj.syncList.push_back(l_sqlResult->GetValue("guid"));
	}
	l_CDTOData.receiveObjects.push_back(l_recObj);

	if (l_CDTOData.receiveObjects[0].noticeList.size() == 0)
	{
		ICC_LOG_DEBUG(m_pLog, "No one pays attention to the current alarm list: [%s]", l_curSendMsg.c_str());
		return;
	}

	m_pMsgCenter->BuildSendMsg(l_curSendMsg, l_CDTOData);
	m_pMsgCenter->Send(l_curSendMsg, m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName);
}

void CBusinessImpl::SendMessageToMsgCenterOnAlarmTypeChange(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest l_oRequest, std::string l_strMsg)
{
	DataBase::SQLRequest l_strDeptSql;
	l_strDeptSql.sql_id = "select_icc_t_jjdb_type";
	l_strDeptSql.param["alarm_id"] = l_oRequest.m_oBody.m_oAlarm.m_strID;

	std::string strTime = m_pDateTime->GetAlarmIdTime(l_oRequest.m_oBody.m_oAlarm.m_strID);
	if (strTime != "")
	{
		l_strDeptSql.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_strDeptSql.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}

	DataBase::IResultSetPtr l_sqlResult = m_pDBConn->Exec(l_strDeptSql);

	if (!l_sqlResult->IsValid())
	{
		std::string l_strSQL = l_sqlResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "%s table failed, SQL: [%s], Error: [%s]",
			l_strDeptSql.sql_id.c_str(),
			l_strSQL.c_str(),
			l_sqlResult->GetErrorMsg().c_str());

		return;
	}
	else if (l_sqlResult->Next())
	{
		if (l_sqlResult->GetValue("jqlbdm") == l_oRequest.m_oBody.m_oAlarm.m_strFirstType && l_sqlResult->GetValue("jqlxdm") == l_oRequest.m_oBody.m_oAlarm.m_strSecondType && l_sqlResult->GetValue("jqxldm") == l_oRequest.m_oBody.m_oAlarm.m_strThirdType && l_sqlResult->GetValue("jqzldm") == l_oRequest.m_oBody.m_oAlarm.m_strFourthType || ((l_sqlResult->GetValue("jqlbdm") == "") && l_sqlResult->GetValue("jqlxdm") == "" && l_sqlResult->GetValue("jqxldm") == "" && l_sqlResult->GetValue("jqzldm") == ""))
		{
			ICC_LOG_DEBUG(m_pLog, "sql success, but all type is equal so return ,SQL: [%s]",
				l_sqlResult->GetSQL().c_str());
			return;
		}
		else
		{
			ICC_LOG_DEBUG(m_pLog, "sql success, type change, SQL: [%s]; cur jqlbdm: %s,  cur jqlbdm: %s,  cur jqlxdm: %s,  cur jqzldm: %s; after jqlbdm: %s,  after jqlbdm: %s,  after jqlxdm: %s,  after jqzldm: %s",
				l_sqlResult->GetSQL().c_str(),
				l_oRequest.m_oBody.m_oAlarm.m_strFirstType.c_str(), 
				l_oRequest.m_oBody.m_oAlarm.m_strSecondType.c_str(),
				l_oRequest.m_oBody.m_oAlarm.m_strThirdType.c_str(),
				l_oRequest.m_oBody.m_oAlarm.m_strFourthType.c_str(),
				l_sqlResult->GetValue("jqlbdm").c_str(),
				l_sqlResult->GetValue("jqlxdm").c_str(),
				l_sqlResult->GetValue("jqxldm").c_str(),
				l_sqlResult->GetValue("jqzldm").c_str()
				);
		}
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "sql success, unknown, SQL: [%s]",
			l_sqlResult->GetSQL().c_str());
	}

	std::string strName = "AlarmTypeChange";
	std::string strParamInfo;
	if (!m_pRedisClient->HGet(PARAM_INFO, strName, strParamInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "HGet Param INFO Failed!!!");
		return ;
	}
	PROTOCOL::CParamInfo l_ParamInfo;
	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	if (!l_ParamInfo.Parse(strParamInfo, l_pJson))
	{
		ICC_LOG_DEBUG(m_pLog, "Parse Param Info failed!!!");
		return ;
	}

	MsgCenter::MessageSendDTOData l_CDTOData;
	std::string l_curSendMsg;
	l_CDTOData.appCode = "icc";
	l_CDTOData.businessCode = "alarminfo";
	l_CDTOData.compensateType = 0;
	l_CDTOData.delayDuration = "";
	l_CDTOData.delayType = 0;
	l_CDTOData.title = m_pString->ReplaceFirst(l_ParamInfo.m_strValue, "$", l_oRequest.m_oBody.m_oAlarm.m_strID);
	l_CDTOData.moduleCode = "alarminfo";
	l_CDTOData.needStorage = "true";
	l_CDTOData.sendType = "0";
	l_CDTOData.sync = "true";
	l_CDTOData.message = l_oRequest.m_oBody.m_oAlarm.m_strID;

	l_strDeptSql.sql_id = "select_icc_t_staff_by_name";
	l_strDeptSql.param["name"] = l_oRequest.m_oBody.m_oAlarm.m_strReceiptName;
	l_strDeptSql.param["code"] = l_oRequest.m_oBody.m_oAlarm.m_strReceiptCode;
	DataBase::IResultSetPtr l_sqlNoticeResult = m_pDBConn->Exec(l_strDeptSql);
	if (!l_sqlNoticeResult->IsValid())
	{
		std::string l_strSQL = l_sqlNoticeResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "%s table failed, SQL: [%s], Error: [%s]",
			l_strDeptSql.sql_id.c_str(),
			l_strSQL.c_str(),
			l_sqlNoticeResult->GetErrorMsg().c_str());

		return;
	}
	else if (l_sqlNoticeResult->Next())
	{
		ICC_LOG_DEBUG(m_pLog, "sql success, guid = %s ,SQL: [%s]",
			l_sqlNoticeResult->GetValue("guid").c_str(),
			l_sqlNoticeResult->GetSQL().c_str());
	}

	MsgCenter::ReceiveObject l_recObj;
	l_recObj.type = "user";

	{
		l_recObj.noticeList.push_back(l_sqlNoticeResult->GetValue("guid"));
		l_recObj.syncList.push_back(l_sqlNoticeResult->GetValue("guid"));
	}

	l_CDTOData.receiveObjects.push_back(l_recObj);

	if (l_CDTOData.receiveObjects[0].noticeList.size() == 0)
	{
		ICC_LOG_DEBUG(m_pLog, "No one pays attention to the current alarm list: [%s]", l_curSendMsg.c_str());
		return;
	}

	m_pMsgCenter->BuildSendMsg(l_curSendMsg, l_CDTOData);
	m_pMsgCenter->Send(l_curSendMsg, m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName);
}



void ICC::CBusinessImpl::_UpdateKeyWordAlarmState(const std::string& strAlarmID, const std::string& strState)
{
	DataBase::SQLRequest l_tSQLRequest;
	l_tSQLRequest.sql_id = "update_icc_t_keyword_alarm";
	l_tSQLRequest.param["alarm_id"] = strAlarmID;
	l_tSQLRequest.param["state"] = strState;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLRequest, true);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());

	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update_icc_t_keyword_alarm failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return;
	}
}

void ICC::CBusinessImpl::_DelKeyWordAlarmInfoByAlarmID(const std::string& in_strAlarmID)
{
	DataBase::SQLRequest l_tSQLRequest;
	l_tSQLRequest.sql_id = "delete_icc_t_keyword_alarm";
	l_tSQLRequest.param["alarm_id"] = in_strAlarmID;


	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLRequest, true);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());

	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert_icc_t_keyword_alarm failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return;
	}
}

bool ICC::CBusinessImpl::_QueryKeyWordAlarmInfoByAlarmID(const std::string& in_strAlarmID, std::vector<PROTOCOL::CKeyWordAlarm>& out_vecKeyWordAlarm)
{
	DataBase::SQLRequest l_tSQLRequest;
	l_tSQLRequest.sql_id = "select_icc_t_keyword_alarm";
	l_tSQLRequest.param["alarm_id"] = in_strAlarmID;

	l_tSQLRequest.param["orderby"] = "create_time desc";


	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());

	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_keyword_alarm failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}


	PROTOCOL::CKeyWordAlarm tmp_oKeyWordAlarm;
	tmp_oKeyWordAlarm.m_strAlarmID = in_strAlarmID;

	while (l_pRSet->Next())
	{
		tmp_oKeyWordAlarm.m_strGuid = l_pRSet->GetValue("guid");
		
		tmp_oKeyWordAlarm.m_strKeyWord = l_pRSet->GetValue("key_word");
		tmp_oKeyWordAlarm.m_strKeyContent = l_pRSet->GetValue("key_word_content");
		tmp_oKeyWordAlarm.m_strAlarmContent = l_pRSet->GetValue("content");
		tmp_oKeyWordAlarm.m_strDeptCode = l_pRSet->GetValue("dept_code");
		out_vecKeyWordAlarm.push_back(tmp_oKeyWordAlarm);
	}

	return true;
}

void ICC::CBusinessImpl::_InsertKeyWordAlarm(const PROTOCOL::CKeyWordAlarmSync& in_oKeyWordAlarm, const std::string& in_strState)
{
	DataBase::SQLRequest l_tSQLRequest;
	l_tSQLRequest.sql_id = "insert_icc_t_keyword_alarm";
	l_tSQLRequest.param["guid"] = in_oKeyWordAlarm.m_oBody.m_oKeyWordAlarm.m_strGuid;
	l_tSQLRequest.param["alarm_id"] = in_oKeyWordAlarm.m_oBody.m_oKeyWordAlarm.m_strAlarmID;
	l_tSQLRequest.param["key_word"] = in_oKeyWordAlarm.m_oBody.m_oKeyWordAlarm.m_strKeyWord;
	l_tSQLRequest.param["key_word_content"] = in_oKeyWordAlarm.m_oBody.m_oKeyWordAlarm.m_strKeyContent;
	l_tSQLRequest.param["content"] = in_oKeyWordAlarm.m_oBody.m_oKeyWordAlarm.m_strAlarmContent;
	l_tSQLRequest.param["dept_code"] = in_oKeyWordAlarm.m_oBody.m_oKeyWordAlarm.m_strDeptCode;
	l_tSQLRequest.param["state"] = in_strState;
	//l_tSQLRequest.param["create_user"] = strContent;
	l_tSQLRequest.param["create_time"] = in_oKeyWordAlarm.m_oHeader.m_strSendTime;


	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLRequest, true);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());

	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert_icc_t_keyword_alarm failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return;
	}
}

bool ICC::CBusinessImpl::_SyncKeyWordAlarmInfo(const PROTOCOL::CAlarmInfo& in_oAlarm,  bool in_bModifyContent)
{

	//先把警情内容删了
	do
	{

		std::vector<PROTOCOL::CKeyWordAlarm> tmp_vecKeyWordAlarm;
		//失败了，不做任务事件
		if (!_QueryKeyWordAlarmInfoByAlarmID(in_oAlarm.m_strID, tmp_vecKeyWordAlarm))
		{
			return false;
		}

		//如果没有记录，则继续处理；
		if (tmp_vecKeyWordAlarm.empty())
		{
			break;
		}

		//如果有记录，但不是修改警情内容则需要继续下面的逻辑, 但有可能需要修改警情状态
		if (!in_bModifyContent)
		{
			return true; //只有这个返回需要更新状态
		}

		//std::vector<PROTOCOL::CKeyWordAlarm> tmp_vecKeyWordAlarm;

	
		//调删除
		_DelKeyWordAlarmInfoByAlarmID(in_oAlarm.m_strID);
		
		PROTOCOL::CKeyWordAlarmSync tmp_oAlarmSync;
		
		tmp_oAlarmSync.m_oBody.m_strSyncType = "3"; //删除
		tmp_oAlarmSync.m_oHeader.m_strCmd = "topic_alarm_notice_sync";
		tmp_oAlarmSync.m_oHeader.m_strRequest = "topic_alarm_notice_sync";
		tmp_oAlarmSync.m_oHeader.m_strRequestType = "1";

		for (auto tmp_oData : tmp_vecKeyWordAlarm)
		{
			tmp_oAlarmSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
			tmp_oAlarmSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();

			tmp_oAlarmSync.m_oBody.m_oKeyWordAlarm.m_strGuid = tmp_oData.m_strGuid;
			tmp_oAlarmSync.m_oBody.m_oKeyWordAlarm.m_strDeptCode = tmp_oData.m_strDeptCode;

			JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
			std::string l_strMessage = tmp_oAlarmSync.ToString(l_pIJson);
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
		}
	} while(0);
	
	
	if (in_oAlarm.m_strContent.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "alarm: [%s] content is empty", in_oAlarm.m_strID.c_str());
		return false;
	}

	DataBase::SQLRequest l_tSQLRequest;

	l_tSQLRequest.sql_id = "query_icc_t_keyword_by_alarm_content";
	l_tSQLRequest.param["content"] = in_oAlarm.m_strContent;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());

	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "query_icc_t_keyword_by_alarm_content failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}

	PROTOCOL::CKeyWordAlarmSync tmp_oAlarmSync;
	tmp_oAlarmSync.m_oBody.m_oKeyWordAlarm.m_strAlarmID = in_oAlarm.m_strID;
	tmp_oAlarmSync.m_oBody.m_oKeyWordAlarm.m_strAlarmContent = in_oAlarm.m_strContent;
	tmp_oAlarmSync.m_oBody.m_strSyncType = "1"; //新增
	tmp_oAlarmSync.m_oHeader.m_strCmd = "topic_alarm_notice_sync";
	tmp_oAlarmSync.m_oHeader.m_strRequest = "topic_alarm_notice_sync";
	tmp_oAlarmSync.m_oHeader.m_strRequestType = "1";

	tmp_oAlarmSync.m_oBody.m_oKeyWordAlarm.m_strDeptCode = in_oAlarm.m_strReceiptDeptCode; //赋值处警部门编码

	while (l_pRSet->Next())
	{
		tmp_oAlarmSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		tmp_oAlarmSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		tmp_oAlarmSync.m_oBody.m_oKeyWordAlarm.m_strGuid = tmp_oAlarmSync.m_oHeader.m_strMsgid;
		tmp_oAlarmSync.m_oBody.m_oKeyWordAlarm.m_strKeyWord = l_pRSet->GetValue("key_word");
		tmp_oAlarmSync.m_oBody.m_oKeyWordAlarm.m_strKeyContent = l_pRSet->GetValue("content");
		

		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMessage = tmp_oAlarmSync.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());

		_InsertKeyWordAlarm(tmp_oAlarmSync, in_oAlarm.m_strState);
	}
	
	return false;
}

bool ICC::CBusinessImpl::_UpdateAlarmSubmitTime(const std::string& strAlarmId, const std::string& strTransGuid)
{
	DataBase::SQLRequest l_tSQLRequest;
	l_tSQLRequest.sql_id = "update_icc_t_jjdb_submittime_by_alarm_id";
	l_tSQLRequest.param["first_submit_time"] = m_pDateTime->CurrentDateTimeStr();
	l_tSQLRequest.param["alarm_id"] = strAlarmId;

	std::string strTime = m_pDateTime->GetAlarmIdTime(strAlarmId);
	if (strTime != "")
	{
		l_tSQLRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_tSQLRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLRequest, false, strTransGuid);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());

	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update alarm submit time failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}

	return true;
}

void ICC::CBusinessImpl::OnNotifiGetAlarmLogInfo(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	std::string l_strRecvMsg(p_pNotify->GetMessages());
	PROTOCOL::CGetAlarmLogInfoRequest l_oGetInfoRequest;
	if (l_strRecvMsg.empty() || !l_oGetInfoRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_WARNING(m_pLog, "request string is empty or syntax error.[%s]", l_strRecvMsg.c_str());
		return;
	}
	//TODO:: 响应回复前端消息
	PROTOCOL::CPutAlarmLogInfoRespond l_oRespond;
	std::string l_strLogGuid = GetLogGuidByRequest(l_oGetInfoRequest.m_oBody.m_strGuID, l_oRespond);
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", l_strLogGuid.c_str());
	BuildAlarmLogInfoRespond(l_strLogGuid, l_oRespond);
	std::string l_strMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
}

std::string CBusinessImpl::GetLogGuidByRequest(const std::string& l_strGuid, PROTOCOL::CPutAlarmLogInfoRespond& l_oRespond)
{
	if (l_strGuid.empty()) {
		return l_strGuid;
	}
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_alarm_log_by_guid";
	l_SqlRequest.param["guid"] = l_strGuid;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "select_icc_t_alarm_log_by_guid sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		l_oRespond.m_oBody.m_strResult = "execute select_icc_t_alarm failed";
		return "";
	}
	std::string strParam;
	while (l_pResult->Next())
	{
		strParam = l_pResult->GetValue("operateattachdesc");
		int post = strParam.find('{');
		if (post != std::string::npos) {
			strParam = strParam.substr(post);
			post = strParam.find('}');
			strParam = strParam.substr(0, post + 1);
		}
		ICC_LOG_DEBUG(m_pLog, "strParam :%s\n", strParam.c_str());
		JsonParser::IJsonPtr p_pJson = ICCGetIJsonFactory()->CreateJson();
		if (!p_pJson->LoadJson(strParam))
		{
			ICC_LOG_DEBUG(m_pLog, "strParam to json failed\n");
		}
		else {
			ICC_LOG_DEBUG(m_pLog, "strParam :%s\n", strParam.c_str());
			return p_pJson->GetNodeValue("/id", "");
		}
	}

	return "";
}

void CBusinessImpl::BuildAlarmLogInfoRespond(const std::string& l_strGuid, PROTOCOL::CPutAlarmLogInfoRespond& l_oRespond)
{
	if (l_strGuid.empty()) {
		l_oRespond.m_oBody.m_strResult = "Guid is empty";
		return;
	}
	// select * from icc_t_jjdb_log where guid = '287b4a2d-ecca-48e9-ab80-708d83dd2e11'
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_jjdb_log_by_guid";
	l_SqlRequest.param["guid"] = l_strGuid;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "select_icc_t_jjdb_log_by_guid sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		l_oRespond.m_oBody.m_strResult = "execute select_icc_t_jjdb_log failed";
		return ;
	}
	if (!l_pResult->Next())
	{
		ICC_LOG_DEBUG(m_pLog, "result is empty");
		l_oRespond.m_oBody.m_strResult = "select_icc_t_jjdb_log is IsValid";
	}
	else
	{
		l_oRespond.m_oBody.m_strGuid = l_pResult->GetValue("guid");
		l_oRespond.m_oBody.m_strId = l_pResult->GetValue("id");
		l_oRespond.m_oBody.m_strAlarmId = l_pResult->GetValue("alarm_id");
		l_oRespond.m_oBody.m_strAlarmTime = l_pResult->GetValue("alarmtime");
		l_oRespond.m_oBody.m_strUpdateUser = l_pResult->GetValue("update_user");
		l_oRespond.m_oBody.m_strAlarmContent = l_pResult->GetValue("alarm_content");
		l_oRespond.m_oBody.m_strResult = "success";
	}
}


void ICC::CBusinessImpl::OnCNotifiAddOrUpdateLinkedDisPatchRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive OnCNotifiAddOrUpdateLinkedDisPatchRequest message:[%s]", p_pNotify->GetMessages().c_str());

	std::string l_strRecvMsg(p_pNotify->GetMessages());
	PROTOCOL::CAddOrUpdateLinkedRequest l_oSetLinkedRequest;
	if (l_strRecvMsg.empty() || !l_oSetLinkedRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_WARNING(m_pLog, "request string is empty or syntax error.[%s]", l_strRecvMsg.c_str());
		return;
	}

	bool l_bLinkedSuccess(false);
	ESyncType strStateType = ADD;
	size_t size = l_oSetLinkedRequest.m_oBody.m_vecData.size();
	for (size_t i = 0; i < size; i++)
	{
		BuildLinkedDataInfo(l_oSetLinkedRequest.m_oBody.m_vecData[i]);
		if (l_oSetLinkedRequest.m_oBody.m_vecData[i].m_bIsNewProcess)
		{
			//alarm id为空，则为新增处警请求			
			l_bLinkedSuccess = AddLinkedProcessInfo(l_oSetLinkedRequest.m_oBody.m_vecData[i]);
			strStateType = ESyncType::ADD;
		}
		else
		{
			//id 非空，则为更新处警请求
			l_bLinkedSuccess = UpdateLinkedProcessInfo(l_oSetLinkedRequest.m_oBody.m_vecData[i]);
			strStateType = ESyncType::EDIT;
		}
		// 发送同步消息;
		SyncLinkedInfo(l_oSetLinkedRequest.m_oBody.m_vecData[i], "sync_linked_dispatch_info", "topic_alarm_sync", strStateType, l_oSetLinkedRequest.m_oBody.m_vecData[i].m_strID);
		
	}
	//TODO::响应消息头
	PROTOCOL::CAddOrUpdateProcessRespond l_oRespond;
	BuildRespondHeader("OnCNotifiAddOrUpdateLinkedDisPatchRequest", l_oSetLinkedRequest.m_oHeader, l_oRespond.m_oHeader);
	if (l_bLinkedSuccess)
	{
		//新增警情成功，回复0
		l_oRespond.m_oHeader.m_strResult = "0";//0成功
	}
	else
	{
		//新增警情失败，回复1
		l_oRespond.m_oHeader.m_strResult = "1";//1失败		
	}

	std::string l_strRespondMsg(l_oRespond.ToString(m_pJsonFty->CreateJson()));
	p_pNotify->Response(l_strRespondMsg);
	
}

void CBusinessImpl::BuildLinkedDataInfo(PROTOCOL::CAddOrUpdateLinkedRequest::CLinkedData& l_oVecData)
{
	if (l_oVecData.m_strAlarmID.empty()) {
		ICC_LOG_WARNING(m_pLog, "BuildLinkedDataInfo m_strAlarmID is empty or syntax error.[%s]", l_oVecData.m_strAlarmID.c_str());
	}
	ICC_LOG_WARNING(m_pLog, "BuildLinkedDataInfo m_strState is .[%s]", l_oVecData.m_strState.c_str());
	if (l_oVecData.m_strID.empty()) {
		std::string strDeptDistrictCode = GetDeptDistrictCode(l_oVecData.m_strAlarmID);
		l_oVecData.m_strID = _GenId(strDeptDistrictCode);
		l_oVecData.m_bIsNewProcess = true;
		l_oVecData.m_strCreateTime = m_pDateTime->CurrentDateTimeStr();
	}
	else {
		l_oVecData.m_bIsNewProcess = false;
		l_oVecData.m_strUpdateTime = m_pDateTime->CurrentDateTimeStr();
	}
	//if (l_oVecData.m_strState.empty()) //如果为空且是新增的数据，则设置为01
	//{
	//	if (l_oVecData.m_bIsNewProcess)
	//	{
	//		l_oVecData.m_strState = LINKED_STATUS_HANDLING;
	//	}
	//	else {
	//		l_oVecData.m_strState = GetCurrentLinkedState(l_oVecData.m_strAlarmID);
	//	}
	//}
}

std::string CBusinessImpl::GetDeptDistrictCode(const std::string strAlarmId)
{
	std::string strSql = "select xzqhdm  from icc_t_jjdb where jjdbh = '" + strAlarmId + "'";
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(strSql);
	ICC_LOG_DEBUG(m_pLog, " GetCurrentLinkedState sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return "";
	}

	//如果没有这返回01状态
	if (!l_pResult->Next())
	{
		ICC_LOG_DEBUG(m_pLog, "result is empty, processid: %s", strAlarmId.c_str());
		return LINKED_STATUS_HANDLING;
	}

	std::string tmpDeptDistrictCode = l_pResult->GetValue("xzqhdm");
	ICC_LOG_DEBUG(m_pLog, "GetCurrentLinkedState: %s, code: %s", strAlarmId.c_str(), tmpDeptDistrictCode.c_str());
	return tmpDeptDistrictCode;
}

void ICC::CBusinessImpl::ModifyAddOrUpdateLinkedDisPatchRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive ModifyAddOrUpdateLinkedDisPatchRequest message:[%s]", p_pNotify->GetMessages().c_str());
	std::string l_strRecvMsg(p_pNotify->GetMessages());
	PROTOCOL::CAddOrUpdateLinkedRequest l_oLinkedRequest;
	if (l_strRecvMsg.empty() || !l_oLinkedRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_WARNING(m_pLog, "request string is empty or syntax error.[%s]", l_strRecvMsg.c_str());
		return;
	}
	std::string tmp_strMsg = "m_vecData is null";
	std::string tmp_option = "";
	bool l_bIsSuccess(false);
	size_t size = l_oLinkedRequest.m_oBody.m_vecData.size();
	for (size_t i = 0; i < size; i++) {
		// 前端传参数是否新增 还是 更新
		if (!l_oLinkedRequest.m_oBody.m_vecData[i].m_strID.empty()) {
			ICC_LOG_DEBUG(m_pLog, " Add Linked ProcessRequest info.", l_oLinkedRequest.m_oBody.m_vecData[i].m_strID.c_str());
			// 新增到联动单位表
			l_bIsSuccess = AddLinkedProcessInfo(l_oLinkedRequest.m_oBody.m_vecData[i]);
			tmp_option = "ADD";
			if (!l_bIsSuccess) {
				tmp_strMsg = "add Linked info failed";
			}
		} else {
			ICC_LOG_DEBUG(m_pLog, " Update Linked ProcessRequest info.", l_oLinkedRequest.m_oBody.m_vecData[i].m_strID.c_str());
			// 更新到联动单位表
			l_bIsSuccess = UpdateLinkedProcessInfo(l_oLinkedRequest.m_oBody.m_vecData[i]);
			tmp_option = "Update";
			if (!l_bIsSuccess) {
				tmp_strMsg = "update Linked info failed";
			}
		}
	}
	// 记录一下流水
	 CreateLinkedLog(l_oLinkedRequest.m_oBody.m_vecData[0], tmp_option);
	// 发送回复消息
	PROTOCOL::CAddOrUpdateAlarmWithProcessRespond l_oRespond;
	BuildRespondHeader("add_or_update_alarm_and_process_respond", l_oLinkedRequest.m_oHeader, l_oRespond.m_oHeader);
	// 新增警情成功回复0,否则回复1
	l_oRespond.m_oHeader.m_strResult = l_bIsSuccess ? "0" : "1";
	l_oRespond.m_oHeader.m_strMsg = tmp_strMsg;
	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	// 发送同步消息
	//if (l_bIsSuccess)
	//{
	//	SendToVCSLinkedInfo(l_oLinkedRequest);
	//}
}

// 新增联动单位 ICC
bool ICC::CBusinessImpl::AddLinkedProcessInfo(PROTOCOL::CAddOrUpdateLinkedRequest::CLinkedData& l_oRequestInfo)
{
	DataBase::SQLRequest l_tSQLRequest;
	l_tSQLRequest.sql_id = "postgres_insert_icc_t_linked_dispatch";
	l_tSQLRequest.param["id"] = l_oRequestInfo.m_strID;
	l_tSQLRequest.param["alarm_id"] = l_oRequestInfo.m_strAlarmID;
	l_tSQLRequest.param["state"] = l_oRequestInfo.m_strState;
	l_tSQLRequest.param["linked_org_code"] = l_oRequestInfo.m_strLinkedOrgCode;
	l_tSQLRequest.param["linked_org_name"] = l_oRequestInfo.m_strLinkedOrgName;
	l_tSQLRequest.param["linked_org_type"] = l_oRequestInfo.m_strLinkedOrgType;
	l_tSQLRequest.param["dispatch_code"] = l_oRequestInfo.m_strDispatchCode;
	l_tSQLRequest.param["dispatch_name"] = l_oRequestInfo.m_strDispatchName;
	l_tSQLRequest.param["create_user"] = l_oRequestInfo.m_strCreateUser;
	l_tSQLRequest.param["create_time"] = l_oRequestInfo.m_strCreateTime;
	l_tSQLRequest.param["update_user"] = l_oRequestInfo.m_strUpdateUser;
	l_tSQLRequest.param["update_time"] = l_oRequestInfo.m_strUpdateTime;
	l_tSQLRequest.param["result"] = l_oRequestInfo.m_strResult;
	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_tSQLRequest);
	ICC_LOG_DEBUG(m_pLog, "postgres_insert_icc_t_linked_dispatch sql:[%s]", l_oResult->GetSQL().c_str());
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert alarm info failed,error msg:[%s]", l_oResult->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

// 新增联动单位 ICC
bool ICC::CBusinessImpl::AddLinkedInfoFromVCS(PROTOCOL::CAddOrUpdateProcessRequest::CLinkedData& l_oRequestInfo)
{
	DataBase::SQLRequest l_tSQLRequest;
	l_tSQLRequest.sql_id = "postgres_insert_icc_t_linked_dispatch";
	l_tSQLRequest.param["id"] = l_oRequestInfo.m_strID;
	l_tSQLRequest.param["alarm_id"] = l_oRequestInfo.m_strAlarmID;
	l_tSQLRequest.param["state"] = l_oRequestInfo.m_strState;
	l_tSQLRequest.param["linked_org_code"] = l_oRequestInfo.m_strLinkedOrgCode;
	l_tSQLRequest.param["linked_org_name"] = l_oRequestInfo.m_strLinkedOrgName;
	l_tSQLRequest.param["linked_org_type"] = l_oRequestInfo.m_strLinkedOrgType;
	l_tSQLRequest.param["dispatch_code"] = l_oRequestInfo.m_strDispatchCode;
	l_tSQLRequest.param["dispatch_name"] = l_oRequestInfo.m_strDispatchName;
	l_tSQLRequest.param["create_user"] = l_oRequestInfo.m_strCreateUser;
	l_tSQLRequest.param["create_time"] = l_oRequestInfo.m_strCreateTime;
	l_tSQLRequest.param["update_user"] = l_oRequestInfo.m_strUpdateUser;
	l_tSQLRequest.param["update_time"] = l_oRequestInfo.m_strUpdateTime;
	l_tSQLRequest.param["result"] = l_oRequestInfo.m_strResult;
	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_tSQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_oResult->GetSQL().c_str());
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert alarm info failed,error msg:[%s]", l_oResult->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

// 记录联动单位流水	
void ICC::CBusinessImpl::CreateLinkedLog(PROTOCOL::CAddOrUpdateLinkedRequest::CLinkedData& l_oRequestInfo, const std::string& strOption)
{
	PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
	std::vector<std::string> l_vecParamList;
	l_oAlarmLogInfo.m_strAlarmID = l_oRequestInfo.m_strID;
	l_vecParamList.push_back(l_oRequestInfo.m_strCreateUser);
	l_vecParamList.push_back(l_oRequestInfo.m_strCreateTime);
	l_vecParamList.push_back(strOption);
	l_oAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
	l_oAlarmLogInfo.m_strCreateUser = l_oRequestInfo.m_strCreateUser;
	l_oAlarmLogInfo.m_strOperate = LOG_RECEIPT_ACCEPTED;
	l_oAlarmLogInfo.m_strSourceName = "icc";
	AddAlarmLogInfo(l_oAlarmLogInfo);

}

// 更新联动单位 ICC
bool ICC::CBusinessImpl::UpdateLinkedProcessInfo(PROTOCOL::CAddOrUpdateLinkedRequest::CLinkedData& l_oRequestInfo)
{
	DataBase::SQLRequest l_tSQLRequest;
	l_tSQLRequest.sql_id = "postgres_update_icc_t_linked_dispatch";
	l_tSQLRequest.param["id"] = l_oRequestInfo.m_strID;
	if (!l_oRequestInfo.m_strAlarmID.empty()) {
		l_tSQLRequest.set["alarm_id"] = l_oRequestInfo.m_strAlarmID;
	}
	if (!l_oRequestInfo.m_strState.empty()) {
		l_tSQLRequest.set["state"] = l_oRequestInfo.m_strState;
	}
	if (!l_oRequestInfo.m_strLinkedOrgCode.empty()) {
		l_tSQLRequest.set["linked_org_code"] = l_oRequestInfo.m_strLinkedOrgCode;
	}
	if (!l_oRequestInfo.m_strLinkedOrgName.empty()) {
		l_tSQLRequest.set["linked_org_name"] = l_oRequestInfo.m_strLinkedOrgName;
	}
	if (!l_oRequestInfo.m_strLinkedOrgType.empty()) {
		l_tSQLRequest.set["linked_org_type"] = l_oRequestInfo.m_strLinkedOrgType;
	}
	if (!l_oRequestInfo.m_strDispatchCode.empty()) {
		l_tSQLRequest.set["dispatch_code"] = l_oRequestInfo.m_strDispatchCode;
	}
	if (!l_oRequestInfo.m_strDispatchName.empty()) {
		l_tSQLRequest.set["dispatch_name"] = l_oRequestInfo.m_strDispatchName;
	}
	if (!l_oRequestInfo.m_strCreateUser.empty()) {
		l_tSQLRequest.set["create_user"] = l_oRequestInfo.m_strCreateUser;
	}
	l_tSQLRequest.set["create_time"] = l_oRequestInfo.m_strCreateTime;
	if (!l_oRequestInfo.m_strUpdateUser.empty()) {
		l_tSQLRequest.set["update_user"] = l_oRequestInfo.m_strUpdateUser;
	}
	l_tSQLRequest.set["update_time"] = l_oRequestInfo.m_strUpdateTime;
	l_tSQLRequest.set["result"] = l_oRequestInfo.m_strResult;
	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_tSQLRequest);
	ICC_LOG_DEBUG(m_pLog, "postgres_update_icc_t_linked_dispatch sql:[%s]", l_oResult->GetSQL().c_str());
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "postgres_update_icc_t_linked_dispatch info failed,error msg:[%s]", l_oResult->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

// 更新联动单位 ICC
bool ICC::CBusinessImpl::UpdateLinkedInfoFromVCS(PROTOCOL::CAddOrUpdateProcessRequest::CLinkedData& l_oRequestInfo)
{
	DataBase::SQLRequest l_tSQLRequest;
	l_tSQLRequest.sql_id = "postgres_update_icc_t_linked_dispatch";
	l_tSQLRequest.param["id"] = l_oRequestInfo.m_strID;
	l_tSQLRequest.param["alarm_id"] = l_oRequestInfo.m_strAlarmID;
	l_tSQLRequest.param["state"] = l_oRequestInfo.m_strState;
	l_tSQLRequest.param["linked_org_code"] = l_oRequestInfo.m_strLinkedOrgCode;
	l_tSQLRequest.param["linked_org_name"] = l_oRequestInfo.m_strLinkedOrgName;
	l_tSQLRequest.param["linked_org_type"] = l_oRequestInfo.m_strLinkedOrgType;
	l_tSQLRequest.param["dispatch_code"] = l_oRequestInfo.m_strDispatchCode;
	l_tSQLRequest.param["dispatch_name"] = l_oRequestInfo.m_strDispatchName;
	l_tSQLRequest.param["create_user"] = l_oRequestInfo.m_strCreateUser;
	l_tSQLRequest.param["create_time"] = l_oRequestInfo.m_strCreateTime;
	l_tSQLRequest.param["update_user"] = l_oRequestInfo.m_strUpdateUser;
	l_tSQLRequest.param["update_time"] = l_oRequestInfo.m_strUpdateTime;
	l_tSQLRequest.param["result"] = l_oRequestInfo.m_strResult;
	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_tSQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_oResult->GetSQL().c_str());
	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert alarm info failed,error msg:[%s]", l_oResult->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

// 发送联动单位信息到 VCS
void ICC::CBusinessImpl::SyncLinkedInfo(const PROTOCOL::CAddOrUpdateLinkedRequest::CLinkedData p_rLinkedInfoSync, std::string strLinkedCmd, std::string strLinkedRequest, ESyncType p_iSyncType, std::string p_strReleatedID)
{
	PROTOCOL::CAlarmLinkedSync l_oAlarmLinkedSync;
	l_oAlarmLinkedSync.m_oHeader.m_strSystemID = SYSTEMID;
	l_oAlarmLinkedSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
	l_oAlarmLinkedSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oAlarmLinkedSync.m_oHeader.m_strRelatedID = p_strReleatedID;
	l_oAlarmLinkedSync.m_oHeader.m_strCmd = strLinkedCmd;
	l_oAlarmLinkedSync.m_oHeader.m_strRequest = strLinkedRequest;
	if (strLinkedRequest.find("queue") == std::string::npos) {
		l_oAlarmLinkedSync.m_oHeader.m_strRequestType = "1";	// topic
	}
	else {
		l_oAlarmLinkedSync.m_oHeader.m_strRequestType = "0";	// queue
	}
	l_oAlarmLinkedSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	if (!p_rLinkedInfoSync.m_strMsgSource.compare("vcs"))
	{
		l_oAlarmLinkedSync.m_oBody.m_strMsgSource = "vcs";
	}
	if (!p_rLinkedInfoSync.m_strMsgSource.compare("mpa"))
	{
		l_oAlarmLinkedSync.m_oBody.m_strMsgSource = "mpa";
	}

	l_oAlarmLinkedSync.m_oBody.m_LinkedData = p_rLinkedInfoSync;
	l_oAlarmLinkedSync.m_oBody.m_strSyncType = std::to_string(p_iSyncType);
	//同步消息
	std::string l_strMsg(l_oAlarmLinkedSync.ToString(m_pJsonFty->CreateJson()));
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "SyncLinkedInfo send message:[%s]", l_strMsg.c_str());
}

void ICC::CBusinessImpl::OnCNotifiAddOrUpdateProcessRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	std::string l_strRecvMsg(p_pNotify->GetMessages());
	PROTOCOL::CAddOrUpdateProcessRequest l_oSetProcessRequest;
	if (l_strRecvMsg.empty() || !l_oSetProcessRequest.ParseString(l_strRecvMsg, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_WARNING(m_pLog, "request string is empty or syntax error.[%s]", l_strRecvMsg.c_str());
		return;
	}

	bool l_bSetProcessSuccess(false);
	if (!IsProcessInfoValid(l_oSetProcessRequest.m_oBody))
	{
		//处警数据不合理，回复失败信息		
		l_bSetProcessSuccess = false;
	}
	else
	{
		for (PROTOCOL::CAddOrUpdateProcessRequest::CProcessData l_oProcessData : l_oSetProcessRequest.m_oBody.m_vecData)
		{
			if (l_oProcessData.m_strID.empty() || l_oProcessData.m_strUpdateType == "0")
			{
				//alarm id为空，则为新增处警请求
				l_bSetProcessSuccess = AddProcessInfo(l_oProcessData);
			}
			else
			{
				//id 非空，则为更新处警请求
				l_bSetProcessSuccess = UpdateProcessInfo(l_oProcessData);
			}
		}

	}

	//TODO::响应消息头
	PROTOCOL::CAddOrUpdateProcessRespond l_oRespond;
	BuildRespondHeader("add_or_update_process_respond", l_oSetProcessRequest.m_oHeader, l_oRespond.m_oHeader);
	// 先不加不要影响原来的逻辑
	if (l_bSetProcessSuccess)
	{
		//新增警情成功，回复0
		l_oRespond.m_oBody.m_strResult = "0";//0成功
		//更新缓存
		//TODO::广播通知新增警情
	}
	else
	{
		//新增警情失败，回复1
		l_oRespond.m_oBody.m_strResult = "1";//1失败		
	}

	std::string l_strRespondMsg(l_oRespond.ToString(m_pJsonFty->CreateJson()));

	p_pNotify->Response(l_strRespondMsg);
}

void CBusinessImpl::OnCNotifiAddAlarmSignRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CAddSignRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}	

	// 发送回复消息
	PROTOCOL::CAddSignRespond l_oRespond;
	bool l_bRes = BuildAddSignRespond(l_oRespond, l_oRequest);

	// 写签收流水
	if (l_bRes)
	{
		//不为VCS才写警情日志
		if (l_oRequest.m_oBody.m_strMsgSource != "vcs_relocated")
		{
			//TODO::记录警情流水_受理接警单	
		  //向数据库添加接警流水并发送通知
			PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
			l_oAlarmLogInfo.m_strAlarmID = l_oRequest.m_oBody.m_strAlarmID;
			std::vector<std::string> l_vecParamList;
			Data::CStaffInfo l_oStaffInfo;
			if (!_GetStaffInfo(l_oRequest.m_oBody.m_strProcessCode, l_oStaffInfo))
			{
				ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
			}
			std::string strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_oRequest.m_oBody.m_strProcessName);

			l_vecParamList.push_back(strStaffName);
			l_vecParamList.push_back(l_oRequest.m_oBody.m_strProcessCode);
			l_vecParamList.push_back(l_oRequest.m_oBody.m_strProcessDeptName);
			l_oAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
			l_oAlarmLogInfo.m_strFromType = "staff";
			l_oAlarmLogInfo.m_strFromObject = l_oRequest.m_oBody.m_strDispatchCode;
			l_oAlarmLogInfo.m_strFromObjectName = l_oRequest.m_oBody.m_strDispatchName;
			l_oAlarmLogInfo.m_strFromObjectOrgName = l_oRequest.m_oBody.m_strDispatchDeptName;
			//l_oAlarmLogInfo.m_strFromObjectOrgCode = l_oRequest.m_oBody.m_strDispatchDeptCode;
			l_oAlarmLogInfo.m_strToType = "org";
			l_oAlarmLogInfo.m_strToObject = l_oRequest.m_oBody.m_strProcessDeptCode;
			l_oAlarmLogInfo.m_strToObjectName = l_oRequest.m_oBody.m_strProcessDeptName;
			l_oAlarmLogInfo.m_strToObjectOrgName = l_oRequest.m_oBody.m_strProcessDeptName;
			//l_oAlarmLogInfo.m_strToObjectOrgCode = l_oRequest.m_oBody.m_strProcessDeptCode;
			l_oAlarmLogInfo.m_strSourceName = "icc";
			l_oAlarmLogInfo.m_strCreateUser = l_oRequest.m_oBody.m_strProcessName;
			l_oAlarmLogInfo.m_strOperate = LOG_DISPATCH_CONFIRM_RECEIVE;
			AddAlarmLogInfo(l_oAlarmLogInfo);
		}
		
	}


	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	bool bFromVCS = false;
	//2021/11/29暂时不处理
	//if (l_oRequest.m_oHeader.m_strSubsystemID == "VCS")
	//{
	//	bFromVCS = true;
	//}

	// 发送同步信息
	if (l_bRes)
	{
		SyncProcessDate(l_oRequest.m_oBody.m_strID, bFromVCS);
	}
}

void CBusinessImpl::OnCNotifiGetAlarmSignRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CGetSignRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	// 发送回复消息
	PROTOCOL::CGetSignRespond l_oRespond;
	BuildRespondHeader("get_alarm_sign_respond", l_oRequest.m_oHeader, l_oRespond.m_oHeader);

	PROTOCOL::CGetSignRespond::CBody l_oDataBody;
	BuildGetSignRespond(l_oDataBody, l_oRequest);
	size_t l_nCount = l_oDataBody.m_vecAlarm.size();
	if (l_nCount == 0)
	{
		l_oRespond.m_oBody.m_strCount = std::to_string(l_nCount);
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send respond:[%s]", l_strSendMsg.c_str());
	}
	else
	{
		for (auto l_oData : l_oDataBody.m_vecAlarm)
		{
			l_oRespond.m_oBody.m_vecAlarm.push_back(l_oData);

			if (MAX_COUNT == l_oRespond.m_oBody.m_vecAlarm.size())
			{
				l_oRespond.m_oBody.m_strCount = std::to_string(l_nCount);
				std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
				p_pNotify->Response(l_strSendMsg, true);
				ICC_LOG_DEBUG(m_pLog, "send sub respond:[%s]", l_strSendMsg.c_str());

				l_oRespond.m_oBody.m_vecAlarm.clear();
			}
		}

		if (!l_oRespond.m_oBody.m_vecAlarm.empty())
		{
			l_oRespond.m_oBody.m_strCount = std::to_string(l_nCount);
			std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
			p_pNotify->Response(l_strSendMsg);
			ICC_LOG_DEBUG(m_pLog, "send end respond:[%s]", l_strSendMsg.c_str());
		}
	}
}

void CBusinessImpl::OnCNotifiAddAlarmRemarkRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
    PROTOCOL::CAddAlarmRemarkRequestEx l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
    {
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
        return;
    }

	std::string strResourceID = l_oRequest.m_oBody.m_strGuid;
	l_oRequest.m_oBody.m_strGuid = m_pString->CreateGuid();

	// 发送回复消息
    PROTOCOL::CAddAlarmRemarkRespond l_oRespond;
	bool l_bRes = BuildAddRemarkRespond(l_oRespond, l_oRequest);	
	if (l_bRes)
	{
		//VCS过来的不写
		if (l_oRequest.m_oBody.m_strMsgSorce != "vcs" && l_oRequest.m_oBody.m_strMsgSorce != "vcs_relocated")
		{
			// 记录警情流水_备注
			PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
			l_oAlarmLogInfo.m_strID = m_pString->CreateGuid();
			l_oAlarmLogInfo.m_strAlarmID = l_oRequest.m_oBody.m_strAlarmID;
			std::vector<std::string> l_vecParamList;

			Data::CStaffInfo l_oStaffInfo;
			if (!_GetStaffInfo(l_oRequest.m_oBody.m_strFeedBackCode, l_oStaffInfo))
			{
				ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
			}
			std::string strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_oRequest.m_oBody.m_strFeedBackName);

			l_vecParamList.push_back(strStaffName);
			l_vecParamList.push_back(l_oRequest.m_oBody.m_strFeedBackCode);
			l_vecParamList.push_back(l_oRequest.m_oBody.m_strFeedBackDeptName);
			l_vecParamList.push_back(l_oRequest.m_oBody.m_strContent);
			if (!l_oRequest.m_oBody.m_strLongitude.empty() && l_oRequest.m_oBody.m_strLongitude.compare("0") != 0)
			{
				l_vecParamList.push_back(l_oRequest.m_oBody.m_strLongitude);
				l_vecParamList.push_back(l_oRequest.m_oBody.m_strLatitude);
			}
			l_oAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
			l_oAlarmLogInfo.m_strFromType = "staff";
			l_oAlarmLogInfo.m_strFromObject = l_oRequest.m_oBody.m_strFeedBackCode;
			l_oAlarmLogInfo.m_strFromObjectName = l_oRequest.m_oBody.m_strFeedBackName;
			l_oAlarmLogInfo.m_strFromObjectOrgName = l_oRequest.m_oBody.m_strFeedBackDeptName;
			l_oAlarmLogInfo.m_strFromObjectOrgCode = l_oRequest.m_oBody.m_strFeedBackDeptOrgCode;
			l_oAlarmLogInfo.m_strCreateUser = l_oRequest.m_oBody.m_strFeedBackName;
			l_oAlarmLogInfo.m_strOperate = LOG_ALARM_REMARK;
			std::string type = RESOURCETYPE_REMARK;
			l_oAlarmLogInfo.m_strOperateAttachDesc = BuildAlarmLogAttach(type, strResourceID);
			l_oAlarmLogInfo.m_strCreateTime = m_pDateTime->CurrentDateTimeStr();
			l_oAlarmLogInfo.m_strSourceName = "icc";

			if (!l_oRequest.m_oBody.m_strFileType.empty() && !l_oRequest.m_oBody.m_strFileGuid.empty())
			{
				//APP普通反馈映射
				int nFileType = atoi(l_oRequest.m_oBody.m_strFileType.c_str());
				nFileType += 9;
				std::string tmp_strFileType(m_pString->Number(nFileType));
				l_oAlarmLogInfo.m_strOperateAttachDesc = BuildAlarmLogAttach(tmp_strFileType, l_oRequest.m_oBody.m_strFileGuid);
				l_oAlarmLogInfo.m_strOperate = LOG_ALARM_REMARK_APP;
			}
			if (!l_oRequest.m_oBody.m_strLongitude.empty() && l_oRequest.m_oBody.m_strLongitude.compare("0") != 0)
			{
				l_oAlarmLogInfo.m_strOperate = LOG_ALARM_REMARK_POSITION;
			}
			AddAlarmLogInfo(l_oAlarmLogInfo);
		}

		SyncRemarkInfo(l_oRequest);
	}

	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	// 转发备注记录至VCS对接服务
	if (l_bRes && ("vcs" != l_oRequest.m_oBody.m_strMsgSorce))
	{

		SendRemark2VCS(l_oRequest);
	}
}

void CBusinessImpl::OnCNotifiGetAlarmRemarkRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
    PROTOCOL::CGetAlarmRemarkRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
    {
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
        return;
    }

	// 发送回复消息
    PROTOCOL::CGetAlarmRemarkRespond l_oRespond;
	bool l_bRes = BuildGetRemarkRespond(l_oRespond, l_oRequest);
    
	if (l_bRes)
	{
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	}
}

void CBusinessImpl::OnCNotifiGetAlarmAllRemarkRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
    PROTOCOL::CGetAlarmAllRemarkRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
    {
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
        return;
    }

	// 发送回复消息
    PROTOCOL::CGetAlarmAllRemarkRespond l_oRespond;
	bool l_bRes = BuildGetAllRemarkRespond(l_oRespond, l_oRequest);

	if (l_bRes)
	{
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	}
}

void CBusinessImpl::OnCNotifiGetAlarmLogRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CGetAlarmLogRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	// 发送回复消息
	PROTOCOL::CGetAlarmLogRespond l_oRespond;
	bool l_bRes = BuildGetAlarmLogRespond(l_oRespond, l_oRequest);
	if (l_oRequest.m_oBody.m_strVcsSyncFlag == "1")
	{
		l_oRespond.m_bVcsSyncFlag = true;
	}
	/*if (l_bRes)
	{*/
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson(), m_pJsonFty->CreateJson(), m_pLog);
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	//}
}

void CBusinessImpl::OnCNotifiGetAlarmLogAlarmRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	return;

	// 解析请求消息
	PROTOCOL::CGetAlarmLogAlarmResRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	// 发送回复消息
	PROTOCOL::CGetAlarmLogAlarmResRespond l_oRespond;
	bool l_bRes = BuildGetAlarmLogAlarmRespond(l_oRespond, l_oRequest);

	if (l_bRes)
	{
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	}
}

void CBusinessImpl::OnCNotifiGetAlarmLogProcessRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	return;

	// 解析请求消息
	PROTOCOL::CGetAlarmProcLogResRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		return;
	}

	// 发送回复消息
	PROTOCOL::CGetAlarmProcLogResRespond l_oRespond;
	bool l_bRes = BuildGetAlarmLogProcessRespond(l_oRespond, l_oRequest);

	if (l_bRes)
	{
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	}
}

void CBusinessImpl::OnCNotifiGetAlarmLogCallrefRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CGetAlarmLogCallrefRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}
	
	// 发送回复消息
	PROTOCOL::CGetAlarmLogCallrefRespond l_oRespond;
	bool l_bRes = BuildGetAlarmLogCallrefRespond(l_oRespond, l_oRequest);
	if (l_bRes)
	{
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	}
}

void CBusinessImpl::OnCNotifiMergeAlarmRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CAddMergeRequest l_oAddMergeRequest;
	if (!l_oAddMergeRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	// 发送回复消息
	PROTOCOL::CAddMergeRespond l_oAddMergeRespond;
	bool l_bRes = BuildMergeAlarmRespond(l_oAddMergeRespond, l_oAddMergeRequest);

	if (l_bRes)
	{
		do
		{
			std::string l_strUserName = l_oAddMergeRequest.m_oBody.m_strUpdateUserName;
			std::string l_strDeptName = l_oAddMergeRequest.m_oBody.m_strUpdateUserDeptName;
			std::string l_strDeptCode = l_oAddMergeRequest.m_oBody.m_strUpdateUserDeptCode;
			std::string l_strUserCode = l_oAddMergeRequest.m_oBody.m_strUpdateUser;
			if (l_strUserCode.empty())
			{
				ICC_LOG_ERROR(m_pLog, "merge user code is empty!!! will not wriete log!");
				break;
			}

			if (l_strUserName.empty() || l_strDeptCode.empty())
			{
				GetStaffDept(l_strUserCode, l_strUserName, l_strDeptName, l_strDeptCode);
			}


			PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
			l_oAlarmLogInfo.m_strAlarmID = l_oAddMergeRequest.m_oBody.m_strAlarmID;
			std::vector<std::string> l_vecParamList;
			Data::CStaffInfo l_oStaffInfo;
			if (!_GetStaffInfo(l_strUserCode, l_oStaffInfo))
			{
				ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
			}
			std::string l_strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_strUserName);

			l_vecParamList.push_back(l_strStaffName);
			l_vecParamList.push_back(l_strUserCode);
			l_vecParamList.push_back(l_strDeptName);
			l_vecParamList.push_back(l_oAddMergeRequest.m_oBody.m_strAlarmID);
			l_vecParamList.push_back(l_oAddMergeRequest.m_oBody.m_strMergeID);
			l_vecParamList.push_back(l_oAddMergeRequest.m_oBody.m_strMergeType);
			l_oAlarmLogInfo.m_strSeatNo = l_oAddMergeRequest.m_oHeader.m_strSeatNo;
			l_oAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
			l_oAlarmLogInfo.m_strCreateUser = l_strUserCode;

			l_oAlarmLogInfo.m_strOperate = LOG_ALARM_MERGED;
			l_oAlarmLogInfo.m_strDeptOrgCode = l_strDeptCode;
			l_oAlarmLogInfo.m_strSourceName = "icc";
			AddAlarmLogInfo(l_oAlarmLogInfo, "");
			l_oAlarmLogInfo.m_strAlarmID = l_oAddMergeRequest.m_oBody.m_strMergeID;
			AddAlarmLogInfo(l_oAlarmLogInfo, "");
		} while (false);

	}

	std::string l_strSendMsg = l_oAddMergeRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	// 发送同步信息并通知今日警情
	if (l_bRes)
	{
		SyncReceiptDate(l_oAddMergeRequest.m_oBody.m_strAlarmID);
		SyncReceiptDate(l_oAddMergeRequest.m_oBody.m_strMergeID);
	}
}

void CBusinessImpl::OnCNotifiUnMergeRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CCancelMergeRequest l_oCancelMergeRequest;
	if (!l_oCancelMergeRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	// 发送回复消息
	PROTOCOL::CCancelMergeRespond l_oCancelMergeRespond;
	bool l_bRes = BuildUnMergeRespond(l_oCancelMergeRespond, l_oCancelMergeRequest);

	if (l_bRes)
	{
		do
		{
			std::string l_strUserName = l_oCancelMergeRequest.m_oBody.m_strUpdateUserName;
			std::string l_strDeptName = l_oCancelMergeRequest.m_oBody.m_strUpdateUserDeptName;
			std::string l_strDeptCode = l_oCancelMergeRequest.m_oBody.m_strUpdateUserDeptCode;
			std::string l_strUserCode = l_oCancelMergeRequest.m_oBody.m_strUpdateUser;
			if (l_strUserCode.empty())
			{
				ICC_LOG_ERROR(m_pLog, "cancel merge user code is empty!!! will not wriete log!");
				break;
			}

			if (l_strUserName.empty() || l_strDeptCode.empty())
			{
				GetStaffDept(l_strUserCode, l_strUserName, l_strDeptName, l_strDeptCode);
			}


			PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
			l_oAlarmLogInfo.m_strAlarmID = l_oCancelMergeRequest.m_oBody.m_strAlarmID;
			std::vector<std::string> l_vecParamList;

			Data::CStaffInfo l_oStaffInfo;
			if (!_GetStaffInfo(l_strUserCode, l_oStaffInfo))
			{
				ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
			}
			std::string l_strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_strUserName);

			l_vecParamList.push_back(l_strStaffName);
			l_vecParamList.push_back(l_strUserCode);
			l_vecParamList.push_back(l_strDeptName);
			l_vecParamList.push_back(l_oCancelMergeRequest.m_oBody.m_strAlarmID);
			l_vecParamList.push_back(l_oCancelMergeRequest.m_oBody.m_strMergeID);
			l_vecParamList.push_back(l_oCancelMergeRequest.m_oBody.m_strMergeType);
			l_oAlarmLogInfo.m_strSeatNo = l_oCancelMergeRequest.m_oHeader.m_strSeatNo;
			l_oAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
			l_oAlarmLogInfo.m_strCreateUser = l_strUserCode;

			l_oAlarmLogInfo.m_strOperate = LOG_ALARM_CANCELLED_MERGED;
			l_oAlarmLogInfo.m_strDeptOrgCode = l_strDeptCode;
			l_oAlarmLogInfo.m_strSourceName = "icc";
			AddAlarmLogInfo(l_oAlarmLogInfo, "");
			l_oAlarmLogInfo.m_strAlarmID = l_oCancelMergeRequest.m_oBody.m_strMergeID;
			AddAlarmLogInfo(l_oAlarmLogInfo, "");
		} while (false);

	}

	std::string l_strSendMsg = l_oCancelMergeRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	// 发送同步信息
	if (l_bRes)
	{
		SyncReceiptDate(l_oCancelMergeRequest.m_oBody.m_strAlarmID);
		// 这里将父警单的同步放在了BuildCancelMergeRespond方法的内部
	}
}

void CBusinessImpl::OnCNotifiGetMergeAlarmRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CGetMergeAlarmRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	// 发送回复消息
	PROTOCOL::CGetMergeAlarmRespond l_oRespond;
	if (l_oRequest.m_oBody.m_strAlarmID.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "Merge Alarm Request Alarm Id is empty!!!");
		
		
		BuildRespondHeader("get_merge_alarm_respond", l_oRequest.m_oHeader, l_oRespond.m_oHeader);
		l_oRespond.m_oHeader.m_strResult = "2";
		l_oRespond.m_oHeader.m_strMsg = "AlarmID is empty";

		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);


		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}
	bool l_bRes = BuildGetMergeAlarmRespond(l_oRespond, l_oRequest);

	if (0 != l_oRequest.m_oBody.m_strIgnorePrivacy.compare("true"))
	{
		std::vector<PROTOCOL::CAlarmInfo>::iterator it = l_oRespond.m_oBody.m_vecAlarmData.begin();
		for (; it != l_oRespond.m_oBody.m_vecAlarmData.end(); ++it)
		{
			if (0 == it->m_strPrivacy.compare("1"))
			{
				it->m_strCallerNo = "******";
				it->m_strCallerName = "******";
				it->m_strCallerAddr = "******";
				it->m_strCallerID = "******";
				it->m_strCallerIDType = "******";
				it->m_strCallerGender = "******";
				//it->m_strCallerAge = "******";
				//it->m_strCallerBirthday = "******";
				it->m_strContactNo = "******";
				//it->m_strContactName = "******";
				//it->m_strContactAddr = "******";
				//it->m_strContactID = "******";
				//it->m_strContactIDType = "******";
				//it->m_strContactGender = "******";
				//it->m_strContactAge = "******";
				//it->m_strContactBirthday = "******";
			}
		}
	}


	if (l_bRes)
	{
		l_oRespond.m_oBody.m_strCount = m_pString->Number(l_oRespond.m_oBody.m_vecAlarmData.size());
		
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);


		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	}
}

void CBusinessImpl::OnCNotifiGetAlarmBysourceIDRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CGetAlarmidBySourceid l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_jjdb";
	l_SqlRequest.param["source_id"] = l_oRequest.m_strSourceID;

	std::string strTime = m_pDateTime->GetCallRefIdTime(l_oRequest.m_strSourceID);
	if (strTime != "")
	{
		l_SqlRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_SqlRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		l_oRequest.m_strAlarmID = "";
		l_oRequest.m_oHeader.m_strResult = "1";
		l_oRequest.m_oHeader.m_strMsg = "execute select_icc_t_alarm failed";
	}
	if (!l_pResult->Next())
	{
		ICC_LOG_DEBUG(m_pLog, "result is empty");
		l_oRequest.m_strAlarmID = "";
		
	}
	else
	{
		l_oRequest.m_strAlarmID = l_pResult->GetValue("id");
	}
	
	/*l_oRequest.m_oHeader.m_strSystemID = SYSTEMID;
	l_oRequest.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
	l_oRequest.m_oHeader.m_strRelatedID = l_oRequest.m_oHeader.m_strMsgid;
	l_oRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oRequest.m_oHeader.m_strCmd = "get_alarmid_respond";;
	l_oRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();*/

	std::string l_strMsg = l_oRequest.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
}


void CBusinessImpl::OnCNotifiAlarmVisitRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CAlarmVisitRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	if (l_oRequest.m_oBody.m_strID.empty())
	{
		l_oRequest.m_oBody.m_strID = m_pDateTime->CreateSerial();
	}

	bool l_bIsSucess = false;

	std::string l_strTransactionGuid = m_pDBConn->BeginTransaction();
	if (0 == l_oRequest.m_oBody.m_strType.compare("1"))
	{// 新增回访单
		if (InsertVisit(l_oRequest, l_strTransactionGuid))
		{// 写流水并发送流水同步
			PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
			l_oAlarmLogInfo.m_strAlarmID = l_oRequest.m_oBody.m_strAlarmID;
			std::vector<std::string> l_vecParamList;

			Data::CStaffInfo l_oStaffInfo;
			if (!_GetStaffInfo(l_oRequest.m_oBody.m_strVisitorCode, l_oStaffInfo))
			{
				ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
			}
			std::string l_strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_oRequest.m_oBody.m_strVisitorName);

			l_vecParamList.push_back(l_strStaffName);
			l_vecParamList.push_back(l_oRequest.m_oBody.m_strVisitorCode);
			l_vecParamList.push_back(l_oRequest.m_oBody.m_strDeptName);
			if (!l_oRequest.m_oBody.m_strCallrefID.empty())
			{
				std::string type = "7";
				l_oAlarmLogInfo.m_strOperateAttachDesc = BuildAlarmLogAttach(type, l_oRequest.m_oBody.m_strCallrefID);
			}
			l_oAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
			l_oAlarmLogInfo.m_strCreateUser = l_oRequest.m_oBody.m_strVisitorName;
			l_oAlarmLogInfo.m_strOperate = LOG_ALARM_VISIT;
			if (AddAlarmLogInfo(l_oAlarmLogInfo, l_strTransactionGuid))
			{// 更新alarm表
				l_bIsSucess = UpdateIsVisitor(l_oRequest.m_oBody.m_strAlarmID, l_strTransactionGuid);
			}

			l_oAlarmLogInfo.m_strSourceName = "icc";
		}		
	}
	else
	{// 编辑回访单
		l_bIsSucess = UpdateVisit(l_oRequest, l_strTransactionGuid);
	}
	l_bIsSucess ? m_pDBConn->Commit(l_strTransactionGuid) : m_pDBConn->Rollback(l_strTransactionGuid);

	PROTOCOL::CAlarmVisitRespond l_oRespond;
	BuildRespondHeader("alarm_visit_respond", l_oRequest.m_oHeader, l_oRespond.m_oHeader);
	l_oRespond.m_oBody.m_strResult = l_bIsSucess ? "0" : "1";
	l_oRespond.m_oHeader.m_strResult = l_bIsSucess ? "0" : "1";

	if (!l_bIsSucess)
	{
		l_oRespond.m_oHeader.m_strMsg = "UpdateIsVisitor failed";
	}

	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	// 发送回访单同步
	PROTOCOL::CAlarmVisitSync l_oVisitSync;
	BuildRespondHeader("sync_alarm_visit", l_oVisitSync.m_oHeader);
	if (BuildVisitSyncBody(l_oRequest.m_oBody.m_strID, l_oVisitSync.m_oBody))
	{
		std::string l_strMessage = l_oVisitSync.ToString(ICCGetIJsonFactory()->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
	}
}

void CBusinessImpl::OnCNotifiSearchAlarmVisitRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CSearchAlarmVisitRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}	

	PROTOCOL::CSearchAlarmVisitRespond::CBody l_oRespBody;
	if (!SelectVisitInfo(l_oRequest.m_oBody, l_oRespBody))
	{//	查询失败则返回
		ICC_LOG_ERROR(m_pLog,"SearchAlarmVisit failed!");
		return;
	}

	// 查询成功则构建相应并分包发送
	PROTOCOL::CSearchAlarmVisitRespond l_oRespond;
	BuildRespondHeader(l_oRequest.m_oBody.m_strRespCMD, l_oRequest.m_oHeader, l_oRespond.m_oHeader);
	l_oRespond.m_oBody.m_strCount = l_oRespBody.m_strCount;

	if (l_oRespBody.m_strCount == "0")
	{
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	}
	else
	{
		for (auto it = l_oRespBody.m_vecData.cbegin(); it != l_oRespBody.m_vecData.cend(); it++)
		{
			l_oRespond.m_oBody.m_vecData.push_back(*it);

			if (MAX_COUNT == l_oRespond.m_oBody.m_vecData.size())
			{
				std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
				p_pNotify->Response(l_strSendMsg);
				ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

				l_oRespond.m_oBody.m_vecData.clear();
			}
		}

		if (!l_oRespond.m_oBody.m_vecData.empty())
		{
			std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
			p_pNotify->Response(l_strSendMsg);
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

			l_oRespond.m_oBody.m_vecData.clear();
		}
	}

	
}

void CBusinessImpl::OnCNotifiSearchAlarmVisitStatisticRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CSearchAlarmVisitStatisticRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	PROTOCOL::CSearchAlarmVisitStatisticRespond::CBody l_oRespBody;
	if (!SelectChildDeptCount(l_oRequest.m_oBody, l_oRespBody))
	{
		ICC_LOG_ERROR(m_pLog, "SearchAlarmVisitStatisticCount failed!");
		return;
	}	

	// 查询成功则构建相应并分包发送
	PROTOCOL::CSearchAlarmVisitStatisticRespond l_oRespond;
	BuildRespondHeader("search_visit_statistic_info_respond", l_oRequest.m_oHeader, l_oRespond.m_oHeader);	
	l_oRespond.m_oBody.m_strCount = l_oRespBody.m_strCount;

	if (l_oRespBody.m_strCount == "0")
	{
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	}
	else
	{
		if (!SelectChildDept(l_oRequest.m_oBody, l_oRespBody))
		{//	查询失败则返回
			ICC_LOG_ERROR(m_pLog, "SearchAlarmVisitStatistic failed!");
			return;
		}		

		for (auto it = l_oRespBody.m_vecData.cbegin(); it != l_oRespBody.m_vecData.cend(); it++)
		{
			l_oRespond.m_oBody.m_vecData.push_back(*it);

			if (MAX_COUNT == l_oRespond.m_oBody.m_vecData.size())
			{
				std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
				p_pNotify->Response(l_strSendMsg);
				ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

				l_oRespond.m_oBody.m_vecData.clear();
			}
		}

		if (!l_oRespond.m_oBody.m_vecData.empty())
		{
			std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
			p_pNotify->Response(l_strSendMsg);
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

			l_oRespond.m_oBody.m_vecData.clear();
		}
	}

	
}

bool CBusinessImpl::IsSyntInfoValid(const PROTOCOL::CAddOrUpdateAlarmWithProcessRequest::CBody& p_pAlarmSyntInfo)
{
	//TODO::检测警情数据是否有效


	return true;
}

bool CBusinessImpl::IsNewAlarm(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& p_oRequest)
{
	ICC_LOG_DEBUG(m_pLog, "IsNewAlarm enter id:%s", p_oRequest.m_oBody.m_oAlarm.m_strID.c_str());

	if (p_oRequest.m_oBody.m_oAlarm.m_strSourceID.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "source id  is empty id:%s", p_oRequest.m_oBody.m_oAlarm.m_strID.c_str());
		return false;
	}

	if (p_oRequest.m_oBody.m_oAlarm.m_strTransAlarmFlag == "1")
	{
		ICC_LOG_DEBUG(m_pLog, "transalarm id:%s", p_oRequest.m_oBody.m_oAlarm.m_strID.c_str());
		return false;
	}

	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_jjdb";
	l_SqlRequest.param["source_id"] = p_oRequest.m_oBody.m_oAlarm.m_strSourceID;

	std::string strTime = m_pDateTime->GetCallRefIdTime(p_oRequest.m_oBody.m_oAlarm.m_strSourceID);
	if (strTime != "")
	{
		l_SqlRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_SqlRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (!l_pResult->Next())
	{
		ICC_LOG_DEBUG(m_pLog, "result is empty");
		return false;
	}

	if (!p_oRequest.m_oBody.m_oAlarm.ParseAlarmRecord(l_pResult))
	{
		ICC_LOG_ERROR(m_pLog, "Parse record failed.");
		return false;
	}

	//p_oRequest.m_oBody.m_oAlarm.m_strID = l_pResult->GetValue("id");
	//p_oRequest.m_oBody.m_oAlarm.m_strTime = l_pResult->GetValue("time");
	/*std::string strTmp = l_pResult->GetValue("receiving_time");
	if (!strTmp.empty())
	{
		p_oRequest.m_oBody.m_oAlarm.m_strTime = strTmp;
	}*/

	ICC_LOG_DEBUG(m_pLog, "is not new alarm id:%s, bjsj:[%s]", p_oRequest.m_oBody.m_oAlarm.m_strID.c_str(), p_oRequest.m_oBody.m_oAlarm.m_strTime.c_str());
	
	return true;
}

void CBusinessImpl::_ModifyBJSJTime(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& p_oAddSyntRequest)
{
	ICC_LOG_DEBUG(m_pLog, "_ModifyBJSJTime enter, id:[%s]", p_oAddSyntRequest.m_oBody.m_oAlarm.m_strID.c_str());

	do
	{
		if (!p_oAddSyntRequest.m_oBody.m_oAlarm.m_strSourceID.empty())
		{
			DataBase::SQLRequest l_SqlRequest;
			l_SqlRequest.sql_id = "query_incomming_time_from_sub_callevent";
			l_SqlRequest.param["callref_id"] = p_oAddSyntRequest.m_oBody.m_oAlarm.m_strSourceID;

			std::string strTime = m_pDateTime->GetCallRefIdTime(p_oAddSyntRequest.m_oBody.m_oAlarm.m_strSourceID);
			if (strTime != "")
			{
				l_SqlRequest.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
				l_SqlRequest.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
			}

			DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
			ICC_LOG_DEBUG(m_pLog, "query_incomming_time_from_sub_callevent sql:[%s]", l_pResult->GetSQL().c_str());
			DataBase::IResultSetPtr l_pResultringstate;
			if (!l_pResult->IsValid())
			{
				ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
				break;
			}
			std::string strTmp;
			if (!l_pResult->Next())
			{
				ICC_LOG_DEBUG(m_pLog, "query_incomming_time_from_sub_callevent result is empty");
				l_SqlRequest.sql_id = "query_ringstate_time_from_sub_callevent";

				l_pResultringstate = m_pDBConn->Exec(l_SqlRequest);
				ICC_LOG_DEBUG(m_pLog, "query_ringstate_time_from_sub_callevent sql:[%s]", l_pResultringstate->GetSQL().c_str());


				if (!l_pResultringstate->IsValid())
				{
					ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pResultringstate->GetErrorMsg().c_str());
					break;
				}

				if (!l_pResultringstate->Next())
				{
					ICC_LOG_DEBUG(m_pLog, "query_ringstate_time_from_sub_callevent result is empty");
					break;
				}

				if (!l_pResultringstate->GetValue("state_time").empty())
				{
					strTmp = l_pResultringstate->GetValue("state_time");
				}
			}

			if (!l_pResult->GetValue("state_time").empty())
			{
				strTmp = l_pResult->GetValue("state_time");
			}
			
			if (!strTmp.empty())
			{				
				p_oAddSyntRequest.m_oBody.m_oAlarm.m_strTime = strTmp;
			}
		}
	} while (false);

	if (p_oAddSyntRequest.m_oBody.m_oAlarm.m_strTime.empty())
	{
		p_oAddSyntRequest.m_oBody.m_oAlarm.m_strTime = p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceivedTime;
	}

	ICC_LOG_DEBUG(m_pLog, "_ModifyBJSJTime complete, id:[%s], bjsj:[%s]", p_oAddSyntRequest.m_oBody.m_oAlarm.m_strID.c_str(), p_oAddSyntRequest.m_oBody.m_oAlarm.m_strTime.c_str());
}

void CBusinessImpl::ModifyAddAlarmRequest(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& p_oAddSyntRequest)
{
	std::string l_strCurTime = m_pDateTime->CurrentDateTimeStr();
	if (p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode.empty())
	{
		if (!GetDeptInfo(p_oAddSyntRequest))
		{
			ICC_LOG_DEBUG(m_pLog, "Get Dept Info Failed!!!");
		}
	}
	ICC_LOG_DEBUG(m_pLog, "ModifyAddAlarmRequest DeptDistrictCode-[%s] SeatNo-[%s] ReceiptDeptCode-[%s]", p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode.c_str(), p_oAddSyntRequest.m_oBody.m_oAlarm.m_strSeatNo.c_str(), p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptDeptCode.c_str());
	// 服务端填写必要的接警信息
	p_oAddSyntRequest.m_oBody.m_oAlarm.m_strID = _GenAlarmID(p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode, p_oAddSyntRequest.m_oBody.m_oAlarm.m_strSeatNo, p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptDeptCode);//m_pDateTime->CreateSerial();
	p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceivedTime = l_strCurTime;
	p_oAddSyntRequest.m_oBody.m_oAlarm.m_strIsMerge = "0";
	p_oAddSyntRequest.m_oBody.m_oAlarm.m_strCreateUser = p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptCode;
	p_oAddSyntRequest.m_oBody.m_oAlarm.m_strCreateTime = l_strCurTime;
	p_oAddSyntRequest.m_oBody.m_oAlarm.m_strIsSigned = "0";

	_ModifyBJSJTime(p_oAddSyntRequest);
	
	//if (INVALID_ALARM == p_oAddSyntRequest.m_oBody.m_oAlarm.m_strFirstType)
	if (INVALID_ALARM == p_oAddSyntRequest.m_oBody.m_oAlarm.m_strIsInvalid)
	{
		p_oAddSyntRequest.m_oBody.m_oAlarm.m_strState = ALARM_STATUS_TERMINAL;
	}
	else
	{
		p_oAddSyntRequest.m_oBody.m_oAlarm.m_strState = ALARM_STATUS_HANDLING;
	}
}

void CBusinessImpl::ModifyAddSyntRequest(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& p_oAddSyntRequest)
{
	std::string l_strCurTime = m_pDateTime->CurrentDateTimeStr();
	
	// 服务端填写必要的接警信息
	p_oAddSyntRequest.m_oBody.m_oAlarm.m_strID = _GenAlarmID(p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode, p_oAddSyntRequest.m_oBody.m_oAlarm.m_strSeatNo, p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptDeptCode);//m_pDateTime->CreateSerial();
	p_oAddSyntRequest.m_oBody.m_oAlarm.m_strTime = l_strCurTime;
	p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceivedTime = l_strCurTime;
	p_oAddSyntRequest.m_oBody.m_oAlarm.m_strIsMerge = "0";
	p_oAddSyntRequest.m_oBody.m_oAlarm.m_strCreateUser = p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptCode;//p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptName;
	p_oAddSyntRequest.m_oBody.m_oAlarm.m_strCreateTime = l_strCurTime;
	p_oAddSyntRequest.m_oBody.m_oAlarm.m_strUpdateTime = l_strCurTime;
	//if (INVALID_ALARM == p_oAddSyntRequest.m_oBody.m_oAlarm.m_strFirstType)
	if (INVALID_ALARM == p_oAddSyntRequest.m_oBody.m_oAlarm.m_strIsInvalid)
	{
		p_oAddSyntRequest.m_oBody.m_oAlarm.m_strState = ALARM_STATUS_TERMINAL;
	}
	else
	{
		p_oAddSyntRequest.m_oBody.m_oAlarm.m_strState = ALARM_STATUS_HANDLED;//ALARM_STATUS_PROCESSED;
	}
	
	// 服务端填写必要的处警信息
	for (int i = 0; i < p_oAddSyntRequest.m_oBody.m_vecProcessData.size();i++)
	{
		p_oAddSyntRequest.m_oBody.m_vecProcessData.at(i).m_strID = _GenId(p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode);//m_pDateTime->CreateSerial();
		p_oAddSyntRequest.m_oBody.m_vecProcessData.at(i).m_strAlarmID = p_oAddSyntRequest.m_oBody.m_oAlarm.m_strID;
		p_oAddSyntRequest.m_oBody.m_vecProcessData.at(i).m_strState = PROCESS_STATUS_SIGIN;//PROCESS_STATUS_ASSGIN; //webicc 创警，已签收？
		p_oAddSyntRequest.m_oBody.m_vecProcessData.at(i).m_strCreateUser = p_oAddSyntRequest.m_oBody.m_vecProcessData.at(i).m_strDispatchName;
		p_oAddSyntRequest.m_oBody.m_vecProcessData.at(i).m_strCreateTime = l_strCurTime;
	}

	// 服务端填写必要的联动单位
	for (int i = 0; i < p_oAddSyntRequest.m_oBody.m_vecLinkedData.size(); i++)
	{
		BuildLinkedDataInfo(p_oAddSyntRequest.m_oBody.m_vecLinkedData[i]);
		
	}

}

void CBusinessImpl::ModifyUpdateSyntRequest(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& p_oUpdateSyntRequest, const std::string& strTransGuid)
{
	std::string l_strCurTime = m_pDateTime->CurrentDateTimeStr();

	// 服务端填写必要的接警信息
	//p_oUpdateSyntRequest.m_oBody.m_oAlarm.m_strUpdateUser = p_oUpdateSyntRequest.m_oBody.m_oAlarm.m_strReceiptName;
	p_oUpdateSyntRequest.m_oBody.m_oAlarm.m_strUpdateTime = l_strCurTime;

	if (p_oUpdateSyntRequest.m_oBody.m_oAlarm.m_strMsgSource == "vcs_relocated")
	{
		ICC_LOG_DEBUG(m_pLog, "vcs_relocated need not update process info! return");
		return;
	}

	//if (INVALID_ALARM == p_oUpdateSyntRequest.m_oBody.m_oAlarm.m_strFirstType)
	if (INVALID_ALARM == p_oUpdateSyntRequest.m_oBody.m_oAlarm.m_strIsInvalid)
	{
		p_oUpdateSyntRequest.m_oBody.m_oAlarm.m_strState = ALARM_STATUS_TERMINAL;
	}
	else
	{
		//此状态需修改
		if (p_oUpdateSyntRequest.m_oBody.m_vecProcessData.empty())
		{
			p_oUpdateSyntRequest.m_oBody.m_oAlarm.m_strState =
				BuildAlarmState(p_oUpdateSyntRequest.m_oBody.m_oAlarm.m_strID,
					p_oUpdateSyntRequest.m_oBody.m_oAlarm.m_strState, ALARM_STATUS_HANDLED, strTransGuid);//ALARM_STATUS_PROCESSED, strTransGuid);
		}
		else
		{
			//如果派接信息不为空，则表示已调派
			p_oUpdateSyntRequest.m_oBody.m_oAlarm.m_strState =
				BuildAlarmState(p_oUpdateSyntRequest.m_oBody.m_oAlarm.m_strID,
					p_oUpdateSyntRequest.m_oBody.m_oAlarm.m_strState, ALARM_STATUS_PROCESSED, strTransGuid);//ALARM_STATUS_PROCESSED, strTransGuid);
		}
	}

	// 服务端填写必要的处警信息
	for (int i = 0; i < p_oUpdateSyntRequest.m_oBody.m_vecProcessData.size(); i++)
	{
		std::string l_strID = p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strID;
		std::string l_strState = p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strState;
		std::string l_strDispatchDeptCode = p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strDispatchDeptCode;
		std::string l_strProcessDeptCode = p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strProcessDeptCode;
		if (l_strID.empty())
		{
			p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_bIsNewProcess = true;
			p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strID = _GenId(p_oUpdateSyntRequest.m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode);//m_pDateTime->CreateSerial();
			p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strAlarmID = p_oUpdateSyntRequest.m_oBody.m_oAlarm.m_strID;
			//p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strState = PROCESS_STATUS_ASSGIN;
			
			if (p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strState.empty()) //如果为空，则设置为01，否则以上层传的状态为准
			{
				p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strState = PROCESS_STATUS_TAKEN;
			}
			
			p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strCreateUser = p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strDispatchCode;
			//p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strDispatchSuggestion = "";//p_oUpdateSyntRequest.m_oBody.m_oAlarm.m_strDispatchSuggestion;
			p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strCreateTime = l_strCurTime;
			

			if (p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strTimeSubmit.empty())
			{
				p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strTimeSubmit = l_strCurTime;
			}
			//VCS过来的使用VCS派警单的状态  
			// 黄元兵让改，调到本单位也只为已调派, 2022/04/15
			//if ("vcs_relocated" != p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strMsgSource && "vcs" != p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strMsgSource)
			//{
			//	if (l_strDispatchDeptCode.compare(l_strProcessDeptCode) == 0)
			//	{
			//		//调派本单位，自动签收
			//		p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strState = PROCESS_STATUS_SIGIN;
			//		p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strTimeSubmit = l_strCurTime;
			//		p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strTimeSigned = l_strCurTime;
			//	}
			//}
		}
		else
		{
			p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_bIsNewProcess = false;

			//VCS过来的使用VCS派警单的状态
			if ("vcs_relocated" != p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strMsgSource && "vcs" != p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strMsgSource)
			{
				//p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strState = BuildProcessState(l_strID, l_strState, PROCESS_STATUS_TAKEN, strTransGuid);
				p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strState = _GetCurrentProcessState(l_strID, l_strState, strTransGuid);
			}

			p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strUpdateUser = p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strDispatchCode;
			p_oUpdateSyntRequest.m_oBody.m_vecProcessData.at(i).m_strUpdateTime = l_strCurTime;
		}
	}

	//04:自处
	//07:已终结
	bool bIsCancel = true;
	for (int i = 0; i < p_oUpdateSyntRequest.m_oBody.m_vecProcessData.size(); ++i)
	{
		if (p_oUpdateSyntRequest.m_oBody.m_vecProcessData[i].m_strState != "17" && p_oUpdateSyntRequest.m_oBody.m_vecProcessData[i].m_strProcessObjectType != "LinkedUnit")
		{
			bIsCancel = false;
			break;
		}
	}

	if ((bIsCancel || p_oUpdateSyntRequest.m_oBody.m_vecProcessData.empty()) && p_oUpdateSyntRequest.m_oBody.m_oAlarm.m_strState != "04" && p_oUpdateSyntRequest.m_oBody.m_oAlarm.m_strState != ALARM_STATUS_TERMINAL)
	{
		p_oUpdateSyntRequest.m_oBody.m_oAlarm.m_strState = ALARM_STATUS_HANDLED;
	}

	// 服务端填写必要的联动单位信息
	for (int i = 0; i < p_oUpdateSyntRequest.m_oBody.m_vecLinkedData.size(); i++)
	{
		BuildLinkedDataInfo(p_oUpdateSyntRequest.m_oBody.m_vecLinkedData[i]);
	}
}


bool ICC::CBusinessImpl::IsHaveNewProcess(const PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& p_oData)
{
	for (auto data : p_oData.m_oBody.m_vecProcessData)
	{
		if (data.m_strID.empty())
		{
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
// 具体业务处理
bool CBusinessImpl::AddSyntInfo(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& p_oAddSyntRequest, const std::string& strTransGuid)
{
	// 记录警情流水_受理接警单	
	PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
	std::vector<std::string> l_vecParamList;
	l_oAlarmLogInfo.m_strAlarmID = p_oAddSyntRequest.m_oBody.m_oAlarm.m_strID;

	if (p_oAddSyntRequest.m_oBody.m_oAlarm.m_strMsgSource != "vcs" && p_oAddSyntRequest.m_oBody.m_oAlarm.m_strMsgSource != "vcs_relocated")
	{
		Data::CStaffInfo l_oStaffInfo;
		if (!_GetStaffInfo(p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptCode, l_oStaffInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
		}
		std::string l_strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptName);

		l_vecParamList.push_back(l_strStaffName);
		l_vecParamList.push_back(p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptCode);
		l_vecParamList.push_back(p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptDeptName);
		l_oAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
		l_oAlarmLogInfo.m_strCreateUser = p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptName;
		l_oAlarmLogInfo.m_strFromType = p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptSrvName == "2" ? "MPA" : "Staff";

		l_oAlarmLogInfo.m_strOperate = LOG_RECEIPT_ACCEPTED;
		l_oAlarmLogInfo.m_strSourceName = "icc";
		l_oAlarmLogInfo.m_strReceivedTime = p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceivedTime;
		
		AddAlarmLogInfo(l_oAlarmLogInfo);
	}


	//if (INVALID_ALARM != p_oAddSyntRequest.m_oBody.m_oAlarm.m_strFirstType)//无效警情
	if (INVALID_ALARM == p_oAddSyntRequest.m_oBody.m_oAlarm.m_strIsInvalid)
	{// 有效警情流程
		// 接警信息处理
		if (!InsertAlarmInfo(p_oAddSyntRequest.m_oBody))
		{
			return false;
		}

		if (p_oAddSyntRequest.m_oBody.m_oAlarm.m_strMsgSource != "vcs" && p_oAddSyntRequest.m_oBody.m_oAlarm.m_strMsgSource != "vcs_relocated")
		{
			// 记录警情流水_完成接警信息录入
			l_oAlarmLogInfo.m_strOperate = LOG_ALARM_SUBMIT;// LOG_RECEIPT_RECORDED;
			AddAlarmLogInfo(l_oAlarmLogInfo);
		}

		// 处警信息处理
		auto& l_oProcessList = p_oAddSyntRequest.m_oBody.m_vecProcessData;//处警列表的引用
		auto it = l_oProcessList.begin();
		for (; it != l_oProcessList.end(); ++it)
		{

			if (it->m_strMsgSource != "vcs_relocated" && it->m_strMsgSource != "vcs")
			{
				// 记录警情流水_分配处警单
				l_vecParamList.clear();
				Data::CStaffInfo l_oStaffInfo;
				if (!_GetStaffInfo(it->m_strDispatchCode, l_oStaffInfo))
				{
					ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
				}
				std::string l_strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, it->m_strDispatchName);

				l_vecParamList.push_back(l_strStaffName);
				l_vecParamList.push_back(it->m_strDispatchCode);
				l_vecParamList.push_back(it->m_strDispatchDeptName);
				l_vecParamList.push_back(it->m_strProcessDeptName);
				l_oAlarmLogInfo.m_strSeatNo = p_oAddSyntRequest.m_oBody.m_oAlarm.m_strSeatNo;
				l_oAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
				l_oAlarmLogInfo.m_strCreateUser = it->m_strDispatchName;
				l_oAlarmLogInfo.m_strOperate = LOG_PROCESS_ASSIGNED;
				l_oAlarmLogInfo.m_strFromType = "staff";
				l_oAlarmLogInfo.m_strFromObject = it->m_strDispatchCode;
				l_oAlarmLogInfo.m_strFromObjectName = it->m_strDispatchName;
				l_oAlarmLogInfo.m_strFromObjectOrgName = it->m_strDispatchDeptName;
				l_oAlarmLogInfo.m_strFromObjectOrgCode = it->m_strDispatchDeptOrgCode;
				l_oAlarmLogInfo.m_strToType = "org";
				l_oAlarmLogInfo.m_strToObject = it->m_strProcessDeptCode;
				l_oAlarmLogInfo.m_strToObjectName = it->m_strProcessDeptName;
				l_oAlarmLogInfo.m_strToObjectOrgName = it->m_strProcessDeptName;
				l_oAlarmLogInfo.m_strToObjectOrgCode = it->m_strProcessDeptOrgCode;
				l_oAlarmLogInfo.m_strDeptOrgCode = p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptDeptOrgCode;
				l_oAlarmLogInfo.m_strSourceName = "icc";
				l_oAlarmLogInfo.m_strReceivedTime = p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceivedTime;
				AddAlarmLogInfo(l_oAlarmLogInfo);
			}
			
			ICC_LOG_DEBUG(m_pLog, "cur receivedTime:%s", p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceivedTime.c_str());
			it->m_strReceivedTime = p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceivedTime;

			if (!InsertProcessInfo(*it, strTransGuid))
			{
				return false;
			}
		}
		// 联动单位信息处理
		auto& l_oLinkedList = p_oAddSyntRequest.m_oBody.m_vecLinkedData;
		for (auto it = l_oLinkedList.begin(); it != l_oLinkedList.end(); ++it)
		{
			// 有没有不需要记流水的场景
			// 记录警情流水_分配联动单位
			/*
			if (it->m_bIsNewProcess) {
				if (it->m_strMsgSource != "vcs_relocated" && it->m_strMsgSource != "vcs") {
					l_vecParamList.clear();
					l_vecParamList.push_back(it->m_strLinkedOrgName);
					l_vecParamList.push_back(it->m_strLinkedOrgCode);
					l_vecParamList.push_back(it->m_strLinkedOrgType);
					l_vecParamList.push_back(it->m_strDispatchName);
					l_vecParamList.push_back(it->m_strDispatchCode);
					l_oAlarmLogInfo.m_strSeatNo = p_oAddSyntRequest.m_oBody.m_oAlarm.m_strSeatNo;
					l_oAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
					l_oAlarmLogInfo.m_strCreateUser = it->m_strDispatchName;
					l_oAlarmLogInfo.m_strOperate = LOG_PROCESS_ASSIGNED;  // 流水暂定
					l_oAlarmLogInfo.m_strFromType = "staff";
					l_oAlarmLogInfo.m_strFromObject = it->m_strDispatchName;
					l_oAlarmLogInfo.m_strFromObjectName = it->m_strDispatchName;
					l_oAlarmLogInfo.m_strFromObjectOrgCode = it->m_strDispatchCode;
					l_oAlarmLogInfo.m_strToType = "org";
					l_oAlarmLogInfo.m_strToObject = it->m_strLinkedOrgName;
					l_oAlarmLogInfo.m_strToObjectName = it->m_strDispatchName;
					l_oAlarmLogInfo.m_strToObjectOrgName = it->m_strDispatchName;
					l_oAlarmLogInfo.m_strToObjectOrgCode = it->m_strDispatchCode;
					l_oAlarmLogInfo.m_strDeptOrgCode = p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptDeptOrgCode;
					l_oAlarmLogInfo.m_strSourceName = "icc";

					AddAlarmLogInfo(l_oAlarmLogInfo);
				}
			}
			*/
			// 这里要根据警情ID来判断是否是新增还是更新
			if (it->m_bIsNewProcess) {
				if (!AddLinkedProcessInfo(*it))
				{
					return false;
				}
			}
			else {
				if (!UpdateLinkedProcessInfo(*it))
				{
					return false;
				}
			}
		}


		// 记录警情流水_完成处警单	
		//不写BS001002002流水 2022-9-13
		/*
		if (!l_oProcessList.empty())
		{
			it = l_oProcessList.begin();
			if (it->m_strMsgSource != "vcs_relocated" && it->m_strMsgSource != "vcs")
			{
				l_vecParamList.clear();
				l_vecParamList.push_back(it->m_strDispatchName);
				l_vecParamList.push_back(it->m_strDispatchCode);
				l_vecParamList.push_back(it->m_strDispatchDeptName);
				l_oAlarmLogInfo.m_strSeatNo = p_oAddSyntRequest.m_oBody.m_oAlarm.m_strSeatNo;
				l_oAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
				l_oAlarmLogInfo.m_strCreateUser = it->m_strDispatchName;
				l_oAlarmLogInfo.m_strOperate = LOG_PROCESS_COMPLETED;
				l_oAlarmLogInfo.m_strFromType = "staff";
				l_oAlarmLogInfo.m_strFromObject = it->m_strDispatchCode;
				l_oAlarmLogInfo.m_strFromObjectName = it->m_strDispatchName;
				l_oAlarmLogInfo.m_strFromObjectOrgName = it->m_strDispatchDeptName;
				l_oAlarmLogInfo.m_strFromObjectOrgCode = it->m_strDispatchDeptOrgCode;
				l_oAlarmLogInfo.m_strToType = "org";
				l_oAlarmLogInfo.m_strToObject = it->m_strProcessDeptCode;
				l_oAlarmLogInfo.m_strToObjectName = it->m_strProcessDeptName;
				l_oAlarmLogInfo.m_strToObjectOrgName = it->m_strProcessDeptName;
				l_oAlarmLogInfo.m_strToObjectOrgCode = it->m_strProcessDeptOrgCode;
				l_oAlarmLogInfo.m_strDeptOrgCode = p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptDeptOrgCode;
				l_oAlarmLogInfo.m_strSourceName = "icc";

				AddAlarmLogInfo(l_oAlarmLogInfo);
			}
		}
		*/
	}
	else
	{
		// 无效警情流程		
		if (!InsertAlarmInfo(p_oAddSyntRequest.m_oBody))
		{
			return false;
		}

		if (p_oAddSyntRequest.m_oBody.m_oAlarm.m_strMsgSource != "vcs_relocated" && p_oAddSyntRequest.m_oBody.m_oAlarm.m_strMsgSource != "vcs")
		{
			Data::CStaffInfo l_oStaffInfo;
			if (!_GetStaffInfo(p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptCode, l_oStaffInfo))
			{
				ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
			}
			std::string l_strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptName);

			l_vecParamList.push_back(l_strStaffName);
			l_vecParamList.push_back(p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptCode);
			l_vecParamList.push_back(p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptDeptName);
			l_oAlarmLogInfo.m_strSeatNo = p_oAddSyntRequest.m_oBody.m_oAlarm.m_strSeatNo;
			l_oAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
			l_oAlarmLogInfo.m_strCreateUser = p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptName;
			l_oAlarmLogInfo.m_strOperate = LOG_RECEIPT_RECORDED;
			l_oAlarmLogInfo.m_strDeptOrgCode = p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptDeptOrgCode;
			l_oAlarmLogInfo.m_strSourceName = "icc";
			l_oAlarmLogInfo.m_strReceivedTime = p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceivedTime;

			AddAlarmLogInfo(l_oAlarmLogInfo);
		}
	}
	
	return true;
}

bool CBusinessImpl::_UpdateVisitCallType(const std::string& strAlarmId, const std::string& strCallNoType, const std::string& strTransGuid)
{
	DataBase::SQLRequest l_tSQLRequest;
	l_tSQLRequest.sql_id = "update_icc_t_alarm_visit_calltype_by_alarmid";
	l_tSQLRequest.param["call_type"] = strCallNoType;
	l_tSQLRequest.param["alarm_id"] = strAlarmId;
	
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLRequest, false, strTransGuid);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());

	if (!l_pRSet->IsValid())
	{		
		ICC_LOG_ERROR(m_pLog, "update visit calltype failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	
	return true;
}

bool CBusinessImpl::_SetAlarmCallref(const std::string& p_strAlarmID, const std::string& p_strCallrefID)
{
	if (p_strCallrefID.empty())
	{
		return false;
	}
	bool bSuccess = false;
	bool bFlag = false;
	std::string strTransGuid;

	do
	{
		strTransGuid = m_pDBConn->BeginTransaction();

		if (strTransGuid.empty())
		{
			ICC_LOG_ERROR(m_pLog, "_SetAlarmCallref get trans connection failed");

			break;
		}

		std::vector<std::string> l_vecAlarmID;
		m_pString->Split(p_strAlarmID, ",", l_vecAlarmID, true);
		std::size_t l_nAlarmSize = l_vecAlarmID.size();
		//支持批量设置
		if (l_nAlarmSize > 1)
		{
			std::vector<std::string> l_vecCallrefID;
			m_pString->Split(p_strCallrefID, ",", l_vecCallrefID, true);
			std::size_t l_nCallrefSize = l_vecCallrefID.size();
			int l_nSize = l_nAlarmSize;
			if (l_nSize > l_nCallrefSize)
			{
				l_nSize = l_nCallrefSize;
			}
			if (l_nSize > 0)
			{
				bFlag = true;
			}
			for (int i = 0; i < l_nSize; i++)
			{
				DataBase::SQLRequest l_tSQLReq;
				l_tSQLReq.sql_id = "update_icc_t_callevent_alarmid";
				l_tSQLReq.param["callref_id"] = l_vecCallrefID[i];
				l_tSQLReq.param["rel_alarm_id"] = l_vecAlarmID[i];

				std::string strTime = m_pDateTime->GetCallRefIdTime(l_vecCallrefID[i]);
				if (strTime != "")
				{
					l_tSQLReq.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
					l_tSQLReq.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
				}

				DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq);
				if (!l_pRSet->IsValid())
				{
					bSuccess = false;
					ICC_LOG_ERROR(m_pLog, "update callevent rel_alarm_id failed! sql:[%s]", l_pRSet->GetSQL().c_str());
					break;
				}
				else
				{
					bSuccess = true;
					ICC_LOG_DEBUG(m_pLog, "1_update callevent rel_alarm_id success! callrefid:[%s],alarm_id:[%s]", l_vecCallrefID[i].c_str(), l_vecAlarmID[i].c_str());
				}

				// 根据 rel_alarm_id jjdbh 在 icc_t_jjdb 中查询 jjlyh 是否为空 ;
				if (_SelectAlarmIdToSourceId(l_vecAlarmID[i], strTransGuid, bSuccess))
				{
					// jjlyh 为空  把 callref_id 更新到 jjlyh 中
					if (!_UpdateAlarmIdToSourceId(l_vecAlarmID[i], l_vecCallrefID[i], strTransGuid, bSuccess))
					{
						ICC_LOG_DEBUG(m_pLog, "update icc_t_jjdb jjlyh failed! callrefid:[%s]", l_vecCallrefID[i].c_str());
						break;
					}
				}
				
				// 插入或者查询失败，break
				if (!bSuccess)
				{
					break;
				}

			}

			break;
		}

		if (!bFlag)
		{
			DataBase::SQLRequest l_tSQLReq;
			l_tSQLReq.sql_id = "update_icc_t_callevent_alarmid";
			l_tSQLReq.param["callref_id"] = p_strCallrefID;
			l_tSQLReq.param["rel_alarm_id"] = p_strAlarmID;

			std::string strTime = m_pDateTime->GetCallRefIdTime(p_strCallrefID);
			if (strTime != "")
			{
				l_tSQLReq.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
				l_tSQLReq.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
			}

			DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq);
			if (!l_pRSet->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "update callevent rel_alarm_id failed! sql:[%s]", l_pRSet->GetSQL().c_str());
			}
			else
			{
				// 关联警单
				// 根据 rel_alarm_id jjdbh 在 icc_t_jjdb 中查询 jjlyh 是否为空 ;
				if (_SelectAlarmIdToSourceId(p_strAlarmID, strTransGuid, bSuccess))
				{
					// jjlyh 为空  把 callref_id 更新到 jjlyh 中
					if (!_UpdateAlarmIdToSourceId(p_strAlarmID, p_strCallrefID, strTransGuid, bSuccess))
					{
						bSuccess = false;
						ICC_LOG_DEBUG(m_pLog, "update icc_t_jjdb jjlyh failed! callrefid:[%s]", p_strCallrefID.c_str());
					}
				}
				
				ICC_LOG_DEBUG(m_pLog, "update callevent rel_alarm_id success! callrefid:[%s]", p_strCallrefID.c_str());
			}
		}
	} while (false);

	if (bSuccess)
	{
		m_pDBConn->Commit(strTransGuid);

	}
	else if (!strTransGuid.empty())
	{
		m_pDBConn->Rollback(strTransGuid);
	}

	return bSuccess;
}

bool CBusinessImpl::_DeleteAlarmCallref(const std::string& p_strAlarmID, const std::string& p_strCallrefID)
{
	if (p_strCallrefID.empty())
	{
		return false;
	}
	bool bSuccess = false;
	bool bFlag = false;
	std::string strTransGuid;

	do
	{
		strTransGuid = m_pDBConn->BeginTransaction();

		if (strTransGuid.empty())
		{
			ICC_LOG_ERROR(m_pLog, "_SetAlarmCallref get trans connection failed");

			break;
		}

		std::vector<std::string> l_vecAlarmID;
		m_pString->Split(p_strAlarmID, ",", l_vecAlarmID, true);
		std::size_t l_nAlarmSize = l_vecAlarmID.size();
		//支持批量设置
		if (l_nAlarmSize > 1)
		{
			std::vector<std::string> l_vecCallrefID;
			m_pString->Split(p_strCallrefID, ",", l_vecCallrefID, true);
			std::size_t l_nCallrefSize = l_vecCallrefID.size();
			int l_nSize = l_nAlarmSize;
			if (l_nSize > l_nCallrefSize)
			{
				l_nSize = l_nCallrefSize;
			}
			if (l_nSize > 0)
			{
				bFlag = true;
			}

			for (int i = 0; i < l_nSize; i++)
			{
				DataBase::SQLRequest l_tSQLReq;
				l_tSQLReq.sql_id = "update_icc_t_callevent_alarmid";
				l_tSQLReq.param["callref_id"] = l_vecCallrefID[i];
				l_tSQLReq.param["rel_alarm_id"] = l_vecAlarmID[i];

				std::string strTime = m_pDateTime->GetCallRefIdTime(l_vecCallrefID[i]);
				if (strTime != "")
				{
					l_tSQLReq.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
					l_tSQLReq.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
				}

				DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq);
				if (!l_pRSet->IsValid())
				{
					bSuccess = false;
					ICC_LOG_ERROR(m_pLog, "update callevent rel_alarm_id failed! sql:[%s]", l_pRSet->GetSQL().c_str());
					break;
				}
				else
				{
					bSuccess = true;
					ICC_LOG_DEBUG(m_pLog, "1_update callevent rel_alarm_id success! callrefid:[%s],alarm_id:[%s]", l_vecCallrefID[i].c_str(), l_vecAlarmID[i].c_str());
				}

				//取消关联警单
				// 根据 callref_id jjlyh 在 icc_t_jjdb 中 更新 
				if (!_UpdateSourceId(l_vecCallrefID[i], strTransGuid, bSuccess))
				{
					break;
				}

				// 插入或者查询失败，break
				if (!bSuccess)
				{
					break;
				}

			}

			break;
		}

		if (!bFlag)
		{
			DataBase::SQLRequest l_tSQLReq;
			l_tSQLReq.sql_id = "update_icc_t_callevent_alarmid";
			l_tSQLReq.param["callref_id"] = p_strCallrefID;
			l_tSQLReq.param["rel_alarm_id"] = "";

			std::string strTime = m_pDateTime->GetCallRefIdTime(p_strCallrefID);
			if (strTime != "")
			{
				l_tSQLReq.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
				l_tSQLReq.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
			}

			DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq);
			if (!l_pRSet->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "update callevent rel_alarm_id failed! sql:[%s]", l_pRSet->GetSQL().c_str());
			}
			else
			{
				//取消关联警单
				// 根据 callref_id jjlyh 在 icc_t_jjdb 中 更新 
				if (!_UpdateSourceId(p_strCallrefID, strTransGuid, bSuccess))
				{
					break;
				}
	
				ICC_LOG_DEBUG(m_pLog, "update callevent rel_alarm_id success! callrefid:[%s]", p_strCallrefID.c_str());
			}
		}
	} while (false);

	if (bSuccess)
	{
		m_pDBConn->Commit(strTransGuid);

	}
	else if (!strTransGuid.empty())
	{
		m_pDBConn->Rollback(strTransGuid);
	}

	return bSuccess;
}


bool CBusinessImpl::_SelectAlarmIdToSourceId(const std::string& p_strAlarmId, const std::string& p_strTransGuid, bool& p_bSuccess)
{
	bool bFlag = false;
	p_bSuccess = false;
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_jjdb_for_jjlyh";
	l_SqlRequest.param["jjdbh"] = p_strAlarmId;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, false, p_strTransGuid);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		p_bSuccess = false;
		bFlag = false;
	}

	if (l_pResult->Next())
	{
		std::string l_strSourceId = l_pResult->GetValue("jjlyh");
		ICC_LOG_DEBUG(m_pLog, "p_strAlarmID is [%s] l_strSourceId is [%s]", p_strAlarmId.c_str(), l_strSourceId.c_str());
		if (l_strSourceId.empty())
		{
			bFlag = true;
		}
		p_bSuccess = true;
	}

	return bFlag;
}

bool CBusinessImpl::_SelectAlarmIdToFirstType(const std::string& p_strAlarmId)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_jjdb";
	l_SqlRequest.param["id"] = p_strAlarmId;

	std::string strTime = m_pDateTime->GetAlarmIdTime(p_strAlarmId);
	if (strTime != "")
	{
		l_SqlRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_SqlRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (l_pResult->Next())
	{
		std::string l_strFirstType = l_pResult->GetValue("initial_first_type");
		ICC_LOG_DEBUG(m_pLog, "[%s] update_time is [%s]", p_strAlarmId.c_str(), l_strFirstType.c_str());
		if (l_strFirstType.empty())
		{
			return true;
		}
	}
	return false;
}

bool CBusinessImpl::_UpdateAlarmIdToSourceId(const std::string& p_strAlarmId, const std::string& p_strSourceId, const std::string& p_strTransGuid, bool& p_bSuccess)
{
	p_bSuccess = false;
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "update_icc_t_jjdb_for_jjdbh";
	l_SqlRequest.param["jjdbh"] = p_strAlarmId;
	l_SqlRequest.param["jjlyh"] = p_strSourceId;
	l_SqlRequest.param["gxsj"] = m_pDateTime->CurrentDateTimeStr();

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, false, p_strTransGuid);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		p_bSuccess = false;
		ICC_LOG_ERROR(m_pLog, "update callevent p_strAlarmID to p_strSourceId failed! sql:[%s]", l_pResult->GetSQL().c_str());
	}
	else
	{
		p_bSuccess = true;
		ICC_LOG_DEBUG(m_pLog, "1_update callevent p_strAlarmID success! alarm_id:[%s],p_strSourceId:[%s]", p_strAlarmId.c_str(), p_strSourceId.c_str());
	}

	return p_bSuccess;
}

bool CBusinessImpl::_UpdateSourceId(const std::string& p_strSourceId, const std::string& p_strTransGuid, bool& p_bSuccess)
{
	p_bSuccess = false;
	std::string l_strRel_SourceId = "";
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "update_icc_t_jjdb_for_jjlyh";
	l_SqlRequest.param["rel_jjlyh"] = l_strRel_SourceId;
	l_SqlRequest.param["jjlyh"] = p_strSourceId;
	l_SqlRequest.param["gxsj"] = m_pDateTime->CurrentDateTimeStr();
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, false, p_strTransGuid);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		p_bSuccess = false;
		ICC_LOG_ERROR(m_pLog, "update callevent p_strAlarmID to p_strSourceId failed! sql:[%s]", l_pResult->GetSQL().c_str());
	}
	else
	{
		p_bSuccess = true;
		ICC_LOG_DEBUG(m_pLog, "1_update callevent p_strAlarmID success! p_strSourceId:[%s]", p_strSourceId.c_str());
	}

	return p_bSuccess;

}

bool CBusinessImpl::_UpdateReviveInfo(PROTOCOL::CReviveAlarmRequest::CBody& p_oReviveAlarmBody, std::string& strTransGuid, std::string & p_strErrMsg)
{
	PROTOCOL::CAddOrUpdateAlarmWithProcessRequest tmp_oAlarm;
	if (!GetAlarm(p_oReviveAlarmBody.m_strID, tmp_oAlarm))
	{
		p_strErrMsg = "Get alarm id failed";
		ICC_LOG_ERROR(m_pLog, "Get alarm id failed, id: %s", p_oReviveAlarmBody.m_strID.c_str());
		return false;
	}
	PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
	if (tmp_oAlarm.m_oBody.m_oAlarm.m_strIsInvalid == "1")
	{
		l_oAlarmLogInfo.m_strOperate = LOG_ALARM_ENABLE;
	}
	else if (tmp_oAlarm.m_oBody.m_oAlarm.m_strState == "04")
	{
		l_oAlarmLogInfo.m_strOperate = LOG_ALARM_REVIVE;
	}
	tmp_oAlarm.m_oBody.m_oAlarm.m_strHandleType = p_oReviveAlarmBody.m_strHandleType;
	tmp_oAlarm.m_oBody.m_oAlarm.m_strIsInvalid = "0";  //设置为有效
	tmp_oAlarm.m_oBody.m_oAlarm.m_strState = "00";    //警情状态改为受理中
	tmp_oAlarm.m_oBody.m_oAlarm.m_strUpdateUser = p_oReviveAlarmBody.m_strOperateCode;
	tmp_oAlarm.m_oBody.m_oAlarm.m_strUpdateTime = p_oReviveAlarmBody.m_strOperateTime;

	// 更新前备份该接警数据
	std::string l_strReceiptResourceID = m_pString->CreateGuid();
	
	strTransGuid = m_pDBConn->BeginTransaction();

	int iModifyField = 0;
	if (!InsertAlarmBookBeforeUpdate(tmp_oAlarm.m_oBody.m_oAlarm, l_strReceiptResourceID, strTransGuid, iModifyField))
	{
		p_strErrMsg = "insert alarm book before update failed";
		return false;
	}

	if (!UpdateAlarmInfo(tmp_oAlarm.m_oBody, strTransGuid))
	{
		p_strErrMsg = "update alarm failed";
		return false;
	}
	// ICC重启警单 同步更新门市告警状态
	if (!UpdateCallProcessInfo(tmp_oAlarm.m_oBody, tmp_oAlarm.m_oBody.m_oAlarm.m_strSourceID))
	{
		p_strErrMsg = "update alarm failed";
		return false;
	}

	//编辑流水
	std::string l_strUserName(p_oReviveAlarmBody.m_strOperateName);
	std::string l_strDeptName(p_oReviveAlarmBody.m_strOperateDeptOrgName);
	std::string l_strDeptCode(p_oReviveAlarmBody.m_strOperateDeptOrgCode);
	std::string l_strUserCode(p_oReviveAlarmBody.m_strOperateCode);

	if (l_strDeptCode.empty() || l_strDeptName.empty())
	{
		GetStaffDept(l_strUserCode, l_strUserName, l_strDeptName, l_strDeptCode);
	}

	//GetStaffDept(l_strUserCode, l_strUserName, l_strDeptName, l_strDeptCode);
	
	l_oAlarmLogInfo.m_strAlarmID = tmp_oAlarm.m_oBody.m_oAlarm.m_strID;
	std::vector<std::string> l_vecParamList;

	Data::CStaffInfo l_oStaffInfo;
	if (!_GetStaffInfo(l_strUserCode, l_oStaffInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
	}
	std::string l_strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_strUserName);
	l_vecParamList.push_back(l_strStaffName);
	l_vecParamList.push_back(l_strUserCode);
	l_vecParamList.push_back(l_strDeptName);
	l_oAlarmLogInfo.m_strSeatNo = tmp_oAlarm.m_oBody.m_oAlarm.m_strSeatNo;
	l_oAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
	l_oAlarmLogInfo.m_strCreateUser = l_strUserName;
	if (!l_strReceiptResourceID.empty())
	{
		std::string type = RESOURCETYPE_ALARM;
		l_oAlarmLogInfo.m_strOperateAttachDesc = BuildAlarmLogAttach(type, l_strReceiptResourceID);
	}
	l_oAlarmLogInfo.m_strDeptOrgCode = p_oReviveAlarmBody.m_strOperateDeptOrgCode;
	l_oAlarmLogInfo.m_strSourceName = "icc";
	
	if (!AddAlarmLogInfo(l_oAlarmLogInfo, strTransGuid))
	{
		p_strErrMsg = "add alarm log failed";
		return false;
	}

	SyncAlarmInfo(tmp_oAlarm.m_oBody.m_oAlarm, ESyncType::ADD);

	_SyncKeyWordAlarmInfo(tmp_oAlarm.m_oBody.m_oAlarm, false);
	return true;
}

bool CBusinessImpl::UpdateSyntInfo(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& l_oUpdateAlarmRequest, const std::string& strTransGuid, int& iModifyField, std::string& strGuidId)
{
	// 更新前备份该接警数据
	std::string l_strReceiptResourceID = m_pString->CreateGuid();
	if (InsertAlarmBookBeforeUpdate(l_oUpdateAlarmRequest.m_oBody.m_oAlarm, l_strReceiptResourceID, strTransGuid, iModifyField))
	{//更新该接警数据
		std::string l_strUserName = "";
		std::string l_strDeptName = "";
		std::string l_strDeptCode = "";
		std::string l_strUserCode = "";
		if (!UpdateAlarmInfo(l_oUpdateAlarmRequest.m_oBody, strTransGuid))
		{
			ICC_LOG_ERROR(m_pLog, "UpdateAlarmInfo failed.");
			return false;
		}
		else
		{
			std::string l_strStaffName;
			//更新后备份该接警数据
			//可视化重定位写警情日志,非人工编辑警单不需要写编辑日志
			if (l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strMsgSource == "vcs_relocated")
			{
				//2022/02/28  与黄元兵确认VCS过来的不写警情日志，找VCS同步警情日志
				//std::vector<std::string> ParamList;
				//PROTOCOL::CAlarmLogSync::CBody LogInfo;
				//LogInfo.m_strAlarmID = l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strID;
				//ParamList.push_back(l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strOperatorName);
				//ParamList.push_back(l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strOperatorCode);
				//ParamList.push_back(l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strLongitude);
				//ParamList.push_back(l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strLatitude);
				//LogInfo.m_strContent = BuildAlarmLogContent(ParamList);
				//LogInfo.m_strCreateUser = l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strReceiptName;
				//LogInfo.m_strResourceID = l_strReceiptResourceID;
				//LogInfo.m_strCode = LOG_VCS_EDIT;
				//AddAlarmLogInfo(LogInfo, strTransGuid);
				return true;
			}
			else
			{
				//接警流水
				l_strUserName = l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUserName;
				l_strDeptName = l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUserDeptName;
				l_strDeptCode = l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUserDeptCode;
				l_strUserCode = l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUser;
				
				if (l_strDeptName.empty() || l_strDeptCode.empty())
				{
					GetStaffDept(l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUser, l_strUserName, l_strDeptName, l_strDeptCode);
				}

				//GetStaffDept(l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUser, l_strUserName, l_strDeptName, l_strDeptCode);
				PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
				l_oAlarmLogInfo.m_strAlarmID = l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strID;
				std::vector<std::string> l_vecParamList;

				Data::CStaffInfo l_oStaffInfo;
				if (!_GetStaffInfo(l_strUserCode, l_oStaffInfo))
				{
					ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
				}
				std::string l_strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_strUserName);

				l_vecParamList.push_back(l_strStaffName);
				l_vecParamList.push_back(l_strUserCode);
				l_vecParamList.push_back(l_strDeptName);
			
				if (!l_oUpdateAlarmRequest.m_oBody.m_strSupplement.empty())
				{
					l_vecParamList.push_back(l_oUpdateAlarmRequest.m_oBody.m_strSupplement);
				}

				l_oAlarmLogInfo.m_strSeatNo = l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strSeatNo;
				l_oAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
				l_oAlarmLogInfo.m_strCreateUser = l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strReceiptName;
				l_oAlarmLogInfo.m_strReceivedTime = l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strReceivedTime;
				if (l_oUpdateAlarmRequest.m_oBody.m_strIsEdit == "1")
				{
					std::string type = RESOURCETYPE_LOG_INFO;
					l_oAlarmLogInfo.m_strOperateAttachDesc = BuildAlarmLogAttach(type, strGuidId);
				}
				
				if (l_strReceiptResourceID.empty())
				{
					l_oAlarmLogInfo.m_strOperate = LOG_ALARM_SUBMIT;
				}
				else
				{
					l_oAlarmLogInfo.m_strOperate = LOG_ALARM_EDIT;
					if (!l_oUpdateAlarmRequest.m_oBody.m_strSupplement.empty())
					{
						l_oAlarmLogInfo.m_strOperate = LOG_ALARM_SUPPLEMENT;
					}
				}

				bool l_bWriteLog = true;
				if (l_oAlarmLogInfo.m_strOperate == LOG_ALARM_EDIT)
				{
					if (l_oUpdateAlarmRequest.m_oBody.m_strIsEdit != "1")
					{
						l_bWriteLog = false;
					}
				}

				if (l_bWriteLog)
				{
					//l_oAlarmLogInfo.m_strOperate = l_strReceiptResourceID.empty() ? LOG_ALARM_SUBMIT : LOG_ALARM_EDIT;
					l_oAlarmLogInfo.m_strDeptOrgCode = l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strReceiptDeptOrgCode;
					l_oAlarmLogInfo.m_strSourceName = "icc";
					AddAlarmLogInfo(l_oAlarmLogInfo, strTransGuid);
				}

				if (l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strHandleTypeCode == "2") {
					l_oAlarmLogInfo.m_strFromType = l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strReceiptSrvName == "2" ? "MPA" : "Staff";
					l_oAlarmLogInfo.m_strOperate = LOG_RECEIPT_ACCEPTED;
					l_oAlarmLogInfo.m_strDeptOrgCode = l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strReceiptDeptOrgCode;
					l_oAlarmLogInfo.m_strSourceName = "icc";
					AddAlarmLogInfo(l_oAlarmLogInfo, strTransGuid);
				}
				
			}
			if (l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strIsInvalid.compare("1")==0)
			{
				PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
				std::vector<std::string> l_vecParamList;
				if (l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUserName.empty()) {
					l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUserName = l_strStaffName;
				}
				if (l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUser.empty()) {
					l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUser = l_strUserCode;
				}
				if (l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUserDeptName.empty()) {
					l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUserDeptName = l_strDeptName;
				}
				l_vecParamList.push_back(l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUserName);
				l_vecParamList.push_back(l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUser);
				l_vecParamList.push_back(l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUserDeptName);
				l_oAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
				l_oAlarmLogInfo.m_strOperate = LOG_ALARM_INVALID;
				l_oAlarmLogInfo.m_strAlarmID = l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strID;
				l_oAlarmLogInfo.m_strSeatNo = l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strSeatNo;;
				l_oAlarmLogInfo.m_strCreateUser = l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strCreateUser;
				l_oAlarmLogInfo.m_strSourceName = "icc";

				AddAlarmLogInfo(l_oAlarmLogInfo, strTransGuid);
			}
		}
	}

	//模拟一个处警单信息
	//if (l_oUpdateAlarmRequest.m_oBody.m_vecProcessData.empty() && l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strMsgSource == "" && l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strFirstType != INVALID_ALARM && l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strHandleType == "01")//"DIC002001")
	if (l_oUpdateAlarmRequest.m_oBody.m_vecProcessData.empty() && l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strMsgSource == "" && l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strIsInvalid != INVALID_ALARM)//"DIC002001")
	{
		ICC_LOG_DEBUG(m_pLog, "[%s]xxxx AddOrUpdateAlarmAndProcessRequest 3.5", l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strID.c_str());
		//AnalogProcessData(l_oUpdateAlarmRequest);
	}

	//更新接警数据库数据成功后，更新或插入处警记录
	auto& l_rvecProcessData = l_oUpdateAlarmRequest.m_oBody.m_vecProcessData;//处警列表的引用
	auto l_itProcessData = l_rvecProcessData.begin();
	for (; l_itProcessData != l_rvecProcessData.end(); ++l_itProcessData)
	{
		// 已退单的的警单不可以再调派（如需再调派，则需要新建警单）
		if (PROCESS_STATUS_RETURN == l_itProcessData->m_strState)
		{
			continue;
		}

		PROTOCOL::CAlarmLogSync::CBody l_oProcessAlarmLogInfo;
		std::vector<std::string> l_vecParamList;
		// 记录警情流水_分配处警单
		//l_vecParamList.push_back(l_itProcessData->m_strDispatchName);
		//l_vecParamList.push_back(l_itProcessData->m_strDispatchCode);
		//l_vecParamList.push_back(l_itProcessData->m_strDispatchDeptName);
		Data::CStaffInfo l_oStaffInfo;
		if (!_GetStaffInfo(l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUser, l_oStaffInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
		}
		std::string l_strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUserName);
		l_vecParamList.push_back(l_strStaffName);
		l_vecParamList.push_back(l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUser);
		l_vecParamList.push_back(l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUserDeptName);
		l_oProcessAlarmLogInfo.m_strAlarmID = l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strID;
		l_oProcessAlarmLogInfo.m_strSeatNo = l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strSeatNo;
		l_oProcessAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
		l_oProcessAlarmLogInfo.m_strCreateUser = l_itProcessData->m_strDispatchName;
		l_oProcessAlarmLogInfo.m_strFromType = "staff";
		l_oProcessAlarmLogInfo.m_strFromObject = l_itProcessData->m_strDispatchCode;
		l_oProcessAlarmLogInfo.m_strFromObjectName = l_itProcessData->m_strDispatchName;
		l_oProcessAlarmLogInfo.m_strFromObjectOrgName = l_itProcessData->m_strDispatchDeptName;
		l_oProcessAlarmLogInfo.m_strFromObjectOrgCode = l_itProcessData->m_strDispatchDeptOrgCode;
		l_oProcessAlarmLogInfo.m_strToType = "org";
		l_oProcessAlarmLogInfo.m_strToObject = l_itProcessData->m_strProcessDeptCode;
		l_oProcessAlarmLogInfo.m_strToObjectName = l_itProcessData->m_strProcessDeptName;
		l_oProcessAlarmLogInfo.m_strToObjectOrgName = l_itProcessData->m_strProcessDeptName;
		l_oProcessAlarmLogInfo.m_strToObjectOrgCode = l_itProcessData->m_strProcessDeptOrgCode;
		l_oProcessAlarmLogInfo.m_strDeptOrgCode = l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strReceiptDeptOrgCode;
		l_oProcessAlarmLogInfo.m_strSourceName = "icc";
		l_oProcessAlarmLogInfo.m_strReceivedTime = l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strReceivedTime;
		if (!l_itProcessData->m_bIsNewProcess)
		{
			// 更新前备份该处警数据
			std::string l_strResourceID = m_pString->CreateGuid();
			//if (!InsertProcessBookBeforeUpdate(*l_itProcessData, l_strResourceID, strTransGuid))
			//{
			//	continue;
			//}

			std::string l_strSaveState = "";
			if (l_itProcessData->m_strState == "01")
			{
				l_strSaveState = _GetCurrentProcessState(l_itProcessData->m_strID, "", "");
			}
			
			// 更新该处警数据
			if (!UpdateProcessInfo(*l_itProcessData, strTransGuid))
			{
				return false;
			}

			//删除调派后再调派
			if (l_strSaveState == "17")
			{
				// 记录警情流水_分配处警单
				l_vecParamList.clear();
				//l_vecParamList.push_back(l_itProcessData->m_strDispatchName);
				//l_vecParamList.push_back(l_itProcessData->m_strDispatchCode);
				//l_vecParamList.push_back(l_itProcessData->m_strDispatchDeptName);
				Data::CStaffInfo l_oStaffInfo;
				if (!_GetStaffInfo(l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUser, l_oStaffInfo))
				{
					ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
				}
				std::string l_strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUserName);

				l_vecParamList.push_back(l_strStaffName);
				l_vecParamList.push_back(l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUser);
				l_vecParamList.push_back(l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUserDeptName);

				l_vecParamList.push_back(l_itProcessData->m_strProcessDeptName);
				l_oProcessAlarmLogInfo.m_strOperate = LOG_PROCESS_ASSIGNED;
				l_oProcessAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
				l_oProcessAlarmLogInfo.m_strFromType = "staff";
				l_oProcessAlarmLogInfo.m_strFromObject = l_itProcessData->m_strDispatchCode;
				l_oProcessAlarmLogInfo.m_strFromObjectName = l_itProcessData->m_strDispatchName;
				l_oProcessAlarmLogInfo.m_strFromObjectOrgName = l_itProcessData->m_strDispatchDeptName;
				l_oProcessAlarmLogInfo.m_strFromObjectOrgCode = l_itProcessData->m_strDispatchDeptOrgCode;
				l_oProcessAlarmLogInfo.m_strToType = "org";
				l_oProcessAlarmLogInfo.m_strToObject = l_itProcessData->m_strProcessDeptCode;
				l_oProcessAlarmLogInfo.m_strToObjectName = l_itProcessData->m_strProcessDeptName;
				l_oProcessAlarmLogInfo.m_strToObjectOrgName = l_itProcessData->m_strProcessDeptName;
				l_oProcessAlarmLogInfo.m_strToObjectOrgCode = l_itProcessData->m_strProcessDeptOrgCode;
				l_oProcessAlarmLogInfo.m_strSourceName = "icc";
		
				//vcs过来的不写流水
				if (l_itProcessData->m_strMsgSource != "vcs_relocated" && l_itProcessData->m_strMsgSource != "vcs")
				{
					bool l_bRes = AddAlarmLogInfo(l_oProcessAlarmLogInfo);

					ICC_LOG_DEBUG(m_pLog, "Update icc_t_alarm_log LOG_PROCESS_ASSIGNED ,ID:%s,res:%d", l_itProcessData->m_strID.c_str(), l_bRes);

					// 删除调派后再调派，更新成功,走完成处警流水
					l_vecParamList.pop_back();
					//不写BS001002002流水 2022-9-13
					//l_oProcessAlarmLogInfo.m_strOperate = LOG_PROCESS_COMPLETED;
					//AddAlarmLogInfo(l_oProcessAlarmLogInfo);
				}
				
			}

			//vcs过来的不写库
			if (l_itProcessData->m_strMsgSource != "vcs_relocated" && l_itProcessData->m_strMsgSource != "vcs")
			{

				// 如果state值为DIC020071，则修改警情状态为已取消
				if (l_itProcessData->m_strState == PROCESS_STATUS_CANCEL && l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strIsInvalid.compare("1") !=0)
				{// 记录警情流水_取消调派		

					l_strSaveState = _GetCurrentProcessState(l_itProcessData->m_strID, "", "");
					if (l_strSaveState != PROCESS_STATUS_CANCEL)
					{
						l_vecParamList.push_back(l_itProcessData->m_strProcessDeptName);
						l_vecParamList.push_back(l_itProcessData->m_strCancelReason);

						l_oProcessAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
						l_oProcessAlarmLogInfo.m_strOperate = LOG_DISPATCH_CANCELLED;
						AddAlarmLogInfo(l_oProcessAlarmLogInfo, strTransGuid);

						//删除最后两个
						l_vecParamList.pop_back();
						l_vecParamList.pop_back();
					}	
					else
					{
						ICC_LOG_INFO(m_pLog, "state is PROCESS_STATUS_CANCEL, not need write alarm log!");
					}
				}
			}

			//vcs过来的不写库
			if (l_itProcessData->m_strMsgSource != "vcs_relocated" && l_itProcessData->m_strMsgSource != "vcs")
			{
				if (!l_itProcessData->m_strProcessFeedback.empty())
				{
					l_vecParamList.push_back(l_itProcessData->m_strProcessFeedback);
					if (!l_strResourceID.empty())
					{
						std::string type = RESOURCETYPE_ALARM;
						l_oProcessAlarmLogInfo.m_strOperateAttachDesc = BuildAlarmLogAttach(type, l_strResourceID);
					}

					l_oProcessAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
					//l_oProcessAlarmLogInfo.m_strCreateUser = l_itProcessData->m_strDispatchName;
					l_oProcessAlarmLogInfo.m_strCreateUser = l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUser;
					
					l_oProcessAlarmLogInfo.m_strOperate = LOG_FEEDBACK_COMMON;
					l_oProcessAlarmLogInfo.m_strSourceName = "icc";
					AddAlarmLogInfo(l_oProcessAlarmLogInfo, strTransGuid);
				}
			}
		}
		else
		{
			//vcs过来的不写库
			if (l_itProcessData->m_strMsgSource != "vcs_relocated" && l_itProcessData->m_strMsgSource != "vcs")
			{
				// 记录警情流水_分配处警单
				l_vecParamList.clear();
				//l_vecParamList.push_back(l_itProcessData->m_strDispatchName);
				//l_vecParamList.push_back(l_itProcessData->m_strDispatchCode);
				//l_vecParamList.push_back(l_itProcessData->m_strDispatchDeptName);
				Data::CStaffInfo l_oStaffInfo;
				if (!_GetStaffInfo(l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUser, l_oStaffInfo))
				{
					ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
				}
				std::string l_strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUserName);
				l_vecParamList.push_back(l_strStaffName);
				l_vecParamList.push_back(l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUser);
				l_vecParamList.push_back(l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUserDeptName);

				l_vecParamList.push_back(l_itProcessData->m_strProcessDeptName);
				l_oProcessAlarmLogInfo.m_strOperate = LOG_PROCESS_ASSIGNED;
				l_oProcessAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
				l_oProcessAlarmLogInfo.m_strFromType = "staff";
				l_oProcessAlarmLogInfo.m_strFromObject = l_itProcessData->m_strDispatchCode;
				l_oProcessAlarmLogInfo.m_strFromObjectName = l_itProcessData->m_strDispatchName;
				l_oProcessAlarmLogInfo.m_strFromObjectOrgName = l_itProcessData->m_strDispatchDeptName;
				l_oProcessAlarmLogInfo.m_strFromObjectOrgCode = l_itProcessData->m_strDispatchDeptOrgCode;
				l_oProcessAlarmLogInfo.m_strToType = "org";
				l_oProcessAlarmLogInfo.m_strToObject = l_itProcessData->m_strProcessDeptCode;
				l_oProcessAlarmLogInfo.m_strToObjectName = l_itProcessData->m_strProcessDeptName;
				l_oProcessAlarmLogInfo.m_strToObjectOrgName = l_itProcessData->m_strProcessDeptName;
				l_oProcessAlarmLogInfo.m_strToObjectOrgCode = l_itProcessData->m_strProcessDeptOrgCode;
				l_oProcessAlarmLogInfo.m_strSourceName = "icc";
		
				AddAlarmLogInfo(l_oProcessAlarmLogInfo, strTransGuid);
			}

			l_itProcessData->m_strReceivedTime = l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strReceivedTime;

			if (!InsertProcessInfo(*l_itProcessData, strTransGuid))
			{
				return false;
			}

			//vcs过来的不写库
			if (l_itProcessData->m_strMsgSource != "vcs_relocated" && l_itProcessData->m_strMsgSource != "vcs")
			{
				// 插入成功，走完成处警流水
				l_vecParamList.pop_back();
				//不写BS001002002流水 2022-9-13
				//l_oProcessAlarmLogInfo.m_strOperate = LOG_PROCESS_COMPLETED;
				//AddAlarmLogInfo(l_oProcessAlarmLogInfo, strTransGuid);
			}
			
		}
	}

	//更新联动单位数据库数据成功后，更新或插入联动单位记录
	auto& l_LinkedData = l_oUpdateAlarmRequest.m_oBody.m_vecLinkedData;		//联动单位列表的引用
	for (auto l_itData = l_LinkedData.begin(); l_itData != l_LinkedData.end(); ++l_itData)
	{
		// 已退单的的警单不可以再调派（如需再调派，则需要新建警单）
		if (LINKED_STATUS_CANCEL == l_itData->m_strState || LINKED_STATUS_INVALID == l_itData->m_strState)
		{
			continue;
		}
		if (l_itData->m_bIsNewProcess) {
			PROTOCOL::CAlarmLogSync::CBody l_oProcessAlarmLogInfo;
			std::vector<std::string> l_vecParamList;
			// 记录联动单位流水
			Data::CStaffInfo l_oStaffInfo;
			if (!_GetStaffInfo(l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUser, l_oStaffInfo))
			{
				ICC_LOG_DEBUG(m_pLog, "get staff info failed! ");
			}
			std::string l_strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUserName);
			l_vecParamList.push_back(l_strStaffName);
			l_vecParamList.push_back(l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUser);
			l_vecParamList.push_back(l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strUpdateUserDeptName);
			l_vecParamList.push_back(l_itData->m_strLinkedOrgName);
			l_oProcessAlarmLogInfo.m_strOperate = LOG_LINKED_DISPATCH;
			l_oProcessAlarmLogInfo.m_strAlarmID = l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strID;
			l_oProcessAlarmLogInfo.m_strSeatNo = l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strSeatNo;
			l_oProcessAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
			l_oProcessAlarmLogInfo.m_strCreateUser = l_itData->m_strDispatchName;
			l_oProcessAlarmLogInfo.m_strFromType = "staff";
			l_oProcessAlarmLogInfo.m_strFromObject = l_itData->m_strDispatchCode;
			l_oProcessAlarmLogInfo.m_strFromObjectName = l_itData->m_strDispatchName;
			l_oProcessAlarmLogInfo.m_strFromObjectOrgName = l_itData->m_strLinkedOrgName;
			l_oProcessAlarmLogInfo.m_strFromObjectOrgCode = l_itData->m_strLinkedOrgCode;
			l_oProcessAlarmLogInfo.m_strToType = "org";
			l_oProcessAlarmLogInfo.m_strToObject = l_itData->m_strLinkedOrgCode;
			l_oProcessAlarmLogInfo.m_strToObjectName = l_itData->m_strLinkedOrgName;
			l_oProcessAlarmLogInfo.m_strToObjectOrgName = l_itData->m_strLinkedOrgName;
			l_oProcessAlarmLogInfo.m_strToObjectOrgCode = l_itData->m_strLinkedOrgCode;
			l_oProcessAlarmLogInfo.m_strDeptOrgCode = l_oUpdateAlarmRequest.m_oBody.m_oAlarm.m_strReceiptDeptOrgCode;
			l_oProcessAlarmLogInfo.m_strSourceName = "icc";
			AddAlarmLogInfo(l_oProcessAlarmLogInfo, strTransGuid);
		}
		
		if (!l_itData->m_bIsNewProcess)
		{
			// 更新前备份该联动单位 的数据

			// 更新联动单位数据
			if (!UpdateLinkedProcessInfo(*l_itData))
			{
				return false;
			}
		}
		else
		{
			if (!AddLinkedProcessInfo(*l_itData))
			{
				return false;
			}
		}
	}
	
	return true;
}

bool CBusinessImpl::AddAlarmLogInfo(PROTOCOL::CAlarmLogSync::CBody& p_pAlarmLogInfo, const std::string& strTransGuid /* = "" */)
{
	PROTOCOL::CAlarmLogSync::CBody l_oData;
	//std::string l_strCurTime(m_pDateTime->ToString(m_pDateTime->CurrentDateTime(), DateTime::DATETIME_STRING_FORMAT_NO_MICROSECOND));
	//std::string l_strCurTime(m_pDateTime->CurrentDateTimeStr());
	//使用带毫秒的流水
	std::string l_strCurTime(m_pDateTime->ToString(m_pDateTime->CurrentDateTime(), DateTime::DEFAULT_DATETIME_STRING_FORMAT));
	std::string l_strGUID(m_pString->CreateGuid());

	if (p_pAlarmLogInfo.m_strID.empty())
	{
		l_oData.m_strID = l_strGUID;
	}
	else
	{
		l_oData.m_strID = p_pAlarmLogInfo.m_strID;
	}

	l_oData.m_strAlarmID = p_pAlarmLogInfo.m_strAlarmID;
	l_oData.m_strProcessID = p_pAlarmLogInfo.m_strProcessID;
	l_oData.m_strFeedbackID = p_pAlarmLogInfo.m_strFeedbackID;
	l_oData.m_strSeatNo = p_pAlarmLogInfo.m_strSeatNo;
	l_oData.m_strOperate = p_pAlarmLogInfo.m_strOperate;
	l_oData.m_strOperateContent = p_pAlarmLogInfo.m_strOperateContent;
	l_oData.m_strFromType = p_pAlarmLogInfo.m_strFromType;
	l_oData.m_strFromObject = p_pAlarmLogInfo.m_strFromObject;
	l_oData.m_strFromObjectName = p_pAlarmLogInfo.m_strFromObjectName;
	l_oData.m_strFromObjectOrgName = p_pAlarmLogInfo.m_strFromObjectOrgName;
	l_oData.m_strFromObjectOrgCode = p_pAlarmLogInfo.m_strFromObjectOrgCode;
	l_oData.m_strToType = p_pAlarmLogInfo.m_strToType;
	l_oData.m_strToObject = p_pAlarmLogInfo.m_strToObject;
	l_oData.m_strToObjectName = p_pAlarmLogInfo.m_strToObjectName;
	l_oData.m_strToObjectOrgName = p_pAlarmLogInfo.m_strToObjectOrgName;
	l_oData.m_strToObjectOrgCode = p_pAlarmLogInfo.m_strToObjectOrgCode;
	l_oData.m_strCreateUser = p_pAlarmLogInfo.m_strCreateUser;
	l_oData.m_strCreateTime = p_pAlarmLogInfo.m_strCreateTime.empty() ? l_strCurTime : p_pAlarmLogInfo.m_strCreateTime;
	l_oData.m_strDeptOrgCode = p_pAlarmLogInfo.m_strDeptOrgCode;
	l_oData.m_strSourceName = p_pAlarmLogInfo.m_strSourceName;
	l_oData.m_strOperateAttachDesc = p_pAlarmLogInfo.m_strOperateAttachDesc;
	l_oData.m_strReceivedTime = p_pAlarmLogInfo.m_strReceivedTime;

	if (!InsertAlarmLogInfo(l_oData, strTransGuid))
	{
		ICC_LOG_DEBUG(m_pLog, "Insert icc_t_alarm_log failed!");
		return false;
	}

	SyncAlarmLogInfo(l_oData);
	return true;
}

void CBusinessImpl::BuildRespondHeader(std::string p_strCmd, const PROTOCOL::CHeaderEx& p_pRequestHeader, PROTOCOL::CHeaderEx& p_pRespHeader)
{
	p_pRespHeader = p_pRequestHeader;
	
	p_pRespHeader.m_strResult = "0";
	p_pRespHeader.m_strSystemID = SYSTEMID;
	p_pRespHeader.m_strSubsystemID = SUBSYSTEMID;
	p_pRespHeader.m_strMsgid = m_pString->CreateGuid();
	p_pRespHeader.m_strRelatedID = p_pRequestHeader.m_strMsgid;
	p_pRespHeader.m_strCmd = p_strCmd;
	p_pRespHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_pRespHeader.m_strRequest = p_pRequestHeader.m_strResponse;
	p_pRespHeader.m_strRequestType = p_pRequestHeader.m_strResponseType;
	p_pRespHeader.m_strResponse = "";
	p_pRespHeader.m_strResponseType = "";

	p_pRespHeader.m_strRequestFlag = p_pRequestHeader.m_strRequestFlag;
}

void CBusinessImpl::BuildRespondHeader(std::string p_strCmd, PROTOCOL::CHeader& p_oSyncHeader)
{
	p_oSyncHeader.m_strSystemID = SYSTEMID;
	p_oSyncHeader.m_strSubsystemID = SUBSYSTEMID;
	p_oSyncHeader.m_strMsgid = m_pString->CreateGuid();
	p_oSyncHeader.m_strRelatedID = "";
	p_oSyncHeader.m_strCmd = p_strCmd;
	p_oSyncHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	p_oSyncHeader.m_strRequest = "topic_alarm_sync";
	p_oSyncHeader.m_strRequestType = "1";
	p_oSyncHeader.m_strResponse = "";
	p_oSyncHeader.m_strResponseType = "";
}

bool CBusinessImpl::BuildAddAlarmRespond(PROTOCOL::CAddOrUpdateAlarmWithProcessRespond& p_oRespond, PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& p_oRequest)
{
	if (!IsNewAlarm(p_oRequest) ||( p_oRequest.m_oHeader.m_strSubsystemID == "ICC_SERVER_TRANSPORT"&& !p_oRequest.m_oBody.m_oAlarm.m_strSourceID.empty()))
	{
		ModifyAddAlarmRequest(p_oRequest);
		if (p_oRequest.m_oBody.m_oAlarm.m_strReceiptDeptCode.empty() || p_oRequest.m_oBody.m_oAlarm.m_strReceiptCode.empty()) {
			ICC_LOG_ERROR(m_pLog, "ReceiptDeptCode or ReceiptCode is empty [%s-%s]!!!", p_oRequest.m_oBody.m_oAlarm.m_strReceiptDeptCode.c_str(), p_oRequest.m_oBody.m_oAlarm.m_strReceiptCode.c_str());
			return false;
		}
		//修改电话报警的字典类型
		if(!p_oRequest.m_oBody.m_oAlarm.m_strSourceID.empty() && p_oRequest.m_oBody.m_oAlarm.m_strSourceType.compare("01") == 0) //DIC001001
		{
			//vcs过来的不写库
			if (p_oRequest.m_oBody.m_oAlarm.m_strMsgSource != "vcs_relocated" && p_oRequest.m_oBody.m_oAlarm.m_strMsgSource != "vcs")
			{
				//--By z13060 2018/08/25 12:01 不存在话务信息，只影响是否写警情流水
				DataBase::SQLRequest l_SqlRequest;
				l_SqlRequest.sql_id = "select_icc_t_callevent";
				l_SqlRequest.param["callref_id"] = p_oRequest.m_oBody.m_oAlarm.m_strSourceID;

				std::string strTime = m_pDateTime->GetCallRefIdTime(p_oRequest.m_oBody.m_oAlarm.m_strSourceID);
				if (strTime != "")
				{
					l_SqlRequest.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
					l_SqlRequest.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
				}

				DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
				ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

				if (l_pResult->Next())
				{
					PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
					l_oAlarmLogInfo.m_strAlarmID = p_oRequest.m_oBody.m_oAlarm.m_strID;
					std::vector<std::string> l_vecParamList;
					l_vecParamList.push_back(l_pResult->GetValue("caller_id"));
					l_vecParamList.push_back(l_pResult->GetValue("called_id"));
					std::string strTalkTime = l_pResult->GetValue("talk_time");
					if (!strTalkTime.empty())
					{
						l_vecParamList.push_back(strTalkTime);
						p_oRespond.m_oBody.m_strTime = strTalkTime;
					}
					else
					{
						l_vecParamList.push_back(p_oRequest.m_oBody.m_oAlarm.m_strReceivedTime);
					}
					l_oAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
					std::string type = RESOURCETYPE_CALLREF;
					l_oAlarmLogInfo.m_strOperateAttachDesc = BuildAlarmLogAttach(type, p_oRequest.m_oBody.m_oAlarm.m_strSourceID);
					l_oAlarmLogInfo.m_strCreateUser = p_oRequest.m_oBody.m_oAlarm.m_strReceiptCode;
					l_oAlarmLogInfo.m_strOperate = LOG_CALL_INCOMMING;
					l_oAlarmLogInfo.m_strSourceName = "icc";
					l_oAlarmLogInfo.m_strReceivedTime = p_oRequest.m_oBody.m_oAlarm.m_strReceivedTime;

					if (p_oRequest.m_oBody.m_oAlarm.m_strTransAlarmFlag == "1" && !(l_pResult->GetValue("hangup_time").empty()))
					{
						//转警并且是挂机后的转警，不写流水。
					}
					else
					{
						AddAlarmLogInfo(l_oAlarmLogInfo);
					}
				}
			}
			
		}		

		if (!InsertAlarmInfo(p_oRequest.m_oBody))
		{
			return false;
		}	

		//vcs过来的不写库
		if (p_oRequest.m_oBody.m_oAlarm.m_strMsgSource != "vcs_relocated" && p_oRequest.m_oBody.m_oAlarm.m_strMsgSource != "vcs")
		{
			PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
			l_oAlarmLogInfo.m_strAlarmID = p_oRequest.m_oBody.m_oAlarm.m_strID;
			std::vector<std::string> l_vecParamList;
			Data::CStaffInfo l_oStaffInfo;
			if (!_GetStaffInfo(p_oRequest.m_oBody.m_oAlarm.m_strReceiptCode, l_oStaffInfo))
			{
				ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
			}
			std::string strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, p_oRequest.m_oBody.m_oAlarm.m_strReceiptName);
			l_vecParamList.push_back(strStaffName);
			l_vecParamList.push_back(p_oRequest.m_oBody.m_oAlarm.m_strReceiptCode);
			l_vecParamList.push_back(p_oRequest.m_oBody.m_oAlarm.m_strReceiptDeptName);
			l_oAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
			l_oAlarmLogInfo.m_strCreateUser = p_oRequest.m_oBody.m_oAlarm.m_strReceiptName;
			l_oAlarmLogInfo.m_strFromType = p_oRequest.m_oBody.m_oAlarm.m_strReceiptSrvName == "2" ? "MPA" : "Staff";
			l_oAlarmLogInfo.m_strOperate = LOG_RECEIPT_ACCEPTED;
			l_oAlarmLogInfo.m_strSourceName = "icc";
			l_oAlarmLogInfo.m_strReceivedTime = p_oRequest.m_oBody.m_oAlarm.m_strReceivedTime;
			AddAlarmLogInfo(l_oAlarmLogInfo);
		}
	}
	else
	{
		p_oRespond.m_oBody.m_strNoCreate = "1";
	}


	p_oRespond.m_oBody.m_strID = p_oRequest.m_oBody.m_oAlarm.m_strID;

	if (p_oRespond.m_oBody.m_strTime.empty())
	{
		p_oRespond.m_oBody.m_strTime = p_oRequest.m_oBody.m_oAlarm.m_strReceivedTime;
	}

	p_oRespond.m_oBody.m_strBJTime = p_oRequest.m_oBody.m_oAlarm.m_strTime;
	if (p_oRespond.m_oBody.m_strBJTime.empty())
	{
		p_oRespond.m_oBody.m_strBJTime = p_oRequest.m_oBody.m_oAlarm.m_strReceivedTime;
	}	

	return true;
}

bool CBusinessImpl::BuildAddSignRespond(PROTOCOL::CAddSignRespond& p_oRespond, const PROTOCOL::CAddSignRequest& p_oRequest)
{
	BuildRespondHeader("add_alarm_sign_respond", p_oRequest.m_oHeader, p_oRespond.m_oHeader);

	DataBase::SQLRequest l_tSQLRequest;
	l_tSQLRequest.sql_id = "update_icc_t_pjdb";
	l_tSQLRequest.param["id"] = p_oRequest.m_oBody.m_strID;
	std::string strTime = m_pDateTime->GetDispatchIdTime(p_oRequest.m_oBody.m_strID);
	if (strTime != "")
	{
		l_tSQLRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 86400 * 15);
		l_tSQLRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	l_tSQLRequest.set["state"] = PROCESS_STATUS_SIGIN;
	l_tSQLRequest.set["time_signed"] = m_pDateTime->CurrentDateTimeStr();
	l_tSQLRequest.set["process_dept_code"] = p_oRequest.m_oBody.m_strProcessDeptCode;
	l_tSQLRequest.set["process_dept_name"] = p_oRequest.m_oBody.m_strProcessDeptName;
	l_tSQLRequest.set["process_code"] = p_oRequest.m_oBody.m_strProcessCode;
	l_tSQLRequest.set["process_name"] = p_oRequest.m_oBody.m_strProcessName;
	l_tSQLRequest.set["process_leader_code"] = p_oRequest.m_oBody.m_strProcessLeaderCode;
	l_tSQLRequest.set["process_leader_name"] = p_oRequest.m_oBody.m_strProcessLeaderName;
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());

	if (!l_pRSet->IsValid())
	{
		p_oRespond.m_oHeader.m_strResult = "1";
		p_oRespond.m_oHeader.m_strMsg = "update_icc_t_pjdb failed";
		//p_oRespond.m_oBody.m_strResult = "1";
		ICC_LOG_ERROR(m_pLog, "update alarm process info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	//p_oRespond.m_oBody.m_strResult = "0";
	return true;
}

bool CBusinessImpl::BuildGetSignRespond(PROTOCOL::CGetSignRespond::CBody& p_oRespondBody, const PROTOCOL::CGetSignRequest& p_oRequest)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_jjdb_by_process_dept_code";
	l_SqlRequest.param["process_dept_code"] = p_oRequest.m_oBody.m_strProcessDeptCode;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	while (l_pResult->Next())
	{
		PROTOCOL::CAlarmInfo l_oAlarm;
		//l_oAlarm.m_strID = l_pResult->GetValue("id");
		//l_oAlarm.m_strMergeID = l_pResult->GetValue("merge_id");
		//l_oAlarm.m_strTitle = l_pResult->GetValue("title");
		//l_oAlarm.m_strContent = l_pResult->GetValue("content");
		//l_oAlarm.m_strTime = l_pResult->GetValue("time");
		////l_oAlarm.m_strActualOccurTime = l_pResult->GetValue("actual_occur_time");
		//l_oAlarm.m_strAddr = l_pResult->GetValue("addr");
		//l_oAlarm.m_strLongitude = l_pResult->GetValue("longitude");
		//l_oAlarm.m_strLatitude = l_pResult->GetValue("latitude");
		//l_oAlarm.m_strState = l_pResult->GetValue("state");
		//l_oAlarm.m_strLevel = l_pResult->GetValue("level");
		//l_oAlarm.m_strSourceType = l_pResult->GetValue("source_type");
		//l_oAlarm.m_strSourceID = l_pResult->GetValue("source_id");
		//l_oAlarm.m_strHandleType = l_pResult->GetValue("handle_type");
		//l_oAlarm.m_strFirstType = l_pResult->GetValue("first_type");
		//l_oAlarm.m_strSecondType = l_pResult->GetValue("second_type");
		//l_oAlarm.m_strThirdType = l_pResult->GetValue("third_type");
		//l_oAlarm.m_strFourthType = l_pResult->GetValue("fourth_type");
		//l_oAlarm.m_strVehicleNo = l_pResult->GetValue("vehicle_no");
		//l_oAlarm.m_strVehicleType = l_pResult->GetValue("vehicle_type");
		//l_oAlarm.m_strSymbolCode = l_pResult->GetValue("symbol_code");
		////l_oAlarm.m_strSymbolAddr = l_pResult->GetValue("symbol_addr");
		////l_oAlarm.m_strFireBuildingType = l_pResult->GetValue("fire_building_type");
		////l_oAlarm.m_strEventType = l_pResult->GetValue("event_type");
		//l_oAlarm.m_strCalledNoType = l_pResult->GetValue("called_no_type");
		////l_oAlarm.m_strActualCalledNoType = l_pResult->GetValue("actual_called_no_type");
		//l_oAlarm.m_strCallerNo = l_pResult->GetValue("caller_no");
		//l_oAlarm.m_strCallerName = l_pResult->GetValue("caller_name");
		//l_oAlarm.m_strCallerAddr = l_pResult->GetValue("caller_addr");
		//l_oAlarm.m_strCallerID = l_pResult->GetValue("caller_id");
		//l_oAlarm.m_strCallerIDType = l_pResult->GetValue("caller_id_type");
		//l_oAlarm.m_strCallerGender = l_pResult->GetValue("caller_gender");
		////l_oAlarm.m_strCallerAge = l_pResult->GetValue("caller_age");
		////l_oAlarm.m_strCallerBirthday = l_pResult->GetValue("caller_birthday");
		//l_oAlarm.m_strContactNo = l_pResult->GetValue("contact_no");
		////l_oAlarm.m_strContactName = l_pResult->GetValue("contact_name");
		////l_oAlarm.m_strContactAddr = l_pResult->GetValue("contact_addr");
		////l_oAlarm.m_strContactID = l_pResult->GetValue("contact_id");
		////l_oAlarm.m_strContactIDType = l_pResult->GetValue("contact_id_type");
		////l_oAlarm.m_strContactGender = l_pResult->GetValue("contact_gender");
		////l_oAlarm.m_strContactAge = l_pResult->GetValue("contact_age");
		////l_oAlarm.m_strContactBirthday = l_pResult->GetValue("contact_birthday");
		////l_oAlarm.m_strAdminDeptDistrictCode = l_pResult->GetValue("admin_dept_district_code");
		//l_oAlarm.m_strAdminDeptCode = l_pResult->GetValue("admin_dept_code");
		//l_oAlarm.m_strAdminDeptName = l_pResult->GetValue("admin_dept_name");
		//l_oAlarm.m_strReceiptDeptDistrictCode = l_pResult->GetValue("receipt_dept_district_code");
		//l_oAlarm.m_strReceiptDeptCode = l_pResult->GetValue("receipt_dept_code");
		//l_oAlarm.m_strReceiptDeptName = l_pResult->GetValue("receipt_dept_name");
		////l_oAlarm.m_strLeaderCode = l_pResult->GetValue("leader_code");
		////l_oAlarm.m_strLeaderName = l_pResult->GetValue("leader_name");
		//l_oAlarm.m_strReceiptCode = l_pResult->GetValue("receipt_code");
		//l_oAlarm.m_strReceiptName = l_pResult->GetValue("receipt_name");
		////l_oAlarm.m_strDispatchSuggestion = l_pResult->GetValue("dispatch_suggestion");
		//l_oAlarm.m_strCreateUser = l_pResult->GetValue("create_user");
		//l_oAlarm.m_strCreateTime = l_pResult->GetValue("create_time");
		//l_oAlarm.m_strUpdateUser = l_pResult->GetValue("update_user");
		//l_oAlarm.m_strUpdateTime = l_pResult->GetValue("update_time");
		//l_oAlarm.m_strPrivacy = l_pResult->GetValue("is_privacy");
		//l_oAlarm.m_strRemark = l_pResult->GetValue("remark");

		if (!l_oAlarm.ParseAlarmRecord(l_pResult))
		{
			ICC_LOG_ERROR(m_pLog, "Parse record failed.");
		}
		
		p_oRespondBody.m_vecAlarm.push_back(l_oAlarm);
	}

	return true;
}

bool CBusinessImpl::BuildAddRemarkRespond(PROTOCOL::CAddAlarmRemarkRespond& p_oRespond, const PROTOCOL::CAddAlarmRemarkRequestEx& p_oRequest)
{
	BuildRespondHeader("add_alarm_remark_respond", p_oRequest.m_oHeader, p_oRespond.m_oHeader);

	DataBase::SQLRequest l_oSetRemarkSQL;
	l_oSetRemarkSQL.sql_id = "insert_icc_t_alarm_remark";
	l_oSetRemarkSQL.param["guid"] = m_pString->CreateGuid();
	l_oSetRemarkSQL.param["alarm_id"] = p_oRequest.m_oBody.m_strAlarmID;
	l_oSetRemarkSQL.param["feedback_code"] = p_oRequest.m_oBody.m_strFeedBackCode;
	l_oSetRemarkSQL.param["feedback_name"] = p_oRequest.m_oBody.m_strFeedBackName;
	l_oSetRemarkSQL.param["feedback_dept_code"] = p_oRequest.m_oBody.m_strFeedBackDeptCode;
	l_oSetRemarkSQL.param["feedback_dept_name"] = p_oRequest.m_oBody.m_strFeedBackDeptName;
	l_oSetRemarkSQL.param["feedback_time"] = p_oRequest.m_oBody.m_strFeedBackTime;
	l_oSetRemarkSQL.param["content"] = p_oRequest.m_oBody.m_strContent;
	l_oSetRemarkSQL.param["create_user"] = p_oRequest.m_oBody.m_strFeedBackName;
	l_oSetRemarkSQL.param["create_time"] = p_oRequest.m_oBody.m_strCreateTime;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_oSetRemarkSQL);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());

	if (!l_pRSet->IsValid())
	{
		p_oRespond.m_oHeader.m_strResult = "1";//失败
		ICC_LOG_ERROR(m_pLog, "Add AlarmRemark Failed:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	else
	{
		p_oRespond.m_oHeader.m_strResult = "0";//成功
	}

	return true;
}

bool CBusinessImpl::BuildGetRemarkRespond(PROTOCOL::CGetAlarmRemarkRespond& p_oRespond, const PROTOCOL::CGetAlarmRemarkRequest& p_oRequest)
{
	BuildRespondHeader("get_alarm_remark_respond", p_oRequest.m_oHeader, p_oRespond.m_oHeader);

	DataBase::SQLRequest l_oGetRemarkSQL;
	l_oGetRemarkSQL.sql_id = "select_icc_t_alarm_remark";
	l_oGetRemarkSQL.param["guid"] = p_oRequest.m_oBody.m_strGuid;

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_oGetRemarkSQL);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		p_oRespond.m_oHeader.m_strResult = "1";
		p_oRespond.m_oHeader.m_strMsg = "execute select_icc_t_alarm_remark failed";
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		return false;
	}
	if (l_result->Next())
	{
		p_oRespond.m_oBody.m_strAlarmID = l_result->GetValue("alarm_id");
		p_oRespond.m_oBody.m_strFeedBackCode = l_result->GetValue("feedback_code");
		p_oRespond.m_oBody.m_strFeedBackCode = l_result->GetValue("feedback_name");
		p_oRespond.m_oBody.m_strFeedBackDeptCode = l_result->GetValue("feedback_dept_code");
		p_oRespond.m_oBody.m_strFeedBackCode = l_result->GetValue("feedback_dept_name");
		p_oRespond.m_oBody.m_strFeedBackTime = l_result->GetValue("feedback_time");
		p_oRespond.m_oBody.m_strContent = l_result->GetValue("content");
	}

	return true;
}

bool CBusinessImpl::BuildGetAllRemarkRespond(PROTOCOL::CGetAlarmAllRemarkRespond& p_oRespond, const PROTOCOL::CGetAlarmAllRemarkRequest& p_oRequest)
{
	BuildRespondHeader("get_alarm_all_remark_respond", p_oRequest.m_oHeader, p_oRespond.m_oHeader);

	DataBase::SQLRequest l_oGetAllRemarkSQL;
	l_oGetAllRemarkSQL.sql_id = "select_icc_t_alarm_remark";
	l_oGetAllRemarkSQL.param["alarm_id"] = p_oRequest.m_oBody.m_strAlarmId;

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_oGetAllRemarkSQL);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
	if (!l_result->IsValid())
	{
		p_oRespond.m_oHeader.m_strResult = "1";
		p_oRespond.m_oHeader.m_strMsg = "execute select_icc_t_alarm_remark failed";
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		return false;
	}
	while (l_result->Next())
	{
		PROTOCOL::CGetAlarmAllRemarkRespond::CData l_oData;
		l_oData.m_strAlarmID = l_result->GetValue("alarm_id");
		l_oData.m_strFeedBackCode = l_result->GetValue("feedback_code");
		l_oData.m_strFeedBackCode = l_result->GetValue("feedback_name");
		l_oData.m_strFeedBackDeptCode = l_result->GetValue("feedback_dept_code");
		l_oData.m_strFeedBackCode = l_result->GetValue("feedback_dept_name");
		l_oData.m_strFeedBackTime = l_result->GetValue("feedback_time");
		l_oData.m_strContent = l_result->GetValue("content");

		p_oRespond.m_oBody.m_vecData.push_back(l_oData);
	}

	return true;
}

bool CBusinessImpl::BuildGetAlarmLogRespond(PROTOCOL::CGetAlarmLogRespond& p_oRespond, const PROTOCOL::CGetAlarmLogRequest& p_oRequest)
{
	BuildRespondHeader("get_alarm_log_respond", p_oRequest.m_oHeader, p_oRespond.m_oHeader);

	if (!SelectAlarmLogByAlarmID(p_oRequest, p_oRespond.m_oBody))
	{
		p_oRespond.m_oHeader.m_strResult = "1";
		p_oRespond.m_oHeader.m_strMsg = "execute select_icc_t_alarm_log failed";
		ICC_LOG_DEBUG(m_pLog, "SelectDBAlarmLogByAlarmID failed!");
		return false;
	}

	return true;
}

bool CBusinessImpl::BuildGetAlarmLogAlarmRespond(PROTOCOL::CGetAlarmLogAlarmResRespond& p_oRespond, const PROTOCOL::CGetAlarmLogAlarmResRequest& p_oRequest)
{
	BuildRespondHeader("get_alarm_log_alarm_respond", p_oRequest.m_oHeader, p_oRespond.m_oHeader);

	if (!SelectAlarmBookByResID(p_oRequest.m_oBody.m_strResourceID, p_oRespond.m_oBody.m_vData))
	{
		p_oRespond.m_oHeader.m_strResult = "1";
		p_oRespond.m_oHeader.m_strMsg = "execute select_icc_t_alarm_book failed";
		return false;
	}

	return true;
}

bool CBusinessImpl::BuildGetAlarmLogProcessRespond(PROTOCOL::CGetAlarmProcLogResRespond& p_oRespond, const PROTOCOL::CGetAlarmProcLogResRequest& p_oRequest)
{
	BuildRespondHeader("get_alarm_log_process_respond", p_oRequest.m_oHeader, p_oRespond.m_oHeader);

	if (!SelectProcessBookByResID(p_oRequest.m_oBody.m_strResourceID, p_oRespond.m_oBody.m_vData))
	{
		p_oRespond.m_oHeader.m_strResult = "1";
		p_oRespond.m_oHeader.m_strMsg = "execute select_icc_t_alarm_process_book failed";
		return false;
	}

	return true;
}

bool CBusinessImpl::BuildGetAlarmLogCallrefRespond(PROTOCOL::CGetAlarmLogCallrefRespond& p_oRespond, const PROTOCOL::CGetAlarmLogCallrefRequest& p_oRequest)
{
	BuildRespondHeader("get_alarm_log_callref_respond", p_oRequest.m_oHeader, p_oRespond.m_oHeader);

	// select_icc_t_callevent
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_callevent";
	l_SqlRequest.param["callref_id"] = p_oRequest.m_oBody.m_strResourceID;

	std::string strTime = m_pDateTime->GetCallRefIdTime(p_oRequest.m_oBody.m_strResourceID);
	if (strTime != "")
	{
		l_SqlRequest.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
		l_SqlRequest.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		p_oRespond.m_oHeader.m_strResult = "1";
		p_oRespond.m_oHeader.m_strMsg = "execute select_icc_t_callevent failed";
		ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (!l_pResult->Next())
	{
		p_oRespond.m_oHeader.m_strResult = "2";
		p_oRespond.m_oHeader.m_strMsg = "no record";
		ICC_LOG_DEBUG(m_pLog, "result is empty");
		return false;
	}
#define GETVALUE(DestName,SourceName)\
 p_oRespond.m_oBody.m_oCallEvent.DestName = l_pResult->GetValue(SourceName);

	GETVALUE(m_strCallrefID, "callref_id");
	GETVALUE(m_strACD, "acd");
	GETVALUE(m_strCallerID, "caller_id");
	GETVALUE(m_strCalledID, "called_id");
	GETVALUE(m_strCallDirection, "call_direction");
	GETVALUE(m_strTalkTime, "talk_time");
	GETVALUE(m_strHangupTime, "hangup_time");
	GETVALUE(m_strHangupType, "hangup_type");

	// select_icc_t_sub_callevent
	DataBase::SQLRequest l_SqlSubRequest;
	l_SqlSubRequest.sql_id = "select_icc_t_sub_callevent";
	l_SqlSubRequest.param["callref_id"] = p_oRequest.m_oBody.m_strResourceID;

	strTime = m_pDateTime->GetCallRefIdTime(p_oRequest.m_oBody.m_strResourceID);
	if (strTime != "")
	{
		l_SqlSubRequest.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
		l_SqlSubRequest.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	DataBase::IResultSetPtr l_pSubResult = m_pDBConn->Exec(l_SqlSubRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pSubResult->GetSQL().c_str());

	if (!l_pSubResult->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pSubResult->GetErrorMsg().c_str());
		return false;
	}

	while (l_pSubResult->Next())
	{
		PROTOCOL::CGetAlarmLogCallrefRespond::CSUBCALLEVENT l_oSubCallEvent;

		l_oSubCallEvent.m_strEventCallState = l_pSubResult->GetValue("eventcall_state", "");
		l_oSubCallEvent.m_strStateTime = l_pSubResult->GetValue("state_time");
		l_oSubCallEvent.m_strSponsor = l_pSubResult->GetValue("sponsor");
		l_oSubCallEvent.m_strReceiver = l_pSubResult->GetValue("receiver");

		p_oRespond.m_oBody.m_vecSubCallEvent.push_back(l_oSubCallEvent);
	}

	return true;
}

bool CBusinessImpl::BuildMergeAlarmRespond(PROTOCOL::CAddMergeRespond& p_oRespond, const PROTOCOL::CAddMergeRequest& p_oRequest)
{
	BuildRespondHeader("merge_alarm_respond", p_oRequest.m_oHeader, p_oRespond.m_oHeader);

	// 1.查询是否已经合并到其他警单，若是，则返回失败
	{
		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = "select_icc_t_jjdb";
		l_SqlRequest.param["id"] = p_oRequest.m_oBody.m_strAlarmID;

		std::string strTime = m_pDateTime->GetAlarmIdTime(p_oRequest.m_oBody.m_strAlarmID);
		if (strTime != "")
		{
			l_SqlRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
			l_SqlRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
		}

		DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

		if (!l_pResult->IsValid())
		{
			p_oRespond.m_oHeader.m_strResult = "1";
			p_oRespond.m_oHeader.m_strMsg = "execute select_icc_t_alarm failed";
			//p_oRespond.m_oBody.m_strResult = "1";
			ICC_LOG_ERROR(m_pLog, "select_icc_t_alarm failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
			return false;
		}

		if (!l_pResult->Next())
		{
			p_oRespond.m_oHeader.m_strResult = "2";
			p_oRespond.m_oHeader.m_strMsg = "no alarm record";
			//p_oRespond.m_oBody.m_strResult = "1";
			ICC_LOG_DEBUG(m_pLog, "result is null");
			return false;
		}

		std::string l_strMerageID = l_pResult->GetValue("merge_id");
		if ((!l_strMerageID.empty()) &&
			(l_strMerageID.compare(p_oRequest.m_oBody.m_strMergeID) != 0))
		{
			p_oRespond.m_oHeader.m_strResult = "3";
			p_oRespond.m_oHeader.m_strMsg = "the alarm has merged to other alarm";
			//p_oRespond.m_oBody.m_strResult = "1";
			ICC_LOG_DEBUG(m_pLog, "the alarm has merged to other alarm!");
			return false;
		}
	}

	// 2.若该警单未合并到其他警单，则更新父警单的is_merge标记为1
	{
		DataBase::SQLRequest l_UpdateSQLRequest;
		l_UpdateSQLRequest.sql_id = "update_icc_t_jjdb";
		l_UpdateSQLRequest.param["id"] = p_oRequest.m_oBody.m_strMergeID;
		l_UpdateSQLRequest.set["is_merge"] = "1";
		l_UpdateSQLRequest.set["update_time"] = m_pDateTime->CurrentDateTimeStr();

		std::string strTime = m_pDateTime->GetAlarmIdTime(p_oRequest.m_oBody.m_strMergeID);
		if (strTime != "")
		{
			l_UpdateSQLRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
			l_UpdateSQLRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
		}

		DataBase::IResultSetPtr l_UpdateResult = m_pDBConn->Exec(l_UpdateSQLRequest);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_UpdateResult->GetSQL().c_str());

		if (!l_UpdateResult->IsValid())
		{
			p_oRespond.m_oHeader.m_strResult = "1";
			p_oRespond.m_oHeader.m_strMsg = "execute update_icc_t_alarm failed";
			//p_oRespond.m_oBody.m_strResult = "1";
			ICC_LOG_ERROR(m_pLog, "update_icc_t_jjdb failed,error msg:[%s]", l_UpdateResult->GetErrorMsg().c_str());
			return false;
		}
	}

	// 3.更新该警单的merge_id
	{

		PROTOCOL::CAlarmInfo alarmInfo;
		bool bQuery = _QueryAlarmInfo(p_oRequest.m_oBody.m_strMergeID, alarmInfo);

		DataBase::SQLRequest l_SQLRequest;
		l_SQLRequest.sql_id = "update_icc_t_jjdb";
		l_SQLRequest.param["id"] = p_oRequest.m_oBody.m_strAlarmID;
		l_SQLRequest.set["merge_id"] = p_oRequest.m_oBody.m_strMergeID;
		l_SQLRequest.set["merge_type"] = p_oRequest.m_oBody.m_strMergeType;
		l_SQLRequest.set["is_invalid"] = "1";
		l_SQLRequest.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
		l_SQLRequest.set["state"] = ALARM_STATUS_TERMINAL;
		//l_SQLRequest.set["handle_type"] = "05";//重复报警
		l_SQLRequest.set["handle_type"] = "08";//重复报警  2022新标准改为08

		std::string strTime = m_pDateTime->GetAlarmIdTime(p_oRequest.m_oBody.m_strAlarmID);
		if (strTime != "")
		{
			l_SQLRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
			l_SQLRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
		}

		if (bQuery)
		{
			ICC_LOG_DEBUG(m_pLog, "merge parent info:id[%s],admindeptcode[%s], admindeptname[%s], first[%s],second[%s],third[%s],fourth[%s]", 
				p_oRequest.m_oBody.m_strMergeID.c_str(), alarmInfo.m_strAdminDeptCode.c_str(), alarmInfo.m_strAdminDeptName.c_str()
				, alarmInfo.m_strFirstType.c_str(), alarmInfo.m_strSecondType.c_str(), alarmInfo.m_strThirdType.c_str()
				, alarmInfo.m_strFourthType.c_str()); 

			l_SQLRequest.set["admin_dept_org_code"] = alarmInfo.m_strAdminDeptOrgCode;
			l_SQLRequest.set["admin_dept_code"] = alarmInfo.m_strAdminDeptCode;
			l_SQLRequest.set["admin_dept_name"] = alarmInfo.m_strAdminDeptName;
			l_SQLRequest.set["admin_dept_org_code"] = alarmInfo.m_strAdminDeptOrgCode;
			l_SQLRequest.set["first_type"] = alarmInfo.m_strFirstType;
			l_SQLRequest.set["second_type"] = alarmInfo.m_strSecondType;
			l_SQLRequest.set["third_type"] = alarmInfo.m_strThirdType;
			l_SQLRequest.set["fourth_type"] = alarmInfo.m_strFourthType;
		}

		DataBase::IResultSetPtr l_Result = m_pDBConn->Exec(l_SQLRequest);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

		if (!l_Result->IsValid())
		{
			p_oRespond.m_oHeader.m_strResult = "1";
			p_oRespond.m_oHeader.m_strMsg = "execute update_icc_t_jjdb failed";
			//p_oRespond.m_oBody.m_strResult = "1";
			ICC_LOG_ERROR(m_pLog, "update_icc_t_jjdb failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
			return false;
		}
	}

	// 4.所有步骤执行成功则，返回成功
	//p_oRespond.m_oBody.m_strResult = "0";
	return true;
}

bool CBusinessImpl::BuildUnMergeRespond(PROTOCOL::CCancelMergeRespond& p_oRespond, const PROTOCOL::CCancelMergeRequest& p_oRequest)
{
	BuildRespondHeader("unmerge_alarm_respond", p_oRequest.m_oHeader, p_oRespond.m_oHeader);

	// 1.查询该警单的父警单有多少个子警单(除自己外)，也就该警单有多少个兄弟警单
	{
		DataBase::SQLRequest l_SqlRequest;
		std::string strTime = m_pDateTime->GetAlarmIdTime(p_oRequest.m_oBody.m_strMergeID);
		l_SqlRequest.sql_id = "select_icc_t_jjdb_brothers_num";
		l_SqlRequest.param["id"] = p_oRequest.m_oBody.m_strMergeID;
		if (strTime != "")
		{
			l_SqlRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
			l_SqlRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
		}
		DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

		if (!l_pResult->IsValid())
		{
			p_oRespond.m_oHeader.m_strResult = "1";
			p_oRespond.m_oHeader.m_strMsg = "execute select_icc_t_jjdb_brothers_num failed";
			//p_oRespond.m_oBody.m_strResult = "1";
			ICC_LOG_ERROR(m_pLog, "select_icc_t_alarm_brothers_num failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
			return false;
		}

		unsigned int l_uiBrothersNum = 0;
		if (l_pResult->Next())
		{
			l_uiBrothersNum = m_pString->ToInt(l_pResult->GetValue("brothers_num"));
		}

		// 如果兄弟数为0，则更新父警单的is_merge为0
		if (l_uiBrothersNum == 0)
		{
			DataBase::SQLRequest l_SqlRequest;
			l_SqlRequest.sql_id = "update_icc_t_jjdb_is_merge";
			l_SqlRequest.param["id"] = p_oRequest.m_oBody.m_strMergeID;
			l_SqlRequest.param["is_merge"] = "0";
			l_SqlRequest.param["update_time"] = m_pDateTime->CurrentDateTimeStr();;

			l_SqlRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
			l_SqlRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);

			DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
			ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
			if (!l_pResult->IsValid())
			{
				p_oRespond.m_oHeader.m_strResult = "1";
				p_oRespond.m_oHeader.m_strMsg = "execute update_icc_t_alarm_is_merge failed";
				//p_oRespond.m_oBody.m_strResult = "1";
				ICC_LOG_ERROR(m_pLog, "update_icc_t_jjdb failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
				return false;
			}

			{// 发送父警单同步
				DataBase::SQLRequest l_SqlRequest;
				l_SqlRequest.sql_id = "select_icc_t_jjdb";
				l_SqlRequest.param["id"] = p_oRequest.m_oBody.m_strAlarmID;

				std::string strTime = m_pDateTime->GetAlarmIdTime(p_oRequest.m_oBody.m_strAlarmID);
				if (strTime != "")
				{
					l_SqlRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
					l_SqlRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
				}

				DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
				ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

				if (!l_pResult->IsValid())
				{
					p_oRespond.m_oHeader.m_strResult = "1";
					p_oRespond.m_oHeader.m_strMsg = "execute select_icc_t_jjdb failed";
					//p_oRespond.m_oBody.m_strResult = "1";
					ICC_LOG_ERROR(m_pLog, "select_icc_t_jjdb failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
					return false;
				}
				if (l_pResult->Next())
				{
					SyncReceiptDate(l_pResult->GetValue("merge_id"));
				}
			}
		}
		else
		{
			SyncReceiptDate(p_oRequest.m_oBody.m_strMergeID);
			ICC_LOG_DEBUG(m_pLog, "SyncReceiptDate,MergeID:[%s]", p_oRequest.m_oBody.m_strMergeID.c_str());
		}
	}

	// 2.更新该警单的merge_id为空
	{
		DataBase::SQLRequest l_SqlRequest;
		l_SqlRequest.sql_id = "update_icc_t_jjdb";
		l_SqlRequest.param["id"] = p_oRequest.m_oBody.m_strAlarmID;
		l_SqlRequest.set["merge_id"] = "";
		l_SqlRequest.set["merge_type"] = "";
		l_SqlRequest.set["update_time"] = m_pDateTime->CurrentDateTimeStr();

		std::string strTime = m_pDateTime->GetAlarmIdTime(p_oRequest.m_oBody.m_strAlarmID);
		if (strTime != "")
		{
			l_SqlRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
			l_SqlRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
		}

		DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

		if (!l_pResult->IsValid())
		{
			p_oRespond.m_oHeader.m_strResult = "1";
			p_oRespond.m_oHeader.m_strMsg = "execute update_icc_t_alarm failed";
			//p_oRespond.m_oBody.m_strResult = "1";
			ICC_LOG_ERROR(m_pLog, "update_icc_t_alarm failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
			return false;
		}
	}

	// 3.所有步骤执行成功则，返回成功
	//p_oRespond.m_oBody.m_strResult = "0";
	return true;
}

bool CBusinessImpl::BuildGetMergeAlarmRespond(PROTOCOL::CGetMergeAlarmRespond& p_oRespond, const PROTOCOL::CGetMergeAlarmRequest& p_oRequest)
{
	BuildRespondHeader("get_merge_alarm_respond", p_oRequest.m_oHeader, p_oRespond.m_oHeader);
	if (p_oRequest.m_oBody.m_strAlarmID.empty())
	{
		p_oRespond.m_oHeader.m_strResult = "2";
		p_oRespond.m_oHeader.m_strMsg = "AlarmID is empty";
		return false;
	}

	if (!SelectMergeAlarmByID(p_oRequest.m_oBody.m_strAlarmID, p_oRespond.m_oBody.m_vecAlarmData))
	{
		p_oRespond.m_oHeader.m_strResult = "1";
		p_oRespond.m_oHeader.m_strMsg = "select MergeAlarmByID failed";
		return false;
	}

	return true;
}

bool CBusinessImpl::BuildVisitSyncBody(std::string p_strID, PROTOCOL::CAlarmVisitSync::CBody& p_oSyncBody)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_alarm_visit";
	l_SqlRequest.param["id"] = p_strID;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (l_pResult->Next())
	{
		p_oSyncBody.m_strID = l_pResult->GetValue("id");
		p_oSyncBody.m_strAlarmID = l_pResult->GetValue("alarm_id");
		p_oSyncBody.m_strCallrefID = l_pResult->GetValue("callref_id");
		p_oSyncBody.m_strDeptCode = l_pResult->GetValue("dept_code");
		p_oSyncBody.m_strDeptName = l_pResult->GetValue("dept_name");
		p_oSyncBody.m_strCallerNo = l_pResult->GetValue("caller_no");
		p_oSyncBody.m_strCallerName = l_pResult->GetValue("caller_name");
		p_oSyncBody.m_strTime = l_pResult->GetValue("time");
		p_oSyncBody.m_strVisitorCode = l_pResult->GetValue("visitor_code");
		p_oSyncBody.m_strVisitorName = l_pResult->GetValue("visitor_name");
		p_oSyncBody.m_strSatisfaction = l_pResult->GetValue("satisfaction");
		p_oSyncBody.m_strSuggest = l_pResult->GetValue("suggest");
		p_oSyncBody.m_strResultContent = l_pResult->GetValue("result_content");
		p_oSyncBody.m_strCallType = l_pResult->GetValue("call_type");
		p_oSyncBody.m_strReason = l_pResult->GetValue("reason");
	}
	ICC_LOG_DEBUG(m_pLog, "SelectDBAlarmLogByAlarmID Success");
	return true;
}

std::string CBusinessImpl::BuildAlarmLogContent(std::vector<std::string> p_vecParamList)
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

std::string CBusinessImpl::BuildAlarmLogAttach(std::string& type, std::string& id)
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

std::string CBusinessImpl::BuildAlarmState(std::string p_strID, std::string p_strCurState, std::string p_strInputState, const std::string& strTransGuid)
{
	// p_strCurState与p_strInputState比较顺序，返回顺序值较大的字符串
	std::string l_strState;
	if (p_strCurState.empty())
	{
		l_strState = p_strInputState;
	}
	else
	{
		l_strState = ((p_strCurState.compare(p_strInputState) > 0) ? p_strCurState : p_strInputState);
	}

	// 查询当前数据库中的state值
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_jjdb";
	l_SqlRequest.param["id"] = p_strID;

	std::string strTime = m_pDateTime->GetAlarmIdTime(p_strID);
	if (strTime != "")
	{
		l_SqlRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_SqlRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, false, strTransGuid);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return l_strState;
	}
	if (!l_pResult->Next())
	{
		ICC_LOG_DEBUG(m_pLog, "result is empty");
		return l_strState;
	}

	std::string l_strPreState = l_pResult->GetValue("state");

	// 数据库中的state值与p_strState比较顺序
	if (!l_strPreState.empty())
	{
		if (l_strPreState.compare(l_strState) > 0)
		{
			return l_strPreState;
		}
	}
	return l_strState;
}

std::string CBusinessImpl::_GetCurrentProcessState(const std::string& p_strID, const std::string& p_strInputState, const std::string& strTransGuid)
{
	//如果请求消息有携带，以请求为准
	if (!p_strInputState.empty())
	{
		return p_strInputState;
	}

	// 查询当前数据库中的state值
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_pjdb";
	l_SqlRequest.param["id"] = p_strID;

	std::string strTime = m_pDateTime->GetDispatchIdTime(p_strID);
	if (strTime != "")
	{
		l_SqlRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 86400 * 15);
		l_SqlRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, false, strTransGuid);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return "";
	}

	//如果没有这返回01状态
	if (!l_pResult->Next())
	{
		ICC_LOG_DEBUG(m_pLog, "result is empty, processid: %s", p_strID.c_str());
		return PROCESS_STATUS_TAKEN;
	}

	std::string tmp_strState(l_pResult->GetValue("state"));

	ICC_LOG_DEBUG(m_pLog, "processid: %s, state: %s", p_strID.c_str(), tmp_strState.c_str());
	return tmp_strState;
}

std::string CBusinessImpl::GetCurrentLinkedState(const std::string& p_strAlarmID)
{
	if (p_strAlarmID.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "p_strAlarmID is empty ");
		return "";
	}
	// 查询当前数据库中的state值
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "postgres_select_icc_t_linked_dispatch";
	l_SqlRequest.param["alarm_id"] = p_strAlarmID;
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, " GetCurrentLinkedState sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return "";
	}

	//如果没有这返回01状态
	if (!l_pResult->Next())
	{
		ICC_LOG_DEBUG(m_pLog, "result is empty, processid: %s", p_strAlarmID.c_str());
		return LINKED_STATUS_HANDLING;
	}

	std::string tmp_strState(l_pResult->GetValue("state"));
	ICC_LOG_DEBUG(m_pLog, "GetCurrentLinkedState: %s, state: %s", p_strAlarmID.c_str(), tmp_strState.c_str());
	return tmp_strState;
}

std::string CBusinessImpl::BuildProcessState(std::string p_strID, std::string p_strCurState, std::string p_strInputState, const std::string& strTransGuid)
{
	// p_strCurState与p_strInputState比较顺序，返回顺序值较大的字符串
	std::string l_strState;
	if (p_strCurState.empty())
	{
		l_strState = p_strInputState;
	}
	else
	{
		l_strState = ((p_strCurState.compare(p_strInputState) > 0) ? p_strCurState : p_strInputState);
	}

	// 查询当前数据库中的state值
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_pjdb";
	l_SqlRequest.param["id"] = p_strID;

	std::string strTime = m_pDateTime->GetDispatchIdTime(p_strID);
	if (strTime != "")
	{
		l_SqlRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 86400 * 15);
		l_SqlRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, false, strTransGuid);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return l_strState;
	}
	if (!l_pResult->Next())
	{
		ICC_LOG_DEBUG(m_pLog, "result is empty");
		return l_strState;
	}

	std::string l_strPreState = l_pResult->GetValue("state");

	// 数据库中的state值与p_strState比较顺序
	if (!l_strPreState.empty())
	{
		if (l_strPreState.compare(l_strState) > 0)
		{
			return l_strPreState;
		}
	}
	return l_strState;
}

void CBusinessImpl::SendRemark2VCS(const PROTOCOL::CAddAlarmRemarkRequestEx& p_oRequest)
{
	PROTOCOL::CAddAlarmRemarkRequest request;
	//2021/11/29 暂时注释
	request.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	request.m_oHeader.m_strRelatedID = "";
	request.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	request.m_oHeader.m_strCmd = "forward_remark_request";
	request.m_oHeader.m_strRequest = "queue_alarm_vcs";
	request.m_oHeader.m_strRequestType = "0";

	request.m_oBody.m_strGuid = m_pString->CreateGuid();
	request.m_oBody.m_strAlarmID = p_oRequest.m_oBody.m_strAlarmID;
	request.m_oBody.m_strFeedBackCode = p_oRequest.m_oBody.m_strFeedBackCode;
	request.m_oBody.m_strFeedBackName = p_oRequest.m_oBody.m_strFeedBackName;
	request.m_oBody.m_strFeedBackDeptCode = p_oRequest.m_oBody.m_strFeedBackDeptCode;
	request.m_oBody.m_strFeedBackDeptName = p_oRequest.m_oBody.m_strFeedBackDeptName;
	request.m_oBody.m_strContent = p_oRequest.m_oBody.m_strContent;
	request.m_oBody.m_strCreateUser = p_oRequest.m_oBody.m_strFeedBackName;
	request.m_oBody.m_strCreateTime = m_pDateTime->CurrentDateTimeStr();

	std::string l_strMessage = request.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));	
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::SyncSyntInfo(const PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& p_rAlarmInfo, ESyncType p_iSyncType)
{
	if (!p_rAlarmInfo.m_oBody.m_strIsVcsSyncFlag.empty())
	{
		//VCS补偿数据，不发同步
	}
	SyncAlarmInfo(p_rAlarmInfo.m_oBody.m_oAlarm, p_iSyncType, p_rAlarmInfo.m_oHeader.m_strMsgId);

	if (p_rAlarmInfo.m_oBody.m_oAlarm.m_strMsgSource == "vcs_relocated")
	{
		ICC_LOG_DEBUG(m_pLog, "vcs message, not need syn process info!!!");
		return;
	}

	auto l_itProcessData = p_rAlarmInfo.m_oBody.m_vecProcessData.begin();
	for (; l_itProcessData != p_rAlarmInfo.m_oBody.m_vecProcessData.end(); ++l_itProcessData)
	{
		// 已退单的的警单不再重复发送同步
		if (PROCESS_STATUS_RETURN == l_itProcessData->m_strState)
		{
			continue;
		}
		SyncProcessInfo(*l_itProcessData, p_iSyncType, p_rAlarmInfo.m_oHeader.m_strMsgId);
	}

	
	for (auto l_itLinkedData = p_rAlarmInfo.m_oBody.m_vecLinkedData.begin(); l_itLinkedData != p_rAlarmInfo.m_oBody.m_vecLinkedData.end(); ++l_itLinkedData)
	{
		// 已退单 和 已作废的的联动单位 要不要发送同步
		/*if (LINKED_STATUS_INVALID == l_itLinkedData->m_strState || LINKED_STATUS_CANCEL == l_itLinkedData->m_strState)
		{
			continue;
		}*/
		// 同步信息
		SyncLinkedInfo(*l_itLinkedData, "linked_dispatch_sync", "topic_linked_sync", p_iSyncType, p_rAlarmInfo.m_oHeader.m_strMsgId);
		SyncLinkedInfo(*l_itLinkedData, "sync_linked_dispatch_info", "topic_alarm_sync", p_iSyncType, p_rAlarmInfo.m_oHeader.m_strMsgId);
		if (l_itLinkedData->m_strLinkedOrgType == LINKED_DISPATCH_TYPE && l_itLinkedData->m_bIsNewProcess)
		{
			SyncLinkedInfo(*l_itLinkedData, "push_alarm_to12345", "queues_linked_info_sync", p_iSyncType, p_rAlarmInfo.m_oHeader.m_strMsgId);
		}
	}
}

void CBusinessImpl::SyncAlarmInfo(const PROTOCOL::CAlarmInfo& p_roAlarmSync, ESyncType p_iSyncType, std::string p_strReleatedID)
{
	std::string l_strGuid = m_pString->CreateGuid();
	PROTOCOL::CAlarmSync l_oAlarmSync;
	l_oAlarmSync.m_oHeader.m_strSystemID = SYSTEMID;
	l_oAlarmSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
	l_oAlarmSync.m_oHeader.m_strMsgid = l_strGuid;
	l_oAlarmSync.m_oHeader.m_strRelatedID = p_strReleatedID;
	l_oAlarmSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oAlarmSync.m_oHeader.m_strCmd = "alarm_sync";
	l_oAlarmSync.m_oHeader.m_strRequest = "topic_alarm_sync";
	l_oAlarmSync.m_oHeader.m_strRequestType = "1";
	l_oAlarmSync.m_oHeader.m_strResponse = "";
	l_oAlarmSync.m_oHeader.m_strResponseType = "";

	l_oAlarmSync.m_oBody.m_oAlarmInfo = p_roAlarmSync;

	if (!p_roAlarmSync.m_strMsgSource.compare("vcs"))
	{
		l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strMsgSource = "vcs";
	}

	if (!p_roAlarmSync.m_strMsgSource.compare("mpa"))
	{
		l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strMsgSource = "mpa";
	}

	l_oAlarmSync.m_oBody.m_strSyncType = std::to_string(p_iSyncType);
	l_oAlarmSync.m_oBody.m_oAlarmInfo.m_strLevel = p_roAlarmSync.m_strLevel.empty() ? "04" : p_roAlarmSync.m_strLevel;//默认最低级
	

	JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
	std::string l_strMessage = l_oAlarmSync.ToString(l_pIJson);
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "SyncAlarmInfo send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::SyncProcessInfo(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_rProcessToSync, ESyncType p_iSyncType, std::string p_strReleatedID)
{
	PROTOCOL::CAlarmProcessSync l_oAlarmProcessSync;
	l_oAlarmProcessSync.m_oHeader.m_strSystemID = SYSTEMID;
	l_oAlarmProcessSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
	l_oAlarmProcessSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oAlarmProcessSync.m_oHeader.m_strRelatedID = p_strReleatedID;
	l_oAlarmProcessSync.m_oHeader.m_strCmd = "alarm_process_sync";
	l_oAlarmProcessSync.m_oHeader.m_strRequest = "topic_alarm_sync";
	l_oAlarmProcessSync.m_oHeader.m_strRequestType = "1";//主题
	l_oAlarmProcessSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	if (!p_rProcessToSync.m_strMsgSource.compare("vcs"))
	{
		l_oAlarmProcessSync.m_oBody.m_strMsgSource = "vcs";
	}
	if (!p_rProcessToSync.m_strMsgSource.compare("mpa"))
	{
		l_oAlarmProcessSync.m_oBody.m_strMsgSource = "mpa";
	}
	l_oAlarmProcessSync.m_oBody.m_ProcessData = p_rProcessToSync;
	l_oAlarmProcessSync.m_oBody.m_strSyncType = std::to_string(p_iSyncType);
	//同步消息
	std::string l_strMsg(l_oAlarmProcessSync.ToString(m_pJsonFty->CreateJson()));
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
}

void CBusinessImpl::SyncRemarkInfo(const PROTOCOL::CAddAlarmRemarkRequestEx& p_oRemarkRequest)
{
	PROTOCOL::CAddAlarmRemarkRequest request;
	request.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	request.m_oHeader.m_strRelatedID = "";
	request.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	request.m_oHeader.m_strCmd = "alarm_remark_sync";
	request.m_oHeader.m_strRequest = "topic_alarm_sync";
	request.m_oHeader.m_strRequestType = "1";

	request.m_oBody.m_strGuid = m_pString->CreateGuid();
	request.m_oBody.m_strAlarmID = p_oRemarkRequest.m_oBody.m_strAlarmID;
	request.m_oBody.m_strFeedBackCode = p_oRemarkRequest.m_oBody.m_strFeedBackCode;
	request.m_oBody.m_strFeedBackName = p_oRemarkRequest.m_oBody.m_strFeedBackName;
	request.m_oBody.m_strFeedBackDeptCode = p_oRemarkRequest.m_oBody.m_strFeedBackDeptCode;
	request.m_oBody.m_strFeedBackDeptName = p_oRemarkRequest.m_oBody.m_strFeedBackDeptName;
	request.m_oBody.m_strContent = p_oRemarkRequest.m_oBody.m_strContent;
	request.m_oBody.m_strCreateUser = p_oRemarkRequest.m_oBody.m_strFeedBackName;
	request.m_oBody.m_strCreateTime = m_pDateTime->CurrentDateTimeStr();

	std::string l_strMessage = request.ToString(m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::SyncAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_rAlarmLogToSync)
{
	std::string l_strGuid = m_pString->CreateGuid();
	PROTOCOL::CAlarmLogSync l_oAlarmLogSync;
	l_oAlarmLogSync.m_oHeader.m_strSystemID = SYSTEMID;
	l_oAlarmLogSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
	l_oAlarmLogSync.m_oHeader.m_strMsgid = l_strGuid;
	l_oAlarmLogSync.m_oHeader.m_strRelatedID = "";
	l_oAlarmLogSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oAlarmLogSync.m_oHeader.m_strCmd = "alarm_log_sync";
	l_oAlarmLogSync.m_oHeader.m_strRequest = "topic_alarm";//?topic_alarm_sync
	l_oAlarmLogSync.m_oHeader.m_strRequestType = "1";
	l_oAlarmLogSync.m_oHeader.m_strResponse = "";
	l_oAlarmLogSync.m_oHeader.m_strResponseType = "";

	l_oAlarmLogSync.m_oBody.m_strID = p_rAlarmLogToSync.m_strID;
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
	std::string l_strMessage = l_oAlarmLogSync.ToString(l_pIJson, m_pJsonFty->CreateJson());
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMessage.c_str());
}

void CBusinessImpl::SyncReceiptDate(std::string p_strID)
{
	PROTOCOL::CAddOrUpdateAlarmWithProcessRequest l_oAlarm;
	GetAlarm(p_strID, l_oAlarm);
	//同步消息
	SyncAlarmInfo(l_oAlarm.m_oBody.m_oAlarm, EDIT);
}

void CBusinessImpl::SyncProcessDate(std::string p_strID, bool bFromVcs)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_pjdb";
	l_SqlRequest.param["id"] = p_strID;

	std::string strTime = m_pDateTime->GetDispatchIdTime(p_strID);
	if (strTime != "")
	{
		l_SqlRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 86400 * 15);
		l_SqlRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{ // 执行SQL失败
		ICC_LOG_ERROR(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return;
	}
	if (!l_pResult->Next())
	{
		ICC_LOG_DEBUG(m_pLog, "result is null");
		return;
	}

	PROTOCOL::CAddOrUpdateProcessRequest::CProcessData l_oProcess;

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
	if (bFromVcs)
	{
		l_oProcess.m_strMsgSource = "vcs";
	}	

	//同步消息
	SyncProcessInfo(l_oProcess, SUBMIT);
}

bool CBusinessImpl::IsUpdateTimeNotEmpty(std::string p_strAlarmID, const std::string& strTransGuid)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_jjdb";
	l_SqlRequest.param["id"] = p_strAlarmID;

	std::string strTime = m_pDateTime->GetAlarmIdTime(p_strAlarmID);
	if (strTime != "")
	{
		l_SqlRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_SqlRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, false, strTransGuid);
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (l_pResult->Next())
	{
		std::string l_strUpdateTime = l_pResult->GetValue("update_time");
		ICC_LOG_DEBUG(m_pLog, "[%s] update_time is [%s]", p_strAlarmID.c_str(), l_strUpdateTime.c_str());
		if (l_strUpdateTime.empty())
		{
			return false;
		}
	}	
	return true;
}

bool CBusinessImpl::SelectAlarmLogByAlarmID(const PROTOCOL::CGetAlarmLogRequest& p_oRequest, PROTOCOL::CGetAlarmLogRespond::CBody& p_mapAlarmLogInfo)
{
	DataBase::SQLRequest l_SqlRequest;
	if (p_oRequest.m_oBody.m_strVcsSyncFlag == "1")
	{
		if (p_oRequest.m_oBody.m_strBeginTime.empty())
		{
			return false;
		}
		
		DataBase::SQLRequest l_tSQLReq;
		std::string l_strEndTime = p_oRequest.m_oBody.m_strEndTime;
		if (l_strEndTime.empty())
		{
			l_strEndTime = m_pDateTime->CurrentDateTimeStr();
		}
		//先查条目
		l_tSQLReq.sql_id = "select_icc_t_alarm_log_count"; //select_icc_t_violation_count

		l_tSQLReq.param["begin_time"] = p_oRequest.m_oBody.m_strBeginTime;
		l_tSQLReq.param["end_time"] = l_strEndTime;

		l_tSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(p_oRequest.m_oBody.m_strBeginTime, 86400 * 15);
		l_tSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(p_oRequest.m_oBody.m_strBeginTime, 1);

		DataBase::IResultSetPtr l_resultCount = m_pDBConn->Exec(l_tSQLReq);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_resultCount->GetSQL().c_str());
		if (!l_resultCount->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_resultCount->GetErrorMsg().c_str());
			return false;
		}

		std::string tmp_strAllCount;
		if (l_resultCount->Next())
		{
			tmp_strAllCount = l_resultCount->GetValue("num");
			ICC_LOG_DEBUG(m_pLog, "Get all monitorapply size[%s]", tmp_strAllCount.c_str());
		}

		//表示没有记录
		if (tmp_strAllCount.empty() || "0" == tmp_strAllCount)
		{
			ICC_LOG_DEBUG(m_pLog, "no record");
			return true;
		}

		int l_nPageSize = m_pString->ToInt(p_oRequest.m_oBody.m_strPageSize.c_str());
		int l_nPageIndex = m_pString->ToInt(p_oRequest.m_oBody.m_strPageIndex.c_str());

		if (l_nPageSize < 0 || l_nPageSize > MAX_COUNT_EX) l_nPageSize = MAX_COUNT_EX;
		if (l_nPageIndex < 1) l_nPageIndex = 1;
		l_nPageIndex = (l_nPageIndex - 1) * l_nPageSize;
		l_tSQLReq.sql_id = "select_icc_t_alarm_log";
		l_tSQLReq.param["limit"] = std::to_string(l_nPageSize);
		l_tSQLReq.param["offset"] = std::to_string(l_nPageIndex);
		l_tSQLReq.param["orderby"] = "operatetime";

		DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_tSQLReq);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
		if (!l_pResult->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
			return false;
		}

		p_mapAlarmLogInfo.m_strAllCount = tmp_strAllCount;
		while (l_pResult->Next())
		{
			PROTOCOL::CGetAlarmLogRespond::CData l_oData;
			l_oData.m_strID = l_pResult->GetValue("id");
			l_oData.m_strAlarmID = l_pResult->GetValue("jjdbh");
			l_oData.m_strProcessID = l_pResult->GetValue("pjdbh");
			l_oData.m_strFeedbackID = l_pResult->GetValue("fkdbh");
			l_oData.m_strSeatNo = l_pResult->GetValue("seat_no");
			l_oData.m_strOperate = l_pResult->GetValue("operate");
			l_oData.m_strOperateContent = l_pResult->GetValue("operate_content");
			std::string  l_strOperateContent = l_oData.m_strOperateContent;
			l_strOperateContent = m_pString->Trim(l_strOperateContent);
			l_strOperateContent = m_pString->ReplaceFirst(l_strOperateContent, "{", "");
			l_strOperateContent = m_pString->ReplaceFirst(l_strOperateContent,"\"param\":","");
			l_strOperateContent = m_pString->ReplaceLast(l_strOperateContent, "}", "");
			l_strOperateContent = m_pString->ReplaceAll(l_strOperateContent, "\n", "");
			l_oData.m_strOperateContent = l_strOperateContent;

			l_oData.m_strFromType = l_pResult->GetValue("from_type");
			l_oData.m_strFromObject = l_pResult->GetValue("from_object");
			l_oData.m_strFromObjectName = l_pResult->GetValue("from_object_name");
			l_oData.m_strFromObjectOrgName = l_pResult->GetValue("from_object_org_name");
			l_oData.m_strFromObjectOrgCode = l_pResult->GetValue("from_object_org_code");
			l_oData.m_strFromOrgIdentifier = l_pResult->GetValue("from_object_org_identifier");
			l_oData.m_strToType = l_pResult->GetValue("to_type");
			l_oData.m_strToObject = l_pResult->GetValue("to_object");
			l_oData.m_strToObjectName = l_pResult->GetValue("to_object_name");
			l_oData.m_strToObjectOrgName = l_pResult->GetValue("to_object_org_name");
			l_oData.m_strToObjectOrgCode = l_pResult->GetValue("to_object_org_code");
			l_oData.m_strToObjectOrgIdentifier = l_pResult->GetValue("to_object_org_identifier");
			l_oData.m_strCreateUser = l_pResult->GetValue("create_user");
			l_oData.m_strCreateTime = l_pResult->GetValue("create_time");
			l_oData.m_strDeptOrgCode = l_pResult->GetValue("dept_org_code");
			l_oData.m_strSourceName = l_pResult->GetValue("source_name");
			l_oData.m_strOperateAttachDesc = l_pResult->GetValue("operate_attach_desc");
			l_oData.m_strReceivedTime = l_pResult->GetValue("jjsj");
			l_oData.m_strDescription = l_pResult->GetValue("description");
			l_oData.m_strCreateOrg = l_pResult->GetValue("create_org");

			p_mapAlarmLogInfo.m_vecData.push_back(l_oData);
		}

		p_mapAlarmLogInfo.m_strCount = std::to_string(p_mapAlarmLogInfo.m_vecData.size());

		ICC_LOG_DEBUG(m_pLog, "SelectDBAlarmLogByAlarmID Get success, size[%s]", p_mapAlarmLogInfo.m_strCount.c_str());

	}
	else
	{
		l_SqlRequest.sql_id = "select_icc_t_alarm_log";
		std::string p_strAlarmID = p_oRequest.m_oBody.m_strAlarmID;
		l_SqlRequest.param["jjdbh"] = p_strAlarmID;
		l_SqlRequest.param["orderby"] = "operatetime";

		std::string strTime = m_pDateTime->GetAlarmIdTime(p_oRequest.m_oBody.m_strAlarmID);
		if (strTime != "")
		{
			l_SqlRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
			l_SqlRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
		}

		ICC_LOG_DEBUG(m_pLog, "SelectDBAlarmLogByAlarmID Begin");

		DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
		if (!l_pResult->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
			return false;
		}
		while (l_pResult->Next())
		{
			PROTOCOL::CGetAlarmLogRespond::CData l_oData;
			l_oData.m_strID = l_pResult->GetValue("id");
			l_oData.m_strAlarmID = l_pResult->GetValue("jjdbh");
			l_oData.m_strProcessID = l_pResult->GetValue("pjdbh");
			l_oData.m_strFeedbackID = l_pResult->GetValue("fkdbh");
			l_oData.m_strSeatNo = l_pResult->GetValue("seat_no");
			l_oData.m_strOperate = l_pResult->GetValue("operate");
			l_oData.m_strOperateContent = l_pResult->GetValue("operate_content");
			l_oData.m_strFromType = l_pResult->GetValue("from_type");
			l_oData.m_strFromObject = l_pResult->GetValue("from_object");
			l_oData.m_strFromObjectName = l_pResult->GetValue("from_object_name");
			l_oData.m_strFromObjectOrgName = l_pResult->GetValue("from_object_org_name");
			l_oData.m_strFromObjectOrgCode = l_pResult->GetValue("from_object_org_code");
			l_oData.m_strFromOrgIdentifier = l_pResult->GetValue("from_object_org_identifier");
			l_oData.m_strToType = l_pResult->GetValue("to_type");
			l_oData.m_strToObject = l_pResult->GetValue("to_object");
			l_oData.m_strToObjectName = l_pResult->GetValue("to_object_name");
			l_oData.m_strToObjectOrgName = l_pResult->GetValue("to_object_org_name");
			l_oData.m_strToObjectOrgCode = l_pResult->GetValue("to_object_org_code");
			l_oData.m_strToObjectOrgIdentifier = l_pResult->GetValue("to_object_org_identifier");
			l_oData.m_strCreateUser = l_pResult->GetValue("create_user");
			l_oData.m_strCreateTime = l_pResult->GetValue("create_time");
			l_oData.m_strDeptOrgCode = l_pResult->GetValue("dept_org_code");
			l_oData.m_strSourceName = l_pResult->GetValue("source_name");
			l_oData.m_strOperateAttachDesc = l_pResult->GetValue("operate_attach_desc");

			l_oData.m_strDescription = l_pResult->GetValue("description");
			l_oData.m_strCreateOrg = l_pResult->GetValue("create_org");
			l_oData.m_strReceivedTime = l_pResult->GetValue("jjsj");

			std::string strType;
			std::string strSourceID;
			GetCallMessage(l_oData, strType, strSourceID);
			if (strType == "jjlyh" || strType == "call")
			{
				if (!GetRecordID(strSourceID, l_oData))
				{
					ICC_LOG_DEBUG(m_pLog, "Get Record ID failed,SourceID:[%s]", strSourceID.c_str());
				}
			}
			p_mapAlarmLogInfo.m_vecData.push_back(l_oData);
		}
		ICC_LOG_DEBUG(m_pLog, "SelectDBAlarmLogByAlarmID Success");
	}
	
	return true;
}

bool CBusinessImpl::SelectAlarmBookByResID(std::string p_strResource, std::vector<PROTOCOL::CGetAlarmLogAlarmResRespond::CData> &p_vecData)
{
	return true;
}

bool CBusinessImpl::SelectProcessBookByResID(std::string p_strResource, std::vector<PROTOCOL::CGetAlarmProcLogResRespond::CData> &p_vecData)
{
	return true;
}

bool CBusinessImpl::SelectMergeAlarmByID(std::string p_strID, std::vector<PROTOCOL::CAlarmInfo>& p_vecAlarm)
{
	DataBase::SQLRequest l_oSeleteAlarmSQLReq;
	l_oSeleteAlarmSQLReq.sql_id = "select_icc_t_jjdb";
	l_oSeleteAlarmSQLReq.param["id"] = p_strID;

	std::string strTime = m_pDateTime->GetAlarmIdTime(p_strID);
	if (strTime != "")
	{
		l_oSeleteAlarmSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_oSeleteAlarmSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}

	if (!m_pDBConn)
	{
		ICC_LOG_ERROR(m_pLog, "DB Connect Is Null!!!");
		return false;
	}
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oSeleteAlarmSQLReq);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	std::string l_strAlarmID;
	PROTOCOL::CAlarmInfo l_oAlarm;
	while (l_pResult->Next())
	{
		l_strAlarmID = l_pResult->GetValue("merge_id");

		if (!l_oAlarm.ParseAlarmRecord(l_pResult))
		{
			ICC_LOG_ERROR(m_pLog, "Parse record failed.");
		}

		p_vecAlarm.push_back(l_oAlarm);
	}

	//无父节点，即自身为父节点
	if (l_strAlarmID == "")
	{
		DataBase::SQLRequest l_oSeleteAlarmByIDSQLReq;
		l_oSeleteAlarmByIDSQLReq.sql_id = "select_icc_t_jjdb";
		l_oSeleteAlarmByIDSQLReq.param["merge_id"] = p_strID;

		std::string strTime = m_pDateTime->GetAlarmIdTime(p_strID);
		if (strTime != "")
		{
			l_oSeleteAlarmByIDSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
			l_oSeleteAlarmByIDSQLReq.param["jjsj_end"] = m_pDateTime->CurrentDateTimeStr();
		}

		DataBase::IResultSetPtr l_pResultAlarmById = m_pDBConn->Exec(l_oSeleteAlarmByIDSQLReq);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResultAlarmById->GetSQL().c_str());


		if (!l_pResultAlarmById->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResultAlarmById->GetErrorMsg().c_str());
			return false;
		}

		while (l_pResultAlarmById->Next())
		{
			PROTOCOL::CAlarmInfo l_oAlarmRelate;

			if (!l_oAlarmRelate.ParseAlarmRecord(l_pResultAlarmById))
			{
				ICC_LOG_ERROR(m_pLog, "Parse record failed.");
			}
			
			p_vecAlarm.push_back(l_oAlarmRelate);
		}
	}
	else
	{
		{// 查出所有字节点 [6/1/2018 x26464]
			DataBase::SQLRequest l_oSeleteAlarmByIDSQLReq;
			l_oSeleteAlarmByIDSQLReq.sql_id = "select_icc_t_jjdb";
			l_oSeleteAlarmByIDSQLReq.param["merge_id"] = l_strAlarmID;

			std::string strTime = m_pDateTime->GetAlarmIdTime(l_strAlarmID);
			if (strTime != "")
			{
				l_oSeleteAlarmByIDSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
				l_oSeleteAlarmByIDSQLReq.param["jjsj_end"] = m_pDateTime->CurrentDateTimeStr();
			}

			p_vecAlarm.clear();
			DataBase::IResultSetPtr l_pResultAlarmById = m_pDBConn->Exec(l_oSeleteAlarmByIDSQLReq);
			ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResultAlarmById->GetSQL().c_str());

			if (!l_pResultAlarmById->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResultAlarmById->GetErrorMsg().c_str());
				return false;
			}

			while (l_pResultAlarmById->Next())
			{
				PROTOCOL::CAlarmInfo l_oAlarmRelate;

				if (!l_oAlarmRelate.ParseAlarmRecord(l_pResultAlarmById))
				{
					ICC_LOG_ERROR(m_pLog, "Parse record failed.");
				}

			
				p_vecAlarm.push_back(l_oAlarmRelate);
			}
		}
		{// 查出父节点 [6/1/2018 x26464]
			DataBase::SQLRequest l_oSeleteAlarmByIDSQLReq;
			l_oSeleteAlarmByIDSQLReq.sql_id = "select_icc_t_jjdb";
			l_oSeleteAlarmByIDSQLReq.param["id"] = l_strAlarmID;

			std::string strTime = m_pDateTime->GetAlarmIdTime(l_strAlarmID);
			if (strTime != "")
			{
				l_oSeleteAlarmByIDSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
				l_oSeleteAlarmByIDSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
			}

			DataBase::IResultSetPtr l_pResultAlarmById = m_pDBConn->Exec(l_oSeleteAlarmByIDSQLReq);
			ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResultAlarmById->GetSQL().c_str());

			if (!l_pResultAlarmById->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
				return false;
			}

			while (l_pResultAlarmById->Next())
			{
				PROTOCOL::CAlarmInfo l_oAlarmRelate;

				if (!l_oAlarmRelate.ParseAlarmRecord(l_pResultAlarmById))
				{
					ICC_LOG_ERROR(m_pLog, "Parse record failed.");
				}

				p_vecAlarm.push_back(l_oAlarmRelate);
			}
		}
	}
	return true;
}

bool CBusinessImpl::SelectVisitInfo(const PROTOCOL::CSearchAlarmVisitRequest::CBody& p_oReqBody, PROTOCOL::CSearchAlarmVisitRespond::CBody& p_oRespBody)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_alarm_visit";
	if (ESearchType::BY_DEPTCODE == p_oReqBody.m_eSearchType)
	{
		if (!p_oReqBody.m_strPageSize.empty() && !p_oReqBody.m_strPageIndex.empty())
		{
			l_SqlRequest.param["page_size"] = p_oReqBody.m_strPageSize;
			l_SqlRequest.param["page_index"] = p_oReqBody.m_strPageIndex;
		}
		
		if (!p_oReqBody.m_strBeginTime.empty() && !p_oReqBody.m_strEndTime.empty())
		{
			l_SqlRequest.param["begin_time"] = p_oReqBody.m_strBeginTime;
			l_SqlRequest.param["end_time"] = p_oReqBody.m_strEndTime;
		}		

		l_SqlRequest.param["dept_code"] = p_oReqBody.m_strDeptCode;
	}
	else if(ESearchType::BY_ALARMID == p_oReqBody.m_eSearchType)
	{
		l_SqlRequest.param["alarm_id"] = p_oReqBody.m_strAlarmID;
	}
	else if (ESearchType::BY_ID == p_oReqBody.m_eSearchType)
	{
		l_SqlRequest.param["id"] = p_oReqBody.m_strID;
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "input param error!");
		return false;
	}
	
	{//获取结果数量
		DataBase::SQLRequest l_SqlReqCount = l_SqlRequest;
		l_SqlReqCount.sql_id = "select_icc_t_alarm_visit_count";
		DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlReqCount);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

		if (!l_pResult->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
			return false;
		}
		if (l_pResult->Next())
		{
			p_oRespBody.m_strCount = l_pResult->GetValue("num");
		}
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	while (l_pResult->Next())
	{
		PROTOCOL::CSearchAlarmVisitRespond::CData l_oData;
		l_oData.m_strID = l_pResult->GetValue("id");
		l_oData.m_strAlarmID = l_pResult->GetValue("alarm_id");
		l_oData.m_strCallrefID = l_pResult->GetValue("callref_id");
		l_oData.m_strDeptCode = l_pResult->GetValue("dept_code");
		l_oData.m_strDeptName = l_pResult->GetValue("dept_name");
		l_oData.m_strCallerNo = l_pResult->GetValue("caller_no");
		l_oData.m_strCallerName = l_pResult->GetValue("caller_name");
		l_oData.m_strTime = l_pResult->GetValue("time");
		l_oData.m_strVisitorCode = l_pResult->GetValue("visitor_code");
		l_oData.m_strVisitorName = l_pResult->GetValue("visitor_name");
		l_oData.m_strSatisfaction = l_pResult->GetValue("satisfaction");
		l_oData.m_strSuggest = l_pResult->GetValue("suggest");
		l_oData.m_strResultContent = l_pResult->GetValue("result_content");
		l_oData.m_strCallType = l_pResult->GetValue("call_type");
		l_oData.m_strReason = l_pResult->GetValue("reason");

		p_oRespBody.m_vecData.push_back(l_oData);
	}
	ICC_LOG_DEBUG(m_pLog, "SelectDBAlarmLogByAlarmID Success");
	return true;
}

bool CBusinessImpl::InsertAlarmInfo(const PROTOCOL::CAddOrUpdateAlarmWithProcessRequest::CBody& p_oAlarmSyntInfo)
{
	DataBase::SQLRequest l_tSQLReqInsertAlarm;
	l_tSQLReqInsertAlarm.sql_id = "insert_icc_t_jjdb";

	//l_tSQLReqInsertAlarm.param["city_code"] = p_oAlarmSyntInfo.m_oAlarm.m_strCityCode;
	l_tSQLReqInsertAlarm.param["id"] = p_oAlarmSyntInfo.m_oAlarm.m_strID;
	l_tSQLReqInsertAlarm.param["merge_id"] = p_oAlarmSyntInfo.m_oAlarm.m_strMergeID;
	l_tSQLReqInsertAlarm.param["merge_type"] = p_oAlarmSyntInfo.m_oAlarm.m_strMergeType;
	l_tSQLReqInsertAlarm.param["receipt_seatno"] = p_oAlarmSyntInfo.m_oAlarm.m_strSeatNo;
	l_tSQLReqInsertAlarm.param["label"] = p_oAlarmSyntInfo.m_oAlarm.m_strTitle;
	l_tSQLReqInsertAlarm.param["content"] = p_oAlarmSyntInfo.m_oAlarm.m_strContent;
	l_tSQLReqInsertAlarm.param["receiving_time"] = p_oAlarmSyntInfo.m_oAlarm.m_strTime;
	l_tSQLReqInsertAlarm.param["received_time"] = p_oAlarmSyntInfo.m_oAlarm.m_strReceivedTime;
	l_tSQLReqInsertAlarm.param["addr"] = p_oAlarmSyntInfo.m_oAlarm.m_strAddr;
	if (!p_oAlarmSyntInfo.m_oAlarm.m_strLongitude.empty())
	{
		l_tSQLReqInsertAlarm.param["longitude"] = p_oAlarmSyntInfo.m_oAlarm.m_strLongitude;
	}
	else if (p_oAlarmSyntInfo.m_oAlarm.m_strLongitude.empty() && !p_oAlarmSyntInfo.m_oAlarm.m_strManualLongitude.empty())
	{
		l_tSQLReqInsertAlarm.param["longitude"] = p_oAlarmSyntInfo.m_oAlarm.m_strManualLongitude;
	}
	if (!p_oAlarmSyntInfo.m_oAlarm.m_strLatitude.empty())
	{
		l_tSQLReqInsertAlarm.param["latitude"] = p_oAlarmSyntInfo.m_oAlarm.m_strLatitude;
	}
	else if (p_oAlarmSyntInfo.m_oAlarm.m_strLatitude.empty() && !p_oAlarmSyntInfo.m_oAlarm.m_strManualLatitude.empty())
	{
		l_tSQLReqInsertAlarm.param["latitude"] = p_oAlarmSyntInfo.m_oAlarm.m_strManualLatitude;
	}
	l_tSQLReqInsertAlarm.param["state"] = p_oAlarmSyntInfo.m_oAlarm.m_strState;
	l_tSQLReqInsertAlarm.param["level"] = p_oAlarmSyntInfo.m_oAlarm.m_strLevel;
	l_tSQLReqInsertAlarm.param["source_type"] = p_oAlarmSyntInfo.m_oAlarm.m_strSourceType;
	l_tSQLReqInsertAlarm.param["source_id"] = p_oAlarmSyntInfo.m_oAlarm.m_strSourceID;
	l_tSQLReqInsertAlarm.param["handle_type"] = p_oAlarmSyntInfo.m_oAlarm.m_strHandleType;
	l_tSQLReqInsertAlarm.param["first_type"] = p_oAlarmSyntInfo.m_oAlarm.m_strFirstType;
	l_tSQLReqInsertAlarm.param["second_type"] = p_oAlarmSyntInfo.m_oAlarm.m_strSecondType;
	l_tSQLReqInsertAlarm.param["third_type"] = p_oAlarmSyntInfo.m_oAlarm.m_strThirdType;
	l_tSQLReqInsertAlarm.param["fourth_type"] = p_oAlarmSyntInfo.m_oAlarm.m_strFourthType;
	l_tSQLReqInsertAlarm.param["vehicle_no"] = p_oAlarmSyntInfo.m_oAlarm.m_strVehicleNo;
	l_tSQLReqInsertAlarm.param["vehicle_type"] = p_oAlarmSyntInfo.m_oAlarm.m_strVehicleType;
	l_tSQLReqInsertAlarm.param["symbol_code"] = p_oAlarmSyntInfo.m_oAlarm.m_strSymbolCode;

	l_tSQLReqInsertAlarm.param["called_no_type"] = p_oAlarmSyntInfo.m_oAlarm.m_strCalledNoType;

	l_tSQLReqInsertAlarm.param["caller_no"] = p_oAlarmSyntInfo.m_oAlarm.m_strCallerNo;
	l_tSQLReqInsertAlarm.param["caller_name"] = p_oAlarmSyntInfo.m_oAlarm.m_strCallerName;
	l_tSQLReqInsertAlarm.param["caller_addr"] = p_oAlarmSyntInfo.m_oAlarm.m_strCallerAddr;
	l_tSQLReqInsertAlarm.param["caller_id"] = p_oAlarmSyntInfo.m_oAlarm.m_strCallerID;
	l_tSQLReqInsertAlarm.param["caller_id_type"] = p_oAlarmSyntInfo.m_oAlarm.m_strCallerIDType;
	l_tSQLReqInsertAlarm.param["caller_gender"] = p_oAlarmSyntInfo.m_oAlarm.m_strCallerGender;


	l_tSQLReqInsertAlarm.param["contact_no"] = p_oAlarmSyntInfo.m_oAlarm.m_strContactNo;

	l_tSQLReqInsertAlarm.param["admin_dept_code"] = p_oAlarmSyntInfo.m_oAlarm.m_strAdminDeptCode;
	l_tSQLReqInsertAlarm.param["admin_dept_name"] = p_oAlarmSyntInfo.m_oAlarm.m_strAdminDeptName;
	l_tSQLReqInsertAlarm.param["receipt_dept_district_code"] = p_oAlarmSyntInfo.m_oAlarm.m_strReceiptDeptDistrictCode;
	l_tSQLReqInsertAlarm.param["receipt_dept_code"] = p_oAlarmSyntInfo.m_oAlarm.m_strReceiptDeptCode;
	l_tSQLReqInsertAlarm.param["receipt_dept_name"] = p_oAlarmSyntInfo.m_oAlarm.m_strReceiptDeptName;
	l_tSQLReqInsertAlarm.param["receipt_code"] = p_oAlarmSyntInfo.m_oAlarm.m_strReceiptCode;
	l_tSQLReqInsertAlarm.param["receipt_name"] = p_oAlarmSyntInfo.m_oAlarm.m_strReceiptName;

	l_tSQLReqInsertAlarm.param["create_time"] = p_oAlarmSyntInfo.m_oAlarm.m_strCreateTime.empty() ? m_pDateTime->CurrentDateTimeStr() : p_oAlarmSyntInfo.m_oAlarm.m_strCreateTime;
	l_tSQLReqInsertAlarm.param["create_user"] = p_oAlarmSyntInfo.m_oAlarm.m_strCreateUser.empty() ? p_oAlarmSyntInfo.m_oAlarm.m_strReceiptCode : p_oAlarmSyntInfo.m_oAlarm.m_strCreateUser;
	//l_tSQLReqInsertAlarm.param["update_time"] = p_oAlarmSyntInfo.m_oAlarm.m_strUpdateTime;
	//l_tSQLReqInsertAlarm.param["update_user"] = p_oAlarmSyntInfo.m_oAlarm.m_strUpdateUser;
	l_tSQLReqInsertAlarm.param["is_privacy"] = p_oAlarmSyntInfo.m_oAlarm.m_strPrivacy;
	l_tSQLReqInsertAlarm.param["remark"] = p_oAlarmSyntInfo.m_oAlarm.m_strRemark;

	//新增
	l_tSQLReqInsertAlarm.param["alarm_addr"] = p_oAlarmSyntInfo.m_oAlarm.m_strAlarmAddr;
	l_tSQLReqInsertAlarm.param["caller_user_name"] = p_oAlarmSyntInfo.m_oAlarm.m_strCallerUserName;
	l_tSQLReqInsertAlarm.param["erpetrators_number"] = p_oAlarmSyntInfo.m_oAlarm.m_strErpetratorsNumber;

	l_tSQLReqInsertAlarm.param["is_armed"] = p_oAlarmSyntInfo.m_oAlarm.m_strIsArmed;

	l_tSQLReqInsertAlarm.param["is_hazardous_substances"] = p_oAlarmSyntInfo.m_oAlarm.m_strIsHazardousSubstances;

	l_tSQLReqInsertAlarm.param["is_explosion_or_leakage"] = p_oAlarmSyntInfo.m_oAlarm.m_strIsExplosionOrLeakage;

	l_tSQLReqInsertAlarm.param["desc_of_trapped"] = p_oAlarmSyntInfo.m_oAlarm.m_strDescOfTrapped;
	l_tSQLReqInsertAlarm.param["desc_of_injured"] = p_oAlarmSyntInfo.m_oAlarm.m_strDescOfInjured;
	l_tSQLReqInsertAlarm.param["desc_of_dead"] = p_oAlarmSyntInfo.m_oAlarm.m_strDescOfDead;
	l_tSQLReqInsertAlarm.param["is_foreign_language"] = p_oAlarmSyntInfo.m_oAlarm.m_strIsForeignLanguage;
	l_tSQLReqInsertAlarm.param["manual_longitude"] = p_oAlarmSyntInfo.m_oAlarm.m_strManualLongitude;
	l_tSQLReqInsertAlarm.param["manual_latitude"] = p_oAlarmSyntInfo.m_oAlarm.m_strManualLatitude;
	l_tSQLReqInsertAlarm.param["emergency_rescue_level"] = p_oAlarmSyntInfo.m_oAlarm.m_strEmergencyRescueLevel;
	l_tSQLReqInsertAlarm.param["is_hazardous_vehicle"] = p_oAlarmSyntInfo.m_oAlarm.m_strIsHazardousVehicle;
	l_tSQLReqInsertAlarm.param["receipt_srv_name"] = p_oAlarmSyntInfo.m_oAlarm.m_strReceiptSrvName;
	l_tSQLReqInsertAlarm.param["admin_dept_org_code"] = p_oAlarmSyntInfo.m_oAlarm.m_strAdminDeptOrgCode;
	l_tSQLReqInsertAlarm.param["receipt_dept_org_code"] = p_oAlarmSyntInfo.m_oAlarm.m_strReceiptDeptOrgCode;
	l_tSQLReqInsertAlarm.param["is_invalid"] = p_oAlarmSyntInfo.m_oAlarm.m_strIsInvalid;
	if (p_oAlarmSyntInfo.m_oAlarm.m_strIsInvalid.empty())
	{
		l_tSQLReqInsertAlarm.param["is_invalid"] = "0";
	}
	l_tSQLReqInsertAlarm.param["business_status"] = p_oAlarmSyntInfo.m_oAlarm.m_strBusinessState;
	l_tSQLReqInsertAlarm.param["create_time"] = m_pDateTime->CurrentDateTimeStr();
	l_tSQLReqInsertAlarm.param["is_delete"] = "0"; //先写死 2022/3/17
	l_tSQLReqInsertAlarm.param["is_feedback"] = "0";
	l_tSQLReqInsertAlarm.param["is_visitor"] = "0";
	
	l_tSQLReqInsertAlarm.param["jurisdictional_orgcode"] = p_oAlarmSyntInfo.m_oAlarm.m_strJurisdictionalOrgcode;
	l_tSQLReqInsertAlarm.param["jurisdictional_orgname"] = p_oAlarmSyntInfo.m_oAlarm.m_strJurisdictionalOrgname;
	l_tSQLReqInsertAlarm.param["jurisdictional_orgidentifier"] = p_oAlarmSyntInfo.m_oAlarm.m_strJurisdictionalOrgidentifier;
	l_tSQLReqInsertAlarm.param["had_push"] = p_oAlarmSyntInfo.m_oAlarm.m_strHadPush;
	if (p_oAlarmSyntInfo.m_oAlarm.m_strHadPush.empty())
	{
		l_tSQLReqInsertAlarm.param["had_push"] = "0";
	}

	l_tSQLReqInsertAlarm.param["is_merge"] = p_oAlarmSyntInfo.m_oAlarm.m_strIsMerge;
	if (p_oAlarmSyntInfo.m_oAlarm.m_strIsMerge.empty())
	{
		l_tSQLReqInsertAlarm.param["is_merge"] = "0";
	}
	l_tSQLReqInsertAlarm.param["is_over"] = p_oAlarmSyntInfo.m_oAlarm.m_strIsOver;
	if (p_oAlarmSyntInfo.m_oAlarm.m_strIsOver.empty())
	{
		l_tSQLReqInsertAlarm.param["is_over"] = "0";
	}

	l_tSQLReqInsertAlarm.param["createTeminal"] = p_oAlarmSyntInfo.m_oAlarm.m_strCreateTeminal;
	l_tSQLReqInsertAlarm.param["updateTeminal"] = p_oAlarmSyntInfo.m_oAlarm.m_strUpdateTeminal;
	l_tSQLReqInsertAlarm.param["alarm_source_type"] = p_oAlarmSyntInfo.m_oAlarm.m_strAlarmSourceType;
	l_tSQLReqInsertAlarm.param["alarm_source_id"] = p_oAlarmSyntInfo.m_oAlarm.m_strAlarmSourceId;
	l_tSQLReqInsertAlarm.param["centre_dept_code"] = p_oAlarmSyntInfo.m_oAlarm.m_strCentreDeptCode;
	l_tSQLReqInsertAlarm.param["centre_relation_dept_code"] = p_oAlarmSyntInfo.m_oAlarm.m_strCentreRelationDeptCode;
	l_tSQLReqInsertAlarm.param["receipt_dept_district_name"] = p_oAlarmSyntInfo.m_oAlarm.m_strReceiptDeptDistrictName;
	l_tSQLReqInsertAlarm.param["develop_dept_code"] = p_oAlarmSyntInfo.m_oAlarm.m_strAlarmSystemReceiptDeptCode;
	l_tSQLReqInsertAlarm.param["develop_dept_name"] = p_oAlarmSyntInfo.m_oAlarm.m_strAlarmSystemReceiptDeptName;
	l_tSQLReqInsertAlarm.param["linked_dispatch_code"] = p_oAlarmSyntInfo.m_oAlarm.m_strLinkedDispatchCode;
	l_tSQLReqInsertAlarm.param["is_signed"] = p_oAlarmSyntInfo.m_oAlarm.m_strIsSigned;
	l_tSQLReqInsertAlarm.param["over_time"] = p_oAlarmSyntInfo.m_oAlarm.m_strOverTime;
	l_tSQLReqInsertAlarm.param["initial_first_type"] = p_oAlarmSyntInfo.m_oAlarm.m_strInitialFirstType;
	l_tSQLReqInsertAlarm.param["initial_second_type"] = p_oAlarmSyntInfo.m_oAlarm.m_strInitialSecondType;
	l_tSQLReqInsertAlarm.param["initial_third_type"] = p_oAlarmSyntInfo.m_oAlarm.m_strInitialThirdType;
	l_tSQLReqInsertAlarm.param["initial_fourth_type"] = p_oAlarmSyntInfo.m_oAlarm.m_strInitialFourthType;
	l_tSQLReqInsertAlarm.param["initial_admin_dept_code"] = p_oAlarmSyntInfo.m_oAlarm.m_strInitialAdminDeptCode;
	l_tSQLReqInsertAlarm.param["initial_admin_dept_name"] = p_oAlarmSyntInfo.m_oAlarm.m_strInitialAdminDeptName;
	l_tSQLReqInsertAlarm.param["initial_admin_dept_org_code"] = p_oAlarmSyntInfo.m_oAlarm.m_strInitialAdminDeptOrgCode;
	l_tSQLReqInsertAlarm.param["initial_addr"] = p_oAlarmSyntInfo.m_oAlarm.m_strInitialAddr;
	l_tSQLReqInsertAlarm.param["receipt_dept_short_name"] = p_oAlarmSyntInfo.m_oAlarm.m_strReceiptDeptShortName;
	l_tSQLReqInsertAlarm.param["admin_dept_short_name"] = p_oAlarmSyntInfo.m_oAlarm.m_strAdminDeptShortName;
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertAlarm);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert alarm info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}

	//_UpdateStatisticAlarmNumInfo(p_oAlarmSyntInfo.m_oAlarm.m_strSeatNo);

	return true;
}

bool CBusinessImpl::_UpdateStatisticAlarmNumInfo(const std::string& strReceiptSeatno)
{
	DataBase::SQLRequest l_oSQLUpDate;
	l_oSQLUpDate.sql_id = "update_icc_t_statistic_by_alarm";
	l_oSQLUpDate.param["receipt_seatno"] = strReceiptSeatno;
	DataBase::IResultSetPtr l_oResult = m_pDBConn->Exec(l_oSQLUpDate);
	if (!l_oResult)
	{
		ICC_LOG_ERROR(m_pLog, "update statistic info for alarm num Failed: sql id[%s]", l_oSQLUpDate.sql_id.c_str());
		return false;
	}	

	if (!l_oResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update statistic info for alarm num Failed:[%s]", l_oResult->GetErrorMsg().c_str());
		return false;;
	}

	ICC_LOG_DEBUG(m_pLog, "update statistic info for alarm num success:[%s][%s]", l_oSQLUpDate.sql_id.c_str(), l_oResult->GetSQL().c_str());

	return true;
}

bool CBusinessImpl::SetSqlInsertProcess(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_oProcessInfo, const std::string &p_strState,DataBase::SQLRequest& p_tSQLInsert)
{
	p_tSQLInsert.sql_id = "insert_icc_t_pjdb";

	p_tSQLInsert.param["id"] = p_oProcessInfo.m_strID;
	p_tSQLInsert.param["alarm_id"] = p_oProcessInfo.m_strAlarmID;
	std::string strTime = m_pDateTime->GetAlarmIdTime(p_oProcessInfo.m_strAlarmID);
	if (strTime != "")
	{
		p_tSQLInsert.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime);
		p_tSQLInsert.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	p_tSQLInsert.param["jjsj"] = p_oProcessInfo.m_strReceivedTime;
	p_tSQLInsert.param["state"] = p_strState;
	p_tSQLInsert.param["dispatch_dept_district_code"] = p_oProcessInfo.m_strDispatchDeptDistrictCode;
	p_tSQLInsert.param["dispatch_dept_code"] = p_oProcessInfo.m_strDispatchDeptCode;
	p_tSQLInsert.param["dispatch_code"] = p_oProcessInfo.m_strDispatchCode;
	p_tSQLInsert.param["dispatch_name"] = p_oProcessInfo.m_strDispatchName;
	p_tSQLInsert.param["record_id"] = p_oProcessInfo.m_strRecordID;
	p_tSQLInsert.param["dispatch_suggestion"] = p_oProcessInfo.m_strDispatchSuggestion;
	p_tSQLInsert.param["process_dept_code"] = p_oProcessInfo.m_strProcessDeptCode;
	p_tSQLInsert.param["time_submit"] = p_oProcessInfo.m_strTimeSubmit;
	p_tSQLInsert.param["time_arrived"] = p_oProcessInfo.m_strTimeArrived;
	p_tSQLInsert.param["time_signed"] = p_oProcessInfo.m_strTimeSigned;
	p_tSQLInsert.param["process_name"] = p_oProcessInfo.m_strProcessName;
	p_tSQLInsert.param["process_code"] = p_oProcessInfo.m_strProcessCode;
	p_tSQLInsert.param["dispatch_personnel"] = p_oProcessInfo.m_strDispatchPersonnel;
	p_tSQLInsert.param["dispatch_vehicles"] = p_oProcessInfo.m_strDispatchVehicles;
	p_tSQLInsert.param["dispatch_boats"] = p_oProcessInfo.m_strDispatchBoats;
	p_tSQLInsert.param["dispatch_dept_name"] = p_oProcessInfo.m_strDispatchDeptName;
	p_tSQLInsert.param["dispatch_dept_org_code"] = p_oProcessInfo.m_strDispatchDeptOrgCode;
	p_tSQLInsert.param["process_dept_name"] = p_oProcessInfo.m_strProcessDeptName;
	p_tSQLInsert.param["process_dept_org_code"] = p_oProcessInfo.m_strProcessDeptOrgCode;
	p_tSQLInsert.param["process_object_type"] = p_oProcessInfo.m_strProcessObjectType;
	p_tSQLInsert.param["process_object_name"] = p_oProcessInfo.m_strProcessObjectName;
	p_tSQLInsert.param["process_object_code"] = p_oProcessInfo.m_strProcessObjectCode;
	p_tSQLInsert.param["business_status"] = p_oProcessInfo.m_strBusinessStatus;
	p_tSQLInsert.param["seat_code"] = p_oProcessInfo.m_strSeatCode;
	p_tSQLInsert.param["cancel_time"] = p_oProcessInfo.m_strCancelTime;
	p_tSQLInsert.param["cancel_reason"] = p_oProcessInfo.m_strCancelReason;
	p_tSQLInsert.param["is_auto_assign_job"] = p_oProcessInfo.m_strIsAutoAssignJob;
	if (p_oProcessInfo.m_strIsAutoAssignJob.empty())
	{
		p_tSQLInsert.param["is_auto_assign_job"] = "0";
	}
	p_tSQLInsert.param["overtime_state"] = p_oProcessInfo.m_strOvertimeState;

	p_tSQLInsert.param["create_user"] = p_oProcessInfo.m_strCreateUser.empty() ? p_oProcessInfo.m_strProcessCode : p_oProcessInfo.m_strCreateUser;
	p_tSQLInsert.param["create_time"] = p_oProcessInfo.m_strCreateTime.empty() ? m_pDateTime->CurrentDateTimeStr() : p_oProcessInfo.m_strCreateTime;
	if (!p_oProcessInfo.m_strUpdateUser.empty())
	{
		p_tSQLInsert.param["update_user"] = p_oProcessInfo.m_strUpdateUser;
	}

	if (!p_oProcessInfo.m_strUpdateTime.empty())
	{
		p_tSQLInsert.param["update_time"] = p_oProcessInfo.m_strUpdateTime;
	}

	p_tSQLInsert.param["process_object_id"] = p_oProcessInfo.m_strProcessObjectID;
	p_tSQLInsert.param["transfers_dept_org_code"] = p_oProcessInfo.m_strTransfDeptOrjCode;
	std::string l_strIsOver = "0";
	if (!p_oProcessInfo.m_strIsOver.empty())
	{
		l_strIsOver = p_oProcessInfo.m_strIsOver;
	}
	p_tSQLInsert.param["is_over"] = l_strIsOver;
	p_tSQLInsert.param["over_remark"] = p_oProcessInfo.m_strOverRemark;
	p_tSQLInsert.param["parent_id"] = p_oProcessInfo.m_strParentID;
	p_tSQLInsert.param["flow_code"] = p_oProcessInfo.m_strGZLDM;

	p_tSQLInsert.param["centre_process_dept_code"] = p_oProcessInfo.m_strCentreProcessDeptCode;
	p_tSQLInsert.param["centre_alarm_dept_code"] = p_oProcessInfo.m_strCentreAlarmDeptCode;
	p_tSQLInsert.param["dispatch_dept_district_name"] = p_oProcessInfo.m_strDispatchDeptDistrictName;
	p_tSQLInsert.param["linked_dispatch_code"] = p_oProcessInfo.m_strLinkedDispatchCode;
	p_tSQLInsert.param["over_time"] = p_oProcessInfo.m_strOverTime;
	p_tSQLInsert.param["finished_timeout_state"] = p_oProcessInfo.m_strFinishedTimeoutState;
	p_tSQLInsert.param["police_type"] = p_oProcessInfo.m_strPoliceType;
	p_tSQLInsert.param["process_dept_short_name"] = p_oProcessInfo.m_strProcessDeptShortName;
	p_tSQLInsert.param["dispatch_dept_short_name"] = p_oProcessInfo.m_strDispatchDeptShortName;
	p_tSQLInsert.param["createTeminal"] = p_oProcessInfo.m_strCreateTeminal;
	p_tSQLInsert.param["updateTeminal"] = p_oProcessInfo.m_strUpdateTeminal;

	return true;
}

bool CBusinessImpl::InsertProcessInfo(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_oProcessInfo, const std::string& strTransGuid)
{
	//VCS过来的使用VCS派警单的状态
	std::string l_strState(p_oProcessInfo.m_strState);
	

    if ("vcs_relocated" != p_oProcessInfo.m_strMsgSource && "vcs" != p_oProcessInfo.m_strMsgSource)
	{
		l_strState = _GetCurrentProcessState(p_oProcessInfo.m_strID, l_strState, strTransGuid);//"DIC020010";
	}
	

	DataBase::SQLRequest l_tSQLReqInsertProcess;
	if (SetSqlInsertProcess(p_oProcessInfo, l_strState,l_tSQLReqInsertProcess))
	{
		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertProcess, false, strTransGuid);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "insert alarm process info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
			return false;
		}
		return true;
	}
	return false;
	
}

bool CBusinessImpl::InsertAlarmLogInfo(PROTOCOL::CAlarmLogSync::CBody& p_AlarmLogInfo, const std::string& strTransGuid)
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

bool CBusinessImpl::InsertAlarmBookBeforeUpdate(const PROTOCOL::CAlarmInfo& p_oAlarmInfo, std::string& p_strResourceID, const std::string& strTransGuid, int& iModifyField)
{
	return true;

	//根据alarm_id从icc_t_alarm表查询警单
	//将查询到的警单备份到icc_t_alarm_book表中
}

bool CBusinessImpl::InsertProcessBookBeforeUpdate(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_oProcessInfo, std::string p_strResourceID, const std::string& strTransGuid)
{
	//不需要写处警book表
	return true;
}

bool CBusinessImpl::InsertAlarmBookAfterUpdate(const PROTOCOL::CAlarmInfo &p_oAlarmInfo, std::string p_strResourceID, const std::string& strTransGuid)
{
	//不需要写处警book表
	return true;

	//警单备份到icc_t_alarm_book表中
}

bool CBusinessImpl::InsertProcessBookAfterUpdate(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo, std::string p_strResourceID, const std::string& strTransGuid)
{
	return true;
	//将查询到的警单备份到book表中
}

bool CBusinessImpl::InsertVisit(const PROTOCOL::CAlarmVisitRequest& p_oVisitInfo, const std::string& strTransGuid)
{
	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "insert_icc_t_alarm_visit";

	l_tSQLReq.param["id"] = p_oVisitInfo.m_oBody.m_strID;
	l_tSQLReq.param["alarm_id"] = p_oVisitInfo.m_oBody.m_strAlarmID;
	l_tSQLReq.param["callref_id"] = p_oVisitInfo.m_oBody.m_strCallrefID;
	l_tSQLReq.param["dept_code"] = p_oVisitInfo.m_oBody.m_strDeptCode;
	l_tSQLReq.param["dept_name"] = p_oVisitInfo.m_oBody.m_strDeptName;
	l_tSQLReq.param["caller_no"] = p_oVisitInfo.m_oBody.m_strCallerNo;
	l_tSQLReq.param["caller_name"] = p_oVisitInfo.m_oBody.m_strCallerName;
	l_tSQLReq.param["time"] = p_oVisitInfo.m_oBody.m_strTime;
	l_tSQLReq.param["visitor_code"] = p_oVisitInfo.m_oBody.m_strVisitorCode;
	l_tSQLReq.param["visitor_name"] = p_oVisitInfo.m_oBody.m_strVisitorName;
	l_tSQLReq.param["satisfaction"] = p_oVisitInfo.m_oBody.m_strSatisfaction;
	l_tSQLReq.param["suggest"] = p_oVisitInfo.m_oBody.m_strSuggest;
	l_tSQLReq.param["result_content"] = p_oVisitInfo.m_oBody.m_strResultContent;
	l_tSQLReq.param["call_type"] = p_oVisitInfo.m_oBody.m_strCallType;
	l_tSQLReq.param["reason"] = p_oVisitInfo.m_oBody.m_strReason;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq, false, strTransGuid);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert_icc_t_alarm_visit failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

bool CBusinessImpl::UpdateVisit(const PROTOCOL::CAlarmVisitRequest& p_oVisitInfo, const std::string& strTransGuid)
{
	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "update_icc_t_alarm_visit";

	l_tSQLReq.param["id"] = p_oVisitInfo.m_oBody.m_strID;

	l_tSQLReq.set["alarm_id"] = p_oVisitInfo.m_oBody.m_strAlarmID;
	l_tSQLReq.set["callref_id"] = p_oVisitInfo.m_oBody.m_strCallrefID;
	l_tSQLReq.set["dept_code"] = p_oVisitInfo.m_oBody.m_strDeptCode;
	l_tSQLReq.set["dept_name"] = p_oVisitInfo.m_oBody.m_strDeptName;
	l_tSQLReq.set["caller_no"] = p_oVisitInfo.m_oBody.m_strCallerNo;
	l_tSQLReq.set["caller_name"] = p_oVisitInfo.m_oBody.m_strCallerName;
	l_tSQLReq.set["time"] = p_oVisitInfo.m_oBody.m_strTime;
	l_tSQLReq.set["visitor_code"] = p_oVisitInfo.m_oBody.m_strVisitorCode;
	l_tSQLReq.set["visitor_name"] = p_oVisitInfo.m_oBody.m_strVisitorName;
	l_tSQLReq.set["satisfaction"] = p_oVisitInfo.m_oBody.m_strSatisfaction;
	l_tSQLReq.set["suggest"] = p_oVisitInfo.m_oBody.m_strSuggest;
	l_tSQLReq.set["result_content"] = p_oVisitInfo.m_oBody.m_strResultContent;
	l_tSQLReq.set["call_type"] = p_oVisitInfo.m_oBody.m_strCallType;
	l_tSQLReq.set["reason"] = p_oVisitInfo.m_oBody.m_strReason;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq, false, strTransGuid);
	ICC_LOG_DEBUG(m_pLog, "UpdateAlarm sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update_icc_t_alarm_visit info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

bool CBusinessImpl::UpdateCallProcessInfo(const PROTOCOL::CAddOrUpdateAlarmWithProcessRequest::CBody& p_pAlarmReceiptInfo, const std::string& p_strSourceID)
{
	// 录音号为空,或者 警情来源方式不是 01 不用更新话务表状态
	ICC_LOG_DEBUG(m_pLog, " UpdateCallProcessInfo p_strSourceID is [%s]", p_strSourceID.c_str());
	if (p_strSourceID.empty() || p_pAlarmReceiptInfo.m_oAlarm.m_strSourceType.compare("01"))
	{
		ICC_LOG_DEBUG(m_pLog, " SourceID is empty or m_strSourceType is [%s]", p_pAlarmReceiptInfo.m_oAlarm.m_strSourceType.c_str());
		return true;
	}
	// 判断此录音话务是否为门市话务。
	if (!IsCallProcessBySourceId(p_strSourceID))
	{
		ICC_LOG_DEBUG(m_pLog, "p_strSourceID not CallProcess : p_strSourceID[%s]", p_strSourceID.c_str());
		return true;
	}

	DataBase::SQLRequest l_SQLRequest;
	l_SQLRequest.sql_id = "update_icc_t_callevent_alarm_relation";
	l_SQLRequest.param["condition"] = m_pString->Format(" accept_state = '%s' ", p_pAlarmReceiptInfo.m_oAlarm.m_strState.c_str());
	l_SQLRequest.param["callref_id"] = p_strSourceID;

	std::string strTime = m_pDateTime->GetCallRefIdTime(p_strSourceID);
	if (strTime != "")
	{
		l_SQLRequest.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
		l_SQLRequest.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	DataBase::IResultSetPtr l_Result = m_pDBConn->Exec(l_SQLRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_dept_by_phone failed,error msg:[%s]", l_Result->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

bool CBusinessImpl::IsCallProcessBySourceId(const std::string& p_strSourceID)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_callevent";
	l_SqlRequest.param["callref_id"] = p_strSourceID;

	std::string strTime = m_pDateTime->GetCallRefIdTime(p_strSourceID);
	if (strTime != "")
	{
		l_SqlRequest.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
		l_SqlRequest.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (l_pResult->RecordSize() == 1) 
	{
		return true;
	}
	return false;
}

bool CBusinessImpl::UpdateAlarmInfo(const PROTOCOL::CAddOrUpdateAlarmWithProcessRequest::CBody& p_pAlarmInfo, const std::string& strTransGuid)
{
	
	DataBase::SQLRequest l_tSQLReqInsertAlarm;
	
	l_tSQLReqInsertAlarm.sql_id = "select_icc_t_jjdb";
	l_tSQLReqInsertAlarm.param["id"] = p_pAlarmInfo.m_oAlarm.m_strID;
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertAlarm);
	ICC_LOG_DEBUG(m_pLog, "Select Alarm sql:[%s]", l_pRSet->GetSQL().c_str());
	
	std::string strLongitude;
	std::string strLatitude;
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select alarm  info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	if (l_pRSet->Next())
	{
		if(l_pRSet->GetValue("bjrxzb").empty()&&!p_pAlarmInfo.m_oAlarm.m_strManualLongitude.empty())
		{
			strLongitude = p_pAlarmInfo.m_oAlarm.m_strManualLongitude;
		}
		if (l_pRSet->GetValue("bjryzb").empty()&&!p_pAlarmInfo.m_oAlarm.m_strManualLatitude.empty())
		{
			strLatitude = p_pAlarmInfo.m_oAlarm.m_strManualLatitude;
		}
	}
	ICC_LOG_LOWDEBUG(m_pLog, "Longitude:[%s],Latitude:[%s]", strLongitude.c_str(), strLatitude.c_str());

	l_tSQLReqInsertAlarm.param.clear();
	l_tSQLReqInsertAlarm.sql_id = "update_icc_t_jjdb";

	//l_tSQLReqInsertAlarm.set["merge_id"] = p_pAlarmInfo.m_oAlarm.m_strMergeID;
	//
	//if (!p_pAlarmInfo.m_oAlarm.m_strSeatNo.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["receipt_seatno"] = p_pAlarmInfo.m_oAlarm.m_strSeatNo;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strTitle.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["title"] = p_pAlarmInfo.m_oAlarm.m_strTitle;
	//}	
	//if (!p_pAlarmInfo.m_oAlarm.m_strContent.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["content"] = p_pAlarmInfo.m_oAlarm.m_strContent;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strTime.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["time"] = p_pAlarmInfo.m_oAlarm.m_strTime;
	//}
	///*if (!p_pAlarmInfo.m_oAlarm.m_strActualOccurTime.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["actual_occur_time"] = p_pAlarmInfo.m_oAlarm.m_strActualOccurTime;
	//}*/
	//if (!p_pAlarmInfo.m_oAlarm.m_strAddr.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["addr"] = p_pAlarmInfo.m_oAlarm.m_strAddr;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strLongitude.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["longitude"] = p_pAlarmInfo.m_oAlarm.m_strLongitude;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strLatitude.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["latitude"] = p_pAlarmInfo.m_oAlarm.m_strLatitude;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strState.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["state"] = p_pAlarmInfo.m_oAlarm.m_strState;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strLevel.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["level"] = p_pAlarmInfo.m_oAlarm.m_strLevel;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strSourceType.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["source_type"] = p_pAlarmInfo.m_oAlarm.m_strSourceType;
	//}	
	//if (!p_pAlarmInfo.m_oAlarm.m_strSourceID.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["source_id"] = p_pAlarmInfo.m_oAlarm.m_strSourceID;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strHandleType.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["handle_type"] = p_pAlarmInfo.m_oAlarm.m_strHandleType;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strFirstType.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["first_type"] = p_pAlarmInfo.m_oAlarm.m_strFirstType;
	//}

	////2、3、4不做非空判断
	//l_tSQLReqInsertAlarm.set["second_type"] = p_pAlarmInfo.m_oAlarm.m_strSecondType;
	//l_tSQLReqInsertAlarm.set["third_type"] = p_pAlarmInfo.m_oAlarm.m_strThirdType;
	//l_tSQLReqInsertAlarm.set["fourth_type"] = p_pAlarmInfo.m_oAlarm.m_strFourthType;
	//
	//if (!p_pAlarmInfo.m_oAlarm.m_strVehicleNo.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["vehicle_no"] = p_pAlarmInfo.m_oAlarm.m_strVehicleNo;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strVehicleType.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["vehicle_type"] = p_pAlarmInfo.m_oAlarm.m_strVehicleType;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strSymbolCode.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["symbol_code"] = p_pAlarmInfo.m_oAlarm.m_strSymbolCode;
	//}
	///*if (!p_pAlarmInfo.m_oAlarm.m_strSymbolAddr.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["symbol_addr"] = p_pAlarmInfo.m_oAlarm.m_strSymbolAddr;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strFireBuildingType.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["fire_building_type"] = p_pAlarmInfo.m_oAlarm.m_strFireBuildingType;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strEventType.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["event_type"] = p_pAlarmInfo.m_oAlarm.m_strEventType;
	//}*/
	//if (!p_pAlarmInfo.m_oAlarm.m_strCalledNoType.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["called_no_type"] = p_pAlarmInfo.m_oAlarm.m_strCalledNoType;
	//}
	///*if (!p_pAlarmInfo.m_oAlarm.m_strActualCalledNoType.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["actual_called_no_type"] = p_pAlarmInfo.m_oAlarm.m_strActualCalledNoType;
	//}*/
	//if (!p_pAlarmInfo.m_oAlarm.m_strCallerNo.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["caller_no"] = p_pAlarmInfo.m_oAlarm.m_strCallerNo;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strCallerName.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["caller_name"] = p_pAlarmInfo.m_oAlarm.m_strCallerName;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strCallerAddr.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["caller_addr"] = p_pAlarmInfo.m_oAlarm.m_strCallerAddr;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strCallerID.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["caller_id"] = p_pAlarmInfo.m_oAlarm.m_strCallerID;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strCallerIDType.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["caller_id_type"] = p_pAlarmInfo.m_oAlarm.m_strCallerIDType;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strCallerGender.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["caller_gender"] = p_pAlarmInfo.m_oAlarm.m_strCallerGender;
	//}
	///*if (!p_pAlarmInfo.m_oAlarm.m_strCallerAge.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["caller_age"] = p_pAlarmInfo.m_oAlarm.m_strCallerAge;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strCallerBirthday.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["caller_birthday"] = p_pAlarmInfo.m_oAlarm.m_strCallerBirthday;
	//}*/
	//if (!p_pAlarmInfo.m_oAlarm.m_strContactNo.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["contact_no"] = p_pAlarmInfo.m_oAlarm.m_strContactNo;
	//}
	///*if (!p_pAlarmInfo.m_oAlarm.m_strContactName.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["contact_name"] = p_pAlarmInfo.m_oAlarm.m_strContactName;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strContactAddr.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["contact_addr"] = p_pAlarmInfo.m_oAlarm.m_strContactAddr;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strContactID.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["contact_id"] = p_pAlarmInfo.m_oAlarm.m_strContactID;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strContactIDType.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["contact_id_type"] = p_pAlarmInfo.m_oAlarm.m_strContactIDType;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strContactGender.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["contact_gender"] = p_pAlarmInfo.m_oAlarm.m_strContactGender;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strContactAge.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["contact_age"] = p_pAlarmInfo.m_oAlarm.m_strContactAge;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strContactBirthday.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["contact_birthday"] = p_pAlarmInfo.m_oAlarm.m_strContactBirthday;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strAdminDeptDistrictCode.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["admin_dept_district_code"] = p_pAlarmInfo.m_oAlarm.m_strAdminDeptDistrictCode;
	//}	*/
	//if (!p_pAlarmInfo.m_oAlarm.m_strAdminDeptCode.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["admin_dept_code"] = p_pAlarmInfo.m_oAlarm.m_strAdminDeptCode;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strAdminDeptName.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["admin_dept_name"] = p_pAlarmInfo.m_oAlarm.m_strAdminDeptName;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strReceiptDeptDistrictCode.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["receipt_dept_district_code"] = p_pAlarmInfo.m_oAlarm.m_strReceiptDeptDistrictCode;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strReceiptDeptCode.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["receipt_dept_code"] = p_pAlarmInfo.m_oAlarm.m_strReceiptDeptCode;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strReceiptDeptName.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["receipt_dept_name"] = p_pAlarmInfo.m_oAlarm.m_strReceiptDeptName;
	//}
	///*if (!p_pAlarmInfo.m_oAlarm.m_strLeaderCode.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["leader_code"] = p_pAlarmInfo.m_oAlarm.m_strLeaderCode;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strLeaderName.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["leader_name"] = p_pAlarmInfo.m_oAlarm.m_strLeaderName;
	//}*/
	//if (!p_pAlarmInfo.m_oAlarm.m_strReceiptCode.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["receipt_code"] = p_pAlarmInfo.m_oAlarm.m_strReceiptCode;
	//}
	//if (!p_pAlarmInfo.m_oAlarm.m_strReceiptName.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["receipt_name"] = p_pAlarmInfo.m_oAlarm.m_strReceiptName;
	//}
	///*if (!p_pAlarmInfo.m_oAlarm.m_strDispatchSuggestion.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["dispatch_suggestion"] = p_pAlarmInfo.m_oAlarm.m_strDispatchSuggestion;
	//}*/
	//if (!p_pAlarmInfo.m_oAlarm.m_strIsMerge.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["is_merge"] = p_pAlarmInfo.m_oAlarm.m_strIsMerge;
	//}

	///*if (!p_pAlarmInfo.m_oAlarm.m_strCityCode.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["city_code"] = p_pAlarmInfo.m_oAlarm.m_strCityCode;
	//}*/

	//if (!p_pAlarmInfo.m_oAlarm.m_strPrivacy.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["is_privacy"] = p_pAlarmInfo.m_oAlarm.m_strPrivacy;
	//}

	//if (!p_pAlarmInfo.m_oAlarm.m_strRemark.empty())
	//{
	//	l_tSQLReqInsertAlarm.set["remark"] = p_pAlarmInfo.m_oAlarm.m_strRemark;
	//}

	//if (!p_pAlarmInfo.m_oAlarm.m_strDeleteFlag.empty())  //2022/3/17 增加是否删除，软删除使用
	//{
	//	l_tSQLReqInsertAlarm.set["is_delete"] = p_pAlarmInfo.m_oAlarm.m_strDeleteFlag;
	//}

	//l_tSQLReqInsertAlarm.set["update_user"] = p_pAlarmInfo.m_oAlarm.m_strUpdateUser;
	//l_tSQLReqInsertAlarm.set["update_time"] = p_pAlarmInfo.m_oAlarm.m_strUpdateTime;

	l_tSQLReqInsertAlarm.set["merge_id"] = p_pAlarmInfo.m_oAlarm.m_strMergeID;

	if (!p_pAlarmInfo.m_oAlarm.m_strSeatNo.empty())
	{
		l_tSQLReqInsertAlarm.set["receipt_seatno"] = p_pAlarmInfo.m_oAlarm.m_strSeatNo;
	}
	
	if (!p_pAlarmInfo.m_oAlarm.m_strTitle.empty() || (p_pAlarmInfo.m_oAlarm.m_strTitle.empty() && p_pAlarmInfo.m_strAllowNull == "1"))
	{
		l_tSQLReqInsertAlarm.set["label"] = p_pAlarmInfo.m_oAlarm.m_strTitle;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strContent.empty())
	{
		l_tSQLReqInsertAlarm.set["content"] = p_pAlarmInfo.m_oAlarm.m_strContent;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strIsMerge.empty())
	{
		l_tSQLReqInsertAlarm.set["is_merge"] = p_pAlarmInfo.m_oAlarm.m_strIsMerge;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strTime.empty())
	{
		l_tSQLReqInsertAlarm.set["receiving_time"] = p_pAlarmInfo.m_oAlarm.m_strTime;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strReceivedTime.empty())
	{
		l_tSQLReqInsertAlarm.set["received_time"] = p_pAlarmInfo.m_oAlarm.m_strReceivedTime;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strAddr.empty())
	{
		l_tSQLReqInsertAlarm.set["addr"] = p_pAlarmInfo.m_oAlarm.m_strAddr;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strLongitude.empty())
	{
		l_tSQLReqInsertAlarm.set["longitude"] = p_pAlarmInfo.m_oAlarm.m_strLongitude;
	}
	else if (!strLongitude.empty() && p_pAlarmInfo.m_oAlarm.m_strLongitude.empty())
	{
		l_tSQLReqInsertAlarm.set["longitude"] = p_pAlarmInfo.m_oAlarm.m_strManualLongitude;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strLatitude.empty())
	{
		l_tSQLReqInsertAlarm.set["latitude"] = p_pAlarmInfo.m_oAlarm.m_strLatitude;
	}
	else if (!strLatitude.empty() && p_pAlarmInfo.m_oAlarm.m_strLatitude.empty())
	{
		l_tSQLReqInsertAlarm.set["latitude"] = p_pAlarmInfo.m_oAlarm.m_strManualLatitude;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strState.empty())
	{
		l_tSQLReqInsertAlarm.set["state"] = p_pAlarmInfo.m_oAlarm.m_strState;		
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strLevel.empty())
	{
		l_tSQLReqInsertAlarm.set["level"] = p_pAlarmInfo.m_oAlarm.m_strLevel;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strSourceType.empty())
	{
		l_tSQLReqInsertAlarm.set["source_type"] = p_pAlarmInfo.m_oAlarm.m_strSourceType;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strSourceID.empty())
	{
		l_tSQLReqInsertAlarm.set["source_id"] = p_pAlarmInfo.m_oAlarm.m_strSourceID;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strHandleType.empty())
	{
		l_tSQLReqInsertAlarm.set["handle_type"] = p_pAlarmInfo.m_oAlarm.m_strHandleType;
	}

	//if (!p_pAlarmInfo.m_oAlarm.m_strFirstType.empty())
	{
		l_tSQLReqInsertAlarm.set["first_type"] = p_pAlarmInfo.m_oAlarm.m_strFirstType;
	}

	////2、3、4不做非空判断
	l_tSQLReqInsertAlarm.set["second_type"] = p_pAlarmInfo.m_oAlarm.m_strSecondType;
	l_tSQLReqInsertAlarm.set["third_type"] = p_pAlarmInfo.m_oAlarm.m_strThirdType;
	l_tSQLReqInsertAlarm.set["fourth_type"] = p_pAlarmInfo.m_oAlarm.m_strFourthType;

	if (!p_pAlarmInfo.m_oAlarm.m_strVehicleNo.empty())
	{
		l_tSQLReqInsertAlarm.set["vehicle_no"] = p_pAlarmInfo.m_oAlarm.m_strVehicleNo;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strVehicleType.empty())
	{
		l_tSQLReqInsertAlarm.set["vehicle_type"] = p_pAlarmInfo.m_oAlarm.m_strVehicleType;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strSymbolCode.empty())
	{
		l_tSQLReqInsertAlarm.set["symbol_code"] = p_pAlarmInfo.m_oAlarm.m_strSymbolCode;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strCalledNoType.empty())
	{
		l_tSQLReqInsertAlarm.set["called_no_type"] = p_pAlarmInfo.m_oAlarm.m_strCalledNoType;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strCallerNo.empty()|| (p_pAlarmInfo.m_oAlarm.m_strCallerNo.empty() && p_pAlarmInfo.m_strAllowNull == "1"))
	{
		l_tSQLReqInsertAlarm.set["caller_no"] = p_pAlarmInfo.m_oAlarm.m_strCallerNo;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strCallerName.empty())
	{
		l_tSQLReqInsertAlarm.set["caller_name"] = p_pAlarmInfo.m_oAlarm.m_strCallerName;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strCallerAddr.empty())
	{
		l_tSQLReqInsertAlarm.set["caller_addr"] = p_pAlarmInfo.m_oAlarm.m_strCallerAddr;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strCallerID.empty())
	{
		l_tSQLReqInsertAlarm.set["caller_id"] = p_pAlarmInfo.m_oAlarm.m_strCallerID;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strCallerIDType.empty())
	{
		l_tSQLReqInsertAlarm.set["caller_id_type"] = p_pAlarmInfo.m_oAlarm.m_strCallerIDType;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strCallerGender.empty())
	{
		l_tSQLReqInsertAlarm.set["caller_gender"] = p_pAlarmInfo.m_oAlarm.m_strCallerGender;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strContactNo.empty())
	{
		l_tSQLReqInsertAlarm.set["contact_no"] = p_pAlarmInfo.m_oAlarm.m_strContactNo;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strAdminDeptCode.empty())
	{
		l_tSQLReqInsertAlarm.set["admin_dept_code"] = p_pAlarmInfo.m_oAlarm.m_strAdminDeptCode;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strAdminDeptName.empty())
	{
		l_tSQLReqInsertAlarm.set["admin_dept_name"] = p_pAlarmInfo.m_oAlarm.m_strAdminDeptName;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strReceiptDeptDistrictCode.empty())
	{
		l_tSQLReqInsertAlarm.set["receipt_dept_district_code"] = p_pAlarmInfo.m_oAlarm.m_strReceiptDeptDistrictCode;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strReceiptDeptCode.empty())
	{
		l_tSQLReqInsertAlarm.set["receipt_dept_code"] = p_pAlarmInfo.m_oAlarm.m_strReceiptDeptCode;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strReceiptDeptName.empty())
	{
		l_tSQLReqInsertAlarm.set["receipt_dept_name"] = p_pAlarmInfo.m_oAlarm.m_strReceiptDeptName;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strReceiptCode.empty())
	{
		l_tSQLReqInsertAlarm.set["receipt_code"] = p_pAlarmInfo.m_oAlarm.m_strReceiptCode;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strReceiptName.empty())
	{
		l_tSQLReqInsertAlarm.set["receipt_name"] = p_pAlarmInfo.m_oAlarm.m_strReceiptName;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strPrivacy.empty())
	{
		l_tSQLReqInsertAlarm.set["is_privacy"] = p_pAlarmInfo.m_oAlarm.m_strPrivacy;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strRemark.empty())
	{
		l_tSQLReqInsertAlarm.set["remark"] = p_pAlarmInfo.m_oAlarm.m_strRemark;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strIsVisitor.empty())
	{
		l_tSQLReqInsertAlarm.set["is_visitor"] = p_pAlarmInfo.m_oAlarm.m_strIsVisitor;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strIsFeedBack.empty())
	{
		l_tSQLReqInsertAlarm.set["is_feedback"] = p_pAlarmInfo.m_oAlarm.m_strIsFeedBack;
	}
	//新增
	if (!p_pAlarmInfo.m_oAlarm.m_strAlarmAddr.empty())
	{
		l_tSQLReqInsertAlarm.set["alarm_addr"] = p_pAlarmInfo.m_oAlarm.m_strAlarmAddr;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strCallerUserName.empty())
	{
		l_tSQLReqInsertAlarm.set["caller_user_name"] = p_pAlarmInfo.m_oAlarm.m_strCallerUserName;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strErpetratorsNumber.empty())
	{
		l_tSQLReqInsertAlarm.set["erpetrators_number"] = p_pAlarmInfo.m_oAlarm.m_strErpetratorsNumber;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strIsArmed.empty())
	{
		l_tSQLReqInsertAlarm.set["is_armed"] = p_pAlarmInfo.m_oAlarm.m_strIsArmed;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strIsHazardousSubstances.empty())
	{
		l_tSQLReqInsertAlarm.set["is_hazardous_substances"] = p_pAlarmInfo.m_oAlarm.m_strIsHazardousSubstances;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strIsExplosionOrLeakage.empty())
	{
		l_tSQLReqInsertAlarm.set["is_explosion_or_leakage"] = p_pAlarmInfo.m_oAlarm.m_strIsExplosionOrLeakage;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strDescOfTrapped.empty()||(p_pAlarmInfo.m_oAlarm.m_strDescOfTrapped.empty() && p_pAlarmInfo.m_strAllowNull == "1"))
	{
		l_tSQLReqInsertAlarm.set["desc_of_trapped"] = p_pAlarmInfo.m_oAlarm.m_strDescOfTrapped;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strDescOfInjured.empty() || (p_pAlarmInfo.m_oAlarm.m_strDescOfInjured.empty() && p_pAlarmInfo.m_strAllowNull == "1"))
	{
		l_tSQLReqInsertAlarm.set["desc_of_injured"] = p_pAlarmInfo.m_oAlarm.m_strDescOfInjured;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strDescOfDead.empty() || (p_pAlarmInfo.m_oAlarm.m_strDescOfDead.empty() && p_pAlarmInfo.m_strAllowNull == "1"))
	{
		l_tSQLReqInsertAlarm.set["desc_of_dead"] = p_pAlarmInfo.m_oAlarm.m_strDescOfDead;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strIsForeignLanguage.empty())
	{
		l_tSQLReqInsertAlarm.set["is_foreign_language"] = p_pAlarmInfo.m_oAlarm.m_strIsForeignLanguage;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strManualLongitude.empty())
	{
		l_tSQLReqInsertAlarm.set["manual_longitude"] = p_pAlarmInfo.m_oAlarm.m_strManualLongitude;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strManualLatitude.empty())
	{
		l_tSQLReqInsertAlarm.set["manual_latitude"] = p_pAlarmInfo.m_oAlarm.m_strManualLatitude;
	}
	
	if (!p_pAlarmInfo.m_oAlarm.m_strEmergencyRescueLevel.empty())
	{
		l_tSQLReqInsertAlarm.set["emergency_rescue_level"] = p_pAlarmInfo.m_oAlarm.m_strEmergencyRescueLevel;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strIsHazardousVehicle.empty())
	{
		l_tSQLReqInsertAlarm.set["is_hazardous_vehicle"] = p_pAlarmInfo.m_oAlarm.m_strIsHazardousVehicle;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strReceiptSrvName.empty())
	{
		l_tSQLReqInsertAlarm.set["receipt_srv_name"] = p_pAlarmInfo.m_oAlarm.m_strReceiptSrvName;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strAdminDeptOrgCode.empty())
	{
		l_tSQLReqInsertAlarm.set["admin_dept_org_code"] = p_pAlarmInfo.m_oAlarm.m_strAdminDeptOrgCode;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strReceiptDeptOrgCode.empty())
	{
		l_tSQLReqInsertAlarm.set["receipt_dept_org_code"] = p_pAlarmInfo.m_oAlarm.m_strReceiptDeptOrgCode;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strIsInvalid.empty())
	{
		l_tSQLReqInsertAlarm.set["is_invalid"] = p_pAlarmInfo.m_oAlarm.m_strIsInvalid;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strBusinessState.empty())
	{
		l_tSQLReqInsertAlarm.set["business_status"] = p_pAlarmInfo.m_oAlarm.m_strBusinessState;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strDeleteFlag.empty())
	{
		l_tSQLReqInsertAlarm.set["is_delete"] = p_pAlarmInfo.m_oAlarm.m_strDeleteFlag;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strIsSameForBackground.empty())
	{
		l_tSQLReqInsertAlarm.set["ybjxxyz"] = p_pAlarmInfo.m_oAlarm.m_strIsSameForBackground;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strIsProcessFlagSynchronized.empty())
	{
		l_tSQLReqInsertAlarm.set["tbcjbs"] = p_pAlarmInfo.m_oAlarm.m_strIsProcessFlagSynchronized;
	}
	
	if (!p_pAlarmInfo.m_oAlarm.m_strFirstSubmitTime.empty())
	{
		l_tSQLReqInsertAlarm.set["first_submit_time"] = p_pAlarmInfo.m_oAlarm.m_strFirstSubmitTime;
	}

	l_tSQLReqInsertAlarm.set["update_user"] = p_pAlarmInfo.m_oAlarm.m_strUpdateUser.empty() ? p_pAlarmInfo.m_oAlarm.m_strReceiptCode : p_pAlarmInfo.m_oAlarm.m_strUpdateUser;
	if (!p_pAlarmInfo.m_oAlarm.m_strUpdateTime.empty()&&p_pAlarmInfo.m_oAlarm.m_strUpdateTime>p_pAlarmInfo.m_oAlarm.m_strCreateTime)
	{
		l_tSQLReqInsertAlarm.set["update_time"] = p_pAlarmInfo.m_oAlarm.m_strUpdateTime;
	}
	else
	{
		l_tSQLReqInsertAlarm.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
	}
	l_tSQLReqInsertAlarm.param["id"] = p_pAlarmInfo.m_oAlarm.m_strID;

	std::string strTime = m_pDateTime->GetAlarmIdTime(p_pAlarmInfo.m_oAlarm.m_strID);
	if (strTime != "")
	{
		l_tSQLReqInsertAlarm.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_tSQLReqInsertAlarm.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strJurisdictionalOrgcode.empty())
	{
		l_tSQLReqInsertAlarm.set["jurisdictional_orgcode"] = p_pAlarmInfo.m_oAlarm.m_strJurisdictionalOrgcode;
	}
	if (!p_pAlarmInfo.m_oAlarm.m_strJurisdictionalOrgname.empty())
	{
		l_tSQLReqInsertAlarm.set["jurisdictional_orgname"] = p_pAlarmInfo.m_oAlarm.m_strJurisdictionalOrgname;
	}
	if (!p_pAlarmInfo.m_oAlarm.m_strJurisdictionalOrgidentifier.empty())
	{
		l_tSQLReqInsertAlarm.set["jurisdictional_orgidentifier"] = p_pAlarmInfo.m_oAlarm.m_strJurisdictionalOrgidentifier;
	}
	if (!p_pAlarmInfo.m_oAlarm.m_strHadPush.empty())
	{
		l_tSQLReqInsertAlarm.set["had_push"] = p_pAlarmInfo.m_oAlarm.m_strHadPush;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strIsOver.empty())
	{
		l_tSQLReqInsertAlarm.set["is_over"] = p_pAlarmInfo.m_oAlarm.m_strIsOver;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strCreateTeminal.empty())
	{
		l_tSQLReqInsertAlarm.set["createTeminal"] = p_pAlarmInfo.m_oAlarm.m_strCreateTeminal;
	}
	
	if (!p_pAlarmInfo.m_oAlarm.m_strUpdateTeminal.empty())
	{
		l_tSQLReqInsertAlarm.set["updateTeminal"] = p_pAlarmInfo.m_oAlarm.m_strUpdateTeminal;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strAlarmSourceType.empty())
	{
		l_tSQLReqInsertAlarm.set["alarm_source_type"] = p_pAlarmInfo.m_oAlarm.m_strAlarmSourceType;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strAlarmSourceId.empty())
	{
		l_tSQLReqInsertAlarm.set["alarm_source_id"] = p_pAlarmInfo.m_oAlarm.m_strAlarmSourceId;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strCentreDeptCode.empty())
	{
		l_tSQLReqInsertAlarm.set["centre_dept_code"] = p_pAlarmInfo.m_oAlarm.m_strCentreDeptCode;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strCentreRelationDeptCode.empty())
	{
		l_tSQLReqInsertAlarm.set["centre_relation_dept_code"] = p_pAlarmInfo.m_oAlarm.m_strCentreRelationDeptCode;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strReceiptDeptDistrictName.empty())
	{
		l_tSQLReqInsertAlarm.set["receipt_dept_district_name"] = p_pAlarmInfo.m_oAlarm.m_strReceiptDeptDistrictName;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strAlarmSystemReceiptDeptCode.empty())
	{
		l_tSQLReqInsertAlarm.set["develop_dept_code"] = p_pAlarmInfo.m_oAlarm.m_strAlarmSystemReceiptDeptCode;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strAlarmSystemReceiptDeptName.empty())
	{
		l_tSQLReqInsertAlarm.set["develop_dept_name"] = p_pAlarmInfo.m_oAlarm.m_strAlarmSystemReceiptDeptName;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strLinkedDispatchCode.empty())
	{
		l_tSQLReqInsertAlarm.set["linked_dispatch_code"] = p_pAlarmInfo.m_oAlarm.m_strLinkedDispatchCode;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strIsSigned.empty())
	{
		l_tSQLReqInsertAlarm.set["is_signed"] = p_pAlarmInfo.m_oAlarm.m_strIsSigned;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strOverTime.empty())
	{
		l_tSQLReqInsertAlarm.set["over_time"] = p_pAlarmInfo.m_oAlarm.m_strOverTime;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strInitialFirstType.empty())
	{
		l_tSQLReqInsertAlarm.set["initial_first_type"] = p_pAlarmInfo.m_oAlarm.m_strInitialFirstType;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strInitialSecondType.empty())
	{
		l_tSQLReqInsertAlarm.set["initial_second_type"] = p_pAlarmInfo.m_oAlarm.m_strInitialSecondType;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strInitialThirdType.empty())
	{
		l_tSQLReqInsertAlarm.set["initial_third_type"] = p_pAlarmInfo.m_oAlarm.m_strInitialThirdType;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strInitialFourthType.empty())
	{
		l_tSQLReqInsertAlarm.set["initial_fourth_type"] = p_pAlarmInfo.m_oAlarm.m_strInitialFourthType;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strInitialAdminDeptCode.empty())
	{
		l_tSQLReqInsertAlarm.set["initial_admin_dept_code"] = p_pAlarmInfo.m_oAlarm.m_strInitialAdminDeptCode;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strInitialAdminDeptName.empty())
	{
		l_tSQLReqInsertAlarm.set["initial_admin_dept_name"] = p_pAlarmInfo.m_oAlarm.m_strInitialAdminDeptName;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strInitialAdminDeptOrgCode.empty())
	{
		l_tSQLReqInsertAlarm.set["initial_admin_dept_org_code"] = p_pAlarmInfo.m_oAlarm.m_strInitialAdminDeptOrgCode;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strInitialAddr.empty())
	{
		l_tSQLReqInsertAlarm.set["initial_addr"] = p_pAlarmInfo.m_oAlarm.m_strInitialAddr;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strReceiptDeptShortName.empty())
	{
		l_tSQLReqInsertAlarm.set["receipt_dept_short_name"] = p_pAlarmInfo.m_oAlarm.m_strReceiptDeptShortName;
	}

	if (!p_pAlarmInfo.m_oAlarm.m_strAdminDeptShortName.empty())
	{
		l_tSQLReqInsertAlarm.set["admin_dept_short_name"] = p_pAlarmInfo.m_oAlarm.m_strAdminDeptShortName;
	}

	l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertAlarm, false, strTransGuid);
	ICC_LOG_DEBUG(m_pLog, "UpdateAlarm sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert alarm process info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	
	return true;
}

std::map<std::string, std::map<std::string, std::string>> CBusinessImpl::GetAlarmDataByAlarmID(const std::string& strAlarmID)
{
	std::map<std::string, std::map<std::string, std::string>> ret;
	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "select_icc_t_jjdb_by_alarm_id";
	l_tSQLReq.param["id"] = strAlarmID;
	std::string strTime = m_pDateTime->GetAlarmIdTime(strAlarmID);
	if (strTime != "")
	{
		l_tSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_tSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_tSQLReq);
	ICC_LOG_DEBUG(m_pLog, "GetAlarmDataByAlarmID sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "GetAlarmDataByAlarmID failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
		return ret;
	}
	std::vector<std::string> vecCloumns = l_pResult->GetFieldNames();
	unsigned int iCloumnsCount = vecCloumns.size();
	while (l_pResult->Next())
	{
		std::map<std::string, std::string> mapTmps;
		for (unsigned int i = 0; i < iCloumnsCount; ++i)
		{
		//	printf( "%s - %s\n", vecCloumns[i].c_str(), l_pResult->GetValue(vecCloumns[i]).c_str());
			mapTmps.insert(std::make_pair(vecCloumns[i], l_pResult->GetValue(vecCloumns[i])));
		}
		ret[strAlarmID] = mapTmps;
	}
	return ret;
}

void CBusinessImpl::UpdateAlarmLog(std::map<std::string, std::map<std::string, std::string>> strAlarmSourceValue, std::map<std::string, std::map<std::string, std::string>> strNewData, const std::string& strGuidId)
{
	for (auto strOld = strAlarmSourceValue.begin(), strNew = strNewData.begin(); strOld != strAlarmSourceValue.end(); strOld++)
	{
		// printf( "%s - %s", strOld->first.c_str(), strNew->first.c_str());
		std::vector<std::string> vecAlarmContent;
		std::string strAlarmContent = "[";
		if (strOld->first == strNew->first)
		{
			for (auto data1 = strOld->second.begin(), data2 = strNew->second.begin(); data1 != strOld->second.end() && data2 != strNew->second.end(); data1++)
			{
				if (data1->first == data2->first)
				{
					if (data1->second != data2->second)
					{
		//				ICC_LOG_DEBUG(m_pLog,"%s - %s : %s - %s", data1->first.c_str(), data2->first.c_str(), data1->second.c_str(), data2->second.c_str());
						std::string strTemp = "[" + data1->first + ", " + data1->second + ", " + data2->second + "]";
						vecAlarmContent.push_back(strTemp);
					}
				}
				data2++;
			}
			if (vecAlarmContent.empty()) {
				return;
			}
			for (int iCount = 0; iCount < vecAlarmContent.size(); iCount++)
			{
				strAlarmContent += vecAlarmContent[iCount];
				if (iCount < vecAlarmContent.size() - 1)
				{
					strAlarmContent  += ",";
				}
			}
			strAlarmContent += "]";
			DataBase::SQLRequest l_tSQLReq;
			l_tSQLReq.sql_id = "insert_icc_t_jjdb_log";
			l_tSQLReq.param["guid"] = strGuidId;
			l_tSQLReq.param["id"] = strOld->first;
			auto mapData = strOld->second.find("jjdwdm");
			if (mapData != strOld->second.end()) 
			{
				l_tSQLReq.param["alarm_id"] = mapData->second;
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "no find icc_t_jjdb. jjdwdm :[%s]", strOld->first.c_str());
			}
			l_tSQLReq.param["alarmtime"] = m_pDateTime->CurrentDateTimeStr();
			mapData = strOld->second.find("gxry");
			if (mapData != strOld->second.end())
			{
				l_tSQLReq.param["update_user"] = mapData->second;
			}
			else
			{
				printf( "no find icc_t_jjdb. gxry :[%s]", strOld->first.c_str());
			}
			ICC_LOG_DEBUG(m_pLog,"find strAlarmContent. gxry :[%s]", strAlarmContent.c_str());
			l_tSQLReq.param["alarm_content"] = strAlarmContent;
			DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq);
			ICC_LOG_DEBUG(m_pLog, "insert_icc_t_jjdb_log sql:[%s]", l_pRSet->GetSQL().c_str());
			if (!l_pRSet->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "insert_icc_t_jjdb_log failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
				return ;
			}
			return;
		}
		ICC_LOG_DEBUG(m_pLog, " strAlarmSourceValue diff  strNewData ");
	}
}

void CBusinessImpl::AnalogProcessData(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& p_oAlarmInfo)
{
	PROTOCOL::CAddOrUpdateProcessRequest::CProcessData l_processData;
	l_processData.m_bIsNewProcess = true;																		//是否是新增的处警单，true代表是，false代表否
	l_processData.m_strMsgSource = p_oAlarmInfo.m_oBody.m_oAlarm.m_strMsgSource;								//消息来源
	l_processData.m_strID = _GenId(p_oAlarmInfo.m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode);//m_pDateTime->CreateSerial();//m_pString->CreateGuid();															//处警ID
	l_processData.m_strAlarmID = p_oAlarmInfo.m_oBody.m_oAlarm.m_strID;											//警情ID
	l_processData.m_strState = "02";																		//处警单状态 默认给"已下达"
	//l_processData.m_strTimeEdit = m_pDateTime->CurrentDateTimeStr();												//派警单填写时间
	l_processData.m_strTimeSubmit = m_pDateTime->CurrentDateTimeStr();;											//派警单提交时间

	l_processData.m_strDispatchDeptDistrictCode = p_oAlarmInfo.m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode;	//派警单位行政区划
	l_processData.m_strDispatchDeptCode = p_oAlarmInfo.m_oBody.m_oAlarm.m_strReceiptDeptCode;					//派警单位代码
	l_processData.m_strDispatchDeptName = p_oAlarmInfo.m_oBody.m_oAlarm.m_strReceiptDeptName;					//派警单位名称
	l_processData.m_strDispatchCode = p_oAlarmInfo.m_oBody.m_oAlarm.m_strReceiptCode;							//派警人警号
	l_processData.m_strDispatchName = p_oAlarmInfo.m_oBody.m_oAlarm.m_strReceiptName;							//派警人姓名

	//l_processData.m_strProcessDeptDistrictCode = p_oAlarmInfo.m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode;	//处警单位行政区划
	l_processData.m_strProcessDeptCode = p_oAlarmInfo.m_oBody.m_oAlarm.m_strReceiptDeptCode;					//处警单位代码
	l_processData.m_strProcessDeptName = p_oAlarmInfo.m_oBody.m_oAlarm.m_strReceiptDeptName;					//处警单位名称
	l_processData.m_strProcessCode = p_oAlarmInfo.m_oBody.m_oAlarm.m_strReceiptCode;							//处警人警号
	l_processData.m_strProcessName = p_oAlarmInfo.m_oBody.m_oAlarm.m_strReceiptName;							//处警人姓名
	p_oAlarmInfo.m_oBody.m_vecProcessData.push_back(l_processData);
}

bool CBusinessImpl::UpdateProcessInfo(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pAlarmProcessInfo, const std::string& strTransGuid)
{
	// VCS过来的使用VCS派警单的状态
    std::string l_strState(p_pAlarmProcessInfo.m_strState);

	if ("vcs_relocated" != p_pAlarmProcessInfo.m_strMsgSource && "vcs" != p_pAlarmProcessInfo.m_strMsgSource)
	{
		//l_strState = RealProcessState(p_pAlarmProcessInfo.m_strID, p_pAlarmProcessInfo.m_strState, PROCESS_STATUS_TAKEN, strTransGuid); //"DIC020010";
		l_strState = _GetCurrentProcessState(p_pAlarmProcessInfo.m_strID, l_strState, strTransGuid);
	}

	if (l_strState.empty())
	{
		return false;
	}

	DataBase::SQLRequest l_tSQLRequest;
	if (SetSqlUpdateProcess(p_pAlarmProcessInfo, l_tSQLRequest))
	{
		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLRequest, false, strTransGuid);
		ICC_LOG_DEBUG(m_pLog, "UpdateProcess sql:[%s]", l_pRSet->GetSQL().c_str());
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "update alarm process info failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
			return false;
		}
		return true;
	}
	return false;
}

bool CBusinessImpl::UpdateIsVisitor(std::string p_strAlarmID, const std::string& strTransGuid)
{
	//std::string l_strQueryAlarmSQL = "select is_visitor FROM public.icc_t_alarm where id = '" + p_strAlarmID + "'";
	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "select_icc_t_jjdb";

	l_tSQLReq.param["id"] = p_strAlarmID;

	std::string strTime = m_pDateTime->GetAlarmIdTime(p_strAlarmID);
	if (strTime != "")
	{
		l_tSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_tSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq, false, strTransGuid);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "QueryAlarmSQL failed message:[%s]", l_pRSet->GetSQL().c_str());
		return false;
	}
	if (l_pRSet->Next() && ("0" == l_pRSet->GetValue("is_visitor")))
	{
		l_tSQLReq.sql_id = "update_icc_t_jjdb";

		l_tSQLReq.set["is_visitor"] = "1";
		l_tSQLReq.param["id"] = p_strAlarmID;

		std::string strTime = m_pDateTime->GetAlarmIdTime(p_strAlarmID);
		if (strTime != "")
		{
			l_tSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
			l_tSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
		}

		//std::string l_strUpdateAlarmSQL = "UPDATE public.icc_t_alarm set is_visitor = '1' where id = '" + p_strAlarmID + "'";
		DataBase::IResultSetPtr l_pUpdateRSet = m_pDBConn->Exec(l_tSQLReq, false, strTransGuid);
		if (!l_pUpdateRSet->IsValid())
		{
			ICC_LOG_DEBUG(m_pLog, "UpdateAlarmSQL failed message:[%s]", l_pUpdateRSet->GetSQL().c_str());
			return false;
		}
	}

	return true;
}

void ICC::CBusinessImpl::GetStaffDept(std::string p_strUserCode, std::string &p_strStaffName, std::string &p_strDeptName, std::string &p_strDeptCode)
{
	DataBase::SQLRequest l_tSQL;
	l_tSQL.sql_id = "select_icc_t_staff_dept";
	l_tSQL.param["user_code"] = p_strUserCode;
	DataBase::IResultSetPtr l_pRSetStaff = m_pDBConn->Exec(l_tSQL);
	ICC_LOG_DEBUG(m_pLog, "GetStaffDept sql:[%s]", l_pRSetStaff->GetSQL().c_str());
	if (!l_pRSetStaff->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert alarm log info failed,error msg:[%s]", l_pRSetStaff->GetErrorMsg().c_str());
	}
	if (l_pRSetStaff->Next())
	{
		p_strStaffName = l_pRSetStaff->GetValue("name");
		p_strDeptCode = l_pRSetStaff->GetValue("deptcode");
		p_strDeptName = l_pRSetStaff->GetValue("deptname");
	}
}

void ICC::CBusinessImpl::GetStaffDept2(std::string p_strUserName, std::string &p_strUserCode, std::string &p_strDeptName, std::string &p_strDeptCode)
{
	DataBase::SQLRequest l_tSQL;
	l_tSQL.sql_id = "select_icc_t_staff_dept2";
	l_tSQL.param["name"] = p_strUserName;
	DataBase::IResultSetPtr l_pRSetStaff = m_pDBConn->Exec(l_tSQL);
	ICC_LOG_DEBUG(m_pLog, "GetStaffDept sql:[%s]", l_pRSetStaff->GetSQL().c_str());
	if (!l_pRSetStaff->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert alarm log info failed,error msg:[%s]", l_pRSetStaff->GetErrorMsg().c_str());
	}
	if (l_pRSetStaff->Next())
	{
		p_strUserCode = l_pRSetStaff->GetValue("usercode");
		p_strDeptCode = l_pRSetStaff->GetValue("deptcode");
		p_strDeptName = l_pRSetStaff->GetValue("deptname");
	}
}

std::string CBusinessImpl::RealProcessState(std::string p_strID, std::string p_strCurState, std::string p_strInputState, const std::string& strTransGuid)
{
	// p_strCurState与p_strInputState比较顺序，返回顺序值较大的字符串
	std::string l_strState;
	if (p_strCurState.empty())
	{
		l_strState = p_strInputState;
	}
	else
	{
		l_strState = (m_mapProcessStateOrder[p_strCurState] > m_mapProcessStateOrder[p_strInputState])
			? p_strCurState : p_strInputState;
	}

	ICC_LOG_DEBUG(m_pLog, "cur state: %s, input state: %s, get real state: %s", p_strCurState.c_str(), p_strInputState.c_str(), l_strState.c_str());
	// 查询当前数据库中的state值
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_pjdb";
	l_SqlRequest.param["id"] = p_strID;

	std::string strTime = m_pDateTime->GetDispatchIdTime(p_strID);
	if (strTime != "")
	{
		l_SqlRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 86400 * 15);
		l_SqlRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest, false, strTransGuid);

	if (!l_pResult->IsValid())
	{
		ICC_LOG_DEBUG(m_pLog, "exec sql fail[%s]", l_pResult->GetErrorMsg().c_str());
		return l_strState;
	}
	if (!l_pResult->Next())
	{
		ICC_LOG_DEBUG(m_pLog, "result is empty");
		return l_strState;
	}

	std::string l_strPreState = l_pResult->GetValue("state");

	/*当前状态为已签收，请求状态为已取消时返回失败
	当前状态为已反馈，请求状态为已取消时返回失败
	当前状态为已取消，请求状态为已签收时返回失败
	当前状态为已取消，请求状态为已反馈时返回失败
	if ((l_strPreState.compare("DIC020030") == 0 && p_strCurState.compare("DIC020071") == 0)
		|| (l_strPreState.compare("DIC020060") == 0 && p_strCurState.compare("DIC020071") == 0)
		|| (l_strPreState.compare("DIC020071") == 0 && p_strCurState.compare("DIC020030") == 0)
		|| (l_strPreState.compare("DIC020071") == 0 && p_strCurState.compare("DIC020060") == 0))*/

	// 当前状态为已取消，请求状态为已签收时返回失败
	if (l_strPreState.compare("DIC020071") == 0 && p_strCurState.compare("DIC020030") == 0)
	{
		return "";
	}

	// 数据库中的state值与p_strState比较顺序
	if (!l_strPreState.empty())
	{
		if (m_mapProcessStateOrder[l_strPreState] > m_mapProcessStateOrder[l_strState])
		{
			return l_strPreState;
		}
	}
	return l_strState;
}

bool CBusinessImpl::SelectChildDeptCount(const PROTOCOL::CSearchAlarmVisitStatisticRequest::CBody& p_oReqBody, PROTOCOL::CSearchAlarmVisitStatisticRespond::CBody& p_oRespBody)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_alarm_visit_statistic_dept_count";

	l_SqlRequest.param["dept_code"] = p_oReqBody.m_strDeptCode;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	if (l_pResult->Next())
	{
		p_oRespBody.m_strCount = l_pResult->GetValue("count");
	}
	return true;
}

std::string CBusinessImpl::_PacketDeptInfo(const std::string& strDeptCode)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_alarm_visit_child_depts_by_deptcode";
	l_SqlRequest.param["dept_code"] = strDeptCode;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
		return "";
	}
	
	std::string strTmpDeptCode;

	while (l_pResult->Next())
	{
		PROTOCOL::CSearchAlarmVisitStatisticRespond::CData l_oData;
		std::string strTmp = "'";
		strTmp += l_pResult->GetValue("code");
		strTmp += "'";
		strTmp += ",";
		strTmpDeptCode += strTmp;
	}
	
	if (!strTmpDeptCode.empty())
	{
		strTmpDeptCode = strTmpDeptCode.substr(0, strTmpDeptCode.length() - 1);
	}

	return strTmpDeptCode;
}

bool CBusinessImpl::SelectChildDept(const PROTOCOL::CSearchAlarmVisitStatisticRequest::CBody& p_oReqBody, PROTOCOL::CSearchAlarmVisitStatisticRespond::CBody& p_oRespBody)
{
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_alarm_visit_statistic_dept";
	if (!p_oReqBody.m_strPageSize.empty() && !p_oReqBody.m_strPageIndex.empty())
	{
		l_SqlRequest.param["page_size"] = p_oReqBody.m_strPageSize;
		l_SqlRequest.param["page_index"] = p_oReqBody.m_strPageIndex;
	}

	l_SqlRequest.param["dept_code"] = p_oReqBody.m_strDeptCode;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	//p_oRespBody.m_strCount = std::to_string(l_pResult->RecordSize());

	//DataBase::IResultSetPtr l_TmpResult = l_pResult;
	while (l_pResult->Next())
	{
		PROTOCOL::CSearchAlarmVisitStatisticRespond::CData l_oData;		

		l_oData.m_strDeptCode = l_pResult->GetValue("code");
		l_oData.m_strDeptName = l_pResult->GetValue("name");
		std::string l_strDepth = l_pResult->GetValue("depth");

		if (!l_oData.m_strDeptCode.empty())
		{
			l_oData.m_strAlarmCount = SelectAlarmCount(l_oData.m_strDeptCode, p_oReqBody);
			l_oData.m_strVisitCount = SelectVisitCount(l_oData.m_strDeptCode, p_oReqBody);
			l_oData.m_strSatisfactoryCount = SelectSatisfactoryCount(l_oData.m_strDeptCode, p_oReqBody);

			unsigned int uiAvailCount = SelectAvailVisitCount(l_oData.m_strDeptCode, p_oReqBody);

			l_oData.m_strUnsatisfactoryCount = ((l_oData.m_strVisitCount >= l_oData.m_strSatisfactoryCount) ? (l_oData.m_strVisitCount - l_oData.m_strSatisfactoryCount) : 0);
			l_oData.m_strSatisfaction = ((l_oData.m_strVisitCount != 0) ? ((double)l_oData.m_strSatisfactoryCount / l_oData.m_strVisitCount) : 0);
			l_oData.m_strVisitRate = ((l_oData.m_strAlarmCount != 0) ? ((double)uiAvailCount / l_oData.m_strAlarmCount) : 0);

			p_oRespBody.m_vecData.push_back(l_oData);
		}		
	}
	ICC_LOG_DEBUG(m_pLog, "SelectDBAlarmLogByAlarmID Success");
	return true;
}

unsigned int CBusinessImpl::SelectAlarmCount(const std::string p_strDeptCode, const PROTOCOL::CSearchAlarmVisitStatisticRequest::CBody& p_oReqBody)
{
	unsigned int l_uiCount = 0;
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_alarm_visit_statistic_alarm";
	if (!p_oReqBody.m_strBeginTime.empty() && !p_oReqBody.m_strEndTime.empty())
	{
		l_SqlRequest.param["begin_time"] = p_oReqBody.m_strBeginTime;
		l_SqlRequest.param["end_time"] = p_oReqBody.m_strEndTime;
	}

	l_SqlRequest.param["dept_code"] = p_strDeptCode;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
		return l_uiCount;
	}

	if (l_pResult->Next())
	{		
		l_uiCount = m_pString->ToUInt(l_pResult->GetValue("alarm_count"));
	}
	return l_uiCount;
}

unsigned int CBusinessImpl::SelectVisitCount(const std::string p_strDeptCode, const PROTOCOL::CSearchAlarmVisitStatisticRequest::CBody& p_oReqBody)
{
	unsigned int l_uiCount = 0;
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_alarm_visit_statistic_visit";
	if (!p_oReqBody.m_strBeginTime.empty() && !p_oReqBody.m_strEndTime.empty())
	{
		l_SqlRequest.param["begin_time"] = p_oReqBody.m_strBeginTime;
		l_SqlRequest.param["end_time"] = p_oReqBody.m_strEndTime;
	}

	l_SqlRequest.param["dept_code"] = p_strDeptCode;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
		return l_uiCount;
	}

	if (l_pResult->Next())
	{
		l_uiCount = m_pString->ToUInt(l_pResult->GetValue("visit_count"));
	}
	return l_uiCount;
}

unsigned int CBusinessImpl::SelectAvailVisitCount(const std::string p_strDeptCode, const PROTOCOL::CSearchAlarmVisitStatisticRequest::CBody& p_oReqBody)
{
	unsigned int l_uiCount = 0;
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_alarm_visit_statistic_visit_availcount";
	if (!p_oReqBody.m_strBeginTime.empty() && !p_oReqBody.m_strEndTime.empty())
	{
		l_SqlRequest.param["begin_time"] = p_oReqBody.m_strBeginTime;
		l_SqlRequest.param["end_time"] = p_oReqBody.m_strEndTime;
	}

	l_SqlRequest.param["dept_code"] = p_strDeptCode;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
		return l_uiCount;
	}

	if (l_pResult->Next())
	{
		l_uiCount = m_pString->ToUInt(l_pResult->GetValue("visit_count"));
	}
	return l_uiCount;
}

unsigned int CBusinessImpl::SelectSatisfactoryCount(const std::string p_strDeptCode, const PROTOCOL::CSearchAlarmVisitStatisticRequest::CBody& p_oReqBody)
{
	unsigned int l_uiCount = 0;
	DataBase::SQLRequest l_SqlRequest;
	l_SqlRequest.sql_id = "select_icc_t_alarm_visit_statistic_statisfactory";
	if (!p_oReqBody.m_strBeginTime.empty() && !p_oReqBody.m_strEndTime.empty())
	{
		l_SqlRequest.param["begin_time"] = p_oReqBody.m_strBeginTime;
		l_SqlRequest.param["end_time"] = p_oReqBody.m_strEndTime;
	}

	l_SqlRequest.param["dept_code"] = p_strDeptCode;

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_SqlRequest);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_pResult->GetErrorMsg().c_str());
		return l_uiCount;
	}

	if (l_pResult->Next())
	{
		l_uiCount = m_pString->ToUInt(l_pResult->GetValue("satisfactory_count"));
	}
	return l_uiCount;
}


bool CBusinessImpl::GetAlarm(std::string l_strID, PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& l_oAlarm)
{
	DataBase::SQLRequest l_oSeleteAlarmSQLReq;
	l_oSeleteAlarmSQLReq.sql_id = "select_icc_t_jjdb";
	l_oSeleteAlarmSQLReq.param["id"] = l_strID;

	std::string strTime = m_pDateTime->GetAlarmIdTime(l_strID);
	if (strTime != "")
	{
		l_oSeleteAlarmSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_oSeleteAlarmSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}
	if (!m_pDBConn)
	{
		ICC_LOG_ERROR(m_pLog, "db connect is null!!!");
		return false;
	}
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oSeleteAlarmSQLReq);
	ICC_LOG_DEBUG(m_pLog, "GetAlarm sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "get alarm failed, [%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "get alarm success, [%s]", l_strID.c_str());

	while (l_pResult->Next())
	{
		//l_oAlarm.m_oBody.m_oAlarm.m_strID = l_pResult->GetValue("id");
		//l_oAlarm.m_oBody.m_oAlarm.m_strMergeID = l_pResult->GetValue("merge_id");
		//l_oAlarm.m_oBody.m_oAlarm.m_strSeatNo = l_pResult->GetValue("receipt_seatno");
		//l_oAlarm.m_oBody.m_oAlarm.m_strTitle = l_pResult->GetValue("title");
		//l_oAlarm.m_oBody.m_oAlarm.m_strContent = l_pResult->GetValue("content");
		//l_oAlarm.m_oBody.m_oAlarm.m_strTime = l_pResult->GetValue("time");
		////l_oAlarm.m_oBody.m_oAlarm.m_strActualOccurTime = l_pResult->GetValue("actual_occur_time");
		//l_oAlarm.m_oBody.m_oAlarm.m_strAddr = l_pResult->GetValue("addr");
		//l_oAlarm.m_oBody.m_oAlarm.m_strLongitude = l_pResult->GetValue("longitude");
		//l_oAlarm.m_oBody.m_oAlarm.m_strLatitude = l_pResult->GetValue("latitude");
		//l_oAlarm.m_oBody.m_oAlarm.m_strState = l_pResult->GetValue("state");
		//
		//l_oAlarm.m_oBody.m_oAlarm.m_strLevel = l_pResult->GetValue("level");
		//l_oAlarm.m_oBody.m_oAlarm.m_strSourceType = l_pResult->GetValue("source_type");
		//l_oAlarm.m_oBody.m_oAlarm.m_strSourceID = l_pResult->GetValue("source_id");
		//l_oAlarm.m_oBody.m_oAlarm.m_strHandleType = l_pResult->GetValue("handle_type");
		//l_oAlarm.m_oBody.m_oAlarm.m_strFirstType = l_pResult->GetValue("first_type");
		//l_oAlarm.m_oBody.m_oAlarm.m_strSecondType = l_pResult->GetValue("second_type");
		//l_oAlarm.m_oBody.m_oAlarm.m_strThirdType = l_pResult->GetValue("third_type");
		//l_oAlarm.m_oBody.m_oAlarm.m_strFourthType = l_pResult->GetValue("fourth_type");
		//l_oAlarm.m_oBody.m_oAlarm.m_strVehicleNo = l_pResult->GetValue("vehicle_no");
		//l_oAlarm.m_oBody.m_oAlarm.m_strVehicleType = l_pResult->GetValue("vehicle_type");
		//l_oAlarm.m_oBody.m_oAlarm.m_strSymbolCode = l_pResult->GetValue("symbol_code");
		///*l_oAlarm.m_oBody.m_oAlarm.m_strSymbolAddr = l_pResult->GetValue("symbol_addr");
		//l_oAlarm.m_oBody.m_oAlarm.m_strFireBuildingType = l_pResult->GetValue("fire_building_type");
		//l_oAlarm.m_oBody.m_oAlarm.m_strEventType = l_pResult->GetValue("event_type");*/
		//l_oAlarm.m_oBody.m_oAlarm.m_strCalledNoType = l_pResult->GetValue("called_no_type");
		////l_oAlarm.m_oBody.m_oAlarm.m_strActualCalledNoType = l_pResult->GetValue("actual_called_no_type");
		//l_oAlarm.m_oBody.m_oAlarm.m_strCallerNo = l_pResult->GetValue("caller_no");
		//l_oAlarm.m_oBody.m_oAlarm.m_strCallerName = l_pResult->GetValue("caller_name");
		//l_oAlarm.m_oBody.m_oAlarm.m_strCallerAddr = l_pResult->GetValue("caller_addr");
		//l_oAlarm.m_oBody.m_oAlarm.m_strCallerID = l_pResult->GetValue("caller_id");
		//l_oAlarm.m_oBody.m_oAlarm.m_strCallerIDType = l_pResult->GetValue("caller_id_type");
		//l_oAlarm.m_oBody.m_oAlarm.m_strCallerGender = l_pResult->GetValue("caller_gender");
		////l_oAlarm.m_oBody.m_oAlarm.m_strCallerAge = l_pResult->GetValue("caller_age");
		////l_oAlarm.m_oBody.m_oAlarm.m_strCallerBirthday = l_pResult->GetValue("caller_birthday");
		//l_oAlarm.m_oBody.m_oAlarm.m_strContactNo = l_pResult->GetValue("contact_no");
		////l_oAlarm.m_oBody.m_oAlarm.m_strContactName = l_pResult->GetValue("contact_name");
		////l_oAlarm.m_oBody.m_oAlarm.m_strContactAddr = l_pResult->GetValue("contact_addr");
		////l_oAlarm.m_oBody.m_oAlarm.m_strContactID = l_pResult->GetValue("contact_id");
		////l_oAlarm.m_oBody.m_oAlarm.m_strContactIDType = l_pResult->GetValue("contact_id_type");
		////l_oAlarm.m_oBody.m_oAlarm.m_strContactGender = l_pResult->GetValue("contact_gender");
		////l_oAlarm.m_oBody.m_oAlarm.m_strContactAge = l_pResult->GetValue("contact_age");
		////l_oAlarm.m_oBody.m_oAlarm.m_strContactBirthday = l_pResult->GetValue("contact_birthday");
		////l_oAlarm.m_oBody.m_oAlarm.m_strAdminDeptDistrictCode = l_pResult->GetValue("admin_dept_district_code");
		//l_oAlarm.m_oBody.m_oAlarm.m_strAdminDeptCode = l_pResult->GetValue("admin_dept_code");
		//l_oAlarm.m_oBody.m_oAlarm.m_strAdminDeptName = l_pResult->GetValue("admin_dept_name");
		//l_oAlarm.m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode = l_pResult->GetValue("receipt_dept_district_code");
		//l_oAlarm.m_oBody.m_oAlarm.m_strReceiptDeptCode = l_pResult->GetValue("receipt_dept_code");
		//l_oAlarm.m_oBody.m_oAlarm.m_strReceiptDeptName = l_pResult->GetValue("receipt_dept_name");
		////l_oAlarm.m_oBody.m_oAlarm.m_strLeaderCode = l_pResult->GetValue("leader_code");
		////l_oAlarm.m_oBody.m_oAlarm.m_strLeaderName = l_pResult->GetValue("leader_name");
		//l_oAlarm.m_oBody.m_oAlarm.m_strReceiptCode = l_pResult->GetValue("receipt_code");
		//l_oAlarm.m_oBody.m_oAlarm.m_strReceiptName = l_pResult->GetValue("receipt_name");
		////l_oAlarm.m_oBody.m_oAlarm.m_strDispatchSuggestion = l_pResult->GetValue("dispatch_suggestion");
		//l_oAlarm.m_oBody.m_oAlarm.m_strIsMerge = l_pResult->GetValue("is_merge");


		//l_oAlarm.m_oBody.m_oAlarm.m_strCreateUser = l_pResult->GetValue("create_user");
		//l_oAlarm.m_oBody.m_oAlarm.m_strCreateTime = l_pResult->GetValue("create_time");
		//l_oAlarm.m_oBody.m_oAlarm.m_strUpdateUser = l_pResult->GetValue("update_user");
		//l_oAlarm.m_oBody.m_oAlarm.m_strUpdateTime = l_pResult->GetValue("update_time");

		//l_oAlarm.m_oBody.m_oAlarm.m_strPrivacy = l_pResult->GetValue("is_privacy");
		//l_oAlarm.m_oBody.m_oAlarm.m_strRemark = l_pResult->GetValue("remark");

		//l_oAlarm.m_oBody.m_oAlarm.m_strDeleteFlag = l_pResult->GetValue("is_delete"); //是否被删除
		if (!l_oAlarm.m_oBody.m_oAlarm.ParseAlarmRecord(l_pResult))
		{
			ICC_LOG_ERROR(m_pLog, "Parse record failed.");
		}
	}

	return true;
}


bool ICC::CBusinessImpl::AddProcessInfo(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& l_oAddProcessInfo)
{
	//向数据库添加处警记录
	l_oAddProcessInfo.m_strID = l_oAddProcessInfo.m_strID.empty() ?_GenId(l_oAddProcessInfo.m_strDispatchDeptDistrictCode) : l_oAddProcessInfo.m_strID;//m_pDateTime->CreateSerial();
	l_oAddProcessInfo.m_strCreateUser = l_oAddProcessInfo.m_strDispatchName;

	if (l_oAddProcessInfo.m_strCreateTime.empty())
	{
		l_oAddProcessInfo.m_strCreateTime = m_pDateTime->CurrentDateTimeStr();
	}

	//派警时间为空，则取当前时间
	if (l_oAddProcessInfo.m_strTimeSubmit.empty())
	{
		l_oAddProcessInfo.m_strTimeSubmit = l_oAddProcessInfo.m_strCreateTime;
	}
	//TODO::记录警情流水_分配处警单
	//向数据库添加接警流水并发送通知
	PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
	l_oAlarmLogInfo.m_strAlarmID = l_oAddProcessInfo.m_strID;
	std::vector<std::string> l_vecParamList;


	//vcs过来的不写库
	if (l_oAddProcessInfo.m_strMsgSource != "vcs_relocated" && l_oAddProcessInfo.m_strMsgSource != "vcs")
	{
		//TODO::记录警情流水_分配处警单
	//向数据库添加接警流水并发送通知
		Data::CStaffInfo l_oStaffInfo;
		if (!_GetStaffInfo(l_oAddProcessInfo.m_strDispatchCode, l_oStaffInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
		}
		std::string l_strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_oAddProcessInfo.m_strDispatchName);
		l_vecParamList.push_back(l_strStaffName);
		l_vecParamList.push_back(l_oAddProcessInfo.m_strDispatchCode);
		l_vecParamList.push_back(l_oAddProcessInfo.m_strDispatchDeptName);
		l_vecParamList.push_back(l_oAddProcessInfo.m_strProcessDeptName);
		l_oAlarmLogInfo.m_strOperateContent = GenAlarmLogContent(l_vecParamList);
		l_oAlarmLogInfo.m_strCreateUser = l_oAddProcessInfo.m_strDispatchName;
		l_oAlarmLogInfo.m_strOperate = LOG_PROCESS_ASSIGNED;
		l_oAlarmLogInfo.m_strFromType = "staff";
		l_oAlarmLogInfo.m_strFromObject = l_oAddProcessInfo.m_strDispatchCode;
		l_oAlarmLogInfo.m_strFromObjectName = l_oAddProcessInfo.m_strDispatchName;
		l_oAlarmLogInfo.m_strFromObjectOrgName = l_oAddProcessInfo.m_strDispatchDeptName;
		l_oAlarmLogInfo.m_strFromObjectOrgCode = l_oAddProcessInfo.m_strDispatchDeptOrgCode;
		l_oAlarmLogInfo.m_strToType = "org";
		l_oAlarmLogInfo.m_strToObject = l_oAddProcessInfo.m_strProcessDeptCode;
		l_oAlarmLogInfo.m_strToObjectName = l_oAddProcessInfo.m_strProcessDeptName;
		l_oAlarmLogInfo.m_strToObjectOrgName = l_oAddProcessInfo.m_strProcessDeptName;
		l_oAlarmLogInfo.m_strToObjectOrgCode = l_oAddProcessInfo.m_strProcessDeptOrgCode;
		l_oAlarmLogInfo.m_strSourceName = "icc";
		l_oAlarmLogInfo.m_strReceivedTime = l_oAddProcessInfo.m_strReceivedTime;
		if (!AddAlarmLogInfo(l_oAlarmLogInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "Insert icc_t_alarm_log LOG_PROCESS_ASSIGNED failed!");
		}

	}
	
	if (!InsertDBProcessInfo(l_oAddProcessInfo))
	{
		//向数据库插入处警记录失败
		//回复失败消息
		return false;
	}

	//不写BS001002002流水 2022-9-13
	//VCS过来的不写警情日志
	/*
	if (l_oAddProcessInfo.m_strMsgSource != "vcs_relocated" && l_oAddProcessInfo.m_strMsgSource != "vcs")
	{
		//TODO::记录警情流水_完成处警单		
		//向数据库添加接警流水并发送通知			
		l_vecParamList.clear();
		l_vecParamList.push_back(l_oAddProcessInfo.m_strDispatchName);
		l_vecParamList.push_back(l_oAddProcessInfo.m_strDispatchCode);
		l_vecParamList.push_back(l_oAddProcessInfo.m_strDispatchDeptName);
		l_oAlarmLogInfo.m_strOperateContent = GenAlarmLogContent(l_vecParamList);
		l_oAlarmLogInfo.m_strCreateUser = l_oAddProcessInfo.m_strDispatchName;
		l_oAlarmLogInfo.m_strOperate = LOG_PROCESS_COMPLETED;
		l_oAlarmLogInfo.m_strFromType = "staff";
		l_oAlarmLogInfo.m_strFromObject = l_oAddProcessInfo.m_strDispatchCode;
		l_oAlarmLogInfo.m_strFromObjectName = l_oAddProcessInfo.m_strDispatchName;
		l_oAlarmLogInfo.m_strFromObjectOrgName = l_oAddProcessInfo.m_strDispatchDeptName;
		l_oAlarmLogInfo.m_strFromObjectOrgCode = l_oAddProcessInfo.m_strDispatchDeptOrgCode;
		l_oAlarmLogInfo.m_strToType = "org";
		l_oAlarmLogInfo.m_strToObject = l_oAddProcessInfo.m_strProcessDeptCode;
		l_oAlarmLogInfo.m_strToObjectName = l_oAddProcessInfo.m_strProcessDeptName;
		l_oAlarmLogInfo.m_strToObjectOrgName = l_oAddProcessInfo.m_strProcessDeptName;
		l_oAlarmLogInfo.m_strToObjectOrgCode = l_oAddProcessInfo.m_strProcessDeptOrgCode;
		l_oAlarmLogInfo.m_strSourceName = "icc";

		if (!AddAlarmLogInfo(l_oAlarmLogInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "Insert icc_t_alarm_log LOG_PROCESS_COMPLETED failed!");
		}
	}
	*/

	//发送同步数据
	SyncProcessInfo(l_oAddProcessInfo, ESyncType::ADD);
	return true;
}

bool CBusinessImpl::_InsertBackReason(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& l_oUpdateProcessInfo)
{
	DataBase::SQLRequest l_tSQLRequest;

	l_tSQLRequest.sql_id = "insert_icc_t_backalarm_reason";
	l_tSQLRequest.param["guid"] = m_pString->CreateGuid();
	l_tSQLRequest.param["process_id"] = l_oUpdateProcessInfo.m_strID;
	l_tSQLRequest.param["back_reason"] = l_oUpdateProcessInfo.m_strCancelReason;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLRequest,false);
	ICC_LOG_DEBUG(m_pLog, "icc Synthetical insert cancel reason sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "icc Synthetical insert cancel reason failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}
	return true;
}

bool ICC::CBusinessImpl::UpdateProcessInfo(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& l_oUpdateProcessInfo)
{
	// VCS过来的使用VCS派警单的状态
	std::string l_strState(l_oUpdateProcessInfo.m_strState);

	if ("vcs_relocated" != l_oUpdateProcessInfo.m_strMsgSource && "vcs" != l_oUpdateProcessInfo.m_strMsgSource)
	{
		// 如果对已签收或者已反馈的处警单进行取消调派操作，则返回false
	   // l_strState = RealProcessState(l_oUpdateProcessInfo.m_strID, l_oUpdateProcessInfo.m_strState, PROCESS_STATUS_TAKEN, ""); //"DIC020010";
		l_strState = _GetCurrentProcessState(l_oUpdateProcessInfo.m_strID, l_strState, "");
	}

	if (l_strState.empty())
	{
		ICC_LOG_ERROR(m_pLog, "UpdateProcessInfo failed,state isempty!");
		return false;
	}

	//TODO::记录警情流水_受理接警单	
	//向数据库添加接警流水并发送通知
	PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
	l_oAlarmLogInfo.m_strAlarmID = l_oUpdateProcessInfo.m_strAlarmID;
	std::vector<std::string> l_vecParamList;
	Data::CStaffInfo l_oStaffInfo;
	if (!_GetStaffInfo(l_oUpdateProcessInfo.m_strDispatchCode, l_oStaffInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
	}
	std::string l_strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_oUpdateProcessInfo.m_strDispatchName);
	l_vecParamList.push_back(l_strStaffName);
	l_vecParamList.push_back(l_oUpdateProcessInfo.m_strDispatchCode);
	l_vecParamList.push_back(l_oUpdateProcessInfo.m_strDispatchDeptName);
	l_vecParamList.push_back(l_oUpdateProcessInfo.m_strProcessDeptName);
	l_vecParamList.push_back(l_oUpdateProcessInfo.m_strCancelReason);
	l_oAlarmLogInfo.m_strOperateContent = GenAlarmLogContent(l_vecParamList);
	l_oAlarmLogInfo.m_strCreateUser = l_oUpdateProcessInfo.m_strDispatchCode;
	l_oAlarmLogInfo.m_strFromType = "staff";
	l_oAlarmLogInfo.m_strFromObject = l_oUpdateProcessInfo.m_strDispatchCode;
	l_oAlarmLogInfo.m_strFromObjectName = l_oUpdateProcessInfo.m_strDispatchName;
	l_oAlarmLogInfo.m_strFromObjectOrgName = l_oUpdateProcessInfo.m_strDispatchDeptName;
	l_oAlarmLogInfo.m_strFromObjectOrgCode = l_oUpdateProcessInfo.m_strDispatchDeptOrgCode;
	l_oAlarmLogInfo.m_strToType = "org";
	l_oAlarmLogInfo.m_strToObject = l_oUpdateProcessInfo.m_strProcessDeptCode;
	l_oAlarmLogInfo.m_strToObjectName = l_oUpdateProcessInfo.m_strProcessDeptName;
	l_oAlarmLogInfo.m_strToObjectOrgName = l_oUpdateProcessInfo.m_strProcessDeptName;
	l_oAlarmLogInfo.m_strToObjectOrgCode = l_oUpdateProcessInfo.m_strProcessDeptOrgCode;
	l_oAlarmLogInfo.m_strSourceName = "icc";
	l_oAlarmLogInfo.m_strReceivedTime = l_oUpdateProcessInfo.m_strReceivedTime;

	l_oUpdateProcessInfo.m_strUpdateUser = l_oUpdateProcessInfo.m_strDispatchCode;
	l_oUpdateProcessInfo.m_strUpdateTime = m_pDateTime->CurrentDateTimeStr();

	//如果state值为DIC020071，则修改警情状态为已取消
	if ((l_oUpdateProcessInfo.m_strState).compare("DIC020071") == 0)
	{
		if (!_InsertBackReason(l_oUpdateProcessInfo))
		{
			return false;
		}

		if (l_oUpdateProcessInfo.m_strMsgSource != "vcs_relocated" && l_oUpdateProcessInfo.m_strMsgSource != "vcs")
		{
			//TODO::记录警情流水_取消调派
			l_oAlarmLogInfo.m_strOperate = LOG_DISPATCH_CANCELLED;
			//向数据库添加接警流水并发送通知
			if (!AddAlarmLogInfo(l_oAlarmLogInfo))
			{
				ICC_LOG_ERROR(m_pLog, "Insert icc_t_alarm_log LOG_DISPATCH_CANCELLED or LOG_PROCESS_ASSIGNED failed!");
			}
	   }
	}

	//更新处警表前迁移数据到处警流水表
	DataBase::SQLRequest l_oSQLReq;
	std::string l_strResourceID = m_pString->CreateGuid();
	if (AddProcessBookByUpProcess(l_oUpdateProcessInfo.m_strID, l_oSQLReq, l_strResourceID))
	{
		std::string l_strSaveState = _GetCurrentProcessState(l_oUpdateProcessInfo.m_strID, "", "");
		//更新处警单
		if (!UpdateDBProcessInfo(l_oUpdateProcessInfo))
		{
			//向数据库插入接警记录失败
			//回复失败消息
			return false;
		}
		/*
		else
		{
			//备份处警信息、备份更新后处警信息
			if (ExecSql(l_oSQLReq) && AddUpdateProcessAlarmBookByResID(l_oUpdateProcessInfo, l_strResourceID))
			{
				ICC_LOG_DEBUG(m_pLog, "Insert Process Book Success");
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "Insert Process Book Failed!");
			}
		}*/

		if (l_oUpdateProcessInfo.m_strMsgSource != "vcs_relocated" && l_oUpdateProcessInfo.m_strMsgSource != "vcs")
		{
			//TODO::记录警情流水完成编辑警单		
			//向数据库添加接警流水并发送通知
			if ((l_oUpdateProcessInfo.m_strState).compare("DIC020071") != 0)
			{
				
				l_vecParamList.clear();
				//取消后再调派走警单分配流水
				if (l_strSaveState == "17" && l_oUpdateProcessInfo.m_strState == "01")
				{
					// 记录警情流水_分配处警单
					l_vecParamList.clear();
					Data::CStaffInfo l_oStaffInfo;
					if (!_GetStaffInfo(l_oUpdateProcessInfo.m_strDispatchCode, l_oStaffInfo))
					{
						ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
					}
					std::string l_strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_oUpdateProcessInfo.m_strDispatchName);
					l_vecParamList.push_back(l_strStaffName);
					l_vecParamList.push_back(l_oUpdateProcessInfo.m_strDispatchCode);
					l_vecParamList.push_back(l_oUpdateProcessInfo.m_strDispatchDeptName);
					l_vecParamList.push_back(l_oUpdateProcessInfo.m_strProcessDeptName);
					l_oAlarmLogInfo.m_strOperate = LOG_PROCESS_ASSIGNED;
					l_oAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
					l_oAlarmLogInfo.m_strFromType = "staff";
					l_oAlarmLogInfo.m_strFromObject = l_oUpdateProcessInfo.m_strDispatchCode;
					l_oAlarmLogInfo.m_strFromObjectName = l_oUpdateProcessInfo.m_strDispatchName;
					l_oAlarmLogInfo.m_strFromObjectOrgName = l_oUpdateProcessInfo.m_strDispatchDeptName;
					l_oAlarmLogInfo.m_strFromObjectOrgCode = l_oUpdateProcessInfo.m_strDispatchDeptOrgCode;
					l_oAlarmLogInfo.m_strToType = "org";
					l_oAlarmLogInfo.m_strToObject = l_oUpdateProcessInfo.m_strProcessDeptCode;
					l_oAlarmLogInfo.m_strToObjectName = l_oUpdateProcessInfo.m_strProcessDeptName;
					l_oAlarmLogInfo.m_strToObjectOrgName = l_oUpdateProcessInfo.m_strProcessDeptName;
					l_oAlarmLogInfo.m_strToObjectOrgCode = l_oUpdateProcessInfo.m_strProcessDeptOrgCode;
					l_oAlarmLogInfo.m_strSourceName = "icc";
		
					bool l_bRes = AddAlarmLogInfo(l_oAlarmLogInfo);

					ICC_LOG_DEBUG(m_pLog, "Insert icc_t_alarm_log LOG_PROCESS_ASSIGNED res:%d", l_bRes);

					// 插入成功，走完成处警流水
					l_vecParamList.pop_back();
					//不写BS001002002流水 2022-9-13
					//l_oAlarmLogInfo.m_strOperate = LOG_PROCESS_COMPLETED;
					//AddAlarmLogInfo(l_oAlarmLogInfo);
				}
				else
				{
					Data::CStaffInfo l_oStaffInfo;
					if (!_GetStaffInfo(l_oUpdateProcessInfo.m_strDispatchCode, l_oStaffInfo))
					{
						ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
					}
					std::string l_strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_oUpdateProcessInfo.m_strDispatchName);
					l_vecParamList.push_back(l_strStaffName);
					l_vecParamList.push_back(l_oUpdateProcessInfo.m_strDispatchCode);
					l_vecParamList.push_back(l_oUpdateProcessInfo.m_strDispatchDeptName);

					l_oAlarmLogInfo.m_strOperateContent = GenAlarmLogContent(l_vecParamList);
					l_oAlarmLogInfo.m_strCreateUser = l_oUpdateProcessInfo.m_strDispatchName;
					std::string type = RESOURCETYPE_ALARM;
					l_oAlarmLogInfo.m_strOperateAttachDesc = BuildAlarmLogAttach(type, l_strResourceID);
					l_oAlarmLogInfo.m_strOperate = LOG_ALARM_EDIT;
					l_oAlarmLogInfo.m_strFromType = "staff";
					l_oAlarmLogInfo.m_strFromObject = l_oUpdateProcessInfo.m_strDispatchCode;
					l_oAlarmLogInfo.m_strFromObjectName = l_oUpdateProcessInfo.m_strDispatchName;
					l_oAlarmLogInfo.m_strFromObjectOrgName = l_oUpdateProcessInfo.m_strDispatchDeptName;
					l_oAlarmLogInfo.m_strFromObjectOrgCode = l_oUpdateProcessInfo.m_strDispatchDeptOrgCode;
					l_oAlarmLogInfo.m_strToType = "org";
					l_oAlarmLogInfo.m_strToObject = l_oUpdateProcessInfo.m_strProcessDeptCode;
					l_oAlarmLogInfo.m_strToObjectName = l_oUpdateProcessInfo.m_strProcessDeptName;
					l_oAlarmLogInfo.m_strToObjectOrgName = l_oUpdateProcessInfo.m_strProcessDeptName;
					l_oAlarmLogInfo.m_strToObjectOrgCode = l_oUpdateProcessInfo.m_strProcessDeptOrgCode;
					l_oAlarmLogInfo.m_strSourceName = "icc";
					if (!AddAlarmLogInfo(l_oAlarmLogInfo))
					{
						ICC_LOG_DEBUG(m_pLog, "Insert icc_t_alarm_log LOG_ALARM_EDIT failed!");
					}
				}
			}
		}

		SyncProcessInfo(l_oUpdateProcessInfo, 2);
	}
	return true;
}

bool ICC::CBusinessImpl::InsertDBProcessInfo(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo)
{
	// VCS过来的使用VCS派警单的状态
	if ("vcs_relocated" != p_pProcessInfo.m_strMsgSource && "vcs" != p_pProcessInfo.m_strMsgSource)
	{
		//p_pProcessInfo.m_strState = RealProcessState(p_pProcessInfo.m_strID, p_pProcessInfo.m_strState, PROCESS_STATUS_TAKEN, ""); //"DIC020010";
		p_pProcessInfo.m_strState = _GetCurrentProcessState(p_pProcessInfo.m_strID, p_pProcessInfo.m_strState, "");
	}

	DataBase::SQLRequest l_tSQLReqInsertProcess;
	if (SetSqlInsertProcess(p_pProcessInfo, p_pProcessInfo.m_strState, l_tSQLReqInsertProcess))
	{
		l_tSQLReqInsertProcess.param["create_user"] = p_pProcessInfo.m_strCreateUser;
		l_tSQLReqInsertProcess.param["create_time"] = p_pProcessInfo.m_strCreateTime;
		if (p_pProcessInfo.m_strUpdateTime.empty())
		{
			l_tSQLReqInsertProcess.param["update_time"] = p_pProcessInfo.m_strCreateTime;
		}
		else
		{
			l_tSQLReqInsertProcess.param["update_time"] = p_pProcessInfo.m_strUpdateTime;
		}

		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReqInsertProcess);
		ICC_LOG_DEBUG(m_pLog, "sql: %s", l_pRSet->GetSQL().c_str());
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "insert alarm process info failed,error msg:[%s],sql:[%s]", l_pRSet->GetErrorMsg().c_str(), l_pRSet->GetSQL().c_str());
			return false;
		}
		return true;
	}
	return false;
}


bool CBusinessImpl::AddProcessBookByUpProcess(std::string p_strProcessId, DataBase::SQLRequest &p_oSQLReq, std::string p_strResourceID)
{
	//不需要写book表
	return true;
}

std::string CBusinessImpl::GenAlarmLogContent(std::vector<std::string> p_vecParamList)
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

bool ICC::CBusinessImpl::IsProcessInfoValid(const PROTOCOL::CAddOrUpdateProcessRequest::CBody& p_pProcessInfo)
{
	for (auto it = p_pProcessInfo.m_vecData.cbegin(); it < p_pProcessInfo.m_vecData.cend(); it++)
	{
		if (it->m_strAlarmID.empty())
		{
			return false;
		}
	}
	return true;
}


void ICC::CBusinessImpl::SyncProcessInfo(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_rProcessToSync, int p_iSyncType)
{
	PROTOCOL::CAlarmProcessSync l_oAlarmProcessSync;
	l_oAlarmProcessSync.m_oHeader.m_strSystemID = SYSTEMID;
	l_oAlarmProcessSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
	l_oAlarmProcessSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_oAlarmProcessSync.m_oHeader.m_strCmd = "alarm_process_sync";
	l_oAlarmProcessSync.m_oHeader.m_strRequest = "topic_alarm_sync";
	l_oAlarmProcessSync.m_oHeader.m_strRequestType = "1";//主题
	l_oAlarmProcessSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_oAlarmProcessSync.m_oHeader.m_strCMSProperty = MSG_SOURCE;

	
	l_oAlarmProcessSync.m_oBody.m_ProcessData = p_rProcessToSync;
	l_oAlarmProcessSync.m_oBody.m_strSyncType = std::to_string(p_iSyncType);// 1:add 2:update

	//同步消息
	std::string l_strMsg(l_oAlarmProcessSync.ToString(m_pJsonFty->CreateJson()));
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "[AlarmProcess]sync alarm process msg[\n%s\n]", l_strMsg.c_str());
}

bool CBusinessImpl::ExecSql(DataBase::SQLRequest p_oSQLReq)
{
	DataBase::IResultSetPtr l_pRSet;
	l_pRSet = m_pDBConn->Exec(p_oSQLReq);

	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "Process Error:[%s], sql:[%s]", l_pRSet->GetErrorMsg().c_str(), l_pRSet->GetSQL().c_str());
		return false;
	}

	return true;
}

bool CBusinessImpl::AddUpdateProcessAlarmBookByResID(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData &p_pProcessAlarmInfo, std::string p_strResourceID)
{
	return true;
	//将查询到的警单备份到book表中
}

bool CBusinessImpl::SetSqlUpdateProcess(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo, DataBase::SQLRequest &p_tSQLRequest)
{
	p_tSQLRequest.sql_id = "update_icc_t_pjdb";
	p_tSQLRequest.param["id"] = p_pProcessInfo.m_strID;
	std::string strTime = m_pDateTime->GetDispatchIdTime(p_pProcessInfo.m_strID);
	if (strTime != "")
	{
		p_tSQLRequest.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 86400 * 15);
		p_tSQLRequest.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	p_tSQLRequest.set["dispatch_dept_district_code"] = p_pProcessInfo.m_strDispatchDeptDistrictCode;
	p_tSQLRequest.set["alarm_id"] = p_pProcessInfo.m_strAlarmID;
	p_tSQLRequest.set["dispatch_dept_code"] = p_pProcessInfo.m_strDispatchDeptCode;
	p_tSQLRequest.set["dispatch_code"] = p_pProcessInfo.m_strDispatchCode;
	p_tSQLRequest.set["dispatch_name"] = p_pProcessInfo.m_strDispatchName;
	p_tSQLRequest.set["record_id"] = p_pProcessInfo.m_strRecordID;
	p_tSQLRequest.set["dispatch_suggestion"] = p_pProcessInfo.m_strDispatchSuggestion;
	p_tSQLRequest.set["process_dept_code"] = p_pProcessInfo.m_strProcessDeptCode;
	p_tSQLRequest.set["time_submit"] = p_pProcessInfo.m_strTimeSubmit;
	p_tSQLRequest.set["time_arrived"] = p_pProcessInfo.m_strTimeArrived;
	p_tSQLRequest.set["time_signed"] = p_pProcessInfo.m_strTimeSigned;
	p_tSQLRequest.set["process_name"] = p_pProcessInfo.m_strProcessName;
	p_tSQLRequest.set["process_code"] = p_pProcessInfo.m_strProcessCode;
	p_tSQLRequest.set["dispatch_personnel"] = p_pProcessInfo.m_strDispatchPersonnel;
	p_tSQLRequest.set["dispatch_vehicles"] = p_pProcessInfo.m_strDispatchVehicles;
	p_tSQLRequest.set["dispatch_boats"] = p_pProcessInfo.m_strDispatchBoats;
	p_tSQLRequest.set["state"] = p_pProcessInfo.m_strState;
	p_tSQLRequest.set["create_time"] = p_pProcessInfo.m_strCreateTime;
	p_tSQLRequest.set["update_time"] = p_pProcessInfo.m_strUpdateTime;
	p_tSQLRequest.set["dispatch_dept_name"] = p_pProcessInfo.m_strDispatchDeptName;
	p_tSQLRequest.set["dispatch_dept_org_code"] = p_pProcessInfo.m_strDispatchDeptOrgCode;
	p_tSQLRequest.set["process_dept_name"] = p_pProcessInfo.m_strProcessDeptName;
	p_tSQLRequest.set["process_dept_org_code"] = p_pProcessInfo.m_strProcessDeptOrgCode;
	p_tSQLRequest.set["process_object_type"] = p_pProcessInfo.m_strProcessObjectType;
	p_tSQLRequest.set["process_object_name"] = p_pProcessInfo.m_strProcessObjectName;
	p_tSQLRequest.set["process_object_code"] = p_pProcessInfo.m_strProcessObjectCode;
	p_tSQLRequest.set["business_status"] = p_pProcessInfo.m_strBusinessStatus;
	p_tSQLRequest.set["seat_code"] = p_pProcessInfo.m_strSeatCode;
	p_tSQLRequest.set["cancel_time"] = p_pProcessInfo.m_strCancelTime;
	p_tSQLRequest.set["cancel_reason"] = p_pProcessInfo.m_strCancelReason;
	if (!p_pProcessInfo.m_strIsAutoAssignJob.empty())
	{
		p_tSQLRequest.set["is_auto_assign_job"] = p_pProcessInfo.m_strIsAutoAssignJob;
	}
	else
	{
		p_tSQLRequest.set["is_auto_assign_job"] = "0";
	}
	p_tSQLRequest.set["create_user"] = p_pProcessInfo.m_strCreateUser;
	p_tSQLRequest.set["update_user"] = p_pProcessInfo.m_strUpdateUser;
	p_tSQLRequest.set["overtime_state"] = p_pProcessInfo.m_strOvertimeState;

	if (!p_pProcessInfo.m_strProcessObjectID.empty())
	{
		p_tSQLRequest.set["process_object_id"] = p_pProcessInfo.m_strProcessObjectID;
	}
	if (!p_pProcessInfo.m_strTransfDeptOrjCode.empty())
	{
		p_tSQLRequest.set["transfers_dept_org_code"] = p_pProcessInfo.m_strTransfDeptOrjCode;
	}
	if (!p_pProcessInfo.m_strIsOver.empty())
	{
		p_tSQLRequest.set["is_over"] = p_pProcessInfo.m_strIsOver;
	}
	if (!p_pProcessInfo.m_strOverRemark.empty())
	{
		p_tSQLRequest.set["over_remark"] = p_pProcessInfo.m_strOverRemark;
	}
	if (!p_pProcessInfo.m_strParentID.empty())
	{
		p_tSQLRequest.set["parent_id"] = p_pProcessInfo.m_strParentID;
	}
	if (!p_pProcessInfo.m_strGZLDM.empty())
	{
		p_tSQLRequest.set["flow_code"] = p_pProcessInfo.m_strGZLDM;
	}

	if (!p_pProcessInfo.m_strCentreProcessDeptCode.empty())
	{
		p_tSQLRequest.set["centre_process_dept_code"] = p_pProcessInfo.m_strCentreProcessDeptCode;
	}

	if (!p_pProcessInfo.m_strCentreAlarmDeptCode.empty())
	{
		p_tSQLRequest.set["centre_alarm_dept_code"] = p_pProcessInfo.m_strCentreAlarmDeptCode;
	}

	if (!p_pProcessInfo.m_strDispatchDeptDistrictName.empty())
	{
		p_tSQLRequest.set["dispatch_dept_district_name"] = p_pProcessInfo.m_strDispatchDeptDistrictName;
	}

	if (!p_pProcessInfo.m_strLinkedDispatchCode.empty())
	{
		p_tSQLRequest.set["linked_dispatch_code"] = p_pProcessInfo.m_strLinkedDispatchCode;
	}

	if (!p_pProcessInfo.m_strOverTime.empty())
	{
		p_tSQLRequest.set["over_time"] = p_pProcessInfo.m_strOverTime;
	}

	if (!p_pProcessInfo.m_strFinishedTimeoutState.empty())
	{
		p_tSQLRequest.set["finished_timeout_state"] = p_pProcessInfo.m_strFinishedTimeoutState;
	}

	if (!p_pProcessInfo.m_strPoliceType.empty())
	{
		p_tSQLRequest.set["police_type"] = p_pProcessInfo.m_strPoliceType;
	}

	if (!p_pProcessInfo.m_strProcessDeptShortName.empty())
	{
		p_tSQLRequest.set["process_dept_short_name"] = p_pProcessInfo.m_strProcessDeptShortName;
	}

	if (!p_pProcessInfo.m_strDispatchDeptShortName.empty())
	{
		p_tSQLRequest.set["dispatch_dept_short_name"] = p_pProcessInfo.m_strDispatchDeptShortName;
	}

	if (!p_pProcessInfo.m_strCreateTeminal.empty())
	{
		p_tSQLRequest.set["createTeminal"] = p_pProcessInfo.m_strCreateTeminal;
	}
	if (!p_pProcessInfo.m_strUpdateTeminal.empty())
	{
		p_tSQLRequest.set["updateTeminal"] = p_pProcessInfo.m_strUpdateTeminal;
	}
	return true;
}

bool ICC::CBusinessImpl::UpdateDBProcessInfo(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo)
{
	// VCS过来的使用VCS派警单的状态
	if ("vcs_relocated" != p_pProcessInfo.m_strMsgSource && "vcs" != p_pProcessInfo.m_strMsgSource)
	{
		p_pProcessInfo.m_strState = RealProcessState(p_pProcessInfo.m_strID, p_pProcessInfo.m_strState, "DIC020010", ""); //"DIC020010";
	}
	DataBase::SQLRequest l_tSQLRequest;
	if (SetSqlUpdateProcess(p_pProcessInfo, l_tSQLRequest))
	{
		DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLRequest);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "update alarm process info failed,error msg:[%s],sql:[%s]", l_pRSet->GetErrorMsg().c_str(), l_pRSet->GetSQL().c_str());
			return false;
		}

		ICC_LOG_DEBUG(m_pLog, "update alarm process info success,sql:[%s]",l_pRSet->GetSQL().c_str());

		return true;
	}
	return false;
}

void CBusinessImpl::OnCNotifiUpdateCalleventFlagRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	
	// 解析请求消息
	PROTOCOL::CUpdateCalleventFlagRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	// 构造回复
	PROTOCOL::CUpdateCalleventFlagRespond l_oRespond;
	BuildRespondHeader("update_callevent_flag_respond", l_oRequest.m_oHeader, l_oRespond.m_oHeader);

	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "update_icc_t_callevent_flag";

	l_tSQLReq.param["callref_id"] = l_oRequest.m_oBody.m_strCallEventId;
	l_tSQLReq.param["flag"] = l_oRequest.m_oBody.m_strFlag;

	std::string strTime = m_pDateTime->GetCallRefIdTime(l_oRequest.m_oBody.m_strCallEventId);
	if (strTime != "")
	{
		l_tSQLReq.param["create_time_begin"] = m_pDateTime->GetFrontTime(strTime);
		l_tSQLReq.param["create_time_end"] = m_pDateTime->GetAfterTime(strTime);
	}

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update callevent flag failed! sql:[%s]", l_pRSet->GetSQL().c_str());
		l_oRespond.m_oBody.m_strResult = "1";
	}
	else
	{
		l_oRespond.m_oBody.m_strResult = "0";
		ICC_LOG_ERROR(m_pLog, "update callevent flag success! callrefid:[%s]", l_oRequest.m_oBody.m_strCallEventId.c_str());

	}
	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());	
}

bool CBusinessImpl::_GetAllKeyWordAlarmRecords(const PROTOCOL::CKeyWordAlarmQryRequest& in_oReq, PROTOCOL::CKeyWordAlarmQryRespond& out_oRes)
{
	DataBase::SQLRequest l_tSQLReq;

	//先查条目
	l_tSQLReq.sql_id = "select_icc_t_keyword_alarm_count";

	if (!in_oReq.m_oBody.m_strDeptCode.empty())
	{
		l_tSQLReq.param["dept_code"] = in_oReq.m_oBody.m_strDeptCode;
		
	}

	if (!in_oReq.m_oBody.m_strBeginTime.empty() && !in_oReq.m_oBody.m_strEndTime.empty())
	{
		l_tSQLReq.param["begin_time"] = in_oReq.m_oBody.m_strBeginTime;
		l_tSQLReq.param["end_time"] = in_oReq.m_oBody.m_strEndTime;
	}
	
	DataBase::IResultSetPtr l_resultCount = m_pDBConn->Exec(l_tSQLReq);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_resultCount->GetSQL().c_str());
	if (!l_resultCount->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_resultCount->GetErrorMsg().c_str());

		out_oRes.m_oHeader.m_strMsg = "execute " + l_tSQLReq.sql_id + " failed";
		return false;
	}

	std::string tmp_strAllCount;

	if (l_resultCount->Next())
	{
		tmp_strAllCount = l_resultCount->GetValue("num");

		ICC_LOG_DEBUG(m_pLog, "Get all keyword alarm size[%s]", tmp_strAllCount.c_str());
	}

	//表示没有记录
	if (tmp_strAllCount.empty() || "0" == tmp_strAllCount)
	{
		ICC_LOG_DEBUG(m_pLog, "no record");
		return true;
	}

	int l_nPageSize = m_pString->ToInt(in_oReq.m_oBody.m_strPageSize.c_str());
	int l_nPageIndex = m_pString->ToInt(in_oReq.m_oBody.m_strPageIndex.c_str());

	if (l_nPageSize < 0 || l_nPageSize > MAX_COUNT_EX) l_nPageSize = MAX_COUNT_EX;
	if (l_nPageIndex < 1) l_nPageIndex = 1;
	l_nPageIndex = (l_nPageIndex - 1) * l_nPageSize;


	l_tSQLReq.sql_id = "select_icc_t_keyword_alarm_bypage";
	l_tSQLReq.param["page_size"] = std::to_string(l_nPageSize);
	l_tSQLReq.param["begin_index"] = std::to_string(l_nPageIndex);

	DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_tSQLReq);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());

	if (!l_result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "ExecQuery Error ,Error Message :[%s]", l_result->GetErrorMsg().c_str());
		out_oRes.m_oHeader.m_strMsg = "execute " + l_tSQLReq.sql_id + " failed";
		return false;
	}

	out_oRes.m_oBody.m_strAllCount = tmp_strAllCount;

	while (l_result->Next())
	{
		PROTOCOL::CKeyWordAlarm tmp_oKeyWordAlarm;
		tmp_oKeyWordAlarm.m_strGuid = l_result->GetValue("guid");
		tmp_oKeyWordAlarm.m_strAlarmID = l_result->GetValue("alarm_id");
		tmp_oKeyWordAlarm.m_strKeyWord = l_result->GetValue("key_word");
		tmp_oKeyWordAlarm.m_strKeyContent = l_result->GetValue("key_word_content");
		tmp_oKeyWordAlarm.m_strAlarmContent = l_result->GetValue("content");
		tmp_oKeyWordAlarm.m_strDeptCode = l_result->GetValue("dept_code");

		out_oRes.m_oBody.m_vecKeyWordAlarm.push_back(tmp_oKeyWordAlarm);
	}

	ICC_LOG_DEBUG(m_pLog, "Get success, keyword alarm size[%d]", out_oRes.m_oBody.m_vecKeyWordAlarm.size());

	out_oRes.m_oBody.m_strCount = std::to_string(out_oRes.m_oBody.m_vecKeyWordAlarm.size());
	return true;

}

void CBusinessImpl::OnCNotifuGetBlackMessageSendRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	JsonParser::IJsonPtr p_pJson = m_pJsonFty->CreateJson();
	if (!p_pJson)
	{
		ICC_LOG_ERROR(m_pLog, "Cur Black Message Create Json Error!!!!");
		return;
	}

	p_pJson->LoadJson(p_pNotify->GetMessages());
	std::string p_strBlackNumber = p_pJson->GetNodeValue("/body/urge_called", "");
	std::string p_strCalledNumber = p_pJson->GetNodeValue("/body/remarks", "");

	if (p_strBlackNumber.empty() || p_strCalledNumber.empty())
	{
		ICC_LOG_ERROR(m_pLog, "Cur Black Message Number Is Null, Message:[%s]!!!!", p_pNotify->GetMessages().c_str());
		return;
	}

	//发送消息中心
	std::string l_strConfigMsg;
	m_pMsgCenter->BuildManualAcceptConfig("BlackMessageSend", l_strConfigMsg);
	l_strConfigMsg = m_pString->ReplaceFirst(l_strConfigMsg, "$", p_strBlackNumber);
	l_strConfigMsg = m_pString->ReplaceFirst(l_strConfigMsg, "$", p_strCalledNumber);

	std::string l_strDeptInfo;
	std::string l_strDeptCode;
	if (!m_pRedisClient->HGet("acd_dept", p_strCalledNumber, l_strDeptInfo))
	{
		ICC_LOG_ERROR(m_pLog, "Cur Black Message Get Redis Info Error!!!!");
		return;
	}

	p_pJson->LoadJson(l_strDeptInfo);
	l_strDeptCode = p_pJson->GetNodeValue("/dept_code", "");
	if (l_strDeptCode.empty())
	{
		ICC_LOG_ERROR(m_pLog, "Cur l_strDeptCode Is Null!!!!");
		return;
	}

	m_pMsgCenter->Send(l_strConfigMsg, l_strDeptCode, true, m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName, "black_incoming");
}

void CBusinessImpl::OnCNotifiGetAllKeyWordAlarmRequest(ObserverPattern::INotificationPtr p_pNotify)
{

	if (NULL == p_pNotify.get())
	{
		ICC_LOG_ERROR(m_pLog, "receive message is NULL");
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	// 解析请求消息
	PROTOCOL::CKeyWordAlarmQryRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	PROTOCOL::CKeyWordAlarmQryRespond l_oRespond;
	l_oRespond.m_oHeader = l_oRequest.m_oHeader;
	l_oRespond.m_oBody.m_strAllCount = "0";
	l_oRespond.m_oBody.m_strCount = "0";

	ICC_LOG_DEBUG(m_pLog, "page_size=%s,page_index=%s", l_oRequest.m_oBody.m_strPageSize.c_str(), l_oRequest.m_oBody.m_strPageIndex.c_str());

	if (!_GetAllKeyWordAlarmRecords(l_oRequest, l_oRespond))
	{
		l_oRespond.m_oHeader.m_strResult = "2";
	}

	std::string l_strMessage = l_oRespond.ToString(ICCGetIJsonFactory()->CreateJson());
	
	p_pNotify->Response(l_strMessage);
	ICC_LOG_DEBUG(m_pLog, "Send apply Info Msg [%s]", l_strMessage.c_str());
}

std::string CBusinessImpl::_GenAlarmID(const std::string& strPre, const std::string& strSeatNo, const std::string& strReceiptDeptCode)
{
	if ("1" == m_strAlarmIdGenRule)//默认规则
	{
		return m_pDateTime->CreateSerial();
	}
	else if ("2" == m_strAlarmIdGenRule)//前面加上部门行政区域代码 巴西使用
	{
		std::string strTmp = m_pDateTime->CreateSerial();
		return strPre + strTmp.substr(2, strTmp.length() - 2);
	}
	else if ("3" == m_strAlarmIdGenRule)  //资阳使用此警情ID生成方式
	{
		std::string strTmp(m_pDateTime->CreateSerial(m_iZoneSecond));
		
		std::string tmp_strXZQH(strPre);

		if (tmp_strXZQH.empty())
		{
			tmp_strXZQH = "000000"; //如果为空，加6个0
		}

		const int YYYYMMDD_LEN = 8;
		//std::string tmp_strCurData = strTmp.substr(0, YYYYMMDD_LEN);
	
		int tmp_uiSerial = 0;
		{
			SAFE_LOCK(m_lkSerial);
			std::string tmp_strSerialNum;

			m_pRedisClient->HGet(REDIS_Serial_Data, strTmp, tmp_strSerialNum);

			//如果没有则删除
			if (tmp_strSerialNum.empty())
			{
				m_pRedisClient->Del(REDIS_Serial_Data);
			}
			else
			{
				tmp_uiSerial = m_pString->ToInt(tmp_strSerialNum);
			}
			
			++tmp_uiSerial;
			std::string tmp_strRedisSerial(std::to_string(tmp_uiSerial));
			m_pRedisClient->HSet(REDIS_Serial_Data, strTmp, tmp_strRedisSerial);
		}
		std::string tmp_strSerialNum(m_pString->Format("%06d", tmp_uiSerial));

		return tmp_strXZQH + strTmp + tmp_strSerialNum;
	}
	else if ("4" == m_strAlarmIdGenRule)
	{
		
		std::string strTmp = m_pDateTime->ToString(m_pDateTime->AddHours(m_pDateTime->CurrentDateTime(), 8), DEFAULT_DATETIME_STRING_FORMAT);
		std::string tmp_strXZQH(strPre);
		std::string tmp_strSerialNum;
		if (tmp_strXZQH.empty())
		{
			tmp_strXZQH = "000000"; //如果为空，加6个0
		}

		if (!strSeatNo.empty() && strSeatNo.size() > 3)
		{
			tmp_strSerialNum = strSeatNo.substr(strSeatNo.size() - 3);
		}
		else if (!strReceiptDeptCode.empty())
		{
			if (strReceiptDeptCode.size() > 8)
			{
				tmp_strSerialNum = "0" + strReceiptDeptCode.substr(6, 2);
			}
		}

		if (tmp_strSerialNum.empty())
		{
			tmp_strSerialNum = "000";
		}

		return tmp_strXZQH + strTmp.substr(0, 17) + tmp_strSerialNum;
	}
	else
	{
		return m_pDateTime->CreateSerial();
	}
}

std::string CBusinessImpl::_GenId(const std::string& strPre /* = "" */)
{
	if ("1" == m_strIdGenRule)//默认规则
	{
		return m_pDateTime->CreateSerial();
	}
	else if ("2" == m_strIdGenRule)//前面加上部门行政区域代码 巴西使用
	{
		std::string strTmp = m_pDateTime->CreateSerial();
		return strPre + strTmp.substr(2, strTmp.length() - 2);				
	}
	else if ("3" == m_strIdGenRule)  //资阳使用此派警ID生成方式
	{
		std::string strTmp(m_pDateTime->CreateSerial());

		std::string tmp_strXZQH(strPre);

		if (tmp_strXZQH.empty())
		{
			tmp_strXZQH = "000000"; //如果为空，加6个0
		}
		//return tmp_strXZQH + strTmp.substr(0, strTmp.length() - 2);
		return tmp_strXZQH + strTmp;
	}
	else
	{
		return m_pDateTime->CreateSerial();
	}
}


void CBusinessImpl::OnAlarmRelatedCarsAddExRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CAlarmRelatedCarsAddExRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	std::string l_strJJDBH = l_oRequest.m_oBody.m_alarmRelatedCarsInfo.m_mapInfo[PROTOCOL::Ex_Cars_JJDBH];

	std::string l_strTmpID = l_oRequest.m_oBody.m_alarmRelatedCarsInfo.m_mapInfo[PROTOCOL::Ex_Cars_JQSACLBH];
	if (l_strTmpID.empty())
	{
		l_oRequest.m_oBody.m_alarmRelatedCarsInfo.m_mapInfo[PROTOCOL::Ex_Cars_JQSACLBH] = m_pString->CreateGuid();
	}
	
	if (l_oRequest.m_oBody.m_alarmRelatedCarsInfo.m_mapInfo[PROTOCOL::Ex_Cars_create_time].empty())
	{
		l_oRequest.m_oBody.m_alarmRelatedCarsInfo.m_mapInfo[PROTOCOL::Ex_Cars_create_time] = m_pDateTime->CurrentDateTimeStr();
	}


	if (l_oRequest.m_oBody.m_alarmRelatedCarsInfo.m_mapInfo[PROTOCOL::Ex_Cars_update_time].empty())
	{
		l_oRequest.m_oBody.m_alarmRelatedCarsInfo.m_mapInfo[PROTOCOL::Ex_Cars_update_time] = m_pDateTime->CurrentDateTimeStr();
	}


	// 构造回复
	PROTOCOL::CAlarmRelatedCarsChangeRespond l_oRespond;
	l_oRespond.m_oHeader.m_strMsgId = l_oRequest.m_oHeader.m_strMsgId;

	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "insert_icc_t_jqclb";

	std::map<std::string, std::string>& mapTmp = l_oRequest.m_oBody.m_alarmRelatedCarsInfo.m_mapInfo;
	std::map<std::string, std::string>::const_iterator itr_const;
	for (itr_const = mapTmp.begin(); itr_const != mapTmp.end(); ++itr_const)
	{
		l_tSQLReq.param[itr_const->first] = itr_const->second;
	}

	l_tSQLReq.param["update_time"] = m_pDateTime->CurrentDateTimeStr();
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq);
	bool bRes = false;
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert_icc_t_jqclb failed! sql:[%s]", l_pRSet->GetSQL().c_str());
		l_oRespond.m_oBody.m_strResult = "1";
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "execute " + l_tSQLReq.sql_id + " failed";
	}
	else
	{
		l_oRespond.m_oBody.m_strResult = "0";
		l_oRespond.m_oHeader.m_strResult = "0";
		ICC_LOG_DEBUG(m_pLog, "insert_icc_t_jqclb success! ");
		bRes = true;
	}
	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	//涉案车辆有新增时，更新接警单表的更新时间字段
	if (!l_strJJDBH.empty())
	{
		DataBase::SQLRequest l_oSQLReq;
		l_oSQLReq.sql_id = "update_icc_t_jjdb";
		l_oSQLReq.param["id"] = l_strJJDBH;
		l_oSQLReq.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
		l_pRSet = m_pDBConn->Exec(l_oSQLReq);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "update_icc_t_jjdb failed! sql:[%s]", l_pRSet->GetSQL().c_str());
		}
	}
	

	if (bRes)
	{
		
		//发同步消息
		SendAlarmCarsChangeSync(l_oRequest.m_oHeader,l_oRequest.m_oBody.m_alarmRelatedCarsInfo, "1");
	}
}

std::string CBusinessImpl::_QueryAlarmRelatedCarsCreateTime(const std::string& p_strID)
{
	if (p_strID.empty())
	{
		return "";
	}

	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "query_icc_t_jqclb_by_id";
	l_tSQLReq.param["id"] = p_strID;
	l_tSQLReq.param["deleted"] = "0";
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "query_icc_t_jqclb_by_id failed! sql:[%s]", l_pRSet->GetSQL().c_str());
		return "";
	}
	else
	{

		while (l_pRSet->Next())
		{
			std::string m_strCreateTime = l_pRSet->GetValue("create_time");
			ICC_LOG_DEBUG(m_pLog, "_QueryAlarmRelatedCars return createTime=%s,sql:[%s]", m_strCreateTime.c_str(), l_pRSet->GetSQL().c_str());
			return m_strCreateTime;
		}
	}
	return "";
}

void CBusinessImpl::OnAlarmRelatedCarsDeleteExRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	// 解析请求消息
	PROTOCOL::CAlarmRelatedCarsDeleteExRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	// 构造回复
	PROTOCOL::CAlarmRelatedCarsChangeRespond l_oRespond;
	l_oRespond.m_oHeader.m_strMsgId = l_oRequest.m_oHeader.m_strMsgId;

	std::string l_strJQSACLBH = l_oRequest.m_oBody.m_alarmRelatedCarsInfo.m_mapInfo[PROTOCOL::Ex_Cars_JQSACLBH];
	std::string l_strJJDBH = l_oRequest.m_oBody.m_alarmRelatedCarsInfo.m_mapInfo[PROTOCOL::Ex_Cars_JJDBH];
	std::string l_strUpdateTime = l_oRequest.m_oBody.m_alarmRelatedCarsInfo.m_mapInfo[PROTOCOL::Ex_Cars_update_time];
	std::string l_strCreateTime = l_oRequest.m_oBody.m_alarmRelatedCarsInfo.m_mapInfo[PROTOCOL::Ex_Cars_create_time];
	if (l_strJQSACLBH.empty())
	{
		ICC_LOG_ERROR(m_pLog, "param error: delete guid is null!!![%s]", p_pNotify->GetMessages().c_str());
		l_oRespond.m_oBody.m_strResult = "1";
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "JQSACLBH empty";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	if (l_strCreateTime.empty())
	{
		l_strCreateTime = _QueryAlarmRelatedCarsCreateTime(l_strJQSACLBH);
		if (l_strCreateTime.empty())
		{
			ICC_LOG_ERROR(m_pLog, "createtime is empty,get CurrentDateTime!");
			l_strCreateTime = m_pDateTime->CurrentDateTimeStr();
		}
	}

	l_oRequest.m_oBody.m_alarmRelatedCarsInfo.m_mapInfo.clear();
	l_oRequest.m_oBody.m_alarmRelatedCarsInfo.m_mapInfo[PROTOCOL::Ex_Cars_JQSACLBH] = l_strJQSACLBH;
	l_oRequest.m_oBody.m_alarmRelatedCarsInfo.m_mapInfo[PROTOCOL::Ex_Cars_JJDBH] = l_strJJDBH;
	if (l_strUpdateTime.empty())
	{
		l_strUpdateTime = m_pDateTime->CurrentDateTimeStr();
	}
	l_oRequest.m_oBody.m_alarmRelatedCarsInfo.m_mapInfo[PROTOCOL::Ex_Cars_update_time] = l_strUpdateTime;
	
	l_oRequest.m_oBody.m_alarmRelatedCarsInfo.m_mapInfo[PROTOCOL::Ex_Cars_create_time] = l_strCreateTime;
	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "delete_icc_t_jqclb_by_guid";
	l_tSQLReq.param[PROTOCOL::Ex_Cars_JQSACLBH] = l_strJQSACLBH;
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq);
	bool bRes = false;
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "delete_icc_t_jqclb_by_guid failed! sql:[%s]", l_pRSet->GetSQL().c_str());
		l_oRespond.m_oBody.m_strResult = "1";
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "execute " + l_tSQLReq.sql_id + " failed";
	}
	else
	{
		l_oRespond.m_oBody.m_strResult = "0";
		l_oRespond.m_oHeader.m_strResult = "0";
		ICC_LOG_DEBUG(m_pLog, "delete_icc_t_jqclb_by_guid success! ");
		bRes = true;
	}
	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	//涉案车辆有删除时，更新接警单表的更新时间字段
	if (!l_strJJDBH.empty())
	{
		DataBase::SQLRequest l_oSQLReq;
		l_oSQLReq.sql_id = "update_icc_t_jjdb";
		l_oSQLReq.param["id"] = l_strJJDBH;
		l_oSQLReq.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
		l_pRSet = m_pDBConn->Exec(l_oSQLReq);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "update_icc_t_jjdb failed! sql:[%s]", l_pRSet->GetSQL().c_str());
		}
	}
	
	if (bRes)
	{
		//发同步消息
		SendAlarmCarsChangeSync(l_oRequest.m_oHeader,l_oRequest.m_oBody.m_alarmRelatedCarsInfo, "3");
	}
}

void CBusinessImpl::OnAlarmRelatedCarsUpdateExRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CAlarmRelatedCarsUpdateExRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	// 构造回复
	PROTOCOL::CAlarmRelatedCarsChangeRespond l_oRespond;
	l_oRespond.m_oHeader.m_strMsgId = l_oRequest.m_oHeader.m_strMsgId;

	std::string l_strJJDBH = l_oRequest.m_oBody.m_alarmRelatedCarsInfo.m_mapInfo[PROTOCOL::Ex_Cars_JJDBH];
	std::string l_strGUID = l_oRequest.m_oBody.m_alarmRelatedCarsInfo.m_mapInfo[PROTOCOL::Ex_Cars_JQSACLBH];
	if (l_strGUID.empty())
	{
		ICC_LOG_ERROR(m_pLog, "param error: update guid is null!!![%s]", p_pNotify->GetMessages().c_str());
		l_oRespond.m_oBody.m_strResult = "1";
		l_oRespond.m_oHeader.m_strResult = "1";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}


	if (l_oRequest.m_oBody.m_alarmRelatedCarsInfo.m_mapInfo[PROTOCOL::Ex_Cars_update_time].empty())
	{
		l_oRequest.m_oBody.m_alarmRelatedCarsInfo.m_mapInfo[PROTOCOL::Ex_Cars_update_time] = m_pDateTime->CurrentDateTimeStr();
	}

	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "update_icc_t_jqclb";
	std::map<std::string, std::string>& mapTmp = l_oRequest.m_oBody.m_alarmRelatedCarsInfo.m_mapInfo;
	std::map<std::string, std::string>::const_iterator itr_const;

	l_tSQLReq.param[PROTOCOL::Ex_Cars_JQSACLBH] = l_strGUID;
	for (itr_const = mapTmp.begin(); itr_const != mapTmp.end(); ++itr_const)
	{
		if (itr_const->first == PROTOCOL::Ex_Cars_JQSACLBH)
		{
			l_tSQLReq.param[itr_const->first] = itr_const->second;
		}
		else
		{
			if (!itr_const->second.empty())
			{
				l_tSQLReq.set[itr_const->first] = itr_const->second;
			}
		}
	}

	bool bRes = false;
	l_tSQLReq.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update_icc_t_jqclb failed! sql:[%s]", l_pRSet->GetSQL().c_str());
		l_oRespond.m_oBody.m_strResult = "1";
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "execute " + l_tSQLReq.sql_id + " failed";
	}
	else
	{
		l_oRespond.m_oBody.m_strResult = "0";
		l_oRespond.m_oHeader.m_strResult = "0";
		ICC_LOG_DEBUG(m_pLog, "update_icc_t_jqclb success! ");
		bRes = true;
	}
	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
	
	//涉案车辆有更新时，更新接警单表的更新时间字段
	if (!l_strJJDBH.empty())
	{
		DataBase::SQLRequest l_oSQLReq;
		l_oSQLReq.sql_id = "update_icc_t_jjdb";
		l_oSQLReq.param["id"] = l_strJJDBH;
		l_oSQLReq.set["update_time"] = m_pDateTime->CurrentDateTimeStr();
		l_pRSet = m_pDBConn->Exec(l_oSQLReq);
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "update_icc_t_jjdb failed! sql:[%s]", l_pRSet->GetSQL().c_str());
		}
	}
	
	if (bRes)
	{
		//发同步消息
		SendAlarmCarsChangeSync(l_oRequest.m_oHeader,l_oRequest.m_oBody.m_alarmRelatedCarsInfo, "2");
	}
}

void CBusinessImpl::OnAlarmRelatedCarsQueryExRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	// 解析请求消息
	PROTOCOL::CAlarmRelatedCarsQueryExRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	// 构造回复
	PROTOCOL::CAlarmRelatedCarsQueryExRespond l_oRespond;
	l_oRespond.m_oHeader.m_strMsgId = l_oRequest.m_oHeader.m_strMsgId;

	if (l_oRequest.m_oBody.m_strAlarmID.empty())
	{
		ICC_LOG_ERROR(m_pLog, "param error: query alarm_id is null!!![%s]", p_pNotify->GetMessages().c_str());
		l_oRespond.m_oBody.m_strResult = "1";
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "AlarmID empty";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "query_icc_t_jqclb_by_alarm_id";
	l_tSQLReq.param["alarm_id"] = l_oRequest.m_oBody.m_strAlarmID;
	l_tSQLReq.param["deleted"] = "0";
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "query_icc_t_jqclb_by_alarm_id failed! sql:[%s]", l_pRSet->GetSQL().c_str());
		l_oRespond.m_oBody.m_strResult = "1";
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "execute " + l_tSQLReq.sql_id + " failed";
	}
	else
	{
		l_oRespond.m_oBody.m_strResult = "0";
		l_oRespond.m_oHeader.m_strResult = "0";
		ICC_LOG_DEBUG(m_pLog, "query_icc_t_jqclb_by_alarm_id success [%s]! ", l_pRSet->GetSQL().c_str());

		while (l_pRSet->Next())
		{
			PROTOCOL::CAlarmRelatedCarsInfoEx l_oData;
			std::vector<std::string> vecFiledNames = l_pRSet->GetFieldNames();
			for (int i = 0; i < vecFiledNames.size(); ++i)
			{
				l_oData.m_mapInfo[vecFiledNames[i]] = l_pRSet->GetValue(vecFiledNames[i]);
			}
			l_oRespond.m_oBody.m_vecAlarmRelatedCarsInfos.push_back(l_oData);
		}

	}
	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "size=%d,send message:[%s]", l_oRespond.m_oBody.m_vecAlarmRelatedCarsInfos.size(), l_strSendMsg.c_str());
}

void CBusinessImpl::OnAlarmRelatedCarsSearchExRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	// 解析请求消息
	PROTOCOL::CAlarmRelatedCarsSearchExRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	// 构造回复
	PROTOCOL::CAlarmRelatedCarsSearchExRespond l_oRespond;
	l_oRespond.m_oHeader.m_strMsgId = l_oRequest.m_oHeader.m_strMsgId;

	if (l_oRequest.m_oBody.m_strStartTime.empty() || l_oRequest.m_oBody.m_strEndTime.empty())
	{
		ICC_LOG_ERROR(m_pLog, "param error: missing query time!!![%s]", p_pNotify->GetMessages().c_str());
		l_oRespond.m_oBody.m_strResult = "1";
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "Query time empty";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "select_icc_t_jqclb_count";
	l_tSQLReq.param["begin_time"] = l_oRequest.m_oBody.m_strStartTime;
	l_tSQLReq.param["end_time"] = l_oRequest.m_oBody.m_strEndTime;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_jqclb_count failed! sql:[%s]", l_pRSet->GetSQL().c_str());
		l_oRespond.m_oBody.m_strResult = "1";
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "execute " + l_tSQLReq.sql_id + " failed";
	}
	else
	{
		std::string l_strAllCount;
		if (l_pRSet->Next())
		{
			l_strAllCount = l_pRSet->GetValue("num");

			ICC_LOG_DEBUG(m_pLog, "Get all jqclb size[%s]", l_strAllCount.c_str());
		}

		l_oRespond.m_oBody.m_strResult = "0";
		l_oRespond.m_oHeader.m_strResult = "0";
		ICC_LOG_DEBUG(m_pLog, "select_icc_t_jqclb_count success [%s]! ", l_pRSet->GetSQL().c_str());

		if (l_strAllCount.empty() || "0" == l_strAllCount)
		{
			std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
			p_pNotify->Response(l_strSendMsg);
			ICC_LOG_DEBUG(m_pLog, "no search record");
			return;
		}

		int l_nPageSize = m_pString->ToInt(l_oRequest.m_oBody.m_strPageSize.c_str());
		int l_nPageIndex = m_pString->ToInt(l_oRequest.m_oBody.m_strPageIndex.c_str());

		if (l_nPageSize < 0 || l_nPageSize > MAX_COUNT_EX) l_nPageSize = MAX_COUNT_EX;
		if (l_nPageIndex < 1) l_nPageIndex = 1;
		l_nPageIndex = (l_nPageIndex - 1) * l_nPageSize;

		l_tSQLReq.sql_id = "search_icc_t_jqclb_by_create_time";
		l_tSQLReq.param["limit"] = std::to_string(l_nPageSize);
		l_tSQLReq.param["offset"] = std::to_string(l_nPageIndex);

		DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_tSQLReq);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
		if (!l_pRSet->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "execquery error ,search_icc_t_jqclb_by_create_time failed! sql:[%s]", l_result->GetSQL().c_str());
			l_oRespond.m_oBody.m_strResult = "1";
			l_oRespond.m_oHeader.m_strResult = "1";
			l_oRespond.m_oHeader.m_strMsg = "execute " + l_tSQLReq.sql_id + " failed";
		}

		while (l_result->Next())
		{
			PROTOCOL::CAlarmRelatedCarsInfoEx l_oData;
			std::vector<std::string> vecFiledNames = l_result->GetFieldNames();
			for (int i = 0; i < vecFiledNames.size(); ++i)
			{
				l_oData.m_mapInfo[vecFiledNames[i]] = l_result->GetValue(vecFiledNames[i]);
			}
			l_oRespond.m_oBody.m_vecAlarmRelatedCarsInfos.push_back(l_oData);
		}

	}
	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "size=%d,send message:[%s]", l_oRespond.m_oBody.m_vecAlarmRelatedCarsInfos.size(), l_strSendMsg.c_str());
}


void CBusinessImpl::SendAlarmCarsChangeSync(const PROTOCOL::CHeaderEx& p_pRequestHeader,const PROTOCOL::CAlarmRelatedCarsInfoEx& p_CAlarmCarsInfoEx, const std::string& p_strSyncType)
{
	PROTOCOL::CAlarmRelatedCarsChangeSync l_CAlarmRelatedCarsChangeSync;
	l_CAlarmRelatedCarsChangeSync.m_oBody.m_alarmRelatedCarsInfo.m_mapInfo = p_CAlarmCarsInfoEx.m_mapInfo;
	l_CAlarmRelatedCarsChangeSync.m_oBody.m_strSyncType = p_strSyncType;

	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strSystemID = SYSTEMID;
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strRelatedID = "";
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strCmd = "topic_jqsacl_sync";
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strRequest = "topic_alarm_notice_sync";
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strRequestType = "1";
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strResponse = p_pRequestHeader.m_strResponse;
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strResponseType = "";

	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strRequestFlag = "MQ";

	//同步消息
	std::string l_strMsg(l_CAlarmRelatedCarsChangeSync.ToString(m_pJsonFty->CreateJson()));
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
}


void CBusinessImpl::AlarmRelatedPersonsChange(ObserverPattern::INotificationPtr p_pNotify,
	const PROTOCOL::CHeaderEx& p_oHead,
	const std::string& p_strSyncType,
	const std::string& p_strGUID,
	std::map<std::string, std::string>& p_mapData)
{
	PROTOCOL::CAlarmRelatedPersonsChangeRespond l_oRespond;
	DataBase::SQLRequest l_tSQLReq;
	if (p_strSyncType == "1")  //增加
	{
		if (p_strGUID.empty())
		{
			p_mapData[PROTOCOL::Ex_Persons_JQDSRDBH] = m_pString->CreateGuid();
		}
		else
		{
			p_mapData[PROTOCOL::Ex_Persons_JQDSRDBH] = p_strGUID;
		}
		
		p_mapData["is_delete"] = "0";
		p_mapData["create_time"] = m_pDateTime->CurrentDateTimeStr();
		//p_mapData["GXSJ"] = m_pDateTime->CurrentDateTimeStr();

		l_tSQLReq.sql_id = "insert_icc_t_jqdsrdb";
		std::map<std::string, std::string>::const_iterator itr_const;
		for (itr_const = p_mapData.begin(); itr_const != p_mapData.end(); ++itr_const)
		{
			l_tSQLReq.param[itr_const->first] = itr_const->second;
		}
	}
	else if (p_strSyncType == "2")  //修改
	{
		if (p_strGUID.empty())
		{
			ICC_LOG_ERROR(m_pLog, "param error: update guid is null!!![%s]", p_pNotify->GetMessages().c_str());
			l_oRespond.m_oBody.m_strResult = "1";
			l_oRespond.m_oHeader.m_strResult = "1";
			l_oRespond.m_oHeader.m_strMsg = "id empty";

			std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
			p_pNotify->Response(l_strSendMsg);
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
			return;
		}

		if (p_mapData["update_time"].empty())
		{
			p_mapData["update_time"] = m_pDateTime->CurrentDateTimeStr();
		}

		l_tSQLReq.sql_id = "update_icc_t_jqdsrdb";
		std::map<std::string, std::string>::const_iterator itr_const;
		for (itr_const = p_mapData.begin(); itr_const != p_mapData.end(); ++itr_const)
		{
			if (itr_const->first == PROTOCOL::Persons_guid)
			{
				l_tSQLReq.param[itr_const->first] = itr_const->second;
			}
			else
			{
				if (!itr_const->second.empty())
				{
					l_tSQLReq.set[itr_const->first] = itr_const->second;
				}
			}
		}
	}
	else if (p_strSyncType == "3")  //删除
	{
		if (p_strGUID.empty())
		{
			ICC_LOG_ERROR(m_pLog, "param error: delete guid is null!!![%s]", p_pNotify->GetMessages().c_str());
			l_oRespond.m_oBody.m_strResult = "1";
			l_oRespond.m_oHeader.m_strResult = "1";
			l_oRespond.m_oHeader.m_strMsg =  "id empty";
			std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
			p_pNotify->Response(l_strSendMsg);
			ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
			return;
		}

		l_tSQLReq.sql_id = "delete_icc_t_jqdsrdb_by_guid";
		l_tSQLReq.param[PROTOCOL::Ex_Persons_JQDSRDBH] = p_strGUID;
	}
	else
	{
		return;
	}

	bool bRes = false;
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "%s failed! sql:[%s]", l_tSQLReq.sql_id.c_str(), l_pRSet->GetSQL().c_str());
		l_oRespond.m_oBody.m_strResult = "1";
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "execute " + l_tSQLReq.sql_id + " failed";
	}
	else
	{
		l_oRespond.m_oBody.m_strResult = "0";
		l_oRespond.m_oHeader.m_strResult = "0";
		ICC_LOG_DEBUG(m_pLog, "%s success! ", l_tSQLReq.sql_id.c_str());
		bRes = true;
	}
	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());

	if (bRes)
	{
		SendAlarmPersonsChangeSync(p_oHead,p_mapData, p_strSyncType);
	}
}


void CBusinessImpl::OnAlarmRelatedPersonsAddExRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	// 解析请求消息
	PROTOCOL::CAlarmRelatedPersonsAddExRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}
	std::string l_strTmpID = l_oRequest.m_oBody.m_alarmRelatedPersonsInfo.m_mapInfo[PROTOCOL::Ex_Persons_JQDSRDBH];
	AlarmRelatedPersonsChange(p_pNotify, l_oRequest.m_oHeader, "1", l_strTmpID, l_oRequest.m_oBody.m_alarmRelatedPersonsInfo.m_mapInfo);
}


void CBusinessImpl::OnAlarmRelatedPersonsDeleteExRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CAlarmRelatedPersonsDeleteExRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}
	std::string l_strGUID = l_oRequest.m_oBody.m_alarmRelatedPersonsInfo.m_mapInfo[PROTOCOL::Ex_Persons_JQDSRDBH];
	AlarmRelatedPersonsChange(p_pNotify, l_oRequest.m_oHeader, "3", l_strGUID, l_oRequest.m_oBody.m_alarmRelatedPersonsInfo.m_mapInfo);
}

void CBusinessImpl::OnAlarmRelatedPersonsUpdateExRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CAlarmRelatedPersonsDeleteExRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}
	std::string l_strGUID = l_oRequest.m_oBody.m_alarmRelatedPersonsInfo.m_mapInfo[PROTOCOL::Ex_Persons_JQDSRDBH];
	AlarmRelatedPersonsChange(p_pNotify, l_oRequest.m_oHeader, "2", l_strGUID, l_oRequest.m_oBody.m_alarmRelatedPersonsInfo.m_mapInfo);
}

void CBusinessImpl::OnAlarmRelatedPersonsQueryExRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	// 解析请求消息
	PROTOCOL::CAlarmRelatedPersonsQueryExRequest l_oRequest;
	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	// 构造回复
	PROTOCOL::CAlarmRelatedPersonsQueryExRespond l_oRespond;
	l_oRespond.m_oHeader.m_strMsgId = l_oRequest.m_oHeader.m_strMsgId;

	if (l_oRequest.m_oBody.m_strAlarmID.empty())
	{
		ICC_LOG_ERROR(m_pLog, "param error: query alarm_id is null!!![%s]", p_pNotify->GetMessages().c_str());
		l_oRespond.m_oBody.m_strResult = "1";
		l_oRespond.m_oHeader.m_strResult = "1";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "query_icc_t_jqdsrdb_by_alarm_id";
	l_tSQLReq.param["alarm_id"] = l_oRequest.m_oBody.m_strAlarmID;
	l_tSQLReq.param["deleted"] = "false";
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "query_icc_t_jqdsrdb_by_alarm_id failed! sql:[%s]", l_pRSet->GetSQL().c_str());
		l_oRespond.m_oBody.m_strResult = "1";
		l_oRespond.m_oHeader.m_strResult = "1";
	}
	else
	{
		l_oRespond.m_oBody.m_strResult = "0";
		l_oRespond.m_oHeader.m_strResult = "0";
		ICC_LOG_DEBUG(m_pLog, "query_icc_t_jqdsrdb_by_alarm_id success! ");

		while (l_pRSet->Next())
		{
			PROTOCOL::CAlarmRelatedPersonsInfoEx l_oData;
			std::vector<std::string> vecFiledNames = l_pRSet->GetFieldNames();
			for (int i = 0; i < vecFiledNames.size(); ++i)
			{
				l_oData.m_mapInfo[vecFiledNames[i]] = l_pRSet->GetValue(vecFiledNames[i]);
			}
			l_oRespond.m_oBody.m_vecAlarmRelatedPersonsInfos.push_back(l_oData);
		}

	}
	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
}

void CBusinessImpl::SendAlarmPersonsChangeSync(const PROTOCOL::CHeaderEx& p_oHead,const std::map<std::string, std::string>& p_mapData, const std::string& p_strSyncType)
{
	PROTOCOL::CAlarmRelatedPersonsChangeSync l_CAlarmRelatedCarsChangeSync;
	l_CAlarmRelatedCarsChangeSync.m_oBody.m_alarmRelatedPersonsInfo.m_mapInfo = p_mapData;
	l_CAlarmRelatedCarsChangeSync.m_oBody.m_strSyncType = p_strSyncType;

	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strSystemID = SYSTEMID;
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strSubsystemID = SUBSYSTEMID;
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strRelatedID = "";
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strCmd = "topic_jqdsr_sync";
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strRequest = "topic_alarm_notice_sync";
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strRequestType = "1";
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strResponse = p_oHead.m_strResponse;
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strResponseType = "";
	l_CAlarmRelatedCarsChangeSync.m_oHeader.m_strRequestFlag = "MQ";
	//同步消息
	std::string l_strMsg(l_CAlarmRelatedCarsChangeSync.ToString(m_pJsonFty->CreateJson()));
	m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strMsg.c_str());
}

std::string CBusinessImpl::_QueryRemarkByAlarmId(const std::string& strAlarmId)
{
	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "query_remark_icc_t_alarm_by_alarm_id";
	l_tSQLReq.param["alarm_id"] = strAlarmId;	
	std::string strTime = m_pDateTime->GetAlarmIdTime(strAlarmId);
	if (strTime != "")
	{
		l_tSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_tSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}
	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq);
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "query_remark_icc_t_alarm_by_alarm_id failed! sql:[%s]", l_pRSet->GetSQL().c_str());		
	}
	else
	{		
		ICC_LOG_DEBUG(m_pLog, "query_remark_icc_t_alarm_by_alarm_id success! ");

		if (l_pRSet->Next())
		{
		
			return l_pRSet->GetValue("alarm_id");
		}
	}

	return "";
}




void CBusinessImpl::OnAlarmRelatedPersonsSearchExRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	// 解析请求消息
	PROTOCOL::CAlarmRelatedPersonsSearchExRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	// 构造回复
	PROTOCOL::CAlarmRelatedPersonsQueryExRespond l_oRespond;
	l_oRespond.m_oHeader.m_strMsgId = l_oRequest.m_oHeader.m_strMsgId;

	if (l_oRequest.m_oBody.m_strStartTime.empty() || l_oRequest.m_oBody.m_strEndTime.empty())
	{
		ICC_LOG_ERROR(m_pLog, "param error: missing query time!!![%s]", p_pNotify->GetMessages().c_str());
		l_oRespond.m_oBody.m_strResult = "1";
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "Query time empty";
		std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
		p_pNotify->Response(l_strSendMsg);
		ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
		return;
	}

	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "select_icc_t_jqdsrdb_count";
	l_tSQLReq.param["begin_time"] = l_oRequest.m_oBody.m_strStartTime;
	l_tSQLReq.param["end_time"] = l_oRequest.m_oBody.m_strEndTime;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(l_tSQLReq);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());
	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_jqdsrdb_count failed! sql:[%s]", l_pRSet->GetSQL().c_str());
		l_oRespond.m_oBody.m_strResult = "1";
		l_oRespond.m_oHeader.m_strResult = "1";
		l_oRespond.m_oHeader.m_strMsg = "execute " + l_tSQLReq.sql_id + " failed";
	}
	else
	{
		std::string l_strAllCount;
		if (l_pRSet->Next())
		{
			l_strAllCount = l_pRSet->GetValue("num");

			ICC_LOG_DEBUG(m_pLog, "Get all jqdsrdb size[%s]", l_strAllCount.c_str());
		}

		l_oRespond.m_oBody.m_strResult = "0";
		l_oRespond.m_oHeader.m_strResult = "0";
		ICC_LOG_DEBUG(m_pLog, "select_icc_t_jqdsrdb_count success [%s]! ", l_pRSet->GetSQL().c_str());

		if (l_strAllCount.empty() || "0" == l_strAllCount)
		{
			std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
			p_pNotify->Response(l_strSendMsg);
			ICC_LOG_DEBUG(m_pLog, "no search record");
			return;
		}

		int l_nPageSize = m_pString->ToInt(l_oRequest.m_oBody.m_strPageSize.c_str());
		int l_nPageIndex = m_pString->ToInt(l_oRequest.m_oBody.m_strPageIndex.c_str());

		if (l_nPageSize < 0 || l_nPageSize > MAX_COUNT_EX) l_nPageSize = MAX_COUNT_EX;
		if (l_nPageIndex < 1) l_nPageIndex = 1;
		l_nPageIndex = (l_nPageIndex - 1) * l_nPageSize;

		l_tSQLReq.sql_id = "search_icc_t_jqdsrdb_by_create_time";
		l_tSQLReq.param["limit"] = std::to_string(l_nPageSize);
		l_tSQLReq.param["offset"] = std::to_string(l_nPageIndex);

		DataBase::IResultSetPtr l_result = m_pDBConn->Exec(l_tSQLReq);
		ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_result->GetSQL().c_str());
		if (!l_result->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "execquery error ,search_icc_t_jqdsrdb_by_create_time failed! sql:[%s]", l_result->GetSQL().c_str());
			l_oRespond.m_oBody.m_strResult = "1";
			l_oRespond.m_oHeader.m_strResult = "1";
			l_oRespond.m_oHeader.m_strMsg = "execute " + l_tSQLReq.sql_id + " failed";
		}
		else
		{
			while (l_result->Next())
			{
				PROTOCOL::CAlarmRelatedPersonsInfoEx l_oData;
				std::vector<std::string> vecFiledNames = l_result->GetFieldNames();
				for (int i = 0; i < vecFiledNames.size(); ++i)
				{
					l_oData.m_mapInfo[vecFiledNames[i]] = l_result->GetValue(vecFiledNames[i]);
				}
				l_oRespond.m_oBody.m_vecAlarmRelatedPersonsInfos.push_back(l_oData);
			}
		}
	}
	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "size=%d,send message:[%s]", l_oRespond.m_oBody.m_vecAlarmRelatedPersonsInfos.size(), l_strSendMsg.c_str());
}


bool CBusinessImpl::_QueryAlarmInfo(const std::string& strAlarmId, PROTOCOL::CAlarmInfo& alarmInfo)
{
	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "select_icc_t_jjdb";
	l_tSQLReq.param["id"] = strAlarmId;

	std::string strTime = m_pDateTime->GetAlarmIdTime(strAlarmId);
	if (strTime != "")
	{
		l_tSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_tSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_tSQLReq);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{		
		ICC_LOG_ERROR(m_pLog, "select_icc_t_alarm failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}

	if (l_pResult->Next())
	{	
		alarmInfo.ParseAlarmRecord(l_pResult);
		return true;
	}

	ICC_LOG_WARNING(m_pLog, "record is null");

	return false;
}

bool CBusinessImpl::Build12345Request(PROTOCOL::CPushAlarmRequest& p_sRequest, PROTOCOL::CPushAlarmTo12345Request& p_dRequest)
{
	DataBase::SQLRequest l_tSQLReq;
	l_tSQLReq.sql_id = "select_icc_t_jjdb_by_alarm_id";
	l_tSQLReq.param["id"] = p_sRequest.m_oBody.m_strAlarmID;
	std::string strTime = m_pDateTime->GetAlarmIdTime(p_sRequest.m_oBody.m_strAlarmID);
	if (strTime != "")
	{
		l_tSQLReq.param["jjsj_begin"] = m_pDateTime->GetFrontTime(strTime, 30 * 86400);
		l_tSQLReq.param["jjsj_end"] = m_pDateTime->GetAfterTime(strTime, 30 * 86400);
	}

	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_tSQLReq);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());

	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_alarm failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	 if (l_pResult->Next())
	{
		p_dRequest.ParseAlarmRecord(l_pResult, p_sRequest);
	}

	l_tSQLReq.sql_id = "select_icc_t_dept_by_code";
	l_tSQLReq.param["code"] = p_dRequest.m_oBody.m_alarmDatas.GXDWDM;
	l_pResult = m_pDBConn->Exec(l_tSQLReq);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_dept_by_code GXDWDM failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (l_pResult->Next())
	{
		p_dRequest.m_oBody.m_alarmDatas.GXDWMC = l_pResult->GetValue("name", "");
	}

	l_tSQLReq.sql_id = "select_icc_t_dept_by_code";
	l_tSQLReq.param["code"] = p_dRequest.m_oBody.m_alarmDatas.JJDWDM;
	l_pResult = m_pDBConn->Exec(l_tSQLReq);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "select_icc_t_dept_by_code JJDWDM failed,error msg:[%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (l_pResult->Next())
	{
		p_dRequest.m_oBody.m_alarmDatas.GXDWMC = l_pResult->GetValue("name", "");
		return true;
	}

	ICC_LOG_WARNING(m_pLog, "record is null");

	return false;
}

bool CBusinessImpl::Get12345Service(std::string& p_strIp, std::string& p_strPort)
{
	std::string m_strNacosServerIp = this->m_strNacosServerIp;
	std::string m_strNacosServerPort = this->m_strNacosServerPort;
	std::string m_strNacosNamespace = m_strNacosServerNamespace;
	std::string m_str12345ServiceName = m_pConfig->GetValue("ICC/Plugin/Synthetical/vcsservicename", "commandcenter-icc-octg");
	std::string m_strNacosGroupName = m_strNacosServerGroupName;
	std::string m_strNacosQueryUrl = m_pConfig->GetValue("ICC/Component/HttpServer/queryurl", "/nacos/v1/ns/instance/list");
	std::string m_str12345ServiceHealthyFlag = m_pConfig->GetValue("ICC/Plugin/Synthetical/servicehealthyflag", "1");

	std::string strTarget = m_pString->Format("%s?namespaceId=%s&serviceName=%s@@%s", m_strNacosQueryUrl.c_str(), m_strNacosNamespace.c_str(),
		m_strNacosGroupName.c_str(), m_str12345ServiceName.c_str());
	std::string strContent;
	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	std::string strErrorMessage;

	std::string strReceive = m_pHttpClient->GetWithTimeout(m_strNacosServerIp, m_strNacosServerPort, strTarget, mapHeaders, strContent, strErrorMessage,2);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_pLog, "not receive nacos server response.NacosServerIp=%s:%s,strTarget=%s", m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str(), strTarget.c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "receive nacos response : [%s]. ", strReceive.c_str());

	JsonParser::IJsonPtr pJson = ICCGetIJsonFactory()->CreateJson();
	if (!pJson->LoadJson(strReceive))
	{
		ICC_LOG_ERROR(m_pLog, "analyze nacos response failed.[%s] ", strReceive.c_str());
		return false;
	}

	int iCount = pJson->GetCount("/hosts");
	for (int i = 0; i < iCount; i++)
	{
		std::string l_strPrefixPath("/hosts/" + std::to_string(i) + "/");
		std::string strHealthy = pJson->GetNodeValue(l_strPrefixPath + "healthy", "");
		if (strHealthy == m_str12345ServiceHealthyFlag)
		{
			p_strIp = pJson->GetNodeValue(l_strPrefixPath + "ip", "");
			p_strPort = pJson->GetNodeValue(l_strPrefixPath + "port", "");
			ICC_LOG_DEBUG(m_pLog, "find healthy 12345 service : [%s:%s]. ", p_strIp.c_str(), p_strPort.c_str());
			return true;
		}
	}

	ICC_LOG_DEBUG(m_pLog, "not find healthy 12345 service,iCount=%d,m_strNacosServerIp=%s:%s!!!", iCount, m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str());
	return false;
}

void CBusinessImpl::SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName)
{
	boost::lock_guard<boost::mutex> lock(m_mutexNacosParams);
	m_strNacosServerIp = strNacosIp;
	m_strNacosServerPort = strNacosPort;
	m_strNacosServerNamespace = strNameSpace;
	m_strNacosServerGroupName = strGroupName;
}

void CBusinessImpl::OnReceiveSynNacosParams(ObserverPattern::INotificationPtr p_pNotifiRequest)
{
	std::string l_strRequestMsg = p_pNotifiRequest->GetMessages();
	ICC_LOG_DEBUG(m_pLog, "Server nacos params Sync: %s", l_strRequestMsg.c_str());

	PROTOCOL::CSyncNacosParams syn;
	if (!syn.ParseString(l_strRequestMsg, ICCGetIJsonFactory()->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "l_strRequestMsg parse json failed");
		return;
	}

	SetNacosParams(syn.m_oBody.m_strNacosServerIp, syn.m_oBody.m_strNacosServerPort, syn.m_oBody.m_strNacosNamespace, syn.m_oBody.m_strNacosGroupName);
}

void CBusinessImpl::SendMessageToMsgCenterOnMajorAlarm(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest l_oRequest, std::string l_strMsg)
{
	bool l_majorAlrm = false;
	DataBase::SQLRequest l_strDeptSql;
	l_strDeptSql.sql_id = "select_icc_t_major_alarm_config_num";
	l_strDeptSql.param["jqlbdm"] = l_oRequest.m_oBody.m_oAlarm.m_strFirstType;
	l_strDeptSql.param["jqlxdm"] = l_oRequest.m_oBody.m_oAlarm.m_strSecondType;
	l_strDeptSql.param["jqxldm"] = l_oRequest.m_oBody.m_oAlarm.m_strThirdType;
	l_strDeptSql.param["jqzldm"] = l_oRequest.m_oBody.m_oAlarm.m_strFourthType;
	l_strDeptSql.param["jqdjdm"] = l_oRequest.m_oBody.m_oAlarm.m_strLevel;
	DataBase::IResultSetPtr l_sqlResult = m_pDBConn->Exec(l_strDeptSql);

	if (!l_sqlResult->IsValid())
	{
		std::string l_strSQL = l_sqlResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "%s table failed, SQL: [%s], Error: [%s]",
			l_strDeptSql.sql_id.c_str(),
			l_strSQL.c_str(),
			l_sqlResult->GetErrorMsg().c_str());

		return;
	}
	else if (l_sqlResult->Next())
	{
		if (m_pString->ToInt(l_sqlResult->GetValue("num", "")) >= 1)
		{
			l_majorAlrm = true;
		}
	}

	if (!l_majorAlrm)
	{
		l_strDeptSql.sql_id = "select_icc_t_major_alarm_config_by_type";
		l_sqlResult = m_pDBConn->Exec(l_strDeptSql);

		if (!l_sqlResult->IsValid())
		{
			std::string l_strSQL = l_sqlResult->GetSQL();
			ICC_LOG_ERROR(m_pLog, "%s table failed, SQL: [%s], Error: [%s]",
				l_strDeptSql.sql_id.c_str(),
				l_strSQL.c_str(),
				l_sqlResult->GetErrorMsg().c_str());

			return;
		}

		while(l_sqlResult->Next())
		{
			if (m_pString->FindFirst(l_oRequest.m_oBody.m_oAlarm.m_strContent, l_sqlResult->GetValue("type_value", "")) != -1)
			{
				l_majorAlrm = true;
				break;
			}
		}
	}

	if (l_majorAlrm)
	{
		std::string strName = "AddMajorAlarmConfig";
		std::string strParamInfo;
		if (!m_pRedisClient->HGet(PARAM_INFO, strName, strParamInfo))
		{
			ICC_LOG_DEBUG(m_pLog, "HGet Param INFO Failed!!!");
			return;
		}
		PROTOCOL::CParamInfo l_ParamInfo;
		JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
		if (!l_ParamInfo.Parse(strParamInfo, l_pJson))
		{
			ICC_LOG_DEBUG(m_pLog, "Parse Param Info failed!!!");
			return;
		}

		MsgCenter::MessageSendDTOData l_CDTOData;
		std::string l_curSendMsg;
		l_CDTOData.appCode = "icc";
		l_CDTOData.businessCode = "alarminfo";
		l_CDTOData.compensateType = 0;
		l_CDTOData.delayDuration = "";
		l_CDTOData.delayType = 0;
		l_CDTOData.title = m_pString->ReplaceFirst(l_ParamInfo.m_strValue, "$", l_oRequest.m_oHeader.m_strSeatNo);
		l_CDTOData.title = m_pString->ReplaceFirst(l_CDTOData.title, "$", l_oRequest.m_oBody.m_oAlarm.m_strID);
		l_CDTOData.moduleCode = "alarminfo";
		l_CDTOData.needStorage = "true";
		l_CDTOData.sendType = "0";
		l_CDTOData.sync = "true";
		l_CDTOData.message = l_oRequest.m_oBody.m_oAlarm.m_strID;

		std::string l_deptGuid = m_pMsgCenter->DeptCodeToDeptGuid(l_oRequest.m_oBody.m_oAlarm.m_strReceiptDeptCode);
		std::vector<std::string> l_curNoticeList = m_pMsgCenter->GetGuidbByDept(l_deptGuid, "RECEIVE-MONITOR", m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName);
		if(l_curNoticeList.empty())
		{
			MsgCenter::ReceiveObject l_recObj;
			l_recObj.type = "role";
			l_recObj.noticeList.push_back(m_pConfig->GetValue("ICC/Plugin/MonitorSeat/MonitorCode", "ICC_BZ"));
			l_recObj.syncList.push_back(m_pConfig->GetValue("ICC/Plugin/MonitorSeat/MonitorCode", "ICC_BZ"));

			l_CDTOData.receiveObjects.push_back(l_recObj);
		}
		else
		{
			MsgCenter::ReceiveObject l_recObj;
			l_recObj.type = "user";
			for (int index = 0; index < l_curNoticeList.size(); index++)
			{
				l_recObj.noticeList.push_back(l_curNoticeList[index]);
				l_recObj.syncList.push_back(l_curNoticeList[index]);
			}
			l_CDTOData.receiveObjects.push_back(l_recObj);
		}

		if (l_CDTOData.receiveObjects[0].noticeList.size() == 0)
		{
			ICC_LOG_DEBUG(m_pLog, "No one pays attention to the current alarm list: [%s]", l_curSendMsg.c_str());
			return;
		}

		m_pMsgCenter->BuildSendMsg(l_curSendMsg, l_CDTOData);
		m_pMsgCenter->Send(l_curSendMsg, m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName);
	}
}

void CBusinessImpl::SendMessageToMsgCenterOnAlarmOverTime(std::string l_strSeatNo, std::string l_strMsg, std::string l_strName, std::string l_strDeptCode)
{
	std::string strName;
	if(!l_strSeatNo.empty())
		strName = "AlarmOverTime";
	else
		strName = "AlarmOverTimePerson";

	std::string strParamInfo;
	if (!m_pRedisClient->HGet(PARAM_INFO, strName, strParamInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "HGet Param INFO Failed!!!");
		return ;
	}
	PROTOCOL::CParamInfo l_ParamInfo;
	JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
	if (!l_ParamInfo.Parse(strParamInfo, l_pJson))
	{
		ICC_LOG_DEBUG(m_pLog, "Parse Param Info failed!!!");
		return ;
	}

	MsgCenter::MessageSendDTOData l_CDTOData;
	std::string l_curSendMsg;
	l_CDTOData.appCode = "icc";
	l_CDTOData.businessCode = "violation";
	l_CDTOData.compensateType = 0;
	l_CDTOData.delayDuration = "";
	l_CDTOData.delayType = 0;
	if (!l_strSeatNo.empty())
		l_CDTOData.title = m_pString->ReplaceFirst(l_ParamInfo.m_strValue, "$", l_strSeatNo);
	else
		l_CDTOData.title = m_pString->ReplaceFirst(l_ParamInfo.m_strValue, "$", l_strName);

	l_CDTOData.title = m_pString->ReplaceFirst(l_CDTOData.title, "$", l_strMsg);
	l_CDTOData.moduleCode = "violation";
	l_CDTOData.needStorage = "true";
	l_CDTOData.sendType = "0";
	l_CDTOData.sync = "true";
	l_CDTOData.message = l_strMsg;

	{
		std::string l_deptGuid = m_pMsgCenter->DeptCodeToDeptGuid(l_strDeptCode);
		std::vector<std::string> l_curNoticeList = m_pMsgCenter->GetGuidbByDept(l_deptGuid, "RECEIVE-MONITOR", m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName);

		if (l_curNoticeList.empty())
		{
			MsgCenter::ReceiveObject l_recObj;
			l_recObj.type = "role";
			l_recObj.noticeList.push_back(m_pConfig->GetValue("ICC/Plugin/MonitorSeat/MonitorCode", "ICC_BZ"));
			l_recObj.syncList.push_back(m_pConfig->GetValue("ICC/Plugin/MonitorSeat/MonitorCode", "ICC_BZ"));

			l_CDTOData.receiveObjects.push_back(l_recObj);
		}
		else
		{
			MsgCenter::ReceiveObject l_recObj;
			l_recObj.type = "user";
			for (int index = 0; index < l_curNoticeList.size(); index++)
			{
				l_recObj.noticeList.push_back(l_curNoticeList[index]);
				l_recObj.syncList.push_back(l_curNoticeList[index]);
			}
			l_CDTOData.receiveObjects.push_back(l_recObj);
		}
	}

	if (l_CDTOData.receiveObjects.size() == 0)
	{
		ICC_LOG_DEBUG(m_pLog, "No one pays attention to the current alarm list: [%s]", l_curSendMsg.c_str());
		return;
	}

	m_pMsgCenter->BuildSendMsg(l_curSendMsg, l_CDTOData);
	m_pMsgCenter->Send(l_curSendMsg, m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName);
}

bool CBusinessImpl::BuildDeptPhoneNum(std::string p_stdDeptId, std::string& p_stdDeptCalled)
{
	DataBase::SQLRequest l_strDeptSql;

	l_strDeptSql.sql_id = "select_icc_t_dept_by_code";
	l_strDeptSql.param["code"] = p_stdDeptId;
	DataBase::IResultSetPtr l_sqlResult = m_pDBConn->Exec(l_strDeptSql);

	if (!l_sqlResult->IsValid())
	{
		std::string l_strSQL = l_sqlResult->GetSQL();
		ICC_LOG_ERROR(m_pLog, "%s table failed, SQL: [%s], Error: [%s]",
			l_strDeptSql.sql_id.c_str(),
			l_strSQL.c_str(),
			l_sqlResult->GetErrorMsg().c_str());
	}
	else if (l_sqlResult->Next())
	{
		ICC_LOG_DEBUG(m_pLog, "sql success, value is %s ,SQL: [%s]",
			l_sqlResult->GetValue("value").c_str(),
			l_sqlResult->GetSQL().c_str());

		p_stdDeptCalled = l_sqlResult->GetValue("phone");
	}

	if (!p_stdDeptCalled.empty())
		return true;

	return false;
}

void CBusinessImpl::AutoUrgeOnUpdateAlarm(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest::CBody l_oSendData)
{
	if (l_oSendData.m_oAlarm.m_strIsSigned == "1")
	{
		ICC_LOG_DEBUG(m_pLog, "Cur Alarm(%s) Already Signed Not Need Urge",
			l_oSendData.m_oAlarm.m_strID.c_str());

		return;
	}

	for (size_t l_index = 0; l_index < l_oSendData.m_vecProcessData.size(); l_index++)
	{
		PROTOCOL::CAddAutoUrgeRequest l_oRequest;
		PROTOCOL::CAddOrUpdateProcessRequest::CProcessData l_processData = l_oSendData.m_vecProcessData[l_index];

		if (l_processData.m_strState != "01")
		{
			continue;
		}

		if (!BuildDeptPhoneNum(l_processData.m_strProcessDeptCode, l_oRequest.m_oBody.m_strUrgeCalled))
		{
			ICC_LOG_DEBUG(m_pLog, "[%s(%s)] not have phone, autourge interrupt", l_processData.m_strProcessDeptCode.c_str(), l_processData.m_strProcessDeptName.c_str());
			continue;
		}

		l_oRequest.m_oHeader.m_strCmd = "add_auto_urge_call_request";
		l_oRequest.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_oRequest.m_oHeader.m_strRequest = "alarm_process_timeout_sync";
		l_oRequest.m_oHeader.m_strRequestType = "1";//主题
		l_oRequest.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();

		if (m_strAutourgeType == "1")
		{
			l_oRequest.m_oBody.m_strAlarmId = l_processData.m_strAlarmID;
			l_oRequest.m_oBody.m_strCreateUser = l_processData.m_strDispatchName;
			l_oRequest.m_oBody.m_strDepartment = l_processData.m_strProcessDeptCode;
			l_oRequest.m_oBody.m_strRemarks = l_processData.m_strOverRemark;
			l_oRequest.m_oBody.m_strSeatNo = l_processData.m_strSeatCode;
			l_oRequest.m_oBody.m_strTTSContent = "";
			l_oRequest.m_oBody.m_strUpdateUser = "";
			l_oRequest.m_oBody.m_strUrgeCount = m_strUrgeCount;
			l_oRequest.m_oBody.m_strUrgeId = BuildUrgeId();
			l_oRequest.m_oBody.m_strUrgeInterval = m_strUrgeInterval;
			l_oRequest.m_oBody.m_strUrgeLevel = "1";
			l_oRequest.m_oBody.m_strUrgeStartTime = m_pDateTime->CurrentDateTimeStr();
			l_oRequest.m_oBody.m_strVoiceId = m_strUrgeVoiceId;
			l_oRequest.m_oBody.m_strVoiceType = "0";
		}
		else if (m_strAutourgeType == "2")
		{
			l_oRequest.m_oBody.m_strAlarmId = l_processData.m_strAlarmID;
			l_oRequest.m_oBody.m_strCreateUser = l_processData.m_strDispatchName;
			l_oRequest.m_oBody.m_strDepartment = l_processData.m_strProcessDeptCode;
			l_oRequest.m_oBody.m_strRemarks = l_processData.m_strOverRemark;
			l_oRequest.m_oBody.m_strSeatNo = l_processData.m_strSeatCode;
			l_oRequest.m_oBody.m_strTTSContent = m_strAutourgeSmsContent;
			l_oRequest.m_oBody.m_strUpdateUser = "";
			l_oRequest.m_oBody.m_strUrgeCount = m_strUrgeCount;
			l_oRequest.m_oBody.m_strUrgeId = BuildUrgeId();
			l_oRequest.m_oBody.m_strUrgeInterval = m_strUrgeInterval;
			l_oRequest.m_oBody.m_strUrgeLevel = "1";
			l_oRequest.m_oBody.m_strUrgeStartTime = m_pDateTime->CurrentDateTimeStr();
			l_oRequest.m_oBody.m_strVoiceId = m_strUrgeVoiceId;
			l_oRequest.m_oBody.m_strVoiceType = "2";
		}
		else if (m_strAutourgeType == "3")
		{
			l_oRequest.m_oBody.m_strAlarmId = l_processData.m_strAlarmID;
			l_oRequest.m_oBody.m_strCreateUser = l_processData.m_strDispatchName;
			l_oRequest.m_oBody.m_strDepartment = l_processData.m_strProcessDeptCode;
			l_oRequest.m_oBody.m_strRemarks = l_processData.m_strOverRemark;
			l_oRequest.m_oBody.m_strSeatNo = l_processData.m_strSeatCode;
			l_oRequest.m_oBody.m_strTTSContent = m_strAutourgeSmsContent;
			l_oRequest.m_oBody.m_strUpdateUser = "";
			l_oRequest.m_oBody.m_strUrgeCount = m_strUrgeCount;
			l_oRequest.m_oBody.m_strUrgeId = BuildUrgeId() + ";" + BuildUrgeId();
			l_oRequest.m_oBody.m_strUrgeInterval = m_strUrgeInterval;
			l_oRequest.m_oBody.m_strUrgeLevel = "1";
			l_oRequest.m_oBody.m_strUrgeStartTime = m_pDateTime->CurrentDateTimeStr();
			l_oRequest.m_oBody.m_strVoiceId = m_strUrgeVoiceId;
			l_oRequest.m_oBody.m_strVoiceType = "0;2";
		}

		std::string l_strSyncMessage = l_oRequest.ToString(m_pJsonFty->CreateJson());
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSyncMessage));

		ICC_LOG_DEBUG(m_pLog, "[%s(%s)] Action Urge,send msg %s", l_processData.m_strDispatchDeptName.c_str(), l_processData.m_strDispatchDeptCode.c_str(), l_strSyncMessage.c_str());

		if (m_strAutourgeType == "1")
		{
			_AutoUrgeAlarmInfoAndMessageCenter("1", l_oSendData.m_oAlarm.m_strReceiptDeptName, l_processData.m_strProcessDeptName, l_oSendData.m_oAlarm.m_strID);
		}
		else if (m_strAutourgeType == "2")
		{
			_AutoUrgeAlarmInfoAndMessageCenter("2", l_oSendData.m_oAlarm.m_strReceiptDeptName, l_processData.m_strProcessDeptName, l_oSendData.m_oAlarm.m_strID);
		}
		else if (m_strAutourgeType == "3")
		{
			_AutoUrgeAlarmInfoAndMessageCenter("1", l_oSendData.m_oAlarm.m_strReceiptDeptName, l_processData.m_strProcessDeptName, l_oSendData.m_oAlarm.m_strID);
			_AutoUrgeAlarmInfoAndMessageCenter("2", l_oSendData.m_oAlarm.m_strReceiptDeptName, l_processData.m_strProcessDeptName, l_oSendData.m_oAlarm.m_strID);
		}
	}
}

void CBusinessImpl::_AutoUrgeAlarmInfoAndMessageCenter(std::string strSendType, std::string strReceiptDeptName, std::string strProcessDeptName, std::string strAlarmId)
{
	//发送至消息中心
	std::string l_curMsg;
	if(strSendType == "1")
		m_pMsgCenter->BuildManualAcceptConfig("UrgeDept", l_curMsg);
	else if(strSendType == "2")
		m_pMsgCenter->BuildManualAcceptConfig("UrgeDeptSms", l_curMsg);
	l_curMsg = m_pString->ReplaceFirst(l_curMsg, "$", strReceiptDeptName);
	l_curMsg = m_pString->ReplaceFirst(l_curMsg, "$", strProcessDeptName);

	m_pMsgCenter->Send(l_curMsg, strAlarmId, true, true, m_strNacosServerIp, m_strNacosServerPort, m_strNacosServerNamespace, m_strNacosServerGroupName);

	//写流水
	PROTOCOL::CAlarmLogSync l_oTHLData;
	l_oTHLData.m_oBody.m_strID = m_pString->CreateGuid();
	std::vector<std::string> l_vecParamList;

	if (strSendType == "1")
	{
		l_vecParamList.push_back(strReceiptDeptName);
		l_vecParamList.push_back(strProcessDeptName);
		l_oTHLData.m_oBody.m_strOperate = "BS001410001";
	}
	else if (strSendType == "2")
	{
		l_vecParamList.push_back(strProcessDeptName);
		l_vecParamList.push_back(m_strAutourgeSmsContent);
		l_oTHLData.m_oBody.m_strOperate = "BS001011002";
	}

	l_oTHLData.m_oBody.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
	l_oTHLData.m_oBody.m_strAlarmID = strAlarmId;
	l_oTHLData.m_oBody.m_strCreateTime = m_pDateTime->CurrentDateTimeStr();

	//写流水
	if (!InsertAlarmLogInfo(l_oTHLData.m_oBody, ""))
	{
		ICC_LOG_ERROR(m_pLog, "insert alarm log info failed");
		return;
	}
	else
	{
		SyncAlarmLogInfo(l_oTHLData.m_oBody);
	}
}

std::string CBusinessImpl::BuildUrgeId()
{
	std::string l_curTime = m_pDateTime->CurrentDateTimeStr();

	for (auto l_itera = l_curTime.begin(); l_itera != l_curTime.end();)
	{
		if (*l_itera >= '0' && *l_itera <= '9')
			l_itera++;
		else
			l_itera = l_curTime.erase(l_itera);
	}

	l_curTime += std::to_string(rand() % 1000000);

	return l_curTime;
}

bool CBusinessImpl::GetServiceInfo(std::string& p_strServiceName, std::string& p_strIp, std::string& p_strPort)
{
	std::string m_strNacosServerIp = this->m_strNacosServerIp;
	std::string m_strNacosServerPort = this->m_strNacosServerPort;
	std::string m_strNacosNamespace = m_strNacosServerNamespace;
	std::string m_strServiceName = p_strServiceName;
	std::string m_strNacosGroupName = m_strNacosServerGroupName;
	std::string m_strNacosQueryUrl = m_pConfig->GetValue("ICC/Component/HttpServer/queryurl", "/nacos/v1/ns/instance/list");
	std::string m_strVcsServiceHealthyFlag = m_pConfig->GetValue("ICC/Plugin/Synthetical/servicehealthyflag", "1");

	std::string strTarget = m_pString->Format("%s?namespaceId=%s&serviceName=%s@@%s", m_strNacosQueryUrl.c_str(), m_strNacosNamespace.c_str(),
		m_strNacosGroupName.c_str(), m_strServiceName.c_str());
	std::string strContent;
	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", "application/x-www-form-urlencoded"));
	std::string strErrorMessage;

	std::string strReceive = m_pHttpClient->GetWithTimeout(m_strNacosServerIp, m_strNacosServerPort, strTarget, mapHeaders, strContent, strErrorMessage, 2);
	if (strReceive.empty())
	{
		ICC_LOG_ERROR(m_pLog, "not receive nacos server response.NacosServerIp=%s:%s,strTarget=%s", m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str(), strTarget.c_str());
		return false;
	}

	ICC_LOG_DEBUG(m_pLog, "receive nacos response : [%s]. ", strReceive.c_str());

	JsonParser::IJsonPtr pJson = ICCGetIJsonFactory()->CreateJson();
	if (!pJson->LoadJson(strReceive))
	{
		ICC_LOG_ERROR(m_pLog, "analyze nacos response failed.[%s] ", strReceive.c_str());
		return false;
	}

	int iCount = pJson->GetCount("/hosts");
	for (int i = 0; i < iCount; i++)
	{
		std::string l_strPrefixPath("/hosts/" + std::to_string(i) + "/");
		std::string strHealthy = pJson->GetNodeValue(l_strPrefixPath + "healthy", "");
		if (strHealthy == m_strVcsServiceHealthyFlag)
		{
			p_strIp = pJson->GetNodeValue(l_strPrefixPath + "ip", "");
			p_strPort = pJson->GetNodeValue(l_strPrefixPath + "port", "");
			ICC_LOG_DEBUG(m_pLog, "find healthy sms send service : [%s:%s]. ", p_strIp.c_str(), p_strPort.c_str());
			return true;
		}
	}

	ICC_LOG_DEBUG(m_pLog, "not find healthy sms send service,iCount=%d,m_strNacosServerIp=%s:%s!!!", iCount, m_strNacosServerIp.c_str(), m_strNacosServerPort.c_str());
	return false;
}

void CBusinessImpl::OnNotifiPostSmsSend(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());
	PROTOCOL::CPostSmsSendRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	PROTOCOL::CPostSmsSendResponse l_oRespond;
	
	do
	{
		std::string strTarget = "/external/sms/send";
		std::string strErrorMessage;
		std::map<std::string, std::string> mapHeaders;
		JsonParser::IJsonPtr pJson = ICCGetIJsonFactory()->CreateJson();
		mapHeaders.insert(std::make_pair("Content-Type", "application/json"));

		std::string strSmsSendServerIp;
		std::string strSmsSendServerPort;


		if (!GetServiceInfo(m_strSmsServiceName, strSmsSendServerIp, strSmsSendServerPort))
		{
			l_oRespond.m_oBody.m_strCode = "500";
			l_oRespond.m_oBody.m_strDataCode = "0";
			l_oRespond.m_oBody.m_strMessage = "GetServiceInfo failed";
			ICC_LOG_DEBUG(m_pLog, "get GetServiceInfo info failed");
			break;
		}

		ICC_LOG_DEBUG(m_pLog, "OnNotifiPostSmsSend Content: %s", l_oRequest.ToString(m_pJsonFty->CreateJson()).c_str());

		//std::string strReceive = m_pHttpClient->PostEx(strSmsSendServerIp, strSmsSendServerPort, strTarget, mapHeaders, l_oRequest.ToString(m_pJsonFty->CreateJson()), strErrorMessage);
		std::string strReceive = m_pHttpClient->PostWithTimeout(strSmsSendServerIp, strSmsSendServerPort, strTarget, mapHeaders, l_oRequest.ToString(m_pJsonFty->CreateJson()), strErrorMessage, 2);
		
		ICC_LOG_DEBUG(m_pLog, "OnNotifiPostSmsSend to strReceive: %s", strReceive.c_str());

		if (strReceive.empty())
		{
			l_oRespond.m_oBody.m_strCode = "500";
			l_oRespond.m_oBody.m_strDataCode = "0";
			l_oRespond.m_oBody.m_strMessage = m_pString->Format("failed ServerIp=%s:%s,strTarget=%s", strSmsSendServerIp.c_str(), strSmsSendServerPort.c_str(),strTarget.c_str());
			break;
		}

		if (!l_oRespond.ParseString(strReceive, m_pJsonFty->CreateJson()))
		{
			l_oRespond.m_oBody.m_strCode = "501";
			l_oRespond.m_oBody.m_strDataCode = "0";
			l_oRespond.m_oBody.m_strMessage = "failed";
			break;
		}
		//返回 200 表示发送成功，写流水
		if (0 == l_oRespond.m_oBody.m_strCode.compare("200"))
		{
			//写流水
			PROTOCOL::CAlarmLogSync::CBody l_oAlarmLogInfo;
			l_oAlarmLogInfo.m_strAlarmID = l_oRequest.m_oBody.m_strAlarmId;
			std::vector<std::string> l_vecParamList;
			DataBase::SQLRequest l_SQLRequest;
			Data::CStaffInfo l_oStaffInfo;
			if (!_GetStaffInfo(l_oRequest.m_oBody.m_strStaffCode, l_oStaffInfo))
			{
				ICC_LOG_DEBUG(m_pLog, "get staff info failed!");
			}
			std::string l_strStaffName = _GetPoliceTypeName(l_oStaffInfo.m_strType, l_oRequest.m_oBody.m_strStaffName);

			l_vecParamList.push_back(l_strStaffName);
			l_vecParamList.push_back(l_oRequest.m_oBody.m_strPhone);
			l_vecParamList.push_back(l_oRequest.m_oBody.m_strContent);
			l_oAlarmLogInfo.m_strOperateContent = BuildAlarmLogContent(l_vecParamList);
			l_oAlarmLogInfo.m_strOperate = "BS001005015";
			l_oAlarmLogInfo.m_strCreateTime = m_pDateTime->CurrentDateTimeStr();

			if (AddAlarmLogInfo(l_oAlarmLogInfo))
			{
				SyncAlarmLogInfo(l_oAlarmLogInfo);
			}
		}

	} while (false);

	std::string l_strSendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_strSendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_strSendMsg.c_str());
}

// 新增警情附件 
void CBusinessImpl::OnNotifiAddAlarmAttachRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "OnNotifiAddAlarmAttachRequest receive message:[%s]", p_pNotify->GetMessages().c_str());
	//解析请求消息
	PROTOCOL::CNotifiAddAlarmAttachRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	//回复
	PROTOCOL::CNotifiAlarmAttachResponse l_oRespond;

	PROTOCOL::CNotifiAddAlarmAttachRequest l_oSyncRespond;

	std::string strGuid;
	std::string strTransGuid;
	bool bSuccess = false;

	do
	{
		if (l_oRequest.m_oBody.m_vecdata.empty())
		{
			l_oRespond.m_oBody.m_strcode = "400";
			l_oRespond.m_oBody.m_strmessage = "data[] empty";
			break;
		}

		strTransGuid = m_pDBConn->BeginTransaction();

		if (strTransGuid.empty())
		{
			l_oRespond.m_oBody.m_strcode = "401";
			l_oRespond.m_oBody.m_strmessage = "connection failed";
			break;
		}

		for (size_t i = 0; i < l_oRequest.m_oBody.m_vecdata.size(); i++)
		{
			strGuid = m_pString->CreateGuid();
			if (!InsertAddAlarmAttach(l_oRequest.m_oBody.m_vecdata[i], strGuid, strTransGuid))
			{
				l_oRespond.m_oBody.m_strcode = "402";
				l_oRespond.m_oBody.m_strmessage = "insert insert_icc_t_attach failed";
				break;
			}
			l_oRespond.m_oBody.m_vecdata.push_back(strGuid);

			//同步信息
			SyncAddAlarmAttach(l_oRequest.m_oBody.m_vecdata[i],strGuid, l_oSyncRespond);

		}

		bSuccess = true;

	} while (false);

	if (bSuccess)
	{
		m_pDBConn->Commit(strTransGuid);
		l_oRespond.m_oBody.m_strcode = "200";
		l_oRespond.m_oBody.m_strmessage = "success";
	}
	else if (!strTransGuid.empty())
	{
		m_pDBConn->Rollback(strTransGuid);
	}

	// vcs 新增附件同步icc
	if (l_oRequest.m_oHeader.m_strRequest == "queue_alarm")
	{
		l_oSyncRespond.m_oHeader.m_strCmd = "alarm_attach_change";
		l_oSyncRespond.m_oHeader.m_strRequest = "topic_alarm_sync";
		l_oSyncRespond.m_oHeader.m_strRequestType = "1";
		l_oSyncRespond.m_oHeader.m_strResponse = "";
		l_oSyncRespond.m_oHeader.m_strResponseType = "";
		l_oSyncRespond.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_oSyncRespond.m_oHeader.m_strRequestFlag = "MQ";
		//Content
		l_oSyncRespond.m_oBody.m_strcode = "200";
		l_oSyncRespond.m_oBody.m_strmessage = "success";
		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMessage = l_oSyncRespond.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
		ICC_LOG_DEBUG(m_pLog, "topic_alarm_sync send message:[%s]", l_strMessage.c_str());
		return;
	}

	std::string strmsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(strmsg);
	ICC_LOG_DEBUG(m_pLog, "add_alarm_attach_request send message:[%s]", strmsg.c_str());

	if (l_oRespond.m_oBody.m_strcode == "200")
	{
		// 发送同步消息
		// 附件信息同步
		// 同步给vcs
		// Header
		l_oSyncRespond.m_oHeader.m_strCmd = "syn_alarm_attach_add";
		l_oSyncRespond.m_oHeader.m_strRequest = "queue_attach_vcs";
		l_oSyncRespond.m_oHeader.m_strRequestType = "0";
		l_oSyncRespond.m_oHeader.m_strResponse = "";
		l_oSyncRespond.m_oHeader.m_strResponseType = "";
		l_oSyncRespond.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_oSyncRespond.m_oHeader.m_strRequestFlag = "MQ";
		//Content
		l_oSyncRespond.m_oBody.m_strcode = "200";
		l_oSyncRespond.m_oBody.m_strmessage = "success";

		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMessage = l_oSyncRespond.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));

		ICC_LOG_DEBUG(m_pLog, "syn_alarm_attach_add send message:[%s]", l_strMessage.c_str());
	}
}

void CBusinessImpl::SyncAddAlarmAttach(const PROTOCOL::CNotifiAddAlarmAttachRequest::data& p_pRequest, const std::string& strGuid, PROTOCOL::CNotifiAddAlarmAttachRequest& p_pResponse)
{
	p_pResponse.m_oBody.m_data.m_strId = strGuid;
	p_pResponse.m_oBody.m_data.m_stralarmId = p_pRequest.m_stralarmId;

	p_pResponse.m_oBody.m_data.m_strattachId = p_pRequest.m_strattachId;
	p_pResponse.m_oBody.m_data.m_strattachName = p_pRequest.m_strattachName;
	p_pResponse.m_oBody.m_data.m_strattachPath = p_pRequest.m_strattachPath;
	p_pResponse.m_oBody.m_data.m_strattachSource = p_pRequest.m_strattachSource;
	p_pResponse.m_oBody.m_data.m_strattachType = p_pRequest.m_strattachType;
	p_pResponse.m_oBody.m_data.m_strcreateTime = p_pRequest.m_strcreateTime;
	p_pResponse.m_oBody.m_data.m_strcreateUser = p_pRequest.m_strcreateUser;
	p_pResponse.m_oBody.m_vecdata.push_back(p_pResponse.m_oBody.m_data);
}

// 查询警情附件
void CBusinessImpl::OnNotifiSearchAlarmAttachRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "OnNotifiSearchAlarmAttachRequest receive message:[%s]", p_pNotify->GetMessages().c_str());
	//解析请求消息
	PROTOCOL::CNotifiSearchAlarmPlanRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	//回复
	PROTOCOL::CNotifiSearchAlarmAttachResponse l_oRespond;

	if (l_oRequest.m_oBody.m_stralarmId.empty())
	{
		l_oRespond.m_oBody.m_strcode = "400";
		l_oRespond.m_oBody.m_strmessage = "alarmId empty";
	}
	else if (!SearchAddAlarmAttach(l_oRequest.m_oBody.m_stralarmId, l_oRespond))
	{
		l_oRespond.m_oBody.m_strcode = "402";
		l_oRespond.m_oBody.m_strmessage = "search search_icc_t_attach failed";
	}

	std::string strmsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(strmsg);

}

// 删除警情附件
void CBusinessImpl::OnNotifiDeleteAlarmAttachRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "OnNotifiDeleteAlarmAttachRequest receive message:[%s]", p_pNotify->GetMessages().c_str());
	//解析请求消息  
	PROTOCOL::CNotifiDeleteAlarmAttachRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}
	//回复
	PROTOCOL::CNotifiAlarmAttachResponse l_oRespond;
	std::string strTransGuid;
	bool bSuccess = false;

	do
	{
		if (l_oRequest.m_oBody.m_vecIds.empty())
		{
			l_oRespond.m_oBody.m_strcode = "400";
			l_oRespond.m_oBody.m_strmessage = "Ids[] empty";
		}

		strTransGuid = m_pDBConn->BeginTransaction();

		if (strTransGuid.empty())
		{
			l_oRespond.m_oBody.m_strcode = "401";
			l_oRespond.m_oBody.m_strmessage = "connection failed";
			break;
		}

		if (!DeleteAddAlarmAttach(l_oRequest, strTransGuid))
		{
			l_oRespond.m_oBody.m_strcode = "402";
			l_oRespond.m_oBody.m_strmessage = "delete delete_icc_t_attach failed";
			break;
		}

		bSuccess = true;

	} while (false);

	if (bSuccess)
	{
		m_pDBConn->Commit(strTransGuid);
		l_oRespond.m_oBody.m_strcode = "200";
		l_oRespond.m_oBody.m_strmessage = "success";

		for (size_t i = 0; i < l_oRequest.m_oBody.m_vecIds.size(); ++i)
		{
			l_oRespond.m_oBody.m_vecdata.push_back(l_oRequest.m_oBody.m_vecIds[i]);
		}
	}
	else if (!strTransGuid.empty())
	{
		m_pDBConn->Rollback(strTransGuid);
	}

	std::string strmsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(strmsg);
	ICC_LOG_DEBUG(m_pLog, "delete_alarm_attach_request send message:[%s]", strmsg.c_str());

	if (l_oRespond.m_oBody.m_strcode == "200")
	{
		// 发送同步消息
		// 附件信息同步
		// 同步给vcs
		// Header
		l_oRespond.m_oHeader.m_strCmd = "syn_alarm_attach_delete";
		l_oRespond.m_oHeader.m_strRequest = "queue_attach_vcs";
		l_oRespond.m_oHeader.m_strRequestType = "0";
		l_oRespond.m_oHeader.m_strResponse = "";
		l_oRespond.m_oHeader.m_strResponseType = "";
		l_oRespond.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_oRespond.m_oHeader.m_strRequestFlag = "MQ";
		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMessage = l_oRespond.ToString_Body(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));

		ICC_LOG_DEBUG(m_pLog, "syn_alarm_attach_delete send message:[%s]", l_strMessage.c_str());
	
	}
}

void CBusinessImpl::OnNotifiAlarmAttachChangeRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "OnNotifiAlarmAttachChangeRequest receive message:[%s]", p_pNotify->GetMessages().c_str());
	//解析请求消息  
	PROTOCOL::CNotifiAlarmInfoChangeRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}
	//回复
	PROTOCOL::CNotifiAlarmAttachResponse l_oRespond;
	std::string l_strWeChatAlarmID;
	bool bSuccess = false;
	// source_id 查询 icc_t_jjdb 获取 jjdbh 
	if (!IsCreateWeChatAlarm(l_oRequest.m_oBody.m_strsourceId, l_strWeChatAlarmID))
	{
		l_oRespond.m_oBody.m_strcode = "400";
		l_oRespond.m_oBody.m_strmessage = "select_icc_t_jjdb jjdbh not empty ";
	}
	else
	{
		if (l_strWeChatAlarmID.empty())
		{
			l_oRespond.m_oBody.m_strcode = "401";
			l_oRespond.m_oBody.m_strmessage = "failed select icc_t_jjdb source_id not empty";
		}
		else
		{
			l_oRespond.m_oBody.m_strcode = "200";
			l_oRespond.m_oBody.m_strmessage = "success";
			l_oRespond.m_oBody.m_vecdata.push_back(l_strWeChatAlarmID);
			bSuccess = true;
		}
	}

	std::string strmsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(strmsg);
	ICC_LOG_DEBUG(m_pLog, "OnNotifiAlarmAttachChangeRequest send message:[%s]", strmsg.c_str());

	if (bSuccess)
	{
		//获取附件信息入库，并同步给前端
		WechatAlarmAttach(l_strWeChatAlarmID, false, true);
	}
}

void CBusinessImpl::OnNotifiAlarmChatInfoChangeRequest(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "OnNotifiAlarmChatInfoChangeRequest receive message:[%s]", p_pNotify->GetMessages().c_str());
	//解析请求消息  
	PROTOCOL::CNotifiAlarmInfoChangeRequest l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parse msg failed.");
		return;
	}

	//回复
	PROTOCOL::CNotifiAlarmAttachResponse l_oRespond;
	std::string l_strWeChatAlarmID;

	bool bSuccess = false;
	// source_id 查询 icc_t_jjdb 获取 jjdbh 
	if (!IsCreateWeChatAlarm(l_oRequest.m_oBody.m_strsourceId, l_strWeChatAlarmID))
	{
		l_oRespond.m_oBody.m_strcode = "400";
		l_oRespond.m_oBody.m_strmessage = "select_icc_t_jjdb jjdbh not empty ";
	}
	else
	{
		if (l_strWeChatAlarmID.empty())
		{
			l_oRespond.m_oBody.m_strcode = "401";
			l_oRespond.m_oBody.m_strmessage = "failed select icc_t_jjdb source_id not empty";
		}
		else
		{
			l_oRespond.m_oBody.m_strcode = "200";
			l_oRespond.m_oBody.m_strmessage = "success";
			l_oRespond.m_oBody.m_vecdata.push_back(l_strWeChatAlarmID);
			bSuccess = true;
		}
	}

	std::string strmsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(strmsg);
	ICC_LOG_DEBUG(m_pLog, "OnNotifiAlarmChatInfoChangeRequest send message:[%s]", strmsg.c_str());

	if (bSuccess)
	{
		// 同步给前端
		PROTOCOL::CNotifiAlarmChatinfoChangeResponse Request;
		//Header
		Request.m_oHeader.m_strCmd = "alarm_chatinfo_change";
		Request.m_oHeader.m_strRequest = "topic_alarm_sync";
		Request.m_oHeader.m_strRequestType = "1";
		Request.m_oHeader.m_strResponse = "";
		Request.m_oHeader.m_strResponseType = "";
		Request.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		//Content
		Request.m_oBody.m_strcode = "200";
		Request.m_oBody.m_strmessage = "success";
		Request.m_oBody.m_strsourceId = l_oRequest.m_oBody.m_strsourceId;
		Request.m_oBody.m_stralarmId = l_strWeChatAlarmID;
		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMessage = Request.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));
		ICC_LOG_DEBUG(m_pLog, "alarm_chatinfo_change send message:[%s]", l_strMessage.c_str());
	}
}

bool CBusinessImpl::InsertAddAlarmAttach(PROTOCOL::CNotifiAddAlarmAttachRequest::data& p_pRequest, const std::string& strGuid, const std::string& strTransGuid)
{
	DataBase::SQLRequest sqlReqeust;
	sqlReqeust.sql_id = "insert_icc_t_attach";

	sqlReqeust.param["id"] = strGuid;
	sqlReqeust.param["alarm_id"] = p_pRequest.m_stralarmId;
	sqlReqeust.param["attach_id"] = p_pRequest.m_strattachId;
	sqlReqeust.param["attach_name"] = p_pRequest.m_strattachName;
	sqlReqeust.param["attach_type"] = p_pRequest.m_strattachType;
	sqlReqeust.param["attach_path"] = p_pRequest.m_strattachPath;
	sqlReqeust.param["attach_source"] = p_pRequest.m_strattachSource;
	sqlReqeust.param["create_user"] = p_pRequest.m_strcreateUser;

	if (p_pRequest.m_strcreateTime.empty())
	{
		p_pRequest.m_strcreateTime = m_pDateTime->CurrentDateTimeStr();
	}

	sqlReqeust.param["create_time"] = p_pRequest.m_strcreateTime;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(sqlReqeust, false, strTransGuid);

	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());

	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "insert insert_icc_t_attach failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}

	return true;
}

bool CBusinessImpl::SearchAddAlarmAttach(std::string p_strAlarmID, PROTOCOL::CNotifiSearchAlarmAttachResponse& p_pResponse)
{
	DataBase::SQLRequest sqlReqeust;
	sqlReqeust.sql_id = "search_icc_t_attach";
	sqlReqeust.param["alarm_id"] = p_strAlarmID;

	DataBase::IResultSetPtr l_Result = m_pDBConn->Exec(sqlReqeust);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

	if (!l_Result->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "search_icc_t_attach error msg:[%s]",  l_Result->GetErrorMsg().c_str());
		return false;
	}
	size_t l_iRecordSize = l_Result->RecordSize();

	if (l_iRecordSize > 0)
	{
		for (size_t index = 0; index < l_iRecordSize; index++)
		{
			p_pResponse.m_oBody.m_data.m_strId = l_Result->GetValue(index, "id");
			p_pResponse.m_oBody.m_data.m_stralarmId = l_Result->GetValue(index, "alarm_id");
			p_pResponse.m_oBody.m_data.m_strattachId = l_Result->GetValue(index, "attach_id");

			p_pResponse.m_oBody.m_data.m_strattachName = l_Result->GetValue(index, "attach_name");
			p_pResponse.m_oBody.m_data.m_strattachType = l_Result->GetValue(index, "attach_type");
			p_pResponse.m_oBody.m_data.m_strattachPath = l_Result->GetValue(index, "attach_path");
			p_pResponse.m_oBody.m_data.m_strattachSource = l_Result->GetValue(index, "attach_source");
			p_pResponse.m_oBody.m_data.m_strcreateUser = l_Result->GetValue(index, "create_user");
			p_pResponse.m_oBody.m_data.m_strcreateTime = l_Result->GetValue(index, "create_time");
			p_pResponse.m_oBody.m_vecdata.push_back(p_pResponse.m_oBody.m_data);
		}
	}

	p_pResponse.m_oBody.m_strcode = "200";
	p_pResponse.m_oBody.m_strmessage = "success";

	return true;
}

bool CBusinessImpl::DeleteAddAlarmAttach(const PROTOCOL::CNotifiDeleteAlarmAttachRequest& p_pRequest, const std::string& strTransGuid)
{
	DataBase::SQLRequest l_SQLRequest;

	if (!p_pRequest.m_oBody.m_vecIds.empty())
	{
		for (size_t i = 0; i < p_pRequest.m_oBody.m_vecIds.size(); i++)
		{
			l_SQLRequest.sql_id = "delete_icc_t_attach";
			l_SQLRequest.param["id"] = p_pRequest.m_oBody.m_vecIds[i];

			DataBase::IResultSetPtr l_Result = m_pDBConn->Exec(l_SQLRequest, false, strTransGuid);
			ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_Result->GetSQL().c_str());

			if (!l_Result->IsValid())
			{
				ICC_LOG_ERROR(m_pLog, "delete_icc_t_attach id[%s],error msg:[%s]", p_pRequest.m_oBody.m_vecIds[i].c_str(), l_Result->GetErrorMsg().c_str());
				return false;
			}
		}
	}

	return true;
}

bool CBusinessImpl::UpdateAlarmAttach(const PROTOCOL::CNotifiAddAlarmAttachRequest::data& p_pRequest, const std::string& strTransGuid)
{
	DataBase::SQLRequest sqlReqeust;

	sqlReqeust.sql_id = "update_icc_t_attach";

	sqlReqeust.param["id"] = p_pRequest.m_strId;
	sqlReqeust.param["alarm_id"] = p_pRequest.m_strattachId;
	sqlReqeust.param["attach_id"] = p_pRequest.m_strattachId;

	if (!p_pRequest.m_strattachName.empty())
	{
		sqlReqeust.set["attach_name"] = p_pRequest.m_strattachName;
	}
	if (!p_pRequest.m_strattachType.empty())
	{
		sqlReqeust.set["attach_type"] = p_pRequest.m_strattachType;
	}
	if (!p_pRequest.m_strattachPath.empty())
	{
		sqlReqeust.set["attach_path"] = p_pRequest.m_strattachPath;
	}

	if (!p_pRequest.m_strattachSource.empty())
	{
		sqlReqeust.set["attach_source"] = p_pRequest.m_strattachSource;
	}
	if (!p_pRequest.m_strcreateUser.empty())
	{
		sqlReqeust.set["create_user"] = p_pRequest.m_strcreateUser;
	}
	sqlReqeust.param["create_time"] = p_pRequest.m_strcreateTime.empty() ? m_pDateTime->CurrentDateTimeStr() : p_pRequest.m_strcreateTime;

	DataBase::IResultSetPtr l_pRSet = m_pDBConn->Exec(sqlReqeust, false, strTransGuid);

	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pRSet->GetSQL().c_str());

	if (!l_pRSet->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "update update_icc_t_attach failed,error msg:[%s]", l_pRSet->GetErrorMsg().c_str());
		return false;
	}

	return true;
}

void CBusinessImpl::OnOpenTemporaryAlarm(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	// 解析请求消息
	PROTOCOL::OpenTemporaryAlarmRequst l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "add Alarm Violation Check,id=%s", l_oRequest.m_oBody.m_alarmId.c_str());
	//_AddAlarmViolationCheck(l_oRequest.m_oBody.m_alarmId.c_str());

	PROTOCOL::OpenTemporaryAlarmRespond l_oRespond;
	l_oRespond.m_oHeader.m_strResult = "0";
	l_oRespond.m_oHeader.m_strMsg = "200OK";

	std::string l_sendMsg = l_oRespond.ToString(m_pJsonFty->CreateJson());
	p_pNotify->Response(l_sendMsg);
	ICC_LOG_DEBUG(m_pLog, "send message:[%s]", l_sendMsg.c_str());
}

void CBusinessImpl::OnCloseTemporaryAlarm(ObserverPattern::INotificationPtr p_pNotify)
{
	ICC_LOG_DEBUG(m_pLog, "receive message:[%s]", p_pNotify->GetMessages().c_str());

	// 解析请求消息
	PROTOCOL::CloseTemporaryAlarmRequst l_oRequest;

	if (!l_oRequest.ParseString(p_pNotify->GetMessages(), m_pJsonFty->CreateJson()))
	{
		ICC_LOG_ERROR(m_pLog, "parser failed message:[%s]", p_pNotify->GetMessages().c_str());
		return;
	}

	ICC_LOG_DEBUG(m_pLog, "delete Alarm Violation Check,id=%s", l_oRequest.m_oBody.m_alarmId.c_str());
	_DeleteAlarmWiolationCheck(l_oRequest.m_oBody.m_alarmId);
}

void CBusinessImpl::WechatAlarmAttach(std::string p_strAlarmId, bool p_bWechatAlarmType, bool p_bAttachchange)
{
	if (p_strAlarmId.empty())
	{
		return;
	}
	//获取微信报警附件信息
	PROTOCOL::CNotifiWechatAttachRequest l_oRequest;
	std::string strWechatServerIp;
	std::string strWechatServerPort;
	do
	{
		std::string strTarget = m_pString->Format("/dealwx/getAlarmFile?jjdbh=%s", p_strAlarmId.c_str());
		std::string strErrorMessage;
		std::string strContent = "";
		std::map<std::string, std::string> mapHeaders;
		JsonParser::IJsonPtr pJson = ICCGetIJsonFactory()->CreateJson();
		mapHeaders.insert(std::make_pair("Content-Type", "application/json"));

		if (!GetServiceInfo(m_strWechatServiceName, strWechatServerIp, strWechatServerPort))
		{
			ICC_LOG_ERROR(m_pLog, "faile strWechatReceive empty strErrorMessage : %s", strErrorMessage.c_str());
			return;
		}

		//std::string strWechatReceive = m_pHttpClient->PostEx(strWechatServerIp, strWechatServerPort, strTarget, mapHeaders, strContent, strErrorMessage);
		std::string strWechatReceive = m_pHttpClient->PostWithTimeout(strWechatServerIp, strWechatServerPort, strTarget, mapHeaders, strContent, strErrorMessage, 2);
		
		ICC_LOG_DEBUG(m_pLog, "WechatAlarmAttach to strReceive: %s", strWechatReceive.c_str());

		if (strWechatReceive.empty())
		{
			ICC_LOG_ERROR(m_pLog, "faile strWechatReceive empty strErrorMessage : %s", strErrorMessage.c_str());
			return;
		}

		if (!l_oRequest.ParseString(strWechatReceive, m_pJsonFty->CreateJson()))
		{
			ICC_LOG_ERROR(m_pLog, "faile strWechatReceive ParseString ");
			return;
		}

		l_oRequest.m_oBody.m_strwechatAttachIp = strWechatServerIp;
		l_oRequest.m_oBody.m_strwechatAttachPort = strWechatServerPort;
		l_oRequest.m_oBody.m_strfileService = m_strWechatfileServiceName;

	} while (false);

	BuildWechatAlarmAttachRespond(l_oRequest, p_strAlarmId, p_bWechatAlarmType, p_bAttachchange);
}

void CBusinessImpl::BuildWechatAlarmAttachRespond(PROTOCOL::CNotifiWechatAttachRequest& p_pRespond, std::string p_strAlarmId, bool p_bWechatAlarmType, bool p_bAttachchange)
{
	if (p_pRespond.m_oBody.m_vecdata.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "vecdata empty");
		return;
	}
	//微信报警附件信息转换为icc的警情附件信息格式入库
	PROTOCOL::CNotifiAddAlarmAttachRequest Request;
	PROTOCOL::CNotifiAddAlarmAttachRequest::data results;

	for (int i = 0; i < p_pRespond.m_oBody.m_vecdata.size(); ++i)
	{
		results.m_stralarmId = p_pRespond.m_oBody.m_vecdata.at(i).m_stralarmId;
		results.m_strattachId = p_pRespond.m_oBody.m_vecdata.at(i).m_strattachId;
		results.m_strattachName = p_pRespond.m_oBody.m_vecdata.at(i).m_strattachName;
		results.m_strattachType = p_pRespond.m_oBody.m_vecdata.at(i).m_strattachType;
		results.m_strattachSource = "wx";
		results.m_strcreateUser = "wxsc";
		results.m_strattachPath = p_pRespond.m_oBody.m_vecdata.at(i).m_strattachId;

		Request.m_oBody.m_vecdata.push_back(results);
	}

	// 新增 
	if (p_bWechatAlarmType)
	{
		if (!AddWechatAlarmAttach(Request))
		{
			ICC_LOG_ERROR(m_pLog, "insert insert_icc_t_attach failed");
			return;
		}
	}
	else
	{
		// 更新
		if (!UpdateWechatAlarmAttach(Request, p_strAlarmId, p_bAttachchange))
		{
			ICC_LOG_ERROR(m_pLog, "Update Update_icc_t_attach failed");
			return;
		}
	}
}

bool CBusinessImpl::AddWechatAlarmAttach(PROTOCOL::CNotifiAddAlarmAttachRequest& p_pRequest)
{
	if (p_pRequest.m_oBody.m_vecdata.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "vecdata empty");
		return true;
	}
	bool bSuccess = false;
	std::string strGuid;
	std::string strTransGuid = m_pDBConn->BeginTransaction();

	PROTOCOL::CNotifiAddAlarmAttachRequest l_oSyncRespond;
	
	do
	{
		if (strTransGuid.empty())
		{
			ICC_LOG_ERROR(m_pLog, "connection failed");
			break;
		}

		for (size_t i = 0; i < p_pRequest.m_oBody.m_vecdata.size(); i++)
		{
			strGuid = m_pString->CreateGuid();
			if (!InsertAddAlarmAttach(p_pRequest.m_oBody.m_vecdata[i], strGuid, strTransGuid))
			{
				ICC_LOG_ERROR(m_pLog, "insert insert_icc_t_attach failed");
				break;
			}

			//同步信息
			SyncAddAlarmAttach(p_pRequest.m_oBody.m_vecdata[i], strGuid, l_oSyncRespond);

		}
		bSuccess = true;

	} while (false);

	if (bSuccess)
	{
		m_pDBConn->Commit(strTransGuid);
		// 发送同步消息
		// 附件信息同步
		// 同步给vcs
		// Header
		l_oSyncRespond.m_oHeader.m_strCmd = "syn_alarm_attach_add";
		l_oSyncRespond.m_oHeader.m_strRequest = "queue_attach_vcs";
		l_oSyncRespond.m_oHeader.m_strRequestType = "0";
		l_oSyncRespond.m_oHeader.m_strResponse = "";
		l_oSyncRespond.m_oHeader.m_strResponseType = "";
		l_oSyncRespond.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		l_oSyncRespond.m_oHeader.m_strRequestFlag = "MQ";
		//Content
		l_oSyncRespond.m_oBody.m_strcode = "200";
		l_oSyncRespond.m_oBody.m_strmessage = "success";

		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMessage = l_oSyncRespond.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));

		ICC_LOG_DEBUG(m_pLog, "alarm_attach_change send message:[%s]", l_strMessage.c_str());
	}
	else if (!strTransGuid.empty())
	{
		m_pDBConn->Rollback(strTransGuid);
	}

	return bSuccess;
}

bool CBusinessImpl::UpdateWechatAlarmAttach(PROTOCOL::CNotifiAddAlarmAttachRequest& p_pRequest, std::string p_strAlarmId, bool p_bAttachchange)
{

	if (p_pRequest.m_oBody.m_vecdata.empty() || p_strAlarmId.empty())
	{
		ICC_LOG_DEBUG(m_pLog, "vecdata or p_strAlarmId empty");
		return false;
	}

	// 先根据 警情id alarm_id  和 警情附件id attach_id  查询附件表是否存在附件
	// 附件不存在，插入

	// 查询结果
	PROTOCOL::CNotifiSearchAlarmAttachResponse l_oRespond;

	if (!SearchAddAlarmAttach(p_strAlarmId, l_oRespond))
	{
		ICC_LOG_ERROR(m_pLog, "search_icc_t_attach  faile");
	}
	//成功值
	bool bSuccess = false;
	bool bequal = false;
	//已存在不入库
	bool bupdateSuccess = false;

	// 发送同步消息
	PROTOCOL::CNotifiSearchAlarmAttachResponse Request;
	PROTOCOL::CNotifiSearchAlarmAttachResponse::data results;

	std::string strGuid;
	std::string strTransGuid = m_pDBConn->BeginTransaction();
	// 结果比较, 不存在插入
	for (int i = 0; i < p_pRequest.m_oBody.m_vecdata.size(); i++)
	{
		for (int j = 0; j < l_oRespond.m_oBody.m_vecdata.size(); j++)
		{
			if (p_pRequest.m_oBody.m_vecdata.at(i).m_stralarmId == l_oRespond.m_oBody.m_vecdata.at(i).m_stralarmId
				&& p_pRequest.m_oBody.m_vecdata.at(i).m_strattachId == l_oRespond.m_oBody.m_vecdata.at(i).m_strattachId)
			{
				//if (!UpdateAlarmAttach(p_pRequest.m_oBody.m_vecdata[i], strTransGuid))
				//{
				//	bSuccess = false;
				//	bequal = true;
				//}
				//else
				{
					bSuccess = true;
					bupdateSuccess = true;
				}
				break;
			}
		}

		if (bequal)
		{
			bSuccess = false;
			break;
		}

		if (bupdateSuccess)
		{
			continue;
		}

		// 新增附件，入库
		strGuid = m_pString->CreateGuid();
		if (!InsertAddAlarmAttach(p_pRequest.m_oBody.m_vecdata[i], strGuid, strTransGuid))
		{
			ICC_LOG_ERROR(m_pLog, "insert insert_icc_t_attach failed");
			bSuccess = false;
			break;
		}

		if (p_bAttachchange)
		{
			results.m_strId = strGuid;
			results.m_stralarmId = p_pRequest.m_oBody.m_vecdata.at(i).m_stralarmId;
			results.m_strattachId = p_pRequest.m_oBody.m_vecdata.at(i).m_strattachId;
			results.m_strattachName = p_pRequest.m_oBody.m_vecdata.at(i).m_strattachName;
			results.m_strattachType = p_pRequest.m_oBody.m_vecdata.at(i).m_strattachType;
			results.m_strattachSource = p_pRequest.m_oBody.m_vecdata.at(i).m_strattachSource;
			results.m_strcreateUser = p_pRequest.m_oBody.m_vecdata.at(i).m_strcreateUser;
			results.m_strattachPath = p_pRequest.m_oBody.m_vecdata.at(i).m_strattachPath;
			results.m_strcreateTime = p_pRequest.m_oBody.m_vecdata.at(i).m_strcreateTime;
			Request.m_oBody.m_vecdata.push_back(results);
		}

		bSuccess = true;
	}

	if (bSuccess)
	{
		m_pDBConn->Commit(strTransGuid);
	}
	else if (!strTransGuid.empty())
	{
		m_pDBConn->Rollback(strTransGuid);
	}

	// 入库后通知前端更新
	if (bSuccess && p_bAttachchange)
	{
		// 同步附件信息
		// 同步给icc
		// Header
		Request.m_oHeader.m_strCmd = "alarm_attach_change";
		Request.m_oHeader.m_strRequest = "topic_alarm_sync";
		Request.m_oHeader.m_strRequestType = "1";
		Request.m_oHeader.m_strResponse = "";
		Request.m_oHeader.m_strResponseType = "";
		Request.m_oHeader.m_strMsgid = m_pString->CreateGuid();
		Request.m_oHeader.m_strRequestFlag = "MQ";
		//Content
		Request.m_oBody.m_strcode = "200";
		Request.m_oBody.m_strmessage = "success";
		JsonParser::IJsonPtr l_pIJson = ICCGetIJsonFactory()->CreateJson();
		std::string l_strMessage = Request.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMessage));

		// 同步给vcs
		Request.m_oHeader.m_strCmd = "syn_alarm_attach_add";
		
		std::string l_strSynMessage = Request.ToString(l_pIJson);
		m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strSynMessage));

		ICC_LOG_DEBUG(m_pLog, "alarm_attach_change send message:[%s]", l_strMessage.c_str());
	}

	return bSuccess;
}

/*****************************************************************
 * 
 ****************************************************************/
unsigned int CBusinessImpl::_GetAlarmId(const std::string& strAlarmId)
{
	if (strAlarmId.size() > 6)
	{
		std::string strTmp = strAlarmId.substr(strAlarmId.size() - 6, 6);
		return m_pString->ToUInt(strTmp);
	}	

	return 0;
}

void CBusinessImpl::_CreateThreads()
{
	m_strDispatchMode = m_pConfig->GetValue("ICC/Plugin/Synthetical/procthreaddispatchmode", "1");

	//创建通用线程
	unsigned int uProcThreadCount = m_pString->ToUInt(m_pConfig->GetValue("ICC/Plugin/Synthetical/procthreadcount", "8"));
	boost::thread h1;
	unsigned int uConCurrency = h1.hardware_concurrency();
	if (uProcThreadCount > uConCurrency)
	{
		uProcThreadCount = uConCurrency;
	}

	ICC_LOG_INFO(m_pLog, "synthetical common proc thread count:%d, mode:%s", uProcThreadCount, m_strDispatchMode.c_str());

	for (int i = 0; i < uProcThreadCount; ++i)
	{
		CommonWorkThreadPtr pThread = boost::make_shared<CCommonWorkThread>();
		if (pThread)
		{
			pThread->SetLogPtr(m_pLog);
			m_vecProcThreads.push_back(pThread);
		}
		
	}

	m_uProcThreadCount = m_vecProcThreads.size();

	ICC_LOG_INFO(m_pLog, "synthetical common real proc thread count:%d", m_uProcThreadCount);

	//创建警单增加与更新线程
	uProcThreadCount = m_pString->ToUInt(m_pConfig->GetValue("ICC/Plugin/Synthetical/alarmprocthreadcount", "8"));	
	if (uProcThreadCount > uConCurrency)
	{
		uProcThreadCount = uConCurrency;
	}

	ICC_LOG_INFO(m_pLog, "synthetical alarm proc thread count:%d", uProcThreadCount);

	for (int i = 0; i < uProcThreadCount; ++i)
	{
		CommonWorkThreadPtr pThread = boost::make_shared<CCommonWorkThread>();
		if (pThread)
		{
			pThread->SetLogPtr(m_pLog);
			m_vecAlarmProcThreads.push_back(pThread);
		}
		
	}

	m_uProcAlarmThreadCount = m_vecAlarmProcThreads.size();

	ICC_LOG_INFO(m_pLog, "synthetical alarm real proc thread count:%d", m_uProcAlarmThreadCount);

}

void CBusinessImpl::_DestoryThreads()
{
	//释放通用线程
	for (int i = 0; i < m_uProcThreadCount; ++i)
	{
		m_vecProcThreads[i]->Stop(0);
	}
	m_vecProcThreads.clear();

	//
	for (int i = 0; i < m_uProcAlarmThreadCount; ++i)
	{
		m_vecAlarmProcThreads[i]->Stop(0);
	}
	m_vecAlarmProcThreads.clear();
}

CommonWorkThreadPtr CBusinessImpl::_GetThread()
{
	int iIndex = 0;
	if (m_strDispatchMode == "1")//轮询模式
	{
		//std::lock_guard<std::mutex> lock(m_mutexThread);
		if (m_uCurrentThreadIndex >= m_uProcThreadCount)
		{
			m_uCurrentThreadIndex = 0;
		}
		iIndex = m_uCurrentThreadIndex;
		m_uCurrentThreadIndex++;
	}
	else//最小消息模式
	{
		//std::lock_guard<std::mutex> lock(m_mutexThread);
		int iMessageCount = 0;
		for (int i = 0; i < m_uProcThreadCount; ++i)
		{
			int iTmpMessageCount = m_vecProcThreads[i]->GetCWorkThreadListCount();
			if (iTmpMessageCount == 0)
			{
				iIndex = i;
				break;
			}

			if (iMessageCount == 0)
			{
				iIndex = i;
				iMessageCount = iTmpMessageCount;
			}
			else if (iMessageCount > iTmpMessageCount)
			{
				iIndex = i;
				iMessageCount = iTmpMessageCount;
			}
		}
	}

	ICC_LOG_LOWDEBUG(m_pLog, "_GetThread complete! index:%d", iIndex);

	return m_vecProcThreads[iIndex];
}

CommonWorkThreadPtr CBusinessImpl::_GetThreadEx(int iAlarmId)
{
	int iIndex = 0;

	if (iAlarmId == 0)
	{		
		int iMessageCount = 0;
		//std::lock_guard<std::mutex> lock(m_mutexAlarmThread);		
		for (int i = 0; i < m_uProcAlarmThreadCount; ++i)
		{
			int iTmpMessageCount = m_vecAlarmProcThreads[i]->GetCWorkThreadListCount();
			if (iTmpMessageCount == 0)
			{
				iIndex = i;
				break;
			}

			if (iMessageCount == 0)
			{
				iIndex = i;
				iMessageCount = iTmpMessageCount;
			}
			else if (iMessageCount > iTmpMessageCount)
			{
				iIndex = i;
				iMessageCount = iTmpMessageCount;
			}
		}
	}
	else
	{
		//std::lock_guard<std::mutex> lock(m_mutexAlarmThread);
		iIndex = iAlarmId % m_uProcAlarmThreadCount;		
	}	

	ICC_LOG_LOWDEBUG(m_pLog, "_GetThreadEx complete! index:%d", iIndex);

	return m_vecAlarmProcThreads[iIndex];
}

void CBusinessImpl::_OnReceiveNotify(ObserverPattern::INotificationPtr p_pNotifiReqeust)
{
	ICC_LOG_LOWDEBUG(m_pLog, "_OnReceiveNotify enter! cmd:%s,guid:%s, ", p_pNotifiReqeust->GetCmdName().c_str(), p_pNotifiReqeust->GetCmdGuid().c_str());

	CommonWorkThreadPtr pThread = _GetThread();
	if (pThread)
	{
		pThread->AddMessage(this, THREADID_RECEIVECOMMONNOTIFY, 0, 0, 0, p_pNotifiReqeust->GetCmdName(), "", p_pNotifiReqeust);
		ICC_LOG_LOWDEBUG(m_pLog, "_OnReceiveNotify complete! cmd:%s,guid:%s, threadid:%u, message count:%u",
			p_pNotifiReqeust->GetCmdName().c_str(), p_pNotifiReqeust->GetCmdGuid().c_str(), pThread->GetThreadId(), pThread->GetCWorkThreadListCount());
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "_OnReceiveNotify failed! cmd:%s,guid:%s", p_pNotifiReqeust->GetCmdName().c_str(), p_pNotifiReqeust->GetCmdGuid().c_str());
	}
}

void CBusinessImpl::_DispatchNotify(ObserverPattern::INotificationPtr p_pNotifiReqeust, const std::string& strCmdName)
{
	ICC_LOG_LOWDEBUG(m_pLog, "_DispatchNotify begin! cmd:%s", strCmdName.c_str());

	std::map<std::string, ProcNotify>::const_iterator itr;
	itr = m_mapFuncs.find(strCmdName);
	if (itr != m_mapFuncs.end())
	{
		ICC_LOG_LOWDEBUG(m_pLog, "_DispatchNotify enter! cmd:%s", strCmdName.c_str());
		(this->*itr->second)(p_pNotifiReqeust);
	}

	ICC_LOG_LOWDEBUG(m_pLog, "_DispatchNotify begin! cmd:%s", strCmdName.c_str());
}

void CBusinessImpl::_DispatchAlarmNotify(ICC::PROTOCOL::AddOrUpdateAlarmWithProcessRequestPtr pAlarmRequestPtr, ObserverPattern::INotificationPtr p_pNotify, const std::string& strCmdName)
{
	std::map<std::string, ProcAlarmNotify>::const_iterator itr;
	itr = m_mapAlarmFuncs.find(strCmdName);
	if (itr != m_mapAlarmFuncs.end())
	{
		(this->*itr->second)(pAlarmRequestPtr, p_pNotify);
	}
}

void CBusinessImpl::ProcMessage(CommonThread_data msg_data)
{
	switch (msg_data.msg_id)
	{
	case THREADID_RECEIVECOMMONNOTIFY:
	{
		try
		{
			std::chrono::system_clock::time_point curBegin;
			curBegin = std::chrono::system_clock::now();

			ICC_LOG_LOWDEBUG(m_pLog, "synthetical THREADID_RECEIVENOTIFY %u begin! cmd:%s, guid:%s",
				msg_data.uThreadId, msg_data.str_msg.c_str(), msg_data.strExt.c_str());

			_DispatchNotify(msg_data.pNotifyPtr, msg_data.str_msg);

			std::chrono::system_clock::time_point curEnd;
			curEnd = std::chrono::system_clock::now();
			unsigned long long ullInterval = std::chrono::duration_cast<std::chrono::microseconds>(curEnd.time_since_epoch()).count() - std::chrono::duration_cast<std::chrono::microseconds>(curBegin.time_since_epoch()).count();


			ICC_LOG_LOWDEBUG(m_pLog, "synthetical THREADID_RECEIVENOTIFY %u enf! cmd:%s, guid:%s, exec time:%lld",
				msg_data.uThreadId, msg_data.str_msg.c_str(), msg_data.strExt.c_str(), ullInterval);

			if (ullInterval > TIMOUT_INTERVAL)
			{
				ICC_LOG_WARNING(m_pLog, "exec slow! threadid:%u,time:%llu, message:%s", msg_data.uThreadId,ullInterval, msg_data.pNotifyPtr->GetMessages().c_str());
			}

		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "THREADID_RECEIVENOTIFY exception!");
		}
	}
	break;
	case THREADID_RECEIVEALARMNOTIFY:
	{
		try
		{
			std::chrono::system_clock::time_point curBegin;
			curBegin = std::chrono::system_clock::now();

			ICC_LOG_LOWDEBUG(m_pLog, "THREADID_RECEIVEALARMNOTIFY begin! cmd:%s,guid:%s", msg_data.str_msg.c_str(), msg_data.pNotifyPtr->GetCmdGuid().c_str());
			_DispatchAlarmNotify(msg_data.pAlarmRequestPtr, msg_data.pNotifyPtr,msg_data.str_msg);

			std::chrono::system_clock::time_point curEnd;
			curEnd = std::chrono::system_clock::now();
			unsigned long long ullInterval = std::chrono::duration_cast<std::chrono::microseconds>(curEnd.time_since_epoch()).count() - std::chrono::duration_cast<std::chrono::microseconds>(curBegin.time_since_epoch()).count();


			ICC_LOG_LOWDEBUG(m_pLog, "THREADID_RECEIVEALARMNOTIFY end! cmd:%s,guid:%s,time:%llu", msg_data.str_msg.c_str(), msg_data.pNotifyPtr->GetCmdGuid().c_str(),ullInterval);
			if (ullInterval > TIMOUT_INTERVAL)
			{
				ICC_LOG_WARNING(m_pLog, "exec slow! time:%llu, message:%s", ullInterval, msg_data.pNotifyPtr->GetMessages().c_str());
			}
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "THREADID_RECEIVEALARMNOTIFY exception!");
		}
	}
	break;
	case ThreadId_Violation:
	{
		try
		{
			m_bViolationIsRunning = true;
			ICC_LOG_DEBUG(m_pLog, "ThreadId_Violation begin!");
			_AlarmViolationCheckProcess();
			ICC_LOG_DEBUG(m_pLog, "ThreadId_Violation end!");
			m_bViolationIsRunning = false;
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "ThreadId_Violation exception!");
		}
	}
	break;
	default:
		break;
	}
}

void CBusinessImpl::GetCallMessage(PROTOCOL::CGetAlarmLogRespond::CData l_oData, std::string &strType,std::string &strSourceID)
{
	JsonParser::IJsonPtr l_pIJson = m_pJsonFty->CreateJson();
	l_pIJson->LoadJson(l_oData.m_strOperateAttachDesc);

	std::string l_strPath = "/";
	std::string l_strNum = std::to_string(0);
	std::string strTypePath = l_strPath + l_strNum + "/type";
	strType = l_pIJson->GetNodeValue(strTypePath, " ");
	if (strType == "jjlyh" || strType == "call")
	{
		std::string strSourcePath = l_strPath + l_strNum + "/id";
		strSourceID = l_pIJson->GetNodeValue(strSourcePath, " ");
	}

}

bool CBusinessImpl::GetRecordID(std::string strSourceID,PROTOCOL::CGetAlarmLogRespond::CData &l_oData)
{
	DataBase::SQLRequest l_oSeleteCallEventSQLReq;
	l_oSeleteCallEventSQLReq.sql_id = "select_icc_t_callevent";
	std::string strCallRefIdTime;
	if (!strSourceID.empty())
	{
		strCallRefIdTime = m_pDateTime->GetCallRefIdTime(strSourceID);
		l_oSeleteCallEventSQLReq.param["callref_id"] = strSourceID;
	}

	if (!strCallRefIdTime.empty())
	{
		l_oSeleteCallEventSQLReq.param["create_time_begin"] = m_pDateTime->GetFrontTime(strCallRefIdTime, 2 * 86400);
		l_oSeleteCallEventSQLReq.param["create_time_end"] = m_pDateTime->GetAfterTime(strCallRefIdTime, 2 * 86400);
	}
	DataBase::IResultSetPtr l_pResult = m_pDBConn->Exec(l_oSeleteCallEventSQLReq, true);
	ICC_LOG_DEBUG(m_pLog, "sql:[%s]", l_pResult->GetSQL().c_str());
	if (!l_pResult->IsValid())
	{
		ICC_LOG_ERROR(m_pLog, "get record file id failed, [%s]", l_pResult->GetErrorMsg().c_str());
		return false;
	}
	if (l_pResult->Next())
	{
		l_oData.m_strRecordFileID = l_pResult->GetValue("record_file_id");
	}
	return true;
}

bool ICC::CBusinessImpl::GetDeptInfo(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& p_oAddSyntRequest)
{
	std::string strDeptInfo;
	if (!m_pRedisClient->HGet("DeptCodeInfoKey", p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptDeptCode, strDeptInfo))
	{
		ICC_LOG_DEBUG(m_pLog, "Hget Dept Info Failed!!!");
		return false;
	}
	PROTOCOL::CDeptInfo l_DeptInfo;
	if (!l_DeptInfo.Parse(strDeptInfo, m_pJsonFty->CreateJson()))
	{
		ICC_LOG_DEBUG(m_pLog, "Parse Dept Info Failed!!!");
		return false;
	}
	p_oAddSyntRequest.m_oBody.m_oAlarm.m_strReceiptDeptDistrictCode = l_DeptInfo.m_strDistrictCode;
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
	else if(m_strCodeMode == STAFF_CODE)
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

