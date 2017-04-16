#include "event_loop.h"
#include "poller.h"
#include "timer_queue.h"
#include "logger.h"

__thread EventLoop* t_loopInThisThread = 0;
const int kPollTimeMs = 10000;


int createEventfd()
{
	int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (evtfd < 0)
	{
		ERROR("Failed in eventfd");
		abort();
	}
	return evtfd;
}

EventLoop* EventLoop::GetEventLoopOfCurrentThread()
{
	return t_loopInThisThread;
}

EventLoop::EventLoop()
	: m_looping(false)
	, m_quit(false)
	, m_eventHanding(false)
	, m_callPendingFunctor(false)
	, m_iteration(0)
	, m_threadId(CurrentThread::Tid())
	, m_poller(Poller::NewDefaultPoller(this))
	, m_timerQueue(new TimerQueue(this))
	, m_wakeupFd(createEventfd())
	, m_wakeupChannel(new Channel(this, m_wakeupFd))
	, m_currentActiveChannel(NULL)
{
	DEBUG("EventLoop created : %p in thread :%d", this, m_threadId);
	if(t_loopInThisThread)
	{
		ERROR("Another EventLoop %p exists in this thread :%d ", this, m_threadId);
	}
	else
	{
		t_loopInThisThread = this;
	}
	m_wakeupChannel->SetReadCallBack(std::bind(&EventLoop::handleRead, this));
	m_wakeupChannel->EnableReading();
}

EventLoop::~EventLoop()
{
	DEBUG("EventLoop %p of thread : %d destructs in thread : %d", this, m_threadId, CurrentThread::Tid());
	m_wakeupChannel->DisableAll();
	m_wakeupChannel->Remove();
	::close(m_wakeupFd);
	t_loopInThisThread = NULL;

}

void EventLoop::Loop()
{
	assert(!m_looping);
	AssertInLoopThread();
	m_looping = true;
	DEBUG("EvnetLoop %p start looping", this);

	while(!m_quit)
	{
		m_vecActiveChannel.clear();
		m_pollReturnTime = m_poller->Poll(kPollTimeMs, &m_vecActiveChannel);
		++m_iteration;
		if(Logger::getLoggerLevel() < LoggerLevel_Error)
		{
			printActiveChannel();
		}
		m_eventHanding = true;
		for(vecChannel::iterator it = m_vecActiveChannel.begin();
		        it != m_vecActiveChannel.end(); ++it)
		{
			m_currentActiveChannel = *it;
			m_currentActiveChannel->HandleEvent(m_pollReturnTime);
		}
		m_currentActiveChannel = NULL;
		m_eventHanding = false;
		doPendingFunctor();

	}
	DEBUG("EvnetLoop %p stop looping", this);
	m_looping = false;
}

void EventLoop::Quit()
{
	m_quit = true;;

	if(!IsInLoopThread())
	{
		Wakeup();
	}
}

void EventLoop::RunInLoop(const Functor& cb)
{
	if(IsInLoopThread())
	{
		cb();
	}
	else
	{
		QueueInLoop(cb);
	}
}

void EventLoop::QueueInLoop(const Functor& cb)
{
	{
		MutexLockGuard lock(m_mutex);
		m_pendingFunctor.push_back(cb);
	}

	if(!IsInLoopThread() || m_callPendingFunctor)
	{
		Wakeup();
	}
}

size_t EventLoop::QueueSize() const
{
	MutexLockGuard lock(m_mutex);
	return m_pendingFunctor.size();
}

TimerId EventLoop::RunAt(const Timestamp& time, const TimerCallback& cb)
{
	return m_timerQueue->AddTimer(cb, time, 0.0);
}
