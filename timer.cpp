#include "timer.h"

int64 Time::m_timesync = 0L;

void Time::Now()
{
	::clock_gettime(CLOCK_REALTIME, &m_ts);
	m_msec = Msec();
}

void Time::AddDelay(const uint64 delay)
{
	uint64 ns = delay * 1000000L + m_ts.tv_nsec;
	m_ts.tv_sec += (ns / 1000000000L);
	m_ts.tv_nsec = (ns % 1000000000L);
	m_msec = Msec();
}

uint64 Time::Sec() const
{
	tzset();
	return m_ts.tv_sec - m_timesync;
}

uint64 Time::Msec() const
{
	return Sec() * 1000L + m_ts.tv_nsec / 1000000L;
}

uint64 Time::Usec() const
{
	return Sec() * 1000000L + m_ts.tv_nsec / 1000L;
}

uint64 Time::Elapse(const Time& rt) const
{
	if(rt.m_msec  < m_msec)
	{
		return 0;
	}
	else
	{
		return rt.m_msec - m_msec;
	}
}

void Time::Format(char *buffer, size_t bufferlen, const char *format)
{
	struct tm tm_data;
	time_t sec = (time_t) Sec();
	localtime_r(&sec, &tm_data);
	strftime(buffer, bufferlen, format, &tm_data);
}
//=====================================================================


Timer::Timer(const uint64 cycle, bool bBegin, const uint64 delay)
	: m_cycle(cycle)
	, m_off(false)
	, m_time()
{
	if(bBegin)
	{
		m_time.AddDelay(m_cycle + delay);
	}
}
Timer::Timer(const uint64 cycle, bool bBegin,  Time& tv)
	: m_cycle(cycle)
	, m_off(false)
	, m_time(tv)
{
	if(bBegin)
	{
		m_time.AddDelay(m_cycle);
	}
}

void Timer::Reset(const uint64 cycle, const Time& cur)
{
	m_off = false;
	m_cycle = cycle;
	m_time = cur;
	m_time.AddDelay(m_cycle);

}

void Timer::Current(const Time& cur)
{
	m_time = cur;
	m_off = false;
}


void Timer::Next(const Time& cur, const uint32 delay)
{
	m_off = false;
	m_time = cur;
	m_time.AddDelay(delay);
}

void Timer::Next(const Time& cur)
{
	m_off = false;
	m_time = cur;
	m_time.AddDelay(m_cycle);
}

uint32 Timer::LeftSec(const Time& cur)
{
	uint32 self_sec = m_time.Sec();
	uint32 other_sec = cur.Sec();
	if(self_sec > other_sec)
	{
		return self_sec - other_sec;
	}
	else
	{
		return 0;
	}
}

uint64 Timer::LeftMSec(const Time& cur)
{
	return m_time.Msec() > cur.Msec() ? m_time.Msec() - cur.Msec() : 0;
}

bool Timer::operator()(const Time& cur)
{
	if(m_off)
	{
		return false;
	}

	if(m_time.Msec() > cur.Msec())
	{
		return false;
	}
	AddDelay(cur);
	cout << " 1:" << m_time.Msec() << endl;
	return true;
}

bool Timer::Empty()
{
	return m_cycle == 0 ? true : false;
}

void Timer::AddDelay(const Time& cur)
{
	m_time = cur;
	m_time.AddDelay(m_cycle);
}

void Timer::Stop()
{
	m_off = true;
}

//=================================================

Clocker::Clocker(const int64 point, const int64 cycle)
	: m_point(point)
	, m_cycle(cycle)
	, m_clock()
{
	m_timesync = Time::m_timesync;
	assert(m_point);
	assert(m_cycle >= m_point);
	Delay();
}

Clocker::Clocker(const int64 point, const int64 cycle, Time& ctv)
	: m_point(point)
	, m_cycle(cycle)
	, m_clock(ctv)
{
	m_timesync = Time::m_timesync;
	assert(m_point);
	assert(m_cycle >= m_point);
	Delay();
}

void Clocker::Delay()
{
	uint64 escape = (m_clock.Sec() + 8 * 3600) % m_cycle;
	if(escape <= m_point)
	{
		m_clock.AddDelay((m_point - escape) * 1000);
	}
	else
	{
		m_clock.AddDelay((m_cycle + m_point - escape) * 1000);
	}
}
void Clocker::Next(const Time& cur)
{
	m_clock = cur;
	Delay();
}

uint64 Clocker::LeftSec(const Time& cur)
{

	return m_clock.Sec() > cur.Sec() ? m_clock.Sec() - cur.Sec() : 0;
}

bool Clocker::operator()(const Time& cur)
{
	if(m_clock.Sec() + Time::m_timesync - m_timesync > cur.Sec())
	{
		return false;
	}
	else
	{
		m_timesync = Time::m_timesync;
		m_clock = cur;
		Delay();
		return true;
	}
}
