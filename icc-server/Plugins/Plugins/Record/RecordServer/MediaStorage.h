#pragma once
#include <RTPPacket.h>
#include <CommonFunc.h>

#pragma pack(push)
#pragma pack(1)
#define FOURCC uint32_t
#define MAKE_FOURCC(a,b,c,d) \
( ((uint32_t)d)|( ((uint32_t)c)<<8 )|( ((uint32_t)b)<<16 )|( ((uint32_t)a)<<24 ) )
template <char ch0, char ch1, char ch2, char ch3> struct MakeFOURCC { enum { value=(ch0<<0)+(ch1<<8)+(ch2<<16)+(ch3<<24) }; };
struct Base_chunk {
	FOURCC fcc; //FourCC id
	uint32_t cb_size; //数据域大小
	Base_chunk(FOURCC fourcc):fcc(fourcc), cb_size(0){
	}
};

#define FORMAT_PCM 0x01
#define FORMAT_G711A 0x06
#define bitsofbyte 8
#define DefaultSampleRate       8000  //默认采样率8000
#define DefaultFileChannels     2
#define DefaultbitsPerSamplePCM 16   //PCM量化位数
#define bitsPerSampleG711A      8   //G711A量化位数
#define DefaultVoiceInterval    20   //默认一包数据的语音间隔(ms)


struct Wave_format {
	uint16_t format_tag;        //WAVE的数据格式, PCM数据该值为1
	uint16_t channels;          //声道数 1--单声道 2--双声道
	uint32_t sample_per_sec;    //采样率 8000 44100 48000 etc
	uint32_t bytes_per_sec;     //码率  channels * sample_per_sec * bits_per_sample / 8
	uint16_t block_align;       //音频数据块，每次采样处理的数据块大小 channels * bits_per_sample / 8
	uint16_t bits_per_sample;   //量化位数 8 16 24 32等
	uint16_t ex_size;           //扩展块的大小 附加块的大小
	Wave_format():format_tag(1), channels(0), sample_per_sec(0), bytes_per_sec(0), block_align(0), bits_per_sample(0), ex_size(0)
	{

	}
	Wave_format(uint16_t nb_channel, uint32_t sample_rate, uint16_t sample_bits, uint16_t voice_format) :format_tag(voice_format), channels(nb_channel), sample_per_sec(sample_rate), bits_per_sample(sample_bits)
	{
		bytes_per_sec = channels * sample_per_sec * bits_per_sample / bitsofbyte;
		block_align = channels * bits_per_sample / bitsofbyte;
		ex_size = 0;
	}
};
struct Wave_header {
	shared_ptr<Base_chunk> riff;
	FOURCC wave_fcc;
	shared_ptr<Base_chunk> fmt;
	shared_ptr<Wave_format> fmt_data;
	shared_ptr<Base_chunk> data;
	Wave_header(uint16_t nb_channel, uint32_t sample_rate, uint16_t sample_bits, uint16_t voice_fmt)
	{
		riff = make_shared<Base_chunk>(MakeFOURCC<'R','I','F','F'>::value);
		fmt = make_shared<Base_chunk>(MakeFOURCC<'f', 'm', 't', ' '>::value);
		fmt->cb_size = sizeof(Wave_format);
		fmt_data = make_shared<Wave_format>(nb_channel, sample_rate, sample_bits, voice_fmt);
		data = make_shared<Base_chunk>(MakeFOURCC<'d', 'a', 't', 'a'>::value);
		wave_fcc = MakeFOURCC<'W', 'A', 'V', 'E'>::value;
	}
	Wave_header()
	{
		riff = nullptr;
		fmt = nullptr;
		fmt_data = nullptr;
		data = nullptr;
		wave_fcc = 0;
	}
};
#pragma pack(pop)

//写WAV文件类
class CWaveFile
{
public:
	CWaveFile(uint16_t nb_channel, uint32_t sample_rate, uint16_t sample_bits, uint16_t voice_fmt);
	void SetFileName(const std::string& strFileName);
	int WriteHeader();
	int Write(const char* pPCMData, uint32_t pPCMLen);
	~CWaveFile();
private:
	std::string m_strFileName;
	Wave_header m_header;
};

