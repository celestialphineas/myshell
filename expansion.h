// Shell expansion
// ===============
// Author: Celestial Phineas @ ZJU
//         (Yehang YIN)
// This file does the shell expansion
// ------------------------------------------------------------------
// The bash do the shell expansions in the order below:
//      1 Brace expansion
//      2 Tilde expansion
//      3 Shell parameter expansion
//      4 Command substitution
//      5 Arithmetic expansion
//      6 Process substitution
//      7 Word splitting
//      8 Filename expansion
//      9 Pattern matching
//      10 Quote Removal
// ------------------------------------------------------------------
// In myshell, the brace expansion feature will not be implemented.
// No command substitution as well, since subshell is not supported.
// No arithmetic expansion.
// And the process substitution is done in the job control phase.
// Word splitting, quote removal are implemented in preprocessor.
// Filename expansion and pattern matching will not be implemented.
// No shell-level wildcards are supported.
// ------------------------------------------------------------------
// Here is what myshell do:
//      1 Tilde expansion
//      2 Shell parameter expansion (variable expansion)
//      3 Escape character expansion
#ifndef CELPHI_EXPANSION_H
#define CELPHI_EXPANSION_H
#include "tokenizer.h"
#include "global.h"

// This function checks if the character pointer "ch" points to an escaped char
// Argument "head" is the starting address of the string
int is_escaped(char *ch, char *head);
// Tilde expansion
// Note that all expansion functions allocate space for the return string
// Rules (These are not regex):
//      [^]~[\0]            $HOME
//      ~/                  $HOME/
//      [^]~+[\0]           $PWD
//      ~+/                 $PWD/
// The standard behavior of tilde expansion in bash is to do only once
// myshell will follow this.
char *tilde_expansion(char *str);
// Shell parameter expansion
// Mode matching (These are not regex):
//      $#                              myshell argc
//      $[@|*]                          myshell argv
//      $[digit]                        myshell arg
//      $[identifier]                   variable
//      ${[string]}                     variable
//      ${#[string]}                    variable string length
//      ${#[identifier][ [@|*] ]}       variable count
//      ${[identifier][ [iteger] ]}     variable element
//      ${[identifier][ [@|*] ]}        variable value
char *var_expansion(char *str);

// Escape expansion
// The escape expansion only deals with shell-defined expansions
// Bash escape all character after '\\' and remove the very first '\\'
char *escape_expansion(char *str);
#endif