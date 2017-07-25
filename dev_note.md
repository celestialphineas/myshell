# myshell dev note

## Specification

PS1 and PS2 defined in POSIX are read-only in myshell

Shell variables like $0 - $9 and $PS1 and $PS2 should be pushed in the hash
table.

Shift support

## Something I have learnt

- The essence of shebang
- 子进程信号的捕获，应当采取循环等待任何信号，再处理的方式。
- 父进程的SIGCHLD不应当忽略。否则将没有办法获得子进程的返回值。
- 当PGID不再存在时setpgid会发生错误

## A list of shell variables

- HOSTNAME
- MYSHELL_PATH
- PS1
- PS2

## Reminders

Don't forget the input command line should be an array of int, for you might
need to handle the EOF (Ctrl + D)... Or there are probably other methods to
handle this.

## TODO

- [ ] signals, handle the keyboard interrupt, etc...
- [x] Hash map find
- [x] Test hash map
- [ ] Parser
- [ ] Evaluate the tokens
- [ ] Data structure for commands
- [x] Signal binding of SIGINT

# Not fully implemented

- parser