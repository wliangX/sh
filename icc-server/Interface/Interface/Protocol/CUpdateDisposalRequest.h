#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/CAddOrUpdateProcessRequest.h>
namespace ICC
{
    namespace PROTOCOL
    {
        class CUpdateDisposalRequest :
			public IRequest, public IRespond
        {
        public:
            virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

                CAddOrUpdateProcessRequest::CProcessData l_pData;
                l_pData.m_bIsNewProcess = false;
                int l_iCount = p_pJson->GetCount("/body/data");
                for (int i = 0; i < l_iCount; i++)
                {
                    std::string l_strPrefixPath("/body/data/" + std::to_string(i) + "/");

                    l_pData.m_strID = p_pJson->GetNodeValue(l_strPrefixPath + "PJDBH", "");  //派警单编号
                    //l_pData.m_strProcessDeptDistrictCode = p_pJson->GetNodeValue(l_strPrefixPath + "XZQHDM", "");  //警情所在地行政区划代码
                    l_pData.m_strAlarmID = p_pJson->GetNodeValue(l_strPrefixPath + "JJDBH", "");   //接警单编号

                    l_pData.m_strDispatchDeptCode = p_pJson->GetNodeValue(l_strPrefixPath + "PJDWDM", "");  //派警单位机构代码
                    l_pData.m_strDispatchCode = p_pJson->GetNodeValue(l_strPrefixPath + "PJYBH", "");   //派警员编号
                    l_pData.m_strDispatchName = p_pJson->GetNodeValue(l_strPrefixPath + "PJYXM", "");  //派警员姓名
                    p_pJson->GetNodeValue(l_strPrefixPath + "PJLYH", "");   //派警录音号
                    l_pData.m_strDispatchSuggestion = p_pJson->GetNodeValue(l_strPrefixPath + "PJYJ", "");   //派警意见
                    l_pData.m_strProcessDeptCode = p_pJson->GetNodeValue(l_strPrefixPath + "CJDWDM", "");  //出警单位代码
                    l_pData.m_strCreateTime = p_pJson->GetNodeValue(l_strPrefixPath + "CJSJ", "");    //派警时间
                    //l_pData.m_strTimeEdit = p_pJson->GetNodeValue(l_strPrefixPath + "PDJSSJ", "");  //派单接收时间（网络下达派警单后，出警单位手工确认接到该派警单的时间）
                    p_pJson->GetNodeValue(l_strPrefixPath + "CDRY", "");   //出动人员说明
                    p_pJson->GetNodeValue(l_strPrefixPath + "CDCL", "");   //出动车辆说明
                    l_pData.m_strState = p_pJson->GetNodeValue(l_strPrefixPath + "JQCLZTDM", ""); //警情处理状态代码
                    p_pJson->GetNodeValue(l_strPrefixPath + "RKSJ", "");    //入库时间
                    l_pData.m_strUpdateTime = p_pJson->GetNodeValue(l_strPrefixPath + "GXSJ", "");    //更新时间
                    l_pData.m_strDispatchCode = p_pJson->GetNodeValue(l_strPrefixPath + "PJYID", "");   //派警员用户id
                    p_pJson->GetNodeValue(l_strPrefixPath + "ZHTBBTH", "");  //最后同步版本号
                    l_pData.m_strDispatchDeptCode = p_pJson->GetNodeValue(l_strPrefixPath + "PJDWID", "");   //派警单位id
                    l_pData.m_strDispatchDeptName = p_pJson->GetNodeValue(l_strPrefixPath + "PJDWMC", "");   //派警单位名称
                    l_pData.m_strProcessCode = p_pJson->GetNodeValue(l_strPrefixPath + "CJDXID", "");   //出警对象id

                    l_pData.m_strProcessDeptName = p_pJson->GetNodeValue(l_strPrefixPath + "CJDXMC", "");   //出警对象名称  -->//处警人姓名
                    //l_pData.m_strProcessDeptName;

                    p_pJson->GetNodeValue(l_strPrefixPath + "CJDXLX", "");   //出警对象类型

                    p_pJson->GetNodeValue(l_strPrefixPath + "DQYWZT", "");   //当前业务状态

                    p_pJson->GetNodeValue(l_strPrefixPath + "KJDPBH", "");  //跨级调派ID
                    p_pJson->GetNodeValue(l_strPrefixPath + "TDSJ", "");    //退单申请时间
                    //l_pData.m_strSeatNo = p_pJson->GetNodeValue(l_strPrefixPath + "ZXBH", "");    //坐席编号
                    l_pData.m_strProcessDeptCode = p_pJson->GetNodeValue(l_strPrefixPath + "CJDWID", "");   //出警单位ID

                    p_pJson->GetNodeValue(l_strPrefixPath + "PJDWIDENTIFIER", ""); //派警单位:组织内部编号
                    p_pJson->GetNodeValue(l_strPrefixPath + "CJDWIDENTIFIER", ""); //出警单位:组织内部编号
                    p_pJson->GetNodeValue(l_strPrefixPath + "IsDispathNotify", "");
                    p_pJson->GetNodeValue(l_strPrefixPath + "BYAUTOASSIGNJOB", "");

                    l_pData.m_strUpdateType = p_pJson->GetNodeValue(l_strPrefixPath + "update_type", "");
                    m_oBody.m_vecProcessData.push_back(l_pData);
                }

                return true;
            }

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				return p_pJson->ToString();
			}

        public:
            class CBody
            {
            public:
                std::vector<CAddOrUpdateProcessRequest::CProcessData> m_vecProcessData;
            };
            CHeaderEx m_oHeader;
            CBody	m_oBody;
        };

    }
}

