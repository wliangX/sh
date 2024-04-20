#pragma once

#include <list>
#include <map>
#include <string>
#include <vector>

#include <IObject.h>
#include <AmqClient/IAsyncCallback.h>
#include <Json/IJsonFactory.h>
#include <Protocol/CHeader.h>

#define NOTIFI_SEND_REQUEST "notifi_send_request"

#define OBSERVER_CENTER_ALARM_TODAYALARM "CommandCenter.ICC.Server.Alarm.TodayAlarm"
#define OBSERVER_CENTER_ALARM_SYNTHETICAL "CommandCenter.ICC.Server.Alarm.Synthetical"
#define OBSERVER_CENTER_ALARM_TELHOTLINE "CommandCenter.ICC.Server.Alarm.TelHotline"

#define ALARM_OBSERVER_CENTER "CommandCenter.ICC.Server.Alarm"
#define BASEDATA_OBSERVER_CENTER "CommandCenter.ICC.Server.BaseData"
#define DBAGENT_OBSERVER_CENTER "CommandCenter.ICC.Server.DBAgent"
#define MSGCENTER_OBSERVER_CENTER "CommandCenter.ICC.Components.MessageCenter"

#define GATEWAY_SMP_OBSERVER_CENTER "CommandCenter.ICC.Gateway.SMP"

#define GATEWAY_IDENTIFYINFO_OBSERVER_CENTER "CommandCenter.ICC.Gateway.IdentityInfo"
#define GATEWAY_PERSONINFO_OBSERVER_CENTER "CommandCenter.ICC.Gateway.PersonInfo"
#define GATEWAY_PERSONINFODATA_OBSERVER_CENTER "CommandCenter.ICC.Gateway.PersonInfoData"

#define GATEWAY_FAST110_OBSERVER_CENTER "CommandCenter.ICC.Gateway.Fast110"
#define GATEWAY_POLICESTATIONALARM_OBSERVER_CENTER "CommandCenter.ICC.Gateway.PoliceStationAlarm"
#define GATEWAY_GPS_OBSERVER_CENTER "CommandCenter.ICC.Gateway.GPS"
#define GATEWAY_FOUNDER_OBSERVER_CENTER "CommandCenter.ICC.Gateway.Founder"

#define GATEWAY_IFDS_OBSERVER_CENTER "CommandCenter.ICC.Gateway.IFDS"
#define GATEWAY_LSP_OBSERVER_CENTER "CommandCenter.ICC.Gateway.LSP"
#define GATEWAY_MPA_OBSERVER_CENTER "CommandCenter.ICC.Gateway.MPA"
#define GATEWAY_MRPS_OBSERVER_CENTER "CommandCenter.ICC.Gateway.MRPS"
#define GATEWAY_SMSLJ_OBSERVER_CENTER "CommandCenter.ICC.Gateway.SmsLJ"

#define GATEWAY_SMT_OBSERVER_CENTER "CommandCenter.ICC.Gateway.SMT"
#define GATEWAY_THIRDALARM_OBSERVER_CENTER "CommandCenter.ICC.Gateway.ThirdAlarm"

#define GATEWAY_VCS_OBSERVER_CENTER "CommandCenter.ICC.Gateway.VCS"
#define GATEWAY_VCS_OBSERVER_CENTER_PROCESS_RECV_VCS_MESSAGE "CommandCenter.ICC.Gateway.VCS.ProcessRecvVcsMessage"

#define GATEWAY_WEB_OBSERVER_CENTER "CommandCenter.ICC.Gateway.Web"

#define GATEWAY_WECHATLJ_OBSERVER_CENTER "CommandCenter.ICC.Gateway.WeChatLJ"
#define GATEWAY_WECHATST_OBSERVER_CENTER "CommandCenter.ICC.Gateway.WeChatST"

#define OBSERVER_CENTER_DELIVBERYSERVER_EXTENSION  "CommandCenter.ICC.DiliveryServer.Extension"

#define BUSINESS_MESSAGE_HEALTHACTIVEMQ "business_message_healthactivemq"

namespace ICC
{
	namespace ObserverPattern
	{
		enum EBehavior
		{
			Received,
			Send
		};

		enum ERequestMode
		{
			Invalid,				//����Ч
			Non_Request_Respond,	//������Ӧ��ģʽ
			Request_Respond			//����Ӧ��ģʽ
		};

		enum ENotifyType
		{
			Post,
			Dispatch,
			PostSync
		};

		class INotification;
		typedef boost::shared_ptr<INotification> INotificationPtr;

		/*
		* class   ֪ͨ�ӿ���
		* author  w16314
		* purpose
		* note
		*/
		class INotification
		{
		public:
			//************************************
			// Method:    GetMessages
			// FullName:  ICC::ObserverPattern::INotification::GetMessages
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: ��ȡ��Ϣ�ַ���
			//************************************
			virtual std::string GetMessages() = 0;
						
