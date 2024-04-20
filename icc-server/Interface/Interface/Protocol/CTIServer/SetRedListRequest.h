#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSetRedListRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strId = p_pJson->GetNodeValue("/body/id", "");
				m_oBody.m_strPhoneNum = p_pJson->GetNodeValue("/body/phone_num", "");
				m_oBody.m_strName = p_pJson->GetNodeValue("/body/name", "");
				m_oBody.m_strDepartment = p_pJson->GetNodeValue("/body/department", "");
				m_oBody.m_strPosition = p_pJson->GetNodeValue("/body/position", "");
				m_oBody.m_strRemark = p_pJson->GetNodeValue("/body/remark", "");
				m_oBody.m_strSyncType = p_pJson->GetNodeValue("/body/sync_type", "");

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

			//	p_pJson->SetNodeValue("/body/id", m_oBody.m_strId);
				p_pJson->SetNodeValue("/body/phone_num", m_oBody.m_strPhoneNum);
				p_pJson->SetNodeValue("/body/name", m_oBody.m_strName);
				p_pJson->SetNodeValue("/body/department", m_oBody.m_strDepartment);
				p_pJson->SetNodeValue("/body/position", m_oBody.m_strPosition);
				p_pJson->SetNodeValue("/body/remark", m_oBody.m_strRemark); 
				p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);

				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strId;			//GUID
				std::string m_strPhoneNum;		//电话
				std::string m_strName;			//姓名
				std::string m_strDepartment;	//部门
				std::string m_strPosition;		//职务
				std::string m_strRemark;		//备注
				std::string m_strSyncType;		//同步类型，1：添加，2：更新，3：删除
			};
			CBody m_oBody;
		};

		class CSetRedListSync :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strId = p_pJson->GetNodeValue("/body/id", "");
				m_oBody.m_strPhoneNum = p_pJson->GetNodeValue("/body/phone_num", "");
				m_oBody.m_strName = p_pJson->GetNodeValue("/body/name", "");
				m_oBody.m_strDepartment = p_pJson->GetNodeValue("/body/department", "");
				m_oBody.m_strPosition = p_pJson->GetNodeValue("/body/position", "");
				m_oBody.m_strRemark = p_pJson->GetNodeValue("/body/remark", "");
				m_oBody.m_strSyncType = p_pJson->GetNodeValue("/body/sync_type", "");

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

			//	p_pJson->SetNodeValue("/body/id", m_oBody.m_strId);
				p_pJson->SetNodeValue("/body/phone_num", m_oBody.m_strPhoneNum);
				p_pJson->SetNodeValue("/body/name", m_oBody.m_strName);
				p_pJson->SetNodeValue("/body/department", m_oBody.m_strDepartment);
				p_pJson->SetNodeValue("/body/position", m_oBody.m_strPosition);
				p_pJson->SetNodeValue("/body/remark", m_oBody.m_strRemark);
				p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);

				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;

			class CBody
			{
			public:
				std::string m_strId;			//GUID
				std::string m_strPhoneNum;		//电话
				std::string m_strName;			//姓名
				std::string m_strDepartment;	//部门
				std::string m_strPosition;		//职务
				std::string m_strRemark;		//备注
				std::string m_strSyncType;		//同步类型，1：添加，2：更新，3：删除
			};
			CBody m_oBody;
		};
	}
}
