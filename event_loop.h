#ifndef __EVENT_LOOOP_H__
#define __EVENT_LOOOP_H__

#include "nocopyable.h"
#include "thread.h"
#include "timer.h"
#include "callback.h"
#include "mutex.h"
#include "any.h"


class Poller;
class Channel;
class TimerQueue;

class EventLoop : private Nocopyable
{
public:
	typedef std::function<void()> Functor;
	EventLoop();
	~EventLoop();

	void Loop();
	void Quit();
	Timestamp PollRetrunTime() const { return m_pollReturnTime;}

	int64 Iteration() const { return m_iteration;}

	void RunInLoop(const Functor& cb);

	void QueueInLoop(const Functor& cb);

	size_t QueueSize() const;

	TimerId RunAt(const Timestamp& time, const TimerCallback& cb);

	TimerId RunAfter(double delay, const TimerCallback& cb);

	TimerId RunEvery(double interval, const TimerCallback& cb);

	void Cancel(TimerId timerId);

	void Wakeup();

	void UpdateChannel(Channel* channel);

	void RemoveChannel(Channel* channel);

	bool HasChannel(Channel* channel);

	void AssertInLoopThread()
	{
		if(!IsInLoopThread())
		{
			abortNotInLoopThread();
		}
	}

	bool IsInLoopThread() const { return m_threadId == CurrentThread::Tid();}

	bool EventHandling() const { return m_eventHanding;}

	void SetContext(const any& context) {m_context = context;}

	const any& GetContext() {return m_context;}

	any* GetMutableContext() {return &m_context;}

	static EventLoop* GetEventLoopOfCurrentThread();


private:
	void abortNotInLoopThread();
	void handleRead();
	void doPendingFunctor();
	void printActiveChannel() const; //DEBUG

	typedef std::vector<Channel*> vecChannel;
	bool m_looping;
	bool m_quit;
	bool m_eventHanding;
	bool m_callPendingFunctor;
	int64 m_iteration;
	const pid_t m_threadId;
	Timestamp m_pollReturnTime;
	std::unique_ptr<Poller> m_poller;
	std::unique_ptr<TimerQueue> m_timerQueue;
	int32 m_wakeupFd;
	std::unique_ptr<Channel> m_wakeupChannel;
	any m_context;
	vecChannel m_vecActiveChannel;
	Channel* m_currentActiveChannel;
	mutable Mutex m_mutex;
	std::vector<Functor> m_pendingFunctor;
};


#endif
