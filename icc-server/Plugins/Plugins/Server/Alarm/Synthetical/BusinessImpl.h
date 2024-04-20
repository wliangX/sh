#pragma once
#include "Define.h"
#include "CommonWorkThread.h"

namespace ICC
{
	//同步状态
	enum ESyncType
	{
		ADD = 1,
		SUBMIT,
		DEL,   //服务端不适用，同步状态禁止使用此状态  [1/29/2019 w16314]
		EDIT
	};

	typedef struct MsgCenterListNode
	{
		DateTime::CDateTime lastExecTime;
		std::string		    sendMsg;
		std::string		    sendTar;
		int					sendCount;
	}MsgCenterListNode;
	//////////////////////////////////////////////////////////////////////////
	class CBusinessImpl; 	
	typedef void (CBusinessImpl::* ProcNotify)(ObserverPattern::INotificationPtr p_pNotifiRequest);
	typedef void (CBusinessImpl::* ProcAlarmNotify)(ICC::PROTOCOL::AddOrUpdateAlarmWithProcessRequestPtr pAlarmRequestPtr, ObserverPattern::INotificationPtr p_pNotify);

	class CBusinessImpl : public CBusinessBase, CCommonThreadCore
	{
	public:
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();

	public:		
		void OnCNotifiAddAlarmRequest(ObserverPattern::INotificationPtr p_pNotify);						// ICC客户端新建警单	

		void OnReviveAlarmRequest(ObserverPattern::INotificationPtr p_pNotify);							//ICC复活警单
		void OnSetAlarmCallrefRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnCNotifiDeleteAlarmCallrefRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnCNotifiAddOrUpdateAlarmAndProcessRequest(ObserverPattern::INotificationPtr p_pNotify);	// 新增或更新接处警信息		
		void OnCNotifiAddOrUpdateProcessRequest(ObserverPattern::INotificationPtr p_pNotify);			// 新增或更新处警信息

		void ModifyAddOrUpdateLinkedDisPatchRequest(ObserverPattern::INotificationPtr p_pNotify);			// 新增或更新联动单位 ICC
		void OnCNotifiAddOrUpdateLinkedDisPatchRequest(ObserverPattern::INotificationPtr p_pNotify);		// 新增或更新联动单位 VCS
		bool AddLinkedProcessInfo(PROTOCOL::CAddOrUpdateLinkedRequest::CLinkedData& l_oRequestInfo);				// 新增联动单位 ICC
		bool UpdateLinkedProcessInfo(PROTOCOL::CAddOrUpdateLinkedRequest::CLinkedData& l_oRequestInfo);			// 更新联动单位 ICC
		void SyncLinkedInfo(const PROTOCOL::CAddOrUpdateLinkedRequest::CLinkedData p_rLinkedInfoSync,
			std::string strLinkedCmd, std::string strLinkedRequest, ESyncType p_iSyncType, std::string p_strReleatedID = "");	// 联动单位发送信息到 消息中心
		void CreateLinkedLog(PROTOCOL::CAddOrUpdateLinkedRequest::CLinkedData& l_oRequestInfo, const std::string& strOption); // 记录联动单位流水
		std::string GetCurrentLinkedState(const std::string& p_strID);									// 根据ID获取联动单位表的状态
		bool AddLinkedInfoFromVCS(PROTOCOL::CAddOrUpdateProcessRequest::CLinkedData& l_oRequestInfo);		// 新增联动单位 vcs
		bool UpdateLinkedInfoFromVCS(PROTOCOL::CAddOrUpdateProcessRequest::CLinkedData& l_oRequestInfo);	// 更新联动单位 vcs
		void BuildLinkedDataInfo(PROTOCOL::CAddOrUpdateLinkedRequest::CLinkedData& l_oVecData);				// 构建联动单位信息
		std::string GetDeptDistrictCode(const std::string strAlarmId);										// 通过警情ID获取部门编码
		void OnCNotifiAddWeChatAlarmRequest(ObserverPattern::INotificationPtr p_pNotify);					// 新增微信 报警	
		bool IsCreateWeChatAlarm(const std::string& strAlarmId, std::string& l_strWeChatAlarmID);			// 是否为新增微信报警
		std::string SearchReceiptDeptDistrictCodeByCode(const std::string& m_strReceiptDeptCode);			// 根据接警单位查询行政区划代码
		void BuildWeChatDataInfo(PROTOCOL::AddOrUpdateAlarmWithProcessRequestPtr l_oRequest);				// 构建微信报警信息
		bool SyncVehicleNoInfo(PROTOCOL::AddOrUpdateAlarmWithProcessRequestPtr l_oRequest);				// 把微信报警的车辆信息更新到车辆表
		void OnCNotifiAddAlarmSignRequest(ObserverPattern::INotificationPtr p_pNotify);					// 新增警情签收		
		void OnCNotifiGetAlarmSignRequest(ObserverPattern::INotificationPtr p_pNotify);					// 获取一个单位下所有未签收的警单
		
