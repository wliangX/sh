#pragma once
#include <map>
#include <string>
#include <Json/IJson.h>

namespace ICC
{
	/*
	* ��������Redis���������Ϣ
	*/
	class CDispatchInfo
	{
	public:
		CDispatchInfo();
		~CDispatchInfo();

	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson);
		bool Parse(std::string p_strDispatchInfo, JsonParser::IJsonPtr p_pJson);

	public:
		std::string m_strGuid;			//�ɵ��ɵ�λguid
		std::string m_strParentGuid;	//�ϼ��ɵ��ɵ�λguid
		std::string m_strCode;			//ԭ��λ����
		std::string m_strDistrictCode;	//ԭ��λ������������
		std::string m_strType;			//ԭ��λ����
		std::string m_strName;			//ԭ��λ����
		std::string m_strPhone;			//ԭ��λ����
	};


	/*
	* ��������Redis����ɵ�����Ϣ
	*/
	class CCanDispatchInfo
	{
	public:
		CCanDispatchInfo();
		~CCanDispatchInfo();

	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson);
		bool Parse(std::string p_strCanDispatchInfo, JsonParser::IJsonPtr p_pJson);

	public:
		std::string m_strGuid;					//�ɵ��ɵ�λguid
		std::string m_strParentGuid;			//�ϼ��ɵ��ɵ�λguid
		std::string m_strBindDeptCode;			//�ýڵ����󶨵ĵ�λguid
		std::string m_strBindParentDeptCode;	//�ýڵ��ϼ����󶨵ĵ�λguid
		std::string m_BelongDeptCode;			//�ýڵ��������ĵ�λguid
		std::string m_strCreateUser;			//�����û�
		std::string m_strCreateTime;			//����ʱ��
		std::string m_strUpdateUser;			//�����û�
		std::string m_strUpdateTime;			//����ʱ��
	};

	typedef std::map<std::string, CCanDispatchInfo> CanDispatchInfoMap;
}