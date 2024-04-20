#pragma once
namespace ICC
{
	namespace HelpTool
	{
		class CHelpToolFactoryImpl :
			public IHelpToolFactory
		{
			ICCFactoryHeaderInitialize(CHelpToolFactoryImpl)

		public:
			virtual IHelpToolPtr CreateHelpTool();
		};
	}
}