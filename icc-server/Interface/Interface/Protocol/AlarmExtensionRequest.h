#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"


namespace ICC
{
	namespace PROTOCOL
	{
		// 新增或修改接警预案
		class CNotifiAddOrUpdateAlarmPlanRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strcreateUserId = p_pJson->GetNodeValue("/createuserid", "");
				m_oBody.m_strcreateUserName = p_pJson->GetNodeValue("/createusername", "");
				m_oBody.m_strcreateOrgCode = p_pJson->GetNodeValue("/createorgcode", "");
				m_oBody.m_strcreateOrgName = p_pJson->GetNodeValue("/createorgname", "");
				m_oBody.m_strcreateTime = p_pJson->GetNodeValue("/createtime", "");
				m_oBody.m_strupdateUserId = p_pJson->GetNodeValue("/updateuserid", "");
				m_oBody.m_strupdateUserName = p_pJson->GetNodeValue("/updateusername", "");
				m_oBody.m_strupdateOrgCode = p_pJson->GetNodeValue("/updateorgcode", "");
				m_oBody.m_strupdateOrgName = p_pJson->GetNodeValue("/updateorgname", "");
				m_oBody.m_strupdatetime = p_pJson->GetNodeValue("/updatetime", "");
				m_oBody.m_strvalidity = p_pJson->GetNodeValue("/validity", "");

				m_oBody.m_strkeyWord = p_pJson->GetNodeValue("/baseInfo/keyword", "");
				m_oBody.m_strplantName = p_pJson->GetNodeValue("/baseInfo/plantname", "");
				m_oBody.m_strbaseId = p_pJson->GetNodeValue("/baseInfo/baseId", "");
				m_oBody.m_strenableCode = p_pJson->GetNodeValue("/baseInfo/enablecode", "");
				m_oBody.m_strenableName = p_pJson->GetNodeValue("/baseInfo/enablename", "");

				int uCount = p_pJson->GetCount("/baseInfo/alarmLabels");
				for (int i = 0; i < uCount; ++i)
				{
					std::string l_strPrefixPath("/baseInfo/alarmLabels/" + std::to_string(i));
					m_oBody.m_keyValuePair.code = p_pJson->GetNodeValue(l_strPrefixPath + "/name", "");
					m_oBody.m_keyValuePair.name = p_pJson->GetNodeValue(l_strPrefixPath + "/code", "");
					m_oBody.m_vecalarmLabels.push_back(m_oBody.m_keyValuePair);
				}

				uCount = p_pJson->GetCount("/baseInfo/alarmLevels");
				for (int i = 0; i < uCount; ++i)
				{
					std::string l_strPrefixPath("/baseInfo/alarmLevels/" + std::to_string(i));
					m_oBody.m_keyValuePair.code = p_pJson->GetNodeValue(l_strPrefixPath + "/name", "");
					m_oBody.m_keyValuePair.name = p_pJson->GetNodeValue(l_strPrefixPath + "/code", "");
					m_oBody.m_vecalarmLevels.push_back(m_oBody.m_keyValuePair);
				}

				uCount = p_pJson->GetCount("/baseInfo/alarmTypes");
				for (int i = 0; i < uCount; ++i)
				{
					std::string l_strPrefixPath("/baseInfo/alarmTypes/" + std::to_string(i));
					m_oBody.m_keyValuePair.code = p_pJson->GetNodeValue(l_strPrefixPath + "/name", "");
					m_oBody.m_keyValuePair.name = p_pJson->GetNodeValue(l_strPrefixPath + "/code", "");
					m_oBody.m_vecalarmTypes.push_back(m_oBody.m_keyValuePair);
				}

