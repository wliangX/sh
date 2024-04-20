#include <CapDataProc.h>
#include <CommonFunc.h>
#include <RTPPackProc.h>
#include <RTPRawPacket.h>
#include <AgentIPMap.h>

namespace ICC
{
    std::string timevalToString(timeval tv)
    {
        char buffSec[20] = { 0 };
        char buffTime[24] = { 0 };
        time_t tSecs = tv.tv_sec;
        tm* tm_time = localtime(&tSecs);
        strftime(buffSec, 20, "%Y%m%d%H%M%S", tm_time);
        snprintf(buffTime, 24, "%s %03u", buffSec, tv.tv_usec / 1000);
        return std::string(buffTime);
    }

    void getPacket(u_char* arg, const struct pcap_pkthdr* pkthdr, const u_char* packet)
    {
        CCapDataProc* pCapture = (CCapDataProc*)arg;
        //printf("id: %x\n", pCapture);

        if (NULL != pCapture)
        {
            pCapture->OnReceiveData(packet, pkthdr->len, pkthdr->ts);
        }
        /*FILE* fp = fopen("rtp.txt", "ab+");
        if (fp)
        {
            //fwrite(packet,pkthdr->len,1,fp);
            unsigned int i;
            char buff[4] = { 0 };
            for (i = 0; i < pkthdr->len; ++i)
            {
                snprintf(buff, 4, " %02x", packet[i]);
                fwrite(buff, 3, 1, fp);
                if ((i + 1) % 16 == 0)
                {
                    //printf("\n");
                    fwrite("\n", 1, 1, fp);
                }
            }
            fwrite("\n", 1, 1, fp);
            fclose(fp);
            fp = NULL;
        }
        printf("\n\n");*/
    }

    CCapDataProc::CCapDataProc(Log::ILogPtr pLog, const std::string& strDevNM, const std::string& strFilter, int Mode /*= OPENMODE_LIVE*/) :
        m_strDeviceName(strDevNM), m_pstDevice(NULL), m_isRunning(false), m_iOpenMode(Mode), m_pLog(pLog), m_strFilter(strFilter),
        m_usVoiceInterval(DefaultVoiceInterval), m_uiSampleRate(DefaultSampleRate), m_usCodeType(FORMAT_G711A), m_usSampleBits(bitsPerSampleG711A), m_llStorageTimeOut(2)
    {
        ;
    }
    CCapDataProc::~CCapDataProc()
    {
        StopCap();
        CloseDevice();
        m_strDeviceName = "";
    }
    int CCapDataProc::SetAudioPara(uint16_t usVoiceInterval, uint32_t uiSampleRate, uint16_t usCodeType, uint16_t usSampleBits)
    {
        m_usVoiceInterval = usVoiceInterval;
        m_uiSampleRate = uiSampleRate;
        m_usCodeType = usCodeType;
        m_usSampleBits = usSampleBits;
        return 0;
    }
    int CCapDataProc::OpenDevice(int Mode/* = OPENMODE_LIVE*/)
    {
        char errBuf[PCAP_ERRBUF_SIZE];
        if (Mode == OPENMODE_LIVE)
        {
            //pcap_open_live的参数：设备名称、接收缓存的大小、是否开启混杂模式、等待的毫秒数、存放错误输出的数组
            m_pstDevice = pcap_open_live(m_strDeviceName.c_str(), BUFSIZ, DEVICE_PROMISCUOUS_MODE, DEVICE_READ_TIME_OUT, errBuf);
            if (!m_pstDevice)
            {
                ICC_LOG_ERROR(m_pLog, "error: pcap_open_live(): %s", errBuf);
                return -1;
            }
        }
        else
        {
            m_pstDevice = pcap_open_offline(m_strDeviceName.c_str(), errBuf);
            if (!m_pstDevice)
            {
                ICC_LOG_ERROR(m_pLog, "error: pcap_open_offline(): %s", errBuf);
                return -1;
            }
        }

        int interface_type = pcap_datalink(m_pstDevice);
        ICC_LOG_INFO(m_pLog, "device name:%s interface type: %d OpenMode:%d", m_strDeviceName.c_str(), interface_type, Mode);

        return 0;
    }

