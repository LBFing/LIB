#ifndef __BUFFER_EX_H__
#define  __BUFFER_EX_H__
#include "type_define.h"
#include "nocopyable.h"

class BufferEx : public Noncopyable
{
public:
	static const size_t nCheapPrepend = 8;
	static const size_t nInitialSize = 1024;

	//初始化buff
	explicit BufferEx(size_t nSize = nInitialSize)
		: m_buffer(nCheapPrepend + nSize)
		, m_read_index(nCheapPrepend)
		, m_write_index(nCheapPrepend)
	{
		assert(ReadableBytes() == 0);
		assert(WriteableBytes() == nSize);
		assert(PrependableBytes() == nCheapPrepend);
	}

	//获取buff 可读的空间
	size_t ReadableBytes() const {return m_write_index - m_read_index;}
	//获取buff 可写的空间
	size_t WriteableBytes() const {return m_buffer.size() - m_write_index;}
	//获取buff 读取的位置
	size_t PrependableBytes() const {return m_read_index;}

	//buff 缓冲区交换
	void Swap(BufferEx& rhs)
	{
		m_buffer.swap(rhs.m_buffer);
		std::swap(m_read_index, rhs.m_read_index);
		std::swap(m_write_index, rhs.m_write_index);
	}

	//获取缓冲区当前读到的数据指针
	const char* Peek()const
	{
		return begin() + m_read_index;
	}

	//找到缓冲区中换行(\r\n)位置的指针
	const char* FindCRLF() const
	{
		const char* crlf = std::search(Peek(), BeginWrite(), kCRLF, kCRLF + 2);
		return crlf == BeginWrite() ? NULL : crlf;
	}

	//从start 位置开始 找到缓冲区中换行(\r\n)位置的指针
	const char* FindCRLF(const char* start) const
	{
		assert(Peek() <= start && start <= BeginWrite());
		const char* crlf = std::search(start, BeginWrite(), kCRLF, kCRLF + 2);
		return crlf == BeginWrite() ? NULL : crlf;
	}

	//找到缓冲区中换行(\n)位置的指针
	const char* FindEOL() const
	{
		const void* eol = memchr(Peek(), '\n', ReadableBytes());
		return static_cast<const char*>(eol);
	}

	//从start开始 找到缓冲区中换行(\n)位置的指针
	const char* FindEOL(const char* start) const
	{
		assert(Peek() <= start && start <= BeginWrite());
		const void* eol = memchr(Peek(), '\n', ReadableBytes());
		return static_cast<const char*>(eol);
	}

	//找到可写位置的指针
	char* BeginWrite()
	{
		return begin() + m_write_index;
	}
	const char* BeginWrite() const
	{
		return begin() + m_write_index;
	}

	//可读区域向前偏移len 如果空间不够，则全部重置
	void Retrieve(size_t len)
	{
		assert(len <= ReadableBytes());
		if(len < ReadableBytes())
		{
			m_read_index += len;
		}
		else
		{
			RetrieveAll();
		}
	}

	//缓冲区重置
	void RetrieveAll()
	{
		m_write_index = nCheapPrepend;
		m_read_index = nCheapPrepend;
	}

	//读取数据到end 位置
	void RetrieveUnitl(const char* end)
	{
		assert(Peek() <= end && end <= BeginWrite());
		Retrieve(end - Peek());
	}


	void RetrieveInt64()
	{
		Retrieve(sizeof(int64));
	}

	void RetrieveInt32()
	{
		Retrieve(sizeof(int32));
	}

	void RetrieveInt16()
	{
		Retrieve(sizeof(int16));
	}

	void RetrieveInt8()
	{
		Retrieve(sizeof(int8));
	}

	//读取全部的数据 返回string
	string RetrieveAllAsString()
	{
		return RetrieveAsString(ReadableBytes());
	}

	string RetrieveAsString(size_t len)
	{
		assert(len <= ReadableBytes());
		string result(Peek(), len);
		Retrieve(len);
		return result;
	}

	

private:
	char* begin() {return &*m_buffer.begin();}
	const char* begin() const { return &*m_buffer.begin();}
	
	//buff空间重新分配
	void makeSpace(size_t len)
	{
		if(WriteableBytes() + PrependableBytes() < len + nCheapPrepend)
		{
			m_buffer.resize(m_write_index + len)
		}
		else
		{
			assert(nCheapPrepend < m_read_index);
			size_t readable = ReadableBytes();
			std::copy(begin() + m_read_index,begin()+ m_write_index,begin(), nCheapPrepend);
			m_read_index = nCheapPrepend;
			m_write_index = m_read_index + readable;
			assert(readable == ReadableBytes());
		}
	}

private:
	std::vector<char> m_buffer;
	size_t m_read_index;
	size_t m_write_index;
	static const char kCRLF[];
};

#endif
