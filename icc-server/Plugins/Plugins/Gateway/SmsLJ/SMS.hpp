#pragma once
#include "Boost.h"

//////////////////////////////////////////////////////////////////////////
#define		CMD_LOGIN								0x0001
#define		CMD_LOGIN_RESP							0xF0001
#define		CMD_SENDMSG								0x0002
#define		CMD_SENDMSG_RESP						0xF0002
#define		CMD_SENDTEXT							0x0003
#define		CMD_SENDTEXT_RESP						0xF0003
#define		CMD_REDISPATCH							0x0004
#define		CMD_REDISPATCH_RESP						0xF0004
#define		CMD_GETLOCATION							0x0005
#define		CMD_GETLOCATION_RESP					0xF0005
#define		CMD_SETREADED							0x0007
#define		CMD_SETREADED_RESP						0xF0007
#define		CMD_CHECKALIVE							0x0008
#define		CMD_CHECKALIVE_RESP						0xF0008
#define		CMD_LOGOUT								0x0009
#define		CMD_LOGOUT_RESP							0xF0009
#define		CMD_REDISPATCHARRIVAL					0x0100
#define		CMD_REDISPATCHARRIVAL_RESP				0xF0100
#define		CMD_TEXTARRIVAL							0x0200
#define		CMD_TEXTARRIVAL_RESP					0xF0200
#define		CMD_SMSARRIVAL							0x0300
#define		CMD_SMSARRIVAL_RESP						0xF0300
#define		CMD_USERSTATUSARRIVAL					0x0400
#define		CMD_USERSTATUSARRIVAL_RESP				0xF0400
#define		CMD_SERVERSTATUSARRIVAL					0x0500
#define		CMD_SERVERSTATUSARRIVAL_RESP			0xF0500

//////////////////////////////////////////////////////////////////////////
// ���Ÿ�����
class SmsUtil
{
public:
	static std::string  toString(const char * pszSrc, int nLen)
	{
		return std::string(pszSrc, nLen);
		std::string str;
		for (int i = 0; i < nLen; i++)
		{
			try
			{
				boost::format fmt("%c");
				fmt%pszSrc[i];
				str.append(fmt.str());
			}			
			catch (...)
			{
			}
			
		}
		// ��Ϊ���ܰ������֣���Ҫת��ΪGBK����
		std::string strUTF = boost::locale::conv::from_utf<char>(str,"GBK");
		
		return strUTF;
	}

	static std::string  FormatBuffer(const char* p, int n)
	{
		std::string  str;
		try
		{

			boost::format fmt("len:%d=>");
			fmt%n;
			str.append(fmt.str());
		}
		catch (...)
		{
		}

		for (int i = 0; i < n; i++)
		{
			try
			{
				char tmp[20] = { 0 };
				boost::log::aux::snprintf(tmp, 20, " 0x%02X", (unsigned char)p[i]);
				str.append(tmp);
			}			
			catch (...)
			{
			}
			
		}
		return str;
	}

	static int NextSeqID4Icc()
	{ // Topic��������������ŷ��������͵�SeqID
		static int s_iSeqID = 10000;
		if (s_iSeqID >= 20000)
		{
			s_iSeqID = 10000;
		}
		return s_iSeqID++;
	}

	static int NextSeqID4Sms()
	{ // ���ŷ������ص��������͵�SeqID
		static int s_iSeqID = 30000;
		if (s_iSeqID >= 40000)
		{
			s_iSeqID = 30000;
		}
		return s_iSeqID++;
	}

	static int NextSeqID4Heart()
	{ // ����SeqID
		static int s_iSeqID = 50000;
		if (s_iSeqID >= 60000)
		{
			s_iSeqID = 50000;
		}
		return s_iSeqID++;
	}

	static int MIN(int p_iNumA, int p_iNumB)
	{
		return (p_iNumA < p_iNumB ? p_iNumA : p_iNumB);
	}
};

