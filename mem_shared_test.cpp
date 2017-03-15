#include "share_memory.h"

struct SharedST
{
	SharedST()
	{
		memset(this, 0, sizeof(SharedST));
	}
	uint32 id;
	uint32 num;
	char name[20];
};


void TestShareMemeory()
{
	SharedMemoryManager::newInstance();
	SharedMemoryManager::getInstance().allSharedMemory("shared.data");
	for (uint32 i = 1 ; i <= 20; i++)
	{
		SharedST* pTemp = new SharedST();
		if(pTemp)
		{
			pTemp->id = i;
			pTemp->num = 100 * i;
			sprintf(pTemp->name, "name_%d", i);
			SharedMemoryManager::getInstance().allocSharedMemory(i, pTemp, sizeof(SharedST));
			delete pTemp;
		}
	}
	struct SharedMemCB : public Callback<SharedST>
	{
		SharedMemCB() { count = 0 ;}
		bool exec(SharedST* pShared)
		{
			if (pShared == NULL)
			{
				return false;
			}

			printf("Id:%d	Num:%d	Name:%s \n", pShared->id, pShared->num, pShared->name);
			count++;
			return true;
		}
		uint32 count;
	};

	SharedMemCB cb;
	SharedMemoryManager::getInstance().execEveryEntry(cb);
	printf("SharedMemory Count:%d\n", cb.count);

	bool isExist = false;
	SharedST* pShared = (SharedST*)SharedMemoryManager::getInstance().setSharedMemory(15, isExist);
	printf("Id:%d	Num:%d	Name:%s\n", pShared->id, pShared->num, pShared->name);

	SharedMemoryManager::delInstance();
}


__thread uint32 seedp;
int main(int argc, char const* argv[])
{
	seedp = (uint32)time(NULL);
	TestShareMemeory();
	return 0;
}