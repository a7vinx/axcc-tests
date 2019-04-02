// Object declarations tests.
// Declarations.
extern int g1;

// Tentative definitions.
int g2;
int g2;
static int g3;
int g4[];

// Extern declaration follows non-extern declaration.
extern int g2;

// Non-extern declaration follows extern declaration.
int g1;
static int g1;

// Conflicting types.
_Bool g1;

// Illegal storage class.
register int g5;

// Illegal specifiers.
inline _Noreturn int g6;

void localvars() {
    int l1;
    double l2;
    long long l3;
    unsigned short l4;
    signed char l5;
    _Bool l6;
    long* l7;
    long*** l8;

    // Qualifiers.
    const int l9;
    short const* const l10;
    restrict char* l11;
    char* restrict l12;
    volatile int l13;

    // Storage specifiers.
    extern int l14;
    extern int g1;
    static int l15;
    auto int l16;
    int register l17;

    // Non-extern declaration follows extern declaration.
    int g1;
    static long l14;

    // local variable redefinitions.
    int l1;
    _Bool l2;

    // Error recovery.
    void x;
    x = 2;
}
