/**
 * 
 * Sieve of erathostenes
 * Search the N first prime numbers
 * author: Victor Correal Ramos
 * 
*/

#include<stdio.h>    
#include<stdlib.h>  
#include<stdbool.h>
#include<omp.h>

#include "trace.h"

#ifdef _DBG
    #define LOG printf
#else
    #define LOG //
#endif

static inline void arrayInitV1(bool * a, int n){
    #pragma omp for schedule(static)
    for(int i = 0; i < n; ++i){
        LOG("Thread %i: %i\n", omp_get_thread_num(), i);
        a[i] = true;
    }
}

static inline void arrayInitV2(bool * a, int n){
    int tid = omp_get_thread_num();
    int nT = omp_get_num_threads();
    for(int i = tid; i < n; i = i+nT){
        LOG("Thread %i: %i\n", omp_get_thread_num(), i);
        a[i] = true;
    }
}


int main(int argc, char *argv[]){

#ifdef _DBG
printf("DEBUG MODE ENABLE, may affect the performance!\n");
#endif

double globalTime;
init_timer(&globalTime);

    if(argc != 3){
        printf("USAGE executable <num prime> <num_threads>\n");
        exit(0);
    }

    int N = atoi(argv[1]);
    int nThreads = atoi(argv[2]);

    if (nThreads > omp_get_max_threads()){
        nThreads = omp_get_max_threads();
        printf("Too many threads where requested, hardware only have support for %i\nSetting num_threads to MAX\n", nThreads);
        
    }

    //alloc memory for the results: 0..N+1
    bool * prime = malloc(sizeof(bool)*(N+1));
    
    //init the array
    #pragma omp parallel num_threads(nThreads)
    {        
        //array inicialization v1
        arrayInitV2(prime, N+1);
        
        #pragma omp barrier
        
        for(int p = 2; p*p <= N; p++){
            if(prime[p]){
                
                #pragma omp for schedule(static)
                for(int i = p*p; i <= N; i+=p){
                    prime[i] = false;
                }
            }
        }
    }

#ifdef _DBG
    //show the result
    printf("Results: \n");
    for(int p = 0; p < N; p++){
        if(prime[p]) printf("%i ", p);
    }
    printf("\n");
#endif

    end_timer(globalTime, "global time");

}
