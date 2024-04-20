#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
#include <Protocol/CAlarmInfo.h>
#include <Protocol/CAddOrUpdateProcessRequest.h>
#include <Protocol/CAlarmLogSync.h>
namespace ICC
{
	namespace PROTOCOL
	{
		class CSetTemporaryHoldRequest : public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strCaseID = p_pJson->GetNodeValue("/body/caseid", "");
				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strCaseID;
			};

			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};

		class CSetTemporaryHoldRespond :public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strResult;
			};
			CBody m_oBody;
		};
		
		class CSetTemporaryHoldSync :public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
            {
				if (nullptr == p_pJson)
				{
					return "";
				}

                m_oHeader.SaveTo(p_pJson);
                p_pJson->SetNodeValue("/body/caseid", m_oBody.m_strCaseID);
                return p_pJson->ToString();
            }

		public:
			CHeader m_oHeader;
            class CBody
            {
            public:
                std::string m_strCaseID;					//¾¯Çéid
            };
			CBody m_oBody;
		};
		//////////////////////////////////////////////////////////////////////////////

	}
}
