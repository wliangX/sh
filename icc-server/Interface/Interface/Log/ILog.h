#pragma once
#pragma warning(disable:4996)
#include <string>
#include <stdarg.h>
#include <IResource.h>

namespace ICC
{
	namespace Log
	{

		/*
		* class   日志接口类
		* author  w16314
		* purpose
		* note
		*/
		class ILog :
			public IObject
		{
		public:
			//************************************
			// Method:    Log
			// FullName:  ICC::ILog::Log
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			// Parameter: const std::string & p_strLog
			// Parameter: const std::string & p_strFile
			// Parameter: const std::string & p_strFunc
			// Parameter: unsigned int p_uiLine
			// Parameter: unsigned int p_iLogLevel
			//************************************
			virtual void Write(unsigned int p_iLogLevel, const std::string& p_strLog, const std::string& p_strFile, const std::string& p_strFunc, unsigned int p_uiLine, const std::string& p_strModuleName) = 0;

			//************************************
			// Method:    SetModuleName
			// FullName:  ICC::Log::ILog::SetModuleName
			// Access:    virtual public 
			// Returns:   void
			// Qualifier: 由log工厂类自动调用，业务模块不需要主动调用
			// Parameter: const std::string & p_strModuleName
			//************************************
			virtual void SetModuleName(const std::string& p_strModuleName) = 0;
		};

		typedef boost::shared_ptr<ILog> ILogPtr;
	}
}

//************************************
// Method:    ICCFormat
// FullName:  ICCFormat
// Access:    public 
// Returns:   std::string
// Qualifier: 字符串格式化
// Parameter: const char * cformat
// Parameter: ...
//************************************
inline std::string ICCFormat(const char* p_pformat, ...)
{
	va_list l_argList, l_argCopyList;
	va_start(l_argList, p_pformat);
	va_copy(l_argCopyList, l_argList);
	size_t num_of_chars;
#if defined(WIN32) || defined(WIN64)
	num_of_chars = _vscprintf(p_pformat, l_argCopyList);
#else
	num_of_chars = vsnprintf(nullptr, 0, p_pformat, l_argCopyList);
#endif
	va_end(l_argCopyList);

	std::string l_strFormat(num_of_chars, 0);
	vsprintf((char*)l_strFormat.data(), p_pformat, l_argList);
	va_end(l_argList);

	return l_strFormat;
}


#ifdef WIN32

#define ICC_LOG_INFO(Logger, Format, ...)													\
{																							\
	std::string strLog = ICCFormat(Format, __VA_ARGS__);									\
	Logger->Write(1, strLog, __FILE__, __FUNCTION__, __LINE__, MODULE_NAME);				\
}
#define ICC_LOG_ERROR(Logger, Format, ...)													\
{																							\
	std::string strLog = ICCFormat(Format, __VA_ARGS__);									\
	Logger->Write(2, strLog, __FILE__, __FUNCTION__, __LINE__, MODULE_NAME);				\
}
#define ICC_LOG_FATAL(Logger, Format, ...)													\
{																							\
	std::string strLog = ICCFormat(Format, __VA_ARGS__);									\
	Logger->Write(3, strLog, __FILE__, __FUNCTION__, __LINE__, MODULE_NAME);				\
}
#define ICC_LOG_WARNING(Logger, Format, ...)												\
{																							\
	std::string strLog = ICCFormat(Format, __VA_ARGS__);									\
	Logger->Write(4, strLog, __FILE__, __FUNCTION__, __LINE__, MODULE_NAME);				\
}
#define ICC_LOG_DEBUG(Logger, Format, ...)													\
{																							\
	std::string strLog = ICCFormat(Format, __VA_ARGS__);									\
	Logger->Write(5, strLog, __FILE__, __FUNCTION__, __LINE__, MODULE_NAME);				\
}
#define ICC_LOG_LOWDEBUG(Logger, Format, ...)												\
{																							\
	std::string strLog = ICCFormat(Format, __VA_ARGS__);									\
	Logger->Write(6, strLog, __FILE__, __FUNCTION__, __LINE__, MODULE_NAME);				\
}


#else

#define ICC_LOG_INFO(Logger, Format, ...)													\
{																							\
	std::string strLog = ICCFormat(Format, ##__VA_ARGS__);									\
	Logger->Write(1, strLog, __FILE__, __FUNCTION__, __LINE__, MODULE_NAME);					\
}

#define ICC_LOG_ERROR(Logger, Format, ...)													\
{																							\
	std::string strLog = ICCFormat(Format, ##__VA_ARGS__);									\
	Logger->Write(2, strLog, __FILE__, __FUNCTION__, __LINE__, MODULE_NAME);					\
}
#define ICC_LOG_FATAL(Logger, Format, ...)													\
{																							\
	std::string strLog = ICCFormat(Format, ##__VA_ARGS__);									\
	Logger->Write(3, strLog, __FILE__, __FUNCTION__, __LINE__, MODULE_NAME);					\
}
#define ICC_LOG_WARNING(Logger, Format, ...)												\
{																							\
	std::string strLog = ICCFormat(Format, ##__VA_ARGS__);									\
	Logger->Write(4, strLog, __FILE__, __FUNCTION__, __LINE__, MODULE_NAME);					\
}
#define ICC_LOG_DEBUG(Logger, Format, ...)													\
{																							\
	std::string strLog = ICCFormat(Format, ##__VA_ARGS__);									\
	Logger->Write(5, strLog, __FILE__, __FUNCTION__, __LINE__, MODULE_NAME);					\
}
#define ICC_LOG_LOWDEBUG(Logger, Format, ...)												\
{																							\
	std::string strLog = ICCFormat(Format, ##__VA_ARGS__);									\
	Logger->Write(6, strLog, __FILE__, __FUNCTION__, __LINE__, MODULE_NAME);				\
}

#endif

