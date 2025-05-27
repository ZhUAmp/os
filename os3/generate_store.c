#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    FILE *f = fopen("BACKING_STORE.bin", "wb");
    srand((unsigned int) time(NULL));
    for (int i = 0; i < 65536; i++) {
        unsigned char val = rand() % 256;
        fwrite(&val, sizeof(unsigned char), 1, f);
    }
    fclose(f);
    return 0;
}
