#ifndef __SOCKET_OPS_H__
#define __SOCKET_OPS_H__

#include "type_define.h"


inline uint64 HostToNetwork64(uint64 host64)
{
	return htobe64(host64);
}

inline uint32 HostToNetwork32(uint32 host32)
{
	return htobe32(host32);
}

inline uint16 HostToNetwork16(uint16 host16)
{
	return htobe16(host16);
}

inline uint64 NetworkToHost64(uint64 net64)
{
	return be64toh(net64);
}

inline uint32 NetworkToHost32(uint32 net32)
{
	return be32toh(net32);
}

inline uint16 NetworkToHost16(uint16 net16)
{
	return be16toh(net16);
}
void FromIpPort(const char* ip, uint16 port, struct sockaddr_in* addr);
void FromIpPort(const char* ip, uint16_t port, struct sockaddr_in6* addr);
void SockToIPPort(char* buf, size_t size, const struct sockaddr* addr);
void SockToIP(char* buf, size_t size, const struct sockaddr* addr);
#endif