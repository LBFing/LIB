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
	//设置开始回调
	FixedBuffer(): m_cur(m_data)
	{
		SetCookie(cookieStart);
	}

	//设置结束回调
	~FixedBuffer()
	{
		SetCookie(cookieEnd);
	}

	//添加字符空间
	void Append(const char* buf, size_t len)
	{
		int32 avali_able = avali();
		if(avali_able > len)
		{
			memcpy(m_cur, buf, len);
			m_cur += len;
		}
	}

	//计算buff剩余空间
	int Avail() const { return static_cast<int>(end() - Current()); }

	//设置工作函数
	void SetCookie(FunCookie cookie)
	{
		m_fun_cookie = cookie;
	}

	//返回数据地址
	const char* Data() const { return m_data; }

	//返回当前已写的长度
	int Length() const { return static_cast<int> (m_cur - m_data); }

	//返回当前的写的地址
	char* Current() { return m_cur };

	//已写的空间便宜
	void Add(size_t len) { m_cur += len; };

	//缓存区重置
	void Reset() { m_cur = m_data };

	//缓冲区数据清0
	void BZero() { ::bzero(m_data, sizeof(m_data)); }

	//调试使用
	char char* DebugString();

	//账号当前已写数据为string
	string ToString() const { return string(m_data, Length()); }


private:

	const char* end() const {return m_data + sizeof(m_data);}
	static void cookieStart();
	static void cookieEnd();

	FunCookie m_fun_cookie;
	char m_data[SIZE];
	char* m_cur;
};




#endif
