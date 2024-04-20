#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		namespace Alarm
		{

			/************************************************************************/
			/*                    ������ָ����Ϣ                                     */
			/************************************************************************/
			class CProcessAppoint : public IRespond
			{
			public:

				virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
				{
					if (nullptr == p_pJson)
					{
						return "";
					}

					m_oHeader.SaveTo(p_pJson);
					p_pJson->SetNodeValue("/body/alarm_id", m_oBody.m_strAlarmID);
					p_pJson->SetNodeValue("/body/process_id", m_oBody.m_strProcessID);
					p_pJson->SetNodeValue("/body/seat_no", m_oBody.m_strSeatNo);

					return p_pJson->ToString();
				}

			public:
				class CBody
				{
				public:
					std::string m_strAlarmID;		//����ID
					std::string m_strProcessID;    //������
					std::string m_strSeatNo;	//������ϯλ

				};
				CHeader m_oHeader;
				CBody m_oBody;
			};

			

		}
	}
}
