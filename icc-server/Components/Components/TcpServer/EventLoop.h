#pragma once

namespace ICC
{
	namespace Tcp
	{
		typedef boost::asio::io_service AsioEngine;

		class EventLoop
		{
		public:
			static void S_RunEventLoop(AsioEngine* p_Engine, std::string p_strNameDesc);
		};
	}
}
