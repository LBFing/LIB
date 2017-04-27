#include "tcp_connection.h"
#include "logger.h"
#include "channel.h"
#include "event_loop.h"
#include "socket_ops.h"

void DefaultConnectionCallback(const TcpConnectionPtr& conn)
{
	DEBUG("%s -> %s is Connect :%d", conn->GetLocalAddr().ToIPPort().c_str(), conn->GetPeerAddr().ToIPPort().c_str(), conn->Connected());
}


void DefaultMessageCallback(const TcpConnectionPtr&, BufferEx* buf, Timestamp)
{
	buf->RetrieveAll();
}

TcpConnection::TcpConnection(EventLoop* loop, const string& name, int32 sockfd, const InetAddress& local_addr, const InetAddress& peer_addr)
	: m_pLoop(loop)
	, m_strName(name)
	, m_state(kConnecting)
	, m_reading(true)
	, m_socket(new Socket(sockfd))
	, m_channel(new Channel(loop, sockfd))
	, m_localAddr(local_addr)
	, m_peerAddr(peer_addr)
{
	m_channel->SetReadCallBack(std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
	m_channel->SetWriteCallBack(std::bind(&TcpConnection::handleWrite, this));
	m_channel->SetCloseCallBack(std::bind(&TcpConnection::handleClose, this));
	m_channel->SetErrorCallBack(std::bind(&TcpConnection::handleError, this));
	DEBUG("TcpConnection::ctor[ %s ] at %p fd=%d", m_strName.c_str(), this, sockfd);
	m_socket->SetKeepAlive(true);
}

TcpConnection::~TcpConnection()
{
	DEBUG("TcpConnection::dctor[ %s ] at %p fd=%d state=%s", m_strName.c_str(), this, m_channel->GetFd(), stateToString());
	assert(m_state == kDisconnected);
}

bool TcpConnection::GetTcpInfo(struct tcp_info* tcpinfo) const
{
	return m_socket->GetTcpInfo(tcpinfo);
}

std::string TcpConnection::GetTcpInfoString() const
{
	char buf[1024] = { 0 };
	m_socket->GetTcpInfoString(buf, sizeof(buf));
	return buf;
}

void TcpConnection::Send(const char* message, int32 len)
{
	if (m_state == kConnected)
	{
		if (m_pLoop->IsInLoopThread())
		{
			sendInLoop(message, len);
		}
		else
		{
			m_pLoop->RunInLoop(std::bind(&TcpConnection::sendInLoop, this, message, len));
		}
	}
}

void TcpConnection::Send(const string& message)
{
	Send(message.c_str(), message.length());
}


void TcpConnection::sendInLoop(const void* message, size_t len)
{
	m_pLoop->AssertInLoopThread();
	ssize_t nwrote = 0;
	size_t remaining = len;
	bool faultError = false;

	if (m_state == kDisconnected)
	{
		WARN("disconnected give up writing");
		return;
	}

	if (!m_channel->IsWriting() && m_OutPutBuff.ReadableBytes() == 0)
	{
		nwrote = Write(m_channel->GetFd(), message, len);
		if (nwrote >= 0 )
		{
			remaining = len - nwrote;
			if (remaining == 0 && m_writeCompleteCallback)
			{
				m_pLoop->QueueInLoop(std::bind(m_writeCompleteCallback, shared_from_this()));
			}
		}
		else
		{
			nwrote = 0;
			if (nwrote != EWOULDBLOCK)
			{
				ERROR("TcpConnection sendInLoop");
				if (errno == EPIPE || errno == ECONNRESET)
				{
					faultError = true;
				}
			}
		}
	}
	assert(remaining <= len);
	if (!faultError && remaining > 0)
	{
		size_t oldlen = m_OutPutBuff.ReadableBytes();
		if (oldlen + remaining >= m_highWaterMark &&
		        oldlen < m_highWaterMark &&
		        m_highWaterMarkCallback)
		{
			m_pLoop->QueueInLoop(std::bind(m_highWaterMarkCallback, shared_from_this(), oldlen + remaining));
		}
		m_OutPutBuff.append(static_cast<const char*>(message) + nwrote, remaining);
		if (!m_channel->IsWriting())
		{
			m_channel->EnableWriting();
		}
	}
}


void TcpConnection::Shutdown()
{
	if (m_state == kConnected)
	{
		setState(kDisconnecting);
		m_pLoop->RunInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
	}
}


void TcpConnection::shutdownInLoop()
{
	m_pLoop->AssertInLoopThread();
	if (!m_channel->IsWriting())
	{
		m_socket->ShutdownWrite();
	}
}

void TcpConnection::ForceClose()
{
	if (m_state == kConnected || m_state == kDisconnecting)
	{
		setState(kDisconnecting);
		m_pLoop->QueueInLoop(std::bind(&TcpConnection::forceCloseInLoop, shared_from_this()));
	}
}

void TcpConnection::ForceCloseWithDelay(double seconds)
{
	if (m_state == kConnected || m_state == kDisconnecting)
	{
		setState(kDisconnecting);
		m_pLoop->RunAfter(seconds, makeWeakCallback(shared_from_this(), &TcpConnection::ForceClose));
	}
}

void TcpConnection::forceCloseInLoop()
{
	m_pLoop->AssertInLoopThread();
	if (m_state == kConnected || m_state == kDisconnected)
	{
		handleClose();
	}
}

const char* TcpConnection::stateToString() const
{
	switch (m_state)
	{
		case kDisconnected:
			return "kDisconnected";
		case kConnecting:
			return "kConnecting";
		case kConnected:
			return "kConnected";
		case kDisconnecting:
			return "kDisconnecting";
		default:
			return "unknown state";
	}
}

void TcpConnection::SetTcpNoDelay(bool on)
{
	m_socket->SetTcpNoDelay(on);
}

void TcpConnection::StartRead()
{
	m_pLoop->RunInLoop(std::bind(&TcpConnection::stopReadInLoop, this));
}

void TcpConnection::startReadInLoop()
{
	m_pLoop->AssertInLoopThread();
	if (!m_reading || !m_channel->IsReading())
	{
		m_channel->EnableReading();
		m_reading = true;
	}
}

void TcpConnection::StopRead()
{
	m_pLoop->RunInLoop(std::bind(&TcpConnection::stopReadInLoop, this));
}

void TcpConnection::stopReadInLoop()
{
	m_pLoop->AssertInLoopThread();
	if (m_reading || m_channel->IsReading())
	{
		m_channel->DisableReading();
		m_reading = false;
	}
}

void TcpConnection::ConnectEstablished()
{
	m_pLoop->AssertInLoopThread();
	assert(m_state == kConnecting);
	setState(kConnected);
	m_channel->Tie(shared_from_this());
	m_channel->EnableReading();
	m_connectionCallback(shared_from_this());
}

void TcpConnection::ConnectDestroyed()
{
	m_pLoop->AssertInLoopThread();
	if (m_state == kConnected)
	{
		setState(kDisconnected);
		m_channel->DisableAll();
		m_connectionCallback(shared_from_this());
	}
	m_channel->Remove();
}

void TcpConnection::handleRead(Timestamp receiveTime)
{
	m_pLoop->AssertInLoopThread();
	int32 savedError = 0;
	ssize_t n = m_InPutBuff.ReadFd(m_channel->GetFd(), &savedError);
	if (n > 0)
	{
		m_messageCallback(shared_from_this(), &m_InPutBuff, receiveTime);
	}
	else if (n == 0)
	{
		handleClose();
	}
	else
	{
		errno = savedError;
		ERROR("TcpConnect::handleRead");
		handleError();
	}
}

void TcpConnection::handleWrite()
{
	m_pLoop->AssertInLoopThread();
	if (m_channel->IsWriting())
	{
		ssize_t n = ::Write(m_channel->GetFd(), m_OutPutBuff.Peek(), m_OutPutBuff.ReadableBytes());
		if (n > 0)
		{
			m_OutPutBuff.Retrieve(n);
			if (m_OutPutBuff.ReadableBytes() == 0);
			{
				m_channel->DisableWriting();
				if (m_writeCompleteCallback)
				{
					m_pLoop->QueueInLoop(std::bind(m_writeCompleteCallback, shared_from_this()));
				}
				if (m_state == kDisconnecting)
				{
					shutdownInLoop();
				}
			}
		}
		else
		{
			ERROR("TcpConnection::handleWrite");
		}
	}
	else
	{
		DEBUG("Connection fd = %d is down, no more writing", m_channel->GetFd());
	}
}

void TcpConnection::handleClose()
{
	m_pLoop->AssertInLoopThread();
	DEBUG("Fd=%d status=%s", m_channel->GetFd(), stateToString());
	assert(m_state == kConnected || m_state == kDisconnecting);
	setState(kDisconnecting);
	m_channel->DisableAll();
	TcpConnectionPtr guardThis(shared_from_this());
	m_connectionCallback(guardThis);
	m_closeCallback(guardThis);
}

void TcpConnection::handleError()
{
	int32 err = GetSocketError(m_channel->GetFd());
	ERROR("TcpConnection::handleError [ %s ] - SO_ERROR = %d", m_strName.c_str(), err);
}







