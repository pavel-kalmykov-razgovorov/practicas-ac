#include "Concurrency.h"

void generateRandomNumbers(unsigned long int* v, int tam) {
	srand(0);
	//for (int i = 0; i < VSIZE; i++) v[i] = rand() % MAXRAND;
	for (int i = 0; i < tam; i++) v[i] = rand() % MAXRAND;
}

int numberCores() {
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	return sysinfo.dwNumberOfProcessors;
}

void concurrentBenchmark() {
	vector<thread> threads;
	int cores = numberCores();

	printf("(numero de cores: %i)\n\n", cores);
	for (int i = 0; i < cores; i++) {
		//Por cada core que tengamos, creamos un nuevo thread al que le pasamos la funcion lambda que sigue
		threads.push_back(thread([](int id) -> void {
			printf("START Thread %i\n", id);
			int tam = VSIZE / numberCores();

			unsigned long int* numbersConcurrent = new unsigned long int[tam];
			generateRandomNumbers(numbersConcurrent, tam);

			bool* fibonacciConcurrent = new bool[tam];
			guessIfFibonacciASM(numbersConcurrent, fibonacciConcurrent, tam);

			bool* parityConcurrent = new bool[tam];
			guessIfEvenASM(numbersConcurrent, parityConcurrent, tam);

			delete[] numbersConcurrent, fibonacciConcurrent, parityConcurrent;
			printf("END Thread %i\n", id);
		}, i + 1));
	}

	//Liberamos todos los cores antes de terminar
	auto firstThread = threads.begin();
	while (firstThread != threads.end()) {
		firstThread->join();
		firstThread++;
	}
}