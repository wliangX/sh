#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CKeyWordInfo
		{
		public:
			std::string m_strGuid;   
			std::string m_strKeyWord;  //一个关键字
			std::string m_strContent;  //对接警单填写的内容进行关键字识别，一旦发现有一些指挥中心关注的重要信息（可事先维护），
				//系统即发出相关警示提醒接警员，也可将警示信息发送到班长台进行提醒，或者将信息通过短信发送给指定的人员。
		};
	}
}
