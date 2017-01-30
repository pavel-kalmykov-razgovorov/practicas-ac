#include "Export.h"

long long milliseconds_now() {
	static LARGE_INTEGER s_frequency;
	static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
	if (s_use_qpc) {
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return (1000LL * now.QuadPart) / s_frequency.QuadPart;
	}
	else return GetTickCount();
}

bool is_file_exist(const char *fileName) {
	ifstream infile(fileName);
	return infile.good();
}

void writeBenchmarkResults(unsigned long int* numbers, bool* fibonacci, bool* parity) {
	FILE *f = NULL;
	fopen_s(&f, "salida.txt", "w");
	if (f != NULL) {
		fprintf(f, "NUM\tFIB\tEVEN\n");
		for (int i = 0; i < VSIZE; i++) {
			fprintf(f, "%lu\t", numbers[i]);
			if (fibonacci[i]) fprintf(f, "T\t");
			else fprintf(f, "F\t");
			if (parity[i]) fprintf(f, "T\t");
			else fprintf(f, "F\t");
			fprintf(f, "\n");
		}
		fclose(f);
	}
}

void printResult(long long timeGen, long long timeFibo, long long timePari, long long timeWrite, long long timeConc) {
	string s = "result";
	int i = 1;
	string name = s + to_string(i) + ".txt";

	//Comprueba qué archivos resulti.txt existen para crear uno nuevo con la ultima numeracion
	while (is_file_exist(name.c_str())) name = s + to_string(++i) + ".txt";

	FILE *f = NULL;
	fopen_s(&f, name.c_str(), "w");

	if (f != NULL) {
		fprintf(f, "------------------------------------------\n");
		fprintf(f, "Resultados:\n");
		fprintf(f, "------------------------------------------\n");
		fprintf(f, "Test generacion:\t\t%lli ms\n", timeGen);
		fprintf(f, "Test fibonacci:\t\t\t%lli ms\n", timeFibo);
		fprintf(f, "Test paridad:\t\t\t%lli ms\n", timePari);
		fprintf(f, "Test total single thread:\t%lli ms\n", timeGen + timeFibo + timePari);
		fprintf(f, "Test multithreading:\t\t%lli ms\n", timeConc);
		fprintf(f, "Test escritura en memoria:\t%lli ms\n", timeWrite);
		fprintf(f, "------------------------------------------\n");
		fclose(f);
	}

	printf("---------------------------------------------\n");
	printf("Resultados:\n");
	printf("---------------------------------------------\n");
	printf("Test generacion:\t\t%lli ms\n", timeGen);
	printf("Test fibonacci:\t\t\t%lli ms\n", timeFibo);
	printf("Test paridad:\t\t\t%lli ms\n", timePari);
	printf("Test total single thread:\t%lli ms\n", timeGen + timeFibo + timePari);
	printf("Test multithreading:\t\t%lli ms\n", timeConc);
	printf("Test escritura en memoria:\t%lli ms\n", timeWrite);
	printf("---------------------------------------------\n");

	char *fileExt;
	char szDir[256]; //dummy buffer
	GetFullPathName(name.c_str(), 256, szDir, &fileExt);
	printf("Resultado guardado en: %s\n\n", szDir);
}