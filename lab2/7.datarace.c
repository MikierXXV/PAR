#include <stdio.h>
#include <omp.h>
/* Execute several times before answering the questions    */
/* with ./3.datarace                                       */
/* Q1: Is the program executing correctly? If not, explain */
/*     why it is not providing the correct result for one  */
/*     or the two variables (countmax and maxvalue)        */
/* Q2: Write a correct way to synchronize the execution    */
/*     of implicit tasks (threads) for this program.       */

#define N 1 << 20 
int vector[N]={0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 15, 14, 13, 12, 11, 10, 9, 8, 15, 15};

int main() 
{
    int i, maxvalue=0;
    int countmax = 0;

    omp_set_num_threads(8);
    #pragma omp parallel private(i) reduction(+: countmax) reduction(max: maxvalue)
    {
        int id = omp_get_thread_num();
        int howmany = omp_get_num_threads();

        //privatizamos con variables el maximo valor i el 
        //contador de maximos i las anteriores creadas
        //se quedarian como shared
        int maxvalueP = 0;
        int countmaxP = 0;

        for (i=id; i < N; i+=howmany) {
            if (vector[i]==maxvalueP) countmaxP++;
            if (vector[i] > maxvalueP) {
               maxvalueP = vector[i];
               countmaxP = 1;
            }
        }
        //una vez acaaba el bucle falta recoger los valores
        //adoptados por las variables privadas para mostrar
        //el resultado correcto en las privadas
        #pragma omp critical
        {
            printf("thread %i in critical\n", id);
            if (maxvalueP >= maxvalue){
                maxvalue = maxvalueP;
                countmax = countmaxP;
            }
            else if (maxvalueP == maxvalue){
                countmax += countmaxP;
            }
        }
    }

    if ((maxvalue==15) && (countmax==3)) 
         printf("Program executed correctly - maxvalue=%d found %d times\n", maxvalue, countmax);
    else printf("Sorry, something went wrong - maxvalue=%d found %d times\n", maxvalue, countmax);

    return 0;
}
