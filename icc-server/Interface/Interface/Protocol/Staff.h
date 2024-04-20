#pragma once
#include <Json/IJsonFactory.h>

#include <Data/IBaseData.h>


#define STAFF_INFO_KEY "StaffInfo"
#define STAFF_INFO_MAP_KEY "StaffInfoMap"

namespace ICC
{
	namespace PROTOCOL
	{
		class CStaffInfo
		{
		public:
			std::string ToJson(JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson)
				{
					return "";
				}
				p_pJson->SetNodeValue("/guid", m_strGuid);
				p_pJson->SetNodeValue("/code", m_strCode);
				p_pJson->SetNodeValue("/dept_guid", m_strDeptGuid);
				p_pJson->SetNodeValue("/name", m_strName);
				p_pJson->SetNodeValue("/sex", m_strSex);
				p_pJson->SetNodeValue("/type", m_strType);
				p_pJson->SetNodeValue("/position", m_strPosition);
				p_pJson->SetNodeValue("/mobile", m_strMobile);
				p_pJson->SetNodeValue("/phone", m_strPhone);
				p_pJson->SetNodeValue("/level", m_strLevel);
				p_pJson->SetNodeValue("/sort", m_strSort);
				p_pJson->SetNodeValue("/shortcut", m_strShortcut);
				p_pJson->SetNodeValue("/isleader", m_strIsLeader);
				p_pJson->SetNodeValue("/org_identifier", m_strOrgIdentifier);
				p_pJson->SetNodeValue("/staff_id_no", m_strStaffIdNo);
				return p_pJson->ToString();
			}

			bool Parse(std::string l_strJson, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson || !p_pJson->LoadJson(l_strJson))
				{
					return false;
				}

				m_strGuid = p_pJson->GetNodeValue("/guid", "");
				m_strDeptGuid = p_pJson->GetNodeValue("/dept_guid", "");
				m_strCode = p_pJson->GetNodeValue("/code", "");
				m_strName = p_pJson->GetNodeValue("/name", "");
				m_strSex = p_pJson->GetNodeValue("/sex", "");
				m_strType = p_pJson->GetNodeValue("/type", "");
				m_strPosition = p_pJson->GetNodeValue("/position", "");
				m_strMobile = p_pJson->GetNodeValue("/mobile", "");
				m_strPhone = p_pJson->GetNodeValue("/phone", "");
				m_strLevel = p_pJson->GetNodeValue("/level", "");
				m_strSort = p_pJson->GetNodeValue("/sort", "");
				m_strShortcut = p_pJson->GetNodeValue("/shortcut", "");
				m_strIsLeader = p_pJson->GetNodeValue("/isleader", "");
				m_strOrgIdentifier = p_pJson->GetNodeValue("/org_identifier", "");
				m_strStaffIdNo = p_pJson->GetNodeValue("/staff_id_no", "");

				return true;
			}

		public:
			std::string m_strGuid;			//警员guid
			std::string m_strDeptGuid;		//警员所在单位guid
			std::string m_strCode;			//警员编码
			std::string m_strName;			//警员姓名
			std::string m_strSex;			//警员性别
			std::string m_strType;			//警员类型
			std::string m_strPosition;		//警员职务
			std::string m_strMobile;		//警员手机
			std::string m_strPhone;			//警员座机
			std::string m_strShortcut;		//警员快捷查询编写			
			std::string m_strSort;			//警员序号，一般从1开始排序
			std::string m_strLevel;			//层级	
			std::string m_strIsLeader;		//是否为领导,是：1，否：0
			std::string m_strOrgIdentifier;
			std::string m_strStaffIdNo;		//警员身份证号
		};
	}

};