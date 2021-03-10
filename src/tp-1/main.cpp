#include <omp.h>
#include <iostream>
#include "tracy/Tracy.hpp"

#define TRACY_NO_EXIT

double compute_pi_single_thread()
{
    ZoneScoped
    const long nb_pas = 1000000000;
    const double pas = 1.0 / (double) nb_pas;
    double x, pi, som = 0.0;
    
    for (int i = 0; i < nb_pas; i++)
    {
        x = (i + 0.5) * pas;
        som = som + 4.0 / (1.0 + x * x);
    }

    return pas * som;
}

double compute_pi_multi_thread()
{
    ZoneScoped
    const long nb_pas = 1000000000;
    const double pas = 1.0 / (double) nb_pas;
    double x, pi, som = 0.0;
    
    // Reduction operation to prevent data-race
    // It creates a private variable for each thread and apply the given operation on them
    #pragma omp parallel for reduction(+ : som)
    for (int i = 0; i < nb_pas; i++)
    {
        // Cannot put tracy profile here are it does a data race
        x = (i + 0.5) * pas;
        som = som + 4.0 / (1.0 + x * x);
    }

    return pas * som;
}

int main(int argc, char const *argv[])
{
    omp_set_num_threads(4);

    // Ex1
    std::cout << "-- Ex1 --" << std::endl;
    #pragma omp parallel
    {
        ZoneScopedN("Ex1")
        std::cout << omp_get_thread_num() << std::endl;
    }

    int val1 = 1000;
    int val2 = 2000;

    // Ex2
    std::cout << std::endl << "-- Ex2 --" << std::endl;
    #pragma omp parallel private(val2)
    {
        ZoneScopedN("Ex2")
        val2 = 2000;
        val1++;
        val2++;
    }
    std::cout << val1 << std::endl;
    std::cout << val2 << std::endl;

    // Ex3
    std::cout << std::endl << "-- Ex3 --" << std::endl;
    #pragma omp parallel for
    for (int i = 1; i < 50; i++)
    {
        ZoneScopedN("Ex3")
        std::cout << "thread: " << omp_get_thread_num() << " i: " << i << std::endl;
    }

    // Ex4
    ZoneScopedN("Ex4")
    std::cout << std::endl << "-- Ex4 --" << std::endl;

    std::cout << "Pi single threaded" << std::endl;
    double start = omp_get_wtime();
    std::cout << compute_pi_single_thread() << std::endl;
    std::cout << "Elapsed seconds : " << omp_get_wtime() - start << std::endl << std::endl;

    std::cout << "Pi multi threaded" << std::endl;
    start = omp_get_wtime();
    std::cout << compute_pi_multi_thread() << std::endl;
    std::cout << "Elapsed seconds : " << omp_get_wtime() - start << std::endl;

    return 0;
}
