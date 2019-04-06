// Initializers tests.
struct S1 {
    int m1;
    int m2;
} s1_1;

// Scalar initializers.
int i1 = 3 + 2 + 3.2;
int i2 = {2};
int i3 = (int)(int*)3;
// Compile-time constant expected.
int i4 = s1_1.m1;

// Pointer initializers.
int* p1 = &i1 + 2;
char* p2 = "AAAA" + 2;
int* p3 = &(s1_1.m1);
// Compile-time constant expected.
int* p4 = (int)&i1;


// Array initializers.
int a1[10] = {1, [4] = 5};
int a2[] = {1, [6] = 7};
_Static_assert(sizeof(a2) == 28, "incorrect sizeof(a2)");

// '}' expected.
int a3[10][10] = {1, {1, 2, }, 1};

// String literal.
char a4[10] = "AAAA";
char a5[10][10] = {"AAAA", [2] = "AAAA"};
unsigned int a6[10] = U"AAAA";

// Initialize char array with incompatible string literal.
unsigned short a7[10] = U"AAAA";


// Struct/Union initalizers.
struct S2 {
    int m1;
    int m2;
    struct S2Inner {
        int m1;
        int m2;
    } m3;
    int m4;
    int m5;
} s2_1 = {1, .m3.m1 = 1, 2, 4, 5};

struct S2 s2_2 = s2_1;
struct S2Inner s2i_1 = {1, 2};
struct S2 s2_3 = {.m3 = s2i_1};


// Put them together.
struct S3 {
    int m1[10];
    int m2;
} s3_1 = {.m1[2] = 3, 4, 5, .m2 = 2};


void initializers() {
    int i1 = 1;
    int i2 = i1;
}
