#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTAAddAlarmRequest :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				m_oBody.m_strType = p_pJson->GetNodeValue("/body/type", "");
				m_oBody.m_strAddress = p_pJson->GetNodeValue("/body/address", "");
				m_oBody.m_strCrcontent = p_pJson->GetNodeValue("/body/content", "");
				m_oBody.m_strIsPhone = p_pJson->GetNodeValue("/body/is_phone", "");
				m_oBody.m_strCreateDate = p_pJson->GetNodeValue("/body/create_date", "");
				m_oBody.m_strUserCode = p_pJson->GetNodeValue("/body/user_code", "");
				m_oBody.m_strUserName = p_pJson->GetNodeValue("/body/user_name", "");
				m_oBody.m_strUserPhone = p_pJson->GetNodeValue("/body/user_phone", "");
				m_oBody.m_strUserAddress = p_pJson->GetNodeValue("/body/user_address", "");
				m_oBody.m_strUnitContactor = p_pJson->GetNodeValue("/body/unit_contactor", "");
				m_oBody.m_strHandPhone = p_pJson->GetNodeValue("/body/hand_phone", "");
				m_oBody.m_strToOrgID = p_pJson->GetNodeValue("/body/to_org_id", "");
				m_oBody.m_strToOrgName = p_pJson->GetNodeValue("/body/to_org_name", "");
				m_oBody.m_strToUserID = p_pJson->GetNodeValue("/body/to_user_id", "");
				m_oBody.m_strToUserName = p_pJson->GetNodeValue("/body/to_user_name", "");
				m_oBody.m_strState = p_pJson->GetNodeValue("/body/state", "");
				m_oBody.m_strComNo = p_pJson->GetNodeValue("/body/com_no", "");
				m_oBody.m_strOpenID = p_pJson->GetNodeValue("/body/open_id", "");
				m_oBody.m_strLong = p_pJson->GetNodeValue("/body/long", "");
				m_oBody.m_strLat = p_pJson->GetNodeValue("/body/lat", "");
				m_oBody.m_strRemark1 = p_pJson->GetNodeValue("/body/remark1", "");
				m_oBody.m_strRemark2 = p_pJson->GetNodeValue("/body/remark2", "");
				//2022-02-14修改短信报警的字典类型
				if (m_oBody.m_strType != "06")//DIC001003
				{
					int l_uiCount = p_pJson->GetCount("/body/material_list");
					for (int i = 0; i < l_uiCount; ++i)
					{
						std::string l_strfixPath = "/body/material_list/" + std::to_string(i) + "/";
						CMaterial l_oMaterial;
						l_oMaterial.m_strFGuid = p_pJson->GetNodeValue(l_strfixPath + "f_guid", "");
						l_oMaterial.m_strFName = p_pJson->GetNodeValue(l_strfixPath + "f_name", "");
						l_oMaterial.m_strAttachType = p_pJson->GetNodeValue(l_strfixPath + "attach_type", "");
						l_oMaterial.m_strAttachPath = p_pJson->GetNodeValue(l_strfixPath + "attach_path", "");

						m_oBody.m_vecMaterial.push_back(l_oMaterial);
					}
				}				

				return true;
			}

		public:
			class CMaterial
			{
			public:
				std::string		m_strFGuid;					//附件ID
				std::string		m_strFName;					//附件名称
				std::string		m_strAttachType;			//附件类型 0:图片，1视频，2语音
				std::string		m_strAttachPath;			//附件路径
			};

			class CBody
			{
			public:
				std::string		m_strGuid;					//报警主键
				std::string		m_strType;					//0未知，1市局微信，2省厅微信，3短信
				std::string		m_strAddress;				//事发地址
				std::string		m_strCrcontent;				//报警内容
				std::string   	m_strIsPhone;				//是否方便接听电话
				std::string		m_strCreateDate;			//报警时间
				std::string		m_strUserCode;				//报警人身份证号码
				std::string		m_strUserName;				//报警人姓名
				std::string		m_strUserPhone;				//报警人手机
				std::string		m_strUserAddress;			//报警人联系地址
				std::string		m_strUnitContactor;			//联系人
				std::string		m_strHandPhone;				//联系电话
				std::string		m_strToOrgID;				//接受部门ID
				std::string		m_strToOrgName;				//接受部门名称
				std::string		m_strToUserID;				//处理人警号
				std::string		m_strToUserName;			//处理人姓名
				std::string		m_strState;					//状态，0未读，1已读，2已创警
				std::string		m_strComNo;					//报警编号
				std::string		m_strOpenID;				//交互窗口的编号(微信交互的主键)
				std::string		m_strLong;					//维度
				std::string		m_strLat;					//经度
				std::string		m_strNotReadChangeCount;	//未读交互信息数量
				std::string		m_strRemark1;				//备用1
				std::string		m_strRemark2;				//备用2
				
				std::vector<CMaterial>		m_vecMaterial;

			};

			CHeaderEx m_oHeader;
			CBody m_oBody;
		};		
	}
}
