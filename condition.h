#ifndef __CONDITION_H__
#define __CONDITION_H__
#include "nocopyable.h"
#include "type_define.h"
#include "mutex.h"

class Conditoin : private Nocopyable
{
public:
	Conditoin(Mutex& mutex) : m_mutex(mutex)
	{
		pthread_cond_init(&m_cond,NULL);
	}
	~Conditoin()
	{
		pthread_cond_destroy(&m_cond);
	}

	void Wait()
	{
		pthread_cond_wait(&m_cond,m_mutex.GetPthreadMutex());
	}

	void Notify()
	{
		pthread_cond_signal(&m_cond);
	}

	void NotifyAll()
	{
		pthread_cond_broadcast(&m_cond);
	}

private:
	Mutex &m_mutex;
	pthread_cond_t m_cond;
};
#endif
