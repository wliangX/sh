#pragma once 
#include <Json/IJson.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class ISync
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson) = 0;
		};
	}
}
