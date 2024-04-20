#pragma once 
#include <Protocol/ISync.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSeatInfoManagerSync :
			public ISync, public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);
				p_pJson->SetNodeValue("/body/no", m_oBody.m_strSeatNo);
				p_pJson->SetNodeValue("/body/name", m_oBody.m_strClientName);
				p_pJson->SetNodeValue("/body/dept_code", m_oBody.m_strDeptCode);
				p_pJson->SetNodeValue("/body/dept_name", m_oBody.m_strDeptName);
				return p_pJson->ToString();
			};
			bool ParseString(std::string p_strRegisterInfo, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson || !p_pJson->LoadJson(p_strRegisterInfo))
				{
					return false;
				}

				m_oBody.m_strSyncType = p_pJson->GetNodeValue("/body/sync_type", "");
				m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/body/no", "");
				m_oBody.m_strClientName = p_pJson->GetNodeValue("/body/name", "");
				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");
				m_oBody.m_strDeptName = p_pJson->GetNodeValue("/body/dept_name", "");
				return true;
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strSyncType;
				std::string m_strSeatNo;
				std::string m_strClientName;
				std::string m_strDeptCode;		//用户所属的单位编码
				std::string m_strDeptName;		//用户所属的单位名称
			};
			CBody m_oBody;
		};
	}
}
