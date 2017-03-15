#include "share_memory.h"
#include "string_tool.h"
#include "logger.h"


SharedMemoryManager::SharedMemoryManager()
{
	m_fd = -1;
	m_pShared_mem = NULL;
	m_share_num = 0;

	m_max_share_num = PAGE_SIZE * 3;
	m_share_size = (SHARE_TRUNK_SIZE + sizeof(uint32)) * m_max_share_num;
}

SharedMemoryManager::~SharedMemoryManager()
{
	final();
}

void* SharedMemoryManager::allSharedMemory(const char* filename)
{
	StringTool::Format(m_filename, "%s.%lu", filename, ::getuid());
	//创建或打开一个共享内存,成功返回一个整数的文件描述符，错误返回-1
	m_fd = ::shm_open(m_filename.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if(m_fd == -1)
	{
		printf("打开文件失败 file:%s errno:%s\n", m_filename.c_str(), strerror(errno));
		return NULL;
	}

	struct stat sb;
	//获取文件状态信息
	if(::fstat(m_fd, &sb) == -1)
	{
		printf("获取文件信息失败 errno:%s\n", strerror(errno));
		shm_unlink(m_filename.c_str());
		return NULL;
	}

	if (uint32(sb.st_size) != m_share_size)
	{
		//设置文件长度
		int32 nLen = ftruncate(m_fd, m_share_size);
		if (nLen == -1)
		{
			printf("设置文件长度失败 file:%s cur_size:%lu set_size:%u errno:%s\n", m_filename.c_str(), sb.st_size, m_share_size, strerror(errno));
			shm_unlink(m_filename.c_str());
			return NULL;
		}
	}

	//将一个文件或者其它对象映射进内存
	m_pShared_mem = ::mmap(0, m_share_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
	if(m_pShared_mem == MAP_FAILED)
	{
		printf("初始化共享内存失败:%s,%s\n", m_filename.c_str(), strerror(errno));
		shm_unlink(m_filename.c_str());
		return NULL;
	}


	for(uint32 i = 0; i < m_max_share_num; i++)
	{
		uint32 charid = *(uint32*)((char*)m_pShared_mem + i * sizeof(uint32));
		if(charid)
		{
			m_share_num++;
		}
	}
	return m_pShared_mem;
}

bool SharedMemoryManager::allocSharedMemory(const uint32 id, const void* p, const uint32 size)
{
	for(uint32 i = 0; i < m_share_num; i++)
	{
		uint32 charid = *(uint32*)((char*)m_pShared_mem + i * sizeof(uint32));
		if (charid == 0) //没有共享数据
		{
			//设置新的共享数据
			*(uint32*)((char*)m_pShared_mem + i * sizeof(uint32)) = id;
			void* pShared = (char*)m_pShared_mem + m_max_share_num * sizeof (uint32) + i * SHARE_TRUNK_SIZE ;
			if(pShared == NULL)
			{
				printf("SharedMemory is NULL\n");
				return false;
			}
			else
			{
				//内存拷贝
				memcpy(pShared, p, size);
			}
			return true;
		}
	}

	if(m_share_num >= m_max_share_num)
	{
		printf("共享内存空间已满 分配失败 CurSize:%u\n", m_share_num);
		return false;
	}

	*(uint32*)((char*)m_pShared_mem + m_share_num * sizeof(uint32)) = id;
	//设置新的共享数据
	void* pShared = (char*)m_pShared_mem + m_max_share_num * sizeof (uint32) + m_share_num * SHARE_TRUNK_SIZE ;
	if(pShared == NULL)
	{
		printf("SharedMemory is NULL\n");
		return false;
	}
	else
	{
		//内存拷贝
		memcpy(pShared, p, size);
		m_share_num++;
	}
	return true;
}

void SharedMemoryManager::freeSharedMemory()
{
	if(m_pShared_mem)
	{
		munmap(m_pShared_mem, m_share_size);
		shm_unlink(m_filename.c_str());
		m_pShared_mem = NULL;
		m_share_num = 0;
	}
}
void* SharedMemoryManager::setSharedMemory(const uint32 id, bool& exist)
{
	for(uint32 i = 0; i < m_share_num; i++)
	{
		uint32 charid = *(uint32*)((char*)m_pShared_mem + i * sizeof(uint32));
		if (charid == id) //找到共享数据
		{
			printf("已存在于内存中 Id:%u\n", id);
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
			cb(i, pShared);
			*(uint32*)((char*)m_pShared_mem + i * sizeof(uint32)) = 0;
		}
	}
	return true;
}

void SharedMemoryManager::final()
{
	freeSharedMemory();
}

