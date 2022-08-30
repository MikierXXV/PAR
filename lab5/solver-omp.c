#define lowerb(id, p, n)  ( id * (n/p) + (id < (n%p) ? id : n%p) )
#define numElem(id, p, n) ( (n/p) + (id < (n%p)) )
#define upperb(id, p, n)  ( lowerb(id, p, n) + numElem(id, p, n) - 1 )

#define min(a, b) ( (a < b) ? a : b )
#define max(a, b) ( (a > b) ? a : b )

#include "heat.h"
#include "omp.h"

extern int userparam;

// Function to copy one matrix into another
void copy_mat (double *u, double *v, unsigned sizex, unsigned sizey) {
    int nblocksi=8;
    int nblocksj=1;
    #pragma omp parallel 
    {
    for (int blocki=0; blocki<nblocksi; ++blocki) {
      int i_start = lowerb(blocki, nblocksi, sizex);
      int i_end = upperb(blocki, nblocksi, sizex);
      for (int blockj=0; blockj<nblocksj; ++blockj) {
      int j_start = lowerb(blockj, nblocksj, sizey);
      int j_end = upperb(blockj, nblocksj, sizey);
      for (int i=max(1, i_start); i<=min(sizex-2, i_end); i++)
        for (int j=max(1, j_start); j<=min(sizey-2, j_end); j++)
        v[i*sizey+j] = u[i*sizey+j];
      }
    }
  }
}

// 2D-blocked solver: one iteration step

//JACOBI
double jacobi(double *u, double *unew, unsigned sizex, unsigned sizey) {
    double tmp, diff, sum=0.0;

    //int nblocksi=8;
    //int nblocksj=1;
    #pragma omp parallel private(tmp,diff) reduction(+:sum)
    { 
    int myid = omp_get_thread_num();
    int how_many = omp_get_num_threads();
    int i_start = lowerb(myid, how_many, sizex);
    int i_end = upperb(myid, how_many, sizex);
    for (int i=max(1, i_start); i<=min(sizex-2, i_end); i++) {
      for (int j=1; j<=sizey-2; j++) {
      tmp = 0.25 * ( u[ i*sizey    + (j-1) ] +  // left
               u[ i*sizey    + (j+1) ] +  // right
               u[ (i-1)*sizey + j     ] +  // top
               u[ (i+1)*sizey + j     ] ); // bottom
        diff = tmp - u[i*sizey+ j];
        sum += diff * diff;
        unew[i*sizey+j] = tmp;
      }
    }
  }

    return sum;
}

//GAUSS
/*
double gauss (double *u, unsigned sizex, unsigned sizey) {
    double tmp, diff, sum=0.0;
  
    /*
     * 
     * barrera i actualizacion del bloque (2 codigos)
     * int nblocksi=8;
    int nblocksj=1;
    int how_many = omp_parallel_num_threads();
    #pragma omp parallel private(tmp,diff) reduction(+:sum)
    for (int blocki=0; blocki<how_many; ++blocki) {
      int i_start = lowerb(blocki, how_many, sizex);
      int i_end = upperb(blocki, how_many, sizex);
      for (int blockj=0; blockj<how_many; ++blockj) {
        int j_start = lowerb(blockj, how_many, sizey);
        int j_end = upperb(blockj, how_many, sizey);
        #pragma omp depend(sink:blocki-1, blockj)
        for (int i=max(1, i_start); i<=min(sizex-2, i_end); i++) {
          for (int j=max(1, j_start); j<=min(sizey-2, j_end); j++) {
          tmp = 0.25 * ( u[ i*sizey    + (j-1) ] +  // left
                           u[ i*sizey    + (j+1) ] +  // right
                           u[ (i-1)*sizey + j     ] +  // top
                           u[ (i+1)*sizey + j     ] ); // bottom
          diff = tmp - u[i*sizey+ j];
          sum += diff * diff;
          u[i*sizey+j] = tmp;
          }
        }
        #pragma omp depend(source)
      }
    }

    return sum;
}
*/


double gauss (double *u, unsigned sizex, unsigned sizey) {
  double tmp, diff, sum=0.0;

  int nblocksi=8;
  int nblocksj=1;

  //ESPERA
  //vector que nos va a decir que bloques no estan proc :)
  //meter un if para no entrar en los 2 for de dentro si el vector NO me lo habilita (lectura atomica)

  //ACTUALIZAR ESTADO
  //actualizar el vector segun termino / consistencia de memoria:

  //int whichone = 0;

  int P = nblocksi*nblocksj;
  int next[P];
  next[0] = 1;
  for (int k = 1; k < P; ++k) next[k] = 0;

  #pragma omp parallel num_threads(P)
    {  
      int myid = omp_get_thread_num();

    //if (u == unew) whichone = 0;        
    //else whichone = 1;
    while (myid < P){

        int tmp;
        do {
            #pragma omp atomic read
            tmp = next[myid-1];    
        } while (tmp == 0);


        int how_many = omp_parallel_num_threads();
        #pragma omp parallel private(tmp,diff) reduction(+:sum)
        for (int blocki=0; blocki<how_many; ++blocki) {
          int i_start = lowerb(blocki, how_many, sizex);
          int i_end = upperb(blocki, how_many, sizex);
          for (int blockj=0; blockj<how_many; ++blockj) {
            int j_start = lowerb(blockj, how_many, sizey);
            int j_end = upperb(blockj, how_many, sizey);
            for (int i=max(1, i_start); i<=min(sizex-2, i_end); i++) {
              for (int j=max(1, j_start); j<=min(sizey-2, j_end); j++) {
              tmp = 0.25 * ( u[ i*sizey    + (j-1) ] +  // left
                               u[ i*sizey    + (j+1) ] +  // right
                               u[ (i-1)*sizey + j     ] +  // top
                               u[ (i+1)*sizey + j     ] ); // bottom
                    diff = tmp - u[i*sizey+ j];
                    sum += diff * diff;
                    u[i*sizey+j] = tmp;
              }
            }
          }
        }
         if (myid < P-1){
            #pragma omp atomic write
            next[myid] = 1;
        }
    }
    
  }
  return sum;
}
