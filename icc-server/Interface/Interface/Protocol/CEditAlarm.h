#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CEditAlarm :
			public IRespond,public IRequest
		{
		public:

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				std::string l_strPrefixPath("/body/alarm/");

				p_pJson->SetNodeValue(l_strPrefixPath + "msg_source", m_oBody.m_strMsgSource);
				p_pJson->SetNodeValue(l_strPrefixPath + "id", m_oBody.m_strID);
				p_pJson->SetNodeValue(l_strPrefixPath + "merge_id", m_oBody.m_strMergeID);				
				p_pJson->SetNodeValue(l_strPrefixPath + "content", m_oBody.m_strCasedesc);
				p_pJson->SetNodeValue(l_strPrefixPath + "time", m_oBody.m_strCallingTime);
				
				p_pJson->SetNodeValue(l_strPrefixPath + "actual_occur_time", m_oBody.m_strCasetime);
				p_pJson->SetNodeValue(l_strPrefixPath + "addr", m_oBody.m_strAddr);
				p_pJson->SetNodeValue(l_strPrefixPath + "longitude", m_oBody.m_strLongitude);
				p_pJson->SetNodeValue(l_strPrefixPath + "latitude", m_oBody.m_strLatitude);
				p_pJson->SetNodeValue(l_strPrefixPath + "state", m_oBody.m_strStatus);

				p_pJson->SetNodeValue(l_strPrefixPath + "level", m_oBody.m_strCaselevel);
				p_pJson->SetNodeValue(l_strPrefixPath + "source_type", m_oBody.m_strSourceType);

				p_pJson->SetNodeValue(l_strPrefixPath + "handle_type", m_oBody.m_strHandleType);
				p_pJson->SetNodeValue(l_strPrefixPath + "first_type", m_oBody.m_strCaseType);
				p_pJson->SetNodeValue(l_strPrefixPath + "second_type", m_oBody.m_strCaseSubType);
				p_pJson->SetNodeValue(l_strPrefixPath + "third_type", m_oBody.m_strCaseThreeType);
				
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_no", m_oBody.m_strCallerNo);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_name", m_oBody.m_strCallerName);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_addr", m_oBody.m_strCallerAddr);
				p_pJson->SetNodeValue(l_strPrefixPath + "caller_gender", m_oBody.m_strCallerGender);
				
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_name", m_oBody.m_strContactName);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_no", m_oBody.m_strContactNo);
				p_pJson->SetNodeValue(l_strPrefixPath + "contact_gender", m_oBody.m_strContactSex);

				p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_district_code", m_oBody.m_strCaseDestrict);
				p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_code", m_oBody.m_strAreaOrg);
				p_pJson->SetNodeValue(l_strPrefixPath + "admin_dept_name", m_oBody.m_strAreaOrgName);

				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_code", m_oBody.m_strReceiptCode);
				p_pJson->SetNodeValue(l_strPrefixPath + "receipt_name", m_oBody.m_strReceiptName);
				p_pJson->SetNodeValue(l_strPrefixPath + "seatno", m_oBody.m_strSeatNo);				


				p_pJson->SetNodeValue(l_strPrefixPath + "update_user", m_oBody.m_strUpdateUser);
				p_pJson->SetNodeValue(l_strPrefixPath + "update_time", m_oBody.m_strUpdateTime);

				return p_pJson->ToString();
			}

            virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strMsgSource = p_pJson->GetNodeValue("/body/msg_source", "");
				m_oBody.m_strID = p_pJson->GetNodeValue("/body/id", "");
				m_oBody.m_strMergeID = p_pJson->GetNodeValue("/body/merge_id", "");
				
				m_oBody.m_strCasedesc = p_pJson->GetNodeValue("/body/casedesc", "");
				m_oBody.m_strCallingTime = p_pJson->GetNodeValue("/body/callingtime", "");
				m_oBody.m_strAnsweringtime = p_pJson->GetNodeValue("/body/answeringtime", "");
				m_oBody.m_strCasetime = p_pJson->GetNodeValue("/body/casetime", "");
				m_oBody.m_strAddr = p_pJson->GetNodeValue("/body/caseaddress", "");
				m_oBody.m_strLongitude = p_pJson->GetNodeValue("/body/longitude", "");
				m_oBody.m_strLatitude = p_pJson->GetNodeValue("/body/latitude", "");
				m_oBody.m_strStatus = p_pJson->GetNodeValue("/body/state", "");


				if (m_oBody.m_strStatus == "JQZT001")//未接收
				{
					m_oBody.m_strStatus = "DIC019050";
				}
				else if (m_oBody.m_strStatus == "JQZT003")//已接收
				{
					m_oBody.m_strStatus = "DIC019060";
				}
				else if (m_oBody.m_strStatus == "JQZT004")//已到场
				{
					m_oBody.m_strStatus = "DIC019080";
				}
				else if (m_oBody.m_strStatus == "JQZT005")//已完成
				{
					m_oBody.m_strStatus = "DIC019100";
				}
				else
				{
					m_oBody.m_strStatus = "DIC019060";
				}
				
				m_oBody.m_strCaselevel = p_pJson->GetNodeValue("/body/caselevel", ""); //字典值转换，默认4级别
				if (m_oBody.m_strCaselevel.empty())
				{
					m_oBody.m_strCaselevel = "04";
				}
				m_oBody.m_strSourceType = p_pJson->GetNodeValue("/body/casesource", "");
				m_oBody.m_strHandleType = p_pJson->GetNodeValue("/body/handletype", "");
				m_oBody.m_strCaseType = p_pJson->GetNodeValue("/body/casetype", "");
				m_oBody.m_strCaseSubType = p_pJson->GetNodeValue("/body/casesubtype", "");
				m_oBody.m_strCaseThreeType = p_pJson->GetNodeValue("/body/casethreetype", "");
				m_oBody.m_strCallerNo = p_pJson->GetNodeValue("/body/callerno", "");
				m_oBody.m_strCallerName = p_pJson->GetNodeValue("/body/callername", "");
				m_oBody.m_strCallerAddr = p_pJson->GetNodeValue("/body/calleraddr", "");
				m_oBody.m_strCallerGender = p_pJson->GetNodeValue("/body/callergender", "");
				m_oBody.m_strCalled = p_pJson->GetNodeValue("/body/calledno", "");
				m_oBody.m_strContactName = p_pJson->GetNodeValue("/body/contact", "");
				m_oBody.m_strContactNo = p_pJson->GetNodeValue("/body/contactno", "");
				m_oBody.m_strContactSex = p_pJson->GetNodeValue("/body/contactsex", "");
				m_oBody.m_strCaseDestrict = p_pJson->GetNodeValue("/body/casedestrict", "");

				m_oBody.m_strAreaOrg = p_pJson->GetNodeValue("/body/areaorg", "");
				m_oBody.m_strAreaOrgName = p_pJson->GetNodeValue("/body/areaorgname", "");
				m_oBody.m_strReceiptCode = p_pJson->GetNodeValue("/body/receiptcode", "");
				m_oBody.m_strReceiptName = p_pJson->GetNodeValue("/body/receiptname", "");
				m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/body/seatno", "");


				m_oBody.m_strUpdateUser = p_pJson->GetNodeValue("/body/update_user", "");
				m_oBody.m_strUpdateTime = p_pJson->GetNodeValue("/body/update_time", "");

				return true;
            }

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strMsgSource;
				std::string	m_strID;						//警情id
				std::string	m_strMergeID;					//警情合并id
				std::string m_strCasedesc;					//警情描述
				std::string m_strCallingTime;				//呼入时间
				std::string m_strAnsweringtime;				//应答时间
				std::string m_strCasetime;					//案发时间
				std::string m_strAddr;						//警情详细发生地址
				std::string m_strLongitude;					//警情经度
				std::string m_strLatitude;					//警情维度
				std::string m_strStatus;					//警情状态
				std::string m_strCaselevel;					//警情级别
				std::string m_strSourceType;				//警情报警来源类型
				std::string m_strHandleType;				//警情处理类型
				std::string m_strCaseType;					//警情一级类型
				std::string m_strCaseSubType;				//警情二级类型
				std::string m_strCaseThreeType;				//警情三级类型
				std::string m_strCallerNo;					//报警人号码
				std::string m_strCallerName;				//报警人姓名
				std::string m_strCallerAddr;				//报警人地址
				std::string m_strCallerGender;				//报警人性别
				std::string m_strCalled;					//报警号码
				std::string m_strContactNo;					//联系人号码
				std::string m_strContactName;				//联系人姓名
				std::string m_strContactAddr;				//联系人地址
				std::string m_strContactSex;				//联系人性别
				std::string m_strCaseDestrict;				//警情行政区划
				std::string m_strAreaOrg;					//管辖单位代码
				std::string m_strAreaOrgName;				//管辖单位名称
				std::string m_strReceiptCode;				//接警人警号
				std::string m_strReceiptName;				//接警人姓名
				std::string m_strSeatNo;					//席位号
				
				std::string m_strUpdateUser;
				std::string m_strUpdateTime;

			};
			CBody m_oBody;
		};
	}
}
