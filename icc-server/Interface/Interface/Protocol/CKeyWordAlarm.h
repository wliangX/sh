/*
���о���ͬ����Ϣ
*/
#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>


namespace ICC
{
	namespace PROTOCOL
	{
		class CKeyWordAlarm
		{
		public:
            std::string m_strGuid;     //Guid
			std::string m_strAlarmID;  //�澯ID
			std::string m_strKeyWord;       //�漰�Ĺؼ���
			std::string m_strKeyContent;    //�漰�ؼ��ֵ���ʾ����
			std::string m_strAlarmContent;  //��������
			std::string m_strDeptCode;     //�����Ĳ��ű��� 
		};	
	}
}
