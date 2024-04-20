#pragma once

namespace ICC
{
	class CControlCenter
	{
	public:
		CControlCenter();
		virtual ~CControlCenter();

	public:
		static boost::shared_ptr<CControlCenter> Instance();
		void ExitInstance();

		void OnInit(IResourceManagerPtr p_pResourceManager, ISwitchEventCallbackPtr p_pCallback);
		void OnStart();
		void OnStop();
		void OnDestroy();

	private:
		static boost::shared_ptr<CControlCenter> m_pInstance;

	};	// end CControlCenter

}// end namespace