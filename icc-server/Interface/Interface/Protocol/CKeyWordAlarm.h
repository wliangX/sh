/*
敏感警情同步消息
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
			std::string m_strAlarmID;  //告警ID
			std::string m_strKeyWord;       //涉及的关键字
			std::string m_strKeyContent;    //涉及关键字的提示内容
			std::string m_strAlarmContent;  //警情内容
			std::string m_strDeptCode;     //处警的部门编码 
		};	
	}
}
