#ifndef __TCP_CONNECTION_H__
#define __TCP_CONNECTION_H__

#include "type_define.h"
#include "callback.h"
#include "buffer_ex.h"
#include "inet_address.h"
#include "any.h"
#include "nocopyable.h"


class Channel;
class EventLoop;
class Socket;

class TcpConnection : private Nocopyable, public std::enable_shared_from_this < TcpConnection >
{
public:

	TcpConnection(EventLoop* loop, const string& name, int32 sockfd, const InetAddress& local_addr, const InetAddress& peer_addr);
	~TcpConnection();

	EventLoop* GetLoop() const { return m_pLoop; }
	const string& GetName() const { return m_strName; }

	const InetAddress& GetLocalAddr() const { return m_localAddr; }
	const InetAddress& GetPeerAddr() const { return m_peerAddr; }

	bool Connected() const { return m_state == kConnected; }
	bool Disconnected() const { return m_state == kDisconnected; }

	bool GetTcpInfo(struct tcp_info* tcpinfo) const;
	string GetTcpInfoString() const;

	void Send(const char* message, int32 len);
	void Send(const string& message);

	void Send(BufferEx* message);
	void Shutdown();

	void ForceClose();
	void ForceCloseWithDelay(double seconds);

	void SetTcpNoDelay(bool on);

	void StartRead();
	void StopRead();

	bool IsReading()const { return m_reading; }


	void SetContext(const any& context)
	{
		m_context = context;
	}

	const any& GetContext() const { return m_context; }

	any* GetMutableContext() { return &m_context; }

	void SetConnectionCallback(const ConnectionCallback& cb)
	{
		m_connectionCallback = cb;
	}

	void SetMessageCallback(const MessageCallback& cb)
	{
		m_messageCallback = cb;
	}

	void SetWriteCompleteCallback(const WriteCompleteCallback& cb)
	{
		m_writeCompleteCallback = cb;
	}

	void SetHighWaterMarkCallback(const HighWaterMarkCallback& cb, size_t highWaterMark)
	{
		m_highWaterMarkCallback = cb;
		m_highWaterMark = highWaterMark;
	}

	BufferEx* GetInputBuffer()
	{
		return &m_InPutBuff;
	}

	BufferEx* GetOutputBuffer()
	{
		return &m_OutPutBuff;
	}

	void SetCloseCallback(const CloseCallback& cb)
	{
		m_closeCallback = cb;
	}

	void ConnectEstablished();

	void ConnectDestroyed();

private:
	enum STATEE { kDisconnected, kConnecting, kConnected, kDisconnecting };


	void handleRead(Timestamp receiveTime);
	void handleWrite();
	void handleClose();
	void handleError();

	void sendInLoop(const void* message, size_t len);
	void shutdownInLoop();

	void forceCloseInLoop();
	void setState(STATEE s) { m_state = s; }

	const char* stateToString() const;

	void startReadInLoop();
	void stopReadInLoop();

	EventLoop* m_pLoop;
	const string m_strName;

	STATEE m_state;
	bool m_reading;

	std::unique_ptr<Socket> m_socket;
	std::unique_ptr<Channel> m_channel;

	const InetAddress m_localAddr;
	const InetAddress m_peerAddr;

	ConnectionCallback m_connectionCallback;
	MessageCallback m_messageCallback;
	WriteCompleteCallback m_writeCompleteCallback;
	HighWaterMarkCallback m_highWaterMarkCallback;
	CloseCallback m_closeCallback;

	size_t m_highWaterMark;
	BufferEx m_InPutBuff;
	BufferEx m_OutPutBuff;
	any m_context;
};


#endif
