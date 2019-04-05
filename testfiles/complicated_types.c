// Complicated types tests.
// Pointers.
int (*p1)();
int (*p2)[10];
int (*(*p3)())();
int (*(*p4)[10])();
int (*const (*const p5)[10])();

// Arrays.
int *a1[10];
int (*a2[10])();

// Functions.
int *f1();
int (*f2(int, int))(int);

// Typedefs
typedef const int *typedef_t1;
volatile typedef_t1 t1;
typedef int (*typedef_t2)();
typedef_t2 t2();

// Malformed types.
// Function returning an array.
int m1()[10];
// Pointer to function returning a function.
int (*m2)()();
// Pointer to an array of function returing a pointer.
int *(*m3)[10]();