#define SMS_COPY_INT(p, i) { i = boost::asio::detail::socket_ops::host_to_network_long(i); memcpy(p, &i, sizeof(i)); p += sizeof(i); i = boost::asio::detail::socket_ops::network_to_host_long(i); }
#define SMS_COPY_DOUBLE(p, d) { memcpy(p, &d, sizeof(d)); p += sizeof(d); }
#define SMS_COPY_CHAR(p, c) { *(char*)p = c; p += sizeof(c); }
#define SMS_COPY_CHAR_ARRAY(p, ca) { memcpy(p, ca, sizeof(ca)); p += sizeof(ca); }
#define SMS_COPY_CHAR_POINTER(p, cp, len) { memcpy(p, cp, len); p += len; }

#define SMS_RCOPY_INT(p, i) { memcpy(&i, p, sizeof(i)); p += sizeof(i); i = boost::asio::detail::socket_ops::network_to_host_long(i); }
#define SMS_RCOPY_DOUBLE(p, d) { memcpy(&d, p, sizeof(d)); p += sizeof(d); }
#define SMS_RCOPY_CHAR(p, c) { c = *(char*)p; p += sizeof(c); }
#define SMS_RCOPY_CHAR_ARRAY(p, ca) { memcpy(ca, p, sizeof(ca)); p += sizeof(ca); }
#define SMS_RCOPY_CHAR_POINTER(p, cp, len) { memcpy(cp, p, len); p += len; }

//////////////////////////////////////////////////////////////////////////
// ���Ż���
class SmsBase
{
public:
	SmsBase()
	{
		m_pBuffer = NULL;
	}
	virtual ~SmsBase()
	{
		ReleaseBuffer();
	}

public:
	virtual int GetLength() = 0;
	virtual const char* GetBuffer() = 0;
	virtual bool SetBuffer(const char* p_pBuffer, int p_nLen) = 0;

protected:
	virtual void ReleaseBuffer()
	{
		if (m_pBuffer)
		{
			delete [] m_pBuffer;
			m_pBuffer = NULL;
		}
	}

protected:
	char* m_pBuffer;
};

//////////////////////////////////////////////////////////////////////////
// ���Ż�����
class SmsBuffer
{
public:
	SmsBuffer()
	{
		m_pBuffer = new char[1024];
		m_nBufferUse = 0;
		m_nBufferLength = 1024;
	}
	~SmsBuffer()
	{
		if (m_pBuffer)
		{
			delete [] m_pBuffer;
			m_pBuffer = NULL;
		}
		m_nBufferUse = 0;
		m_nBufferLength = 0;
	}

public:
	int GetLength()
	{
		return m_nBufferUse;
	}
	const char* GetBuffer()
	{
		return m_pBuffer;
	}

public:
	void Append(const char* buf, int nLen)
	{
		if (nullptr == buf)
		{
			return;
		}

		if (m_nBufferLength - m_nBufferUse < nLen)
		{ // �ڴ治�㣬��������ռ�
			int l_nNewBufferLength = m_nBufferLength * 2;
			while (1)
			{
				if (l_nNewBufferLength >= m_nBufferUse + nLen)
				{
					break;
				}
				l_nNewBufferLength *= 2;
			}

			char* l_pBuffer = new char[l_nNewBufferLength];
			memcpy(l_pBuffer, m_pBuffer, m_nBufferUse);

			delete m_pBuffer;
			m_pBuffer = l_pBuffer;
			// m_nBufferUseʹ�ÿռ䲢û�б仯
			m_nBufferLength = l_nNewBufferLength;
		}

		memcpy(m_pBuffer + m_nBufferUse, buf, nLen);
		m_nBufferUse += nLen;
	}

	void OffsetLeft(int nLen)
	{ // ����nLen���ֽ�
		memcpy(m_pBuffer, (const char*)((char*)m_pBuffer + nLen), m_nBufferUse - nLen);
		m_nBufferUse -= nLen;
	}

public:
	void Append(SmsBase* p_SmsBase)
	{
		Append(p_SmsBase->GetBuffer(), p_SmsBase->GetLength());
	}

public:
	char* m_pBuffer;
	int m_nBufferUse;
	int m_nBufferLength;
};

//////////////////////////////////////////////////////////////////////////
// ��Ϣͷ
class SmsHeader : public SmsBase
{
public:
	SmsHeader()
	{
		m_nTotalLength = 0;
		m_nSequenceID = 0;
		m_nCommandID = 0;
	}
	~SmsHeader()
	{
	}

public:
	int m_nTotalLength;
	int m_nSequenceID;
	int m_nCommandID;

public:
	virtual int GetLength()
	{
		return sizeof(m_nTotalLength)
			+ sizeof(m_nSequenceID)
			+ sizeof(m_nCommandID);
	}

