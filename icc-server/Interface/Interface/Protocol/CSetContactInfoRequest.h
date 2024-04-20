#pragma once
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSetContactInfoRequest :
			public IRequest,public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strSyncType = p_pJson->GetNodeValue("/body/sync_type", "");
				m_oBody.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				m_oBody.m_strGrpGuid = p_pJson->GetNodeValue("/body/group_guid", "");
				m_oBody.m_strType = p_pJson->GetNodeValue("/body/type", "");
				m_oBody.m_strName = p_pJson->GetNodeValue("/body/name", "");
				m_oBody.m_strPhone = p_pJson->GetNodeValue("/body/phone", "");
				m_oBody.m_strSort = p_pJson->GetNodeValue("/body/sort", "");
				m_oBody.m_strRemark = p_pJson->GetNodeValue("/body/remark", "");

				return true;
			}
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);
				p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
				p_pJson->SetNodeValue("/body/group_guid", m_oBody.m_strGrpGuid);
				p_pJson->SetNodeValue("/body/type", m_oBody.m_strType);
				p_pJson->SetNodeValue("/body/name", m_oBody.m_strName);
				p_pJson->SetNodeValue("/body/phone", m_oBody.m_strPhone);
				p_pJson->SetNodeValue("/body/sort", m_oBody.m_strSort);
				p_pJson->SetNodeValue("/body/remark", m_oBody.m_strRemark);

				return p_pJson->ToString();
			}
		public:
			class CBody
			{
			public:
				std::string m_strGuid;			//guid
				std::string m_strGrpGuid;		//所属组guid
				std::string m_strType;			//1：UserCode是坐席号，2：登录用户的警员编号
				std::string m_strName;			//名称
				std::string m_strPhone;			//联系电话
				std::string m_strSort;			//编号
				std::string m_strRemark;		//备注
				std::string m_strSyncType;
			};
			CHeaderEx m_oHeader;			
			CBody	m_oBody;
		};

		typedef CSetContactInfoRequest CContactInfo;
	}
}
