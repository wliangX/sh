#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/CSetFeedbackRequest.h>
namespace ICC
{
    namespace PROTOCOL
    {
        class CUpdateFeedbackRequest :
			public IRequest, public IRespond
        {
        public:
            virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

                CSetFeedBackRequest::CBody l_pData;
                int l_iCount = p_pJson->GetCount("/body/data");
                for (int i = 0; i < l_iCount; i++)
                {
                    std::string l_strPrefixPath("/body/data/" + std::to_string(i) + "/");
                    
                    l_pData.m_strID = p_pJson->GetNodeValue(l_strPrefixPath + "FKDBH", ""); //反馈单编号
                    l_pData.m_strFeedbackDeptDistrictCode = p_pJson->GetNodeValue(l_strPrefixPath + "XZQHDM", "");//警情所在地行政区划代码
                    l_pData.m_strAlarmID= p_pJson->GetNodeValue(l_strPrefixPath + "JJDBH", "");//接警单编号(外键)
                    l_pData.m_strProcessID = p_pJson->GetNodeValue(l_strPrefixPath + "PJDBH", "");//派警单编号(外键)
                    p_pJson->GetNodeValue(l_strPrefixPath + "FKLYH", "");//反馈录音号 (外键)
                    l_pData.m_strFeedbackDeptCode = p_pJson->GetNodeValue(l_strPrefixPath + "FKDWDM", "");//反馈单位代码
                    l_pData.m_strFeedbackCode = p_pJson->GetNodeValue(l_strPrefixPath + "FKYBH", "");//反馈员编号
                    l_pData.m_strFeedbackName = p_pJson->GetNodeValue(l_strPrefixPath + "FKYXM", "");//反馈员姓名
                    l_pData.m_strTimeSubmit = p_pJson->GetNodeValue(l_strPrefixPath + "FKSJ", "");//反馈时间
                    l_pData.m_strTimePoliceDispatch = p_pJson->GetNodeValue(l_strPrefixPath + "SJCJSJ", "");//出警时间
                    l_pData.m_strTimePoliceArrived = p_pJson->GetNodeValue(l_strPrefixPath + "DDXCSJ", "");//到达现场时间
                    p_pJson->GetNodeValue(l_strPrefixPath + "CLWBSJ", "");//现场处理完毕时间
                    p_pJson->GetNodeValue(l_strPrefixPath + "JQLBDM", "");//警情类别代码
                    p_pJson->GetNodeValue(l_strPrefixPath + "JQLXDM", "");//警情类型代码
                    p_pJson->GetNodeValue(l_strPrefixPath + "JQXLDM", "");//警情细类代码
                    l_pData.m_strActualOccurTime = p_pJson->GetNodeValue(l_strPrefixPath + "JQFSSJ", "");//警情发生时间
                    l_pData.m_strAlarmLongitude = p_pJson->GetNodeValue(l_strPrefixPath + "FKDWXZB", "");//反馈定位X坐标（处置现场位置）
                    l_pData.m_strAlarmLatitude = p_pJson->GetNodeValue(l_strPrefixPath + "FKDWYZB", "");//反馈定位Y坐标（处置现场位置）
                    p_pJson->GetNodeValue(l_strPrefixPath + "CJQK", "");//出警处置状况
                    l_pData.m_strPoliceCarNumDispatch = p_pJson->GetNodeValue(l_strPrefixPath + "CDCLQK", "");//出动车次
                    l_pData.m_strPoliceNumDispatch = p_pJson->GetNodeValue(l_strPrefixPath + "CDRYQK", "");//出动人次
                    l_pData.m_strPeopleNumRescue = p_pJson->GetNodeValue(l_strPrefixPath + "JZRS", "");//救助人数
                    p_pJson->GetNodeValue(l_strPrefixPath + "JZRSSM", "");//救助人数说明
                    p_pJson->GetNodeValue(l_strPrefixPath + "SSRS", "");//受伤人数
                    p_pJson->GetNodeValue(l_strPrefixPath + "SSRSSM", "");//受伤人数说明
                    l_pData.m_strPeopleNumDeath = p_pJson->GetNodeValue(l_strPrefixPath + "SWRS", "");//死亡人数
                    p_pJson->GetNodeValue(l_strPrefixPath + "SWRSSM", "");//死亡人数说明
                    p_pJson->GetNodeValue(l_strPrefixPath + "CLJGDM", "");//警情处理结果代码
                    p_pJson->GetNodeValue(l_strPrefixPath + "JQCLJG", "");//警情处理结果说明，对警情要素及现场处理情况的文字描述
                    p_pJson->GetNodeValue(l_strPrefixPath + "TQQKDM", "");//天气情况代码
                    p_pJson->GetNodeValue(l_strPrefixPath + "SSQKMS", "");//损失情况描述
                    l_pData.m_strPeopleNumCapture = p_pJson->GetNodeValue(l_strPrefixPath + "ZHRS", "");//抓获人数
                    p_pJson->GetNodeValue(l_strPrefixPath + "SARS", "");//涉案人数
                    p_pJson->GetNodeValue(l_strPrefixPath + "TPRS", "");//逃跑人数
                    p_pJson->GetNodeValue(l_strPrefixPath + "JTSGXTDM", "");//交通事故形态代码
                    p_pJson->GetNodeValue(l_strPrefixPath + "SFWHP", "");//是否装载危化品(0：否，1：是，默认为0。)
                    p_pJson->GetNodeValue(l_strPrefixPath + "SGCCYYDM", "");///交通事故初查原因代码
                    p_pJson->GetNodeValue(l_strPrefixPath + "NJDDM", "");//能见度代码
                    p_pJson->GetNodeValue(l_strPrefixPath + "LMZKDM", "");//路面状况代码
                    p_pJson->GetNodeValue(l_strPrefixPath + "SHJDCS", "");//损坏机动车数
                    p_pJson->GetNodeValue(l_strPrefixPath + "SHFJDCS", "");//损坏非机动车数
                    p_pJson->GetNodeValue(l_strPrefixPath + "DLLXDM", "");//道路类型代码
                    p_pJson->GetNodeValue(l_strPrefixPath + "JQZTDM", "");//警情处理状态代码
                    p_pJson->GetNodeValue(l_strPrefixPath + "RKSJ", "");//入库时间
                    p_pJson->GetNodeValue(l_strPrefixPath + "GXSJ", "");//更新时间
                    p_pJson->GetNodeValue(l_strPrefixPath + "JDXZ", "");//街道（乡镇）
                    p_pJson->GetNodeValue(l_strPrefixPath + "SDCS", "");//属地村社
                    p_pJson->GetNodeValue(l_strPrefixPath + "SFQZ", "");//是否取证(0：否，1：是，默认为0。)
                    p_pJson->GetNodeValue(l_strPrefixPath + "YJDW", "");//移交单位
                    p_pJson->GetNodeValue(l_strPrefixPath + "YJDWLXR", "");//移交单位联系人
                    p_pJson->GetNodeValue(l_strPrefixPath + "YJDWFKQK", "");//移交单位反馈情况(类型代码)
                    p_pJson->GetNodeValue(l_strPrefixPath + "YJDWFKQKSM", "");//移交单位反馈情况说明
                    p_pJson->GetNodeValue(l_strPrefixPath + "AJBH", "");//案件编号
                    p_pJson->GetNodeValue(l_strPrefixPath + "NRBQ", "");//内容标签
                    p_pJson->GetNodeValue(l_strPrefixPath + "JQFSDY", "");//警情发生地域
                    p_pJson->GetNodeValue(l_strPrefixPath + "JQFSBW", "");//警情发生部位(代码)
                    p_pJson->GetNodeValue(l_strPrefixPath + "FORCEID", "");//警力id
                    p_pJson->GetNodeValue(l_strPrefixPath + "FORCETYPE", "");//警力类型（人、车、设备类型）
                    p_pJson->GetNodeValue(l_strPrefixPath + "ZHTBBTH", "");//最后同步版本号
                    p_pJson->GetNodeValue(l_strPrefixPath + "DQYWZT", "");//当前业务状态
                    p_pJson->GetNodeValue(l_strPrefixPath + "FKDWIDENTIFIER", "");//反馈单位:组织内部编号


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
                std::vector<CSetFeedBackRequest::CBody> m_vecProcessData;
            };
            CHeaderEx m_oHeader;
            CBody	m_oBody;
        };

    }
}

