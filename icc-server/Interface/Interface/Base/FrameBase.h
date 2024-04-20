#pragma once
#include <IFrame.h>

namespace ICC
{
	class CFrameBase :
		public IFrame
	{
	private:
		virtual void Start()
		{
			OnStart();
		}

		virtual void Stop()
		{
			OnStop();
		}

	public:
		virtual void OnStart()
		{
		}
		virtual void OnStop()
		{
		}
	};
}
