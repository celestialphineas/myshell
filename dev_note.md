# myshell dev note

## Specification

PS1 and PS2 defined in POSIX are read-only in myshell

Shell variables like $0 - $9 and $PS1 and $PS2 should be pushed in the hash
table.

Shift support

## A list of shell variables

- HOSTNAME
- MYSHELL_PATH
- PS1
- PS2

## TODO

- [ ] signals, handle the keyboard interrupt, etc...
- [x] Hash map find
- [x] Test hash map
