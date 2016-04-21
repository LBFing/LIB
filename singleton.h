#pragma once
#include "nocopyable.h"
#include <assert.h>

template <typename T>
class SingletonBase : private Noncopyable
{
protected:
	SingletonBase() {}
	~SingletonBase() {}
public:

	static T& getInstance()
	{
		assert(m_instance);
		return *m_instance;
	}

	static void newInstance()
	{
		if (m_instance)
		{
			delete m_instance;
			m_instance = NULL;
		}
		m_instance = new T();
	}

	static void delInstance()
	{
		if(m_instance)
		{
			delete m_instance;
		}
	}

protected:
	static T *m_instance;
};

template<typename T>
T *SingletonBase<T>::m_instance = NULL;