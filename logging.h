#ifndef __LOGGING_H__
#define __LOGGING_H__
#include "timer.h"
#include "log_stream.h"

class Logging
{
public:
	enum LogLevel
	{
		DEBUG,
		INFO,
		WARN,
		ERROR,
		NUM_LOG_LEVELS,
	};

	class SourceFile
	{
	public:
		template<int N>
		inline SourceFile(const char (&arr)[N]) : m_data(arr), m_size(N - 1)
		{
			const char* slash = strrchr(m_data, '/');
			if(slash)
			{
				m_data = slash + 1;
				m_size -= static_cast<int32>(m_data - arr);
			}
		}

		explicit SourceFile(const char* filename) : m_data(filename)
		{
			const char* slash = strrchr(filename, '/');
			if (slash)
			{
				m_data = slash + 1;
			}
			m_size = static_cast<int>(strlen(m_data));
		}

		const char* m_data;
		int m_size;
	};




private:
	class Impl
	{
	public:
		typedef Logging::LogLevel LogLevel;
		Impl(LogLevel level, int32 savedErrno, const SourceFile& file, int line);
		void FormatTime();
		void Finish();

		Time m_time;
		LogStream m_stream;
		LogLevel m_level;
		int m_line;
		SourceFile m_basename;

	};

	Impl m_Impl;
};



#endif
