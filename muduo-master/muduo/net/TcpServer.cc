// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include <muduo/net/TcpServer.h>

#include <muduo/base/Logging.h>
#include <muduo/net/Acceptor.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/EventLoopThreadPool.h>
#include <muduo/net/SocketsOps.h>

#include <boost/bind.hpp>

#include <stdio.h>  // snprintf

using namespace muduo;
using namespace muduo::net;

/**
 * 设置默认的connectionCallback_ 和messageCallback_，设置acceptor_的newConnectionCallback_为
 * TcpServer::newConnection（...），在新连接建立完成时(即acceptor_->accept(...)函数返回的connfd>0时)
 * 便回调TcpServer::newConnection(...connfd...)创建TcpConnection对象
 */
TcpServer::TcpServer(EventLoop* loop,
                     const InetAddress& listenAddr,
                     const string& nameArg,
                     Option option)
  : loop_(CHECK_NOTNULL(loop)),
    ipPort_(listenAddr.toIpPort()),
    name_(nameArg),
    acceptor_(new Acceptor(loop, listenAddr, option == kReusePort)),
    threadPool_(new EventLoopThreadPool(loop, name_)),
    connectionCallback_(defaultConnectionCallback),
    messageCallback_(defaultMessageCallback),
    nextConnId_(1)
{
  //！！！！！ acceptor->accept()函数返回connfd后会调用newConnectionCallback_   
  acceptor_->setNewConnectionCallback(
      boost::bind(&TcpServer::newConnection, this, _1, _2));
}

/**
 * 析构TcpServer之前必须先destroy其成员connections_中所有的TcpConnection
 * destroy TcpConnection的工作必须在LOOP IO thread中由TcpConnection::connectDestroyed(...)
 * 完成
 */
TcpServer::~TcpServer()
{
  loop_->assertInLoopThread(); //只能在LOOP IO thread 析构TcpServer
  LOG_TRACE << "TcpServer::~TcpServer [" << name_ << "] destructing";

  for (ConnectionMap::iterator it(connections_.begin());
      it != connections_.end(); ++it)
  {
    TcpConnectionPtr conn(it->second);
    it->second.reset();
    //!!! 在loopThread执行 conn.connectDestroyed()
    conn->getLoop()->runInLoop(
      boost::bind(&TcpConnection::connectDestroyed, conn));
  }
}

void TcpServer::setThreadNum(int numThreads)
{
  assert(0 <= numThreads);
  threadPool_->setThreadNum(numThreads);
}

/**
 * Acceptor::listen(...)开启监听......
 */
void TcpServer::start()
{
  if (started_.getAndSet(1) == 0)//？？
  {
    threadPool_->start(threadInitCallback_);

    assert(!acceptor_->listenning());
    
    loop_->runInLoop(
        boost::bind(&Acceptor::listen, get_pointer(acceptor_)));
  }
}

/**
 * TcpServer::newConnection(...)为acceptor_的newConnectionCallback_;
 * newConnection(...)根据已建立连接的connfd创建TcpConnection对象conn
 * 并设置conn的**Callback_;然后运行TcpConnection::connectEstablished(....)
 */
void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
  loop_->assertInLoopThread();
  EventLoop* ioLoop = threadPool_->getNextLoop();
  char buf[64];
  snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_);
  ++nextConnId_;
  string connName = name_ + buf;

  LOG_INFO << "TcpServer::newConnection [" << name_
           << "] - new connection [" << connName
           << "] from " << peerAddr.toIpPort();
  InetAddress localAddr(sockets::getLocalAddr(sockfd));
  // FIXME poll with zero timeout to double confirm the new connection
  // FIXME use make_shared if necessary
  TcpConnectionPtr conn(new TcpConnection(ioLoop,
                                          connName,
                                          sockfd,
                                          localAddr,
                                          peerAddr));
  //将conn添加到connections_                                          
  connections_[connName] = conn;
  /*set connection **Callback in TcpServer class */
  conn->setConnectionCallback(connectionCallback_);
  conn->setMessageCallback(messageCallback_);
  conn->setWriteCompleteCallback(writeCompleteCallback_);
  
  /**设置conn断开连接时的回调函数为TcpServer::removeConnection(...),断开连接时应将conn从TcpServer
   * 的connections_中删除，并将conn对应的channel_从poller的channels_中删除
   */
  conn->setCloseCallback(
      boost::bind(&TcpServer::removeConnection, this, _1)); // FIXME: unsafe
  /**
   * 在connectEstablished（）中应设置conn对应的channel所关注的事件
   */
  ioLoop->runInLoop(boost::bind(&TcpConnection::connectEstablished, conn));
}


/**
 * TcpServer::removeConnection为 TcpConnection的closeCallback_ ，当TcpConnection断开时需要将TcpServer的
 * connections_中对应的conn 删除
 */
void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
  // FIXME: unsafe
  loop_->runInLoop(boost::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
  loop_->assertInLoopThread();
  LOG_INFO << "TcpServer::removeConnectionInLoop [" << name_
           << "] - connection " << conn->name();
  
  size_t n = connections_.erase(conn->name());
  (void)n;
  assert(n == 1);
  EventLoop* ioLoop = conn->getLoop();
  ioLoop->queueInLoop(
      boost::bind(&TcpConnection::connectDestroyed, conn));
}

