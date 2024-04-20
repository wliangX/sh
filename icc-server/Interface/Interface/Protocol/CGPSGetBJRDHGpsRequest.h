#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGPSGetBJRDHGpsRequest :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strCallrefID = p_pJson->GetNodeValue("/body/callref_id", "");
				m_oBody.m_strXM = p_pJson->GetNodeValue("/body/xm", "");
				m_oBody.m_strSFZH = p_pJson->GetNodeValue("/body/sfzh", "");
				m_oBody.m_strJGDM = p_pJson->GetNodeValue("/body/jgdm", "");
				m_oBody.m_strPhoneNo = p_pJson->GetNodeValue("/body/phone_no", "");
				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strCallrefID;			//话务ID
				std::string m_strXM;			    //姓名
				std::string m_strSFZH;			    //身份证号
				std::string m_strJGDM;			    //机构代码
				std::string m_strPhoneNo;           //电话号码
			};
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};		
	}
}
