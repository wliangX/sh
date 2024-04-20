#pragma once

#include <mutex>

namespace ICC
{
	class CResultSetCounter
	{
	public:
		void                      AddCounter();
		void                      ReduceCounter();
		unsigned int              GetCounter();

		static CResultSetCounter* Instance();

	private:
		CResultSetCounter();
		~CResultSetCounter();

	private:
		static CResultSetCounter* sm_instance;
		static std::mutex sm_mutexInstance;

		std::mutex      m_mutexResultSetCounter;
		unsigned int    m_uResultSetCounter;
	};
}