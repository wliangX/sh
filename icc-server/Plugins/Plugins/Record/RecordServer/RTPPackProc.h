#pragma once
#include <RTPPacket.h>
#include <CommonFunc.h>
#include <MediaStorage.h>
#include <CommonTimer.h>

//************************
//RTP�������࣬����Դ��ַ����Ŀ�ĵ�ַȷ�����ĸ�RTPWorker���д���
//����agentid��Ӧ��IP��Ӧһ����
//
//************************
namespace ICC
{
	class CRTPPackProc : public CCommonTimerCore
	{
	public:
		//����˵����pLog--��־��ӡָ��, strAgentIP--��ϯ�绰IP, ullAudioTimeOut--��ʱδ���յ�������ʼд�ļ�(��λ:����)
		//usVoiceInterval--��Ƶ����ʱ����, uiSampleRate--������, usCodeType--��Ƶ��ʽ, usSampleBits--������С
		CRTPPackProc(Log::ILogPtr pLog, const std::string& strAgentIP, long long ullAudioTimeOut = 2000, uint16_t usVoiceInterval = DefaultVoiceInterval,
			uint32_t uiSampleRate = DefaultSampleRate, uint16_t usCodeType = FORMAT_G711A, uint16_t usSampleBits = bitsPerSampleG711A);
		~CRTPPackProc();

		void PushRTPRawPacket(RTPRawPacket* data);

		//ĳһ���˿ڵ�RTP����ʼ�洢�����ļ�
		void StartMediaStorage(uint16_t usRtpPort);

		//����ĳһ���˿ڵ�ҵ�������Ϣ
		bool BindCallInfo(const CallInfo& callInfo);

		//����һ������,����ʼ�洢�ļ�
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
		std::list<RTPRawPacket*> rawpacketlist;//��˫���rtp����
		mutex                    rawlist_mt;

		//rtp���洢����
		CMediaStoragePtr CreateMediaStorage(unsigned short usAgentPort);
		CMediaStoragePtr GetMediaStorage(unsigned short usAgentPort);
		void RmvMediaStorage(unsigned short usAgentPort);
		CMediaStoragePtr GetFirstMediaStorage();

		//ͨ��������Ϣ���Ҷ�Ӧ��ý��洢����
		CMediaStoragePtr GetMediaStorageByCallInfo(const CallInfo& callInfo);

		uint16_t m_usVoiceInterval; //��Ƶ�������
		uint32_t m_uiSampleRate;    //��Ƶ������
		uint16_t m_usCodeType;      //��Ƶ���뷽ʽ
		uint16_t m_usSampleBits;    //��Ƶ�������


		std::map<unsigned short, CMediaStoragePtr> m_mapMediaStore;
		mutex                    m_mtMediaStorage;

		std::string              m_strAgentIp;
		std::thread              m_thread;
		std::atomic_bool		 m_isRunning; //>����״̬�������̼߳���˳�
		Semaphore				 m_event;


		void SetPortLastReceiveTime(unsigned short usAgentPort);
		void GetTimeOutPort(long long llTimeOutSpan, std::vector<unsigned short>& vecTimeOutPorts);
		void RmvPortLastReceiveTime(unsigned short usAgentPort);

		std::map<unsigned short, std::chrono::system_clock::time_point> m_usPortlastReceiveTime; //ÿ���˿ڵ������հ���ʱ��
		mutex                    m_mtPortlastReceiveTime;
		unsigned long            m_ulStorageTimerID;  //����Ƿ񱣴涨ʱ��
		long long                m_llStorageTimeOut;  //��ʱд�ļ�(��λ����)


		mutex                    m_tmpCallInfoMutex;
		CallInfoPtr              m_pTmpCallInfo;//agentid�������Ϣ�Ļ���


		Log::ILogPtr			 m_pLog;//��־��ӡ��
	};
	typedef std::shared_ptr<CRTPPackProc> CRTPPackProcPtr;
}
