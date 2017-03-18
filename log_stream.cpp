#include "log_stream.h"

template class FixedBuffer < kSmallBuffer > ;
template class FixedBuffer < KLargeBuffer >;

template<int SIZE>
const char* FixedBuffer<SIZE>::DebugString()
{
	*m_cur = '\0';
	return m_cur;
}

template<int SIZE>
void FixedBuffer<SIZE>::cookieStart()
{
}

template<int SIZE>
void FixedBuffer<SIZE>::cookieEnd()
{
}
