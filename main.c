#include <stdio.h>

int hello_world(int n) {
    for (int i=1; i < n+1; i++) {
        printf("Hello, World!\n");
    }

    return 0;
}


int main(int argc, char** argv){
    puts("Hello, World!");
    
    int n = 0;

    for (int i = 1; i < 6; i++) {
    printf("%i Hello, World!\n", i);
    };

    printf("\n");
    printf("Please enter times you wish to print hello world: ");
    scanf("%d", &n);
    hello_world(n);

    return 0;
}