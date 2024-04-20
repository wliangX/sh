#pragma once 
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"
#include <Protocol/CAddOrUpdateProcessRequest.h>
namespace ICC
{
	namespace PROTOCOL
	{
        class CProcessInfoChange :
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

                for (size_t i = 0; i < m_oBody.m_vecData.size(); i++)
                {
                    std::string l_strIndex = std::to_string(i);

                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/PJDBH", m_oBody.m_vecData[i].m_strID);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/XZQHDM", m_oBody.m_vecData[i].m_strDispatchDeptDistrictCode);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/JJDBH", m_oBody.m_vecData[i].m_strAlarmID);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/ZXBH", m_oBody.m_vecData[i].m_strSeatCode);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/PJDWDM", m_oBody.m_vecData[i].m_strDispatchDeptCode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/PJDWID", m_oBody.m_vecData[i].m_strDispatchDeptCode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/PJDWMC", m_oBody.m_vecData[i].m_strDispatchDeptName);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/PJYBH", m_oBody.m_vecData[i].m_strDispatchCode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/PJYID", m_oBody.m_vecData[i].m_strDispatchCode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/PJYXM", m_oBody.m_vecData[i].m_strDispatchName);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/PJYJ", m_oBody.m_vecData[i].m_strDispatchSuggestion);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/CJSJ", m_oBody.m_vecData[i].m_strCreateTime);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/CJDWDM", m_oBody.m_vecData[i].m_strProcessDeptCode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/CJDWID", m_oBody.m_vecData[i].m_strProcessDeptCode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/CJDWMC", m_oBody.m_vecData[i].m_strProcessDeptName);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/JQCLZTDM", m_oBody.m_vecData[i].m_strState);

                   
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/createuser", m_oBody.m_vecData[i].m_strCreateUser);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/RKSJ", m_oBody.m_vecData[i].m_strCreateTime);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/updateuser", m_oBody.m_vecData[i].m_strUpdateUser);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/GXSJ", m_oBody.m_vecData[i].m_strUpdateTime);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/delflag", m_oBody.m_vecData[i].m_strDelFlag);
                }

                return p_pJson->ToString();
            }


		public:
			CHeader m_oHeader;

            class CBody
            {
			public:
                std::vector<CAddOrUpdateProcessRequest::CProcessData> m_vecData;
            };

			CBody m_oBody;
		};
	}
}