    int CCapDataProc::SetDeviceCaptureFilter(const std::string& strFilter)
    {
        if (NULL == m_pstDevice)
        {
            return -1;
        }
        /* construct a filter */
        struct bpf_program filter;
        int nRet = pcap_compile(m_pstDevice, &filter, strFilter.c_str(), 1, 0);
        if (0 != nRet)
        {
            ICC_LOG_ERROR(m_pLog, "error: pcap_compile(): %s ret %d", m_strDeviceName.c_str(), nRet);
            return -2;
        }
        nRet = pcap_setfilter(m_pstDevice, &filter);
        if (0 != nRet)
        {
            ICC_LOG_ERROR(m_pLog, "error: pcap_setfilter(): %s ret %d", m_strDeviceName.c_str(), nRet);
            return -3;
        }
        return 0;
    }

    int CCapDataProc::SetFilter(const std::string& strFilter)
    {
        m_strFilter = strFilter;
        return 0;
    }

    int CCapDataProc::StartCap()
    {
        if (!m_isRunning)
        {
            auto func = [this]() {this->run(); };
            m_isRunning = true;
            ICC_LOG_INFO(m_pLog, "CCapDataProc Start %s %d.", __FILE__, __LINE__);
            try
            {
                m_thread = std::thread(func);
            }
            catch (...)
            {
                m_isRunning = false;
                ICC_LOG_ERROR(m_pLog, "CCapDataProc Stop %s %d.", __FILE__, __LINE__);
                return -1;
            };
        }
        return 0;
    }

