#include "type_define.h"
#include "logger.h"

static void ctrlHandler(int signum)
{
	DEBUG("catch signal value:%d", signum);
}

static void otherHandler(int signum)
{
	DEBUG("catch signal value:%d", signum);
}

//设置要捕获的信号
void SetSignedCatched()
{
	//设置要捕获的信号 调用ctrlHandler
	struct sigaction sig;
	sigemptyset(&sig.sa_mask);
	sig.sa_flags = 0;
	sig.sa_handler = ctrlHandler;
	sigaction(SIGINT, &sig, NULL);
	sigaction(SIGQUIT, &sig, NULL);
	sigaction(SIGABRT, &sig, NULL);
	sigaction(SIGTERM, &sig, NULL);

	//设置SIGHUP信号收到时只需 otherHandler
	sig.sa_handler = otherHandler;
	sigaction(SIGHUP, &sig, NULL);

	// 忽略PIPE信号
	sig.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sig, NULL);

}

