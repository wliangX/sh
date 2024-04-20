/**
 @Copyright Copyright (C), 2014
 @file		CommonThreadCore.h
 @created	2014/06/27
 @brief		

 @author	xujinghua
*/

#ifndef __CommonThreadCore_H__
#define __CommonThreadCore_H__

#include <string>
#include <memory>
#include <Observer/IObserverFactory.h>
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>
#include <Protocol/CAddOrUpdateAlarmWithProcessRequest.h>

#if  defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#ifdef _WIN64
#define __clong_ long long
#define __culong_ unsigned long long
#else
#define __clong_ long
#define __culong_ unsigned long
#endif

#else
#define __clong_ long
#define __culong_ unsigned long
#endif


struct CommonThread_data 
{
	__culong_	wParam;
	__culong_	lParam;
	__culong_	msg_id;
	__clong_	data;	
	unsigned int uThreadId;
    std::string str_msg;
	std::string strExt;	
	ICC::ObserverPattern::INotificationPtr pNotifyPtr;	
	ICC::PROTOCOL::IRequestPtr pRequestPtr;
	ICC::PROTOCOL::IRespondPtr pRespondPtr;
	ICC::PROTOCOL::AddOrUpdateAlarmWithProcessRequestPtr pAlarmRequestPtr;
};


class CCommonThreadCore
{
public:
	CCommonThreadCore(void)
	{
	};
	virtual ~CCommonThreadCore(void)
	{
	};

public:
	virtual void ProcMessage(CommonThread_data msg_data) = 0;
	virtual void DeleteData(__clong_ lParam, __clong_ wParam, __clong_ data){};
};


#endif


