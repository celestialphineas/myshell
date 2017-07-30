// Built-in interface
// ==================
// Author: Celesital Phineas @ ZJU
//         (Yehang YIN)
// This file implements the interface for the built-in utilities
#ifndef CELPHI_BUILT_IN_H
#define CELPHI_BUILT_IN_H
#include "hash_map.h"
#include "message.h"

#ifndef true
#define true    (1)
#endif
#ifndef false
#define false   (0)
#endif
#ifndef BUILT_IN_HASH_ERR_
#define BUILT_IN_HASH_ERR_      9
#endif

// Usage:
// BuiltIn *built_in;
// built_in = get_built_in("echo");
// (*built_in)(argc, argv);

// BuiltIn is the universal type definition of the built-in functions
typedef int(*BuiltIn)(int, char**);

// Todo: don't forget to initialize it in init()
// Initialize the built-in table
void init_built_in_table();
// This function tells if a built-in function exists
int is_built_in(const char *str);
// And this one returns the function address
BuiltIn *get_built_in(const char *str);

#endif