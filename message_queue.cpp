#include "message_queue.h"

MsgQueue::MsgQueue()
{
	m_cmd_write = 0;
	m_cmd_read = 0;
}
MsgQueue::~MsgQueue()
{
	Clear();
}

bool MsgQueue::Put(unsigned int nLen, unsigned char *cmd)
{
	unsigned char *cmd_buf = new unsigned char [nLen];
	if(cmd_buf)
	{
		//bcopy(cmd, cmd_buf, nLen);
		memcpy(cmd_buf, cmd, nLen);
		if(PutQueueToArray() && m_cmd[m_cmd_write].first == false)
		{
			m_cmd[m_cmd_write].first = true;
			m_cmd[m_cmd_write].second.first = nLen;
			m_cmd[m_cmd_write].second.second = cmd_buf;
			m_cmd_write = (m_cmd_write + 1) % MAX_QUEUE_SIZE;
		}
		else
		{
			m_queue.push(make_pair(nLen, cmd_buf));
		}
		return true;
	}
	else
	{
		return false;
	}
}
CmdPair *MsgQueue::Get()
{
	CmdPair *cmd_pair = NULL;
	if(m_cmd[m_cmd_read].first)
	{
		cmd_pair =  &m_cmd[m_cmd_read].second;
	}

	if(cmd_pair == NULL && m_queue.size() > 0)
	{
		PutQueueToArray();
	}

	if(m_cmd[m_cmd_read].first)
	{
		cmd_pair =  &m_cmd[m_cmd_read].second;
	}

	return cmd_pair;
}
void MsgQueue::Erase()
{
	delete m_cmd[m_cmd_read].second.second;
	m_cmd[m_cmd_read].second.second = NULL;
	m_cmd[m_cmd_read].first = false;
	m_cmd_read = (m_cmd_read + 1) % MAX_QUEUE_SIZE;
}
void MsgQueue::Clear()
{
	while(m_cmd[m_cmd_read].first)
	{
		Erase();
	}

	while(!m_queue.empty())
	{
		delete m_queue.front().second;
		m_queue.front().second = NULL;
		m_queue.pop();
	}
	m_cmd_write = 0;
	m_cmd_read = 0;
}

bool MsgQueue::PutQueueToArray()
{
	bool result = true;
	while(!m_queue.empty())
	{
		if(m_cmd[m_cmd_write].first == false)
		{
			m_cmd[m_cmd_write].first = true;
			m_cmd[m_cmd_write].second = m_queue.front();
			m_cmd_write = (m_cmd_write + 1) % MAX_QUEUE_SIZE;
			m_queue.pop();
		}
		else
		{
			result = false;
			break;
		}
	}
	return result;
}