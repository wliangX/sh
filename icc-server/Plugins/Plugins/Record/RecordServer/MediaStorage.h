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
	uint32_t cb_size; //�������С
	Base_chunk(FOURCC fourcc):fcc(fourcc), cb_size(0){
	}
};

#define FORMAT_PCM 0x01
#define FORMAT_G711A 0x06
#define bitsofbyte 8
#define DefaultSampleRate       8000  //Ĭ�ϲ�����8000
#define DefaultFileChannels     2
#define DefaultbitsPerSamplePCM 16   //PCM����λ��
#define bitsPerSampleG711A      8   //G711A����λ��
#define DefaultVoiceInterval    20   //Ĭ��һ�����ݵ��������(ms)


struct Wave_format {
	uint16_t format_tag;        //WAVE�����ݸ�ʽ, PCM���ݸ�ֵΪ1
	uint16_t channels;          //������ 1--������ 2--˫����
	uint32_t sample_per_sec;    //������ 8000 44100 48000 etc
	uint32_t bytes_per_sec;     //����  channels * sample_per_sec * bits_per_sample / 8
	uint16_t block_align;       //��Ƶ���ݿ飬ÿ�β�����������ݿ��С channels * bits_per_sample / 8
	uint16_t bits_per_sample;   //����λ�� 8 16 24 32��
	uint16_t ex_size;           //��չ��Ĵ�С ���ӿ�Ĵ�С
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

//дWAV�ļ���
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
	//ý�屣����
	class CMediaStorage
	{
	public:
		CMediaStorage(Log::ILogPtr pLog, const std::string& strAgentIP, uint16_t usAgentPort, uint16_t voice_interval = DefaultVoiceInterval,
			uint32_t sample_rate = DefaultSampleRate, uint16_t code_type = FORMAT_G711A, uint16_t sample_bits = bitsPerSampleG711A,
			 uint16_t nb_channel = DefaultFileChannels);
		~CMediaStorage();

		//�������������
		void PushInRTPRawPacket(RTPRawPacket* data);

		//�������������
		void PushOutRTPRawPacket(RTPRawPacket* data);

		//��ʼ�����ļ�
		int StartProcStorage();

		//���ú�����Ϣ
		int BindCTICallInfo(const CallInfo& callInfo);

		//��պ�����Ϣ
		int ClearCallInfo();

		//�Ƿ�ʱ��ƥ��
		bool IsMatchStartTime(uint64_t callStartTime);

		//��ȡ������Ķ˿ں�
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

		//��ȡ����յ�RTP����ʱ��
		RTPTime GetLastRtpTime();

		void GetRtpInLastTime(RTPTime& rtpStopTime);
		void GetRtpOutLastTime(RTPTime& rtpStopTime);

		void ProcRTPPacket();
		int WriteRtpPacket(const std::string strFileName, RTPPacket* pRtpLeftPacket, RTPPacket* pRtpRightPacket);
		int WriteWaveFile(RTPPacket* pRtpLeftPacket, RTPPacket* pRtpRightPacket);

		std::list<RTPRawPacket*> m_outraw;         //�浥��Ļ���������rtp���ݣ�ʹ��list��֤���յ�˳��ʹ����˳����ͬ
		mutex m_mtOutRaw;
		std::list<RTPRawPacket*> m_inraw;          //�浥��Ļ����յ���rtp����
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
		bool  m_isRunning;           //>����״̬�������̼߳���˳�

		unsigned short		  m_usAgentCallPort;     //������Ӧ�ĺ��ж˿�
		std::string			  m_strAgentIP;          //������Ӧ��AgentIP
		std::string			  m_strAgentID;          //������Ӧ��ID��ʶ
		std::set<std::string> m_setCallID;           //��Ӧ�ĺ���ID�����ܴ��ڶ������ID����ͬһ��¼���ļ���������˴�ʹ�ü���
		std::string			  m_strFileID;           //�ļ��������϶�Ӧ���ļ�ID

		string				m_strCaller;
		string				m_strCalled;
		bool                m_bIsBindCTICall;

		//RTPʱ��
		RTPTime             m_startTime;          //RTP��ʼʱ��
		RTPTime				m_stopTime;           //RTP����ʱ��

		unsigned short      m_rtpinterval;        //�������
		unsigned int        m_packet_len;         //����С
		CWaveFile           m_waveFile;           //wav�ļ�������
		Log::ILogPtr        m_pLog;               //��־������
	};
	typedef std::shared_ptr<CMediaStorage> CMediaStoragePtr;
}