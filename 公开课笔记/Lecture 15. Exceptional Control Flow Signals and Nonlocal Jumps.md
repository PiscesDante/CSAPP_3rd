# Lecture 15. Exceptional Control Flow: Signals and Nonlocal Jumps



## 异常控制流在系统的所有级别都存在



## Shell 程序

Shell 程序本质上是一个代表用户来运行程序的应用程序；执行程序就是在 Shell 中的一系列读和求值的步骤：

```C
int main(int argc, char* argv[])
{
    char command_line[MAX_LINE]; /* Command Line */
    while (1) {
        /* READ */
        printf("> ");
        Fgets(command_line, MAX_LINE, stdin);
        if (feof(stdin)) {
            exit(0);
        } else {
            eval(command_line);
        }
    }
}
```

## 简单的 Shell `eval` 函数

```C
void eval(char* command_line)
{
    char* argv[MAX_ARGS]; /* 执行程序所需要的参数列表 */
    char buf[MAX_LINE]; /* 保存修改过的命令行指令 */
    int is_background; /* 被执行的程序应该是前台还是后台 */
    pid_t process_id; /* 进程 ID */
    
    strcpy(buf, command_line);
    is_background = parseline(buf, argv);
    if (argv[0] == NULL) {
        return; /* 忽略掉空白行 */
    }
    if (!is_builtin_command(argv)) {
        if ((process_id = Fork()) == 0) { /* 子进程部分 */
            if (execve(argv[0], argv, environ) < 0) {
                printf("%s: Command not found.\n", argv[0]);
                exit(0);
            }
        } else { /* 父进程部分 */
            if (!is_background) { /* 如果是前台程序 */
                int status;
                if (waitpid(process_id, &status, 0) < 0) {
                    unix_error("wait foreground: waitpid error");
                } else {
                    printf("%d - %s", process_id, command_line);
                }
            } else {
                /* 问题：如果是后台程序该怎么办？ 
                 * 这个部分的缺失也就是下面要讨论的问题。 */
            }
        }
    }
    return;
}
```

## 上面那个简单 Shell 案例的问题

*   我们的 Shell 可以正确的等待和回收前台工作；
*   但是对于后台工作应当如何处理？
    *   后台程序在终止时会变成僵尸程序；
    *   因为 Shell 不会终止运行，所以后台程序永远不会被回收；
    *   这就会导致内存泄漏，并且最终导致系统崩溃；

## 使用异常控制流来解决问题

### 解决方案：异常控制流

*   当一个后台进程完成的时候，内核会设置一个中断来通知我们；
*   在 Unix 系统中，这个警告机制就是所谓的「信号」。

## 信号

**<u>信号就是一条小信息，这个信息就会提示进程在系统中发生了某个类型的事件</u>** 。

*   信号一般是由系统内核发送的，但是有时也是另一个进程发送的；
*   信号很像中断和异常；
*   信号种类是由一个小整数来标识的（1 ~ 30）；
*   信号能携带的就只是那个小整数，已经它发生的事实；

## 信号的概念：发送一个信号

**<u>操作系统内核使用更新一个进程的上下文中的某个状态来通知目标进程事件的发生，这个操作本身也就是发送信号</u>** 。

操作系统内核可能会因为以下的原因来发送信号：

*   内核侦测到一个系统事件，例如除数为零或者子进程终止运行；
*   或者另一个进程请求（系统调用）内核代替其发送一个信号传递给另一个进程，比方说 `kill` 指令；

## 信号的概念：接收一个信号

**<u>当一个目标进程被内核强制对某些事件作出相应时，就称为接收信号</u>** 。

在设置目标进程上下文中的状态时，有两种差异很大的方式；收到信号是目标进程最终发生地事情。

一些可行的对于信号的反应方式：

*   忽略这个信号，也就是什么都不做；

*   在接受到信号之后终结进程；

*   使用用户级的信号处理函数来捕获它，这个用于响应信号的函数就称为「信号响应函数」。这个和硬件异常处理中的，用于响应中断的响应函数很相似。

    异常处理是系统级的，也就是操作系统负责一个异常应该如何处理；但是信号响应是用户级的，也就是用户决定一个信号被接收的时候应该如何处理。

