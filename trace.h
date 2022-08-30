#include <omp.h>


static inline void init_timer(double * tStart){
    *tStart = omp_get_wtime();
}



static inline void end_timer(double tStart, const char * txt){
    double tEnd = omp_get_wtime();

    printf("%s -> %f\n", txt, tEnd-tStart);

}
