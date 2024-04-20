#include <RTPPackProc.h>
#include <AgentIPMap.h>

namespace ICC
{

    CRTPPackProc::CRTPPackProc(Log::ILogPtr pLog, const std::string& strAgentIP, long long llAudioTimeOut/*=2000*/, uint16_t usVoiceInterval/* = DefaultVoiceInterval*/,
        uint32_t uiSampleRate/* = DefaultSampleRate*/, uint16_t usCodeType/* = FORMAT_G711A*/, uint16_t usSampleBits/* = bitsPerSampleG711A*/) : 
        m_strAgentIp(strAgentIP), m_isRunning(false), 
        m_usVoiceInterval(usVoiceInterval), m_uiSampleRate(uiSampleRate), m_usCodeType(usCodeType), m_usSampleBits(usSampleBits),
        m_ulStorageTimerID(0), m_llStorageTimeOut(llAudioTimeOut), m_pTmpCallInfo(NULL), m_pLog(pLog)
    {
        Start();
    }
    CRTPPackProc::~CRTPPackProc()
    {
        Stop();
    }
    void CRTPPackProc::PushRTPRawPacket(RTPRawPacket* data)
    {
        if (NULL == data)
        {
            return;
        }
        {
            lock_guard<mutex> unique(rawlist_mt);
            rawpacketlist.push_back(data);
        }
        m_event.signal();
    }

    void CRTPPackProc::StartMediaStorage(uint16_t usRtpPort)
    {
        CMediaStoragePtr pMediaStorage = GetMediaStorage(usRtpPort);
        if (NULL != pMediaStorage)
        {
            RmvMediaStorage(usRtpPort);
            ICC_LOG_INFO(m_pLog, "StartMediaStorage by RtpPort : %u", usRtpPort);
            pMediaStorage->StartProcStorage();
        }
    }

    bool CRTPPackProc::BindCallInfo(const CallInfo& callInfo)
    {
        CMediaStoragePtr pMediaStorage = GetMediaStorageByCallInfo(callInfo);
        if (NULL != pMediaStorage)
        {
            pMediaStorage->BindCTICallInfo(callInfo);
            return true;
        }
        else
        {
            CallInfoPtr pCallInfo = std::make_shared<CallInfo>(callInfo);
            AddTmpCallInfo(pCallInfo);
        }
        return false;
    }

    bool CRTPPackProc::StartMediaStorageByCallInfo(const CallInfo& callInfo)
    {
        CMediaStoragePtr pMediaStorage = GetMediaStorageByCallInfo(callInfo);
        if (NULL != pMediaStorage)
        {
            //保存前检查之前有没有绑定呼叫信息，没有就重新绑定一次
            pMediaStorage->BindCTICallInfo(callInfo);
            RmvMediaStorage(pMediaStorage->GetMediaPort());
            ICC_LOG_INFO(m_pLog, "StartMediaStorage by callinfo : agent_id:%s callid:%s agentip:%s", callInfo.m_strAgentID.c_str(), callInfo.m_strCallID.c_str(), m_strAgentIp.c_str());
            pMediaStorage->StartProcStorage();
            return true;
        }
        return false;
    }


    RTPRawPacket* CRTPPackProc::PopRTPRawPacket()
    {
        lock_guard<mutex> unique(rawlist_mt);
        if (rawpacketlist.empty())
        {
            return NULL;
        }
        RTPRawPacket* rtpdata = rawpacketlist.front();
        rawpacketlist.pop_front();
        return rtpdata;
    }

    int CRTPPackProc::ProcRTPPacket()
    {
        RTPRawPacket* pRtpRaw = PopRTPRawPacket();
        if (NULL == pRtpRaw)
        {
            return -1;
        }

        if (pRtpRaw->GetSenderAddress().strIP == m_strAgentIp)
        {
            unsigned short usport = pRtpRaw->GetSenderAddress().usPort;
            SetPortLastReceiveTime(usport);//设置端口号最新接收时间
            CMediaStoragePtr pMediaStorage = GetMediaStorage(usport);
            if (NULL == pMediaStorage)
            {
                pMediaStorage = CreateMediaStorage(usport);
            }
            if (NULL != pMediaStorage)
            {
                pMediaStorage->PushOutRTPRawPacket(pRtpRaw);
                return 0;
            }

        }
        else if (pRtpRaw->GetReceiverAddress().strIP == m_strAgentIp)
        {
            unsigned short usport = pRtpRaw->GetReceiverAddress().usPort;
            SetPortLastReceiveTime(usport);//设置端口号最新接收时间
            CMediaStoragePtr pMediaStorage = GetMediaStorage(usport);
            if (NULL == pMediaStorage)
            {
                pMediaStorage = CreateMediaStorage(usport);
            }
            if (NULL != pMediaStorage)
            {
                pMediaStorage->PushInRTPRawPacket(pRtpRaw);
                return 0;
            }
        }
        //没有找到对应的AgentIP，则丢弃
        delete pRtpRaw;
        pRtpRaw = NULL;
        return -2;
    }

