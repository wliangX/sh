#pragma once
#include <IObject.h>

namespace ICC
{
	class IFrame :
		public IObject
	{
	public:
		virtual void Start() = 0;
		virtual void Stop() = 0;
	};

	typedef boost::shared_ptr<IFrame> IFramePtr;
}
