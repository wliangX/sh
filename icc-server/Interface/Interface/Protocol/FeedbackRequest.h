#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"
#include <vector>
#include <map>
#include <boost/algorithm/string.hpp>
namespace ICC
{
	namespace PROTOCOL
	{
        class CFeedBackRequest :  public IRequest, public IRespond
		{            

        public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
                {
                    return false;
                }

                for (unsigned int i = 0; i < m_oBody.m_vecNodeNames.size(); ++i)
                {            
                    std::string strName = m_oBody.m_vecNodeNames[i];
                    boost::algorithm::to_upper(strName);
                    std::string strPath = "/body/";
                    strPath += strName;
                    std::string strValue = p_pJson->GetNodeValue(strPath, "");
                    m_oBody.m_mapDatas.insert(std::make_pair(strName, strValue));
                }
            
                return true;
            }

            virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
            {
				if (nullptr == p_pJson)
				{
					return "";
				}

                m_oHeader.SaveTo(p_pJson);

                std::map<std::string, std::string>::const_iterator itr;
                for (itr = m_oBody.m_mapDatas.begin(); itr != m_oBody.m_mapDatas.end(); ++itr)
                {
                    std::string strTmpPath = "/body/";
                    p_pJson->SetNodeValue(strTmpPath + itr->first, itr->second);
                }                
                return p_pJson->ToString();
            }
		public:
			CHeader m_oHeader;
			class CBody
			{
            public:
                CBody()
                {
					m_vecNodeNames.push_back("msg_source");
					m_vecNodeNames.push_back("xzqhdm");
					m_vecNodeNames.push_back("fkdbh");
					m_vecNodeNames.push_back("jjdbh");
					m_vecNodeNames.push_back("pjdbh");
					m_vecNodeNames.push_back("fklyh");
					m_vecNodeNames.push_back("fkdwdm");
					m_vecNodeNames.push_back("fkybh");
					m_vecNodeNames.push_back("fkyxm");
					m_vecNodeNames.push_back("fksj");
					m_vecNodeNames.push_back("cjsj01");
					m_vecNodeNames.push_back("ddxcsj");
					m_vecNodeNames.push_back("xcclwbsj");
					m_vecNodeNames.push_back("jqlbdm");
					m_vecNodeNames.push_back("jqlxdm");
					m_vecNodeNames.push_back("jqxldm");
					m_vecNodeNames.push_back("jqzldm");
					m_vecNodeNames.push_back("jqfssj");
					m_vecNodeNames.push_back("bjdz");
					m_vecNodeNames.push_back("jqdz");
					m_vecNodeNames.push_back("fkdwxzb");
					m_vecNodeNames.push_back("fkdwyzb");
					m_vecNodeNames.push_back("cjczqk");
					m_vecNodeNames.push_back("cdcc");
					m_vecNodeNames.push_back("cdrc");
					m_vecNodeNames.push_back("cdct");
					m_vecNodeNames.push_back("jzrs");
					m_vecNodeNames.push_back("jzrssm");
					m_vecNodeNames.push_back("ssrs");
					m_vecNodeNames.push_back("ssrssm");
					m_vecNodeNames.push_back("swrs");
					m_vecNodeNames.push_back("swrssm");
					m_vecNodeNames.push_back("jqcljgdm");
					m_vecNodeNames.push_back("jqcljgsm");
					m_vecNodeNames.push_back("tqqkdm");
					m_vecNodeNames.push_back("ssqkms");
					m_vecNodeNames.push_back("zhrs");
					m_vecNodeNames.push_back("sars");
					m_vecNodeNames.push_back("tprs");
					m_vecNodeNames.push_back("jtsgxtdm");
					m_vecNodeNames.push_back("sfzzwxp");
					m_vecNodeNames.push_back("jtsgccyydm");
					m_vecNodeNames.push_back("njddm");
					m_vecNodeNames.push_back("lmzkdm");
					m_vecNodeNames.push_back("shjdcs");
					m_vecNodeNames.push_back("shfjdcs");
					m_vecNodeNames.push_back("dllxdm");
					m_vecNodeNames.push_back("jqclztdm");
					m_vecNodeNames.push_back("cjsj");
					m_vecNodeNames.push_back("gxsj");
					m_vecNodeNames.push_back("jdxz");
					m_vecNodeNames.push_back("sdcs");
					m_vecNodeNames.push_back("sfqz");
					m_vecNodeNames.push_back("yjdwdm");
					m_vecNodeNames.push_back("yjdwlxr");
					m_vecNodeNames.push_back("yjdwfkqk");
					m_vecNodeNames.push_back("yjdwfkqksm");
					m_vecNodeNames.push_back("ajbh");
					m_vecNodeNames.push_back("nrbq");
					m_vecNodeNames.push_back("jqbwdm");
					m_vecNodeNames.push_back("fklx");
					m_vecNodeNames.push_back("tbzt");
                }