    int CRTPPackProc::Start()
    {
        if (!m_isRunning)
        {
            auto func = [this]() {this->run(); };
            m_isRunning = true;
            ICC_LOG_INFO(m_pLog, "RTPPackeProc Start : %s", m_strAgentIp.c_str());
            try
            {
                m_thread = std::thread(func);
            }
            catch (std::exception& ex)
            {
                m_isRunning = false;
                ICC_LOG_ERROR(m_pLog, "RTPPackeProc Start exception: %s", ex.what());
                return -1;
            };

            m_ulStorageTimerID = CCommonTimerManager::GetInstance().AddTimer(this, 1000); //1秒定时器
        }
        return 0;
    }
    void CRTPPackProc::Stop()
    {
        if (m_isRunning)
        {
            CCommonTimerManager::GetInstance().RemoveTimer(m_ulStorageTimerID);
            m_isRunning = false;
            ICC_LOG_INFO(m_pLog, "RTPPackeProc Stop : %s", m_strAgentIp.c_str());
            m_event.signal();
            if (m_thread.joinable())
            {
                m_thread.join();
            }

            CMediaStoragePtr pMediaStorage = GetFirstMediaStorage();
            while (NULL != pMediaStorage)
            {
                RmvMediaStorage(pMediaStorage->GetMediaPort());
                ICC_LOG_INFO(m_pLog, "StartMediaStorage by Stop : agentip:%s", m_strAgentIp.c_str());
                pMediaStorage->StartProcStorage();
                pMediaStorage = GetFirstMediaStorage();
            }
            ICC_LOG_INFO(m_pLog, "RTPPackeProc Stop end : %s", m_strAgentIp.c_str());
        };
    }
    void CRTPPackProc::run()
    {
        try
        {
            //static int iCount = 0;
            while (m_isRunning)
            {
                if (-1 != ProcRTPPacket())
                {
                    //ICC_LOG_DEBUG(m_pLog, "Write A Packet %d", ++iCount);
                }
                m_event.wait();
            }
            while (-1 != ProcRTPPacket())
            {
                ICC_LOG_DEBUG(m_pLog, "Write A Packet after not running : %s", m_strAgentIp.c_str());
            }
            ICC_LOG_INFO(m_pLog, "run end : %s", m_strAgentIp.c_str());
        }
        catch (std::exception& ex)
        {
            ICC_LOG_INFO(m_pLog, "exception : %s : %s", m_strAgentIp.c_str(),ex.what());
        }
        catch (boost::exception & exboost)
        {
            ICC_LOG_INFO(m_pLog, "boost exception : %s", m_strAgentIp.c_str());
        }
        catch (...)
        {
            ICC_LOG_INFO(m_pLog, "unknown exception. %s", m_strAgentIp.c_str());
        }
    }

    void CRTPPackProc::OnTimer(unsigned long ulTimerID)
    {
        if (ulTimerID == m_ulStorageTimerID)
        {
            std::vector<unsigned short> vecTimeOutPorts;
            GetTimeOutPort(m_llStorageTimeOut, vecTimeOutPorts);//获取超过2秒未接收数据的端口号
            for (std::vector<unsigned short>::const_iterator itr = vecTimeOutPorts.begin();itr < vecTimeOutPorts.end();itr++)
            {
                CMediaStoragePtr pMediaStorage = GetMediaStorage(*itr);
                if (NULL != pMediaStorage)
                {
                    RmvMediaStorage(*itr);
                    ICC_LOG_INFO(m_pLog, "StartMediaStorage by TimeOut : agentip:%s", m_strAgentIp.c_str());
                    pMediaStorage->StartProcStorage();
                }
            }
        }
    }

