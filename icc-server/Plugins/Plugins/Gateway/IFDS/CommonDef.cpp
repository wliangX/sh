#include "CommonDef.h"
#include "BusinessImpl.h"


void gfunc_SendMessage(const std::string& strMsg, msgSendType type)
{
	if (g_pBusinessImpl)
	{
		g_pBusinessImpl->SendMessage(strMsg, type);
	}
}
