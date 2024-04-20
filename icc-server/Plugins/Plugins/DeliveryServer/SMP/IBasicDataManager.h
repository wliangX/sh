#pragma once 

#include <string>
#include <vector>
#include <Json/IJsonFactory.h>

namespace ICC
{
	namespace Data
	{		
		class IBasicData
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson) = 0;
			virtual bool Parse(const std::string& strJson, JsonParser::IJsonPtr pJson) = 0;
			virtual std::string Guid() { return m_strGuid; }

		public:
			IBasicData(){}
			virtual ~IBasicData(){}

		public:
			std::string m_strGuid;
		};

		typedef boost::shared_ptr<IBasicData> IBasicDataPtr;


		class IBasicDataManager
		{
		public:		
			void AppendData(const std::vector<std::string>& vecDatas)
			{
				int iCount = vecDatas.size();
				for (int i = 0; i < iCount; i++)
				{
					m_vecInputDatas.push_back(vecDatas[i]);
				}
			}

			bool AnalyzeData()
			{
				std::map<std::string, IBasicDataPtr> mapSources;
				if (!_LoadDataFromDb(mapSources))
				{
					return false;
				}

				std::map<std::string, IBasicDataPtr> mapInputs;
				int iCount = m_vecInputDatas.size();
				for (int i = 0; i < iCount; i++)
				{
					if (!_ParseData(m_vecInputDatas[i], mapInputs))
					{
						return false;
					}
				}				

				std::vector<IBasicDataPtr> vecAddDatas;
				std::vector<IBasicDataPtr> vecUpdateDatas;
				std::vector<IBasicDataPtr> vecDeleteDatas;

				_CompareData(mapSources, mapInputs, vecAddDatas, vecUpdateDatas, vecDeleteDatas);

				if (!_AddData(vecAddDatas))
				{
					return false;
				}

				if (!_UpdateData(vecUpdateDatas))
				{
					return false;
				}

				if (!_DeleteData(vecDeleteDatas))
				{
					return false;
				}
				m_vecInputDatas.clear();
				return true;
			}

			virtual bool _ParseData(const std::string& strData, std::map<std::string, IBasicDataPtr>& mapInputs) = 0;
			virtual bool _AddData(const std::vector<IBasicDataPtr>& vecAdds) = 0;
			virtual bool _UpdateData(const std::vector<IBasicDataPtr>& vecUpdates) = 0;
			virtual bool _DeleteData(const std::vector<IBasicDataPtr>& vecDeletes) = 0;

		protected:

			void _CompareData(std::map<std::string, IBasicDataPtr>& mapSources, std::map<std::string, IBasicDataPtr>& mapInputs,
				std::vector<IBasicDataPtr>& vecAdds, std::vector<IBasicDataPtr>& vecUpdates, std::vector<IBasicDataPtr>& vecDeletes)
			{
				std::map<std::string, IBasicDataPtr>::iterator itrInput;
				for (itrInput = mapInputs.begin(); itrInput != mapInputs.end(); ++itrInput)
				{
					std::map<std::string, IBasicDataPtr>::iterator itrSource;
					itrSource = mapSources.find(itrInput->first);
					if (itrSource != mapSources.end())
					{
						if (itrInput->second->ToString(m_pJsonFty->CreateJson()) != itrSource->second->ToString(m_pJsonFty->CreateJson()))
						{
							vecUpdates.push_back(itrInput->second);
						}						
						mapSources.erase(itrSource);
					}
					else
					{
						vecAdds.push_back(itrInput->second);
					}
				}

				std::map<std::string, IBasicDataPtr>::iterator itrSource;
				for (itrSource = mapSources.begin(); itrSource != mapSources.end(); ++itrSource)
				{
					vecDeletes.push_back(itrSource->second);
				}
			}
				
			
			/*{
				IBasicDataPtr pData = _CreateBasicDataPtr();
				if (pData == nullptr)
				{
					return false;
				}
				
				if (!pData->Parse(strData, m_pJsonFty->CreateJson()))
				{
					return false;
				}

				mapInputs.insert(std::make_pair(pData->Guid(), pData));
			}*/

		public:
			IBasicDataManager(JsonParser::IJsonFactoryPtr pJsonFty) : m_pJsonFty(pJsonFty) { m_vecInputDatas.clear(); }
			virtual ~IBasicDataManager() {}

		protected:
			virtual IBasicDataPtr _CreateBasicDataPtr() = 0;
			virtual bool _LoadDataFromDb(std::map<std::string, IBasicDataPtr>& mapSources) = 0;					
			

		protected:
			std::vector<std::string> m_vecInputDatas;
			JsonParser::IJsonFactoryPtr m_pJsonFty;
		};

		typedef boost::shared_ptr<IBasicDataManager> IBasicDataManagerPtr;
	}
}
