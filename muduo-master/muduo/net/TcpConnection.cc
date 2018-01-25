// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include <muduo/net/TcpConnection.h>

#include <muduo/base/Logging.h>
#include <muduo/base/WeakCallback.h>
#include <muduo/net/Channel.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/Socket.h>
#include <muduo/net/SocketsOps.h>

#include <boost/bind.hpp>

#include <errno.h>

using namespace muduo;
using namespace muduo::net;

//连接建立完成的默认回调函数
void muduo::net::defaultConnectionCallback(const TcpConnectionPtr& conn)
{
  LOG_TRACE << conn->localAddress().toIpPort() << " -> "
            << conn->peerAddress().toIpPort() << " is "
            << (conn->connected() ? "UP" : "DOWN");
  // do not call conn->forceClose(), because some users want to register message callback only.
}

//消息到达TcpServer的默认回调函数
void muduo::net::defaultMessageCallback(const TcpConnectionPtr&,
                                        Buffer* buf,
                                        Timestamp)
{
  buf->retrieveAll();
}


/**
 *利用acceptor->accept(...)函数返回的connfd(即参数sockfd)来创建TcpConnection对象，此时将state_ 
 设置为kConnecting状态；
 TcpConnection拥有socket_ 和 channel_,但channel_只与fd_ 一一对应，并不拥有socket_;
 当channel_ 上有事件发生（消息抵达等）时，便会调用相应的回调函数处理，也即调用TcpConnection的
 handleRead()、handleWrite()、handleClose()、handleError()响应事件；
 */
TcpConnection::TcpConnection(EventLoop* loop,
                             const string& nameArg,
                             int sockfd,
                             const InetAddress& localAddr,
                             const InetAddress& peerAddr)
  : loop_(CHECK_NOTNULL(loop)),
    name_(nameArg),
    state_(kConnecting),//
    reading_(true),
    socket_(new Socket(sockfd)),
    channel_(new Channel(loop, sockfd)),
    localAddr_(localAddr),
    peerAddr_(peerAddr),
    highWaterMark_(64*1024*1024)
{
  /**
   * 设置channel_的各回调函数
   */
  channel_->setReadCallback(
      boost::bind(&TcpConnection::handleRead, this, _1));
  channel_->setWriteCallback(
      boost::bind(&TcpConnection::handleWrite, this));
  channel_->setCloseCallback(
      boost::bind(&TcpConnection::handleClose, this));
  channel_->setErrorCallback(
      boost::bind(&TcpConnection::handleError, this));

  LOG_DEBUG << "TcpConnection::ctor[" <<  name_ << "] at " << this
            << " fd=" << sockfd;
  socket_->setKeepAlive(true);
}


/**
 * TcpConnection::connectDestroyed()是~TcpConnection()执行前最后一个被调用的函数
 * 在connectDestroyed()中设置了state_ = kDisconnected
 */
TcpConnection::~TcpConnection()
{
  LOG_DEBUG << "TcpConnection::dtor[" <<  name_ << "] at " << this
            << " fd=" << channel_->fd()
            << " state=" << stateToString();
  assert(state_ == kDisconnected);
}



bool TcpConnection::getTcpInfo(struct tcp_info* tcpi) const
{
  return socket_->getTcpInfo(tcpi);
}

string TcpConnection::getTcpInfoString() const
{
  char buf[1024];
  buf[0] = '\0';
  socket_->getTcpInfoString(buf, sizeof buf);
  return buf;
}

/**
 * void TcpConnection::send(const void* data, int len);
 * void TcpConnection::send(const StringPiece& message);
 * void TcpConnection::send(Buffer* buf);//most important
 * TcpConnection 发送data的三个重载函数
 */

void TcpConnection::send(const void* data, int len)
{
  send(StringPiece(static_cast<const char*>(data), len));
}

/**
 * 必须是在 loop IO thread 线程内发送数据；核心是调用sendInLoop(...)函数
 */
