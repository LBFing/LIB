#include "timer_queue.h"
#include "logger.h"

int32 createTimerfd()
{
	//创建一个定时器文件
	//CLOCK_MONOTONIC:CLOCK_REALTIME相反，它是以绝对时间为准，获取的时间为系统重启到现在的时间
	//CLOCK_REALTIME：相对时间，从1970.1.1到目前的时间。更改系统时间会更改获取的值
	//TFD_NONBLOCK（非阻塞），TFD_CLOEXEC（同O_CLOEXEC）
	int32 timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	if (timerfd < 0)
	{
		ERROR("Failed in timerfd_create");
	}
	return timerfd;
}

void readTimerfd(int32 timerfd, Timestamp now)
{
	int64 howmany;
	ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
	DEBUG("TimerQueue::handleRead() %lld at %s", howmany, now.ToString().c_str());
	if( n != sizeof(howmany))
	{
		ERROR("TimerQueue::handleRead() reads %lu bytes instead of 8", n);
	}
}

struct timespec howMuchTimeFromNow(Timestamp when)
{
	int64 microSeconds = when.GetMicroSeconds() - Timestamp::Now().GetMicroSeconds();
	if(microSeconds < 100)
	{
		microSeconds = 100;
	}
	struct timespec ts;
	ts.tv_sec = static_cast<time_t>(microSeconds / Timestamp::kMicroSecondsPerSecond);
	ts.tv_nsec = static_cast<long>((microSeconds % Timestamp::kMicroSecondsPerSecond) *  1000);
	return ts;
}

void resetTimerfd(int32 timerfd, Timestamp expiration)
{
	struct itimerspec newValue;
	struct itimerspec oldValue;
	bzero(&newValue, sizeof newValue);
	bzero(&oldValue, sizeof oldValue);
	newValue.it_value = howMuchTimeFromNow(expiration);
	int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
	if (ret)
	{
		INFO("timerfd_settime()");
	}
}

TimerQueue::TimerQueue(EventLoop* loop)
	: m_pLoop(loop)
	, m_timerFd(createTimerfd())
	, m_timerFdChannel(loop, m_timerFd)
	, m_setTimer()
	, m_callingExpiredTimer(false)
{
	m_timerFdChannel.SetReadCallBack(std::bind(&TimerQueue::handleRead, this));
	m_timerFdChannel.EnableReading();
}

TimerQueue::~TimerQueue()
{
	m_timerFdChannel.DisableAll();
	m_timerFdChannel.Remove();
	::close(m_timerFd);
	for(TimerSet::iterator it = m_setTimer.begin(); it != m_setTimer.end(); ++it)
	{
		delete it->second;
	}
	m_setTimer.clear();
}
