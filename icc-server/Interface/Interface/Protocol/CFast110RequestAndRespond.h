#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
    namespace PROTOCOL
    {
        class CAddConferPartyRequest :
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

				p_pJson->SetNodeValue("/body/callref_id", m_oBody.m_strCallrefID);
				p_pJson->SetNodeValue("/body/compere", m_oBody.m_strCompere);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				
				for (size_t i = 0; i < m_oBody.m_vParty.size();i++)
				{
					p_pJson->SetNodeValue("/body/party/" + std::to_string(i) + "/target", m_oBody.m_vParty[i].m_strTarget);
					p_pJson->SetNodeValue("/body/party/" + std::to_string(i) + "/target_device_type", m_oBody.m_vParty[i].m_strTargetDeviceType);
				}

				return p_pJson->ToString();
            }

        public:

			class CBody
			{
			public:
				class CParty
				{
				public:
					std::string m_strTarget;
					std::string m_strTargetDeviceType;		//加入会议的设备类型：inside:内部分机，outside：外线电话,wireless：无线
				};
			public:
				std::string m_strCallrefID;				//话务ID
				std::string m_strCompere;				//会议主持人
				std::string m_strCount;
				std::vector<CParty> m_vParty;
			};
            CHeader m_oHeader;
            CBody	m_oBody;
        };



		class CAddConferPartyRespond :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strResult = p_pJson->GetNodeValue("/body/result", "");

				return true;
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strResult;		//0：表示成功，1:表示失败
			};
			CBody m_oBody;
		};


		class CMakeConferPartySync :
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

				p_pJson->SetNodeValue("/body/callref_id", m_oBody.m_strCallrefID);
				p_pJson->SetNodeValue("/body/compere", m_oBody.m_strCompere);
				p_pJson->SetNodeValue("/body/sponsor", m_oBody.m_strSponsor);
				p_pJson->SetNodeValue("/body/target", m_oBody.m_strTarget);
				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strCallrefID;				//话务ID
				std::string m_strCompere;				//会议主持人
				std::string m_strSponsor;				//会议发起者或者执行新增会议成员操作的坐席号
				std::string m_strTarget;				//会议成员（一个号码，一个回复）
				std::string m_strResult;				//0：表示成功，1:表示失败
			};
			CBody m_oBody;
		};





		class CDeleteConferPartyRequest :
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

				p_pJson->SetNodeValue("/body/callref_id", m_oBody.m_strCallrefID);
				p_pJson->SetNodeValue("/body/compere", m_oBody.m_strCompere);
				p_pJson->SetNodeValue("/body/target", m_oBody.m_strTarget);

				return p_pJson->ToString();
			}

		public:

			class CBody
			{
			public:
				std::string m_strCallrefID;				//话务ID
				std::string m_strCompere;				//会议主持人
				std::string m_strTarget;				//成员号码
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};



		class CDeleteConferPartyRespond :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strResult = p_pJson->GetNodeValue("/body/result", "");

				return true;
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strResult;		//0：表示成功，1:表示失败
			};
			CBody m_oBody;
		};



		class CDeviceStateSync :
			public IRequest
		{

		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strDevice = p_pJson->GetNodeValue("/body/device", "");
				m_oBody.m_strDeviceType = p_pJson->GetNodeValue("/body/device_type", "");
				m_oBody.m_strCallDirection = p_pJson->GetNodeValue("/body/call_direction", "");
				m_oBody.m_strCallRefId = p_pJson->GetNodeValue("/body/callref_id", "");
				m_oBody.m_strCallerId = p_pJson->GetNodeValue("/body/caller_id", "");
				m_oBody.m_strCalledId = p_pJson->GetNodeValue("/body/called_id", "");
				m_oBody.m_strState = p_pJson->GetNodeValue("/body/state", "");
				m_oBody.m_strTime = p_pJson->GetNodeValue("/body/time", "");
				m_oBody.m_strHangupType = p_pJson->GetNodeValue("/body/hangup_type", "");
				m_oBody.m_strHangupDevice = p_pJson->GetNodeValue("/body/hangup_device", "");

				return true;
			}


		public:
			CHeader m_oHeader;

			class CBody
			{
			public:
				std::string m_strDevice;
				std::string m_strDeviceType;
				std::string m_strCallDirection;
				std::string m_strCallRefId;
				std::string m_strCallerId;
				std::string m_strCalledId;
				std::string m_strState;
				std::string m_strTime;
				std::string m_strHangupType;
				std::string m_strHangupDevice;
			};
			CBody m_oBody;
		};


    }
}

