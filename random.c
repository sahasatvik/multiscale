#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "random.h"


double random_uniform() {
        return rand() / (RAND_MAX + 1.0);
}

double random_exponential(double beta) {
        return -log(random_uniform()) * beta;
}

double random_normal(double mu, double sigma) {
        double z = sqrt(-2 * log(random_uniform()))
                 * cos(2 * M_PI * random_uniform());
        return sigma * z + mu;
}

bool random_bernoulli(double p) {
        return rand() < (p * RAND_MAX);
}

int random_index(int max) {
        return rand() % max;
}