## 信号的概念：被暂缓处理（挂起）以及被阻塞的信号

**<u>一个信号如果已经被发送但是还没有被接收，那么这个信号就是暂缓处理的信号</u>** 。

*   每种类型的信号至多只能有一个被挂起（因为上下文中对应这个类型的信号的状态值也就一个）；

*   至关重要： **<u>信号不能被以队列的方式存放</u>** 。

    也就是说，如果一个进程的上下文中已经存在一个被挂起的 K 类信号，那么接下来发送给该进程的任何 K 类信号都会被直接丢弃（不存在排队的说法）。

**<u>一个进程可以阻塞某些种类的信号的接收</u>** 。

对于一个进程来说，其他进程或者内核可以向其发送被阻塞类型的信号，但是它不会接收，直到这个进程重新开放这个信号的接收。

也就是说你可以发，但是我不理不查看不响应。

**<u>待处理的信号最多被接收一次</u>** 。

## 信号的概念：被挂起的/阻塞的位

**<u>操作系统内核在每个进程中维护着一个信号位向量，这个位向量中的每个位代表一种类型的信号</u>** 。

**<u>待处理位向量中的每一个位都对应着一个特定的信号，因为每一个信号只有一个位来表示，在位向量中只有一个位。所以当我们传递相同类型的信号的时候，内核仅仅会重新设置那个位，本质上来说其实什么都没有做（这也就是为什么待处理信号无法排队的原因）</u>** 。

**<u>当信号被传递过来的时候，内核将设置目标进程上下文中的待处理位向量中的特定位，当且仅当信号被接收时，内核将重新将其清零</u>** 。

内核也向用户提供了可以阻塞信号的阻塞位向量。阻塞位向量和待处理位向量基本相同，都是一个 32 位的 `int` 类型。

*   可以调用 `sigprocmask` 函数来设置或者清零；
*   在 Linux 系统中，阻塞位向量有时也被称为信号掩码；

## 发送信号：进程组

**<u>每个进程属于一个特定的进程组</u>** 。

每个进程组都有一个进程组标识码，这个标识码可以由属于这个进程组的进程（一般来说是第一个创建这个进程组的进程）来设置一个进程组标识码。这个进程组标识码可以和当前进程的进程标识码相同。

当同进程组标识码的进程创建子进程的时候，子进程也会在相同的进程组中，并且拥有相同的进程组标识码。

发送信号的手段有三种：

*   在 Linux 系统中有一个 `/bin/kill` 的程序，这个程序可以向特定的进程或者进程组发出任意信号。

    ```
    /bin/kill -9 24818
    其中 -9 是 SIGKILL ，而 24818 是进程标识码
    
    /bin/kill -9 -24817
    其中 -9 是 SIGKILL ，而 -24817 是进程组的标识码
    针对进程组中的所有进程
    ```

*   使用 Ctrl + C 或者 Ctrl + Z 来发送信号；

*   使用系统调用来发送信号：

    ```C
    void fork_systemcall()
    {
        pid_t process_ids[N];
        int child_status;
        
        /* 创建无限循环的子进程 */
        for (int i = 0; i < N; ++i) {
            if ((process_ids[i] = Fork()) == 0) {
                /* CHILD PROCESS */
                while (true) {  }
            }
        }
        
        /* 使用系统调用 kill 来终止创建的子进程 */
        for (int i = 0; i < N; ++i) {
            printf("Killing process %d\n", process_ids[i]);
            kill(process_ids[i], SIGINT);
        }
        
        /* 等待并且回收所有的子进程结束并且监测其退出的状态 */
        for (int i = 0; i < N; ++i) {
            pid_t waiting_pid = wait(&child_status);
            if (WIFEXITED(child_status)) {
                printf("Child %d terminated with exit status %d", waiting_pid, WEXITSTATUS(child_status));
            } else {
                printf("Child %d terminated abnormally\n", waiting_pid)
            }
        }
    }
    ```

## 接收信号

**<u>假设系统内核正在从一个异常控制流中返回，并且将控制权交给一个进程 P</u>** 。

