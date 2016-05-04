#include <fstream>
#include <stdarg.h>

#include "logger.h"
enum LogType
{
	LogType_One,
	LogType_Two,
};

__thread char	g_log_file_name[256];
__thread int	g_log_line;
__thread char	g_class_name[32];
__thread uint32 g_class_id;
__thread char	g_name[32];
__thread bool	g_log_type;

Logger& SetLoggerHead(Logger& logger,const char* filename,const uint32 line)
{
	strncpy( g_log_file_name, filename, sizeof( g_log_file_name ) );
	g_log_file_name[strlen( filename )] = '\0';
	g_log_line = line;
	g_log_type = LogType_One;
	return logger;
}


void SetLoggerHead(const char* class_name,const uint32& id,const char* name,const char* filename,const uint32& line)
{
	strncpy( g_log_file_name, filename, sizeof( g_log_file_name ) );
	g_log_file_name[strlen( filename )] = '\0';
	g_log_line = line;
	strncpy( g_class_name, class_name, sizeof( g_class_name ) );
	g_class_name[strlen( class_name )] = '\0';
	strncpy( g_name, name, sizeof( g_name ) );
	g_name[strlen(name)] = '\0';
	g_class_id = id;
	g_log_type = LogType_Two;
}


void InitLogger(const string filename,const char* Level)
{
	logger = new Logger();
	logger->setLevel(Level);
	logger->setLogFileName(filename);
}

inline void FetchLoggerHead( std::string &str )
{
	char buf[256] = { 0 };
	if( LogType_Two == g_log_type )
	{
		snprintf( buf, 256, "%s [%u, %s] %s:%d", g_class_name, g_class_id,g_name, g_log_file_name, g_log_line );
	}
	else
	{
		snprintf( buf, 256, "%s:%d", g_log_file_name, g_log_line );
	}
	str = buf;
}

#define getMessage(msg,msglen,pat)	\
do{\
	va_list ap;\
	memset(msg,0,msglen);\
	va_start(ap,pat);\
	vsnprintf(msg,msglen - 1,pat,ap);\
	va_end(ap);\
}while(false)


LoggerLevel Logger::_level = LoggerLevel_Debug;
bool Logger::_need_logfile = true;
bool Logger::_need_console = true;

Logger::Logger(std::string name)
{
	_ofile = NULL;
	_lastWriteTime = 0;
	this->_logname = name;
}

Logger::~Logger()
{
	delete _ofile;
	_ofile = NULL:
}

void Logger::debug(const char *pattern, ...)
{
	char msg[MSGBUF_MAX] = { 0 };
	getMessage(msg,MSGBUF_MAX,pattern);
	this->log(msg,LoggerLevel_Debug);
}

void Logger::info(const char *pattern, ...)
{
	char msg[MSGBUF_MAX] = { 0 };
	getMessage(msg,MSGBUF_MAX,pattern);
	this->log(msg,LoggerLevel_Info);
}

void Logger::warn(const char *pattern, ...)
{
	char msg[MSGBUF_MAX] = { 0 };
	getMessage(msg,MSGBUF_MAX,pattern);
	this->log(msg,LoggerLevel_Warn);
}

void Logger::error(const char *pattern, ...)
{
	char msg[MSGBUF_MAX] = { 0 };
	getMessage(msg,MSGBUF_MAX,pattern);
	this->log(msg,LoggerLevel_Error);
}

void Logger::log(const char* msg,LoggerLevel level)
{
	if(level < _level)
	{
		return;
	}

	_time.now();
	char tbuf[64]  = {0};
	_time.format(tbuf,64,"%y%m%d-%T");
	char lbuf[MSGBUF_MAX+64];
	std::string head_str;
	FetchLoggerHead(head_str);
	std::string strLevel;
	switch(level)
	{
		case LoggerLevel_Debug:
			{
				strLevel = "DEBUG";
			}
			break;
		case LoggerLevel_Info:
			{
				strLevel = "INFO";
			}
			break;
		case LoggerLevel_Warn:
			{
				strLevel = "WARN";
			}
			break;
		case LoggerLevel_Error:
			{
				strLevel = "ERROR";
			}
			break;
		default:break;
	}
	
	snprintf(lbuf,MSGBUF_MAX+64,"%s %s %s:%s ==>%s",tbuf,_logname.c_str(),strLevel.c_str(),head_str.c_str(),msg);
	_mutex.lock();
	if(_need_console)
	{
		writeLogToConsole(lbuf,level);
	}
	if(_need_logfile)
	{
		writeLogToFile(lbuf);
	}
	_mutex.unlock();
}

void Logger::writeLogToConsole(const char* msg,LoggerLevel level)
{
	char pre[12] = {0};
	char post[10] = "\033[0m";
	switch(level)
	{
		case LoggerLevel_Debug:
			{
				bzero(post,10);
			}
			break;
		case LoggerLevel_Info:
			{
				strcmp(pre,"\033[32;1m")
			}
			break;
		case LoggerLevel_Warn:
			{
				strcmp(pre,"\033[33;1m")
			}
			break;
		case LoggerLevel_Error:
			{
				strcmp(pre,"\033[31;1m")
			}
			break;
		default:break;
	}
	printf("%s%s%s\n",pre,msg,post);
}

void Logger::writeLogToFile(const char* msg)
{
	uint32 sec = _time.sec();
	if(!_ofile || sec /(60*60) != _lastWriteTime/(60*60))
	{
		if(_ofile != NULL) 
		{
			delete _ofile;
			_ofile = NULL:
		}
		else if(_filename.empty())
		{
			return ;
		}

		_ofile = new std::ofstream;
		char buf[50];
		_time.format(buf,50,".%y%m%d-%H");
		std::string of = _filename + std::string(buf);
		_ofile->open(of.c_str(),std::ios::out|std::ios::app);
		if(! (*_ofile)) 
		{
			printf("打开日志文件错误%s\n",of.c_str());
			return; 
		}
		std::ostringstream oss;
		oss.str("");
		oss<<"ln --force -s "<< of <<" "<<_filename;
		int ret = system(oss.str().c_str());
		if(ret == -1)
		{
			printf("链接文件错误%s\n",oss.str().c_str());
		}
	}
	(*_ofile)<<msg<<std::endl;
	_lastWriteTime = sec;

}

void Logger::setLevel(const char *level)
{
	if(strcmp(level,"debug") == 0)
	{
		_level = LoggerLevel_Debug;
	}
	else if(strcmp(level,"info") == 0)
	{
		_level = LoggerLevel_Info;
	}
	else if(strcmp(level,"warn") == 0)
	{
		_level = LoggerLevel_Warn;
	}
	else if(strcmp(level,"error") == 0)
	{
		_level = LoggerLevel_Error;
	}
}
