# myshell dev note

## Specification

PS1 and PS2 defined in POSIX are read-only in myshell

Shell variables like $0 - $9 and $PS1 and $PS2 should be pushed in the hash
table.

Shift support

Environs and arguments passed are strictly distinguished!

Behavior different from bash:
- & is a pipeline connector, not a seperator
- The job control is global

Finally...
- I decided on no control flow support this time.
- No serious Unicode support
- No localization! No i18n!! Even no Chinese!!!
- No export feature.
- And no subshell feature!
- And no $()!
- No shell-level wildcards.
- ~~ANSI-C Quoting $'' support?~~ NO!
- ↑ ↓ ← → keyboard key support? Handle the cursor??

## Something I have learnt

- The essence of shebang
- 子进程信号的捕获，应当采取循环等待任何信号，再处理的方式。
    - 最后采用了SIGCHLD处理函数的方式，不知道能不能工作。
- 父进程的SIGCHLD不应当忽略。否则将没有办法获得子进程的返回值。
- 当PGID不再存在时setpgid会发生错误
- bash 规定的种种
- 实现控制流其中一个难点在于关键字不是保留字……虽然这些关键字几乎一定在行首，很好地控制还是一个难点。
- 那个过程叫作shell expansion……
- 我明白了什么叫栈缓冲区溢出（stack smashing）……如果在某个scope分配了栈上的数组，越界的话可能会导致访问到下面的区域，静态变量、返回地址和正文等等……
- 对了，如果说写代码其间遇到什么问题，有一点一定要写上：我在写shell expansion的时候，内心极其憋闷。人肉正则实在是……
- ⁄(⁄ ⁄•⁄ω⁄•⁄ ⁄)⁄

对，让我们好好梳理一下shell做了什么？┑(￣Д ￣)┍（以bash为例，参考*Bash Reference Manual*）
1. 读取输入
2. 分词，简单的词法分析
3. 句法分析，将符号转换为简单和复合命令
4. 执行shell扩展（shell expansion，如变量替换、转义、波浪线替换）
5. 重定向，将重定向操作符从命令中移除
6. 执行命令
7. 等待命令结束，获得调用程序的返回值（exit status）

myshell做的事情类似，只是因为不打算加入subshell和控制流特性，句法分析被砍了很多。
要知道bash源代码有二百多兆呢啊啊啊～！！我也是Linux编程初学者。
1. 读取输入
2. 词法预处理
3. 词法分析
4. shell扩展
5. 生成进程和任务
6. 执行进程，收集返回值

## A list of shell variables

- HOSTNAME
- MYSHELL_PATH
- PS1
- PS2

## Reminders

~~Don't forget the input command line should be an array of int, for you might
need to handle the EOF (Ctrl + D)... Or there are probably other methods to
handle this.~~

Done.

## TODO

- [x] signals, handle the keyboard interrupt, etc...
- [x] Hash map find
- [x] Test hash map
- [ ] Parser
- [x] ~~Evaluate the tokens~~           Implemented by expansion.c
- [x] ~~Data structure for commands~~   Implemented by universal job control
- [ ] Full notification of job control
- [ ] Built-ins
- [x] Signal binding of SIGINT

## Ref

### Hash

- 数据结构与算法分析——C语言描述

### Process & Job control

- APUE
- https://www.gnu.org/software/libc/manual/html_node/Implementing-a-Shell.html
- https://www.andrew.cmu.edu/course/15-310/applications/homework/homework4/lab4.pdf

- *Bash Reference Manual*, Chet Ramey, Case Western Reserve University, Brain Fox, Free Software Foundation

# Not fully implemented

- ~~parser~~ /(ㄒoㄒ)/～没学过编译原理的人写的东西不能叫parser