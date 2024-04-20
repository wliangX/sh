#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAddNoticeSync :
			public IReceive, public ISend
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strNoticeId = p_pJson->GetNodeValue("/body/notice_id", "");
				m_oBody.m_strUserCode = p_pJson->GetNodeValue("/body/user_code", "");
				m_oBody.m_strUserName = p_pJson->GetNodeValue("/body/user_name", "");
				m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/body/seat_no", "");

				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");
				m_oBody.m_strDeptName = p_pJson->GetNodeValue("/body/dept_name", "");
				m_oBody.m_strTargetDeptCode = p_pJson->GetNodeValue("/body/target_dept_code", "");
				m_oBody.m_strTargetDeptName = p_pJson->GetNodeValue("/body/target_dept_name", "");

				m_oBody.m_strNoticeType = p_pJson->GetNodeValue("/body/notice_type", "");
				m_oBody.m_strTitle = p_pJson->GetNodeValue("/body/title", "");
				m_oBody.m_strContent = p_pJson->GetNodeValue("/body/content", "");
				m_oBody.m_strTime = p_pJson->GetNodeValue("/body/time", "");

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/notice_id", m_oBody.m_strNoticeId);
				p_pJson->SetNodeValue("/body/user_code", m_oBody.m_strUserCode);
				p_pJson->SetNodeValue("/body/user_name", m_oBody.m_strUserName);
				p_pJson->SetNodeValue("/body/seat_no", m_oBody.m_strSeatNo);

				p_pJson->SetNodeValue("/body/dept_code", m_oBody.m_strDeptCode);
				p_pJson->SetNodeValue("/body/dept_name", m_oBody.m_strDeptName);
				p_pJson->SetNodeValue("/body/target_dept_code", m_oBody.m_strTargetDeptCode);
				p_pJson->SetNodeValue("/body/target_dept_name", m_oBody.m_strTargetDeptName);

				p_pJson->SetNodeValue("/body/notice_type", m_oBody.m_strNoticeType);
				p_pJson->SetNodeValue("/body/title", m_oBody.m_strTitle);
				p_pJson->SetNodeValue("/body/content", m_oBody.m_strContent);
				p_pJson->SetNodeValue("/body/time", m_oBody.m_strTime);

				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string m_strNoticeId;			//公告/通知ID
				std::string m_strUserCode;			//用户ID
				std::string m_strUserName;			//用户姓名
				std::string m_strSeatNo;			//席位号

				std::string m_strDeptCode;			//单位编码
				std::string m_strDeptName;			//单位名称
				std::string m_strTargetDeptCode;	//目标单位编码
				std::string m_strTargetDeptName;	//目标单位名称

				std::string m_strNoticeType;		//通知类型（1：公告，2：通知）
				std::string m_strTitle;				//标题
				std::string m_strContent;			//公告/通知内容
				std::string m_strTime;				//时间
			};

			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}