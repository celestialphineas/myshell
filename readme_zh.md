myshell 自述文档
===============

version 0.1.0                                   \
Author: Celestial Phineas @ ZJU (Yehang YIN)    \
Contact: yehangyin AT outlook DOT com           \
License: MIT

*该文档使用了Markdown语法。*

本文系 myshell 中文用户手册，如果您需要获知本项目的整体说明，请参阅 readme.md。

This file is the user's manual for myshell in simplified Chinese. See readme.md for a more general perspective and a few details of implementation, as well as a list of references.

概述
----

myshell 是一个 Linux 上的命令行 shell 实现，设计初衷是实现不含控制流的 bash 的一个子集。shell 中文一般译为“壳层”，是用户访问操作系统的界面。称作“壳层”的原因是它提供了系统内核之上的一层，可与用户交互。bash的全称是 Bourne-Again SHell，最早的作者是 Brain Fox，具有完备的通配符匹配、管道、重定向、命令替换、变量替换、控制流和任务控制等功能。myshell 最终达成了上述功能的部分。

myshell 具有如下主要特性：
- 管道、重定向
- 变量设置与替换
- 相当一部分 shell 展开
    - 波浪线展开（解释 home `~`）
    - 变量展开
    - 数组
    - 数组元素
    - 字符串长度
- 任务控制
- 内建命令
- 完善的命令提示符机制
- 支持 shebang 的处理
- ……

myshell 不支持的主要特性：
- 命令替换 $()
- 子壳 (subshell，即圆括号)
- 控制流（if, while, for）
- shell 级的通配符展开（*和?的匹配）

myshell 不支持或没有做过充足测试的次要特性：
- 本地化，甚至不做汉化
- Unicode 支持
- ANSI-C 转义字符展开 $''
- 历史命令
- 光标移动

构建
----

myshell 使用 make 工具构建，您只需要在 myshell 目录下键入命令 `make` 即可编译。

```
username@hostname:~/path/myshell$ make
```

make 会自动完成编译过程。

运行
----

在命令行中键入 myshell 可以交互模式运行 myshell。myshell 支持如下参数：

```
myshell
Usage: myshell [options] script ...
GNU options:
    --help      Show this help file
    --version   Show version
Shell options:
    -i          Run in interactive mode
```

将脚本文件作为参数传入即可令 myshell 运行脚本，脚本文件名后面的参数将被视作脚本的参数传入，可在脚本中用 $1 ~ $9 ~ ${10} ... 访问。

myshell 支持对 shebang 的处理，也就是说您可以在终端中直接运行脚本，如果脚本的第一行指定了myshell 的完整路径，myshell 即可正确处理脚本的打开与运行。shebang 指脚本文件头出现的“#!”字符序列，其对应的二进制为 Unix 下约定的脚本文件魔数。在执行脚本文件时，会将该行剩余内容作为执行这一脚本文件的命令。

下面是一些例子，命令提示符用“$”表示：

```
$ ./myshell
username@hostname:pwd/
myshell>
```

```
$ ./myshell script.sh
<运行脚本…>
```

```
$ cat script.sh
#! ./myshell
echo hello world
$ chmod 777 script.sh
$ ./script
<运行脚本…>
```

基本使用
--------

经过成功编译与运行，myshell 已经可以开始使用了。终端中打印出的彩色字符是 myshell 的命令提示符，您可以看到自己的用户名、计算机名和当前操作目录。myshell 的命令提示符尽可能与 bash 相一致，但是为了彰示区别，命令提示符去掉了“$”符号，另起一行输出紫色 `myshell>`。

### Hello world

下面一个例子将演示您如何在 myshell 中打印“Hello world”。您只需输入命令 `echo Hello world`

```
username@hostname:~/myshell
myshell> echo Hello world
```

### 调用其他命令

您可以尝试键入 `ls`、`ps` 和 `pwd` 看看会发生什么。

### 清屏

清屏的命令是 `clear`，尝试一下。你会发现终端的文本内容清空了或上滚了一屏（取决于您所使用图形终端的特性）。