void TcpConnection::send(const StringPiece& message)
{
  if (state_ == kConnected)//连接已建立 才能send data
  {
    if (loop_->isInLoopThread())
    {
      sendInLoop(message);
    }
    else
    {
      loop_->runInLoop(
          boost::bind(&TcpConnection::sendInLoop,
                      this,     // FIXME
                      message.as_string()));
                    //std::forward<string>(message)));
    }
  }
}

// FIXME efficiency!!!
void TcpConnection::send(Buffer* buf)
{
  if (state_ == kConnected)
  {
    if (loop_->isInLoopThread())
    {
      sendInLoop(buf->peek(), buf->readableBytes());
      buf->retrieveAll();
    }
    else
    {
      loop_->runInLoop(
          boost::bind(&TcpConnection::sendInLoop,
                      this,     // FIXME
                      buf->retrieveAllAsString()));
                    //std::forward<string>(message)));
    }
  }
}

void TcpConnection::sendInLoop(const StringPiece& message)
{
  sendInLoop(message.data(), message.size());
}

/**
 * TcpConnection发送数据的核心函数；
 */
void TcpConnection::sendInLoop(const void* data, size_t len)
{
  loop_->assertInLoopThread();
  ssize_t nwrote = 0;
  size_t remaining = len;
  bool faultError = false;
  if (state_ == kDisconnected)
  {
    LOG_WARN << "disconnected, give up writing";
    return;
  }

 
  /**
   * if no thing in output queue, try writing directly,直接将data写入channel_->fd()
   * !channel_->isWriting() 为true表示channel_还未关注POLLOUT事件（writable事件）
   * outputBuffer_.readableBytes() == 0 表示outputBuffer_中还未填充数据
   */
  if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0) // why????
  {
    //nwrote 实际写入channel_->fd()中的字节数
    nwrote = sockets::write(channel_->fd(), data, len);

    if (nwrote >= 0)
    {
      remaining = len - nwrote;
      //data 中的数据已全部写入了channel->fd(),则回调writeCompleteCallback_输出相应信息
      if (remaining == 0 && writeCompleteCallback_)
      {
        loop_->queueInLoop(boost::bind(writeCompleteCallback_, shared_from_this()));
      }
    }
    else // nwrote < 0 
    {
      nwrote = 0;
      if (errno != EWOULDBLOCK)
      {
        LOG_SYSERR << "TcpConnection::sendInLoop";
        if (errno == EPIPE || errno == ECONNRESET) // FIXME: any others?
        {
          faultError = true;
        }
      }
    }
  }
  
  /** data中的数据未全部写入channel_->fd()中 */
  assert(remaining <= len);
  if (!faultError && remaining > 0)
  {
    size_t oldLen = outputBuffer_.readableBytes();
    //outputBuffer_中原有的数据oldLen和data中剩余的数据的总和的字节数超过了highWaterMark_，此时需要回调
    //highWaterMarkCallback_进行相关处理...
    if (oldLen + remaining >= highWaterMark_
        && oldLen < highWaterMark_
        && highWaterMarkCallback_)
    {
      loop_->queueInLoop(boost::bind(highWaterMarkCallback_, shared_from_this(), oldLen + remaining));
    }
    //将data中余下数据存储到outputBuffer_中
    outputBuffer_.append(static_cast<const char*>(data)+nwrote, remaining);
    //channel_开始关注POLLOUT事件
    if (!channel_->isWriting())
    {
      channel_->enableWriting();
    }
  }
}



/**
 * 在LOOP IO THREAD 中关闭连接......
 */
void TcpConnection::shutdown()
{
  // FIXME: use compare and swap
  if (state_ == kConnected)
  {
    setState(kDisconnecting);
    // FIXME: shared_from_this()?
    loop_->runInLoop(boost::bind(&TcpConnection::shutdownInLoop, this));
  }
}

