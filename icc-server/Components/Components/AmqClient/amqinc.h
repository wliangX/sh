/*
* amqinc.h
*
*  Created on: 2017Äê12ÔÂ5ÈÕ
*      Author: weijl
*/

#ifndef AMQINC_H_
#define AMQINC_H_

#include <activemq/library/ActiveMQCPP.h>
#include <activemq/transport/TransportListener.h>
#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/util/StlQueue.h>
#include <decaf/lang/Integer.h>
#include <decaf/lang/Long.h>
#include <decaf/lang/System.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/commands/ActiveMQMessage.h>
#include <activemq/commands/ConnectionInfo.h>
#include <activemq/util/Config.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/MapMessage.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <memory>
#include <map>
#include <string>
#include <list>

using namespace activemq::core;
using namespace activemq::transport;
using namespace decaf::util::concurrent;
using namespace decaf::util;
using namespace decaf::lang;
using namespace cms;
using namespace std;

#define ICC_DEST_TYPE 2

#endif /* AMQINC_H_ */
