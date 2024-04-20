#pragma once
#include <map>
#include <PGClient/IResultSet.h>

namespace ICC
{	
	namespace DataBase
	{
		class CResultSetImpl :
			public IResultSet
		{
		public:
			typedef boost::shared_ptr<PGresult> PGResultPtr;

		public:
			CResultSetImpl(PGResultPtr p_pPGResult = nullptr);
			~CResultSetImpl();

			//������Ƿ���Ч����Чtrue,��Чfalse
			virtual bool IsValid();
			//�޿��ý��false,�п��ý��true
			virtual bool Next();
			//���ؽ�����е�����
			virtual unsigned int FieldSize();
			//���ؽ�����еļ�¼����������
			virtual unsigned int RecordSize();
			//��ȡ��ǰ�е�����ֵ
			virtual unsigned int GetCurRow();
			//���õ�ǰ���������
			virtual bool SetCurRow(unsigned int p_uiCurRow);
			//��ȡ���е�����
			virtual std::vector<std::string> GetFieldNames();
			//��ȡ��iRow�У�iCol�еĽ��,���������Դ�0��ʼ
			virtual std::string GetValue(unsigned int p_uiRow, unsigned int p_uiColm, const std::string p_strDefault = "");
			//ȡ��ǰ���ֶ���Ϊp_strColName��ֵ
			virtual std::string GetValue(const std::string p_strColName, const std::string p_strDefault = "");
			//��ȡ��iRow��(��һ��Ϊ����Ϊ0)���ֶ���ΪFieldName������
			virtual std::string GetValue(unsigned int p_uiRow, const std::string p_strColName, const std::string p_strDefault = "default");
			//���ô���������Ϣ
			virtual void SetErrorMsg(std::string p_strErrorMsg);
			//���ش���������Ϣ
			virtual std::string GetErrorMsg();			
			//�����ڲ�ִ�е�SQL
			virtual std::string GetSQL();

		public:			
			void SetSQL(std::string p_strSQL);	//�����ڲ�ִ�е�SQL
			void SetResultStatus(bool p_bIsValid);
			void SetRecordSize(unsigned int p_uiSize);			
			void AddFieldIndex(std::string p_strFieldName, unsigned int p_uiFieldIndex);

		private:
			bool m_bValid;					//��ѯ�ɹ���ʧ�ܣ��ɹ�true,ʧ��false
			int	 m_uiCurRow;				//������ĵ�ǰ��
			unsigned int m_uiRecordSize;	//������еļ�¼����������
			std::string m_strErrorMsg;		//������Ϣ
			std::string m_strSQL;			//ִ�е�SQL
			PGResultPtr m_pResult;			//Postgresql��libpq�ⷵ�صĲ�ѯ���ָ��
			std::vector<std::string> m_vecFieldNames;			// ������е��������Բ�ѯ����е�˳������
			std::map<std::string, unsigned int>	m_mapFieldIndex; // field_name -- column index
			
		};
	}
}