#include "ICCTransportListener.h"


ICCTransportListener::ICCTransportListener()
{
}


ICCTransportListener::~ICCTransportListener()
{
}

void ICCTransportListener::onCommand(const Pointer<Command> command)
{
// 	try
// 	{
// 		cout << "[Command]" << command->toString() << endl;
// 	}
// 	catch (const decaf::lang::Exception& ex)
// 	{
// 		ex.getStackTraceString();
// 	}
// 	catch (...)
// 	{
// 
// 	}
}

void ICCTransportListener::onException(const decaf::lang::Exception& ex)
{
// 	cout << "CMS Exception occurred.  Shutting down client." << endl;
// 	ex.printStackTrace();
//	exit(1);
}

void ICCTransportListener::transportInterrupted()
{
	//cout << "+++�����ѶϿ�+++" << endl;
}

void ICCTransportListener::transportResumed()
{
	//cout << "+++����������+++" << endl;
}