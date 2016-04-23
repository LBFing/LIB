#pragma once
#include "type_define.h"
#include "nocopyable.h"
//时间定义类，封装对时间的操作
class Time
{
public:
	Time() { Now();}
	~Time() {}
	Time(const Time& rt)
	{
		m_ts = rt.m_ts;
		m_msec = rt.m_msec;
	}

	Time& operator= (const Time& rt)
	{
		m_ts = rt.m_ts;
		m_msec = rt.m_msec;
		return *this;
	}

	bool operator > (const Time& rt) const
	{
		return m_msec > rt.m_msec;
	}

	bool operator >= (const Time& rt) const
	{
		return m_msec >= rt.m_msec;
	}

	bool operator < (const Time& rt) const
	{
		return m_msec < rt.m_msec;
	}

	bool operator <= (const Time& rt) const
	{
		return m_msec <= rt.m_msec;
	}

	bool operator == (const Time& rt) const
	{
		return m_msec == rt.m_msec;
	}

	//delay为毫秒级别的
	void AddDelay(const uint64 delay);
	void Now();
	uint64 Sec() const;
	uint64 Msec() const;
	uint64 Usec() const;
	uint64 Elapse(const Time& rt) const;
	void Format(char *buffer, size_t bufferlen, const char *format = "%C/%m/%d %T");
private:
	friend class Clocker;
	struct timespec m_ts;
	uint64 m_msec;
	static int64 m_timesync;
};

//定时器封装
class Timer : public Noncopyable
{
public:
	explicit Timer(const uint64 cycle, bool bBegin = false, const uint64 delay = 0);
	explicit Timer(const uint64 cycle, bool bBegin,  Time& tv);

	void Reset(const uint64 cycle, const Time& cur);
	void Current(const Time& cur);
	void Next(const Time& cur, const uint32 delay);
	void Next(const Time& cur);
	uint32 LeftSec(const Time& cur);
	uint64 LeftMSec(const Time& cur);
	bool operator()(const Time& cur);
	bool Empty();
	void Stop();
	void AddDelay(const Time& cur);
private:
	//计时周期 毫秒时长
	uint64 m_cycle;
	//定时器开关
	bool m_off;
	Time m_time;
};


class Clocker: public Noncopyable
{
public:
	// 以当前时间开始，interval作为闹钟周期，cycle作为闹钟点构造一个闹钟
	explicit Clocker(const int64 point, const int64 cycle);
	explicit Clocker(const int64 point, const int64 cycle, Time& ctv);
	void Next(const Time& cur);
	uint64 LeftSec(const Time& cur);
	bool operator()(const Time& cur);

private:
	void Delay();

private:
	const uint64 m_point;
	const uint64 m_cycle;
	int64 m_timesync;
	Time m_clock;
};
