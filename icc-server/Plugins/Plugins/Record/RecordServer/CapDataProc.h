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

//��ȡ��ʱʱ�䣬��λ������
#define DEVICE_READ_TIME_OUT 10 

#define PROTO_TYPE_TCP 6
#define PROTO_TYPE_UDP 17

#define RTP_PAYLOADTYPE_G711  8
namespace ICC
{
	//����������ֹͣץȡ�����İ�,�����зּ��ɲ�ͬ��Rtp����������д���
	class CCapDataProc
	{
	public:
		CCapDataProc(Log::ILogPtr pLog, const std::string& strDevNM, const std::string& strFilter, int Mode = OPENMODE_LIVE);
		~CCapDataProc();
		//������Ƶ������ usVoiceInterval--��Ƶ������� Ĭ��20 uiSampleRate--��Ƶ������ Ĭ��8000 usCodeType--��Ƶ���뷽ʽ Ĭ��FORMAT_G711 usSampleBits--��Ƶ������� Ĭ��16bit
		int SetAudioPara(uint16_t usVoiceInterval = DefaultVoiceInterval, uint32_t uiSampleRate = DefaultSampleRate, uint16_t usCodeType = FORMAT_G711A, uint16_t usSampleBits = bitsPerSampleG711A);
		int SetFilter(const std::string& strFilter);
		int StartCap();
		int StopCap();
		int OnReceiveData(const u_char* pData, unsigned int ulDataLen, timeval tvTime);
	private:
		//����ץȡ��������
		int OnReceiveCapData(uint8_t prototype, const char* cSrcIp, uint16_t usSrcPort, const char* cDstIp, uint16_t usDstPort, const u_char* pData, uint32_t ulDataLen, timeval tvTime);
		int OpenDevice(int Mode = OPENMODE_LIVE);
		int SetDeviceCaptureFilter(const std::string& strFilter);
		void run();
		int CloseDevice();
		int DoCapLoop();

		thread m_thread;
		bool m_isRunning;
		string m_strDeviceName;//�����豸����
		pcap_t* m_pstDevice;   //ץ�������豸����
		int m_iOpenMode;       //�豸�򿪷�ʽ
		string m_strFilter;    //��������

		Log::ILogPtr m_pLog;

	public:
		//����ץȡ������RTP��,���ݷ��ͺͽ���IPȷ�����ĸ�RTPPackProc����
		void PushRtpRawPacket(RTPRawPacket* pkg);

		void SetPaiDuiJiIp(const std::string& strPaiDuiJiIp);
		void RmvPaiDuiJiIp(const std::string& strPaiDuiJiIp);
		void ClearPaiDuiJiIp();

		//�󶨺�����Ϣ
		bool BindCallInfo(const CallInfo& callInfo);

		//���н�������ʼ�洢¼���ļ�
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

		uint16_t m_usVoiceInterval; //��Ƶ�������
		uint32_t m_uiSampleRate;    //��Ƶ������
		uint16_t m_usCodeType;      //��Ƶ���뷽ʽ
		uint16_t m_usSampleBits;    //��Ƶ�������

		long long m_llStorageTimeOut; //�������û���յ����ʹ��ļ����룩

		mutex                    rtpProcMapMutex;
		std::map<std::string, CRTPPackProcPtr> m_mapRTPProc; //��ϯIP��RTP��������Ķ�Ӧ��ϵ
		std::set<std::string> m_setPaiDuiJiIp; //�Ŷӻ���IP����
	};

	typedef std::shared_ptr<CCapDataProc> CCapDataProcPtr;
}