		void OnCNotifiAddAlarmRemarkRequest(ObserverPattern::INotificationPtr p_pNotify);				// 新增备注		
		void OnCNotifiGetAlarmRemarkRequest(ObserverPattern::INotificationPtr p_pNotify);				// 获取备注		
		void OnCNotifiGetAlarmAllRemarkRequest(ObserverPattern::INotificationPtr p_pNotify);			// 获取所有备注
		
		void OnCNotifiGetAlarmLogRequest(ObserverPattern::INotificationPtr p_pNotify);					// 获取警情流水		
		void OnCNotifiGetAlarmLogAlarmRequest(ObserverPattern::INotificationPtr p_pNotify);				// 获取流水信息中接警资源信息		
		void OnCNotifiGetAlarmLogProcessRequest(ObserverPattern::INotificationPtr p_pNotify);			// 获取流水信息中处警资源信息		
		void OnCNotifiGetAlarmLogCallrefRequest(ObserverPattern::INotificationPtr p_pNotify);			// 获取流水信息中话务资源信息
		//void OnCNotifiGetAlarmInvolvedLogRequest(ObserverPattern::INotificationPtr p_pNotify);			// 获取流水信息中警情关联资源信息

		void OnCNotifiMergeAlarmRequest(ObserverPattern::INotificationPtr p_pNotify);					// 新增合并警单		
		void OnCNotifiUnMergeRequest(ObserverPattern::INotificationPtr p_pNotify);						// 取消合并警单
		void OnCNotifiGetMergeAlarmRequest(ObserverPattern::INotificationPtr p_pNotify);				// 获取合并警单		
		void OnCNotifiGetAlarmBysourceIDRequest(ObserverPattern::INotificationPtr p_pNotify);			// 获取警情id

		void OnCNotifiAlarmVisitRequest(ObserverPattern::INotificationPtr p_pNotify);					// 新增或编辑回访单
		void OnCNotifiSearchAlarmVisitRequest(ObserverPattern::INotificationPtr p_pNotify);				// 回访单信息查询
		void OnCNotifiSearchAlarmVisitStatisticRequest(ObserverPattern::INotificationPtr p_pNotify);	// 回访统计信息查询

		void OnCNotifiUpdateCalleventFlagRequest(ObserverPattern::INotificationPtr p_pNotify);

		void OnCNotifiGetAllKeyWordAlarmRequest(ObserverPattern::INotificationPtr p_pNotify);

		void OnCNotifuGetBlackMessageSendRequest(ObserverPattern::INotificationPtr p_pNotify);			// 黑名单信息发送

		void OnAlarmRelatedCarsAddExRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnAlarmRelatedCarsQueryExRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnAlarmRelatedCarsDeleteExRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnAlarmRelatedCarsUpdateExRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnAlarmRelatedCarsSearchExRequest(ObserverPattern::INotificationPtr p_pNotify);

		void OnAlarmRelatedPersonsAddExRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnAlarmRelatedPersonsQueryExRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnAlarmRelatedPersonsDeleteExRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnAlarmRelatedPersonsUpdateExRequest(ObserverPattern::INotificationPtr p_pNotify);
		void OnAlarmRelatedPersonsSearchExRequest(ObserverPattern::INotificationPtr p_pNotify);

		void OnAlarmDeleteRequest(ObserverPattern::INotificationPtr p_pNotify);  //警情取消

		void OnGetAlarmIDRequest(ObserverPattern::INotificationPtr p_pNotify); //获取警情ID， VCS专用

