#include "Boost.h"
#include "LogImpl.h"

CLogImpl::CLogImpl(IResourceManagerPtr p_ResourceManagerPtr) :
	m_pResourceManager(p_ResourceManagerPtr),
	m_Permanence(m_IoService),
	m_Timer(m_IoService)//延迟启动
{
	printf("logger enter!\n");
	m_pStaticConfig = ICCGetIConfigFactory()->CreateStaticConfig();
	Init();
}

CLogImpl::~CLogImpl()
{
	UnInit();
}

void CLogImpl::Write(unsigned int p_iLogLevel, const std::string& p_strLog, const std::string& p_strFile, const std::string& p_strFunc, unsigned int p_uiLine, const std::string& p_strModuleName)
{
	std::string l_strFileName = p_strFile.substr(p_strFile.rfind('\\') + 1);
	BOOST_LOG_SEV(m_channelLoggers["icc"], sign_severity_level(p_iLogLevel)) << "[" << p_strModuleName << "]" << "[" << p_strFunc << "]" << "[" << l_strFileName << ":" << p_uiLine << "]" << p_strLog;
}

void CLogImpl::SetModuleName(const std::string& p_strModuleName)
{
	printf("set module name begin!\n");
	boost::log::sources::severity_channel_logger<sign_severity_level, std::string> l_moduleLogger(boost::log::keywords::channel = p_strModuleName);
	m_channelLoggers[p_strModuleName] = l_moduleLogger;
	
	std::string l_strSavePath = m_strTarget/* + "/" + p_strModuleName*/; //文件夹名
	std::string l_strFileName = l_strSavePath + "/" /* + p_strModuleName*/ + "%Y-%m-%d_%H-%M-%S.%N.log";//日志文件名
	
	printf("log path = %s\n", l_strFileName.c_str());

	boost::shared_ptr<boost::log::sinks::text_file_backend> backend = boost::make_shared<boost::log::sinks::text_file_backend>(
		boost::log::keywords::file_name = l_strFileName,//文件名
		boost::log::keywords::rotation_size = atoi(m_strRotationSize.c_str()) * 1024 * 1024,//单个文件限制大小 10M
		boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),//每天重建
		boost::log::keywords::open_mode = std::ios::app,
		boost::log::keywords::auto_flush = (m_strAutoFlush == "1") ? true : false
		);

	typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend> TextSink;
	boost::shared_ptr<TextSink> g_pSink;

	g_pSink = boost::make_shared<TextSink>(backend);

	g_pSink->locked_backend()->set_file_collector(boost::log::sinks::file::make_collector(
		boost::log::keywords::target = l_strSavePath,//目标文件夹
		boost::log::keywords::max_size = atoi(m_strMaxSize.c_str()) * 1024 * 1024, //文件夹所占最大空间,
		boost::log::keywords::min_free_space = atoi(m_strMinFreeSpace.c_str()) * 1024 * 1024 //磁盘最小预留空间
		));

	g_pSink->set_formatter
		(
		boost::log::expressions::stream
		<< "@[" << boost::log::expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S.%f") << "]"
		<< "[" << boost::log::expressions::attr<boost::log::attributes::current_thread_id::value_type>("ThreadID") << "]"
		<< "[" << boost::log::expressions::attr<sign_severity_level>("Severity") << "]"
		<< "[" << boost::log::expressions::attr<std::string>("Channel") << "]"
		<< boost::log::expressions::smessage
		);
	g_pSink->set_filter(boost::log::expressions::attr<sign_severity_level>("Severity") <= sign_severity_level(m_uiLogLevel) && boost::log::expressions::attr<std::string>("Channel") == p_strModuleName);
	g_pSink->locked_backend()->set_open_handler(&WriteHeader);
	if (0 == m_IsCompress.compare("true"))
	{
		g_pSink->locked_backend()->set_close_handler(&FileEnd);
		
		CompressProcesser::Instance()->Start(g_pSink);
	}
	else
	{
		CompressProcesser::Instance()->SetSink(g_pSink);
	}

	boost::log::core::get()->add_sink(g_pSink);
}

void CLogImpl::WriteHeader(boost::log::sinks::text_file_backend::stream_type& file)
{
	CompressProcesser::Instance()->SetCurrentFile();
}

void CLogImpl::FileEnd(boost::log::sinks::text_file_backend::stream_type& file)
{
	CompressProcesser::Instance()->SetFile();
}

