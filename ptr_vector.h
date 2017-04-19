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
		{ delete *it; } //�ͷ�ָ��ָ����ڴ�

		std::vector<T*>::clear();  //�ͷ�ָ�뱾��
	}

	void push_back(T* const& val)
	{
		std::auto_ptr<T> ptr(val);              // ��auto_ptr�ӹ�val����Ȩ
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