			//************************************
			// Method:    Response
			// FullName:  ICC::ObserverPattern::INotification::Response
			// Access:    virtual public 
			// Returns:   void
			// Qualifier: ����Ӧ��ģʽͨ���ýӿڻظ�����
			// Parameter: std::string p_strResponse
			// Parameter: bool p_IsSubPackage �Ƿ�ְ���Ĭ�ϲ��ְ�
			//************************************
			virtual void Response(std::string p_strResponse, bool p_IsSubPackage = false) = 0;

			//************************************
			// Method:    GetBehavior
			// FullName:  ICC::ObserverPattern::INotification::GetBehavior
			// Access:    virtual public 
			// Returns:   ICC::ObserverPattern::EBehavior
			// Qualifier: ��ȡ������Ϊ�����ͣ�����
			//************************************
			virtual EBehavior GetBehavior() = 0;
			virtual void SetBehavior(EBehavior type) {};

			//************************************
			// Method:    GetRequestMode
			// FullName:  ICC::ObserverPattern::INotification::GetRequestMode
			// Access:    virtual public 
			// Returns:   ICC::ObserverPattern::ERequestMode
			// Qualifier: ��ȡ����ģʽ
			//************************************
			virtual ERequestMode GetRequestMode() = 0;

			//************************************
			// Method:    GetNotification
			// FullName:  ICC::ObserverPattern::INotification::GetNotification
			// Access:    virtual public 
			// Returns:   ICC::ObserverPattern::INotificationPtr
			// Qualifier: ��ȡ֪ͨʵ��
			//************************************
			virtual INotificationPtr GetNotification() = 0;

			//************************************
			// Method:    GetRequestGuid
			// FullName:  ICC::ObserverPattern::INotification::GetRequestGuids
			// Access:    virtual public 
			// Returns:   std::string
			// Qualifier: ��ȡ����Ӧ��ģʽ��,����ID
			//************************************
			virtual std::string GetRequestGuid() = 0;

			virtual void SetCmdName(const std::string& strCmdName) { m_strCmdName = strCmdName; }
			virtual std::string GetCmdName() { return m_strCmdName; }

			virtual void SetCmdGuid(const std::string& strCmdGuid) { m_strCmdGuid = strCmdGuid; }
			virtual std::string GetCmdGuid() { return m_strCmdGuid; }

		private:
			std::string m_strCmdName;
			std::string m_strCmdGuid;
		protected:
			EBehavior   m_behavior;
		};

		
		class IAbstractObserver;
		typedef boost::shared_ptr<IAbstractObserver> IAbstractObserverPtr;
		typedef std::list<IAbstractObserverPtr> IObserverList;
		typedef std::vector<IAbstractObserverPtr> IObserverListEx;
		/*
		* class   �۲��߳���ӿ�
		* author  w16314
		* purpose
		* note
		*/
		class IAbstractObserver
		{
		public:
			virtual bool Equals(IAbstractObserverPtr p_Observer) = 0;
			virtual void NotifyObserver(INotificationPtr p_NtfPtr) = 0;
			virtual std::string GetCmdName() = 0;
		};


		/*
		* class   �۲������Ľӿ���
		* author  w16314
		* purpose
		* note
		*/
		class IObserverCenter : public IObject
		{
		public:

			//************************************
			// Method:    Notify
			// FullName:  ICC::ObserverPattern::IObserverCenter::Notify
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			// Parameter: INotificationPtr p_NtfPtr
			// Parameter: ObserverPattern::ENotifyType p_eNotifyType ��������Post���첽���ã�dispatch:ͬ������
			//************************************
			virtual void Notify(INotificationPtr p_NtfPtr, ObserverPattern::ENotifyType p_eNotifyType = ObserverPattern::ENotifyType::Post) = 0;
			virtual void NotifyPrivate(INotificationPtr p_NtfPtr, bool bIsToMq = false, ObserverPattern::ENotifyType p_eNotifyType = ObserverPattern::ENotifyType::Post) = 0;

			//************************************
			// Method:    AddObserver
			// FullName:  ICC::IObserverCenter::AddObserver
			// Access:    virtual public 
			// Returns:   void
			// Qualifier: ע��۲���
			// Parameter: AbstractObserver & abstractObserver
			//************************************
			virtual void AddObserver(IAbstractObserverPtr p_AbstractObserver) = 0;

			//************************************
			// Method:    RemoveObserver
			// FullName:  ICC::ObserverPattern::IObserverCenter::RemoveObserver
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			// Parameter: IAbstractObserverPtr p_AbstractObserver
			//************************************
			virtual void RemoveObserver(IAbstractObserverPtr p_AbstractObserver) = 0;

			//************************************
			// Method:    GetObservers
			// FullName:  ICC::ObserverPattern::IObserverCenter::GetObservers
			// Access:    virtual public 
			// Returns:   IObserverList
			// Qualifier:
			// Parameter: 
			//************************************
			virtual IObserverList GetObservers() = 0;

			virtual bool GetObserversByCmdName(const std::string& strCmdName, IObserverListEx& lsObservers) = 0;
		};

