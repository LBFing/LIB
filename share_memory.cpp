#include "share_memory.h"
#include "string_tool.h"
#include "logger.h"

#define PAGE_SIZE			256		//定义页面大小
#define SHARE_TRUNK_SIZE	24800	//定义每个共享对象大小占用的字节
//共享内存设置模式前面为ID 后面Wie数据
//前面设置为数据ID1ID2 。。。。Data1Data2 。。。。。

SharedMemoryManager::SharedMemoryManager()
{
	m_fd = -1;
	m_pShared_mem = NULL;
	m_share_num = 0;

	m_max_share_num = PAGE_SIZE * 3;
	m_share_size = (SHARE_TRUNK_SIZE + sizeof(uint32)) * m_max_share_num;
}


void* SharedMemoryManager::allSharedMemory(const char* filename)
{
	StringTool::format(m_filename, "%s.%lu", filename, ::getuid());
	//创建或打开一个共享内存,成功返回一个整数的文件描述符，错误返回-1
	m_fd = ::shm_open(m_filename.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if(m_fd == -1)
	{
		ERROR("打开文件失败 file:%s errno:%s", m_filename.c_str(), strerror(errno));
		return NULL;
	}

	struct stat sb;
	//获取文件状态信息
	if(::fstat(m_fd, &sb) == -1)
	{
		ERROR("获取文件信息失败 errno:%s", strerror(errno));
		shm_unlink(m_filename.c_str());
		return NULL;
	}

	if (uint32(sb.st_size) != m_share_size)
	{
		//设置文件长度
		int32 nLen = ftruncate(m_fd, m_share_size);
		if (nLen == -1)
		{
			ERROR("设置文件长度失败 file:%s cur_size:%lu set_size:%u errno:%s", m_filename.c_str(), sb.st_size, m_share_size, strerror(errno));
			shm_unlink(m_filename.c_str());
			return NULL;
		}
	}

	//将一个文件或者其它对象映射进内存
	m_pShared_mem = ::mmap(0, m_share_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
	if(m_pShared_mem == MAP_FAILED)
	{
		ERROR("初始化共享内存失败:%s,%s", m_filename.c_str(), strerror(errno));
		shm_unlink(m_filename.c_str());
		return NULL;
	}
	return m_pShared_mem;
}

void SharedMemoryManager::freeSharedMemory()
{
	if(m_pShared_mem)
	{
		munmap(m_pShared_mem, m_share_size);
		shm_unlink(m_filename.c_str());
		m_pShared_mem = NULL;
	}
}
void* SharedMemoryManager::setSharedMemory(const uint32 id, bool& exist)
{
	for(uint32 i = 0; i < m_share_num; i++)
	{
		uint32 charid = *(uint32*)((char*)m_pShared_mem + i * sizeof(uint32));
		if (charid == id) //找到共享数据
		{
			DEBUG("已存在于内存中 Id：%u", id);
			exist = true;
			void* pShared = (char*)m_pShared_mem + m_max_share_num * sizeof (uint32) + i * SHARE_TRUNK_SIZE ;
			return pShared;
		}
		if (charid == 0) //没有共享数据
		{
			//设置新的共享数据
			*(uint32*)((char*)m_pShared_mem + i * sizeof(uint32)) = id;
			void* pShared = (char*)m_pShared_mem + m_max_share_num * sizeof (uint32) + i * SHARE_TRUNK_SIZE ;
			return pShared;
		}
	}
	return NULL;
}
const void* SharedMemoryManager::getSharedMemory(const uint32 id)
{
	for(uint32 i = 0; i < m_share_num; i++)
	{
		uint32 charid = *(uint32*)((char*)m_pShared_mem + i * sizeof(uint32));
		if(charid == id)
		{
			void* pShared = (char*)m_pShared_mem + m_max_share_num * sizeof (uint32) + i * SHARE_TRUNK_SIZE ;
			return pShared;
		}
	}
	return NULL;
}

bool SharedMemoryManager::randSharedMemory(const uint32 size, SetSharedMemory& setOut)
{
	vector<void*>vec;
	for(uint32 i = 0; i < m_share_num; i++)
	{
		uint32 charid = *(uint32*)((char*)m_pShared_mem + i * sizeof(uint32));
		if(charid)
		{
			void* pShared = (char*)m_pShared_mem + m_max_share_num * sizeof (uint32) + i * SHARE_TRUNK_SIZE ;
			vec.push_back(pShared);
		}
	}

	if(vec.empty())
	{
		return false;
	}

	uint32 i = 0;
	while (i < size && i < vec.size())
	{
		uint32 index = randBetween(0, vec.size() - 1);
		void* pShare = vec[index];

		if (setOut.insert(pShare).second)
		{
			i++;
		}
	}
	return i == size;
}
void SharedMemoryManager::clearSharedMemory(void* p)
{
	ptrdiff_t diff =  (ptrdiff_t)((char*)p - ((char*)m_pShared_mem + m_max_share_num * sizeof(uint32)));
	uint32 index = diff / SHARE_TRUNK_SIZE;
	*(uint32*)((char*)m_pShared_mem + index * sizeof(uint32)) = 0;
}

bool SharedMemoryManager::wreteBackDB(ClearSharedMemoryCallBack cb)
{
	for(uint32 i = 0; i < m_share_num; i++)
	{
		uint32 charid = *(uint32*)((char*)m_pShared_mem + i * sizeof(uint32));
		if(charid)
		{
			void* pShared = (char*)m_pShared_mem + m_max_share_num * sizeof (uint32) + i * SHARE_TRUNK_SIZE ;
			cb(i,pShared);
			*(uint32*)((char*)m_pShared_mem + i * sizeof(uint32)) = 0;
		}
	}
	return true;
}

void SharedMemoryManager::final()
{
	freeSharedMemory();
}

bool SharedMemoryManager::execEveryEntry(Callback<void>& cb)
{
	for(uint32 i = 0; i < m_share_num; i++)
	{
		uint32 charid = *(uint32*)((char*)m_pShared_mem + i * sizeof(uint32));
		if(charid)
		{
			void* pShared = (char*)m_pShared_mem + m_max_share_num * sizeof (uint32) + i * SHARE_TRUNK_SIZE ;
			if(!cb.exec(pShared))
			{
				return false;
			}
		}
	}

	return true;
}
