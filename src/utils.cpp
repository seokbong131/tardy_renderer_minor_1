#include "utils.h"

#include <iostream>

#include <omp.h>

#pragma warning(push)
#pragma warning(disable : 6993)
void print_openmp_info() {
// check compiler support about _OPENMP macro
#ifdef _OPENMP
    std::cout << "[INFO] OpenMP version: " << _OPENMP << std::endl;
    std::cout << "[INFO] max threads: " << omp_get_max_threads() << std::endl;
    std::cout << "----------------------------------------" << std::endl;

#    pragma omp parallel
    {
#    pragma omp critical
        std::cout << "thread " << omp_get_thread_num() << "\t\t(of " << omp_get_num_threads() << ")"
                  << std::endl;
    }
#else
    std::cout << "OpenMP is not enabled." << std::endl;
#endif
}
#pragma warning(pop)
