
#include "logger.h"

void TestLogger()
{
	InitLogger("/root/study/LIB/log/test.log", "DEBUG");
	DEBUG("++++++++++++++++++++++");
	WARN("++++++++++++++++++++++");
	INFO("++++++++++++++++++++++");
	ERROR("++++++++++++++++++++++");
}

int main(int argc, char const* argv[])
{
	TestLogger();
	return 0;
}
