#pragma once
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSetContactGrpRequest :
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
				m_oBody.m_strTitle = p_pJson->GetNodeValue("/body/title", "");
				m_oBody.m_strUserCode = p_pJson->GetNodeValue("/body/user_code", "");
				m_oBody.m_strType = p_pJson->GetNodeValue("/body/type", "");
				m_oBody.m_strSort = p_pJson->GetNodeValue("/body/sort", "");

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
				p_pJson->SetNodeValue("/body/title", m_oBody.m_strTitle);
				p_pJson->SetNodeValue("/body/user_code", m_oBody.m_strUserCode);
				p_pJson->SetNodeValue("/body/type", m_oBody.m_strType);
				p_pJson->SetNodeValue("/body/sort", m_oBody.m_strSort);

				return p_pJson->ToString();
			}
		public:
			class CBody
			{
			public:
				std::string m_strSyncType;		//1：表示添加，2：表示更新, 3:表示删除
				std::string m_strGuid;			//组guid
				std::string m_strTitle;			//标题
				std::string m_strUserCode;		//坐席编号警员编码
				std::string m_strType;			//1：UserCode是坐席号，2：登录用户的警员编号
				std::string m_strSort;			//警员序号，一般从1开始排序
			};
			CHeaderEx m_oHeader;			
			CBody	m_oBody;
		};

		typedef CSetContactGrpRequest CContactGrpInfo;
	}
}