void CLogImpl::Init()
{
	printf("logger init begin!\n");
	Config::IConfigPtr m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_uiLogLevel = atoi(m_pConfig->GetValue("ICC/Component/Logger/Level", "6").c_str());
	m_strTarget = m_pConfig->GetValue("ICC/Component/Logger/Target", "logs") + "/" + m_pStaticConfig->GetValue("ICC/Component/Logger/TargetMidPath", "");
	m_strAutoFlush = m_pConfig->GetValue("ICC/Component/Logger/AutoFlush", "1");
	m_strRotationSize = m_pConfig->GetValue("ICC/Component/Logger/RotationSize", "10");
	m_strMaxSize = m_pConfig->GetValue("ICC/Component/Logger/MaxSize", "1000");
	m_strMinFreeSpace = m_pConfig->GetValue("ICC/Component/Logger/MinFreeSpace", "1000");
	m_IsCompress = m_pConfig->GetValue("ICC/Component/Logger/IsCompress", "false");
	m_uiPreserveDays = atoi(m_pConfig->GetValue("ICC/Component/Logger/PreserveDays", "30").c_str());
	m_uiCheckInterval = atoi(m_pConfig->GetValue("ICC/Component/Logger/CheckInterval", "60").c_str());
	
	printf("logger target = %s\n", m_strTarget.c_str());
   
	if (0 != GetOldestFile(m_strTarget, m_OldestFile))
	{
		printf("GetOldestFile error [%s]\n", m_strTarget.c_str());
		//return;
	}

	std::string newest;
	if (0 != GetNewestFile(m_strTarget, newest))
	{
		printf("GetNewestFile error [%s]\n", m_strTarget.c_str());
		//return;
	}

	if ((!newest.empty()) && (0 == m_IsCompress.compare("true")) && (std::string::npos != newest.find(".log")))
	{
		CompressProcesser::CompressFile(newest);
	}

	try
	{
		printf("logger init 0!\n");
		boost::log::add_common_attributes();
	}
	catch (...)
	{
		std::cout << boost::current_exception_diagnostic_information() << std::endl;
	}
	
    printf("logger init 1!\n");
	try
	{
		boost::shared_ptr<boost::log::core> l_pCore = boost::log::core::get(); //get本身是线程安全的，不用添加同步

		printf("logger init 2!\n");
		l_pCore->set_logging_enabled(true);
		printf("logger init 3!\n");
	}
	catch (...)
	{
		std::cout << boost::current_exception_diagnostic_information() << std::endl;
	}	
	
	m_pThread = boost::make_shared<boost::thread>([this](){
		m_IoService.run();
	});

	boost::uuids::random_generator rgen;//随机生成器  
	boost::uuids::uuid u = rgen();//生成一个随机的UUID
	std::string p_strTimerName = boost::uuids::to_string(u);

	try
	{
		m_Timer.async_wait([this, p_strTimerName](const boost::system::error_code &){
			this->OnTimer(p_strTimerName);
		});

		m_Timer.expires_from_now(boost::posix_time::seconds(1));
	}
	catch (...)
	{
		std::cout << boost::current_exception_diagnostic_information() << std::endl;
	}
	
	printf("logger init end!\n");
}

void CLogImpl::UnInit()
{
	printf("uninit begin!\n");
	
	boost::log::core::get()->flush();
	boost::log::core::get()->remove_all_sinks();
	boost::log::core::get()->set_logging_enabled(false);

	m_IoService.stop();
	if (m_pThread)
	{
		m_pThread->join();
	}
}

IResourceManagerPtr CLogImpl::GetResourceManager()
{
	return m_pResourceManager;
}