重定向
--------

myshell 支持重定向。重定向可以将当前命令的输入或输出从终端定位到其他文件。在 Unix/Linux 中，所有输入输出设备都被视作文件，文件也可以用于输入输出。将键盘输入和屏幕输出重定向到其他文件中当然也是可行的。

### 向一个文件写 Hello world

尝试一下这个命令：`echo hello world >hello.txt`，它会创建一个名为 `hello.txt` 的文件并向其中写入 “hello world”。

### 向文件中追加

对上面的例子做一个小小的修改：`echo hello world >>hello.txt`，看看会得到什么。

### 从文件中输入

完成上面两个例子，我们可以试试 `cat <hello.txt`，这个命令会将 `hello.txt` 文件中的文本内容打印出来。

### 更复杂的例子

myshell 同样支持文件描述符的重定向（限制为标准输入输出和错误文件）和多个重定向。如果您装有 `apt-get` 的话可以试试下面的例子。

`apt-get` 有一个众所周知的彩蛋，键入 `apt moo` 会打印一头牛，我们可以重定向这个输出。（`apt` 是 `apt-get` 的一个链接）。

`apt moo` 会直接打出这头牛。而 `apt moo 1>/dev/null` 则会打印出一条警告信息：

`WARNING: apt does not have a stable CLI interface. Use with caution in scripts.`

上面的命令中，`1` 的含义是文件描述符为 1 的文件，即标准输出。

`apt-get` 在运行时会检查当前输出是否为终端，如果不是则会发出这条警告，因为它有运行时中断的风险。这样的警告在使用管道重定向 `apt-get` 时尤为常见。命令中的 `/dev/null` 俗称为黑洞，是Linux 的一个空设备，没有任何反馈。

如果我们尝试 `apt moo 1>/dev/null 2>/dev/null` 这条命令，整条命令会变得彻底悄无声息。

我们还可以交换标准输出和标准错误文件，这样我们同样会得到 `apt-get` 的报错，但是报错会写到标准输出而不是标准错误上。命令为 `apt moo 2>&1 1>&2`。

上面的例子都在 myshell 上测试通过了。

管道
----

管道即将一个命令的输出作为下一个命令的输入。管道使用连接符 `|`。

您可以尝试下面这个命令：`gerp --help | more`，它将 `grep` 的帮助信息输出到 `more` 程序的输入，使得您可以用浏览的方式查看该帮助文档。

或者是 `ls | cat`，这个命令可以列出当前目录的所有文件并列出。

环境变量
--------

在使用 shell 时，您会有一个当前操作目录(current working directory)，所有相对路径都会由系统在这个当前操作目录查找。

### 打印当前操作目录

我们可以在命令提示符处看到当前操作目录，除此之外，还有可以使用 myshell 命令打印当前操作目录。

打印当前操作目录有两种方式，myshell 提供了一个名为 `pwd` 的内建命令用于打印当前操作目录。您只需要输入 `pwd` 即可。

myshell 提供了环境变量的替换，这意味着您同样可以使用 `${PWD}` 或 `$PWD` 得到当前目录。试试 `echo $PWD` 或 `echo ${PWD}`。

### 改变当前操作目录

改变当前目录可以使用内建命令 `cd` (change directory)，您只需在 `cd` 后面加上要转到的目录路径就行了。当 `cd` 命令没有参数的时候，会默认改变当前目录到 `$HOME`（你可以试试 `echo $HOME` 看看会打印出什么）。

特别地，`cd ..` 可以跳转到上一级目录，这是 Unix/Linux 系统的一个特性。

### 打印所有环境变量

大多数 Linux 发行版都提供了 `env` 程序，可以打印当前程序的所有环境变量。myshell 不提供导出功能（不支持 `export`），也同样不覆写系统提供的 `env`，在 myshell 中输出的是原封不动的程序运行环境。但是 myshell 加赠了一个环境变量称作 `PARENT` 其值即 myshell 的程序位置。

