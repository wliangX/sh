#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSetKeyDeptRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strType =   p_pJson->GetNodeValue("/body/type","");
				m_oBody.m_strName =  p_pJson->GetNodeValue("/body/name","");
				m_oBody.m_strPhone =p_pJson->GetNodeValue("/body/phone","");
				m_oBody.m_strAddress = p_pJson->GetNodeValue("/body/address", "");
				m_oBody.m_strLongitude =p_pJson->GetNodeValue("/body/longitude","");
				m_oBody.m_strLattitude = p_pJson->GetNodeValue("/body/latitude", "");

				return true;
			}
		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
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