	virtual const char* GetBuffer()
	{
		if (NULL == m_pBuffer)
		{
			m_pBuffer = new char[GetLength()];

			char* l_pBuffer = m_pBuffer;
			SMS_COPY_INT(l_pBuffer, m_nTotalLength);
			SMS_COPY_INT(l_pBuffer, m_nSequenceID);
			SMS_COPY_INT(l_pBuffer, m_nCommandID);
		}
		return m_pBuffer;
	}

	virtual bool SetBuffer(const char* p_pBuffer, int p_nLen)
	{
		if (p_nLen < GetLength())
		{
			return false;
		}

		ReleaseBuffer();

		char* l_pBuffer = (char*)p_pBuffer;
		SMS_RCOPY_INT(l_pBuffer, m_nTotalLength);
		SMS_RCOPY_INT(l_pBuffer, m_nSequenceID);
		SMS_RCOPY_INT(l_pBuffer, m_nCommandID);

		return true;
	}

public:
	void SetInfo(int p_nTotalLength, int p_nSequenceID, int p_nCommandID)
	{
		ReleaseBuffer();

		m_nTotalLength = p_nTotalLength;
		m_nSequenceID = p_nSequenceID;
		m_nCommandID = p_nCommandID;
	}

	std::string  GetInfo()
	{
		char tmp[1024] = { 0 };
		boost::log::aux::snprintf(tmp, 1024, "SmsHeader: m_nTotalLength[%d], m_nSequenceID[%d], m_nCommandID[0x%04X]",
			m_nTotalLength, m_nSequenceID, (unsigned int)m_nCommandID);
		return tmp;
	}
};

//////////////////////////////////////////////////////////////////////////
// ��Ϣ��
// SmsBody
// SmsBodyLogin
// SmsBodySendMsg
// SmsMsgBodySetReaded
// SmsMsgBodyGetLocation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ��Ϣ��
class SmsBody : public SmsBase
{
public:
	SmsBody()
	{
	}
	virtual ~SmsBody()
	{
	}
};

//////////////////////////////////////////////////////////////////////////
// ��¼
class SmsBodyLogin : public SmsBody
{
public:
	SmsBodyLogin()
	{
		memset(m_user, 0, 16);
		memset(m_password, 0, 16);
	}
	~SmsBodyLogin()
	{
	}

public:
	char m_user[16]; // 16�ֽڣ���½�û���
	char m_password[16]; // 16�ֽڣ��û�����

public:
	virtual int GetLength()
	{
		return sizeof(m_user)
			+ sizeof(m_password);
	}

	virtual const char* GetBuffer()
	{
		if (NULL == m_pBuffer)
		{
			m_pBuffer = new char[GetLength()];

			char* l_pBuffer = m_pBuffer;
			SMS_COPY_CHAR_ARRAY(l_pBuffer, m_user);
			SMS_COPY_CHAR_ARRAY(l_pBuffer, m_password);
		}
		return m_pBuffer;
	}

	virtual bool SetBuffer(const char* p_pBuffer, int p_nLen)
	{
		// ������Ҫ�����л�

		return false;
	}

public:
	void SetInfo(std::string  p_strUser, std::string  p_strPassword)
	{
		ReleaseBuffer();

		memset(m_user, 0, sizeof(m_user));
		memcpy(m_user, p_strUser.c_str(), SmsUtil::MIN(GetLength(), (int)(p_strUser.length())));

		memset(m_password, 0, sizeof(m_password));
		memcpy(m_password, p_strPassword.c_str(), SmsUtil::MIN(GetLength(), (int)(p_strPassword.length())));
	}

	std::string  GetInfo()
	{
		char tmp[1024] = { 0 };
		boost::log::aux::snprintf(tmp, 1024, "SmsBodyLogin: m_user[%s], m_password[%s]",
			SmsUtil::toString(m_user, sizeof(m_user)).c_str(),
			SmsUtil::toString(m_password, sizeof(m_password)).c_str());
		return tmp;
	}
};