		void OnAlarmViolationCheckTimer(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void OnUpdateAlarmGpsRequest(ObserverPattern::INotificationPtr p_pNotifiRequest);

		void OnReceiveSynNacosParams(ObserverPattern::INotificationPtr p_pNotifiRequest);

		//新增打开暂存警单部分
		void OnOpenTemporaryAlarm(ObserverPattern::INotificationPtr p_pNotify);
		void OnCloseTemporaryAlarm(ObserverPattern::INotificationPtr p_pNotify);

		// 新增 短信发送 接口
		void OnNotifiPostSmsSend(ObserverPattern::INotificationPtr p_pNotify);
		// 获取流水更改详细信息
		void OnNotifiGetAlarmLogInfo(ObserverPattern::INotificationPtr p_pNotify);
	
		// 新增警情附件 
		void OnNotifiAddAlarmAttachRequest(ObserverPattern::INotificationPtr p_pNotify);
		// 查询警情附件
		void OnNotifiSearchAlarmAttachRequest(ObserverPattern::INotificationPtr p_pNotify);
		// 删除警情附件
		void OnNotifiDeleteAlarmAttachRequest(ObserverPattern::INotificationPtr p_pNotify);
		// 警情附件变更
		void OnNotifiAlarmAttachChangeRequest(ObserverPattern::INotificationPtr p_pNotify);	
		// 聊天记录变更
		void OnNotifiAlarmChatInfoChangeRequest(ObserverPattern::INotificationPtr p_pNotify);

		void OnReceiveCallOver(ObserverPattern::INotificationPtr p_pNotify);
		
	private:
		bool _UpdateJJWCTime(const std::string& strCallId, const std::string& strHangupTime);
		bool _QueryAlarmInfoBySourceId(const std::string& strSourceId, PROTOCOL::CAlarmInfo& alarmInfo);

		bool IsSyntInfoValid(const PROTOCOL::CAddOrUpdateAlarmWithProcessRequest::CBody& p_pAlarmSyntInfo);

		bool IsNewAlarm(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& p_oRequest);

		void ModifyAddAlarmRequest(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& p_oAddSyntRequest);
		
		void _ModifyBJSJTime(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& p_oAddSyntRequest);
		void ModifyAddSyntRequest(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& p_oAddSyntRequest);
		void ModifyUpdateSyntRequest(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& p_oUpdateSyntRequest, const std::string& strTransGuid);

		bool IsHaveNewProcess(const PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& p_oData);

		void SendAlarmCarsChangeSync(const PROTOCOL::CHeaderEx& p_pRequestHeader,const PROTOCOL::CAlarmRelatedCarsInfoEx& p_CAlarmCarsInfoEx, const std::string& p_strSyncType);
		void SendAlarmPersonsChangeSync(const PROTOCOL::CHeaderEx& p_oHead,const std::map<std::string, std::string>& p_mapData, const std::string& p_strSyncType);

		//消息中心部分
		void SendMessageToMsgCenterOnAlarmChange(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest l_oRequest, std::string l_strMsg);
		void SendMessageToMsgCenterOnAlarmTypeChange(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest l_oRequest, std::string l_strMsg);
		void SendMessageToMsgCenterOnMajorAlarm(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest l_oRequest, std::string l_strMsg);
		void SendMessageToMsgCenterOnAlarmOverTime(std::string l_strSeatNo, std::string l_strMsg, std::string l_strName, std::string l_strDeptCode);

		void AlarmRelatedPersonsChange(ObserverPattern::INotificationPtr p_pNotify, 
			const PROTOCOL::CHeaderEx &p_oHead,
			const std::string& p_strSyncType,
			const std::string &p_strGUID,
			std::map<std::string, std::string> &p_mapData);
	private:
		bool AddSyntInfo(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& p_oAddSyntRequest, const std::string& strTransGuid);
		bool UpdateSyntInfo(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& l_oAddAlarmSyntRequest, const std::string& strTransGuid, int& iModifyField, std::string& strGuidId);
		bool _InsertBackReason(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& l_oUpdateProcessInfo);
		bool AddAlarmLogInfo(PROTOCOL::CAlarmLogSync::CBody& p_pAlarmLogInfo, const std::string& strTransGuid = "");
		void GetStaffDept(std::string p_strUserCode, std::string &p_strStaffName, std::string &p_strDeptName, std::string &p_strDeptCode);
		void GetStaffDept2(std::string p_strUserName, std::string &p_strUserCode, std::string &p_strDeptName, std::string &p_strDeptCode);
		bool _UpdateVisitCallType(const std::string& strAlarmId, const std::string& strCallNoType, const std::string& strTransGuid);
		
		bool _SetAlarmCallref(const std::string& p_strAlarmID, const std::string& p_strCallrefID);
		bool _DeleteAlarmCallref(const std::string& p_strAlarmID, const std::string& p_strCallrefID);
		bool _UpdateReviveInfo(PROTOCOL::CReviveAlarmRequest::CBody &p_oReviveAlarmBody, std::string& strTransGuid, std::string &p_strErrMsg);  //警情复活

	    // 根据 rel_alarm_id jjdbh 在 icc_t_jjdb 中查询 jjlyh 是否为空 ;
		bool _SelectAlarmIdToSourceId(const std::string& p_strAlarmId, const std::string& p_strTransGuid, bool& p_bSuccess);
		bool _UpdateAlarmIdToSourceId(const std::string& p_strAlarmId, const std::string& p_strSourceId, const std::string& p_strTransGuid, bool& p_bSuccess);
		bool _UpdateSourceId(const std::string& p_strSourceId, const std::string& p_strTransGuid, bool& p_bSuccess);
		
		bool _SelectAlarmIdToFirstType(const std::string& p_strAlarmId);
		std::string _QueryAlarmRelatedCarsCreateTime(const std::string& p_strID);
		/*同步关键告警信息
		p1: 告警ID
		p2:告警内容
		返回是否需要更新警情状态*/
		bool _SyncKeyWordAlarmInfo(const PROTOCOL::CAlarmInfo &in_oAlarm, bool in_bModifyContent);

		/*根据告警ID，查询*/
		bool _QueryKeyWordAlarmInfoByAlarmID(const std::string& in_strAlarmID, std::vector<PROTOCOL::CKeyWordAlarm> &out_vecKeyWordAlarm);

		/*插入敏感警情*/
		void _InsertKeyWordAlarm(const PROTOCOL::CKeyWordAlarmSync&in_oKeyWordAlarm, const std::string &in_strState);

		//更新敏感警情状态
		void _UpdateKeyWordAlarmState(const std::string& strAlarmID, const std::string &strState);

		/*根据alarmid删除*/
		void _DelKeyWordAlarmInfoByAlarmID(const std::string& in_strAlarmID);

		bool _GetAllKeyWordAlarmRecords(const PROTOCOL::CKeyWordAlarmQryRequest& in_oReq, PROTOCOL::CKeyWordAlarmQryRespond& out_oRes);
	private:
		// 构建响应消息 
		void BuildRespondHeader(std::string p_strCmd, const PROTOCOL::CHeaderEx& p_pRequestHeader, PROTOCOL::CHeaderEx& p_pRespHeader);
		void BuildRespondHeader(std::string p_strCmd, PROTOCOL::CHeader& p_oSyncHeader);

		bool BuildAddAlarmRespond(PROTOCOL::CAddOrUpdateAlarmWithProcessRespond& p_oRespond, PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& p_oRequest);

		bool BuildAddSignRespond(PROTOCOL::CAddSignRespond& p_oRespond, const PROTOCOL::CAddSignRequest& p_oRequest);
		bool BuildGetSignRespond(PROTOCOL::CGetSignRespond::CBody& p_DataBody, const PROTOCOL::CGetSignRequest& p_oRequest);

		bool BuildAddRemarkRespond(PROTOCOL::CAddAlarmRemarkRespond& p_oRespond, const PROTOCOL::CAddAlarmRemarkRequestEx& p_oRequest);
		bool BuildGetRemarkRespond(PROTOCOL::CGetAlarmRemarkRespond& p_oRespond, const PROTOCOL::CGetAlarmRemarkRequest& p_oRequest);
		bool BuildGetAllRemarkRespond(PROTOCOL::CGetAlarmAllRemarkRespond& p_oRespond, const PROTOCOL::CGetAlarmAllRemarkRequest& p_oRequest);

		bool BuildGetAlarmLogRespond(PROTOCOL::CGetAlarmLogRespond& p_oRespond, const PROTOCOL::CGetAlarmLogRequest& p_oRequest);
		bool BuildGetAlarmLogAlarmRespond(PROTOCOL::CGetAlarmLogAlarmResRespond& p_oRespond, const PROTOCOL::CGetAlarmLogAlarmResRequest& p_oRequest);
		bool BuildGetAlarmLogProcessRespond(PROTOCOL::CGetAlarmProcLogResRespond& p_oRespond, const PROTOCOL::CGetAlarmProcLogResRequest& p_oRequest);
		bool BuildGetAlarmLogCallrefRespond(PROTOCOL::CGetAlarmLogCallrefRespond& p_oRespond, const PROTOCOL::CGetAlarmLogCallrefRequest& p_oRequest);

		bool BuildMergeAlarmRespond(PROTOCOL::CAddMergeRespond& p_oRespond, const PROTOCOL::CAddMergeRequest& p_oRequest);
		bool BuildUnMergeRespond(PROTOCOL::CCancelMergeRespond& p_oRespond, const PROTOCOL::CCancelMergeRequest& p_oRequest);
		bool BuildGetMergeAlarmRespond(PROTOCOL::CGetMergeAlarmRespond& p_oRespond, const PROTOCOL::CGetMergeAlarmRequest& p_oRequest);

		bool BuildVisitSyncBody(std::string p_strID, PROTOCOL::CAlarmVisitSync::CBody& p_oSyncBody);
		bool BuildDeptPhoneNum(std::string p_stdDeptId, std::string& p_stdDeptCalled);

		bool Build12345Request(PROTOCOL::CPushAlarmRequest& p_sRequest, PROTOCOL::CPushAlarmTo12345Request& p_dRequest);

		//自动催办写流水与消息中心
		void _AutoUrgeAlarmInfoAndMessageCenter(std::string strSendType, std::string strReceiptDeptName, std::string strProcessDeptName, std::string strAlarmId);

		//获取12345Ip与端口
		bool Get12345Service(std::string& p_strIp, std::string& p_strPort);

		//获取Nacos 指定服务信息， 输入 服务名称   返回： Ip与端口  
		bool GetServiceInfo(std::string& p_strServiceName, std::string& p_strIp, std::string& p_strPort);

		void WechatAlarmAttach(std::string p_strAlarmId, bool p_bWechatAlarmType = false, bool p_bAttachchange = false);
		void BuildWechatAlarmAttachRespond(PROTOCOL::CNotifiWechatAttachRequest& p_pRespond, std::string p_strAlarmId, bool p_bWechatAlarmType = false, bool p_bAttachchange = false);
		bool AddWechatAlarmAttach(PROTOCOL::CNotifiAddAlarmAttachRequest& p_pRequest);

		bool UpdateWechatAlarmAttach(PROTOCOL::CNotifiAddAlarmAttachRequest& p_pRequest, std::string p_strAlarmId, bool p_bAttachchange = false);

		// 构建部分字段
		std::string BuildAlarmState(std::string p_strID, std::string p_strCurState, std::string p_strInputState, const std::string& strTransGuid);
		std::string BuildProcessState(std::string p_strID, std::string p_strCurState, std::string p_strInputState, const std::string& strTransGuid);
		std::string BuildAlarmLogContent(std::vector<std::string> p_vecParamList);
		std::string BuildAlarmLogAttach(std::string& type, std::string& id);

		std::string _GetCurrentProcessState(const std::string &p_strID, const std::string &p_strInputState, const std::string& strTransGuid);
		// 转发至VCS对接服务
		void SendRemark2VCS(const PROTOCOL::CAddAlarmRemarkRequestEx& p_oRequest);

		// 同步操作
		void SyncSyntInfo(const PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& p_AlarmSyntInfo, ESyncType p_iSyncType);
		void SyncAlarmInfo(const PROTOCOL::CAlarmInfo& l_oSetAlarmRequest, ESyncType p_iSyncType, std::string p_strReleatedID = "");
		void SyncProcessInfo(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_rProcessToSync, ESyncType p_iSyncType, std::string p_strReleatedID = "");
		void SyncAlarmLogInfo(const PROTOCOL::CAlarmLogSync::CBody& p_rAlarmLogToSync);
		void SyncRemarkInfo(const PROTOCOL::CAddAlarmRemarkRequestEx& p_oRemarkRequest);
		void SyncReceiptDate(std::string p_strID);
		void SyncProcessDate(std::string p_strID, bool bFromVcs);

		// 数据库操作
		bool IsUpdateTimeNotEmpty(std::string p_strAlarmID, const std::string& strTransGuid);
		bool SelectAlarmLogByAlarmID(const PROTOCOL::CGetAlarmLogRequest& p_oRequest, PROTOCOL::CGetAlarmLogRespond::CBody& p_mapAlarmLogInfo);
		bool SelectAlarmBookByResID(std::string p_strResource, std::vector<PROTOCOL::CGetAlarmLogAlarmResRespond::CData> &p_vecData);
		bool SelectProcessBookByResID(std::string p_strResource, std::vector<PROTOCOL::CGetAlarmProcLogResRespond::CData> &p_vecData);
		bool SelectMergeAlarmByID(std::string p_strID, std::vector<PROTOCOL::CAlarmInfo>& p_vecAlarm);
		bool SelectVisitInfo(const PROTOCOL::CSearchAlarmVisitRequest::CBody& p_oReqBody, PROTOCOL::CSearchAlarmVisitRespond::CBody& p_oRespBody);


		std::string _PacketDeptInfo(const std::string& strDeptCode);

		bool SelectChildDeptCount(const PROTOCOL::CSearchAlarmVisitStatisticRequest::CBody& p_oReqBody, PROTOCOL::CSearchAlarmVisitStatisticRespond::CBody& p_oRespBody);
		bool SelectChildDept(const PROTOCOL::CSearchAlarmVisitStatisticRequest::CBody& p_oReqBody, PROTOCOL::CSearchAlarmVisitStatisticRespond::CBody& p_oRespBody);
		unsigned int SelectAlarmCount(const std::string p_strDeptCode, const PROTOCOL::CSearchAlarmVisitStatisticRequest::CBody& p_oReqBody);
		unsigned int SelectVisitCount(const std::string p_strDeptCode, const PROTOCOL::CSearchAlarmVisitStatisticRequest::CBody& p_oReqBody);
		unsigned int SelectAvailVisitCount(const std::string p_strDeptCode, const PROTOCOL::CSearchAlarmVisitStatisticRequest::CBody& p_oReqBody);
		unsigned int SelectSatisfactoryCount(const std::string p_strDeptCode, const PROTOCOL::CSearchAlarmVisitStatisticRequest::CBody& p_oReqBody);

		bool InsertAlarmInfo(const PROTOCOL::CAddOrUpdateAlarmWithProcessRequest::CBody& p_oAlarmSyntInfo);	
		bool InsertProcessInfo(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_oProcessInfo, const std::string& strTransGuid);
		bool InsertAlarmLogInfo(PROTOCOL::CAlarmLogSync::CBody& p_AlarmLogInfo, const std::string& strTransGuid);
		bool InsertAlarmBookBeforeUpdate(const PROTOCOL::CAlarmInfo& p_oAlarmInfo, std::string& p_strResourceID, const std::string& strTransGuid, int& iModifyField);
		bool InsertProcessBookBeforeUpdate(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_oProcessInfo, std::string p_strResourceID, const std::string& strTransGuid);
		bool InsertAlarmBookAfterUpdate(const PROTOCOL::CAlarmInfo&p_oAlarmInfo, std::string p_strResourceID, const std::string& strTransGuid);
		bool InsertProcessBookAfterUpdate(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo, std::string p_strResourceID, const std::string& strTransGuid);
		bool InsertVisit(const PROTOCOL::CAlarmVisitRequest& p_oVisitInfo, const std::string& strTransGuid);
		bool UpdateVisit(const PROTOCOL::CAlarmVisitRequest& p_oVisitInfo, const std::string& strTransGuid);
		
		bool InsertAddAlarmAttach(PROTOCOL::CNotifiAddAlarmAttachRequest::data& p_pRequest, const std::string& strGuid, const std::string& strTransGuid);
		bool SearchAddAlarmAttach(std::string p_strAlarmID, PROTOCOL::CNotifiSearchAlarmAttachResponse& p_pResponse);
		bool DeleteAddAlarmAttach(const PROTOCOL::CNotifiDeleteAlarmAttachRequest& p_pRequest, const std::string& strTransGuid);
		bool UpdateAlarmAttach(const PROTOCOL::CNotifiAddAlarmAttachRequest::data& p_pRequest, const std::string& strTransGuid);
		void SyncAddAlarmAttach(const PROTOCOL::CNotifiAddAlarmAttachRequest::data& p_pRequest, const std::string& strGuid, PROTOCOL::CNotifiAddAlarmAttachRequest& p_pResponse);

		std::string GetLogGuidByRequest(const std::string& strDeptCode, PROTOCOL::CPutAlarmLogInfoRespond& p_oVisitInfo);
		void BuildAlarmLogInfoRespond(const std::string& strDeptCode, PROTOCOL::CPutAlarmLogInfoRespond& p_oVisitInfo);

		bool UpdateAlarmInfo(const PROTOCOL::CAddOrUpdateAlarmWithProcessRequest::CBody& p_pAlarmReceiptInfo, const std::string& strTransGuid);
		// ICC重启警单 同步更新门市告警状态
		bool UpdateCallProcessInfo(const PROTOCOL::CAddOrUpdateAlarmWithProcessRequest::CBody& p_pAlarmReceiptInfo, const std::string& p_strSourceID);
		// 根据录音号判断是否是门市话务
		bool IsCallProcessBySourceId(const std::string& p_strSourceID);
		std::map<std::string, std::map<std::string, std::string>> GetAlarmDataByAlarmID(const std::string& strAlarmID);
		void UpdateAlarmLog(std::map<std::string, std::map<std::string, std::string>>strAlarmSourceValue, std::map<std::string, std::map<std::string, std::string>> strNewData, const std::string& strGuidId);
		bool UpdateProcessInfo(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo, const std::string& strTransGuid);

		bool UpdateIsVisitor(std::string p_strAlarmID, const std::string& strTransGuid);

		bool _UpdateAlarmSubmitTime(const std::string& strAlarmId, const std::string& strTransGuid);

		void AnalogProcessData(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& p_oAlarmInfo);
		bool GetDeptInfo(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& p_oAddSyntRequest);
	public:
		std::string RealProcessState(std::string p_strID, std::string p_strCurState, std::string p_strInputState, const std::string& strTransGuid);
		void InitHandleStateOrder();
		std::map<std::string, unsigned int> m_mapProcessStateOrder;

	private:
		bool _UpdateStatisticAlarmNumInfo(const std::string& strReceiptSeatno);
		//void BuildAlarmDeptCodeByDistrict(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& l_oUpdateAlarmRequest);
		bool GetAlarm(std::string l_strID, PROTOCOL::CAddOrUpdateAlarmWithProcessRequest& l_oAlarm);

		// 新增处警信息
		bool AddProcessInfo(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& l_oAddProcessInfo);
		// 更新处警信息
		bool UpdateProcessInfo(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& l_oUpdateProcessInfo);


		// 数据库操作：更新处警数据
		bool UpdateDBProcessInfo(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo);
		// 数据库操作：插入处警数据
		bool InsertDBProcessInfo(PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo);

		//更新前备份数据库处警数据到insert_icc_t_pjdb_book
		bool AddProcessBookByUpProcess(std::string p_strProcessId, DataBase::SQLRequest &p_oSQLReq, std::string p_strResourceID);
		//根据输入的参数列表构造流水的Content值
		std::string GenAlarmLogContent(std::vector<std::string> p_vecParamList);
		// 检测处警数据的有效性，有效返回true，无效则返回false
		bool IsProcessInfoValid(const PROTOCOL::CAddOrUpdateProcessRequest::CBody& p_pProcessInfo);
		// 同步处警数据
		void SyncProcessInfo(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessToSync, int p_iSyncType);
		bool ExecSql(DataBase::SQLRequest p_oSQLReq);
		//更新后前备份数据库处警数据到insert_icc_t_pjdb_book
		bool AddUpdateProcessAlarmBookByResID(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData &p_pProcessAlarmInfo, std::string p_strResourceID);

		std::string _GenId(const std::string& strPre = "");

		std::string _GenAlarmID(const std::string& strPre = "", const std::string& strSeatNo = "", const std::string& strReceiptDeptCode = "");

		void _DeleteAlarmWiolationCheck(const std::string& strID);
		void _AddAlarmViolationCheck(const std::string& strID, std::string strCreateTime);
		void _AlarmViolationCheckProcess();
		void ReportAlarmViolation(const std::string& strID, const std::string& p_strSeatNo, const std::string& p_strReceiptName, const std::string& p_strReceiptDeptCode);

		std::string _QueryRemarkByAlarmId(const std::string& strAlarmId);

		bool _QueryAlarmInfo(const std::string& strAlarmId, PROTOCOL::CAlarmInfo& alarmInfo);

		bool SetSqlUpdateProcess(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo, DataBase::SQLRequest& p_tSQLRequest);
		bool SetSqlInsertProcess(const PROTOCOL::CAddOrUpdateProcessRequest::CProcessData& p_pProcessInfo, const std::string& p_strState, DataBase::SQLRequest& p_tSQLRequest);
		void SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName);

