#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
    namespace PROTOCOL
    {
        class CAlarmTransportAcceptRequest :
            public IRequest
        {
        public:
            virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

                m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/alarm_id", "");
				m_oBody.m_strUserCode = p_pJson->GetNodeValue("/body/user_code", "");
				m_oBody.m_strUserName = p_pJson->GetNodeValue("/body/user_name", "");
				m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/body/seat_no", "");
				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");
				m_oBody.m_strDeptName = p_pJson->GetNodeValue("/body/dept_name", "");
                return true;
            }

        public:

            class CBody
            {
            public:
                std::string m_strAlarmID;				
                std::string m_strUserCode;			
                std::string m_strUserName;			
                std::string m_strSeatNo;		
                std::string m_strDeptCode;		
                std::string m_strDeptName;						
            };
            CHeaderEx m_oHeader;
            CBody	m_oBody;
        };


		class CAlarmTransportRefuseRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}

				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/alarm_id", "");
				m_oBody.m_strUserCode = p_pJson->GetNodeValue("/body/user_code", "");
				m_oBody.m_strUserName = p_pJson->GetNodeValue("/body/user_name", "");
				m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/body/seat_no", "");
				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");
				m_oBody.m_strDeptName = p_pJson->GetNodeValue("/body/dept_name", "");
				m_oBody.m_strRefuseReason = p_pJson->GetNodeValue("/body/refuse_reason", "");
				return true;
			}

		public:

			class CBody
			{
			public:
				std::string m_strAlarmID;
				std::string m_strUserCode;
				std::string m_strUserName;
				std::string m_strSeatNo;
				std::string m_strDeptCode;
				std::string m_strDeptName;
				std::string m_strRefuseReason;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};



		class CAlarmTransportCacheRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}
				m_oBody.m_strUserCode = p_pJson->GetNodeValue("/body/user_code", "");
				m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/body/seat_no", "");
				return true;
			}

		public:

			class CBody
			{
			public:
				std::string m_strUserCode;		//发起者席位号
				std::string m_strSeatNo;		//发起者坐席号
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
    }
}

