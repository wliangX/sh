#pragma once

namespace ICC
{
	namespace DateTime
	{
		/*
		* class   ʱ��ʵ����
		* author  w16314
		* purpose
		* note
		*/
		class CDateTimeImpl : public IDateTime
		{
		public:
			CDateTimeImpl();
			virtual ~CDateTimeImpl();
		public:
			// ���ַ�����������ʱ��
			virtual CDateTime FromString(std::string p_strDateTime);
			// ������ʱ��ת��Ϊ�ַ���
			virtual std::string ToString(CDateTime p_DateTime, std::string p_strFormat = DATETIME_STRING_FORMAT_NO_MICROSECOND);

			virtual std::string ToStringEx(std::time_t p_DateTime, std::string p_strFormat = DATETIME_STRING_FORMAT_NO_MICROSECOND);

			virtual CDateTime FromTime_T(std::time_t p_DateTime);
			virtual std::time_t ToTime_T(CDateTime p_DateTime);

			virtual std::string FromUTCToLocal(const std::string& strUTCDateTime, int iDefaultOffset = 8, std::string p_strFormat = DATETIME_STRING_FORMAT_NO_MICROSECOND);
			virtual std::string FromLocalToUTC(const std::string& strLocalDateTime, int iDefaultOffset = -8, std::string p_strFormat = DATETIME_STRING_FORMAT_NO_MICROSECOND);

			//������ˮ��
			virtual std::string CreateSerial();
			virtual std::string CreateSerial2();
			virtual std::string CreateSerial(const int& iZoneSecond); //����ʹ��
			// ��ȡ��ǰʱ��
			virtual CDateTime CurrentDateTime();
			virtual CDateTime CurrentLocalDateTime();

			// ��ȡ��ǰʱ��
			virtual std::string CurrentDateTimeStr();
			virtual std::string CurrentLocalDateTimeStr();

			//�������
			virtual CDateTime AddSeconds(CDateTime p_DateTime, int p_iSeconds);
			//��ӷ�����
			virtual CDateTime AddMinutes(CDateTime p_DateTime, int p_iMinutes);
			//���Сʱ��
			virtual CDateTime AddHours(CDateTime p_DateTime, int p_iHours);
			//�������
			virtual CDateTime AddDays(CDateTime p_DateTime, int p_iDays);
			//�������
			virtual CDateTime AddMonths(CDateTime p_DateTime, int p_iMonths);
			//�������
			virtual CDateTime AddYears(CDateTime p_DateTime, int p_iYears);
			//����ʱ���룩
			virtual unsigned int SecondsDifference(CDateTime p_st, CDateTime p_et);
			//����ʱ���꣩
			virtual unsigned int DaysDifference(CDateTime p_st, CDateTime p_et);

			//��ȡָ��ʱ���ǰn��(Ĭ��һ��)
			virtual std::string GetFrontTime(std::string strTargetTime, long long l_secs = 86400);
			//��ȡָ��ʱ��ĺ�n��(Ĭ��һ��)
			virtual std::string GetAfterTime(std::string strTargetTime, long long l_secs = 86400);
			//��ȡ�Ӿ���IDʱ��
			virtual std::string GetAlarmIdTime(std::string strAlarmId);
			//��ȡ����IDʱ��
			virtual std::string GetCallRefIdTime(std::string strCallRefId);
			//��ȡ�ɾ���IDʱ��
			virtual std::string GetDispatchIdTime(std::string strDispatchId);

		private:
			static boost::mt19937 m_engine;//���������
			std::string _ConvertLocalUTC(const std::string& strDateTime, int iOffset, std::string p_strFormat = DATETIME_STRING_FORMAT_NO_MICROSECOND);
		};
	}
}