		typedef boost::shared_ptr<IObserverCenter> IObserverCenterPtr;
		typedef std::list<IObserverCenterPtr> IObserverCenterList;
		typedef std::map<std::string, IObserverCenterPtr> ObserverCenterMap;
		typedef std::list<IAbstractObserverPtr> IObserverList;
		
		typedef std::map<std::string, IObserverListEx> IObserversMap;

		/*
		* class   �۲���
		* author  w16314
		* purpose
		* note
		*/
		template <class C>
		class Observer : public IAbstractObserver
		{
		public:
			typedef void (C::*Callback)(INotificationPtr);

			Observer(C* p_object, std::string p_CmdName, Callback p_method) :
				m_pObject(p_object),
				m_method(p_method),
				m_strCmdName(p_CmdName)
			{

			}

			~Observer()
			{

			}

			Observer(const Observer& p_observer)
			{
				m_pObject = p_observer.m_pObject;
				m_method = p_observer.m_method;
			}

			Observer& operator = (const Observer& p_observer)
			{
				if (&p_observer != this)
				{
					m_pObject = p_observer.m_pObject;
					m_method = p_observer.m_method;
				}
				return *this;
			}

			void NotifyObserver(INotificationPtr p_NtfPtr)
			{
				if (!m_pObject || !p_NtfPtr)
				{
					return;
				}								

				(m_pObject->*m_method)(p_NtfPtr);				
			}

			bool Equals(IAbstractObserverPtr p_abstractObserver)
			{
				const Observer* l_pObs = dynamic_cast<const Observer*>(p_abstractObserver.get());
				return l_pObs && l_pObs->m_pObject == m_pObject && l_pObs->m_method == m_method;
			}

			std::string GetCmdName()
			{
				return m_strCmdName;
			}

		private:
			Observer();
		private:
			C*       m_pObject;
			Callback m_method;
			std::string m_strCmdName;
		};
	}

	class CNotifiSendRequest : public ObserverPattern::INotification
	{
	public:
		CNotifiSendRequest(std::string p_str, ObserverPattern::ERequestMode p_eRequestMode = ObserverPattern::ERequestMode::Non_Request_Respond, ObserverPattern::INotificationPtr p_NtfPtr = nullptr)
			: m_str(p_str), 
			m_eRequestMode(p_eRequestMode), 
			m_pNtfPtr(p_NtfPtr)
		{
			m_behavior = ObserverPattern::EBehavior::Send;;
		}
		virtual std::string GetMessages()
		{
			return m_str;
		}
		virtual void Response(std::string p_strResponse, bool p_IsSubPackage = false)
		{

		}
		virtual ObserverPattern::EBehavior GetBehavior()
		{
			return m_behavior;
		}
		virtual void SetBehavior(ObserverPattern::EBehavior type)
		{
			m_behavior = type;
		}

		virtual ObserverPattern::ERequestMode GetRequestMode()
		{
			return m_eRequestMode;
		}
		virtual ObserverPattern::INotificationPtr GetNotification()
		{
			return m_pNtfPtr;
		}
		virtual std::string GetRequestGuid()
		{
			return "";
		}
	private:
		std::string m_str;
		ObserverPattern::ERequestMode m_eRequestMode;
		ObserverPattern::INotificationPtr m_pNtfPtr;
	};

	class CNotifiReceive : public ObserverPattern::INotification
	{
	public:
		CNotifiReceive(std::string p_str, std::string p_strGuid) : m_str(p_str)
		{
			SetCmdGuid(p_strGuid);
			m_behavior = ObserverPattern::EBehavior::Received;;
		}
		virtual std::string GetMessages()
		{
			return m_str;
		}
		virtual void Response(std::string p_strResponse, bool p_IsSubPackage = false)
		{

		}
		virtual void SetBehavior(ObserverPattern::EBehavior type)
		{
			m_behavior = type;
		}
		virtual ObserverPattern::EBehavior GetBehavior()
		{
			return m_behavior;
		}
		virtual ObserverPattern::ERequestMode GetRequestMode()
		{
			return ObserverPattern::ERequestMode::Invalid;
		}
		virtual ObserverPattern::INotificationPtr GetNotification()
		{
			return nullptr;
		}
		virtual std::string GetRequestGuid()
		{
			return "";
		}
	private:
		std::string m_str;
	};
}

#define ADDOBSERVER(observercenter,clstype,cmdname,memberfunc) \
observercenter->AddObserver(boost::make_shared<ObserverPattern::Observer<clstype>>(this, cmdname, &clstype::memberfunc)); \
m_AbstractObserverList.push_back(boost::make_shared<ObserverPattern::Observer<clstype>>(this, cmdname, &clstype::memberfunc));

#define MANUL_ADDOBSERVER(observercenter,clstype,cmdname,memberfunc) \
observercenter->AddObserver(boost::make_shared<ObserverPattern::Observer<clstype>>(this, cmdname, &clstype::memberfunc));

#define MANUL_REMOVEOBSERVER(observercenter,clstype,cmdname,memberfunc) \
observercenter->RemoveObserver(boost::make_shared<ObserverPattern::Observer<clstype>>(this, cmdname, &clstype::memberfunc));

