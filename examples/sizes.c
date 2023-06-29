#include <stdio.h>


struct A {
    char a;
};

struct B {
    char a;
    unsigned long long b;
};

struct C {
    char a;
    struct B b;
};

int main(void)
{
    printf("sizeof(char) = %zu\n", sizeof(char));
    printf("sizeof(void*) = %zu\n", sizeof(void*));
    char a[2];
    printf("sizeof(char a[2]) = %zu\n", sizeof(a));
    char *b;
    printf("sizeof(char *b) = %zu\n", sizeof(b));

    printf("sizeof(struct A) = %zu\n", sizeof(struct A));
    printf("sizeof(struct B) = %zu\n", sizeof(struct B));
    printf("sizeof(struct C) = %zu\n", sizeof(struct C));

    return 0;
}
