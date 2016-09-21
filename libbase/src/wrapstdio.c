#include "base.h"

//fgets 读取一行数据
//从stream 中读取至少1 但是小于n个字符到 ptr指针中
//当遇到EOF或者新的一行数据时停止，并追加一个 '\0'
//到末尾中，注意换行符会别读入并保持
char* Fgets(char* ptr, int n, FILE* stream)
{
	if(ptr == NULL || stream == NULL)
	{
		err_sys("Fgets ptr or FILE* is NULL");
		return NULL;
	}

	char* rptr;
	if( (rptr = fgets(ptr, n, stream)) == NULL && ferror(stream))
	{
		err_sys("fgets error");
	}
	return rptr;
}

//write 写入数据
//write()会把指针buf所指的内存写入count个字节到参数fd所指的文件内
//如果顺利write()会返回实际写入的字节数。当有错误发生时则返回-1，错误代码存入errno中
void Write(int fd, void* ptr, size_t nbytes)
{
	if (write(fd, ptr, nbytes) != nbytes)
	{
		err_sys("write error");
	}
}

//read()会把参数fd所指的文件传送nbyte个字节到buf指针所指的内存中
//若参数nbyte为0，则read()不会有作用并返回0。返回值为实际读取到的字节数，如果返回0，
//表示已到达文件尾或无可读取的数据。错误返回-1,并将根据不同的错误原因适当的设置错误码。
ssize_t Read(int fd, void* ptr, size_t nbytes)
{
	ssize_t		n;

	if ( (n = read(fd, ptr, nbytes)) == -1)
	{
		err_sys("read error");
	}
	return(n);
}

//close 关闭描述符 出差时返回-1
void Close(int fd)
{
	if (close(fd) == -1)
	{
		err_sys("close error");
	}
}

//pipe 创建管道
//参数数组包含pipe使用的两个文件的描述符。fd[0]:读管道，fd[1]:写管道
//成功，返回0，否则返回-1
void Pipe(int* fds)
{
	if (pipe(fds) < 0)
	{
		err_sys("pipe error");
	}
}


//waitpid()会暂时停止目前进程的执行，直到有信号来到或子进程结束。
//如果在调用 waitpid()时子进程已经结束,则 waitpid()会立即返回子进程结束状态值
pid_t Waitpid(pid_t pid, int* iptr, int options)
{
	pid_t	retpid;

	if ( (retpid = waitpid(pid, iptr, options)) == -1)
	{
		err_sys("waitpid error");
	}
	return(retpid);
}

//fork 函数通过系统调用创建一个与原来进程几乎完全相同的进程
pid_t Fork(void)
{
	pid_t	pid;

	if ( (pid = fork()) == -1)
	{ err_sys("fork error"); }
	return(pid);
}

