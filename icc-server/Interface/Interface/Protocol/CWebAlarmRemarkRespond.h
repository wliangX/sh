#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CWebLoginRespond :
			public ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				p_pJson->SetNodeValue("/body/token", m_oBody.m_strToken);

				p_pJson->SetNodeValue("/body/user/code", m_oBody.m_oUser.m_strCode);
				p_pJson->SetNodeValue("/body/user/name", m_oBody.m_oUser.m_strName);

				p_pJson->SetNodeValue("/body/staff/code", m_oBody.m_oStaff.m_strCode);
				p_pJson->SetNodeValue("/body/staff/name", m_oBody.m_oStaff.m_strName);
				p_pJson->SetNodeValue("/body/staff/sex", m_oBody.m_oStaff.m_strSex);
				p_pJson->SetNodeValue("/body/staff/type", m_oBody.m_oStaff.m_strType);
				p_pJson->SetNodeValue("/body/staff/position", m_oBody.m_oStaff.m_strPosition);
				p_pJson->SetNodeValue("/body/staff/mobile", m_oBody.m_oStaff.m_strMobile);
				p_pJson->SetNodeValue("/body/staff/phone", m_oBody.m_oStaff.m_strPhone);

				p_pJson->SetNodeValue("/body/dept/code", m_oBody.M_oDept.m_strCode);
				p_pJson->SetNodeValue("/body/dept/name", m_oBody.M_oDept.m_strName);
				p_pJson->SetNodeValue("/body/dept/district_code", m_oBody.M_oDept.m_strDistrictCode);
				p_pJson->SetNodeValue("/body/dept/type", m_oBody.M_oDept.m_strType);
				p_pJson->SetNodeValue("/body/dept/phone", m_oBody.M_oDept.m_strPhone);
				
				return p_pJson->ToString();
			}

		public:
			class CUser
			{
			public:
				std::string m_strCode;				//用户编码
				std::string m_strName;				//用户名称
			};
			class CStaff
			{
			public:
				std::string m_strCode;				//警员编码
				std::string m_strName;				//警员名称
				std::string m_strSex;				//警员性别
				std::string m_strType;				//警员类型
				std::string m_strPosition;			//警员职务
				std::string m_strMobile;			//警员手机
				std::string m_strPhone;				//警员座机
			};
			class CDept
			{
			public:
				std::string m_strCode;				//单位编码
				std::string m_strName;				//单位名称
				std::string m_strDistrictCode;		//单位行政区划编码
				std::string m_strType;				//单位类型
				std::string m_strPhone;				//单位座机
			};
			class CBody
			{
			public:
				std::string m_strResult;			//0：表示成功，1：表示失败
				std::string m_strToken;				//授权码，格式为icc_web_user_yyyymmddhhmissmmmmmm
				CUser m_oUser;
				CStaff m_oStaff;
				CDept M_oDept;
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}