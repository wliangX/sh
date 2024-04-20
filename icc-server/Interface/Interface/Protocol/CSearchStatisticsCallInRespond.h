#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchStatisticsCallInRespond :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strAllCount);
				
				unsigned int l_uiIndex = 0;
				for (const CData& data : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");

					p_pJson->SetNodeValue(l_strPrefixPath + "seat_no", data.m_strSeatNo);
					p_pJson->SetNodeValue(l_strPrefixPath + "called_count", data.m_strCalledCount);
					p_pJson->SetNodeValue(l_strPrefixPath + "count_110", data.m_strCount110);
					p_pJson->SetNodeValue(l_strPrefixPath + "count_119", data.m_strCount119);
					p_pJson->SetNodeValue(l_strPrefixPath + "count_122", data.m_strCount122);
					p_pJson->SetNodeValue(l_strPrefixPath + "ended_by_aller_count", data.m_strCountHangupByCaller);
					p_pJson->SetNodeValue(l_strPrefixPath + "ended_by_aller_count_called", data.m_strCountHangupByCalled);
					p_pJson->SetNodeValue(l_strPrefixPath + "blacked_count", data.m_strCountHangupByBlack);
					p_pJson->SetNodeValue(l_strPrefixPath + "timeout_count", data.m_strCountHangupByTimeout);
					p_pJson->SetNodeValue(l_strPrefixPath + "release_count", data.m_strCountHangupByRelease);
					p_pJson->SetNodeValue(l_strPrefixPath + "transferred_count", data.m_strCountTransfer);
					p_pJson->SetNodeValue(l_strPrefixPath + "talk_time", data.m_strTotalTalkTime);

					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

		public:
			class CData
			{
			public:
				std::string	m_strSeatNo;				    //��ϯ���
				std::string	m_strCalledCount;				//����������
				std::string m_strCount110;					//110����
				std::string m_strCount119;					//119����
				std::string m_strCount122;					//122����
				std::string m_strCountHangupByCaller;		//���йҶ�����
				std::string m_strCountHangupByCalled;		//���йҶ�����
				std::string m_strCountHangupByBlack;		//�������ܽ�����
				std::string m_strCountHangupByTimeout;		//��ʱ�Ҷ�����
				std::string m_strCountHangupByRelease;		//��������
				std::string m_strCountTransfer;				//����ת������			
				std::string m_strTotalTalkTime;			    //��ͨ��ʱ��
				
			};
			class CBody
			{
			public:
				std::string m_strCount;					//��ǰ����¼����
				std::string m_strAllCount;		        //����������¼������
				std::vector<CData> m_vecData;
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}