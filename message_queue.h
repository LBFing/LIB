#pragma once

#include <stdio.h>
#include <iostream>
#include <map>
#include <queue>
#include <stdlib.h>
#include <string.h>

using namespace std;

#define MAX_QUEUE_SIZE 1024

typedef pair<unsigned int, unsigned char *>  CmdPair;
typedef pair<volatile bool, CmdPair> CmdQueue;


class MsgQueue
{
public:
	MsgQueue();
	~MsgQueue();

	bool Put(unsigned int nLen, unsigned char *cmd);
	CmdPair *Get();
	void Erase();
private:
	void Clear();
	bool PutQueueToArray();
public:
	//private:
	CmdQueue m_cmd[MAX_QUEUE_SIZE];
	queue<CmdPair> m_queue;

	unsigned int m_cmd_read;
	unsigned int m_cmd_write;
};
