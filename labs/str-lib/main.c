#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    if(argc != 4){
        exit(-1);
    }

    int strlen = mystrlen(argv[1]);
    char* newstr = mystradd(argv[1],argv[2]);
    printf("Original string length: %d\n",strlen);
    printf("New string: %s\n",newstr);
    int *found;
    printf("Substring was found : %s\n", found = mystrfind(newstr,argv[3])? "yes":"no");
    return 0;
}
