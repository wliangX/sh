#pragma once

#include "amqinc.h"

class ICCTransportListener : public TransportListener
{
public:
	ICCTransportListener();
	~ICCTransportListener();
public:
	virtual void onCommand(const Pointer<Command> command);
	virtual void onException(const decaf::lang::Exception& ex);
	virtual void transportInterrupted();
	virtual void transportResumed();
};

