#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CSmpHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
        class CGetStaffInfoRequest :
            public IRequest
        {
        public:
            bool ParseString(std::string p_strJson, JsonParser::IJsonPtr p_pJson)
            {
				if (!m_Header.ParseString(p_strJson, p_pJson))
				{
					return false;
				}
                m_Body.m_strCount = p_pJson->GetNodeValue("/body/count", "");
				m_Body.m_strVersionData = p_pJson->GetNodeValue("/body/version_data", "");
                int l_iCount = atoi(m_Body.m_strCount.c_str());
                for (int i = 0; i < p_pJson->GetCount("/body/data"); i++)
                {
                    std::string l_strPath = "/body/data/";
                    std::string l_strNum = std::to_string(i);
                    CBody::CData l_CData;
                    l_CData.m_strGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/guid", "");
                    l_CData.m_strStaffCode = p_pJson->GetNodeValue(l_strPath + l_strNum + "/staff_code", "");
                    l_CData.m_strStaffName = p_pJson->GetNodeValue(l_strPath + l_strNum + "/staff_name", "");
                    l_CData.m_strStaffType = p_pJson->GetNodeValue(l_strPath + l_strNum + "/staff_type", "");
                    l_CData.m_strCategory = p_pJson->GetNodeValue(l_strPath + l_strNum + "/category", "");
                    l_CData.m_strStaffStatus = p_pJson->GetNodeValue(l_strPath + l_strNum + "/staff_status", "");
                    l_CData.m_strOrgGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/org_guid", "");
                    l_CData.m_strFromOrgGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/from_org_guid", "");
                    l_CData.m_strStartTime = p_pJson->GetNodeValue(l_strPath + l_strNum + "/start_time", "");
                    l_CData.m_strEndTime = p_pJson->GetNodeValue(l_strPath + l_strNum + "/end_time", "");
                    l_CData.m_strInterphoneGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/interphone_guid", "");
                    l_CData.m_strBindType = p_pJson->GetNodeValue(l_strPath + l_strNum + "/bind_type", "");
                    l_CData.m_strBindConfirm = p_pJson->GetNodeValue(l_strPath + l_strNum + "/bind_confirm", "");
                    l_CData.m_strSeq = p_pJson->GetNodeValue(l_strPath + l_strNum + "/seq", "");
                    l_CData.m_strPosition = p_pJson->GetNodeValue(l_strPath + l_strNum + "/position", "");
                    l_CData.m_strCreateUser = p_pJson->GetNodeValue(l_strPath + l_strNum + "/create_user", "");
                    l_CData.m_strCreateTime = p_pJson->GetNodeValue(l_strPath + l_strNum + "/create_time", "");
                    l_CData.m_strUpdateUser = p_pJson->GetNodeValue(l_strPath + l_strNum + "/update_user", "");
                    l_CData.m_strUpdateTime = p_pJson->GetNodeValue(l_strPath + l_strNum + "/update_time", "");
                    l_CData.m_strRemark = p_pJson->GetNodeValue(l_strPath + l_strNum + "/remark", "");
                    l_CData.m_strSex = p_pJson->GetNodeValue(l_strPath + l_strNum + "/sex", "");
                    l_CData.m_strIdNo = p_pJson->GetNodeValue(l_strPath + l_strNum + "/id_no", "");
                    l_CData.m_strTelephone = p_pJson->GetNodeValue(l_strPath + l_strNum + "/telephone", "");
                    l_CData.m_strMobile = p_pJson->GetNodeValue(l_strPath + l_strNum + "/mobile", "");
                    l_CData.m_strAddress = p_pJson->GetNodeValue(l_strPath + l_strNum + "/address", "");
                    l_CData.m_strImage = p_pJson->GetNodeValue(l_strPath + l_strNum + "/image", "");
					l_CData.m_strEnableFlag = p_pJson->GetNodeValue(l_strPath + l_strNum + "/enable_flag", "");
                    l_CData.m_strOrgIdentifier = p_pJson->GetNodeValue(l_strPath + l_strNum + "/org_identifier", "");
                    l_CData.m_strXjType = p_pJson->GetNodeValue(l_strPath + l_strNum + "/xj_type", "");
                    l_CData.m_strIsLeader = p_pJson->GetNodeValue(l_strPath + l_strNum + "/isleader", "");

                    m_Body.m_vecData.push_back(l_CData);
                }
                return true;
            }
        public:

            class CBody
            {
            public:
                class CData
                {
                public:
                    std::string m_strGuid;				std::string m_strStaffCode;
                    std::string m_strStaffName;			std::string m_strStaffType;
                    std::string m_strCategory;			std::string m_strStaffStatus;
                    std::string m_strOrgGuid;			std::string m_strFromOrgGuid;
                    std::string m_strStartTime;			std::string m_strEndTime;
                    std::string m_strInterphoneGuid;	std::string m_strBindType;
                    std::string m_strBindConfirm;		std::string m_strSeq;
                    std::string m_strPosition;			std::string m_strSex;
                    std::string m_strIdNo;				std::string m_strTelephone;
                    std::string m_strMobile;			std::string m_strAddress;
                    std::string m_strImage;				std::string m_strCreateUser;
                    std::string m_strCreateTime;		std::string m_strUpdateUser;
                    std::string m_strUpdateTime;		std::string m_strRemark;
					std::string m_strEnableFlag;        std::string m_strOrgIdentifier;
					std::string m_strXjType;            std::string m_strIsLeader;
                };
                std::vector<CData>m_vecData;
                std::string m_strCount;
				std::string m_strVersionData;
            };
            CSmpHeader m_Header;
            CBody m_Body;
        };
	}
}
