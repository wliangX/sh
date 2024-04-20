#pragma once

namespace ICC
{
#if defined(_MSC_VER)
	typedef  unsigned __int64     uint64;
	typedef  __int64              int64;
#elif defined(__GNUC__)
	typedef  unsigned long long   uint64;
	typedef  long long            int64;
#else
	#error   "Compiler not support int64."
#endif

	typedef unsigned char uchar;
	typedef char int8;
	typedef unsigned char uint8;
	typedef short int16;
	typedef unsigned short uint16;
	typedef unsigned int uint;
}

