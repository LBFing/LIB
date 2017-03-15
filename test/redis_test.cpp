#include "redis_command.h"

void TestRedisCommand()
{
	Redis* r = new Redis();
	if(!r->connect("127.0.0.1", 6379))
	{
		printf("redis connect error!\n");
		return;
	}
	printf("redis connect success!\n");
	r->set("name", "Mayuyu");
	printf("Redis name:%s\n", r->get("name").c_str());
	delete r;
}

int main(int argc, char const* argv[])
{
	TestRedisCommand();
	return 0;
}