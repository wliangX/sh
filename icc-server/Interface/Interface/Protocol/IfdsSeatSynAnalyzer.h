#ifndef __IfdsSeatSynAnalyzer_H__
#define __IfdsSeatSynAnalyzer_H__

#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>


namespace ICC
{
	namespace PROTOCOL
	{
		/*enum SyncType
		{
			ADD = 1,
			UPDATE,
			DELETE
		};*/

		class CIfdsSeatSynAnalyzer : public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strNo = p_pJson->GetNodeValue("/body/no", "");
				m_oBody.m_strName = p_pJson->GetNodeValue("/body/name", "");
				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");
				m_oBody.m_strDeptName = p_pJson->GetNodeValue("/body/dept_name", "");
				m_oBody.m_strSyncType = p_pJson->GetNodeValue("/body/sync_type", "");
				
				return true;
			}
			
			class CBody
			{
			public:
				std::string m_strNo;
				std::string m_strName;
				std::string m_strDeptCode;
				std::string m_strDeptName;
				std::string m_strSyncType;
			};

		public:
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}

}

#endif