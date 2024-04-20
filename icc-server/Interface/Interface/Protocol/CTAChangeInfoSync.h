#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTAChangeInfoSync
		{
		public:
			std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);
				p_pJson->SetNodeValue("/body/third_type", m_oBody.m_strThirdType);

				p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
				p_pJson->SetNodeValue("/body/third_alarm_guid", m_oBody.m_strThirdAlarmGuid);
				p_pJson->SetNodeValue("/body/orientation", m_oBody.m_strOrientation);
				p_pJson->SetNodeValue("/body/state", m_oBody.m_strState);
				p_pJson->SetNodeValue("/body/phone", m_oBody.m_strPhone);
				p_pJson->SetNodeValue("/body/open_id", m_oBody.m_strOpenID);
				p_pJson->SetNodeValue("/body/change_type", m_oBody.m_strChangeType);
				p_pJson->SetNodeValue("/body/change_content", m_oBody.m_strChangeContent);
				p_pJson->SetNodeValue("/body/change_date", m_oBody.m_strChangeDate);
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
				std::string		m_strSyncType;				//1：表示添加，2：表示更新，3：表示删除
				std::string		m_strThirdType;					//第三方字典值
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
					
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
