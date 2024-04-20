#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSmsTransmitAlarmRequest :
			public ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
				p_pJson->SetNodeValue("/body/type", m_oBody.m_strType);
				p_pJson->SetNodeValue("/body/address", m_oBody.m_strAddress);
				p_pJson->SetNodeValue("/body/content", m_oBody.m_strCrcontent);
				p_pJson->SetNodeValue("/body/is_phone", m_oBody.m_strIsPhone);
				p_pJson->SetNodeValue("/body/create_date", m_oBody.m_strCreateDate);
				p_pJson->SetNodeValue("/body/user_code", m_oBody.m_strUserCode);
				p_pJson->SetNodeValue("/body/user_name", m_oBody.m_strUserName);
				p_pJson->SetNodeValue("/body/user_phone", m_oBody.m_strUserPhone);
				p_pJson->SetNodeValue("/body/user_address", m_oBody.m_strUserAddress);
				p_pJson->SetNodeValue("/body/unit_contactor", m_oBody.m_strUnitContactor);
				p_pJson->SetNodeValue("/body/hand_phone", m_oBody.m_strHandPhone);
				p_pJson->SetNodeValue("/body/to_org_id", m_oBody.m_strToOrgID);
				p_pJson->SetNodeValue("/body/to_org_name", m_oBody.m_strToOrgName);
				p_pJson->SetNodeValue("/body/to_user_id", m_oBody.m_strToUserID);
				p_pJson->SetNodeValue("/body/to_user_name", m_oBody.m_strToUserName);
				p_pJson->SetNodeValue("/body/state", m_oBody.m_strState);
				p_pJson->SetNodeValue("/body/com_no", m_oBody.m_strComNo);
				p_pJson->SetNodeValue("/body/open_id", m_oBody.m_strOpenID);
				p_pJson->SetNodeValue("/body/long", m_oBody.m_strLong);
				p_pJson->SetNodeValue("/body/lat", m_oBody.m_strLat);
				p_pJson->SetNodeValue("/body/remark1", m_oBody.m_strRemark1);
				p_pJson->SetNodeValue("/body/remark2", m_oBody.m_strRemark2);

				return p_pJson->ToString();
			}

		public:
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
				std::string		m_strRemark1;				//备用1
				std::string		m_strRemark2;				//备用2
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};		
	}
}