//定时器响应
void CLogImpl::OnTimer(std::string p_strTimerName)
{
	//printf("ontime begin!\n");
	CheckLogFileIsExist();
	static int nCount = 0; 
	if (++nCount > m_uiCheckInterval)
	{
		nCount = 0;


		Config::IConfigPtr m_pConfigPtr = ICCGetIConfigFactory()->CreateConfig();
		m_uiLogLevel = atoi(m_pConfigPtr->GetValue("ICC/Component/Logger/Level", "6").c_str());
		m_uiPreserveDays = atoi(m_pConfigPtr->GetValue("ICC/Component/Logger/PreserveDays", "30").c_str());
		boost::log::core::get()->set_filter(boost::log::expressions::attr<sign_severity_level>("Severity") <= sign_severity_level(m_uiLogLevel));

		if ((m_OldestFile.empty()) || (!boost::filesystem::exists(m_OldestFile)))
		{
			if (0 != GetOldestFile(m_strTarget, m_OldestFile))
			{
				printf("GetOldestFile error [%s]\n", m_strTarget.c_str());
			}
		}

		if ((!m_OldestFile.empty()) && (boost::filesystem::exists(m_OldestFile)))
		{
			std::time_t t = boost::filesystem::last_write_time(m_OldestFile);
			std::time_t currTime = std::time(nullptr);

			if ((currTime - t) > (std::time_t)m_uiPreserveDays * 24 * 60 * 60)
			{
				boost::filesystem::remove(m_OldestFile);

				if (0 != GetOldestFile(m_strTarget, m_OldestFile))
				{
					printf("GetOldestFile error [%s]\n", m_strTarget.c_str());
				}
			}
		}
	}

	try
	{
		//激活下一次定时器
		m_Timer.expires_from_now(boost::posix_time::seconds(1));

		m_Timer.async_wait([this, p_strTimerName](const boost::system::error_code&) {
			this->OnTimer(p_strTimerName);
			});
	}
	catch (...)
	{

	}
}

int CLogImpl::GetOldestFile(std::string& strDir, std::string& strOldestFile)
{
	boost::filesystem::path strTmpDir(strDir);

	if (!boost::filesystem::exists(strTmpDir))
	{
		return -1;
	}

	std::string strOldest;
	std::time_t last = 0;
	boost::filesystem::directory_iterator itEnd;
	for (boost::filesystem::directory_iterator it(strTmpDir); it != itEnd; ++it)
	{
		if (boost::filesystem::is_directory(*it))
		{
			continue;
		}

		std::time_t t = boost::filesystem::last_write_time((*it).path().string());
		
		if (0 == last)
		{
			last = t;
			strOldest = (*it).path().string();
		}

		if (t < last)
		{
			last = t;
			strOldest = (*it).path().string();
		}
	}

	strOldestFile = strOldest;

	return 0;
}

int CLogImpl::GetNewestFile(std::string& strDir, std::string& strNewestFile)
{
	boost::filesystem::path strTmpDir(strDir);

	if (!boost::filesystem::exists(strTmpDir))
	{
		return -1;
	}

	std::string strNewest;
	std::time_t last = 0;
	boost::filesystem::directory_iterator itEnd;
	for (boost::filesystem::directory_iterator it(strTmpDir); it != itEnd; ++it)
	{
		if (boost::filesystem::is_directory(*it))
		{
			continue;
		}

		std::time_t t = boost::filesystem::last_write_time((*it).path().string());

		if (t > last)
		{
			last = t;
			strNewest = (*it).path().string();
		}
	}

	strNewestFile = strNewest;

	return 0;
}

void CLogImpl::CheckLogFileIsExist()
{
	std::string l_strFileName = CompressProcesser::Instance()->GetCurrentFile();
	if (l_strFileName.empty())
	{
		return;
	}
	if (boost::filesystem::exists(l_strFileName))
	{
		return;
	}
	bool bFlag = false;
#ifdef _WIN32
	std::string::size_type pos = l_strFileName.rfind("\\");
#else
	std::string::size_type pos = l_strFileName.rfind("/");
#endif

	std::string filePath = l_strFileName.substr(0, pos + 1);
	std::string fileName = l_strFileName.substr(pos + 1);

	if (!boost::filesystem::is_directory(filePath))
	{
		if (!boost::filesystem::create_directory(filePath))
		{
			//cout << "create_directories failed: " << filePath << endl;
			return;
		}
		printf("CheckLogFileIsExist -------------2222222222222222222222---%s---\n", l_strFileName.c_str());
		//boost::filesystem::ofstream file(l_strFileName);
		//file << "\n";
		//file.close();
		bFlag = true;
	}
	else
	{
		printf("CheckLogFileIsExist -------------1111111111111111111111---%s---\n",l_strFileName.c_str());
		//cout << filePath << " aleardy exist" << end
		bFlag = true;
	}

	if (bFlag)
	{
		//boost::log::core::get()->remove_all_sinks();
		CompressProcesser::Instance()->RotateFile();
		//SetModuleName("icc");
	}
}
