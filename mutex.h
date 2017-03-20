#ifndef __MUTXT_H__
#define __MUTXT_H__
#include "nocopyable.h"
#include "type_define.h"

class Mutex : private Nocopyable
{
public:
	Mutex()
	{
		pthread_mutex_init(&m_mutex, NULL);
	}
	~Mutex()
	{
		pthread_mutex_destroy(&m_mutex);
	}
	void Lock()
	{
		pthread_mutex_lock(&m_mutex);
	}
	void UnLock()
	{
		pthread_mutex_unlock(&m_mutex);
	}
	bool TryLock()
	{
		return (pthread_mutex_trylock(&m_mutex) == 0);
	}

	pthread_mutex_t* GetPthreadMutex()
	{
		return &m_mutex;
	}

private:
	pthread_mutex_t m_mutex;
};


class MutexLockGuard : private Nocopyable
{
public:
	explicit MutexLockGuard(Mutex& mutex) : m_mutex(mutex)
	{
		m_mutex.Lock();
	}

	~MutexLockGuard()
	{
		m_mutex.UnLock();
	}
private:
	Mutex& m_mutex;
};

class RWLock : private Nocopyable
{
public:
	RWLock()
	{
		pthread_rwlock_init(&m_rwlock, NULL);
	}
	~RWLock()
	{
		pthread_rwlock_destroy(&m_rwlock);
	}
	void rdlock()
	{
		pthread_rwlock_rdlock(&m_rwlock);
	}
	void wrlock()
	{
		pthread_rwlock_wrlock(&m_rwlock);
	}

	void unlock()
	{
		pthread_rwlock_unlock(&m_rwlock);
	}

private:
	pthread_rwlock_t m_rwlock;
};
#endif
