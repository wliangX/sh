#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
		class CNotifiAlarmPlanResponse :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				p_pJson->SetNodeValue("/code", m_oBody.m_strcode);
				p_pJson->SetNodeValue("/message", m_oBody.m_strmessage);
				if (!m_oBody.m_vecdata.empty())
				{
					for (size_t i = 0; i < m_oBody.m_vecdata.size(); i++)
					{
						std::string l_strPrefixPath("/data/" + std::to_string(i));
						p_pJson->SetNodeValue(l_strPrefixPath , m_oBody.m_vecdata.at(i));
					}
				}
				else
				{
					p_pJson->SetNodeValue("/data", m_oBody.m_strdata);
				}
				return p_pJson->ToString();
			}


		public:
			CHeaderEx m_oHeader;


			class CBody
			{
			public:
				std::string m_strcode;                         // 返回信息码,默认:200
				std::string m_strdata;                         // 返回的数据(Object)
				std::string m_strmessage;                      // 返回信息内容，默认:success
				std::vector<std::string> m_vecdata;
			};
			CBody m_oBody;
		};

		class CNotifiSaveAlarmPlanResponse :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				p_pJson->SetNodeValue("/code", m_oBody.m_strcode);
				p_pJson->SetNodeValue("/message", m_oBody.m_strmessage);

				p_pJson->SetNodeValue("/data/alarmId", m_oBody.m_stralarmId);
				p_pJson->SetNodeValue("/data/alarmLevel", m_oBody.m_stralarmLevel);
				p_pJson->SetNodeValue("/data/alarmNum", m_oBody.m_stralarmNum);
				p_pJson->SetNodeValue("/data/alarmType", m_oBody.m_stralarmType);
				p_pJson->SetNodeValue("/data/createOrgCode", m_oBody.m_strcreateOrgCode);
				p_pJson->SetNodeValue("/data/createOrgName", m_oBody.m_strcreateOrgName);
				p_pJson->SetNodeValue("/data/createTime", m_oBody.m_strcreateTime);
				p_pJson->SetNodeValue("/data/createUserId", m_oBody.m_strcreateUserId);
				p_pJson->SetNodeValue("/data/createUserName", m_oBody.m_strcreateUserName);
				p_pJson->SetNodeValue("/data/id", m_oBody.m_strId);

				p_pJson->SetNodeValue("/data/keyWord", m_oBody.m_strkeyWord);
				p_pJson->SetNodeValue("/data/planData", m_oBody.m_strplanData);
				p_pJson->SetNodeValue("/data/planId", m_oBody.m_strplanId);

				p_pJson->SetNodeValue("/data/updateOrgCode", m_oBody.m_strupdateOrgCode);
				p_pJson->SetNodeValue("/data/updateOrgName", m_oBody.m_strupdateOrgName);
				p_pJson->SetNodeValue("/data/updateTime", m_oBody.m_strupdatetime);

				p_pJson->SetNodeValue("/data/updateUserId", m_oBody.m_strupdateUserId);
				p_pJson->SetNodeValue("/data/updateUserName", m_oBody.m_strupdateUserName);
				p_pJson->SetNodeValue("/data/validity", m_oBody.m_strvalidity);

				return p_pJson->ToString();
			}


		public:
			CHeaderEx m_oHeader;


			class CBody
			{
			public:

				std::string m_strId;
				std::string m_strcreateUserId;                       // 创建人ID
				std::string m_strcreateUserName;                     // 创建人名称
				std::string m_strcreateOrgCode;                      // 创建单位代码
				std::string m_strcreateOrgName;                      // 创建单位名称
				std::string m_strcreateTime;                         // 创建时间
				std::string m_strupdateUserId;                       // 修改人ID
				std::string m_strupdateUserName;                     // 修改人名称
				std::string m_strupdateOrgCode;                      // 修改单位代码
				std::string m_strupdateOrgName;                      // 修改单位名称
				std::string m_strupdatetime;                         // 修改时间
				std::string m_strvalidity;                           // 是否有效

				std::string m_strplanData;                           // 预案数据信息(包含模版信息,填写信息)
				std::string m_stralarmNum;                           // 警情编号                 
				std::string m_strkeyWord;                            // 关键字
				std::string m_strplanId;                             // 预案id
				std::string m_stralarmType;                          // 警情类型
				std::string m_stralarmLevel;                         // 警情级别
				std::string m_strcode;                               // 返回信息码,默认:200
				std::string m_strmessage;                            // 返回信息内容，默认:success
				std::string m_stralarmId;                            //警情id
			
				std::vector<std::string> m_vecdata;
			};
			CBody m_oBody;
		};

		// 查询警情的预案信息
		class CNotifiSearchAlarmPlanResponse :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				p_pJson->SetNodeValue("/code", m_oBody.m_strcode);
				p_pJson->SetNodeValue("/message", m_oBody.m_strmessage);

				std::string l_strPrefixPath("/data/");
				p_pJson->SetNodeValue(l_strPrefixPath + "planId", m_oBody.m_strplanId);
				p_pJson->SetNodeValue(l_strPrefixPath + "alarmNum", m_oBody.m_stralarmNum);
				p_pJson->SetNodeValue(l_strPrefixPath + "keyWord", m_oBody.m_strkeyWord);
				p_pJson->SetNodeValue(l_strPrefixPath + "alarmId", m_oBody.m_stralarmId);

				p_pJson->SetNodeValue(l_strPrefixPath + "alarmLevel", m_oBody.m_stralarmLevel);
				p_pJson->SetNodeValue(l_strPrefixPath + "alarmType", m_oBody.m_stralarmType);
				p_pJson->SetNodeValue(l_strPrefixPath + "planData", m_oBody.m_strplanData);

				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;
	

			class CBody
			{
			public:
				std::string m_stralarmId;                       // 警情id
				std::string m_stralarmNum;                      // 警情编号
				std::string m_strplanId;                        // 预案id
				std::string m_strkeyWord;                       // 预案关键字
				std::string m_strplanName;                      // 预案名称

				std::string m_stralarmLevel;                    // 警情级别
				std::string m_stralarmType;                     // 警情类型
				std::string m_strplanData;                      // 预案填写信息

				std::string m_strcode;                          // 返回信息码,默认:200                       
				std::string m_strmessage;                       // 返回信息内容，默认:success 
	
			};
			CBody m_oBody;
		};

		// 查询常用预案
		class CNotifiSearchCommonUsePlanResponse :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				p_pJson->SetNodeValue("/code", m_oBody.m_strcode);
				p_pJson->SetNodeValue("/message", m_oBody.m_strmessage);

				for (size_t i = 0; i < m_oBody.m_vecdata.size(); ++i)
				{
					std::string l_strPrefixPath("/data/" + std::to_string(i));
					p_pJson->SetNodeValue(l_strPrefixPath + "/baseId", m_oBody.m_vecdata.at(i).m_strbaseId);
					p_pJson->SetNodeValue(l_strPrefixPath + "/enableCode", m_oBody.m_vecdata.at(i).m_strenableCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/enableName", m_oBody.m_vecdata.at(i).m_strenableName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/keyWord", m_oBody.m_vecdata.at(i).m_strkeyWord);
					p_pJson->SetNodeValue(l_strPrefixPath + "/planName", m_oBody.m_vecdata.at(i).m_strplanName);

					for (size_t j = 0; j < m_oBody.m_vecdata.at(i).m_vecalarmLevelNames.size(); ++j)
					{
						std::string l_stralarmLevelNamesPath(l_strPrefixPath + "/alarmLevelNames/" + std::to_string(j));
						p_pJson->SetNodeValue(l_stralarmLevelNamesPath, m_oBody.m_vecdata.at(i).m_vecalarmLevelNames[j]);
					}

					for (size_t j = 0; j < m_oBody.m_vecdata.at(i).m_vecalarmTipTitles.size(); ++j)
					{
						std::string l_stralarmTipTitlesPath(l_strPrefixPath + "/alarmTipTitles/" + std::to_string(j));
						p_pJson->SetNodeValue(l_stralarmTipTitlesPath, m_oBody.m_vecdata.at(i).m_vecalarmTipTitles[j]);
					}

					for (size_t j = 0; j < m_oBody.m_vecdata.at(i).m_vecalarmTypeNames.size(); ++j)
					{
						std::string l_stralarmTypeNamesPath(l_strPrefixPath + "/alarmTypeNames/" + std::to_string(j));
						p_pJson->SetNodeValue(l_stralarmTypeNamesPath, m_oBody.m_vecdata.at(i).m_vecalarmTypeNames[j]);
					}
				}

				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;

			struct data
			{
				std::vector<std::string> m_vecalarmLevelNames;  // 警情级别
				std::vector<std::string> m_vecalarmTipTitles;   // 警情提示语标题
				std::vector<std::string> m_vecalarmTypeNames;   // 警情类型名称
				std::string m_strbaseId;                        // 基础信息id
				std::string m_strenableCode;                    // 启用状态代码：01-启用，02-不启用
				std::string m_strenableName;                    // 状态名称
				std::string m_strkeyWord;                       // 预案关键字
				std::string m_strplanName;                      // 预案名称
			};

			class CBody
			{
			public:
				std::string m_strcode;                          // 返回信息码,默认:200                       
				std::string m_strmessage;                       // 返回信息内容，默认:操作成功 
				std::vector<data> m_vecdata;                   	// 返回的数据(Object)
				data m_data;                                    
			};
			CBody m_oBody;
		};

		// 查询预案列表
		class CNotifiSearchPlanListResponse :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				p_pJson->SetNodeValue("/code", m_oBody.m_strcode);
				p_pJson->SetNodeValue("/message", m_oBody.m_strmessage);

				p_pJson->SetNodeValue("/data/current", m_oBody.m_data.m_strcurrent);

				//p_pJson->SetNodeValue("/data/hitCount", m_oBody.m_data.m_strhitCount);
				p_pJson->SetNodeValue("/data/pages", m_oBody.m_data.m_strpages);
				//p_pJson->SetNodeValue("/data/searchCount", m_oBody.m_data.m_strsearchCount);
				p_pJson->SetNodeValue("/data/size", m_oBody.m_data.m_strsize);
				p_pJson->SetNodeValue("/data/total", m_oBody.m_data.m_strtotal);

				for (size_t i = 0; i < m_oBody.m_data.m_vecrecords.size(); ++i)
				{
					std::string l_strPrefixPath("/data/records/" + std::to_string(i));
					p_pJson->SetNodeValue(l_strPrefixPath + "/baseId", m_oBody.m_data.m_vecrecords.at(i).m_strbaseId);
					p_pJson->SetNodeValue(l_strPrefixPath + "/enableCode", m_oBody.m_data.m_vecrecords.at(i).m_strenableCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "/enableName", m_oBody.m_data.m_vecrecords.at(i).m_strenableName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/keyWord", m_oBody.m_data.m_vecrecords.at(i).m_strkeyWord);
					p_pJson->SetNodeValue(l_strPrefixPath + "/planName", m_oBody.m_data.m_vecrecords.at(i).m_strplanName);

					for (size_t j = 0; j < m_oBody.m_data.m_vecrecords.at(i).m_vecalarmLevelNames.size(); ++j)
					{
						std::string l_stralarmLevelNamesPath(l_strPrefixPath + "/alarmLevelNames/" + std::to_string(j));
						p_pJson->SetNodeValue(l_stralarmLevelNamesPath, m_oBody.m_data.m_vecrecords.at(i).m_vecalarmLevelNames[j]);
					}

					for (size_t j = 0; j < m_oBody.m_data.m_vecrecords.at(i).m_vecalarmTipTitles.size(); ++j)
					{
						std::string l_stralarmTipTitlesPath(l_strPrefixPath + "/alarmTipTitles/" + std::to_string(j));
						p_pJson->SetNodeValue(l_stralarmTipTitlesPath, m_oBody.m_data.m_vecrecords.at(i).m_vecalarmTipTitles[j]);
					}

					for (size_t j = 0; j < m_oBody.m_data.m_vecrecords.at(i).m_vecalarmTypeNames.size(); ++j)
					{
						std::string l_stralarmTypeNamesPath(l_strPrefixPath + "/alarmTypeNames/" + std::to_string(j));
						p_pJson->SetNodeValue(l_stralarmTypeNamesPath, m_oBody.m_data.m_vecrecords.at(i).m_vecalarmTypeNames[j]);
					}
				}

				return p_pJson->ToString();
			}


		public:
			CHeaderEx m_oHeader;

			struct records
			{
				std::vector<std::string> m_vecalarmLevelNames;  // 警情级别
				std::vector<std::string> m_vecalarmTipTitles;   // 警情提示语标题
				std::vector<std::string> m_vecalarmTypeNames;   // 警情类型名称
				std::string m_strbaseId;                        // 基础信息id
				std::string m_strenableCode;                    // 启用状态代码：01-启用，02-不启用
				std::string m_strenableName;                    // 状态名称
				std::string m_strkeyWord;                       // 预案关键字
				std::string m_strplanName;                      // 预案名称
			};

			struct data
			{
				std::string m_strcurrent;                       // 页码
				//bool m_strhitCount;                             //
				std::string m_strpages;                         // 页数
				//bool m_strsearchCount;                          //
				std::string m_strsize;                          // 条数
				std::string m_strtotal;                         // 总数
				std::vector<records> m_vecrecords;              //  预案详情展示信息
				struct records  m_records;                       
			};

			class CBody
			{
			public:
				std::string m_strcode;                          // 返回信息码,默认:200                       
				std::string m_strmessage;                       // 返回信息内容，默认:操作成功 
				std::vector<data> m_vecdata;                   	// 返回的数据(Object)
				data m_data;                                    
			};
			CBody m_oBody;
		};

		class CNotifiSearchPlanInfoResponse :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				p_pJson->SetNodeValue("/code", m_oBody.m_strcode);
				p_pJson->SetNodeValue("/message", m_oBody.m_strmessage);

				std::string l_strPrefixPath("/data/baseInfo/");
				p_pJson->SetNodeValue(l_strPrefixPath + "/baseId", m_oBody.m_strbaseId);
				p_pJson->SetNodeValue(l_strPrefixPath + "/keyWord", m_oBody.m_strkeyWord);
				p_pJson->SetNodeValue(l_strPrefixPath + "/plantName", m_oBody.m_strplantName);

				for (size_t i = 0; i < m_oBody.m_vecalarmTypes.size(); i++)
				{
					p_pJson->SetNodeValue(l_strPrefixPath + "/alarmTypes/code", m_oBody.m_vecalarmTypes.at(i).code);
					p_pJson->SetNodeValue(l_strPrefixPath + "/alarmTypes/name", m_oBody.m_vecalarmTypes.at(i).name);
				}

				for (size_t i = 0; i < m_oBody.m_vecalarmLevels.size(); i++)
				{
					p_pJson->SetNodeValue(l_strPrefixPath + "/alarmLevels/code", m_oBody.m_vecalarmLevels.at(i).code);
					p_pJson->SetNodeValue(l_strPrefixPath + "/alarmLevels/name", m_oBody.m_vecalarmLevels.at(i).name);
				}

				for (size_t i = 0; i < m_oBody.m_vecalarmLabels.size(); i++)
				{
					p_pJson->SetNodeValue(l_strPrefixPath + "/alarmLabels/code", m_oBody.m_vecalarmLabels.at(i).code);
					p_pJson->SetNodeValue(l_strPrefixPath + "/alarmLabels/name", m_oBody.m_vecalarmLabels.at(i).name);
				}

				for (size_t i = 0; i < m_oBody.m_vecdetails.size(); ++i)
				{
					std::string l_strDetailsPath("/data/details/" + std::to_string(i));
					p_pJson->SetNodeValue(l_strDetailsPath + "/detailId", m_oBody.m_vecdetails.at(i).m_strdetailId);
					p_pJson->SetNodeValue(l_strDetailsPath + "/type", m_oBody.m_vecdetails.at(i).m_strtype);

					p_pJson->SetNodeValue(l_strDetailsPath + "/configNotePre", m_oBody.m_vecdetails.at(i).m_strconfigNotePre);
					p_pJson->SetNodeValue(l_strDetailsPath + "/configNoteSuf", m_oBody.m_vecdetails.at(i).m_strconfigNoteSuf);

					p_pJson->SetNodeValue(l_strDetailsPath + "/data", m_oBody.m_vecdetails.at(i).m_strdata);
					p_pJson->SetNodeValue(l_strDetailsPath + "/dataType", m_oBody.m_vecdetails.at(i).m_strdataType);

					p_pJson->SetNodeValue(l_strDetailsPath + "/dataValid", m_oBody.m_vecdetails.at(i).m_strdataValid);
					p_pJson->SetNodeValue(l_strDetailsPath + "/dataView", m_oBody.m_vecdetails.at(i).m_strdataView);

					p_pJson->SetNodeValue(l_strDetailsPath + "/inputData", m_oBody.m_vecdetails.at(i).m_strinputData);
					p_pJson->SetNodeValue(l_strDetailsPath + "/sort", m_oBody.m_vecdetails.at(i).m_strsort);

					p_pJson->SetNodeValue(l_strDetailsPath + "/title", m_oBody.m_vecdetails.at(i).m_strtitle);
					p_pJson->SetNodeValue(l_strDetailsPath + "/viewType", m_oBody.m_vecdetails.at(i).m_strviewType);
				}

				return p_pJson->ToString();
			}


		public:
			CHeaderEx m_oHeader;



			struct KeyValuePair
			{
				std::string code;                                    // 代码
				std::string name;                                    // 名称
			};

			struct PlanDetail
			{
				std::string m_strdetailId;                           // 模块id，新增时为空，修改时候传入原详情id    
				std::string m_strtype;                               // 类型：01-警情提示语    
				std::string m_strtitle;                              // 标题
				std::string m_strdata;                               // 数据值
				std::string m_strinputData;                          // 填写的数据值(单选值/输入值/多选值)  
				std::string m_strdataType;                           // 数据类型：String,Array,Date,DateTime等 
				std::string m_strviewType;                           // 界面类型：input，select等，用于展示样式    
				std::string m_strdataValid;                          // 数据校验规则：必填，范围（包含范围值等）  
				std::string m_strdataView;                           // 数据展示信息：样式等
				std::string m_strconfigNotePre;                      // 配置语前缀
				std::string m_strconfigNoteSuf;                      // 配置语后缀
				std::string m_strsort;                               // 排序
			};
			class CBody
			{
			public:
				std::string m_strcode;                               // 返回信息码,默认:200                       
				std::string m_strmessage;                            // 返回信息内容，默认:操作成功 
				std::string m_strkeyWord;                            // 关键字
				std::string m_strplantName;                          // 预案名称
				std::string m_strbaseId;                             // 基础信息ID
				std::vector<KeyValuePair>m_vecalarmTypes;            // 警情类型
				std::vector<KeyValuePair>m_vecalarmLevels;           // 警情级别
				std::vector<KeyValuePair>m_vecalarmLabels;           // 警情标签

				std::vector<PlanDetail>m_vecdetails;                 // 预案详情

				KeyValuePair m_keyValuePair;
				PlanDetail m_planDetail;
			};

			CBody m_oBody;
		};
	}
}
