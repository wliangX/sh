#pragma once
#include <boost/thread/thread.hpp>
#include "CompressProcesser.h"

namespace ICC
{
	namespace Log
	{
		enum sign_severity_level
		{
			none,
			info,
			error,
			fatal,
			warning,
			debug,
			lowdebug
		};

		template < typename CharT, typename TraitsT >
		inline std::basic_ostream< CharT, TraitsT >& operator<< (std::basic_ostream< CharT, TraitsT >& strm, sign_severity_level lvl)
		{
			static const char* const str[] =
			{
				"none",
				"info",
				"error",
				"fatal",
				"warning",
				"debug",
				"lowdebug"
			};

			if (static_cast<std::size_t>(lvl) < (sizeof(str) / sizeof(*str)))
				strm << str[lvl];
			else
				strm << static_cast<int>(lvl);

			return strm;
		};

		/*
		* class   ��־ʵ����
		* author  w16314
		* purpose
		* note
		*/
		class CLogImpl :
			public ILog
		{
		public:
			CLogImpl(IResourceManagerPtr p_ResourceManagerPtr);
			virtual~CLogImpl();
		public:
			virtual void Write(unsigned int p_iLogLevel, const std::string& p_strLog, const std::string& p_strFile, const std::string& p_strFunc, unsigned int p_uiLine, const std::string& p_strModuleName);
			virtual void SetModuleName(const std::string& p_strModuleName);

			static void FileEnd(boost::log::sinks::text_file_backend::stream_type& file);
			static void WriteHeader(boost::log::sinks::text_file_backend::stream_type& file);
		private:
			void Init();
			void UnInit();

			//��ʱ����Ӧ
			void OnTimer(std::string p_strTimerName);

			int GetOldestFile(std::string& strDir, std::string& strOldestFile);
			int GetNewestFile(std::string& strDir, std::string& strNewestFile);

			void CheckLogFileIsExist();
		private:
			virtual IResourceManagerPtr GetResourceManager();
		private:
			IResourceManagerPtr m_pResourceManager;			
			StringUtil::IStringUtilPtr m_pString;
			Config::IConfigPtr m_pStaticConfig;
			//�߳�����
			std::string m_strThreadName;
			//���б�־
			volatile bool m_bIsRunning;
			//�߳�ʵ�����boost���߳�
			boost::shared_ptr<boost::thread> m_pThread;
			//io���������¼���������
			boost::asio::io_service m_IoService;
			//�ö�����Ϊ�˱�֤io_service�ڲ�����stop�������Զ���˳���ȷ������������
			boost::asio::io_service::work m_Permanence;
			boost::mutex m_Mutex;
			//��ʱ��
			boost::asio::deadline_timer m_Timer;   
			//��־��
			std::map<std::string, boost::log::sources::severity_channel_logger<sign_severity_level, std::string>> m_channelLoggers;
		private:
			std::string m_strTarget;
			std::string m_strAutoFlush;
			std::string m_strRotationSize;
			std::string m_strMaxSize;
			std::string m_strMinFreeSpace;
			std::string m_IsCompress;
			std::string m_OldestFile;
			unsigned int m_uiPreserveDays;
			unsigned int m_uiCheckInterval;
			unsigned int m_uiLogLevel;
		};
	}
}



