// Function declaration tests.
// Declarations.
int f1(void);
int f1(void);
extern int f1(void);
void f2(int a, int b);
void f2(int, int);
static int f3(int);
// K&R style function declaration.
int f1();
int f1(a, b);

// Declaration with parameters of incomplete types.
int f4(struct S s);

// Non-extern declaration follows extern declaration.
static int f1();

// Conflicting types.
void f1();
int f1(int a, int b);

// Declaration with parameters of void type.
int f5(void t);

// Declaration with malformed parameters.
int f6(int, void);

// Definitions.
int f1(void) {
}

void f2(int a, int b) {
    // Nesetd function declarations.
    extern void f2();
    void nested_func1(int, int);
    // Illegal storage class.
    static int nested_func2();

    a = 3;
}

// Function definition with parameters of incomplete types.
int f4(struct S s) {
}

// K&R style function definition.
void kr_func_def1(a, b, c)
int a, b;
char c;
{
}

// Unmatched K&R style function definition.
void kr_func_def2()
int a;
{
}

void kr_func_def3(a, b)
{
}

// Redefinitions.
int f1() {
}
