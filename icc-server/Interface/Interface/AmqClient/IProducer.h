#pragma once
#include <string>
#include <map>
#include <IObject.h>

/************************************************************************/
/* C#��װ�Ľӿڣ�AMQͷ�����Զ�����������(�ŵ�ҵ��Э�����У�ҵ��ģ�鴦��)            */
/* �˴�����AMQ��ͷ�����ԣ�ҵ����������ɲ�������                                */
/************************************************************************/
typedef struct _tagCMSHeader 
{
	std::map<std::string, std::string> msProperties;		//��Ϣ���ԣ�����AMQ��Ϣ���ԣ���������Ϣ���˵ĳ���
	std::map<std::string, long long> mlProperties;			//��Ϣ���ԣ�����AMQ��Ϣ���ԣ���������Ϣ���˵ĳ���
	std::map<std::string, int> mnProperties;				//��Ϣ���ԣ�����AMQ��Ϣ���ԣ���������Ϣ���˵ĳ���
	std::map<std::string, float> mfProperties;				//��Ϣ���ԣ�����AMQ��Ϣ���ԣ���������Ϣ���˵ĳ���
	std::map<std::string, double> mdProperties;				//��Ϣ���ԣ�����AMQ��Ϣ���ԣ���������Ϣ���˵ĳ���
	std::map<std::string, unsigned char> mcProperties;		//��Ϣ���ԣ�����AMQ��Ϣ���ԣ���������Ϣ���˵ĳ���
	std::map<std::string, bool> mbProperties;				//��Ϣ���ԣ�����AMQ��Ϣ���ԣ���������Ϣ���˵ĳ���
	_tagCMSHeader()
	{
		msProperties.clear();
		mlProperties.clear();
		mnProperties.clear();
		mfProperties.clear();
		mdProperties.clear();
		mcProperties.clear();
		mbProperties.clear();
	}
}CMSHeader, *PCMSHeader;

namespace ICC 
{
	class IProducer :
		public IObject
	{
	public:
		/*
		* ��ʼ�����ӵ�activemq�������Ҵ���һ�����ӻỰ�̣߳���Ϣ���պ��Զ�ȷ��
		* @���� strBrokerURI
		*      ���ӵ�broker��URI.
		*      ��Ϣ���պ�ȷ��ģʽ:Ĭ���Զ�ȷ��AUTO_ACKNOWLEDGE
		* @���� strClientID
		*      �ͻ���Ψһ��ʶ.
		* @���� strUserName
		*      ����MQ���������֤���û���.
		* @���� strPassword
		*      ����MQ���������֤������.
		* @���� true:	�ɹ�
		*	   false:	ʧ�ܣ�ʧ��ԭ����ӡ��־
		*/
		virtual bool Connect(std::string strBrokerURI, std::string &strClientID, std::string strUserName = std::string(), std::string strPassword = std::string()) = 0;
		/*
		* ͬ��������Ϣ��������߶��У�������߶��в�����ʱ���Զ�����һ������/����
		* @���� strMsg
		*      ��Ϣ�壬Json/Xml��ʽ.
		* @���� strDest
		*      ������߶�������.
		* @���� iMode
		*      ��Ϣģʽ:0->����   1->����
		* @���� pHeader
		*      CMS��Ϣͷ��xml/Json��ʽ�ı�����������׼��Ϣͷ�������������ԣ������ȼ�������ʱ�䣬��Ϣѡ�����������Ե�
		*	   ��CMS��Ϣͷû������ʱ������Ҫ����
		* @���� true:	�ɹ�
		*	   false:	ʧ�ܣ�ʧ��ԭ����ӡ��־
		*/
		virtual bool Send(std::string strMsg, std::string strDest, int iMode, PCMSHeader pHeader = NULL) = 0;
		/*
		* �첽������Ϣ��������߶��У�������߶��в�����ʱ���Զ�����һ������/����
		* @���� strMsg
		*      ��Ϣ�壬Json/Xml��ʽ.
		* @���� strDest
		*      ������߶�������.
		* @���� p_iMode
		*      ��Ϣģʽ:0->����   1->����
		* @���� pHeader
		*      CMS��Ϣͷ��xml/Json��ʽ�ı�����������׼��Ϣͷ�������������ԣ������ȼ�������ʱ�䣬��Ϣѡ�����������Ե�
		*	   ��CMS��Ϣͷû������ʱ������Ҫ����
		* @���� true:	�ɹ�
		*	   false:	ʧ�ܣ�ʧ��ԭ����ӡ��־
		*/
		virtual bool Post(std::string strMsg, std::string strDest, int iMode, PCMSHeader pHeader = NULL) = 0;
		/*
		* ����Ӧ��ģʽ��������ע����ջص�������Ϣ
		* @���� strMsg
		*      ��Ϣ�壬Json/Xml��ʽ.
		* @���� strDest
		*      ��������.
		* @���� pResCallback
		*      Ӧ���������Ϣ����Ӧ��Ϣ�ص�
		* @���� pHeader
		*      CMS��Ϣͷ��xml/Json��ʽ�ı�����������׼��Ϣͷ�������������ԣ������ȼ�������ʱ�䣬��Ϣѡ�����������Ե�
		*	   ��CMS��Ϣͷû������ʱ������Ҫ����
		* @���� true:	�ɹ�
		*	   false:	ʧ�ܣ�ʧ��ԭ����ӡ��־
		*/
		virtual bool Request(std::string strMsg, std::string strDest, IResCallbackPtr pResCallback, PCMSHeader pHeader = NULL) = 0;
	};

	typedef boost::shared_ptr<IProducer> IProducerPtr;

} /*namespace ICC*/