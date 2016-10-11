#pragma once

#include "type_define.h"
#include "singleton.h"
#include "entry.h"

typedef set<void*> SetSharedMemory;
typedef SetSharedMemory::iterator SetSharedMemoryIter;
typedef bool (ClearSharedMemoryCallBack)(const uint32 id, void* p);

class SharedMemoryManager : public SingletonBase<SharedMemoryManager>
{
private:
	SharedMemoryManager();
	friend class SingletonBase<SharedMemoryManager>;
public:
	~SharedMemoryManager() {};

	void* allSharedMemory(const char* filename);
	void  freeSharedMemory();
	void* setSharedMemory(const uint32 id,bool& exist);
	const void* getSharedMemory(const uint32 id);
	bool randSharedMemory(const uint32 size,SetSharedMemory& setOut);
	void  clearSharedMemory(void* p);
	bool  wreteBackDB(ClearSharedMemoryCallBack cb);

	void final();
	bool execEveryEntry(Callback<void>& cb);
private:
	int m_fd;
	string m_filename;
	void* m_pShared_mem;
	uint32 m_share_size;
	uint32 m_max_share_num;
	uint32 m_share_num;
};
