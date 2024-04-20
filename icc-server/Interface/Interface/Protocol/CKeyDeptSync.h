#pragma once 

#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CKeyDeptSync :
			public IRespond
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
				p_pJson->SetNodeValue("/body/type", m_oBody.m_strType);
				p_pJson->SetNodeValue("/body/name", m_oBody.m_strName);
				p_pJson->SetNodeValue("/body/phone", m_oBody.m_strPhone);
				p_pJson->SetNodeValue("/body/address", m_oBody.m_strAddress);
				p_pJson->SetNodeValue("/body/longitude", m_oBody.m_strLongitude);
				p_pJson->SetNodeValue("/body/latitude", m_oBody.m_strLattitude);

				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strSyncType;
				std::string m_strType;		//重点单位类型		
				std::string m_strName;     
				std::string m_strPhone;   
				std::string m_strAddress;  
				std::string m_strLongitude; //经度  
				std::string m_strLattitude;	//纬度	
			};
			CBody m_oBody;
		};
	}
}