void TcpConnection::shutdownInLoop()
{
  loop_->assertInLoopThread();
  //!channel_->isWriting() 为真表明数据已全部发送完成，才能执行socket_->shutdownWrite();
  if (!channel_->isWriting())
  {
    // we are not writing
    socket_->shutdownWrite();
  }
}

// void TcpConnection::shutdownAndForceCloseAfter(double seconds)
// {
//   // FIXME: use compare and swap
//   if (state_ == kConnected)
//   {
//     setState(kDisconnecting);
//     loop_->runInLoop(boost::bind(&TcpConnection::shutdownAndForceCloseInLoop, this, seconds));
//   }
// }

// void TcpConnection::shutdownAndForceCloseInLoop(double seconds)
// {
//   loop_->assertInLoopThread();
//   if (!channel_->isWriting())
//   {
//     // we are not writing
//     socket_->shutdownWrite();
//   }
//   loop_->runAfter(
//       seconds,
//       makeWeakCallback(shared_from_this(),
//                        &TcpConnection::forceCloseInLoop));
// }



void TcpConnection::forceClose()
{
  // FIXME: use compare and swap
  if (state_ == kConnected || state_ == kDisconnecting)
  {
    setState(kDisconnecting);
    loop_->queueInLoop(boost::bind(&TcpConnection::forceCloseInLoop, shared_from_this()));
  }
}



void TcpConnection::forceCloseWithDelay(double seconds)
{
  if (state_ == kConnected || state_ == kDisconnecting)
  {
    setState(kDisconnecting);
    loop_->runAfter(
        seconds,
        makeWeakCallback(shared_from_this(),
                         &TcpConnection::forceClose));  // not forceCloseInLoop to avoid race condition
  }
}



void TcpConnection::forceCloseInLoop()
{
  loop_->assertInLoopThread();
  if (state_ == kConnected || state_ == kDisconnecting)
  {
    // as if we received 0 byte in handleRead();
    handleClose();
  }
}