		void AutoUrgeOnUpdateAlarm(PROTOCOL::CAddOrUpdateAlarmWithProcessRequest::CBody l_oSendData);
		std::string BuildUrgeId();

		void GetCallMessage(PROTOCOL::CGetAlarmLogRespond::CData l_oData, std::string& strType,std::string &strSourceID);
		bool GetRecordID(std::string strSourceID, PROTOCOL::CGetAlarmLogRespond::CData& l_oData);

	private:
		std::string ToStringTime(std::string p_pDateTime);
		void    _OnReceiveNotify(ObserverPattern::INotificationPtr p_pNotifiReqeust);
		void    _DispatchNotify(ObserverPattern::INotificationPtr p_pNotifiReqeust, const std::string& strCmdName);
		virtual void ProcMessage(CommonThread_data msg_data);
		CommonWorkThreadPtr _GetThread();
		CommonWorkThreadPtr _GetThreadEx(int iAlarmId);
		void    _InitProcNotifys();	 
		void    _CreateThreads();
		void    _DestoryThreads();

		void    _DispatchAlarmNotify(ICC::PROTOCOL::AddOrUpdateAlarmWithProcessRequestPtr pAlarmRequestPtr, ObserverPattern::INotificationPtr p_pNotify, const std::string& strCmdName);
		void    _ProcessAddAlarmRequest(ICC::PROTOCOL::AddOrUpdateAlarmWithProcessRequestPtr pAlarmRequestPtr, ObserverPattern::INotificationPtr p_pNotify);
		void    _ProcessAddOrUpdateAlarmReqeust(ICC::PROTOCOL::AddOrUpdateAlarmWithProcessRequestPtr pAlarmRequestPtr, ObserverPattern::INotificationPtr p_pNotify);
		