    CMediaStoragePtr CRTPPackProc::CreateMediaStorage(unsigned short usAgentPort)
    {
        lock_guard<mutex> autolk(m_mtMediaStorage);
        CMediaStoragePtr ptrMediaStorage = CMediaStoragePtr(new CMediaStorage(m_pLog, m_strAgentIp, usAgentPort, m_usVoiceInterval, m_uiSampleRate, m_usCodeType, m_usSampleBits));
        m_mapMediaStore.insert(std::make_pair(usAgentPort, ptrMediaStorage));
        CallInfoPtr pCallInfo = GetTmpCallInfo();
        if (NULL != pCallInfo)
        {
            ICC_LOG_DEBUG(m_pLog, "BindTmpCallInfo: AgentID(%s) CallRefID(%s)", pCallInfo->m_strAgentID.c_str(), pCallInfo->m_strCallID.c_str());
            ptrMediaStorage->BindCTICallInfo(*pCallInfo);
        }
        return ptrMediaStorage;
    }
    CMediaStoragePtr CRTPPackProc::GetMediaStorage(unsigned short usAgentPort)
    {
        lock_guard<mutex> autolk(m_mtMediaStorage);
        if (m_mapMediaStore.find(usAgentPort) != m_mapMediaStore.end())
        {
            return m_mapMediaStore[usAgentPort];
        }
        return CMediaStoragePtr((CMediaStorage*)NULL);
    }
    void CRTPPackProc::RmvMediaStorage(unsigned short usAgentPort)
    {
        lock_guard<mutex> autolk(m_mtMediaStorage);
        m_mapMediaStore.erase(usAgentPort);
    }
    CMediaStoragePtr CRTPPackProc::GetFirstMediaStorage()
    {
        lock_guard<mutex> autolk(m_mtMediaStorage);
        if (m_mapMediaStore.begin() != m_mapMediaStore.end())
        {
            CMediaStoragePtr ptrMediaStorage = m_mapMediaStore.begin()->second;
            m_mapMediaStore.erase(m_mapMediaStore.begin());
            return ptrMediaStorage;
        }
        return CMediaStoragePtr((CMediaStorage*)NULL);;
    }

    CMediaStoragePtr CRTPPackProc::GetMediaStorageByCallInfo(const CallInfo& callInfo)
    {
        std::string strAgentID = CAgentIPTable::GetInstance().QueryPhoneByIP(m_strAgentIp);
        if (callInfo.m_strAgentID == strAgentID)
        {
            lock_guard<mutex> autolk(m_mtMediaStorage);
            if (m_mapMediaStore.size() == 1) //仅有一个直接绑定
            {
                return m_mapMediaStore.begin()->second;
            }
            //通过时间匹配
            for (std::map<unsigned short, CMediaStoragePtr>::const_iterator iter = m_mapMediaStore.begin(); iter != m_mapMediaStore.end(); iter++)
            {
                if (iter->second->IsMatchStartTime(callInfo.m_u64CallStart))
                {
                    return iter->second;
                }
            }
        }
        //通过IP端口号匹配
        if (callInfo.m_strAgentIP == m_strAgentIp)
        {
            if (m_mapMediaStore.find(callInfo.m_usRtpPort) != m_mapMediaStore.end())
            {
                return m_mapMediaStore[callInfo.m_usRtpPort];
            }
        }
        return CMediaStoragePtr((CMediaStorage*)NULL);
    }
    inline void CRTPPackProc::SetPortLastReceiveTime(unsigned short usAgentPort)
    {
        m_usPortlastReceiveTime[usAgentPort] = std::chrono::system_clock::now();
    }
    void CRTPPackProc::GetTimeOutPort(long long llTimeOutSpan, std::vector<unsigned short>& vecTimeOutPorts)
    {
        std::chrono::system_clock::time_point tmNow = std::chrono::system_clock::now();
        std::map<unsigned short, std::chrono::system_clock::time_point>::const_iterator itr = m_usPortlastReceiveTime.begin();
        for (;itr != m_usPortlastReceiveTime.end();itr++)
        {
            if (std::chrono::duration_cast<std::chrono::milliseconds>(tmNow.time_since_epoch()).count() - std::chrono::duration_cast<std::chrono::milliseconds>(itr->second.time_since_epoch()).count() > llTimeOutSpan)
            {
                vecTimeOutPorts.push_back(itr->first);
            }
        }
    }
    inline void CRTPPackProc::RmvPortLastReceiveTime(unsigned short usAgentPort)
    {
        m_usPortlastReceiveTime.erase(usAgentPort);
    }


    void CRTPPackProc::AddTmpCallInfo(CallInfoPtr pCallInfo)
    {
        if (pCallInfo != NULL)
        {
            ICC_LOG_DEBUG(m_pLog, "AddTmpCallInfo: AgentID(%s) RefCallID(%s)", pCallInfo->m_strAgentID.c_str(), pCallInfo->m_strCallID.c_str());
            lock_guard<mutex> lk(m_tmpCallInfoMutex);
            m_pTmpCallInfo = pCallInfo;
        }
    }
    CallInfoPtr CRTPPackProc::GetTmpCallInfo()
    {
        lock_guard<mutex> lk(m_tmpCallInfoMutex);
        if (m_pTmpCallInfo != NULL)
        {
            CallInfoPtr pCallInfo = m_pTmpCallInfo;
            m_pTmpCallInfo = NULL ;
            return  pCallInfo;
        }
        return CallInfoPtr(NULL);
    }
    void CRTPPackProc::RmvTmpCallInfo()
    {
        lock_guard<mutex> lk(m_tmpCallInfoMutex);
        m_pTmpCallInfo=NULL;
    }

}


