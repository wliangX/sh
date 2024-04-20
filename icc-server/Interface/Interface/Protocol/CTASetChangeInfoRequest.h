#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTASetChangeInfoRequest :
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
				m_oBody.m_strThirdAlarmGuid = p_pJson->GetNodeValue("/body/third_alarm_guid", "");
				m_oBody.m_strOrientation = p_pJson->GetNodeValue("/body/orientation", "");
				m_oBody.m_strState = p_pJson->GetNodeValue("/body/state", "");
				m_oBody.m_strPhone = p_pJson->GetNodeValue("/body/phone", "");
				m_oBody.m_strOpenID = p_pJson->GetNodeValue("/body/open_id", "");
				m_oBody.m_strChangeType = p_pJson->GetNodeValue("/body/change_type", "");
				m_oBody.m_strChangeContent = p_pJson->GetNodeValue("/body/change_content", "");
				m_oBody.m_strChangeDate = p_pJson->GetNodeValue("/body/change_date", "");
				m_oBody.m_strLong = p_pJson->GetNodeValue("/body/long", "");
				m_oBody.m_strLat = p_pJson->GetNodeValue("/body/lat", "");
				m_oBody.m_strRemark1 = p_pJson->GetNodeValue("/body/remark1", "");
				m_oBody.m_strRemark2 = p_pJson->GetNodeValue("/body/remark2", "");

				return true;
			}

		public:
			class CBody
			{
			public:
				std::string		m_strGuid;					//主键
				std::string		m_strThirdAlarmGuid;		//报警主键
				std::string		m_strOrientation;			//0报警人给警员发送，1警员给报警人发送
				std::string		m_strState;					//状态，0未读，1已读
				std::string   	m_strPhone;					//手机号码
				std::string		m_strOpenID;				//交互窗口的编号(微信交互的主键)
				std::string		m_strChangeType;			//交互类型（1、文字；2、图片；3、视频；4、语音）
				std::string		m_strChangeContent;			//交互内容（交互类型为1时为文字内容，2-4：为多媒体文件全路径）
				std::string		m_strChangeDate;			//交互时间  yyyy-mm-dd  hh24:mi:ss
				std::string		m_strLong;					//经度（wgs84坐标系）
				std::string		m_strLat;					//纬度（wgs84坐标系）
				std::string		m_strRemark1;				//备用字段1
				std::string		m_strRemark2;				//备用字段2
			};
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};		
	}
}