namespace ICC
{
	//媒体保存类
	class CMediaStorage
	{
	public:
		CMediaStorage(Log::ILogPtr pLog, const std::string& strAgentIP, uint16_t usAgentPort, uint16_t voice_interval = DefaultVoiceInterval,
			uint32_t sample_rate = DefaultSampleRate, uint16_t code_type = FORMAT_G711A, uint16_t sample_bits = bitsPerSampleG711A,
			 uint16_t nb_channel = DefaultFileChannels);
		~CMediaStorage();

		//加入呼入语音包
		void PushInRTPRawPacket(RTPRawPacket* data);

		//加入呼出语音包
		void PushOutRTPRawPacket(RTPRawPacket* data);

		//开始保存文件
		int StartProcStorage();

		//设置呼叫信息
		int BindCTICallInfo(const CallInfo& callInfo);

		//清空呼叫信息
		int ClearCallInfo();

		//是否时间匹配
		bool IsMatchStartTime(uint64_t callStartTime);

		//获取保存类的端口号
		unsigned short GetMediaPort()
		{
			return m_usAgentCallPort;
		}

		std::string GetCallIDSetString()
		{
			std::string strTmp("");
			std::set<std::string>::iterator iter = m_setCallID.begin();
			for (;iter != m_setCallID.end();)
			{
				strTmp += *iter;
				if (++iter != m_setCallID.end())
				{
					strTmp += ";";
				}
			}
			return strTmp;
		}

	private:
		RTPRawPacket* PopInRtpRawPacket(RTPTime& pkgTime);
		RTPRawPacket* PopOutRtpRawPacket(RTPTime& pkgTime);

		//获取最后收到RTP包的时间
		RTPTime GetLastRtpTime();

		void GetRtpInLastTime(RTPTime& rtpStopTime);
		void GetRtpOutLastTime(RTPTime& rtpStopTime);

		void ProcRTPPacket();
		int WriteRtpPacket(const std::string strFileName, RTPPacket* pRtpLeftPacket, RTPPacket* pRtpRightPacket);
		int WriteWaveFile(RTPPacket* pRtpLeftPacket, RTPPacket* pRtpRightPacket);

		std::list<RTPRawPacket*> m_outraw;         //存单向的话机发出的rtp数据，使用list保证接收的顺序和处理的顺序相同
		mutex m_mtOutRaw;
		std::list<RTPRawPacket*> m_inraw;          //存单向的话机收到的rtp数据
		mutex m_mtInRaw;

		std::thread         m_thread;

		mutex               m_lkRunState;
		bool                TrySetRunState(bool bRunState)
		{
			lock_guard<mutex> lk(m_lkRunState);
			if (bRunState && m_isRunning)
			{
				return false;
			}
			m_isRunning = bRunState;
			return true;
		}
		bool  m_isRunning;           //>运行状态：用于线程检查退出

		unsigned short		  m_usAgentCallPort;     //话机对应的呼叫端口
		std::string			  m_strAgentIP;          //话机对应的AgentIP
		std::string			  m_strAgentID;          //话机对应的ID标识
		std::set<std::string> m_setCallID;           //对应的呼叫ID，可能存在多个呼叫ID关联同一个录音文件的情况，此处使用集合
		std::string			  m_strFileID;           //文件服务器上对应的文件ID

		string				m_strCaller;
		string				m_strCalled;
		bool                m_bIsBindCTICall;

		//RTP时间
		RTPTime             m_startTime;          //RTP开始时间
		RTPTime				m_stopTime;           //RTP结束时间

		unsigned short      m_rtpinterval;        //发包间隔
		unsigned int        m_packet_len;         //包大小
		CWaveFile           m_waveFile;           //wav文件保存类
		Log::ILogPtr        m_pLog;               //日志保存类
	};
	typedef std::shared_ptr<CMediaStorage> CMediaStoragePtr;
}