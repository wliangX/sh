#pragma once
#include <vector>

namespace ICC
{
	namespace PROTOCOL
	{
		struct tDept
		{
			std::string m_strGuid;			//单位guid
			std::string m_strParentGuid;	//上级单位guid
			std::string m_strCode;			//单位编码
			std::string m_strDistrictCode;	//单位行政区划编码
			std::string m_strType;			//单位类型
			std::string m_strName;			//单位名称
			std::string m_strPhone;			//单位座机
			std::string m_strShortcut;		//单位快捷查询缩写
			std::string m_strSort;			//单位序号，一般从1开始排序
		};

		struct tStaff
		{
			std::string m_strGuid;			//警员guid
			std::string m_strDeptGuid;		//警员所在单位guid
			std::string m_strCode;			//警员编码
			std::string m_strName;			//警员姓名
			std::string m_strSex;			//警员性别
			std::string m_strType;			//警员类型
			std::string m_strPosition;		//警员职务
			std::string m_strMobile;		//警员手机
			std::string m_strPhone;			//警员座机
			std::string m_strShortcut;		//警员快捷查询编写			
			std::string m_strSort;			//警员序号，一般从1开始排序
			std::string m_strLevel;			//层级
			//std::string m_strFlag;			//删除标记
			/*std::string m_strCreateUser;	//创建者
			std::string m_strCreateTime;	//创建时间
			std::string m_strUpdateUser;	//修改者
			std::string m_strUpdateTime;	//修改时间
			std::string m_strReserved1;
			std::string m_strReserved2;*/
		};

        struct tDict
        {
            std::string m_strGuid;
            std::string m_strParentGuid;
            std::string m_strCode;
            std::string m_strShortCut;
            std::string m_strSort;

            struct tValue
            {
                std::string m_strGuid;
                std::string m_strValue;
                std::string m_strLangGuid;
            };
            std::vector<tValue>m_vecDictValue;
        };

        struct tLanguage
        {
            std::string m_strGuid;
            std::string m_strCode;
            std::string m_strName;
            std::string m_strSort;
        };

		
		struct tFuncPrivilege
		{
			std::string m_strGuid;
			std::string m_strParentGuid;
			std::string m_strCode;
			std::string m_strName;
			std::string m_strSort;
		};

		struct tRole
		{
			std::string m_strGuid;
			std::string m_strParentGuid;
			std::string m_strCode;
			std::string m_strName;
			std::string m_strSort;
		};

		typedef struct _tagBindInfo
		{
			std::string m_strGuid;
			std::string m_strType;
			std::string m_strFromGuid;
			std::string m_strToGuid;
			std::string m_strFlag;
			std::string m_strLevel;
			std::string m_strSort;
			std::string m_strShortcut;
			std::string m_strCreateUser;
			std::string m_strCreateTime;
			std::string m_strUpdateUser;
			std::string m_strUpdatetime;
		}BindInfo, *PBindInfo;
	}
}