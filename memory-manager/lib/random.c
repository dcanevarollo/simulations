#include "random.h"

/* Uses the processors's clock to generate random numbers.  */
int rdtsc() {
    __asm__ __volatile__("rdtsc");
}

int getRandom(int min, int max) {
    srand(rdtsc());
    return (rand() % (max - min + 1) + min);
}