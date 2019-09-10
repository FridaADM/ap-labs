#include <stdio.h>
#include <stdlib.h>

int main() {
    char *q = NULL;

    printf("Requesting space for \"Goodbye\"\n");
    q = (char *)malloc(strlen("Goodbye")+1);
    p = (char *)malloc(strlen("Goodbye")+1);
    printf("About to copy \"Goodbye\" to q at address %u\n", q);
    strcpy(q, "Goodbye");
    strcpy(q, "lajsudbcewibier");
    strcpy(p, "Goodbye");
    printf("String copied\n");
    printf("%s\n", q);
    printf("%s\n", p);

    return 0;

}
