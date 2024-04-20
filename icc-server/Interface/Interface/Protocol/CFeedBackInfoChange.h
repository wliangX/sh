#pragma once 
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
        class CFeedBackInfoChange :
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
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/FKDBH", m_oBody.m_vecData[i].m_strID);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/JJDBH", m_oBody.m_vecData[i].m_strCaseId);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/PJDBH", m_oBody.m_vecData[i].m_strDisPosalId);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/casedispatchstatus", m_oBody.m_vecData[i].m_strCaseDispatchStatus);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/FORCEID", m_oBody.m_vecData[i].m_strInterphoneId);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/fbway", m_oBody.m_vecData[i].m_strFBWay);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/fbcontent", m_oBody.m_vecData[i].m_strFBContent);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/FKYBH", m_oBody.m_vecData[i].m_strFeedBacker);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/FKDWDM", m_oBody.m_vecData[i].m_strFeedBackerOrg);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/FKSJ", m_oBody.m_vecData[i].m_strFBTime);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/JQLBDM", m_oBody.m_vecData[i].m_strCaseType);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/JQLXDM", m_oBody.m_vecData[i].m_strCaseSubType);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/JQXLDM", m_oBody.m_vecData[i].m_strCaseThreeType);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/case_processor", m_oBody.m_vecData[i].m_strCaseProcessor);
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
                class CData
                {
                public:
                public:
                    std::string m_strID;
                    std::string m_strCaseId;					//警情ID
                    std::string m_strDisPosalId;				//处警单ID
                    std::string m_strCaseDispatchStatus;		//反馈类型
                    std::string m_strInterphoneId;              //设备ID列表
                    std::string m_strFBWay;                     //反馈方式:1终端快捷键；2短信；3电话
                    std::string m_strFBContent;                 //反馈内容
                    std::string m_strFeedBacker;                //反馈人
                    std::string m_strFeedBackerOrg;             //反馈单位编码
                    std::string m_strFBTime;                    //反馈时间
                    std::string m_strCaseType;					//案件类型
                    std::string m_strCaseSubType;				//案件中类
                    std::string m_strCaseThreeType;				//案件小类
                    std::string m_strCaseProcessor;				//处警人列表（逗号分割）
                    std::string m_strCreateUser;				//创建人
                    std::string m_strCreateTime;				//创建时间
                    std::string m_strUpdateUser;				//修改人,取最后一次修改值
                    std::string m_strUpdateTime;				//修改时间,取最后一次修改值
                    std::string m_strDelFlag;				    //0有效，1删除
                };

                std::vector<CData> m_vecData;
            };
			CBody m_oBody;
		};
	}
}
