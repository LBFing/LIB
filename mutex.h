#include "nocopyable.h"
#include "type_define.h"

class Mutex : private Noncopyable
{
public:
	Mutex();
	~Mutex();
	void Lock();
	void UnLock();
	bool TryLock();
private:
	pthread_mutex_t m_mutex; 
};
