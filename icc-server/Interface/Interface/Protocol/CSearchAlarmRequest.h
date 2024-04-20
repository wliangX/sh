#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchAlarmRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strVcsSyncFlag = p_pJson->GetNodeValue("/body/vcs_sync_flag", "");

				m_oBody.m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
				m_oBody.m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");
				m_oBody.m_strBeginTime = p_pJson->GetNodeValue("/body/begin_time", "");
				m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				m_oBody.m_strID = p_pJson->GetNodeValue("/body/id", "");

				m_oBody.m_strTitle = p_pJson->GetNodeValue("/body/title", "");
				m_oBody.m_strContent = p_pJson->GetNodeValue("/body/content", "");

				m_oBody.m_strAddr = p_pJson->GetNodeValue("/body/addr", "");
				m_oBody.m_strState = p_pJson->GetNodeValue("/body/state", "");
				m_oBody.m_strLevel = p_pJson->GetNodeValue("/body/level", "");
				m_oBody.m_strSourceType = p_pJson->GetNodeValue("/body/source_type", "");

				m_oBody.m_strHandleType = p_pJson->GetNodeValue("/body/handle_type", "");
				m_oBody.m_strIsVerity = p_pJson->GetNodeValue("/body/is_verity", "");
			/*	m_oBody.m_strFirstType = p_pJson->GetNodeValue("/body/first_type", "");
				m_oBody.m_strSecondType = p_pJson->GetNodeValue("/body/second_type", "");
				m_oBody.m_strThirdType = p_pJson->GetNodeValue("/body/third_type", "");
				m_oBody.m_strFourthType = p_pJson->GetNodeValue("/body/fourth_type", "");*/
				m_oBody.m_strAlarmType = p_pJson->GetNodeValue("/body/alarm_type", "");


				m_oBody.m_strFeedbackFirstType = p_pJson->GetNodeValue("/body/feedback_first_type", "");
				m_oBody.m_strFeedbackSecondType = p_pJson->GetNodeValue("/body/feedback_second_type", "");
				m_oBody.m_strFeedbackThirdType = p_pJson->GetNodeValue("/body/feedback_third_type", "");
				m_oBody.m_strFeedbackFourthType = p_pJson->GetNodeValue("/body/feedback_fourth_type", "");
