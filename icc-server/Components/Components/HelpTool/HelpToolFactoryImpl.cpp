#include "Boost.h"
#include "HelpToolFactoryImpl.h"
#include "HelpToolImpl.h"

ICCFactoryCppInitialize(CHelpToolFactoryImpl)

HelpTool::IHelpToolPtr CHelpToolFactoryImpl::CreateHelpTool()
{
	return boost::make_shared<CHelpToolImpl>(m_IResourceManagerPtr);
}
