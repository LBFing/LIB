#ifndef __PTR_VECTOR_H__
#define __PTR_VECTOR_H__

#include "type_define.h"

template <typename T>
class ptr_vector : public std::vector < T* >
{
public:
	~ptr_vector()
	{
		clear();
	}

	void clear()
	{
		std::vector<T*>::iterator it;
		for (it = begin(); it != end(); ++it)
		{ delete *it; } //释放指针指向的内存

		std::vector<T*>::clear();  //释放指针本身
	}

	void push_back(T* const& val)
	{
		std::auto_ptr<T> ptr(val);              // 用auto_ptr接管val所有权
		std::vector<T*>::push_back(val);        // operator new
		ptr.release();
	}

	void push_back(std::auto_ptr<T>& val)
	{
		std::vector<T*>::push_back(val.get());
		val.release();
	}
};

#endif // _PTR_VECTOR_H_