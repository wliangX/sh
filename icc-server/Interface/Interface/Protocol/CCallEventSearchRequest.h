#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CCallEventSearchRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_EffectiveValue = 0;

				m_oBody.m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
				if (!m_oBody.m_strPageSize.empty()) m_oBody.m_EffectiveValue++;

				m_oBody.m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");
				if (!m_oBody.m_strPageIndex.empty()) m_oBody.m_EffectiveValue++;

				m_oBody.m_strCallerID = p_pJson->GetNodeValue("/body/caller_id", "");
				if (!m_oBody.m_strCallerID.empty()) m_oBody.m_EffectiveValue++;

				m_oBody.m_strCalledID = p_pJson->GetNodeValue("/body/called_id", "");
				if (!m_oBody.m_strCalledID.empty()) m_oBody.m_EffectiveValue++;

				m_oBody.m_strPhoneNo = p_pJson->GetNodeValue("/body/phone_no", "");
				if (!m_oBody.m_strPhoneNo.empty()) m_oBody.m_EffectiveValue++;

				m_oBody.m_strCallDirection = p_pJson->GetNodeValue("/body/call_direction", "");
				if (!m_oBody.m_strCallDirection.empty()) m_oBody.m_EffectiveValue++;

				m_oBody.m_strStartTime = p_pJson->GetNodeValue("/body/start_time", "");
				if (!m_oBody.m_strStartTime.empty()) m_oBody.m_EffectiveValue++;

				m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				if (!m_oBody.m_strEndTime.empty()) m_oBody.m_EffectiveValue++;

				m_oBody.m_strHangupType = p_pJson->GetNodeValue("/body/hangup_type", "");
				if (!m_oBody.m_strHangupType.empty()) m_oBody.m_EffectiveValue++;

				m_oBody.m_strCaseID = p_pJson->GetNodeValue("/body/case_id", "");
				if (!m_oBody.m_strCaseID.empty()) m_oBody.m_EffectiveValue++;

				m_oBody.m_strCallrefID = p_pJson->GetNodeValue("/body/callref_id", "");
				if (!m_oBody.m_strCallrefID.empty()) m_oBody.m_EffectiveValue++;

				m_oBody.m_strReceiptCodeOrName = p_pJson->GetNodeValue("/body/receipt_code_or_name", "");
				if (!m_oBody.m_strReceiptCodeOrName.empty()) m_oBody.m_EffectiveValue++;

				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");
				if (!m_oBody.m_strDeptCode.empty()) m_oBody.m_EffectiveValue++;

				m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/body/seat_no", "");
				if (!m_oBody.m_strSeatNo.empty()) m_oBody.m_EffectiveValue++;

				m_oBody.m_strRelAlarmID = p_pJson->GetNodeValue("/body/rel_alarm_id", "");
				if (!m_oBody.m_strRelAlarmID.empty()) m_oBody.m_EffectiveValue++;

				m_oBody.m_strExtraBusinessScene = p_pJson->GetNodeValue("/body/extra_business_scene", "");
				if (!m_oBody.m_strExtraBusinessScene.empty()) m_oBody.m_EffectiveValue++;

				m_oBody.m_strReleaseSearchDept = p_pJson->GetNodeValue("/body/release_search_dept", "");
				if (!m_oBody.m_strReleaseSearchDept.empty()) m_oBody.m_EffectiveValue++;

				m_oBody.m_strIsInnerCall = p_pJson->GetNodeValue("/body/isInnerCall", "");
				if (!m_oBody.m_strIsInnerCall.empty()) m_oBody.m_EffectiveValue++;

				m_oBody.m_strSearchTag = p_pJson->GetNodeValue("/body/searchTag", "");
				if (!m_oBody.m_strSearchTag.empty()) m_oBody.m_EffectiveValue++;

				m_oBody.m_strIsVcsSearch = p_pJson->GetNodeValue("/body/vcs_sync_flag", "");
				if (!m_oBody.m_strIsVcsSearch.empty()) m_oBody.m_EffectiveValue++;
				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strPageSize;
				std::string m_strPageIndex;
				std::string m_strCallerID;
				std::string m_strCalledID;
				std::string m_strPhoneNo;
				std::string m_strCallDirection;
				std::string m_strStartTime;
				std::string m_strEndTime;
				std::string m_strHangupType;
				std::string m_strCaseID;
				std::string m_strCallrefID;
				std::string m_strReceiptCodeOrName;
				std::string m_strDeptCode;	//用户所在部门
				std::string m_strSeatNo;	//席位号，用于早释电话查询，用以区分归属地的目标号码
				std::string m_strRelAlarmID;

				std::string m_strExtraBusinessScene;//只输出有主叫或被叫是本坐席的话务
				std::string m_strReleaseSearchDept;//早释查询归属部门
				std::string m_strIsInnerCall;      //是否内部呼叫
				
				std::string m_strIsVcsSearch;
				std::string m_strSearchTag;

				int m_EffectiveValue;
			};
			CBody m_oBody;
		};
	}
}
