#include "CompressProcesser.h"

CompressProcesser* CompressProcesser::_Instance = NULL;

CompressProcesser* CompressProcesser::Instance()
{
	if (NULL == _Instance)
	{
		_Instance = new CompressProcesser;
	}

	return _Instance;
}

void CompressProcesser::Finalize()
{
	if (NULL != _Instance)
	{
		delete _Instance;
		_Instance = NULL;
	}
}

CompressProcesser::CompressProcesser()
{
	m_Thread = NULL;
	m_pSink = NULL;
	m_strCruurentLogName = "";
}

CompressProcesser::~CompressProcesser()
{
	if(NULL != m_Thread)
	{
		delete m_Thread;
		m_Thread = NULL;
	}
}

void CompressProcesser::SetSink(boost::shared_ptr<TextSink> sink)
{
	//if (NULL == m_pSink)
	{
		m_pSink = sink;
	}
}

void CompressProcesser::SetCurrentFile()
{
	if (NULL == m_pSink)
	{
		return;
	}

	boost::filesystem::path path = m_pSink->locked_backend()->get_current_file_name();
	std::string tmp = path.string();
	m_strCruurentLogName = tmp;

	printf("SetCurrentFile=[%s]!\n", tmp.c_str());
}

std::string CompressProcesser::GetCurrentFile()
{
	return m_strCruurentLogName;
}

void CompressProcesser::RotateFile()
{
	if (NULL == m_pSink)
	{
		return;
	}
	m_strCruurentLogName = "";
	m_pSink->locked_backend()->rotate_file();
}

void CompressProcesser::Start(boost::shared_ptr<TextSink> sink)
{
	m_pSink = sink;

	if (NULL == m_Thread)
	{
		m_Thread = new boost::thread(ThreadProc, this);
	}
}

void CompressProcesser::ThreadProc(void* pParam)
{
	CompressProcesser*th = NULL;
	if( pParam )
	{
		th = (CompressProcesser*)pParam;
	}
	else
	{
		return;
	}

	while(true)
	{
		std::string file;
		th->GetFile(file);
		try
		{
			CompressProcesser::CompressFile(file);
		}
		catch (...)
		{
			printf("CompressProcesser::CompressFile exception");
			boost::this_thread::sleep(boost::posix_time::millisec(3000));
			continue;
		}
	}
}

int CompressProcesser::CompressFile(const std::string& strFilePath)
{
#ifdef _WIN32
	std::string::size_type pos = strFilePath.rfind("\\");
#else
	std::string::size_type pos = strFilePath.rfind("/");
#endif

	if (pos == std::string::npos)
	{
		printf("FilePath Error [%s]\n", strFilePath.c_str());
		return -1;
	}

	std::string filePath = strFilePath.substr(0, pos + 1);
	std::string fileName = strFilePath.substr(pos+1);

	std::string::size_type pos2 = fileName.find(".log");

	if (pos2 == std::string::npos)
	{
		printf("FileName Error [%s]\n", fileName.c_str());
		return -1;
	}

	std::string timeName = fileName.substr(0, pos2);

	std::string outFile = filePath + timeName + ".zip";

	if (boost::filesystem::exists(outFile))
	{
		printf("zip file is exist! [%s]", outFile.c_str());

		return -1;
	}

	zipFile zf;
	zf = zipOpen64(outFile.c_str(), 0);
	if (zf == NULL)
	{
		printf("zipOpen64 error [%s]", outFile.c_str());
		return -1;
	}

	zip_fileinfo zi;
	try
	{
		std::time_t stm = boost::filesystem::last_write_time(strFilePath);
		struct tm* filedate = localtime(&stm);

		zi.tmz_date.tm_sec = filedate->tm_sec;
		zi.tmz_date.tm_min = filedate->tm_min;
		zi.tmz_date.tm_hour = filedate->tm_hour;
		zi.tmz_date.tm_mday = filedate->tm_mday;
		zi.tmz_date.tm_mon = filedate->tm_mon;
		zi.tmz_date.tm_year = filedate->tm_year;
		zi.dosDate = 0;
		zi.internal_fa = 0;
		zi.external_fa = 0;
	}
	catch (boost::filesystem::filesystem_error& err)
	{
		printf("filesystem_error [%s],error what [%s]", strFilePath.c_str(), err.what());
		return -1;
	}
	catch(...)
	{
		printf("exception exists!");
		return -1;
	}

	int err = 0;
	err = zipOpenNewFileInZip(zf, fileName.c_str(), &zi,
		NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
	if (err != ZIP_OK)
	{
		printf("zipOpenNewFileInZip failed!!! [%s]", fileName.c_str());
		zipClose(zf, NULL);
		return -1;
	}

	FILE* fin;
	fin = FOPEN_FUNC(strFilePath.c_str(), "rb");
	if (fin == NULL)
	{
		printf("FOPEN_FUNC error [%s]", strFilePath.c_str());
		zipCloseFileInZip(zf);
		zipClose(zf, NULL);
		return -1;
	}

	int size_read;
	int size_buf = 0;
	char buf[WRITEBUFFERSIZE] = { 0 };
	size_buf = WRITEBUFFERSIZE;

	do
	{
		err = ZIP_OK;
		size_read = (int)fread(buf, 1, size_buf, fin);
		if (size_read < size_buf)
		{
			if (feof(fin) == 0)
			{
				printf("fread error [%s]", strFilePath.c_str());
				err = ZIP_ERRNO;
			}
		}

		if (size_read > 0)
		{
			err = zipWriteInFileInZip(zf, buf, size_read);
			if (err < 0)
			{
				printf("zipWriteInFileInZip error [%s]", strFilePath.c_str());
			}
		}
	} while ((err == ZIP_OK) && (size_read > 0));

	if (fin)
	{
		fclose(fin);
	}

	if (err < 0)
	{
		err = ZIP_ERRNO;
	}
	else
	{
		err = zipCloseFileInZip(zf);
		if (err != ZIP_OK)
		{
			printf("zipCloseFileInZip error [%s]", strFilePath.c_str());
		}

	}

	int errclose;
	errclose = zipClose(zf, NULL);
	if (errclose != ZIP_OK)
	{
		printf("zipClose error [%s]", outFile.c_str());
		return -1;
	}

	boost::filesystem::remove(strFilePath);

	return 0;
}

void CompressProcesser::SetFile()
{
	if(NULL == m_pSink)
	{
		return;
	}

	boost::filesystem::path path = m_pSink->locked_backend()->get_current_file_name();

	std::string tmp = path.string();
	m_FileList.Put(tmp);
	
	printf("SetFile=[%s]!\n", tmp.c_str());
}

void CompressProcesser::GetFile(std::string& File)
{
	m_FileList.Get(File);
	printf("GetFile=[%s]!\n", File.c_str());
}

