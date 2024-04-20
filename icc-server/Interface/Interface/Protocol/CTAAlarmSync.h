#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/CTAPublic.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTAAlarmSync
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);

				p_pJson->SetNodeValue("/body/guid", m_oBody.m_oThirdAlarm.m_strGuid);
				p_pJson->SetNodeValue("/body/type", m_oBody.m_oThirdAlarm.m_strType);
				p_pJson->SetNodeValue("/body/address", m_oBody.m_oThirdAlarm.m_strAddress);
				p_pJson->SetNodeValue("/body/content", m_oBody.m_oThirdAlarm.m_strCrcontent);
				p_pJson->SetNodeValue("/body/is_phone", m_oBody.m_oThirdAlarm.m_strIsPhone);
				p_pJson->SetNodeValue("/body/create_date", m_oBody.m_oThirdAlarm.m_strCreateDate);
				p_pJson->SetNodeValue("/body/user_code", m_oBody.m_oThirdAlarm.m_strUserCode);
				p_pJson->SetNodeValue("/body/user_name", m_oBody.m_oThirdAlarm.m_strUserName);
				p_pJson->SetNodeValue("/body/user_phone", m_oBody.m_oThirdAlarm.m_strUserPhone);
				p_pJson->SetNodeValue("/body/user_address", m_oBody.m_oThirdAlarm.m_strUserAddress);
				p_pJson->SetNodeValue("/body/unit_contactor", m_oBody.m_oThirdAlarm.m_strUnitContactor);
				p_pJson->SetNodeValue("/body/hand_phone", m_oBody.m_oThirdAlarm.m_strHandPhone);
				p_pJson->SetNodeValue("/body/to_org_id", m_oBody.m_oThirdAlarm.m_strToOrgID);
				p_pJson->SetNodeValue("/body/to_org_name", m_oBody.m_oThirdAlarm.m_strToOrgName);
				p_pJson->SetNodeValue("/body/to_user_id", m_oBody.m_oThirdAlarm.m_strToUserID);
				p_pJson->SetNodeValue("/body/to_user_name", m_oBody.m_oThirdAlarm.m_strToUserName);
				p_pJson->SetNodeValue("/body/state", m_oBody.m_oThirdAlarm.m_strState);
				p_pJson->SetNodeValue("/body/com_no", m_oBody.m_oThirdAlarm.m_strComNo);
				p_pJson->SetNodeValue("/body/open_id", m_oBody.m_oThirdAlarm.m_strOpenID);
				p_pJson->SetNodeValue("/body/long", m_oBody.m_oThirdAlarm.m_strLong);
				p_pJson->SetNodeValue("/body/lat", m_oBody.m_oThirdAlarm.m_strLat);
				p_pJson->SetNodeValue("/body/not_read_change_count", m_oBody.m_oThirdAlarm.m_strNotReadChangeCount);
				p_pJson->SetNodeValue("/body/remark1", m_oBody.m_oThirdAlarm.m_strRemark1);
				p_pJson->SetNodeValue("/body/remark2", m_oBody.m_oThirdAlarm.m_strRemark2);

				unsigned int l_uiIndex = 0;
				for (auto material : m_oBody.m_oThirdAlarm.m_vecMaterial)
				{
					std::string l_strPrefixPath("/body/material_list/" + std::to_string(l_uiIndex) + "/");

					p_pJson->SetNodeValue(l_strPrefixPath + "f_guid", material.m_strFGuid);
					p_pJson->SetNodeValue(l_strPrefixPath + "f_name", material.m_strFName);
					p_pJson->SetNodeValue(l_strPrefixPath + "attach_type", material.m_strAttachType);
					p_pJson->SetNodeValue(l_strPrefixPath + "attach_path", material.m_strAttachPath);

					l_uiIndex++;
				}

				return p_pJson->ToString();
			}
		public:
			
			class CBody
			{
			public:
				std::string m_strSyncType;			//1：表示添加，2：表示更新，3：表示删除
				PROTOCOL::CThirdAlarm m_oThirdAlarm;
			};
					
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
