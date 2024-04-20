#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>
#include <Protocol/CTAPublic.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTAGetAlarmRespond :
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

				unsigned int l_uiIndexX = 0;
				for (auto thirdalarm : m_oBody.m_vecThirdAlarm)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndexX) + "/");
					p_pJson->SetNodeValue(l_strPrefixPath + "guid", thirdalarm.m_strGuid);
					p_pJson->SetNodeValue(l_strPrefixPath + "type", thirdalarm.m_strType);
					p_pJson->SetNodeValue(l_strPrefixPath + "address", thirdalarm.m_strAddress);
					p_pJson->SetNodeValue(l_strPrefixPath + "content", thirdalarm.m_strCrcontent);
					p_pJson->SetNodeValue(l_strPrefixPath + "is_phone", thirdalarm.m_strIsPhone);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_date", thirdalarm.m_strCreateDate);
					p_pJson->SetNodeValue(l_strPrefixPath + "user_code", thirdalarm.m_strUserCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "user_name", thirdalarm.m_strUserName);
					p_pJson->SetNodeValue(l_strPrefixPath + "user_phone", thirdalarm.m_strUserPhone);
					p_pJson->SetNodeValue(l_strPrefixPath + "user_address", thirdalarm.m_strUserAddress);
					p_pJson->SetNodeValue(l_strPrefixPath + "unit_contactor", thirdalarm.m_strUnitContactor);
					p_pJson->SetNodeValue(l_strPrefixPath + "hand_phone", thirdalarm.m_strHandPhone);
					p_pJson->SetNodeValue(l_strPrefixPath + "to_org_id", thirdalarm.m_strToOrgID);
					p_pJson->SetNodeValue(l_strPrefixPath + "to_org_name", thirdalarm.m_strToOrgName);
					p_pJson->SetNodeValue(l_strPrefixPath + "to_user_id", thirdalarm.m_strToUserID);
					p_pJson->SetNodeValue(l_strPrefixPath + "to_user_name", thirdalarm.m_strToUserName);
					p_pJson->SetNodeValue(l_strPrefixPath + "state", thirdalarm.m_strState);
					p_pJson->SetNodeValue(l_strPrefixPath + "com_no", thirdalarm.m_strComNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "open_id", thirdalarm.m_strOpenID);
					p_pJson->SetNodeValue(l_strPrefixPath + "long", thirdalarm.m_strLong);
					p_pJson->SetNodeValue(l_strPrefixPath + "lat", thirdalarm.m_strLat);
					p_pJson->SetNodeValue(l_strPrefixPath + "not_read_change_count", thirdalarm.m_strNotReadChangeCount);
					p_pJson->SetNodeValue(l_strPrefixPath + "remark1", thirdalarm.m_strRemark1);
					p_pJson->SetNodeValue(l_strPrefixPath + "remark2", thirdalarm.m_strRemark2);

					unsigned int l_uiIndexY = 0;
					for (auto material : thirdalarm.m_vecMaterial)
					{
						std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndexX) + "/" 
							+ "material_list/" + std::to_string(l_uiIndexY) + "/");

						p_pJson->SetNodeValue(l_strPrefixPath + "f_guid", material.m_strFGuid);
						p_pJson->SetNodeValue(l_strPrefixPath + "f_name", material.m_strFName);
						p_pJson->SetNodeValue(l_strPrefixPath + "attach_type", material.m_strAttachType);
						p_pJson->SetNodeValue(l_strPrefixPath + "attach_path", material.m_strAttachPath);

						l_uiIndexY++;
					}

					l_uiIndexX++;
				}
				

				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string	m_strCount;
				std::vector<PROTOCOL::CThirdAlarm> m_vecThirdAlarm;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}