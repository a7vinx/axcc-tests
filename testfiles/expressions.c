// Expressions tests.


struct S {
    int m1;
    int m2;
};

void f(int a, int b) {
}

void expressions() {
    int i1 = 0;
    int i2 = 4 * (2 + 3) >> 1;
    _Bool b1 = (i1 > 2) && 1 || 2;
    int a1[10];
    ++i1 ? 1 : 2 = 3;
    f((long)(i1 + i2), (char)a1[i1]);

    // Compound literals.
    (struct S){1, 2};
    (int []){1, 2, 3,};

    // Generic selection.
    _Generic(i1, long: 2, int: 3);
    _Generic(i1, long: 2);
    // Extra default label.
    _Generic(i1, default: 3, default: 4);

    // Undeclared identifier.
    undecl_ident;
}
