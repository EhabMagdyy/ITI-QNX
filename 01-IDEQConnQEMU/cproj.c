#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Hello Ehab from QNX on QEMU!\n");

    for(int i = 0; i < 10; i++){
        printf("%d ", i);
        fflush(stdout);
        sleep(1);
    }
    printf("\n");

    return 0;
}
