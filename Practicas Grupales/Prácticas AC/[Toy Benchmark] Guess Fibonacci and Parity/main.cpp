#include "Utils.h"
#include "FunctionsC.h"
#include "FunctionsASM.h"
#include "Export.h"
#include "Concurrency.h"

int main() {
	printf("Generamos %d numeros aleatorios\n", VSIZE);
	long long initGen = milliseconds_now();
	unsigned long int* numbers = new unsigned long int[VSIZE];
	generateRandomNumbers(numbers, VSIZE);
	long long endGen = milliseconds_now();

	printf("Adivinamos si los numeros pertenecen a la serie de Fibonacci o no\n");	
	long long initFibo = milliseconds_now();
	bool* fibonacci = new bool[VSIZE];
	guessIfFibonacciASM(numbers, fibonacci, VSIZE);
	long long endFibo = milliseconds_now();
	
	printf("Adivinamos si los numeros son pares o no\n");
	long long initPari = milliseconds_now();
	bool* parity = new bool[VSIZE];
	guessIfEvenASM(numbers, parity, VSIZE);
	long long endPari = milliseconds_now();

	printf("Ahora volvemos a realizar todo de manera concurrente. ");
	long long initConc = milliseconds_now();
	concurrentBenchmark();
	long long endConc = milliseconds_now();

	printf("Escribimos lo adivinado en \"salida.txt\"\n\n");
	long long initWrite = milliseconds_now();
	writeBenchmarkResults(numbers, fibonacci, parity);
	long long endWrite = milliseconds_now();


	delete[] numbers, fibonacci, parity;
	printResult(endGen-initGen, endFibo - initFibo, endPari - initPari, endWrite - initWrite, endConc - initConc);

	printf("\n");
	system("pause");
	return 0;
}
