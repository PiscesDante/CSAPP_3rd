# Lecture 23. Concurrent Programming



## 并发编程是非常困难的

一般来说，只要有多个进程共享一个资源，那么整个程序就会出现各种奇怪的问题。

**<u>并发编程所面临的经典问题</u>** ：

*   竞争： **<u>结果的好坏取决于任意的调度决策</u>** 。举个例子来说：如果两个人同时访问航空订票系统，那么谁会得到飞机上的最后一个座位？这个问题取决于系统的调度问题。
*   死锁： **<u>有多个控制流等待永远不会发生地事情</u>** 。
*   活锁/饥饿/公平：举个例子来说，进程的饥饿表示因为其他进程的优先级较高或者操作系统需要安排的进程过多，那么优先级较低的进程永远不会被操作系统安排使用硬件资源（饥饿）。

## 迭代式服务器

**<u>在不使用并发的情况下是不可能写出一个正确的服务器的</u>** 。

迭代式服务器的核心问题就是，一旦在对一个客户端服务过程中出现不能完成的情况，那么就会阻塞后来的所有客户端访问服务。

### 解决方案：撰写支持并发的服务器

对于每一个客户请求，服务器都有一个独立的并发流去处理，这样我们通过并发的方式，即使在某一个客户端卡住的情况下，剩下的客户端也不会受到影响。

## 实现并发服务器的几种方式

### 1. 基于进程的并发

*   操作系统内核将自动调度若干个逻辑控制流；
*   每个控制流都会有自己的私有空间；

#### 典型实现方式

基于进程的并发是使用 `fork` 创建子进程去处理发送来的请求，而父进程继续等在原地不会走动，继续侦听其他客户端发送来的请求。也就是说，有多少个客户端请求，父进程就会制造多少个子进程来处理请求。

这样父进程永远不会被请求阻塞。和客户端交互的任务都交给了子进程来执行和处理。

```C
int main(int argc, char* argv[])
{
    int listen_file_descriptor; /* 侦听套接字 */
    int connect_file_descriptor; /* 连接套接字 */
    struct sockaddr_storage client_address;
    
    Signal(SIGCHLD, sigchld_handler);
    listen_file_descriptor = Open_listen_file_descriptor(argv[1]);
    while (1) {
        client_length = sizeof(struct sockaddr_storage);
        connect_file_descriptor = Accept(listen_file_descriptor, (SA*)&client_address, &client_length);
        if (Fork() == 0) { /* 子进程中需要执行的部分 */
            Close(listen_file_descriptor); /* 子进程关闭自己的侦听套接字 */
            echo(connect_file_descriptor); /* 子进程需要执行的任务 */
            Close(connect_file_descriptor); /* 子进程关闭连接套接字 */
            exit(0); /* 子进程退出 */
        }
        Close(connect_file_descriptor); /* 父进程关闭连接套接字 */
        /* 避免内存泄漏 */
    }
}
```

#### 基于进程并发的服务器的问题所在

* 负责侦听的父进程必须处理掉僵尸子进程；
* 父进程必须将自己的那一份【连接文件描述符】关掉；
  * 因为操作系统内核会计算一个文件描述符上的引用数量；
  * 在 `fork` 之后，连接文件描述符的引用计数会变成 2 ；
  * 如果父进程不关掉自己不使用的那个连接描述符，则该描述符不能被彻底关掉，就会造成内存泄漏。

### 2. 基于事件的并发（事件驱动）

*   程序员需要自己手动调用流程，而不是依赖内核来调度；
*   所有流都共享一个地址空间；
*   使用的技术叫做 **<u>I/O 复用技术</u>** ；

设计高性能 Web 服务器或者搜索引擎的时候，需要使用基于事件并发的技术：Node.js 和 Nginx 都是基于事件并发的。

### 3. 基于线程的并发（进程和事件的混合模式）

*   内核就像进程一样会自动调度线程；
*   但是和进程不同的是，线程共享相同的地址空间；
*   混合了进程和事件模式；

 #### 一个拥有多个线程的进程

* 每个线程拥有自己的逻辑控制流；
* 每个线程共享相同的代码，数据和内核上下文；
* 每个线程拥有自己的局部变量栈（但是这个栈并不受保护）；
* 每个线程都拥有自己的线程 ID ；

### 线程 VS 进程

#### 线程和进程是相似的

* 线程和进程都拥有自己的控制流；
* 进程和线程都可以和其他同类进行并发（可能运行在不同的内核上）；
* 进程和线程都可以进行上下文的切换；

#### 线程和进程又是不同的

* 线程会共享所有的数据和代码（除了线程自己的局部栈），而进程不会；
* 线程实现起来的代价要比进程更低：进程的控制代价要比线程的控制代价高两倍；

### Posix 线程（ `pthread` ）接口

Pthreads ：针对 C 语言的，拥有 60 多个函数的，用来管理线程的标准接口库；

这是一个很新的接口标准，基本上所有平台都适用；

### Pthread 的 `Hello World` 程序

```C
#include <stdio.h>
#include <pthread.h>

/* 函数的声明必须为这个形式，否则会编译错误 */
void* hello_world(void* args);

int main(int argc, char* argv[])
{
    pthread_t thread_id;
    /* 创建一个平行的线程并且开始执行 */
    pthread_create(&thread_id, NULL, hello_world, NULL);
    /* 主线程等待这个平行的线程执行完毕 */
    pthread_join(thread_id, NULL);
    return 0;
}

void* hello_world(void* args)
{
    printf("HELLO WORLD! - FROM PTHREAD\n");
    return NULL;
}

/* clang -std=c11 pthread_demo.c -lpthread -o pthread_demo */
```

因为 `pthread.h` 不属于 C 标准库的一部分，所以必须在编译时加上 `-lpthread` 指令才能通过编译。