/*
				m_oBody.m_strIsInvolveForeign = p_pJson->GetNodeValue("/body/is_involve_foreign", "");
				m_oBody.m_strIsInvolvePolice = p_pJson->GetNodeValue("/body/is_involve_police", "");
				m_oBody.m_strIsInvolveGun = p_pJson->GetNodeValue("/body/is_involve_gun", "");
				m_oBody.m_strIsInvolveKnife = p_pJson->GetNodeValue("/body/is_involve_knife", "");
				m_oBody.m_strIsInvolveTerror = p_pJson->GetNodeValue("/body/is_involve_terror", "");
				m_oBody.m_strIsInvolvePornography = p_pJson->GetNodeValue("/body/is_involve_pornograp", "");
				m_oBody.m_strIsInvolveGamble = p_pJson->GetNodeValue("/body/is_involve_gamble", "");
				m_oBody.m_strIsInvolvePoison = p_pJson->GetNodeValue("/body/is_involve_poison", "");*/
				m_oBody.m_strEventType = p_pJson->GetNodeValue("/body/event_type", "");
								
				m_oBody.m_strCalledNoType = p_pJson->GetNodeValue("/body/called_no_type", "");
				m_oBody.m_strCallerNo = p_pJson->GetNodeValue("/body/caller_no", "");
				m_oBody.m_strCallerName = p_pJson->GetNodeValue("/body/caller_name", "");

				m_oBody.m_strContactNo = p_pJson->GetNodeValue("/body/contact_no", "");
				m_oBody.m_strContactName = p_pJson->GetNodeValue("/body/contact_name", "");

				m_oBody.m_strAdminDeptCode = p_pJson->GetNodeValue("/body/admin_dept_code", "");
				m_oBody.m_strAdminDeptCodeRecursion = p_pJson->GetNodeValue("/body/admin_dept_code_recursion", "");
				m_oBody.m_strReceiptDeptCode = p_pJson->GetNodeValue("/body/receipt_dept_code", "");
				m_oBody.m_strReceiptDeptCodeRecursion = p_pJson->GetNodeValue("/body/receipt_dept_code_recursion", "");
				m_oBody.m_strLeaderCode = p_pJson->GetNodeValue("/body/leader_code", "");
				m_oBody.m_strReceiptCode = p_pJson->GetNodeValue("/body/receipt_code", "");

				m_oBody.m_strCurUserDeptCode = p_pJson->GetNodeValue("/body/cuur_user_dept_code", "");

				m_oBody.m_strIsFeedBack = p_pJson->GetNodeValue("/body/is_feedback", "");
				m_oBody.m_strIsVisitor = p_pJson->GetNodeValue("/body/is_visitor", "");

				m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/body/receipt_seatno", "");
				/*if (m_oBody.m_strSeatNo.empty())
				{
					m_oBody.m_strSeatNo = m_oHeader.m_strSeatNo;
				}*/

				m_oBody.m_strCityCode = p_pJson->GetNodeValue("/body/city_code", "");
				
				m_oBody.m_strCallerKey = p_pJson->GetNodeValue("/body/caller_key", "");
				
				m_oBody.m_strIsInvalid = p_pJson->GetNodeValue("/body/is_invalid", "");
				
				m_oBody.m_strQueryKey = p_pJson->GetNodeValue("/body/query_key", "");

				m_oBody.m_strMajorAlarmFlag = p_pJson->GetNodeValue("/body/majoralarm_flag", "");

				m_oBody.m_strIsClosure = p_pJson->GetNodeValue("/body/is_over", "");

				m_oBody.m_strReceiverCode = p_pJson->GetNodeValue("/body/receiver_code", "");

				m_oBody.m_strCodeWhenQueryAll = p_pJson->GetNodeValue("/body/code_when_query_all", "");

				m_oBody.m_strTelHotLine = p_pJson->GetNodeValue("/body/is_telhotline", "");
				m_oBody.m_str110Transfer12345 = p_pJson->GetNodeValue("/body/110_transfer_12345", "");
				m_oBody.m_str12345Transfer110 = p_pJson->GetNodeValue("/body/12345_transfer_110", "");
				m_oBody.m_strIsSigned = p_pJson->GetNodeValue("/body/is_signed", "");
				return true;
			}

		public:

			class CBody
			{
			public:
				std::string m_strVcsSyncFlag;
				std::string	m_strPageSize;					//每页数量（不能为空）
				std::string	m_strPageIndex;					//页码，1表示第一页（不能为空）
				std::string	m_strBeginTime;					//查询开始时间（不能为空）
				std::string	m_strEndTime;					//查询结束时间（不能为空）
				std::string	m_strID;						//警情id（模糊查询）
				std::string	m_strTitle;						//警情标题（模糊查询）
				std::string	m_strContent;					//警情内容（模糊查询）
				std::string	m_strAddr;						//警情详细发生地址（模糊查询）
				std::string	m_strLevel;						//警情状态（多值查询，逗号分隔）
				std::string	m_strState;						//警情级别（多值查询，逗号分隔）
				std::string	m_strSourceType;				//警情报警来源类型（多值查询，逗号分隔）
				std::string	m_strHandleType;				//警情处理类型（多值查询，逗号分隔）
				std::string m_strIsVerity;					//只显示有效警情标志
				std::string	m_strFirstType;					//警情一级类型（多值查询，逗号分隔）
				std::string	m_strSecondType;				//警情二级类型（多值查询，逗号分隔）
				std::string	m_strThirdType;					//警情三级类型（多值查询，逗号分隔）
				std::string	m_strFourthType;				//警情四级类型（多值查询，逗号分隔)

				std::string	m_strFeedbackFirstType;			//反馈警情一级类型（多值查询，逗号分隔）
				std::string	m_strFeedbackSecondType;		//反馈警情二级类型（多值查询，逗号分隔）
				std::string	m_strFeedbackThirdType;			//反馈警情三级类型（多值查询，逗号分隔）
				std::string	m_strFeedbackFourthType;		//反馈警情四级类型（多值查询，逗号分隔）

				std::string m_strEventType;					//事件类型，涉恐涉暴等（编码，逗号隔开）
				std::string	m_strCalledNoType;				//警情报警号码类型（多值查询，逗号分隔）
				std::string	m_strCallerNo;					//警情报警人号码（模糊查询）
				std::string	m_strCallerName;				//警情报警人姓名（模糊查询）
				std::string	m_strContactNo;					//警情联系人号码（模糊查询）
				std::string	m_strContactName;				//警情联系人姓名（模糊查询）
				std::string	m_strAdminDeptCode;				//警情管辖单位编码（多值查询，逗号分隔）
				std::string	m_strAdminDeptCodeRecursion;	//0表示不递归查询，1表示递归查询
				std::string	m_strReceiptDeptCode;			//警情接警单位编码（递归查询）
				std::string	m_strReceiptDeptCodeRecursion;	//0表示不递归查询，1表示递归查询
				std::string	m_strLeaderCode;				//警情值班领导警号（多值查询，逗号分隔）
				std::string	m_strReceiptCode;				//警情接警人警号（多值查询，逗号分隔）
				std::string m_strCurUserDeptCode;			//当前用户所属部门编号（只允许查询用户所属部门及其子部门的警情）

				std::string m_strIsFeedBack;				//是否已反馈0：未反馈，1：已反馈				
				std::string m_strIsVisitor;					//是否已回访0：未回访，1：已回访		

				std::string m_strSeatNo;                    //接警台号(席位号)
				std::string m_strCityCode;
				std::string m_strCallerKey;                 //查询键值，如果有传值
                                                            //只做报警人姓名，报警人电话号码，报警人证件号精确匹配
				std::string m_strIsInvalid;                 //是否无效
				
				std::string m_strQueryKey;                  //查询警情键值，如果有传值
                                                            //只做警单号，警情地址，报警内容模糊匹配

				std::string m_strMajorAlarmFlag;			//重大警情查询标识；1：查询重大警情

				std::string m_strIsClosure;					// 是否结案，0：未结案，1：已结案

				std::string m_strReceiverCode;

				std::string m_strCodeWhenQueryAll;

				std::string m_strAlarmType;					//警情类别（多值查询，逗号分隔）

				std::string m_strTelHotLine;				//是否查询联动12345警情

				std::string m_str12345Transfer110;
				std::string m_str110Transfer12345;
				std::string m_strIsSigned;
			};


			CHeaderEx m_oHeader;
			CBody m_oBody;
		};
	}
}