### 变量的定义与替换

myshell 支持常见的变量定义方式，并提供变量替换的功能。例如，`$@` 会输出当前正在运行的myshell 的所有参数，`$#` 则会输出参数的数目。具体到变量可以使用 `${#var[@]}` 访问到名字为`var` 的数组中的元素个数。`${#var}` 则是这个数组第 0 个元素的字符串长度（变量皆视作长度为 1的数组，在绝大多数 shell 实现中，变量都被统一处理成字符串数组，字符串是唯一存在的东西）。访问变量的值我们刚刚已经接触过了，花括号加与不加在大多数情况中作用是一致的。不加花括号时，变量扩展将会按照贪婪的方式向后匹配字符，直至遇到不合法的变量名字符和符号 (token) 末尾为止。两者区别最显著之处在于 `$10` 和 `${10}` 是不一样的，前者只会替换 `$1`，而保留 `0` 作为字符，后者会将 `10` 视作一个整体去进行替换。

变量的定义使用 `=`，需要注意的是，变量名与等号之间是不能有空白字符的。等号的右侧可以为任何字符串，如果右边的串不加引号的话，会匹配到空白字符为止。

数组的定义使用 `var=(arg1 arg2 arg3)` 的形式，参数与参数之间使用空格隔开。

当前程序参数的更改可以使用内建的 `set` 命令。myshell 只实现 `set` 的无选项子集。命令
`set arg1 arg2 arg3` 会将 `$1`、`$2` 和 `$3` 分别设置为 `arg1`、`arg2` 和 `arg3`。

尝试如下会话：

```
$ set arg1 arg2 arg3
$ echo $@
./myshell arg1 arg2 arg3
```

如果想避免诸如 `${10}` 的变量访问方式，您可以使用 `shift` 命令将参数向前移一位，接续上面的会话：

```
$ shift
$ echo $@
arg1 arg2 arg3
```

以上特性均在 myshell 上测试通过。

任务控制
--------

myshell 支持任务控制，目前似乎还存在一些小问题。与 bash 任务控制不同的是，myshell 就是以任务为中心的，每个输入命令都对应于一个任务。编号原则与实现方式也和 bash 有所不同，myshell 中任务的编号是从开始运行之后顺次向下编的，第一条命令对应于任务 0，第二条对应于任务 1，依次类推。

### 创建后台任务

创建后台任务在命令后加 `&` 即可。

与 bash 行为不同，myshell 不将 `&` 视作分割后台任务的字符，而视作连接后台进程流水线的连接符。`sleep 10 & sleep 10 &` 将创建一个后台任务，这个后台任务的进程流水线拥有两个进程，每个都是等待 10 秒钟的操作。

### 查看所有任务

`jobs` 是一个内建命令，可以列出所有任务的任务号、完成状态和对应命令。你或许会看到 `jobs` 这个任务本身也赫然在列，这是由于 myshell 和 bash 的任务控制机制不同所致。

### 切换前台后台任务

前台任务和后台任务的区别在于是否占据终端的输入输出。myshell 中，您可以改变任务的前台后台状态。

一般地，没有被创建为后台任务的命令会被自动创建为前台任务，前台任务中您可以使用 `Ctrl + Z` 发送停止信号，使任务被挂起。

使用 `jobs` 可以列出任务，包括被挂起的任务，`fg [任务号]` 可以将任务重新放回前台 `bg [任务号]` 则会对任务中的进程发送继续信号，让进程继续。

写在最后
--------

如果您要写一个实用的 shell，绝大多数建议都是不要这样做，因为已经有很多足够好的解决方案，且有很多比 shell 好的解决方案，可以让 shell 少承担一些负担，比如各种脚本语言。虽说如此，自己写一个 shell 会极大提升对 Unix 和 Linux 的认知。

这一文档以使用说明为主，有很多地方会流露出自己在编写过程中对 shell 的理解，希望这一文档，连同这一项目的其他文档和源代码会对您有所启发。