#pragma once
#include <IResourceManager.h>

namespace ICC
{
	class CResourceManagerBase :
		public IResourceManager
	{
	private:
		virtual void SetResource(std::string p_strName, IResourcePtr p_pResource)
		{
			OnSetResource(p_strName, p_pResource);
		}
		virtual IResourcePtr GetResource(std::string p_strName)
		{
			return OnGetResource(p_strName);
		}

	public:
		virtual void OnSetResource(std::string p_strName, IResourcePtr p_pResource)
		{
		}
		virtual IResourcePtr OnGetResource(std::string p_strName)
		{
			return IResourcePtr();
		}
	};
}