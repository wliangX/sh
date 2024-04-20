#pragma once 
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
        class CPushLanguageRequest :
			public IRespond
		{
		public:
            virtual std::string ToString(IJsonPtr p_pJson)
            {
                m_oHeader.SaveTo(p_pJson);
                p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
                p_pJson->SetNodeValue("/body/code", m_oBody.m_strCode);
                p_pJson->SetNodeValue("/body/name", m_oBody.m_strName);
                p_pJson->SetNodeValue("/body/sort", m_oBody.m_strSort);
                return p_pJson->ToString();
            }

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strGuid;
				std::string m_strCode;
				std::string m_strName;
                std::string m_strSort;
			};
			CBody m_oBody;
		};
	}
}