				uCount = p_pJson->GetCount("/details");
				for (int i = 0; i < uCount; ++i)
				{
					std::string l_strPrefixPath("/details/" + std::to_string(i));
					m_oBody.m_planDetail.m_strdetailId = p_pJson->GetNodeValue(l_strPrefixPath + "/detailId", "");
					m_oBody.m_planDetail.m_strtype = p_pJson->GetNodeValue(l_strPrefixPath + "/type", "");
					m_oBody.m_planDetail.m_strtitle = p_pJson->GetNodeValue(l_strPrefixPath + "/title", "");
					m_oBody.m_planDetail.m_strdata = p_pJson->GetNodeValue(l_strPrefixPath + "/data", "");
					m_oBody.m_planDetail.m_strinputData = p_pJson->GetNodeValue(l_strPrefixPath + "/inputData", "");
					m_oBody.m_planDetail.m_strdataType = p_pJson->GetNodeValue(l_strPrefixPath + "/dataType", "");
					m_oBody.m_planDetail.m_strviewType = p_pJson->GetNodeValue(l_strPrefixPath + "/viewType", "");
					m_oBody.m_planDetail.m_strdataValid = p_pJson->GetNodeValue(l_strPrefixPath + "/dataValid", "");
					m_oBody.m_planDetail.m_strdataView = p_pJson->GetNodeValue(l_strPrefixPath + "/dataView", "");
					m_oBody.m_planDetail.m_strconfigNotePre = p_pJson->GetNodeValue(l_strPrefixPath + "/configNotePre", "");
					m_oBody.m_planDetail.m_strconfigNoteSuf = p_pJson->GetNodeValue(l_strPrefixPath + "/configNoteSuf", "");
					m_oBody.m_planDetail.m_strsort = p_pJson->GetNodeValue(l_strPrefixPath + "/sort", "");

					m_oBody.m_vecdetails.push_back(m_oBody.m_planDetail);
				}

				return true;
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

				std::string m_strenableCode;                         // 状态代码：01-启用，02-不启用
				std::string m_strenableName;                         // 状态名称 预案基础信息
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

				std::string m_strkeyWord;                            // 关键字
				std::string m_strplantName;                          // 预案名称
				std::string m_strbaseId;                             // 基础信息ID 新增时为空，修改时候传入原详情id  
				std::vector<KeyValuePair>m_vecalarmTypes;            // 警情类型
				std::vector<KeyValuePair>m_vecalarmLevels;           // 警情级别
				std::vector<KeyValuePair>m_vecalarmLabels;           // 警情标签

				std::vector<PlanDetail>m_vecdetails;                 // 预案详情

				KeyValuePair m_keyValuePair;
				PlanDetail m_planDetail;
			};

