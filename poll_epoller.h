#ifndef __POLL_EPOLLER_H__
#define  __POLL_EPOLLER_H__
#include "poller.h"


class EPollPoller : public Poller
{
public:
	EPollPoller(EventLoop* loop);
	virtual ~EPollPoller();

	virtual Timestamp Poll(int32 timeOuts, ChannelVec* activeChannels);
	virtual void UpdateChannel(Channel* channel);
	virtual void RemoveChannel(Channel* channel);

private:
	static const int kInitEventListSize = 16;

	static const char* operationToString(int op);

	void fillActiveChannels(int numEvents, ChannelVec* activeChannels) const;
	void update(int operation, Channel* channel);

	typedef std::vector<struct epoll_event> EventVec;

	int m_epollfd;
	EventVec m_vecEvent;

};


#endif
