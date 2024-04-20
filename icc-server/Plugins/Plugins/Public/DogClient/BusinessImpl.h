#pragma once

namespace ICC
{
	class CBusinessImpl :
		public CBusinessBase
	{
	public:
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();

	public:
		Log::ILogPtr m_pLog;
		Config::IConfigPtr m_pConfig;
		boost::dll::shared_library m_dll;
	};
}