			public:
				std::string m_strMsgSource;                         //��Ϣ��Դ
                std::string m_str_xzqhdm;                            //�������ڵ������������룬���Ӧ�Ӿ�����������
                std::string m_str_fkdbh;                             //��������ţ���������Ϊ����������������+ϵͳ��������ţ����ⲻͬ��������ͬ����ţ�Ψһ�ţ���������
                std::string m_str_jjdbh;                             //�����Ӿ������еĽӾ�������ֶ�
                std::string m_str_pjdbh;                             //�����ɾ������е��ɾ�������ֶ�
                std::string m_str_fklyh;                             //����¼���ţ���¼��ϵͳ�Զ��������Ӵ���ϵͳ�Զ�����
                std::string m_str_fkdwdm;                            //������λ��������
                std::string m_str_fkybh;                             //����Ա��ţ�������ʶ����Ա���
                std::string m_str_fkyxm;                             //������Ա����
                std::string m_str_fksj;                              //����Ա��д��ɷ�������ʱ�䣬��ʽ:YYYYMMDDHHMMSS(24Сʱ��)
                std::string m_str_cjsj01;                            //����ΪCJSJ�ʹ���ʱ���ͻ�ˣ����ƽ̨ʹ��CJSJ01
                std::string m_str_ddxcsj;                            //�����ֳ�ʱ�䣬��ʽ:YYYYMMDDHHMMSS(24Сʱ��)
                std::string m_str_xcclwbsj;                          //�������ʱ�䣬��ʽ:YYYYMMDDHHMMSS(24Сʱ��)
                std::string m_str_jqlbdm;                            //���������룬�ھ��鴦����Ϻ�Ӧ����ʵ�����¼��
                std::string m_str_jqlxdm;                            //�������ʹ��룬�ھ��鴦����Ϻ�Ӧ����ʵ�����¼��
                std::string m_str_jqxldm;                            //����ϸ����룬�ھ��鴦����Ϻ�Ӧ����ʵ�����¼�롣8.5������������͡�ϸ�ࡢ������������ϸ��Ĳ���
                std::string m_str_jqzldm;                            //����������룬�ھ��鴦����Ϻ�Ӧ����ʵ�����¼�롣8.5������������͡�ϸ�ࡢ����������������Ĳ���
                std::string m_str_jqfssj;                            //���鷢��ʱ�䣬��ʽ:YYYYMMDDHHMMSS(24Сʱ��)
                std::string m_str_bjdz;                              //���������ڵ�ַ����λ����
                std::string m_str_jqdz;                              //���鷢���ľ����ַ
                std::string m_str_fkdwxzb;                           //�����ֳ����ڵص�ľ���
                std::string m_str_fkdwyzb;                           //�����ֳ����ڵص��γ��
                std::string m_str_cjczqk;                            //��¼���������鴦�ù���
                std::string m_str_cdcc;                              //����������(����)
                std::string m_str_cdrc;                              //������Ա��(�˴�)
                std::string m_str_cdct;                              //��¼������ͧ����ϸ��Ϣ
                std::string m_str_jzrs;                              //������Ա����(��)
                std::string m_str_jzrssm;                            //������������ϸ˵��
                std::string m_str_ssrs;                              //������Ա����(��)
                std::string m_str_ssrssm;                            //����������ϸ˵��
                std::string m_str_swrs;                              //������Ա����(��)
                std::string m_str_swrssm;                            //����������ϸ˵��
                std::string m_str_jqcljgdm;                          //����Ĵ���������
                std::string m_str_jqcljgsm;                          //�Ծ���Ҫ�ؼ��ֳ�������������������
                std::string m_str_tqqkdm;                            //�����������
                std::string m_str_ssqkms;                            //����ɵ���ʧ�����ϸ����
                std::string m_str_zhrs;                              //�ֳ�ץ��������������(��)
                std::string m_str_sars;                              //�永��Ա����(��)
                std::string m_str_tprs;                              //������Ա����(��)
                std::string m_str_jtsgxtdm;                          //��ͨ�¹���̬����
                std::string m_str_sfzzwxp;                           //0����1���ǣ�Ĭ��Ϊ0
                std::string m_str_jtsgccyydm;                        //���������Ľ�ͨ�¹�ԭ�����
                std::string m_str_njddm;                             //�¹ʷ���ʱ���ܼ���
                std::string m_str_lmzkdm;                            //�¹ʷ���·�ε�·��״��
                std::string m_str_shjdcs;                            //�¹���ɻ�����������
                std::string m_str_shfjdcs;                           //�¹���ɷǻ�����������
                std::string m_str_dllxdm;                            //�¹ʷ���·�εĵ�·����
                std::string m_str_jqclztdm;                          //��¼��������Ŀǰ��ҵ��״̬
                std::string m_str_cjsj;                              //��¼������ʱ�䣬��ʽ:YYYYMMDDHHMMSS(24Сʱ��)
                std::string m_str_gxsj;                              //��¼�����������ʱ�䣬��ʽ:YYYYMMDDHHMMSS(24Сʱ��)
                std::string m_str_jdxz;                              //���鷢�����ڵ�����ֵ�
                std::string m_str_sdcs;                              //���鷢�����ڵĴ���
                std::string m_str_sfqz;                              //0����1���ǣ�Ĭ��Ϊ0
                std::string m_str_yjdwdm;                            //�����ƽ��н��յ�λ�Ĵ���
                std::string m_str_yjdwlxr;                           //�����ƽ���Ľ��յ�λ��ϵ������
                std::string m_str_yjdwfkqk;                          //�ƽ����ƽ���λ���淴������
                std::string m_str_yjdwfkqksm;                        //�����ƽ���Ľ��յ�λ���ý��������
                std::string m_str_ajbh;                              //ִ���참ϵͳ���Զ����ɵİ������
                std::string m_str_nrbq;                              //�����ǩ�ǶԾ������ݹؼ�Ҫ�ص���ȡ�ͱ�ע
                std::string m_str_jqbwdm;                            //���鷢�����岿λ�Ĵ���
                std::string m_str_fklx;                              //10������30��������
                std::string m_str_tbzt;                              //0��ʾ��ͬ����1��ʾ����ϵͳ��Ҫͬ����ͬ�������Ϊ0��2��ʾ�Ӿ�ϵͳ��Ҫͬ����ͬ�������Ϊ0    

                std::map<std::string, std::string> m_mapDatas;
                std::vector<std::string> m_vecNodeNames;
			};
			CBody m_oBody;
		};
	}
}
