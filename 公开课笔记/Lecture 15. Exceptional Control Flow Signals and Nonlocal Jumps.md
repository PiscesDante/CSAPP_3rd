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
    char  buf[MAX_LINE];  /* 保存修改过的命令行指令 */
    int   is_background;  /* 被执行的程序应该是前台还是后台 */
    pid_t process_id;     /* 进程 ID */
    
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
                /* 问题：如果是后台程序该怎么办？ */
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

