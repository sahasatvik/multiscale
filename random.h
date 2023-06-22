#ifndef _RANDOM_H
#define _RANDOM_H

#include <stdbool.h>

/* Get a uniformly random number from [0, 1] */
double random_uniform();
/* Get an exponentially distributed random number with mean beta */
double random_exponential(double beta);
/* Get a normally distributed random number with mean mu, standard deviation sigma */
double random_normal(double mu, double sigma);

/* Flip a biased coin */
bool   random_bernoulli(double p);
/* Get a random index from 0 to max - 1 */
int    random_index(int maxindex);

#endif