		unsigned int _GetAlarmId(const std::string& strAlarmId);
		bool	_GetStaffInfo(const std::string& strStaffCode, Data::CStaffInfo& l_oStaffInfo);
		std::string	_GetPoliceTypeName(const std::string& strStaffType,const std::string& strStaffName);

		std::string                         m_strDispatchMode;
		unsigned int                        m_uCurrentThreadIndex;
		unsigned int                        m_uProcThreadCount;

		std::mutex                          m_mutexThread;		
		std::vector<CommonWorkThreadPtr>    m_vecProcThreads;			
		std::map<std::string, ProcNotify>   m_mapFuncs;

		unsigned int                        m_uCurrentAlarmThreadIndex;
		unsigned int                        m_uProcAlarmThreadCount;
		std::mutex                          m_mutexAlarmThread;
		std::vector<CommonWorkThreadPtr>    m_vecAlarmProcThreads;
		std::map<std::string, ProcAlarmNotify>   m_mapAlarmFuncs;
		std::mutex                          m_mutexCallMap;
		std::map<std::string, std::string>  m_mapCall;

		CCommonWorkThread                   m_threadViolation;
		std::atomic_bool                    m_bViolationIsRunning;

	private:
		Timer::ITimerManagerPtr m_pTimerMgr;

		DateTime::IDateTimePtr m_pDateTime;
		HelpTool::IHelpToolPtr m_pHelpTool;
		StringUtil::IStringUtilPtr m_pString;
		Config::IConfigPtr m_pConfig;
		Log::ILogPtr m_pLog;
		DataBase::IDBConnPtr m_pDBConn;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		std::string m_strIdGenRule;
		std::string m_strAlarmIdGenRule;
		IHttpClientPtr m_pHttpClient;
		MsgCenter::IMessageCenterPtr m_pMsgCenter;