//////////////////////////////////////////////////////////////////////////
// ���Ͷ���
class SmsBodySendMsg : public SmsBody
{
public:
	SmsBodySendMsg()
	{
		memset(m_szTelNum, 0, 24);
		m_nContenLen = 0;
		m_pContent = NULL;
	}
	~SmsBodySendMsg()
	{
		if (m_pContent)
		{
			delete m_pContent;
			m_pContent = NULL;
		}
		m_nContenLen = 0;
	}

public:
	char m_szTelNum[24];	// 24�ֽڣ������ߵ绰����
	int m_nContenLen;		// 4�ֽڣ��������ݳ���
	char* m_pContent;		// ��������

public:
	virtual int GetLength()
	{
		return sizeof(m_szTelNum)
			+ sizeof(m_nContenLen)
			+ m_nContenLen;
	}

	virtual const char* GetBuffer()
	{
		if (NULL == m_pBuffer)
		{
			m_pBuffer = new char[GetLength()];

			char* l_pBuffer = m_pBuffer;
			SMS_COPY_CHAR_ARRAY(l_pBuffer, m_szTelNum);
			SMS_COPY_INT(l_pBuffer, m_nContenLen);
			SMS_COPY_CHAR_POINTER(l_pBuffer, m_pContent, m_nContenLen);
		}
		return m_pBuffer;
	}

	virtual bool SetBuffer(const char* p_pBuffer, int p_nLen)
	{
		// ������Ҫ�����л�

		return false;
	}

public:
	void SetInfo(std::string  p_strTelNum, std::string  p_strContent)
	{
		ReleaseBuffer();
		
		memset(m_szTelNum, 0, sizeof(m_szTelNum));
		memcpy(m_szTelNum, p_strTelNum.c_str(), SmsUtil::MIN(GetLength(), (int)(p_strTelNum.length())));

		if (m_pContent)
		{
			delete m_pContent;
			m_pContent = NULL;
		}
		
		m_nContenLen = p_strContent.length();
		m_pContent = new char[p_strContent.length()];
		memcpy(m_pContent, p_strContent.c_str(), SmsUtil::MIN(GetLength(), (int)(p_strContent.length())));
	}

	std::string  GetInfo()
	{
		char tmp[10240] = { 0 };
		boost::log::aux::snprintf(tmp, 10240, "SmsBodySendMsg: m_szTelNum[%s], m_nContenLen[%d], m_pContent[%s]",
			SmsUtil::toString(m_szTelNum, sizeof(m_szTelNum)).c_str(), m_nContenLen,
			SmsUtil::toString(m_pContent, m_nContenLen).c_str());
		return tmp;
	}
};

//////////////////////////////////////////////////////////////////////////
// ����Ϊ�Ѷ�
class SmsBodySetReaded : public SmsBody
{
public:
	SmsBodySetReaded()
	{
		memset(m_id, 0, sizeof(m_id));
	}
	~SmsBodySetReaded()
	{
	}

public:
	char m_id[32];		// 32�ֽڣ���ϢID

public:
	virtual int GetLength()
	{
		return sizeof(m_id);
	}

	virtual const char* GetBuffer()
	{
		if (NULL == m_pBuffer)
		{
			m_pBuffer = new char[GetLength()];

			char* l_pBuffer = m_pBuffer;
			SMS_COPY_CHAR_ARRAY(l_pBuffer, m_id);
		}
		return m_pBuffer;
	}

	virtual bool SetBuffer(const char* p_pBuffer, int p_nLen)
	{
		// ������Ҫ�����л�

		return false;
	}

public:
	void SetInfo(std::string  p_strID)
	{
		ReleaseBuffer();
		
		memset(m_id, 0, sizeof(m_id));
		memcpy(m_id, p_strID.c_str(), SmsUtil::MIN(GetLength(), (int)(p_strID.length())));
	}

	std::string  GetInfo()
	{
		char tmp[1024] = { 0 };
		boost::log::aux::snprintf(tmp, 1024, "SmsBodySetReaded: m_id[%s]",
			SmsUtil::toString(m_id, sizeof(m_id)).c_str());
		return tmp;
	}
};

//////////////////////////////////////////////////////////////////////////
// ����λ
class SmsBodyGetLocation : public SmsBody
{
public:
	SmsBodyGetLocation()
	{
		memset(m_szTelNum, 0, sizeof(m_szTelNum));
	}
	~SmsBodyGetLocation()
	{
	}

public:
	char m_szTelNum[24];		// 24�ֽ�,�绰����

public:
	virtual int GetLength()
	{
		return sizeof(m_szTelNum);
	}

