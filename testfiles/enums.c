// Enumerations tests.
// No such forward declaration in C.
enum E1;

// Definitions.
enum E1 {
    E1_Item1,
    E1_Item2 = 8,
} e1_1;

enum {
    // Enumertator redefinition.
    E1_Item1
};

int i1 = E1_Item1;
int i2 = E1_Item2;
