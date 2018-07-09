// #include
#include "directives_include.h"
// file not found
#include "file_not_exist"
#include "\"file_not_exist_in_quotes\""
// expected "FILENAME" or <FILENAME>
#include wrong_token

// #define
#define obj_macro1 something
#define obj_macro2 something
#define func_macro1() something
#define func_macro2() something
// macro name must be an identifier
#define @
// 'defined' cannot be used as a macro name
#define defined
// missing ')' in macro parameter list
#define func_macro_wrong(p1,
// expected identifier in macro parameter list
#define func_macro_wrong(@, +) something
// expected comma in macro parameter list
#define func_macro_wrong(p1+) something
// macro redefined
#define func_macro2() something

// #undef
#undef obj_macro1
#undef func_macro1
// macro name must be an identifier
#undef 100

// conditional directive
#ifdef obj_macro1
#define obj_macro_ifdef_true something_2
void f1();
#else
#define obj_macro_ifdef_false something_2
void f2();
// #else after #else
#else
#endif
// #else without #if
#else
// #endif without #if
#endif
// macro name missing
#ifdef
#endif
// macro name must be an identifier
#ifdef @
#endif
// extra tokens at end of directive
#ifdef macro_not_exist extra tokens
#endif

// #line
#line 100 "new_name"
#line 100 "\"name_in_quotes\""
// invalid filename for #line directive
#line 100 not_a_name
// #line directive requires a positive integer argument
#line -100
// #line directive requires a simple digit sequence
#line 9f

// #error
#error
#error this is an "error"
