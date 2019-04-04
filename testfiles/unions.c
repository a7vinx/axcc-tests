// Unions tests.
// Don't repeat the part that has been tested in structs.c
// Definitions.
union U1 {
    int i1;

    // Bit fields.
    short b1 : 3;

    // Anonymous union.
    union {
        long i2;
    };

    // Nested declaration.
    union U1Inner {
        long i3[2];
    } u1i_1;
} u1;

_Static_assert(sizeof(union U1) == 16, "incorrect sizeof(union U1)");
_Static_assert(_Alignof(union U1) == 8, "incorrect alignof(union U1)");
