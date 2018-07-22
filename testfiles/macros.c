// object-like macro
#define om1 something
om1
#define om2 ()
om2
// builtin macro
__FILE__
__LINE__

// function-like macro
#define fm1() (2 + 2)
fm1()
#define fm2(p1) p1
fm2()
fm2
fm2
()
#define fm3(p1, p2) (p1 + p2)
fm3(arg1, arg2)
fm3(arg1,)
// variadic function
#define fm4(p1, ...) (p1 + __VA_ARGS__)
fm4(@, 1, 2,,)
#define fm5(p1, p2, ...) (p1 + p2 + __VA_ARGS__)
fm5(om1, om2,
    fm1(), fm3(arg1 + arg2, arg1 - arg2))
// embedding directives within macro arguments is not supported
fm3(
#define not_supported1
#define not_supported2
arg1, arg2)
// too many arguments provided to function-like macro
fm3(arg1, arg2, arg3)
// too few arguments provided to function-like macro invocation
fm3(arg1)
// # and ## operators
#define sharp_macro1(p1) #p1
sharp_macro1(name)
sharp_macro1("\"str\"")
#define sharp_macro2(...) #__VA_ARGS__
sharp_macro2(1, 2,
3)
#define dsharp_macro1(p1, p2) p1##p2
dsharp_macro1(name1, name2)
dsharp_macro1(1, 2)
dsharp_macro1(+, =)
dsharp_macro1(,)
dsharp_macro1(,2)
#define dsharp_macro2(p1, ...) p1##__VA_ARGS__
dsharp_macro2(1, 2, 3)
#define dsharp_macro3(p1) name##p1
dsharp_macro3(_@)
// pasting formed '***', an invalid preprocessing token
dsharp_macro1(+, -)

// recursive macro
#define foo foo a bar b baz c
#define bar foo 1
#define baz bar 2
// It should be expanded to "foo a foo 1 b foo 1 2 c"
foo

// unterminated function-like macro invocation
fm3(p1,
