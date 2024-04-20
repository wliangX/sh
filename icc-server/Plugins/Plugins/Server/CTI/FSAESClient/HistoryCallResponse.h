#pragma once 
#include <map>
#include <string>
#include <Json/IJsonFactory.h>

namespace ICC
{
	class CRefuseCallEvent
	{
	public:
		virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
		{
			if (nullptr == p_pJson)
			{
				return "";
			}

			return p_pJson->ToString();
		}

	public:

	};
}
