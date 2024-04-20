#ifndef __COMPRESS_PROCESSER_H__
#define __COMPRESS_PROCESSER_H__

#include "Boost.h"
#include "SemDataList_T.h"
#include "zip.h"

#ifdef __APPLE__
// In darwin and perhaps other BSD variants off_t is a 64 bit value, hence no need for specific 64 bit functions
#define FOPEN_FUNC(filename, mode) fopen(filename, mode)
#define FTELLO_FUNC(stream) ftello(stream)
#define FSEEKO_FUNC(stream, offset, origin) fseeko(stream, offset, origin)
#else
#define FOPEN_FUNC(filename, mode) fopen64(filename, mode)
#define FTELLO_FUNC(stream) ftello64(stream)
#define FSEEKO_FUNC(stream, offset, origin) fseeko64(stream, offset, origin)
#endif

#define WRITEBUFFERSIZE (16384)
#define MAXFILENAME (256)

class CompressProcesser
{
	typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend> TextSink;

public:
	static CompressProcesser* Instance();
	static void Finalize();

protected:
	CompressProcesser();
	~CompressProcesser();

private:
	static CompressProcesser*                _Instance;

public:
	void Start(boost::shared_ptr<TextSink> sink);
	void SetSink(boost::shared_ptr<TextSink> sink);

	void SetCurrentFile();
	std::string GetCurrentFile();

	static void ThreadProc(void* pParam);

	static int CompressFile(const std::string& strFilePath);

	void SetFile();
	void GetFile(std::string& File);

	void RotateFile();
private:

	boost::shared_ptr<TextSink>    m_pSink;
	boost::thread*                 m_Thread;
	boost::mutex                   m_Mutex;
	SemDataList<std::string>       m_FileList;

	std::string m_strCruurentLogName;
};
#endif


