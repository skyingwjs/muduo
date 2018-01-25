# Install script for directory: /home/wjs/code/vscode/muduo/muduo-master/muduo/base

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/wjs/code/vscode/muduo/build/release-install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/wjs/code/vscode/muduo/build/release/lib/libmuduo_base.a")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/wjs/code/vscode/muduo/build/release/lib/libmuduo_base_cpp11.a")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/muduo/base" TYPE FILE FILES
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/Singleton.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/Mutex.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/Atomic.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/copyable.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/CountDownLatch.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/Thread.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/Types.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/Condition.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/FileUtil.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/ThreadLocalSingleton.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/GzipFile.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/TimeZone.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/WeakCallback.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/Exception.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/ThreadLocal.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/LogStream.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/Timestamp.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/CurrentThread.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/BlockingQueue.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/Date.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/LogFile.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/StringPiece.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/ProcessInfo.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/BoundedBlockingQueue.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/ThreadPool.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/Logging.h"
    "/home/wjs/code/vscode/muduo/muduo-master/muduo/base/AsyncLogging.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/wjs/code/vscode/muduo/build/release/muduo/base/tests/cmake_install.cmake")

endif()

