#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <omp.h>

using namespace std;

/*
Implementa un histograma para la entrada hist.in. Puede ser una buena idea 
utilizar el map de c++, pero si quereis hacerlo de otra manera tambien se puede.

Se proporciona un codigo para leer de un fichero con un canal de entrada.

Implementa usando task y sin usarlo. Recuerda que para compilar tienes que añadir el 
flag -fopenmp y la libreria OMP #include<omp.h>

map reference
http://www.cplusplus.com/reference/map/map/
*/

int main(){

	#pragma omp parallel
	{
	    ifstream ifs("hist.in");
	    int aux;
	    for(int i = 0; i < 10; ++i){
	        ifs >> aux;
	        cout << aux << endl;
	    }
		#pragma omp single 
		for(int i = 0; i < 1000000; i += chunk.size()){
			vector<int> aux;
			for(int j = i; j < i+chunk.size(); ++j){
				aux.push_back(j);
	        	#pragma omp task


			}  
	    }
	    // cada posicion hace referencia al numero del histograma
	    vector<int> v(100,0);
	    while (ifs << aux){
	    	v[aux]++;
	    }
	   
	}
    

}