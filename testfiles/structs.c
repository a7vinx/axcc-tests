// Structs tests.
// Forword declarations.
struct S1;
struct S1;

// Definitions.
struct S1 {
    int m1;
    int m2[2];
    const int m3;
    // Illegal specifiers.
    static int m4;

    // Illegal members.
    int m5();
    // Here the size of this struct should be 20.

    // Bit fields.
    long b1 : 3;
    // Illegal bit fields.
    void* b2 : 2;
    _Bool b3 : 2;
    long b4 : 300;
    // Here the size of this struct should be 48.

    // Anonymous struct.
    struct {
        char c1;
    };
    // Here the size of this struct should be 49.

    // Nested declaration.
    struct S1Inner {
        _Alignas(8) char si_c1;
    } s1i_1;
    // Here the size of this struct should be 57.
} s1_1;

struct S1Inner s1i_2;

_Static_assert(sizeof(struct S1) == 64, "incorrect sizeof(struct S1)");
_Static_assert(_Alignof(struct S1) == 8, "incorrect alignof(struct S1)");

// Flexible array member.
struct S2 {
    int m1;
    int m2[];
};
struct S3 {
    int m1[];
};
struct S4 {
    int m1[];
    int m2;
};

_Static_assert(sizeof(struct S2) == 4, "incorrect sizeof(struct S2)");

// Redefinitions.
struct S1 {
    int m1;
};

void structs() {
    struct S1 s1_2, s1_3;
    _Static_assert(sizeof(s1_2) == 64, "incorrect sizeof(s1_2)");

    // Hide outer struct S1.
    struct S1 {
        int padding[10];
    };
    struct S1 s1_4;
    _Static_assert(sizeof(s1_4) == 40, "incorrect sizeof(s1_3)");

    // Member access.
    s1_3.c1 = 'a';
    s1_3.s1i_1.si_c1 = 'b';
    // Assign to const member.
    s1_3.m3 = 2;
    // Error Recovery.
    s1_3.b4 = 1;
}
