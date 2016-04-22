#pragma once
#include "nocopyable.h"
#include <assert.h>

//单例模式基类
//创建单例时，继承此类，privte Noncopyable 紧致对象的new 拷贝构造，赋值操作
//必须调用newInstance
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
