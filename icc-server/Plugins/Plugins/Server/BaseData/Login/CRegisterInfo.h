#pragma once
#include <map>
#include <string>
#include <Json/IJson.h>

namespace ICC
{
	// �û�ע����Ϣ
	// ����˽Ӵ�������������ֶ�
	// [6/23/2020 151116314]
	class CRegisterInfo
	{
	public:
		CRegisterInfo();
		~CRegisterInfo();

	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson);
		bool Parse(std::string p_strUserInfo, JsonParser::IJsonPtr p_pJson);

	public:
		std::string m_strDistrictCode; //���������������

		std::string m_strClientID;		//MQClientID�����ڿͻ��ˣ�������ϯλ��
		std::string m_strClientName;	//���ڵ�¼�Ŀͻ��ˣ���UserCode
		std::string m_strClientType;
		std::string m_strClientIP;

		std::string  m_strHeartTime;    //����ʱ��

		// �����������ֶ�				[6/23/2020 151116314]
		std::string m_strUserName;		//���ڵ�¼�Ŀͻ��ˣ���UserName
		std::string m_strDeptCode;		//�û������ĵ�λ����
		std::string m_strDeptName;		//�û������ĵ�λ����
		std::string m_strSeatType;		//ϯλ���ͣ�Ĭ���ݶ��ǽӴ���ϯ
		std::string m_strStaffCode;		//��Ա����
		std::string m_strStaffName;		//��Ա����
	};

}