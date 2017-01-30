#ifndef EXPORT_H_
#define EXPORT_H_

#include "Utils.h"

long long milliseconds_now();
bool is_file_exist(const char *fileName);
void writeBenchmarkResults(unsigned long int* numbers, bool* fibonacci, bool* parity);
void printResult(long long timeGen, long long timeFibo, long long timePari, long long timeWrite, long long timeConc);


#endif // !EXPORT_H_

