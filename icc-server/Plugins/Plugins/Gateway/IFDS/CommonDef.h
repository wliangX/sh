#ifndef __CommonDef_H__
#define __CommonDef_H__

#include <string>

#define SYSTEMID "ICC"

namespace ICC
{
	class CBusinessImpl;
	extern CBusinessImpl* g_pBusinessImpl;
}


enum msgSendType
{
	msgSendType_Post = 1,
	msgSendType_RequestRespond,
};
void gfunc_SendMessage(const std::string& strMsg, msgSendType type = msgSendType_Post);

#endif