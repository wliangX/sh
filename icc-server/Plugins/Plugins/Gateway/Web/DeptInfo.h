#pragma once
#include <Json/IJsonFactory.h>

#define DEPT_INFO_KEY "DeptInfoKey"

namespace ICC
{
	class CDeptInfo
	{
	public:
		CDeptInfo();
		~CDeptInfo();
	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson);
		bool Parse(std::string p_strDeptInfo, JsonParser::IJsonPtr p_pJson);

	public:
		std::string m_strGuid;			//��λguid
		std::string m_strParentGuid;	//�ϼ���λguid
		std::string m_strCode;			//��λ����
		std::string m_strDistrictCode;	//��λ������������
		std::string m_strType;			//��λ����
		std::string m_strName;			//��λ����
		std::string m_strPhone;			//��λ����
		std::string m_strShortcut;		//��λ��ݲ�ѯ��д
		std::string m_strLevel;
		std::string m_strSort;			//��λ��ţ�һ���1��ʼ����	
	};
};