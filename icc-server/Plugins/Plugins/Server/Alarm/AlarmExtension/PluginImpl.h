#pragma once
#pragma once

namespace ICC
{
	class CPluginImpl :
		public CPluginBase
	{
	public:
		virtual unsigned int GetPriorityLevel();
		virtual void OnStart();
		virtual void OnStop();

	private:
		IBusinessPtr m_pBusiness;
	};
}