const char* TcpConnection::stateToString() const
{
  switch (state_)
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



void TcpConnection::setTcpNoDelay(bool on)
{
  socket_->setTcpNoDelay(on);
}



void TcpConnection::startRead()
{
  loop_->runInLoop(boost::bind(&TcpConnection::startReadInLoop, this));
}



void TcpConnection::startReadInLoop()
{
  loop_->assertInLoopThread();
  if (!reading_ || !channel_->isReading())
  {
    channel_->enableReading();
    reading_ = true;
  }
}



void TcpConnection::stopRead()
{
  loop_->runInLoop(boost::bind(&TcpConnection::stopReadInLoop, this));
}



void TcpConnection::stopReadInLoop()
{
  loop_->assertInLoopThread();
  if (reading_ || channel_->isReading())
  {
    channel_->disableReading();
    reading_ = false;
  }
}



/**连接建立已完成，回调connectionCallback_输出相关信息；  */
void TcpConnection::connectEstablished()
{
  loop_->assertInLoopThread();
  assert(state_ == kConnecting);
  setState(kConnected);
  //??
  channel_->tie(shared_from_this());
  //channel_关注POLLIN | POLLPRI 事件，接收数据
  channel_->enableReading();
  //回调 connectionCallback_来自于TcpServer的connectionCallback_
  connectionCallback_(shared_from_this());
}

/**断开链接，回调connectionCallback_ 输出断开连接信息；
 * ～TcpConnection（）调用前最后一个调用的函数
 */
void TcpConnection::connectDestroyed()
{
  loop_->assertInLoopThread();
  if (state_ == kConnected)
  {
    setState(kDisconnected);
    channel_->disableAll();

    connectionCallback_(shared_from_this());
  }
  channel_->remove();//删除poller_ 的map中对应的channel
}


/**
 * TcpConnection::handleRead(Timestamp receiveTime)在构造函数中注册为
 * channel_的readCallback_; 从channel->fd()中读取数据到inputBuffer_中；
 * n>0 :消息抵达，回调用户的messageCallback_处理message
 * n=0 ：client 发送了关闭连接的message，执行handleClose()
 * n<0 :发生错误，保存errno， handleError()
 */
void TcpConnection::handleRead(Timestamp receiveTime)
{
  loop_->assertInLoopThread();
  int savedErrno = 0;
  //读取接收缓冲区的消息
  ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
  if (n > 0)
  {
    //messageCallback_ was set in TcpServer::newConnection(); 
    //messageCallback_ 来自于TcpServer的messageCallback_
    messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
  }
  else if (n == 0)
  {
    handleClose();
  }
  else
  {
    errno = savedErrno;//errno定义在errno.h中，是外部变量，任何包含了errno.h的源文件都可以获取errno的当前值
    LOG_SYSERR << "TcpConnection::handleRead";
    handleError();
  }
}

/**
 * channel_的writeCallback_,主要工作是将outputBuffer_中数据写到channel_->fd()中，直至
 * outputBuffer_.readableBytes()==0,表示数据已全部写入fd；
 */
void TcpConnection::handleWrite()
{
  loop_->assertInLoopThread();
  //channel_已关注POLLOUT事件
  if (channel_->isWriting())
  {
    //将outputBuffer_中的数据写入到channel_->fd(),返回值n表示实际写入的字节数
    ssize_t n = sockets::write(channel_->fd(),
                               outputBuffer_.peek(),
                               outputBuffer_.readableBytes());
    if (n > 0)
    {
      //从outputBuffer_中提取出n字节的数据并更新buffer的readIdx、writeIdx .
      outputBuffer_.retrieve(n);

      //outputBuffer_中的数据已全部写入channel_->fd()
      if (outputBuffer_.readableBytes() == 0)
      {
        //取消 channel_上的POLLOUT事件！！！如果不取消会发生什么？？？
        channel_->disableWriting();
        //回调writeCompleteCallback_ 输出相关信息
        if (writeCompleteCallback_)
        {
          loop_->queueInLoop(boost::bind(writeCompleteCallback_, shared_from_this()));
        }
        //正在断开连接.....
        if (state_ == kDisconnecting)
        {
          shutdownInLoop();
        }
      }
    }
    else
    {
      LOG_SYSERR << "TcpConnection::handleWrite";
      // if (state_ == kDisconnecting)
      // {
      //   shutdownInLoop();
      // }
    }
  }
  else
  {
    LOG_TRACE << "Connection fd = " << channel_->fd()
              << " is down, no more writing";
  }
}

/**
 * channel_的closeCallback_，即为TcpServer::removeConnection(const TcpConnectionPtr& conn)
 * TcpServer::removeConnection(...)---->TcpServer::removeConnectionInLoop(...)---->
 * TcpConnection::connectDestroyed()---->Channel::remove()---->loop_->removeChannel(this)
 * ---->poll->removeChannel();
 */
void TcpConnection::handleClose()
{
  loop_->assertInLoopThread();
  LOG_TRACE << "fd = " << channel_->fd() << " state = " << stateToString();
  //只有处于kConnected或者kDisconnecting状态的TcpConnection才有可能执行handleClose函数
  assert(state_ == kConnected || state_ == kDisconnecting);
  // we don't close fd, leave it to dtor, so we can find leaks easily.
  setState(kDisconnected);
  //channel_不关注任何事件；更新poller中channel_相关状态
  channel_->disableAll();
  TcpConnectionPtr guardThis(shared_from_this());
  //显示连接已断开....
  connectionCallback_(guardThis);
  // must be the last line
  closeCallback_(guardThis);
}

/**
 * channel_的errorCallback_
 */
void TcpConnection::handleError()
{
  int err = sockets::getSocketError(channel_->fd());
  LOG_ERROR << "TcpConnection::handleError [" << name_
            << "] - SO_ERROR = " << err << " " << strerror_tl(err);
}

