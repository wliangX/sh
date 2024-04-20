#pragma once
#include <RTPPacket.h>
#include <CommonFunc.h>
#include <MediaStorage.h>
#include <CommonTimer.h>

//************************
//RTP包处理类，根据源地址或者目的地址确定由哪个RTPWorker进行处理
//根据agentid对应的IP对应一个类
//
//************************
namespace ICC
{
	class CRTPPackProc : public CCommonTimerCore
	{
	public:
		//参数说明：pLog--日志打印指针, strAgentIP--坐席电话IP, ullAudioTimeOut--超时未接收到语音开始写文件(单位:毫秒)
		//usVoiceInterval--音频发包时间间隔, uiSampleRate--采样率, usCodeType--音频格式, usSampleBits--采样大小
		CRTPPackProc(Log::ILogPtr pLog, const std::string& strAgentIP, long long ullAudioTimeOut = 2000, uint16_t usVoiceInterval = DefaultVoiceInterval,
			uint32_t uiSampleRate = DefaultSampleRate, uint16_t usCodeType = FORMAT_G711A, uint16_t usSampleBits = bitsPerSampleG711A);
		~CRTPPackProc();

		void PushRTPRawPacket(RTPRawPacket* data);

		//某一个端口的RTP包开始存储本地文件
		void StartMediaStorage(uint16_t usRtpPort);

		//关联某一个端口的业务呼叫信息
		bool BindCallInfo(const CallInfo& callInfo);

		//结束一个呼叫,并开始存储文件
		bool StartMediaStorageByCallInfo(const CallInfo& callInfo);

		int Start();
		void Stop();
		void run();

		void OnTimer(unsigned long ulTimerID);

		void AddTmpCallInfo(CallInfoPtr pCallInfo);
		CallInfoPtr GetTmpCallInfo();
		void RmvTmpCallInfo();

	private:
		RTPRawPacket* PopRTPRawPacket();
		int ProcRTPPacket();
		std::list<RTPRawPacket*> rawpacketlist;//存双向的rtp数据
		mutex                    rawlist_mt;

		//rtp流存储管理
		CMediaStoragePtr CreateMediaStorage(unsigned short usAgentPort);
		CMediaStoragePtr GetMediaStorage(unsigned short usAgentPort);
		void RmvMediaStorage(unsigned short usAgentPort);
		CMediaStoragePtr GetFirstMediaStorage();

		//通过呼叫信息查找对应的媒体存储对象
		CMediaStoragePtr GetMediaStorageByCallInfo(const CallInfo& callInfo);

		uint16_t m_usVoiceInterval; //音频发包间隔
		uint32_t m_uiSampleRate;    //音频采样率
		uint16_t m_usCodeType;      //音频编码方式
		uint16_t m_usSampleBits;    //音频采样深度


		std::map<unsigned short, CMediaStoragePtr> m_mapMediaStore;
		mutex                    m_mtMediaStorage;

		std::string              m_strAgentIp;
		std::thread              m_thread;
		std::atomic_bool		 m_isRunning; //>运行状态：用于线程检查退出
		Semaphore				 m_event;


		void SetPortLastReceiveTime(unsigned short usAgentPort);
		void GetTimeOutPort(long long llTimeOutSpan, std::vector<unsigned short>& vecTimeOutPorts);
		void RmvPortLastReceiveTime(unsigned short usAgentPort);

		std::map<unsigned short, std::chrono::system_clock::time_point> m_usPortlastReceiveTime; //每个端口的最后接收包的时间
		mutex                    m_mtPortlastReceiveTime;
		unsigned long            m_ulStorageTimerID;  //检查是否保存定时器
		long long                m_llStorageTimeOut;  //超时写文件(单位毫秒)


		mutex                    m_tmpCallInfoMutex;
		CallInfoPtr              m_pTmpCallInfo;//agentid与呼叫信息的缓存


		Log::ILogPtr			 m_pLog;//日志打印类
	};
	typedef std::shared_ptr<CRTPPackProc> CRTPPackProcPtr;
}
