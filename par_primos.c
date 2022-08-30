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
#include <time.h>

int main(int argc, char *argv[]){

#ifdef _DBG
printf("DEBUG MODE ENABLE, may affect the performance!\n");
#endif

    if(argc != 2){
        printf("USAGE executable <num prime>\n");
        exit(0);
    }

    int N = atoi(argv[1]);
    
    //call the OS from some dynamic space:
    int *prime = malloc(sizeof(bool)*(N+1));
    
    #pragma omp parallel num_threads(4)
    {
        #pragma omp for
        //init the array:
        for (int i = 0; i <= N; ++i){
            //calloc ( ,inicialicamos) SO
            prime[i] = true;
        }

    }

    #pragma omp parallel
    {
        //sieve: will be under sqrt(n)
        #pragma omp for
        for(int p = 2; p*p <= N; p++){
            if(prime[p]){
                //set false the multiples of p, under N:
                for(int i = p*p; i <= N; i+=p){
                    prime[i] = false;
                }
            }
        }
    }

#if _DBG
    //show the result:
    for(int p = 0; p < N; p++){
        if(prime[p]) printf("%i ", p);
    }
    printf("\n");    
#endif
}