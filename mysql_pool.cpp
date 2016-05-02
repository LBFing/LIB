#include "mysql_pool.h"

MysqlPool::MysqlPool(int mMaxHandle)
{
	m_max_handle = mMaxHandle;
	printf("Version of the mysql libs is %s\n" , mysql_get_client_info());
	if(!mysql_thread_safe())
	{
		printf("The mysql libs is not thread safe...\n");
	}
}

MysqlPool::~MysqlPool()
{

}
