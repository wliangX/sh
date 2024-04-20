/**
 @Copyright Copyright (C), 2014
 @file		CommonTimerCore.h
 @created	2014/06/27
 @brief		

 @author	xujinghua
*/

#ifndef __CommonTimerCore_H__
#define __CommonTimerCore_H__

class CStatisticsTimerCore
{
public:
	CStatisticsTimerCore(void)
	{
	};
	virtual ~CStatisticsTimerCore(void)
	{
	};

	virtual void OnStatisticsTimer(unsigned long uMsgId) = 0;
};

#endif