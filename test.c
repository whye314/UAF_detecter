#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **args){
    int* a=malloc(sizeof(int));
    printf("a: %p\n", a);
    printf("%d\n", *(a-2));
    free(a);
    printf("%d\n", *(a-2));
}
