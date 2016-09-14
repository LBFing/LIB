#pragma once

#include "type_define.h"

#define MAX_QUEUE_SIZE 1024
typedef pair<unsigned int, unsigned char*>  CmdPair;
typedef pair<volatile bool, CmdPair> CmdQueue;

class MsgQueue
{
public:
	MsgQueue();
	~MsgQueue();

	bool Put(unsigned int nLen, unsigned char* cmd);
	CmdPair* Get();
	void Erase();
private:
	void Clear();
	bool PutQueueToArray();
public:
	//private:
	CmdQueue m_cmd[MAX_QUEUE_SIZE];
	queue<CmdPair> m_queue;

	uint32 m_cmd_read;
	uint32 m_cmd_write;
};
