#ifndef CONCURRENCY_H_
#define CONCURRENCY_H_

#include "Utils.h"
#include "FunctionsASM.h"

void generateRandomNumbers(unsigned long int* v, int tam);
int numberCores();
void concurrentBenchmark();

#endif // !CONCURRENCY_H_