	virtual const char* GetBuffer()
	{
		if (NULL == m_pBuffer)
		{
			m_pBuffer = new char[GetLength()];

			char* l_pBuffer = m_pBuffer;
			SMS_COPY_CHAR_ARRAY(l_pBuffer, m_szTelNum);
		}
		return m_pBuffer;
	}

	virtual bool SetBuffer(const char* p_pBuffer, int p_nLen)
	{
		// ������Ҫ�����л�

		return false;
	}

public:
	void SetInfo(std::string  p_strTelNum)
	{
		ReleaseBuffer();

		memset(m_szTelNum, 0, sizeof(m_szTelNum));
		memcpy(m_szTelNum, p_strTelNum.c_str(), SmsUtil::MIN(GetLength(), (int)(p_strTelNum.length())));
	}

	std::string  GetInfo()
	{
		char tmp[1024] = { 0 };
		boost::log::aux::snprintf(tmp, 1024, "SmsBodyGetLocation: m_szTelNum[%s]",
			SmsUtil::toString(m_szTelNum, sizeof(m_szTelNum)).c_str());
		return tmp;
	}
};

//////////////////////////////////////////////////////////////////////////
// ������չ�ֶ�
class SmsTag : public SmsBase
{
public:
	SmsTag()
	{
		m_nTagType = 0;
	}
	~SmsTag()
	{
	}

public:
	int m_nTagType;

public:
	virtual int GetLength()
	{
		return sizeof(m_nTagType);
	}

	virtual const char* GetBuffer()
	{
		// Ŀǰû�ж���

		return NULL;
	}

	virtual bool SetBuffer(const char* p_pBuffer, int p_nLen)
	{
		// Ŀǰû�ж���

		return false;
	}

public:
	void SetInfo()
	{
		// Ŀǰû�ж���
	}

	std::string  GetInfo()
	{
		return std::string("SmsTag");
	}
};

//////////////////////////////////////////////////////////////////////////
// ��¼�ظ�
class SmsBodyLoginResp : public SmsBody
{
public:
	SmsBodyLoginResp()
	{
		m_iResult = 0;
		m_iSendSmsRight = 0;
		m_iSendTextRight = 0;
		m_iRedispatchRigth = 0;
		memset(m_szUnitId, 0, sizeof(m_szUnitId));
		m_dVersion = 0;
	}
	~SmsBodyLoginResp()
	{
	}

public:
	int m_iResult;			// 4�ֽڣ���½���
	int m_iSendSmsRight;	// 4�ֽڣ����Ͷ���Ȩ�ޣ�0�����ƣ�1ֻ�ܻظ�
	int m_iSendTextRight;	// 4�ֽڣ������ı�Ȩ��
	int m_iRedispatchRigth; // 4�ֽڣ�ת��Ȩ��
	char m_szUnitId[16];	// 16�ֽڣ���λID
	double m_dVersion;		// 8�ֽڣ��������汾

public:
	virtual int GetLength()
	{
		return sizeof(m_iResult)
			+ sizeof(m_iSendSmsRight)
			+ sizeof(m_iSendTextRight)
			+ sizeof(m_iRedispatchRigth)
			+ sizeof(m_szUnitId)
			+ sizeof(m_dVersion);
	}

	virtual const char* GetBuffer()
	{
		// �ظ�����Ҫ���л�

		return NULL;
	}

	virtual bool SetBuffer(const char* p_pBuffer, int p_nLen)
	{
		if (p_nLen < GetLength())
		{
			return false;
		}

		ReleaseBuffer();

		char* l_pBuffer = (char*)p_pBuffer;
		SMS_RCOPY_INT(l_pBuffer, m_iResult);
		SMS_RCOPY_INT(l_pBuffer, m_iSendSmsRight);
		SMS_RCOPY_INT(l_pBuffer, m_iSendTextRight);
		SMS_RCOPY_INT(l_pBuffer, m_iRedispatchRigth);
		SMS_RCOPY_CHAR_ARRAY(l_pBuffer, m_szUnitId);
		SMS_RCOPY_DOUBLE(l_pBuffer, m_dVersion);

		return true;
	}

public:
	void SetInfo()
	{
		// Ŀǰû�ж���
	}

