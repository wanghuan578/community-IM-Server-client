/*
 * File: random.h
 * Version: 1.0
 * Last modified on Fri Jul 22 16:44:36 1994 by eroberts
 * -----------------------------------------------------
 * This interface provides several functions for generating
 * pseudo-random numbers.
 */

#include <stdlib.h>
#include <time.h>
#include "SL_Random.h"

/*
 * Function: randomize
 * -------------------
 * This function operates by setting the random number
 * seed to the current time.  The srand function is
 * provided by the <stdlib.h> library and requires an
 * integer argument.  The time function is provided
 * by <time.h>.
 */
void SL_Random::randomize(int seed)
{
    if (seed == 0)
        ::srand((unsigned int)::time(NULL));
    else
        ::srand(seed);
}

/*
 * Function: randomInteger
 * -----------------------
 * This function first obtains a random integer in
 * the range [0..RAND_MAX] by applying four steps:
 * (1) Generate a real number between 0 and 1.
 * (2) Scale it to the appropriate range size.
 * (3) Truncate the value to an integer.
 * (4) Translate it to the appropriate starting point.
 */
int SL_Random::random_integer()
{
    return rand();
}

int SL_Random::random_integer(int low, int high)
{
    if (high < low)
    {
        int tmp = high;
        high = low;
        low  = tmp;
    }

    int k;
    double d;
    d = (double) (::rand() / ((double)RAND_MAX + 1));
    k = (int) (d * (high - low + 1));
    return (low + k);
}

/*
 * Function: randomReal
 * --------------------
 * The implementation of RandomReal is similar to that
 * of RandomInteger, without the truncation step.
 */
double SL_Random::random_real()
{
    return (double) (::rand() / ((double)RAND_MAX + 1));
}

double SL_Random::random_real(double low, double high)
{
    if (high < low)
    {
        double tmp = high;
        high = low;
        low  = tmp;
    }

    double d;
    d = (double) (::rand() / ((double)RAND_MAX + 1));
    return (low + d * (high - low));
}

/*
 * Function: randomChance
 * ----------------------
 * This function uses RandomReal to generate a number
 * between 0 and 100, which it then compares to p.
 */
bool SL_Random::random_chance(double p)
{
    return (random_real(0, 1) < p);
}

