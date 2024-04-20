#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CBJRDHPositionRespond : public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{		
				if (nullptr == p_pJson)
				{
					return false;
				}
				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}

				int count = p_pJson->GetCount("/data");
				for (int i = 0; i < count; i++)
				{
					std::string l_strPrefixPath("/data/" + std::to_string(i) + "/");
					CPosition l_oData;
					l_oData.m_strArea = p_pJson->GetNodeValue(l_strPrefixPath + "area", "");
					l_oData.m_strCallingNumber = p_pJson->GetNodeValue(l_strPrefixPath + "callingnumber", "");
					l_oData.m_strAddress = p_pJson->GetNodeValue(l_strPrefixPath + "address", "");
					l_oData.m_strLng = p_pJson->GetNodeValue(l_strPrefixPath + "lng", "");
					l_oData.m_strCalledNumber = p_pJson->GetNodeValue(l_strPrefixPath + "callednumber", "");
					l_oData.m_strCi = p_pJson->GetNodeValue(l_strPrefixPath + "ci", "");
					l_oData.m_strCallingTime = p_pJson->GetNodeValue(l_strPrefixPath + "callingtime", "");
					l_oData.m_strLat = p_pJson->GetNodeValue(l_strPrefixPath + "lat", "");
					l_oData.m_strLac = p_pJson->GetNodeValue(l_strPrefixPath + "lac", "");

					m_vecData.push_back(l_oData);
				}
				return true;
			}

		public:
			class CPosition
			{
			public:
				std::string	m_strArea;
				std::string	m_strCallingNumber;
				std::string m_strAddress;
				std::string m_strLng;
				std::string m_strCalledNumber;
				std::string m_strCi;
				std::string m_strCallingTime;
				std::string m_strLat;
				std::string m_strLac;
			};

			std::vector<CPosition> m_vecData;
		};

		class CBJRDHPositionRespondJZ : public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}
				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}
				
				// int count = p_pJson->GetCount("/data");
				std::string l_strPrefixPath("/data/" + std::to_string(0) + "/");
				CPosition l_oData;
				l_oData.m_strStatus = p_pJson->GetNodeValue(l_strPrefixPath + "status", "");
				l_oData.m_strTime = p_pJson->GetNodeValue(l_strPrefixPath + "time", "");
				l_oData.m_strInfo = p_pJson->GetNodeValue(l_strPrefixPath + "info", "");
				l_oData.m_strCaller = p_pJson->GetNodeValue(l_strPrefixPath + "caller", "");
				l_oData.m_strCalled = p_pJson->GetNodeValue(l_strPrefixPath + "called", "");
				l_oData.m_strLocationId = p_pJson->GetNodeValue(l_strPrefixPath + "locationId", "");
				l_oData.m_strLng = p_pJson->GetNodeValue(l_strPrefixPath + "lng", "");
				l_oData.m_strLat = p_pJson->GetNodeValue(l_strPrefixPath + "lat", "");
				l_oData.m_strCallTime = p_pJson->GetNodeValue(l_strPrefixPath + "callTime", "");
				l_oData.m_strLatLngType = p_pJson->GetNodeValue(l_strPrefixPath + "latLngType", "");
				l_oData.m_strAddress = p_pJson->GetNodeValue(l_strPrefixPath + "address", "");
				l_oData.m_strShortAddress = p_pJson->GetNodeValue(l_strPrefixPath + "shortAddress", "");
				l_oData.m_strOrgan = p_pJson->GetNodeValue(l_strPrefixPath + "organ", "");

				m_vecData.push_back(l_oData);
				return true;
			}

		public:
			class CPosition
			{
			public:
				std::string	m_strStatus;
				std::string	m_strTime;
				std::string	m_strInfo;
				std::string m_strCaller;
				std::string m_strCalled;
				std::string m_strLocationId;
				std::string m_strLng;
				std::string m_strLat;
				std::string m_strCallTime;
				std::string m_strLatLngType;
				std::string m_strAddress;
				std::string m_strShortAddress;
				std::string m_strOrgan;
			};

			std::vector<CPosition> m_vecData;
		};

		class CLonLoatByAddrInfo : public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.strCode = p_pJson->GetNodeValue("/code", "");
				m_oBody.strMessage = p_pJson->GetNodeValue("/message", "");
				int iCount = p_pJson->GetCount("/data/records");

				if (iCount > 0)
				{
					std::string tmp_path("/data/records/" + std::to_string(0));
					m_oBody.strLongitude = p_pJson->GetNodeValue(tmp_path + "/location/0", "");
					m_oBody.strLatitude = p_pJson->GetNodeValue(tmp_path + "/location/1", "");
					m_oBody.strAddress = p_pJson->GetNodeValue(tmp_path + "/address", "");
				}

				return true;
			}
		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string strCode;            
				std::string strMessage;         

				std::string strLongitude;       // 经度
				std::string strLatitude;        // 维度
				std::string strAddress;         // 基础位置
			};
			CBody m_oBody;
		};

	}
}
