#pragma once
#include <boost/smart_ptr.hpp>

namespace ICC
{
	class IObject
	{
	public:
		virtual ~IObject() {}
	};

	typedef boost::shared_ptr<IObject> IObjectPtr;
}
