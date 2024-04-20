#pragma once


#include <string>
#include <Json/IJson.h>
#include "CMetadata.h"

namespace ICC
{
	// nacos����ʱЯ����beat��Ϣ
	class CHeartBeatData
	{
	public:
		CHeartBeatData() = default;

		~CHeartBeatData() = default;

	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson)
		{
			if (NULL == p_pJson.get())
			{
				return "";
			}

			p_pJson->SetNodeValue("/cluster", m_strCluster);
			p_pJson->SetNodeValue("/ip", m_strIp);
			
			p_pJson->SetNodeValue("/metadata/preserved.heart.beat.timeout", m_oMetaData.m_strBeatTimeOut);
			p_pJson->SetNodeValue("/metadata/preserved.ip.delete.timeout", m_oMetaData.m_strDeleteTimeOut);
			p_pJson->SetNodeValue("/metadata/preserved.heart.beat.interval", m_oMetaData.m_strBeatInterval);

			p_pJson->SetNodeValue("/port", m_strPort);
			p_pJson->SetNodeValue("/scheduled", m_strScheduled);
			p_pJson->SetNodeValue("/serviceName", m_strServiceName);
			p_pJson->SetNodeValue("/weight", m_strWeight);
			p_pJson->SetNodeValue("/period", "5000");
			p_pJson->SetNodeValue("/stopped", "false");
			return p_pJson->ToString();
		}

	public:
		//��Ⱥ��
		std::string m_strCluster;

		//# �������IP
		std::string m_strIp;


		/*������string��ֵ�� �Ž�ȥ����ת������Զ˱���
		* [caused: errCode: 101, errMsg: Nacos deserialize for class [com.alibaba.nacos.naming.healthcheck.RsInfo] failed.  ;caused: Cannot construct instance of `java.util.LinkedHashMap` (although at least one Creator exists): no String-argument constructor/factory method to deserialize from String value ('{
    "preserved.heart.beat.timeout": "15000",
    "preserved.ip.delete.timeout": "129600000",
    "preserved.heart.beat.interval": "5000"
}')
 at [Source: (String)"{
    "cluster": "DefaultCluster",
    "ip": "192.168.69.53",
    "metadata": "{\n    \"preserved.heart.beat.timeout\": \"15000\",\n    \"preserved.ip.delete.timeout\": \"129600000\",\n    \"preserved.heart.beat.interval\": \"5000\"\n}",
    "port": "21010",
    "scheduled": "true",
    "serviceName": "global@@icc-incident-service",
    "weight": "1"
}"; line: 4, column: 17] (through reference chain: com.alibaba.nacos.naming.healthcheck.RsInfo["metadata"]);]
		*/
		//ý������
		CMetadata m_oMetaData;

		//������Ķ˿�
		std::string m_strPort;

		std::string m_strScheduled;

		//������
		std::string m_strServiceName;

		//Ȩ��
		std::string m_strWeight;

	};

}