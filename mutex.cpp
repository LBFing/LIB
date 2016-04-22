#include "mutex.h"

Mutex::Mutex()
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_FAST_NP);
	pthread_mutex_init(&m_mutex, &attr);
}

Mutex::~Mutex()
{
	pthread_mutex_destroy(&m_mutex); 
}

void Mutex::Lock()
{ 
	pthread_mutex_lock(&m_mutex); 
}

void Mutex::UnLock() 
{ 
	pthread_mutex_unlock(&m_mutex); 
}

bool Mutex::TryLock()
{ 
	return (pthread_mutex_trylock(&m_mutex) == 0); 
} 