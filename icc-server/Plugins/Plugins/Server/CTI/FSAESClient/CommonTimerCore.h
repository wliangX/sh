/**
 @Copyright Copyright (C), 2014
 @file		CommonTimerCore.h
 @created	2014/06/27
 @brief		

 @author	xujinghua
*/

#ifndef __CommonTimerCore_H__
#define __CommonTimerCore_H__

class CCommonTimerCore
{
public:
	CCommonTimerCore(void)
	{
	};
	virtual ~CCommonTimerCore(void)
	{
	};

	virtual void OnTimer(unsigned long uMsgId) = 0;
};

#endif