    void CCapDataProc::run()
    {
        while (m_isRunning)
        {
            int nRet = DoCapLoop();
            //正常退出或者调用LoopBreak退出，则退出线程
            if (0 == nRet || PCAP_ERROR_BREAK == nRet)
            {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }

    int CCapDataProc::DoCapLoop()
    {
        if (0 != OpenDevice(m_iOpenMode))
        {
            ICC_LOG_ERROR(m_pLog, "OpenDevice error mode:%d", m_iOpenMode);
            return -1;
        }
        if (0 != SetFilter(m_strFilter))
        {
            ICC_LOG_ERROR(m_pLog, "SetFilter error filter:%s", m_strFilter.c_str());
            return -1;
        }
        /* wait loop forever */
        int nRet = pcap_loop(m_pstDevice, -1, getPacket, (u_char*)this);
        ICC_LOG_INFO(m_pLog, "pcap_loop(): %s ret %d", m_strDeviceName.c_str(), nRet);
        CloseDevice();
        return nRet;
    }

    int CCapDataProc::StopCap()
    {
        if (m_isRunning)
        {
            m_isRunning = false;
            if (NULL != m_pstDevice)
            {
                pcap_breakloop(m_pstDevice);
            }
            m_thread.join();
        }
        return 0;
    }

    int CCapDataProc::CloseDevice()
    {
        if (NULL != m_pstDevice)
        {
            pcap_close(m_pstDevice);
            m_pstDevice = NULL;
        }
        return 0;
    }

    int CCapDataProc::OnReceiveData(const u_char* pData, unsigned int ulDataLen, timeval tvTime)
    {
        if (NULL == pData)
        {
            return -1;
        }
        const u_char* p = pData;
        unsigned int leftlen = ulDataLen;
        ETHERNETHEADER* ethernetHeader = (ETHERNETHEADER*)p;
        char dst_mac[20] = { 0 };
        char src_mac[20] = { 0 };
        int iIndex = 0;
        snprintf(dst_mac, 20, "%02x-%02x-%02x-%02x-%02x-%02x", ethernetHeader->dst[0], ethernetHeader->dst[1], ethernetHeader->dst[2], ethernetHeader->dst[3], ethernetHeader->dst[4], ethernetHeader->dst[5]);
        snprintf(src_mac, 20, "%02x-%02x-%02x-%02x-%02x-%02x", ethernetHeader->src[0], ethernetHeader->src[1], ethernetHeader->src[2], ethernetHeader->src[3], ethernetHeader->src[4], ethernetHeader->src[5]);
         p += sizeof(ETHERNETHEADER);
        leftlen -= sizeof(ETHERNETHEADER);

        IPHeader* iphdr = (IPHeader*)p;
        char srcip[20] = { 0 };
        char dstip[20] = { 0 };
        snprintf(srcip, 20, "%d.%d.%d.%d", iphdr->ip_src.S_un.S_un_b.s_b1, iphdr->ip_src.S_un.S_un_b.s_b2, iphdr->ip_src.S_un.S_un_b.s_b3, iphdr->ip_src.S_un.S_un_b.s_b4);
        snprintf(dstip, 20, "%d.%d.%d.%d", iphdr->ip_dst.S_un.S_un_b.s_b1, iphdr->ip_dst.S_un.S_un_b.s_b2, iphdr->ip_dst.S_un.S_un_b.s_b3, iphdr->ip_dst.S_un.S_un_b.s_b4);
        p += sizeof(IPHeader);
        leftlen -= sizeof(IPHeader);

        //ICC_LOG_DEBUG(m_pLog, "dst mac: %s src mac %s received Length %d time: %s ip_proto:%d ipversion:%d", dst_mac, src_mac, ulDataLen, timevalToString(tvTime).c_str(), iphdr->ip_p, ethernetHeader->ipversion == IP_V4 ? 4 : 6);
        if (iphdr->ip_p == PROTO_TYPE_UDP)
        {
            UDPHeader* udphdr = (UDPHeader*)p;
            p += sizeof(UDPHeader);
            leftlen -= sizeof(UDPHeader);
            return OnReceiveCapData(iphdr->ip_p, srcip, ntohs(udphdr->uh_sport), dstip, ntohs(udphdr->uh_dport), p, leftlen, tvTime);
        }
        else if (iphdr->ip_p == PROTO_TYPE_TCP)
        {
            TCPHeader* tcphdr = (TCPHeader*)p;
            //ICC_LOG_DEBUG(m_pLog, "tcp src ipport(%s:%d) dst ipport(%s:%d)\n", srcip, ntohs(tcphdr->th_sport), dstip, ntohs(tcphdr->th_dport));
            p += sizeof(TCPHeader);
            leftlen -= sizeof(TCPHeader);
            return OnReceiveCapData(iphdr->ip_p, srcip, ntohs(tcphdr->th_sport), dstip, ntohs(tcphdr->th_dport), p, leftlen, tvTime);
        }
        return 0;
    }

    int CCapDataProc::OnReceiveCapData(uint8_t prototype, const char* cSrcIp, uint16_t usSrcPort, const char* cDstIp, uint16_t usDstPort, const u_char* pData, uint32_t ulDataLen, timeval tvTime)
    {
        std::string strSrcIp = cSrcIp;
        std::string strDstIp = cDstIp;
        
        const RTPHeader* rtphdr = (const RTPHeader*)pData;
        uint8_t* rawpacket = new uint8_t[ulDataLen];
        memcpy(rawpacket, pData, ulDataLen);
        RTPIPAddress srcIpPort(strSrcIp, usSrcPort);
        RTPIPAddress dstIpPort(strDstIp, usDstPort);
        RTPTime receivetime(tvTime.tv_sec, tvTime.tv_usec);
        //ICC_LOG_DEBUG(m_pLog, "prototype %d src ipport(%s:%d) dst ipport(%s:%d) datalen %d payloadtype %d", prototype, strSrcIp.c_str(), usSrcPort, strDstIp.c_str(), usDstPort, ulDataLen, rtphdr->payloadtype);
        if (rtphdr->payloadtype == RTP_PAYLOADTYPE_G711)
        {
            //写临时RTP文件
            //FILE* fpcap = fopen("cap.rtp", "ab+");
            //if (NULL != fpcap)
            //{
            //    fwrite(pData, ulDataLen, 1, fpcap);
            //    fclose(fpcap);
            //    fpcap = NULL;
            //}
            RTPRawPacket* pRawPacket = new RTPRawPacket(rawpacket, ulDataLen, srcIpPort, dstIpPort, receivetime, 1);
            PushRtpRawPacket(pRawPacket);
        }
        else
        {
            delete rawpacket;
            rawpacket = NULL;
        }
        return 0;
    }


    void CCapDataProc::SetPaiDuiJiIp(const std::string& parastrPaiDuiJiIp)
    {
        m_setPaiDuiJiIp.insert(parastrPaiDuiJiIp);
    }
    void CCapDataProc::RmvPaiDuiJiIp(const std::string& strPaiDuiJiIp)
    {
        m_setPaiDuiJiIp.erase(strPaiDuiJiIp);
    }
    void CCapDataProc::ClearPaiDuiJiIp()
    {
        m_setPaiDuiJiIp.clear();
    }
    bool CCapDataProc::IsPaiDuiJiIP(const std::string& strRtpIp)
    {
        return m_setPaiDuiJiIp.find(strRtpIp) != m_setPaiDuiJiIp.end();
    }

    void CCapDataProc::PushRtpRawPacket(RTPRawPacket* pkg)
    {
        if (NULL == pkg)
        {
            return;
        }
        CRTPPackProcPtr pRtpPackProc = CRTPPackProcPtr((CRTPPackProc*)NULL);
        std::string strReceiverIP = pkg->GetReceiverAddress().strIP;
        std::string strSenderIP = pkg->GetSenderAddress().strIP;

        if (IsPaiDuiJiIP(strReceiverIP) && !IsPaiDuiJiIP(strSenderIP))
        {
            pRtpPackProc = GetRtpProc(strSenderIP);
            if (NULL == pRtpPackProc)
            {
                pRtpPackProc = CreateRtpProc(strSenderIP);
            }
        }
        else if (!IsPaiDuiJiIP(strReceiverIP) && IsPaiDuiJiIP(strSenderIP))
        {
            pRtpPackProc = GetRtpProc(strReceiverIP);
            if (NULL == pRtpPackProc)
            {
                pRtpPackProc = CreateRtpProc(strReceiverIP);
            }
        }
        else if (IsPaiDuiJiIP(strReceiverIP) && IsPaiDuiJiIP(strSenderIP))
        {
            pRtpPackProc = GetRtpProc(strReceiverIP);
            if (NULL == pRtpPackProc)
            {
                pRtpPackProc = GetRtpProc(strSenderIP);
                if (NULL == pRtpPackProc)
                {
                    pRtpPackProc = CreateRtpProc(strReceiverIP);
                }
            }
        }
        if (NULL != pRtpPackProc)
        {
            pRtpPackProc->PushRTPRawPacket(pkg);
        }
        else
        {
            delete pkg;
            pkg = NULL;
        }
    }
    CRTPPackProcPtr CCapDataProc::CreateRtpProc(const std::string& strAgentIP)
    {
        lock_guard<mutex> autolock(rtpProcMapMutex);
        CRTPPackProcPtr newRTPPackProc = CRTPPackProcPtr(new CRTPPackProc(m_pLog, strAgentIP, m_llStorageTimeOut*1000, m_usVoiceInterval, m_uiSampleRate, m_usCodeType, m_usSampleBits));
        m_mapRTPProc.insert(std::make_pair<>(strAgentIP, newRTPPackProc));
        return newRTPPackProc;
    }
    void CCapDataProc::RemoveRtpProc(const std::string& strAgentIP)
    {
        lock_guard<mutex> autolock(rtpProcMapMutex);
        m_mapRTPProc.erase(strAgentIP);
    }
    void CCapDataProc::ClearRtpProc()
    {
        lock_guard<mutex> autolock(rtpProcMapMutex);
        m_mapRTPProc.clear();
    }

    CRTPPackProcPtr CCapDataProc::GetRtpProc(const std::string& strAgentIP)
    {
        lock_guard<mutex> autolock(rtpProcMapMutex);
        if (m_mapRTPProc.find(strAgentIP) != m_mapRTPProc.end())
        {
            return m_mapRTPProc[strAgentIP];
        }
        return CRTPPackProcPtr((CRTPPackProc*)NULL);
    }

    CRTPPackProcPtr CCapDataProc::PopFirstRtpProc()
    {
        lock_guard<mutex> autolock(rtpProcMapMutex);
        if (m_mapRTPProc.begin() != m_mapRTPProc.end())
        {
            CRTPPackProcPtr pRtpProc = m_mapRTPProc.begin()->second;
            m_mapRTPProc.erase(m_mapRTPProc.begin());
            return pRtpProc;
        }
        return CRTPPackProcPtr((CRTPPackProc*)NULL);
    }

    bool CCapDataProc::BindCallInfo(const CallInfo& callInfo)
    {
        if (!callInfo.m_strAgentID.empty())
        {
            std::string strAgentIP = CAgentIPTable::GetInstance().QueryIPByPhone(callInfo.m_strAgentID);
            if (!strAgentIP.empty())
            {
                CRTPPackProcPtr pRtpPackProc = GetRtpProc(strAgentIP);
                if (NULL != pRtpPackProc)
                {
                    return pRtpPackProc->BindCallInfo(callInfo);
                }
                else
                {
                    pRtpPackProc = CreateRtpProc(strAgentIP);
                    CallInfoPtr pCallInfo = std::make_shared<CallInfo>(callInfo);
                    pRtpPackProc->AddTmpCallInfo(pCallInfo);
                }
            }
            else
            {
                ICC_LOG_DEBUG(m_pLog, "agent id :%s ip is emtpy", callInfo.m_strAgentID.c_str());
            }
        }
        else if (!callInfo.m_strAgentIP.empty())
        {
            CRTPPackProcPtr pRtpPackProc = GetRtpProc(callInfo.m_strAgentIP);
            if (NULL != pRtpPackProc)
            {
                return pRtpPackProc->BindCallInfo(callInfo);
            }
            else
            {
                pRtpPackProc = CreateRtpProc(callInfo.m_strAgentIP);
                CallInfoPtr pCallInfo = std::make_shared<CallInfo>(callInfo);
                pRtpPackProc->AddTmpCallInfo(pCallInfo);
            }
        }
        

        return false;
    }

    bool CCapDataProc::StartMediaStorageByCallInfo(const CallInfo& callInfo)
    {
        //使用呼叫消息触发保存时，等待200ms待所有包都接收完毕
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        if (!callInfo.m_strAgentID.empty())
        {
            std::string strAgentIP = CAgentIPTable::GetInstance().QueryIPByPhone(callInfo.m_strAgentID);
            CRTPPackProcPtr pRtpPackProc = GetRtpProc(strAgentIP);
            if (NULL != pRtpPackProc)
            {
                return pRtpPackProc->StartMediaStorageByCallInfo(callInfo);
            }
        }
        else if (!callInfo.m_strAgentIP.empty())
        {
            CRTPPackProcPtr pRtpPackProc = GetRtpProc(callInfo.m_strAgentIP);
            if (NULL != pRtpPackProc)
            {
                return pRtpPackProc->StartMediaStorageByCallInfo(callInfo);
            }
        }
        return false;
    }
}

