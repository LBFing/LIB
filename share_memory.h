#ifndef __SHARE_MEMORY_H__
#define __SHARE_MEMORY_H__

#include "type_define.h"
#include "singleton.h"
#include "entry.h"


#define PAGE_SIZE			256		//定义页面大小
#define SHARE_TRUNK_SIZE	24800	//定义每个共享对象大小占用的字节
//共享内存设置模式前面为ID 后面Wie数据
//前面设置为数据ID1ID2 。。。。Data1Data2 。。。。。

typedef set<void*> SetSharedMemory;
typedef SetSharedMemory::iterator SetSharedMemoryIter;
typedef bool (ClearSharedMemoryCallBack)(const uint32 id, void* p);

class SharedMemoryManager : public SingletonBase<SharedMemoryManager>
{
private:
	SharedMemoryManager();
	friend class SingletonBase<SharedMemoryManager>;
public:
	~SharedMemoryManager();

	void* allSharedMemory(const char* filename);
	bool  allocSharedMemory(const uint32 id, const void* p, const uint32 size);
	void  freeSharedMemory();
	void* setSharedMemory(const uint32 id, bool& exist);
	const void* getSharedMemory(const uint32 id);
	bool randSharedMemory(const uint32 size, SetSharedMemory& setOut);
	void  clearSharedMemory(void* p);
	bool  wreteBackDB(ClearSharedMemoryCallBack cb);

	void final();
	template <typename T>
	bool execEveryEntry(Callback<T>& cb);
private:
	int m_fd;
	string m_filename;
	void* m_pShared_mem;
	uint32 m_share_size;
	uint32 m_max_share_num;
	uint32 m_share_num;
};

//模板的实现和声明必须放在同一个文件内
template <typename T>
bool SharedMemoryManager::execEveryEntry(Callback<T>& cb)
{
	for(uint32 i = 0; i < m_share_num; i++)
	{
		uint32 charid = *(uint32*)((char*)m_pShared_mem + i * sizeof(uint32));
		if(charid)
		{
			void* pTemp = (char*)m_pShared_mem + m_max_share_num * sizeof (uint32) + i * SHARE_TRUNK_SIZE ;
			T* pShared = (T*)pTemp;
			if(!cb.exec(pShared))
			{
				return false;
			}
		}
	}

	return true;
}

#endif