#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTAGetChangeInfoRespond :
			public ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				unsigned int l_uiIndex = 0;
				for (auto changeinfo : m_oBody.m_vecChangeInfo)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");
					p_pJson->SetNodeValue(l_strPrefixPath + "guid", changeinfo.m_strGuid);
					p_pJson->SetNodeValue(l_strPrefixPath + "third_alarm_guid", changeinfo.m_strThirdAlarmGuid);
					p_pJson->SetNodeValue(l_strPrefixPath + "orientation", changeinfo.m_strOrientation);
					p_pJson->SetNodeValue(l_strPrefixPath + "state", changeinfo.m_strState);
					p_pJson->SetNodeValue(l_strPrefixPath + "phone", changeinfo.m_strPhone);
					p_pJson->SetNodeValue(l_strPrefixPath + "open_id", changeinfo.m_strOpenID);
					p_pJson->SetNodeValue(l_strPrefixPath + "change_type", changeinfo.m_strChangeType);
					p_pJson->SetNodeValue(l_strPrefixPath + "change_content", changeinfo.m_strChangeContent);
					p_pJson->SetNodeValue(l_strPrefixPath + "change_date", changeinfo.m_strChangeDate);
					p_pJson->SetNodeValue(l_strPrefixPath + "long", changeinfo.m_strLong);
					p_pJson->SetNodeValue(l_strPrefixPath + "lat", changeinfo.m_strLat);
					p_pJson->SetNodeValue(l_strPrefixPath + "remark1", changeinfo.m_strRemark1);
					p_pJson->SetNodeValue(l_strPrefixPath + "remark2", changeinfo.m_strRemark2);

					l_uiIndex++;
				}

				return p_pJson->ToString();
			}

		public:
			class CChangeInfo
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

			class CBody
			{
			public:
				std::string m_strCount;
				std::vector<CChangeInfo> m_vecChangeInfo;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}