		std::string m_strLang_code;

		std::string m_strNacosServerIp;
		std::string m_strNacosServerPort;
		std::string m_strNacosServerNamespace;
		std::string m_strNacosServerGroupName;
		boost::mutex     m_mutexNacosParams;
		std::string m_strIsUseAutourge;
		std::string m_strAutourgeType;
		std::string m_strAutourgeSmsContent;
		std::string m_strAssistantPolice;	//配置项是否为辅警
		std::string m_strPolice;			//配置项是否为民警
		std::string m_strCodeMode;			//警员号码模式：1、警员身份证号码 2、警员编号

		int m_iZoneSecond;   //相差的时区秒

		std::recursive_mutex m_lkSerial;  //可重入锁

		Redis::IRedisClientPtr		m_pRedisClient;		//redis客户端

		std::mutex	m_violationCheckMutex;
		std::map<std::string, int> m_mAlarmViolationCheckList;
		int m_nAlarmViolationTimeout;

		std::mutex	m_msgCenterMutex;
		std::map<std::string, MsgCenterListNode> m_mMsgCenterRetransmissionList;
		int m_nMsgCenterExecTime;
		int m_nMsgCenterExecCount;

		std::string m_strUrgeCount;
		std::string m_strUrgeInterval;
		std::string m_strUrgeVoiceId;

		std::string m_strSmsServiceName;           // 短信服务名称
		std::string m_strWechatServiceName;        // 微信报警服务名称
		std::string m_strWechatfileServiceName;    // 微信附件服务名称
	};
}