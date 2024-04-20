#pragma once
#include <pcap.h>
#include <CommonFunc.h>
#include <RTPPackProc.h>

#define OPENMODE_LIVE 0
#define OPENMODE_FILE 1

#define PROTOCOL_UDP 0
#define PROTOCOL_TCP 1

#define DEVICE_PROMISCUOUS_MODE 1
#define DEVICE_NONPROMISCUOUS_MODE 0

//读取超时时间，单位：毫秒
#define DEVICE_READ_TIME_OUT 10 

#define PROTO_TYPE_TCP 6
#define PROTO_TYPE_UDP 17

#define RTP_PAYLOADTYPE_G711  8
namespace ICC
{
	//负责启动和停止抓取网卡的包,并进行分拣交由不同的Rtp包处理类进行处理
	class CCapDataProc
	{
	public:
		CCapDataProc(Log::ILogPtr pLog, const std::string& strDevNM, const std::string& strFilter, int Mode = OPENMODE_LIVE);
		~CCapDataProc();
		//设置音频参数： usVoiceInterval--音频发包间隔 默认20 uiSampleRate--音频采样率 默认8000 usCodeType--音频编码方式 默认FORMAT_G711 usSampleBits--音频采样深度 默认16bit
		int SetAudioPara(uint16_t usVoiceInterval = DefaultVoiceInterval, uint32_t uiSampleRate = DefaultSampleRate, uint16_t usCodeType = FORMAT_G711A, uint16_t usSampleBits = bitsPerSampleG711A);
		int SetFilter(const std::string& strFilter);
		int StartCap();
		int StopCap();
		int OnReceiveData(const u_char* pData, unsigned int ulDataLen, timeval tvTime);
	private:
		//处理抓取到的数据
		int OnReceiveCapData(uint8_t prototype, const char* cSrcIp, uint16_t usSrcPort, const char* cDstIp, uint16_t usDstPort, const u_char* pData, uint32_t ulDataLen, timeval tvTime);
		int OpenDevice(int Mode = OPENMODE_LIVE);
		int SetDeviceCaptureFilter(const std::string& strFilter);
		void run();
		int CloseDevice();
		int DoCapLoop();

		thread m_thread;
		bool m_isRunning;
		string m_strDeviceName;//网卡设备名称
		pcap_t* m_pstDevice;   //抓包网卡设备对象
		int m_iOpenMode;       //设备打开方式
		string m_strFilter;    //过滤条件

		Log::ILogPtr m_pLog;

	public:
		//放入抓取的网络RTP包,根据发送和接收IP确定由哪个RTPPackProc处理
		void PushRtpRawPacket(RTPRawPacket* pkg);

		void SetPaiDuiJiIp(const std::string& strPaiDuiJiIp);
		void RmvPaiDuiJiIp(const std::string& strPaiDuiJiIp);
		void ClearPaiDuiJiIp();

		//绑定呼叫信息
		bool BindCallInfo(const CallInfo& callInfo);

		//呼叫结束，开始存储录音文件
		bool StartMediaStorageByCallInfo(const CallInfo& callInfo);

		void StopAllProc()
		{
			CRTPPackProcPtr pGetAProc = PopFirstRtpProc();
			while (NULL != pGetAProc)
			{
				pGetAProc->Stop();
				pGetAProc = PopFirstRtpProc();
			}
		}

	private:
		bool IsPaiDuiJiIP(const std::string& strRtpIp);
		CRTPPackProcPtr CreateRtpProc(const std::string& strAgentIP);
		CRTPPackProcPtr GetRtpProc(const std::string& strAgentIP);
		CRTPPackProcPtr PopFirstRtpProc();
		void RemoveRtpProc(const std::string& strAgentIP);
		void ClearRtpProc();

		uint16_t m_usVoiceInterval; //音频发包间隔
		uint32_t m_uiSampleRate;    //音频采样率
		uint16_t m_usCodeType;      //音频编码方式
		uint16_t m_usSampleBits;    //音频采样深度

		long long m_llStorageTimeOut; //经过多久没有收到包就存文件（秒）

		mutex                    rtpProcMapMutex;
		std::map<std::string, CRTPPackProcPtr> m_mapRTPProc; //坐席IP与RTP包处理类的对应关系
		std::set<std::string> m_setPaiDuiJiIp; //排队机的IP集合
	};

	typedef std::shared_ptr<CCapDataProc> CCapDataProcPtr;
}
