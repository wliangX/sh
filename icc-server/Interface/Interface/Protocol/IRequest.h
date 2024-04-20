#pragma once 
#include <Json/IJson.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson) = 0;
		};

		typedef boost::shared_ptr<IRequest> IRequestPtr;

		class IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson) = 0;
		};

		typedef boost::shared_ptr<IReceive> IReceivePtr;
	}
}