	std::string GetInfo()
	{
		char tmp[1024] = { 0 };
		boost::log::aux::snprintf(tmp, 1024, "SmsBodyLoginResp: m_iResult[%d], m_iSendSmsRight[%d], m_iSendTextRight[%d], m_iRedispatchRigth[%d], m_szUnitId[%s], m_dVersion[%lf]",
			m_iResult, m_iSendSmsRight, m_iSendTextRight, m_iRedispatchRigth,
			SmsUtil::toString(m_szUnitId, sizeof(m_szUnitId)).c_str(), m_dVersion);
		return tmp;
	}
};

//////////////////////////////////////////////////////////////////////////
// ��λ�ظ�
class SmsBodyGetLocationResp : public SmsBody
{
public:
	SmsBodyGetLocationResp()
	{
		m_iResult = 0;
		m_dX = 0;
		m_dY = 0;
		m_iVenderError = 0;
		memset(m_szDescription, 0, sizeof(m_szDescription));
	}
	~SmsBodyGetLocationResp()
	{
	}

public:
	int m_iResult;				// 4�ֽڣ������0Ϊ�ɹ�
	double m_dX;				// 8�ֽڣ�X����
	double m_dY;				// 8�ֽڣ�Y����
	int m_iVenderError;			// 4�ֽڣ���Ӫ�̷��صĴ������
	char m_szDescription[128];	// 24�ֽڣ�λ��������Ϣ�����������Ϣ

public:
	virtual int GetLength()
	{
		return sizeof(m_iResult)
			+ sizeof(m_dX)
			+ sizeof(m_dY)
			+ sizeof(m_iVenderError)
			+ sizeof(m_szDescription);
	}

	virtual const char* GetBuffer()
	{
		// �ظ�����Ҫ���л�

		return NULL;
	}

	virtual bool SetBuffer(const char* p_pBuffer, int p_nLen)
	{
		if (p_nLen < GetLength())
		{
			return false;
		}

		ReleaseBuffer();

		char* l_pBuffer = (char*)p_pBuffer;
		SMS_RCOPY_INT(l_pBuffer, m_iResult);
		SMS_RCOPY_DOUBLE(l_pBuffer, m_dX);
		SMS_RCOPY_DOUBLE(l_pBuffer, m_dY);
		SMS_RCOPY_INT(l_pBuffer, m_iVenderError);
		SMS_RCOPY_CHAR_ARRAY(l_pBuffer, m_szDescription);

		return true;
	}

public:
	void SetInfo()
	{
		// Ŀǰû�ж���
	}

	std::string  GetInfo()
	{
		char tmp[1024] = { 0 };
		boost::log::aux::snprintf(tmp, 1024, "SmsBodyGetLocationResp: m_iResult[%d], m_dX[%lf], m_dY[%lf], m_iVenderError[%d], m_szDescription[%s]",
			m_iResult, m_dX, m_dY, m_iVenderError, SmsUtil::toString(m_szDescription, sizeof(m_szDescription)).c_str());
		return tmp;
	}
};

//////////////////////////////////////////////////////////////////////////
// �յ�������״̬
class SmsBodyServerStatusArrival : public SmsBody
{
public:
	SmsBodyServerStatusArrival()
	{
		m_iType = 0;
		m_bStatus = 0;
		memset(m_szDescription, 0, sizeof(m_szDescription));
	}
	~SmsBodyServerStatusArrival()
	{
	}

public:
	int m_iType;
	char m_bStatus;
	char m_szDescription[128];

public:
	virtual int GetLength()
	{
		return sizeof(m_iType)
			+ sizeof(m_bStatus)
			+ sizeof(m_szDescription);
	}

	virtual const char* GetBuffer()
	{
		// �ظ�����Ҫ���л�

		return NULL;
	}

	virtual bool SetBuffer(const char* p_pBuffer, int p_nLen)
	{
		if (p_nLen < GetLength())
		{
			return false;
		}

		ReleaseBuffer();

		char* l_pBuffer = (char*)p_pBuffer;
		SMS_RCOPY_INT(l_pBuffer, m_iType);
		SMS_RCOPY_CHAR(l_pBuffer, m_bStatus);
		SMS_RCOPY_CHAR_ARRAY(l_pBuffer, m_szDescription);

		return true;
	}

public:
	void SetInfo()
	{
		// Ŀǰû�ж���
	}

