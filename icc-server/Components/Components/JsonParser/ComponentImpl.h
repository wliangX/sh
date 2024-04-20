#pragma once

namespace ICC
{
	class CComponentImpl :
		public CComponentBase
	{
		ICCResourceInitialize;
	public:
		virtual unsigned int GetPriorityLevel();
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();
	};
};
