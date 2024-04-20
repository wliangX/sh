#pragma once 
#include <Protocol/ISync.h>

namespace ICC
{
	namespace PROTOCOL
	{
        class CSeatSync :
			public ISync
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);

                p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);
                p_pJson->SetNodeValue("/body/no", m_oBody.m_strNo);
				p_pJson->SetNodeValue("/body/name", m_oBody.m_strName);
                p_pJson->SetNodeValue("/body/dept_code", m_oBody.m_strDeptCode);
                p_pJson->SetNodeValue("/body/dept_name", m_oBody.m_strDeptName);
				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
                std::string m_strSyncType;
                std::string m_strNo;
				std::string m_strName;
                std::string m_strDeptCode;
                std::string m_strDeptName;
			};
			CBody m_oBody;
		};
	}
}