	std::string  GetInfo()
	{
		char tmp[1024] = { 0 };
		boost::log::aux::snprintf(tmp, 1024, "SmsBodyServerStatusArrival: m_iType[%d], m_bStatus[%s], m_szDescription[%s]",
			m_iType, (m_bStatus > 0 ? L"1" : L"0"), SmsUtil::toString(m_szDescription, sizeof(m_szDescription)).c_str());
		return tmp;
	}
};

//////////////////////////////////////////////////////////////////////////
// �յ�������״̬
class SmsBodyUserStatusArrival : public SmsBody
{
public:
	SmsBodyUserStatusArrival()
	{
		m_iType = 0;
		m_iCount = 0;
		m_pUnitIDList = NULL;

	}
	~SmsBodyUserStatusArrival()
	{
		if (m_pUnitIDList)
		{
			delete m_pUnitIDList;
			m_pUnitIDList = NULL;
		}
	}

public:
	int m_iType;
	int m_iCount;
	char* m_pUnitIDList; // m_iCount*16

public:
	virtual int GetLength()
	{
		return sizeof(m_iType)
			+ sizeof(m_iCount)
			+ m_iCount * 16;
	}

	virtual const char* GetBuffer()
	{
		// �ظ�����Ҫ���л�

		return NULL;
	}

	virtual bool SetBuffer(const char* p_pBuffer, int p_nLen)
	{
		if (p_nLen < (sizeof(m_iType) + sizeof(m_iCount)))
		{ // �ж���С����
			return false;
		}

		{ // �ж����ݳ���
			int l_iType = 0;
			int l_iCount = 0;

			char* l_pBuffer = (char*)p_pBuffer;
			SMS_RCOPY_INT(l_pBuffer, l_iType);
			SMS_RCOPY_INT(l_pBuffer, l_iCount);

			if (p_nLen < (int)(sizeof(m_iType) + sizeof(m_iCount) + l_iCount * 16))
			{
				return false;
			}
		}

		ReleaseBuffer();

		if (m_pUnitIDList)
		{
			delete m_pUnitIDList;
			m_pUnitIDList = NULL;
		}

		char* l_pBuffer = (char*)p_pBuffer;
		SMS_RCOPY_INT(l_pBuffer, m_iType);
		SMS_RCOPY_INT(l_pBuffer, m_iCount);
		if (m_iCount > 0)
		{
			m_pUnitIDList = new char[m_iCount * 16];
			SMS_RCOPY_CHAR_POINTER(l_pBuffer, m_pUnitIDList, m_iCount * 16);
		}

		return true;
	}

public:
	void SetInfo()
	{
		// Ŀǰû�ж���
	}

	std::string  GetInfo()
	{
		std::string tmp_UnitIDList;
		for (int i = 0; i < m_iCount; i++)
		{
			tmp_UnitIDList += SmsUtil::toString(m_pUnitIDList + i * 16, 16);
			tmp_UnitIDList += ",";
		}
		char tmp[10240] = { 0 };
		boost::log::aux::snprintf(tmp, 10240, "SmsBodyUserStatusArrival: m_iType[%d], m_iCount[%d], m_pUnitIDList[%s]",
			m_iType, m_iCount, tmp_UnitIDList.c_str());
		return tmp;
	}
};