			CBody m_oBody;
		};

		// 删除接警预案
		class CNotifiDeleteAlarmPlanRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				int uCount = p_pJson->GetCount("/baseIds");

				for (int i = 0; i < uCount; ++i)
				{
					std::string l_strPrefixPath("/baseIds/" + std::to_string(i));
					std::string baseId = p_pJson->GetNodeValue(l_strPrefixPath, "");
					m_oBody.m_vecbaseIds.push_back(baseId);
				}
				return true;
			}
		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::vector<std::string>m_vecbaseIds;     // 基础信息ID集合
			};

			CBody m_oBody;
		};

		//	启用-停用接警预案
		class CNotifiEnableAlarmPlanRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strenableCode = p_pJson->GetNodeValue("/enablecode", "");
				m_oBody.m_strenableName = p_pJson->GetNodeValue("/enablename", "");

				int uCount = p_pJson->GetCount("/baseIds");
				for (int i = 0; i < uCount; ++i)
				{
					std::string l_strPrefixPath("/baseIds/" + std::to_string(i));
					std::string baseId = p_pJson->GetNodeValue(l_strPrefixPath, "");
					m_oBody.m_vecbaseId.push_back(baseId);
				}
				return true;
			}
		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strenableCode;             // 是否启用代码：01-启用，02-停用
				std::string m_strenableName;             // 状态名称
				std::vector<std::string>m_vecbaseId;     // 基础信息ID集合
			};

			CBody m_oBody;
		};
		// 保存警情的预案信息
		class CNotifiSaveAlarmPlanRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strId = p_pJson->GetNodeValue("/id", "");

				m_oBody.m_strcreateUserId = p_pJson->GetNodeValue("/createuserid", "");
				m_oBody.m_strcreateUserName = p_pJson->GetNodeValue("/createusername", "");
				m_oBody.m_strcreateOrgCode = p_pJson->GetNodeValue("/createorgcode", "");
				m_oBody.m_strcreateOrgName = p_pJson->GetNodeValue("/createorgname", "");
				m_oBody.m_strcreateTime = p_pJson->GetNodeValue("/createtime", "");
				m_oBody.m_strupdateUserId = p_pJson->GetNodeValue("/updateuserid", "");
				m_oBody.m_strupdateUserName = p_pJson->GetNodeValue("/updateusername", "");
				m_oBody.m_strupdateOrgCode = p_pJson->GetNodeValue("/updateorgcode", "");
				m_oBody.m_strupdateOrgName = p_pJson->GetNodeValue("/updateorgname", "");
				m_oBody.m_strupdatetime = p_pJson->GetNodeValue("/updatetime", "");
				m_oBody.m_strvalidity = p_pJson->GetNodeValue("/validity", "");

				m_oBody.m_stralarmId = p_pJson->GetNodeValue("/alarmId", "");
				m_oBody.m_stralarmNum = p_pJson->GetNodeValue("/alarmNum", "");
				m_oBody.m_strkeyWord = p_pJson->GetNodeValue("/keyWord", "");
				m_oBody.m_strplanId = p_pJson->GetNodeValue("/planId", "");

				m_oBody.m_stralarmLevel = p_pJson->GetNodeValue("/alarmLevel", "");
				m_oBody.m_stralarmType = p_pJson->GetNodeValue("/alarmType", "");
				m_oBody.m_strplanData = p_pJson->GetNodeValue("/planData", "");

				return true;
			}
		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:

				std::string m_strId;                                 // id
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

				std::string m_stralarmId;                            // 警情id
				std::string m_stralarmNum;                           // 警情编号                 
				std::string m_strkeyWord;                            // 关键字
				std::string m_strplanId;                             // 预案id

				std::string m_stralarmLevel;                         // 警情级别(key-value形式json字符串)
				std::string m_stralarmType;                          // 警情类型(key-value形式json字符串)
				std::string m_strplanData;                           // 预案数据信息(包含模版信息,填写信息)
			}; 
			CBody m_oBody;
		};
		// 查询警情的预案信息
		class CNotifiSearchAlarmPlanRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_stralarmId = p_pJson->GetNodeValue("/alarmId", "");
				return true;
			}
		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_stralarmId;                           // 接警单号
			};

			CBody m_oBody;
		};

		// 查询常用预案
		class CNotifiSearchCommonUsePlanRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strorgCode = p_pJson->GetNodeValue("/orgCode", "");
				return true;
			}
		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strorgCode;                           // 单位代码
			};

			CBody m_oBody;
		};

		// 查询预案列表 
		class CNotifiSearchPlanListRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strenableCode = p_pJson->GetNodeValue("/enablecode", "");
				m_oBody.m_strkeyword = p_pJson->GetNodeValue("/keyword", "");
				m_oBody.m_strcurrent = p_pJson->GetNodeValue("/current", "");
				m_oBody.m_strsize = p_pJson->GetNodeValue("/size", "");

				int uCount = p_pJson->GetCount("/alarmLevelCodes");

				for (int i = 0; i < uCount; ++i)
				{
					std::string l_strPrefixPath("/alarmLevelCodes/" + std::to_string(i));
					std::string l_stralarmLevelCodes = p_pJson->GetNodeValue(l_strPrefixPath, "");
					m_oBody.m_vecalarmLevelCodes.push_back(l_stralarmLevelCodes);
				}

				uCount = p_pJson->GetCount("/alarmTypeCodes");
				for (int i = 0; i < uCount; ++i)
				{
					std::string l_strPrefixPath("/alarmTypeCodes/" + std::to_string(i));
					std::string l_stralarmTypeCodes = p_pJson->GetNodeValue(l_strPrefixPath, "");
					m_oBody.m_vecalarmTypeCodes.push_back(l_stralarmTypeCodes);
				}

				return true;
			}
		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strenableCode;                        // 是否启用代码：01-启用，02-停用
				std::string m_strkeyword;                           // 关键字
				std::string m_strcurrent;                           // 页码,默认为1,示例值(1)
				std::string m_strsize;                              // 每页条数,默认为20,示例值(20)
				std::vector<std::string>m_vecalarmLevelCodes;       // 警情级别代码集合
				std::vector<std::string>m_vecalarmTypeCodes;        // 警情类型代码集合
			};

			CBody m_oBody;
		};
		// 查询预案详情
		class CNotifiSearchPlanInfoRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strbaseId = p_pJson->GetNodeValue("/baseId", "");
				return true;
			}
		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strbaseId;                           // 基础信息ID
			};

			CBody m_oBody;
		};

	}

}

