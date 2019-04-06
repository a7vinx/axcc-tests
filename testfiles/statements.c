// Statements tests.
int statements() {
    int i1 = 1;
begin:
    goto begin;
    goto undecl_label;

    // If statement.
    if (i1)
        i1 = 2;
    if (i1)
        i1 = 1;
    else {
        i1 = 0;
    }
    // Statement expected.
    if (i1) int m;

    // While-loop statement.
    while (i1) {
        continue;
        break;
        // Do-while-loop statement.
        do {
            continue;
            break;
        } while (1);
    }

    // For-loop statement.
    for (;;) {
        continue;
        break;
    }

    // Switch statement.
    switch (i1) {
        case 1: {
            // Nested switch statement.
            switch (i1) {
                int i2 = 0;
                case 1: break;
            }
        }
        case 2: i1 = 3; break;
        // Duplicated case value.
        case 2: break;
        default: ;
        // Extra default label.
        default: ;
    }

    // continue not in loop statement.
    continue;
    // break not in loop or switch statement.
    break;

    return i1;
    return;
    long i3;
    return i3;
}