//////////////////////////////////////////////////////////////////////////
// �յ�����
class SmsBodySmsArrival : public SmsBody
{
public:
	SmsBodySmsArrival()
	{
		memset(m_szID, 0, sizeof(m_szID));
		memset(m_szSender, 0, sizeof(m_szSender));
		memset(m_szReceiver, 0, sizeof(m_szReceiver));
		m_iContentLen = 0;
		m_pContent = NULL;
		memset(m_szTime, 0, sizeof(m_szTime));
		m_dX = 0;
		m_dY = 0;
		memset(m_szDescription, 0, sizeof(m_szDescription));
	}
	~SmsBodySmsArrival()
	{
		if (m_pContent)
		{
			delete m_pContent;
			m_pContent = NULL;
		}
		m_iContentLen = 0;
	}

public:
	char m_szID[32];			// 32�ֽڣ���ϢID
	char m_szSender[24];		// 24�ֽڣ����ͷ�����
	char m_szReceiver[24];		// 24�ֽڣ������ߺ���
	int m_iContentLen;			// 4�ֽڣ��������ݳ���
	char* m_pContent;			//�ֽ���nContentLen
	char m_szTime[14];			//ʱ�䣬��ʽΪyyyyMMddHHmmss
	double m_dX;				//x����
	double m_dY;				//y����
	char m_szDescription[128];	//λ��������Ϣ	

public:
	virtual int GetLength()
	{
		return sizeof(m_szID)
			+ sizeof(m_szSender)
			+ sizeof(m_szReceiver)
			+ sizeof(m_iContentLen)
			+ m_iContentLen
			+ sizeof(m_szTime)
			+ sizeof(m_dX)
			+ sizeof(m_dY)
			+ sizeof(m_szDescription);
	}

	virtual const char* GetBuffer()
	{
		// �ظ�����Ҫ���л�

		return NULL;
	}

	virtual bool SetBuffer(const char* p_pBuffer, int p_nLen)
	{
		if (p_nLen < GetLength() - m_iContentLen)
		{ // �ж���С����
			return false;
		}

		{ // �ж����ݳ���
			char l_szID[32];
			char l_szSender[24];
			char l_szReceiver[24];
			int l_iContentLen;

			char* l_pBuffer = (char*)p_pBuffer;
			SMS_RCOPY_CHAR_ARRAY(l_pBuffer, l_szID);
			SMS_RCOPY_CHAR_ARRAY(l_pBuffer, l_szSender);
			SMS_RCOPY_CHAR_ARRAY(l_pBuffer, l_szReceiver);
			SMS_RCOPY_INT(l_pBuffer, l_iContentLen);

			if (p_nLen < GetLength() - m_iContentLen + l_iContentLen)
			{
				return false;
			}
		}

		ReleaseBuffer();

		if (m_pContent)
		{
			delete m_pContent;
			m_pContent = NULL;
		}

		char* l_pBuffer = (char*)p_pBuffer;
		SMS_RCOPY_CHAR_ARRAY(l_pBuffer, m_szID);
		SMS_RCOPY_CHAR_ARRAY(l_pBuffer, m_szSender);
		SMS_RCOPY_CHAR_ARRAY(l_pBuffer, m_szReceiver);
		SMS_RCOPY_INT(l_pBuffer, m_iContentLen);
		if (m_iContentLen > 0)
		{
			m_pContent = new char[m_iContentLen+1];
			memset(m_pContent, 0, sizeof(char) * m_iContentLen + 1);
			SMS_RCOPY_CHAR_POINTER(l_pBuffer, m_pContent, m_iContentLen);
		}
		SMS_RCOPY_CHAR_ARRAY(l_pBuffer, m_szTime);
		SMS_RCOPY_DOUBLE(l_pBuffer, m_dX);
		SMS_RCOPY_DOUBLE(l_pBuffer, m_dY);
		SMS_RCOPY_CHAR_ARRAY(l_pBuffer, m_szDescription);

		return true;
	}

public:
	void SetInfo()
	{
		// Ŀǰû�ж���
	}

	std::string  GetInfo()
	{
		char tmp[10240] = { 0 };
		boost::log::aux::snprintf(tmp, 10240, "SmsBodySmsArrival: m_szID[%s], m_szSender[%s], m_szReceiver[%s], m_iContentLen[%d], m_pContent[%s],m_szTime[%s], m_dX[%lf],m_dY[%lf], m_szDescription[%s]",
			SmsUtil::toString(m_szID, sizeof(m_szID)).c_str(),
			SmsUtil::toString(m_szSender, sizeof(m_szSender)).c_str(),
			SmsUtil::toString(m_szReceiver, sizeof(m_szReceiver)).c_str(),
			m_iContentLen,
			SmsUtil::toString(m_pContent, m_iContentLen).c_str(),
			SmsUtil::toString(m_szTime, sizeof(m_szTime)).c_str(),
			m_dX, m_dY,
			SmsUtil::toString(m_szDescription, sizeof(m_szDescription)).c_str());
		return tmp;
	}
};