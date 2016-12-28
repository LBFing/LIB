#include "nocopyable.h"
#include "type_define.h"
#include "mutex.h"

class Conditoin : private Noncopyable
{
public:
	
private:
	Mutex &m_mutex;
	pthread_cond_t m_cond;
};
