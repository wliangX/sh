#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CVoiceRecordQueryRespond :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				for (size_t i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strValueNum = std::to_string(i);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/callref_id", m_oBody.m_vecData.at(i).m_strCallref_Id);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/caller_id", m_oBody.m_vecData.at(i).m_strCaller_Id);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/called_id", m_oBody.m_vecData.at(i).m_strCalled_Id);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/record_file", m_oBody.m_vecData.at(i).m_strRecord_File);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/file_path", m_oBody.m_vecData.at(i).m_strFile_Path);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/begin_time", m_oBody.m_vecData.at(i).m_strBegin_Time);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/end_time", m_oBody.m_vecData.at(i).m_strEnd_Time);
					p_pJson->SetNodeValue("/body/value/" + l_strValueNum + "/duration", m_oBody.m_vecData.at(i).m_strDuration);
				}
				return p_pJson->ToString();
			}


		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strCount;
				class CData
				{
				public:
					std::string m_strCallref_Id;
					std::string m_strAcd;
					std::string m_strCaller_Id;
					std::string m_strCalled_Id;
					std::string m_strRecord_File;
					std::string m_strFile_Path;
					std::string m_strBegin_Time;
					std::string m_strEnd_Time;
					std::string m_strDuration;
				};
				std::vector<CData>m_vecData;
			};
			CBody m_oBody;
		};
	}
}
