#pragma once 
#include <Json/IJson.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson) = 0;
		};

		typedef boost::shared_ptr<IRespond> IRespondPtr;

		class ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson) = 0;
		};

		typedef boost::shared_ptr<ISend> ISendPtr;
	}
}
