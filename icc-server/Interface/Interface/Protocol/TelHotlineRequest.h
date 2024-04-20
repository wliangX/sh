#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"

namespace ICC
{
	namespace PROTOCOL
	{
		//110转入12345非警务警情信息
		class CTelHotlineVisitRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/JJDBH", "");
				m_oBody.m_strFlowID = p_pJson->GetNodeValue("/body/FLOWID", "");
				m_oBody.m_strVisitTime = p_pJson->GetNodeValue("/body/HFSJ", "");
				m_oBody.m_strSendReason = p_pJson->GetNodeValue("/body/DYYY", "");
				//回访部分(Visit)
				m_oBody.m_strVisitContent = p_pJson->GetNodeValue("/body/CLJGMC", "");
				m_oBody.m_strVisitResult = p_pJson->GetNodeValue("/body/CLJGDM", ""); 
				m_oBody.m_strTelVisitResult = p_pJson->GetNodeValue("/body/DHHFHSQK", "");
				m_oBody.m_strSendTime = p_pJson->GetNodeValue("/body/TSSJ", "");
				//反馈部分(Feedback)
				m_oBody.m_strFeedbackTime = p_pJson->GetNodeValue("/body/FKSJ", "");
				m_oBody.m_strAlarmDisposal = p_pJson->GetNodeValue("/body/CJCZQK", "");
				m_oBody.m_strAlarmResult = p_pJson->GetNodeValue("/body/JQCLJGSM", "");
				m_oBody.m_strFeedbackId = p_pJson->GetNodeValue("/body/FKYBH", "");
				m_oBody.m_strFeedbackName = p_pJson->GetNodeValue("/body/FKYXM", "");
				m_oBody.m_strFeedbackDeptCode = p_pJson->GetNodeValue("/body/FKDWDM", "");
				m_oBody.m_strFeedbackDeptName = p_pJson->GetNodeValue("/body/FKDWMC", "");
				//退回部分(Return)
				m_oBody.m_strReturnAlarmer = p_pJson->GetNodeValue("/body/HANDLEUSERNAME", "");
				m_oBody.m_strReturnDeptName = p_pJson->GetNodeValue("/body/HANDLEBMNAME", "");
				m_oBody.m_strReturnResult = p_pJson->GetNodeValue("/body/HANDLEOPINION", "");
				m_oBody.m_strReturnTime = p_pJson->GetNodeValue("/body/HANDLETIME", "");
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strAlarmID;             //接警单ID
				std::string m_strFlowID;              //工单ID
				std::string m_strSendTime;            //推送时间
				std::string m_strSendReason;		  //推送原因

				std::string m_strVisitTime;           //回访时间  YYMMDDhhmmss
				std::string m_strVisitContent;        //回访评价内容
				std::string m_strVisitResult;         //回访评价结果  1 - 非常满意、2 - 满意、3 - 一般、4 - 不满意、5 - 非常不满意
				std::string m_strTelVisitResult;      //电话回访核实情况

				std::string m_strFeedbackTime;        //反馈时间
				std::string m_strAlarmDisposal;		  //出警处置情况
				std::string m_strAlarmResult;		  //出警处理结果
				std::string m_strFeedbackId;		  //出警反馈人员编号
				std::string m_strFeedbackName;		  //出警反馈人员姓名
				std::string m_strFeedbackDeptCode;    //出警反馈单位编号
				std::string m_strFeedbackDeptName;    //出警反馈单位名称

