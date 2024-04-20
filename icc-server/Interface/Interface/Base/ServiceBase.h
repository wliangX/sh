#pragma once
#include <IService.h>

namespace ICC
{
	class CServiceBase :
		public IService
	{
	private:
		virtual void Run()
		{
			OnRun();
		}

	public:
		virtual void OnRun()
		{
		}
	};
}
