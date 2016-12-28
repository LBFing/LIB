#include "nocopyable.h"
#include "type_define.h"

class Mutex : private Noncopyable
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
private:
	pthread_mutex_t m_mutex;
};


class MutexLockGuard : private Noncopyable
{
public:
	explicit MutexLockGuard(Mutex& mutex) : m_mutex(mutex)
	{
		m_mutex.Lock();
	}

	MutexLockGuard::~MutexLockGuard
	{
		m_mutex.UnLock();
	}
private:
	Mutex& m_mutex;
}

class RWLock : private Noncopyable
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
