#include "socket_ops.h"
#include "logger.h"

void FromIpPort(const char* ip, uint16 port, struct sockaddr_in* addr)
{
	addr->sin_family = AF_INET;
	addr->sin_port = HostToNetwork16(port);
	if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
	{
		ERROR("FromIpPort");
	}
}

void FromIpPort(const char* ip, uint16_t port, struct sockaddr_in6* addr)
{
	addr->sin6_family = AF_INET6;
	addr->sin6_port = HostToNetwork16(port);
	if (::inet_pton(AF_INET6, ip, &addr->sin6_addr) <= 0)
	{
		ERROR("FromIpPort");
	}
}

void SockToIPPort(char* buf, size_t size, const struct sockaddr* addr)
{

}


void SockToIP(char* buf, size_t size, const struct sockaddr* addr)
{

}
