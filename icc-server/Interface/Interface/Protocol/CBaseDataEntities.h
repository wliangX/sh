#pragma once
#include <vector>

namespace ICC
{
	namespace PROTOCOL
	{
		struct tDept
		{
			std::string m_strGuid;			//��λguid
			std::string m_strParentGuid;	//�ϼ���λguid
			std::string m_strCode;			//��λ����
			std::string m_strDistrictCode;	//��λ������������
			std::string m_strType;			//��λ����
			std::string m_strName;			//��λ����
			std::string m_strPhone;			//��λ����
			std::string m_strShortcut;		//��λ��ݲ�ѯ��д
			std::string m_strSort;			//��λ��ţ�һ���1��ʼ����
		};

		struct tStaff
		{
			std::string m_strGuid;			//��Աguid
			std::string m_strDeptGuid;		//��Ա���ڵ�λguid
			std::string m_strCode;			//��Ա����
			std::string m_strName;			//��Ա����
			std::string m_strSex;			//��Ա�Ա�
			std::string m_strType;			//��Ա����
			std::string m_strPosition;		//��Աְ��
			std::string m_strMobile;		//��Ա�ֻ�
			std::string m_strPhone;			//��Ա����
			std::string m_strShortcut;		//��Ա��ݲ�ѯ��д			
			std::string m_strSort;			//��Ա��ţ�һ���1��ʼ����
			std::string m_strLevel;			//�㼶
			//std::string m_strFlag;			//ɾ�����
			/*std::string m_strCreateUser;	//������
			std::string m_strCreateTime;	//����ʱ��
			std::string m_strUpdateUser;	//�޸���
			std::string m_strUpdateTime;	//�޸�ʱ��
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