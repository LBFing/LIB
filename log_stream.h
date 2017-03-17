//日志流文件
#ifndef __LOG_STREAM_H__
#define __LOG_STREAM_H__

#include "type_define.h"
#include "nocopyable.h"

const int32 kSmallBuffer = 4000;
const int32 KLargeBuffer = 4000 * 1000;

typedef void (*FunCookie)();

template <int SIZE>
class FixedBuffer : public Noncopyable
{
public:
	FixedBuffer(): m_cur(m_data)
	{
		SetCookie(cookieStart);
	}

	~FixedBuffer()
	{
		SetCookie(cookieEnd);
	}

	void Append(const char* buf, size_t len)
	{
		int32 avali_able = avali();
		if(avali_able > len)
		{
			memcpy(m_cur, buf, len);
			m_cur += len;
		}
	}

	int32 avali() const
	{
		return static_cast<int32>(m_cur - m_data);
	}

	void SetCookie(FunCookie cookie)
	{
		m_fun_cookie = cookie;
	}

	const char* Data() const { return m_data; }
	int Length() const { return static_cast<int> (m_cur - m_data); }


private:

	const char* end() const {return m_data + sizeof(m_data);}
	static void cookieStart();
	static void cookieEnd();

	FunCookie m_fun_cookie;
	char m_data[SIZE];
	char* m_cur;
};




#endif
