#include "logging.h"

__thread char t_errnobuf[512];
__thread char t_time[32];
__thread uint32 t_lastSecond;

const char* strerror_tl(int32 savedErrno)
{
	return strerror_r(savedErrno, t_errnobuf, sizeof(t_errnobuf));
}
const char* LogLevelName[Logging::NUM_LOG_LEVELS] =
{
	"DEBUG ",
	"INFO  ",
	"WARN  ",
	"ERROR ",
};

class T
{
public:
	T(const char* str, unsigned len)
		: str_(str),
		  len_(len)
	{
		assert(strlen(str) == len_);
	}

	const char* str_;
	const unsigned len_;
};

inline LogStream& operator<<(LogStream& s, T v)
{
	s.append(v.str_, v.len_);
	return s;
}

inline LogStream& operator<<(LogStream& s, const Logging::SourceFile& v)
{
	s.append(v.m_data, v.m_size);
	return s;
}

Logging::Impl::Impl(LogLevel level, int32 savedErrno, const SourceFile& file, int32 line)
	: m_time()
	, m_stream()
	, m_level(level)
	, m_line(line)
	, m_basename(file)
{
	FormatTime();
	m_stream << T(LogLevelName[level], 6);
	if (savedErrno != 0)
	{
		m_stream << strerror_tl(savedErrno) << " (errno= " << savedErrno << ") ";
	}
}
void Logging::Impl::FormatTime()
{
	m_time.Now();
	uint32 seconds  = m_time.Sec();
	uint32 microsec = m_time.LeftUsec();
	if(seconds != t_lastSecond)
	{
		t_lastSecond = seconds;
		int32 len = m_time.Format(t_time, sizeof(t_time));
		assert(len == 17);
	}
	Fmt us(".%06d ", microsec);
	assert(us.Length() == 8);
	m_stream << T(t_time, 17) << T(us.Data(), 8);
}
void Logging::Impl::Finish()
{
	m_stream << " - " << m_basename << ":" << m_line << "\n";
}
