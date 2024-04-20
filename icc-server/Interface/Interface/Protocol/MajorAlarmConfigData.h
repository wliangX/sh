#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
		class CMajorAlarmConfigData
		{
		public:	
            std::string m_guid;
			std::string m_type_name;//Êý¾Ý¿â×Ö¶ÎÃû
			std::string m_type_value;
			std::string m_remark;			
		};
	}
}
