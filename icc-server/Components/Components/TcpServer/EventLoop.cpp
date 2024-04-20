#include "Boost.h"
#include <boost/asio.hpp>

#include "EventLoop.h"

void EventLoop::S_RunEventLoop(AsioEngine* p_Engine, std::string p_strNameDesc)
{
	boost::asio::io_service::work permanence(*p_Engine);
	p_Engine->run();
}