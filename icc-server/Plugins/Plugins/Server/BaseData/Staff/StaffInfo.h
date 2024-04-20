#pragma once
#include <map>
#include <string>
#include <Json/IJsonFactory.h>

namespace ICC
{
	class CStaffInfo		
	{	
	public:
		CStaffInfo();
		~CStaffInfo();

	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson = nullptr);
		bool Parse(std::string p_strUserInfo, JsonParser::IJsonPtr p_pJson);

	public:
		std::string m_strGuid;			//��Աguid
		std::string m_strDeptGuid;		//��Ա���ڵ�λguid
		std::string m_strCode;			//��Ա����
		std::string m_strName;			//��Ա����
		std::string m_strSex;			//��Ա�Ա�
		std::string m_strType;			//��Ա����
		std::string m_strPosition;		//��Աְ��
		std::string m_strMobile;		//��Ա�ֻ�
		std::string m_strPhone;			//��Ա����
		std::string m_strShortcut;		//��Ա��ݲ�ѯ��д			
		std::string m_strSort;			//��Ա��ţ�һ���1��ʼ����
		std::string m_strLevel;			//�㼶
		std::string m_strIsLeader;		//�Ƿ�Ϊ�쵼,�ǣ�1����0
	};

	// ��Ա��Ϣ���棬�������ã�ʹ��Redis����
	typedef std::map<std::string, CStaffInfo> StaffInfoMap;
}