或者假设内核正在从进程 A 通过上下文切换将控制权移交给进程 B ，在移交给进程 B 的时候，内核会检查进程 B 上下文中所有挂起的信号。

检查的方法就是计算 `pending_nonblocked` 向量来确定哪些信号是待处理的：

```
pending_nonblocked = pending & ~blocked;
```

所以 `pending_nonblocked` 是所有未阻塞待处理信号的列表；

如果 `pending_nonblocked` 中的元素全是 `0` ，那么就说明没有待处理的信号。

但是如果 `pending_nonblocked` 中有非零的元素，则说明有待处理的信号。那么：

*   选择具有最小索引的非零位 `k` ，并且使得进程来处理这个序号为 `k` 的信号；
*   信号的接收将导致进程对于信号的处理函数……
*   然后选择下一个非零位 `j` 并执行同样地动作，直到处理完成所有的信号；
*   最后执行当前进程中的下一条指令；

## 默认动作

**<u>所有的信号类型都有一个对应的预定义默认动作</u>** ，比方说：

*   终止进程；
*   暂停进程，直到接收到 `SIGCONT` 信号；
*   进程忽略信号；

## 安装信号处理函数

**<u>我们可以使用一个叫做 `signal` 的系统调用来更改进程对于信号的默认行为</u>** ：

```C
handled_t* signal(int signum, handler_t* handler);
/* 修改和特定信号相关的响应函数 */
```

第一个参数是信号标识码，也就是说我们要修改默认行为的的信号种类，第二个参数是代替默认行为的响应函数，当进程接收到信号时，我们就会使用指定的响应函数而非默认的动作。

## 信号处理案例

```C
/* SIGINT HANDLER */
void sigint_handler(int sig)
{
    printf("SO YOU THINK YOU CAN STOP THE BOMB WITH CTRL-C, DO YOU?\n");
    sleep(2);
    printf("WELL...");
    fflush(stdout);
    sleep(1);
    printf("OK. :-)\n");
    exit(0);
}

int main(int argc, char* argv[])
{
    /* INSTALL THE SIGINT HANDLER */
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        unix_error("SIGNAL ERROR");
    } else {
        pause(); /* WAIT FOR THE RECEIPT OF A SIGNAL */
    }
}
```

## 信号处理是一种并发流

**<u>信号处理函数是一种独立的逻辑流（并非进程），这个逻辑流可以并发的运行在主函数中</u>** 。因为这个信号处理函数和 `main` 函数运行在同一个进程中，所以我们说它是并发的。

## 被阻塞的和未被阻塞的信号

*   隐式的信号阻塞机制
    *   内核会阻塞任何一种已经在被处理的信号类型；
    *   举个例子，就是信号 `SIGINT` 的响应函数不能再被 `SIGINT` 信号打断；
*   显式阻塞和放行机制
    *   `sigprocmask` 函数；

## 临时阻塞信号

```C
sigint_t mask;
sigint_t prev_mask;

Sigemptyset(&mask); /* 创建一个空的掩码，这是一个全为 0 的掩码 */
Sigaddset(&mask, SIGINT); /* 添加一个 SIGINT 信号到这个集合中 */

/* 现在我们有一些不想被 SIGINT 打断的代码，那么： */
Sigprocmask(SIG_BLOCK, &mask, &prev_mask);

/* 这里撰写不想被 SIGINT 信号打断的代码…… */

/* 恢复到之前的信号阻塞掩码 */
Sigprocmask(SIG_SETMASK, &prev_mask, NULL);
```

## 安全的信号处理

*   因为信号处理函数并发的和 `main` 函数运行在同一个进程中，所以他们都享有同样的全局数据；
*   我们会在以后讨论并发的事情；
*   现在介绍几条安全准则；

## 撰写安全信号处理函数的准则

*   让信号处理函数尽量的简单，比方说只返回一个全局变量；
*   只调用异步安全的函数；
*   在开始和结束时存储并且负责恢复 `errno` ，防止自己复写这个变量；
*   当访问共享的数据结构的时候，要临时阻塞其他信号；
*   将全局变量声明为 `volatile` ，这样的话全局变量就不会使用寄存器进行存储，而是始终通过内存进行存储；

## 异步信号安全