				std::string m_strReturnAlarmer;       //退回接警员
				std::string m_strReturnDeptName;      //退回部门
				std::string m_strReturnResult;        //退回原因
				std::string m_strReturnTime;          //退回原因
			};
			CBody m_oBody;
		};

		//110转入12345确认信息
		class CTelHotlineConfirmRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/JJDBH", "");
				m_oBody.m_strReceiveState = p_pJson->GetNodeValue("/body/RECEIVE", "");
				m_oBody.m_strReceiveTime = p_pJson->GetNodeValue("/body/RECEIVETIME", "");
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strAlarmID;                //接警单ID
				std::string m_strReceiveState;           //获取状态 0:未获取, 1:获取
				std::string m_strReceiveTime;            //获取时间
			};
			CBody m_oBody;
		};

		class CSyncLinkedTableState :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/id", m_oBody.id);
				p_pJson->SetNodeValue("/body/alarm_id", m_oBody.alarm_id);
				p_pJson->SetNodeValue("/body/state", m_oBody.state);
				p_pJson->SetNodeValue("/body/linked_org_code", m_oBody.linked_org_code);
				p_pJson->SetNodeValue("/body/linked_org_name", m_oBody.linked_org_name);
				p_pJson->SetNodeValue("/body/linked_org_type", m_oBody.linked_org_type);
				p_pJson->SetNodeValue("/body/dispatch_code", m_oBody.dispatch_code);
				p_pJson->SetNodeValue("/body/dispatch_name", m_oBody.dispatch_name);
				p_pJson->SetNodeValue("/body/create_user", m_oBody.create_user);
				p_pJson->SetNodeValue("/body/create_time", m_oBody.create_time);
				p_pJson->SetNodeValue("/body/update_user", m_oBody.update_user);
				p_pJson->SetNodeValue("/body/update_time", m_oBody.update_time);
				p_pJson->SetNodeValue("/body/result", m_oBody.result);

				return p_pJson->ToString();
			}

			std::string ToStringEx(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/msg_source", m_oBody.m_strMsgSource);
				unsigned int l_uiIndex = 0;

				std::string l_strPrefixPath("/body/linked_data/" + std::to_string(l_uiIndex) + "/");

				p_pJson->SetNodeValue(l_strPrefixPath + "id", m_oBody.id);
				p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", m_oBody.alarm_id);
				p_pJson->SetNodeValue(l_strPrefixPath + "state", m_oBody.state);
				p_pJson->SetNodeValue(l_strPrefixPath + "linked_org_code", m_oBody.linked_org_code);
				p_pJson->SetNodeValue(l_strPrefixPath + "linked_org_name", m_oBody.linked_org_name);
				p_pJson->SetNodeValue(l_strPrefixPath + "linked_org_type", m_oBody.linked_org_type);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_code", m_oBody.dispatch_code);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_name", m_oBody.dispatch_name);
				p_pJson->SetNodeValue(l_strPrefixPath + "create_user", m_oBody.create_user);
				p_pJson->SetNodeValue(l_strPrefixPath + "create_time", m_oBody.create_time);
				p_pJson->SetNodeValue(l_strPrefixPath + "update_user", m_oBody.update_user);
				p_pJson->SetNodeValue(l_strPrefixPath + "update_time", m_oBody.update_time);
				p_pJson->SetNodeValue(l_strPrefixPath + "result", m_oBody.result);

				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string id;
				std::string alarm_id;
				std::string state;
				std::string linked_org_code;
				std::string linked_org_name;
				std::string linked_org_type;
				std::string dispatch_code;
				std::string dispatch_name;
				std::string create_user;
				std::string create_time;
				std::string update_user;
				std::string update_time;
				std::string result;

				std::string m_strMsgSource;
			};
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};

		//---------------泸州版本保存----------------
		//110转入12345工单流程办理
		//class CTelHotlineAddRequest :
		//	public IRequest
		//{
		//public:
		//	virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
		//	{
		//		if (!m_oHeader.ParseString(p_strReq, p_pJson))
		//		{
		//			return false;
		//		}
		//		m_oBody.m_strRunID = p_pJson->GetNodeValue("/body/runid", "");
		//		m_oBody.m_strFlowID = p_pJson->GetNodeValue("/body/flowid", "");
		//		m_oBody.m_strCode = p_pJson->GetNodeValue("/body/code", "");
		//		m_oBody.m_strNodeName = p_pJson->GetNodeValue("/body/node_name", "");
		//		m_oBody.m_strNodeType = p_pJson->GetNodeValue("/body/node_type", "");
		//		m_oBody.m_strHandleBMName = p_pJson->GetNodeValue("/body/handle_bm_name", "");
		//		m_oBody.m_strHandleUserName = p_pJson->GetNodeValue("/body/handle_user_name", "");
		//		m_oBody.m_strHandleTime = p_pJson->GetNodeValue("/body/handle_time", "");
		//		m_oBody.m_strHandleOpinion = p_pJson->GetNodeValue("/body/handle_opinion", "");
		//		m_oBody.m_strIsPolice = p_pJson->GetNodeValue("/body/is_police", "");
		//		m_oBody.m_strPoliceCode = p_pJson->GetNodeValue("/body/police_code", "");
		//		return true;
		//	}
		//public:
		//	CHeaderEx m_oHeader;
		//	class CBody
		//	{
		//	public:
		//		std::string m_strRunID;              //流转办理ID
		//		std::string	m_strFlowID;	         //工单ID
		//		std::string	m_strCode;	             //受理编号
		//		std::string m_strNodeName;	         //节点名称
		//		std::string m_strNodeType;	         //节点分类 1 - 交办、2 - 派单、3 - 退回、4 - 办结、5 - 归档、6 - 会签、7 - 特提、8 - 重办
		//		std::string m_strHandleBMName;	     //办理部门              
		//		std::string m_strHandleUserName;	 //办理人               
		//		std::string m_strHandleTime;	     //办理时间           
		//		std::string m_strHandleOpinion;	     //办理意见            
		//		std::string m_strIsPolice;	         //转110办理单  0 - 12345办理、1 - 12345转110办理
		//		std::string m_strPoliceCode;	     //接警单编号           jjdbh
		//	};
		//	CBody m_oBody;
		//};

		////110转入12345工单延期申请
		//class CTelHotlineFpideRequest :
		//	public IRequest
		//{
		//public:
		//	virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
		//	{
		//		if (!m_oHeader.ParseString(p_strReq, p_pJson))
		//		{
		//			return false;
		//		}
		//		m_oBody.m_strFPID = p_pJson->GetNodeValue("/body/fpid", "");
		//		m_oBody.m_strFlowID = p_pJson->GetNodeValue("/body/flowid", "");
		//		m_oBody.m_strCode = p_pJson->GetNodeValue("/body/code", "");
		//		m_oBody.m_strRunID = p_pJson->GetNodeValue("/body/runid", "");
		//		m_oBody.m_strApplyBMName = p_pJson->GetNodeValue("/body/apply_bm_name", "");
		//		m_oBody.m_strApplyUserName = p_pJson->GetNodeValue("/body/apply_user_name", "");
		//		m_oBody.m_strApplyTimeLimit = p_pJson->GetNodeValue("/body/apply_time_limit", "");
		//		m_oBody.m_strApplyContent = p_pJson->GetNodeValue("/body/apply_content", "");
		//		m_oBody.m_strApplyTime = p_pJson->GetNodeValue("/body/apply_time", "");
		//		m_oBody.m_strPoliceCode = p_pJson->GetNodeValue("/body/police_code", "");
		//		return true;
		//	}
		//public:
		//	CHeaderEx m_oHeader;
		//	class CBody
		//	{
		//	public:
		//		std::string m_strFPID;	            //延期申请ID
		//		std::string m_strFlowID;	        //工单ID
		//		std::string m_strCode;	            //受理编号
		//		std::string m_strRunID;	            //流转办理ID
		//		std::string m_strApplyBMName;	    //申请部门
		//		std::string m_strApplyUserName;	    //申请人
		//		std::string m_strApplyTimeLimit;	//申请时限
		//		std::string m_strApplyContent;	    //申请内容
		//		std::string m_strApplyTime;	        //申请时间 YYMMDDhhmmss
		//		std::string m_strPoliceCode;	    //接警单编号
		//	};
		//	CBody m_oBody;
		//};

		////110转入12345工单办理状态
		//class CTelHotlineHandleStateRequest :
		//	public IRequest
		//{
		//public:
		//	virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
		//	{
		//		if (!m_oHeader.ParseString(p_strReq, p_pJson))
		//		{
		//			return false;
		//		}
		//		m_oBody.m_strFlowID = p_pJson->GetNodeValue("/body/flowid", "");
		//		m_oBody.m_strCode = p_pJson->GetNodeValue("/body/code", "");
		//		m_oBody.m_strPoliceCode = p_pJson->GetNodeValue("/body/police_code", "");
		//		m_oBody.m_strHandleState = p_pJson->GetNodeValue("/body/handle_state", "");
		//		return true;
		//	}
		//public:
		//	CHeaderEx m_oHeader;
		//	class CBody
		//	{
		//	public:
		//		std::string m_strFlowID;           //工单ID
		//		std::string m_strCode;             //受理编号
		//		std::string m_strPoliceCode;       //接警单编号
		//		std::string m_strHandleState;      //办理状态   1 - 已签收、2 - 已交办、3 - 已受理、4 - 已办结、5 - 已归档
		//	};
		//	CBody m_oBody;
		//};

		//110转入12345工单回访信息
		// class CTelHotlineVisitRequest :
			// public IRequest
		// {
		// public:
			// virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			// {
				// if (!m_oHeader.ParseString(p_strReq, p_pJson))
				// {
					// return false;
				// }
				// m_oBody.m_strVisitID = p_pJson->GetNodeValue("/body/visitid", "");
				// m_oBody.m_strFlowID = p_pJson->GetNodeValue("/body/flowid", "");
				// m_oBody.m_strCode = p_pJson->GetNodeValue("/body/code", "");
				// m_oBody.m_strVisitMethod = p_pJson->GetNodeValue("/body/visit_method", "");
				// m_oBody.m_strUserName = p_pJson->GetNodeValue("/body/user_nmae", "");
				// m_oBody.m_strVisitTime = p_pJson->GetNodeValue("/body/visit_time", "");
				// m_oBody.m_strSeatsContent = p_pJson->GetNodeValue("/body/seats_content", "");
				// m_oBody.m_strSeatsResult = p_pJson->GetNodeValue("/body/seats_result", "");
				// m_oBody.m_strBMContent = p_pJson->GetNodeValue("/body/bm_content", "");
				// m_oBody.m_strBMResult = p_pJson->GetNodeValue("/body/bm_result", "");
				// m_oBody.m_strPoliceCode = p_pJson->GetNodeValue("/body/police_code", "");
				// return true;
			// }
		// public:
			// CHeaderEx m_oHeader;
			// class CBody
			// {
			// public:
				// std::string m_strVisitID;             //回访ID
				// std::string m_strFlowID;              //工单ID
				// std::string m_strCode;                //受理编号
				// std::string m_strVisitMethod;         //回访方式 1 - 电话、2 - 短信、3 - 门户网站
				// std::string m_strUserName;            //回访人
				// std::string m_strVisitTime;           //回访时间  YYMMDDhhmmss
				// std::string m_strSeatsContent;        //话务员评价内容
				// std::string m_strSeatsResult;         //话务员评价结果  1 - 非常满意、2 - 满意、3 - 一般、4 - 不满意、5 - 非常不满意
				// std::string m_strBMContent;           //部门评价内容
				// std::string m_strBMResult;            //部门评价结果  1 - 非常满意、2 - 满意、3 - 一般、4 - 不满意、5 - 非常不满意
				// std::string m_strPoliceCode;          //接警单编号
			// };
			// CBody m_oBody;
		// };

		////110转入12345工单办理结果
		//class CTelHotlineRequest :
		//	public IRequest
		//{
		//public:
		//	virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
		//	{
		//		if (!m_oHeader.ParseString(p_strReq, p_pJson))
		//		{
		//			return false;
		//		}
		//		m_oBody.m_strGDBH = p_pJson->GetNodeValue("/body/code", "");
		//		m_oBody.m_strBLR = p_pJson->GetNodeValue("/body/handleusername", "");
		//		m_oBody.m_strBLZT = p_pJson->GetNodeValue("/body/handlestate", "");
		//		m_oBody.m_strBLBM = p_pJson->GetNodeValue("/body/handlebmname", "");
		//		m_oBody.m_strBLJG = p_pJson->GetNodeValue("/body/handleopinion", "");
		//		m_oBody.m_strBLSJ = p_pJson->GetNodeValue("/body/handletime", "");
		//		m_oBody.m_strPoliceCode = p_pJson->GetNodeValue("/body/police_code", "");
		//		return true;
		//	}
		//public:
		//	CHeaderEx m_oHeader;
		//	class CBody
		//	{
		//	public:
		//		std::string m_strGDBH;	        //工单编号
		//		std::string m_strBLR;	        //办理人
		//		std::string m_strBLZT;          //办理状态
		//		std::string m_strBLBM;	        //办理部门
		//		std::string m_strBLJG;	        //办理结果  1办结 2退回
		//		std::string m_strBLSJ;          //办理时间
		//		std::string m_strPoliceCode;	//接警单编号

		//	};
		//	CBody m_oBody;
		//};

	}
}