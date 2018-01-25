#ifndef MUDUO_EXAMPLES_ASIO_CHAT_CODEC_H
#define MUDUO_EXAMPLES_ASIO_CHAT_CODEC_H

#include <muduo/base/Logging.h>
#include <muduo/net/Buffer.h>
#include <muduo/net/Endian.h>
#include <muduo/net/TcpConnection.h>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

class LengthHeaderCodec : boost::noncopyable
{
 public:
  typedef boost::function<void (const muduo::net::TcpConnectionPtr&,
                                const muduo::string& message,
                                muduo::Timestamp)> StringMessageCallback;

  explicit LengthHeaderCodec(const StringMessageCallback& cb)//StringMessageCallback come from ChatServer::onStringMessage
    : messageCallback_(cb)
  {
  }

/*
1.TcpServer在message抵达时会调用LengthHeaderCodec::onMessage()来解析出string meassge，
然后再回调ChatServer::onStringMessage()来处理并打印message

2.LengthHeaderCodec::onMessage(...)何时何地被调用，参数怎样传入的？？实质是在
TcpConnection::handleRead(Timestamp receiveTime)中调用的
messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);

与LengthHeaderCodec::onMessage(const muduo::net::TcpConnectionPtr& conn,
                          muduo::net::Buffer* buf,
                          muduo::Timestamp receiveTime)
的参数正好匹配。

*/
  void onMessage(const muduo::net::TcpConnectionPtr& conn,
                 muduo::net::Buffer* buf,
                 muduo::Timestamp receiveTime)
  {
    while (buf->readableBytes() >= kHeaderLen) // kHeaderLen == 4  message header已完全可读
    {
      // FIXME: use Buffer::peekInt32()
      const void* data = buf->peek();
      int32_t be32 = *static_cast<const int32_t*>(data); // SIGBUS
      const int32_t len = muduo::net::sockets::networkToHost32(be32);
      if (len > 65536 || len < 0)
      {
        LOG_ERROR << "Invalid length " << len;
        conn->shutdown();  // FIXME: disable reading
        break;
      }
      else if (buf->readableBytes() >= len + kHeaderLen)//buf中有一条完整的message后再进行buf->retrieve操作
      {
        buf->retrieve(kHeaderLen);
        muduo::string message(buf->peek(), len);
        messageCallback_(conn, message, receiveTime);//ChatServer::onStringMessage(conn, message, receiveTime);
        buf->retrieve(len);
      }
      else//message header不可读
      {
        break;
      }
    }
  }

  // FIXME: TcpConnectionPtr
  void send(muduo::net::TcpConnection* conn,
            const muduo::StringPiece& message)
  {
    muduo::net::Buffer buf;
    buf.append(message.data(), message.size());
    int32_t len = static_cast<int32_t>(message.size());
    int32_t be32 = muduo::net::sockets::hostToNetwork32(len);
    buf.prepend(&be32, sizeof be32);
    conn->send(&buf);
  }

 private:
  StringMessageCallback messageCallback_;//获取一条完整的message后(由LengthHeaderCodec解析出完整的message)，回调messageCallback_ (即ChatServer::onStringMessage)
  const static size_t kHeaderLen = sizeof(int32_t);//4 bytes
};

#endif  // MUDUO_EXAMPLES_ASIO_CHAT_CODEC_H
