#include <cstdlib>
#include <cstdio>
#include <time.h>
#include <Windows.h>

#define tam 100000

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

void bubblesortC(int* v) {
	for (int i = 0; i < tam - 1; i++) {
		for (int j = tam - 1; j > i; j--) {
			if (v[j] < v[j - 1]) {
				int intercambio = v[j - 1];
				v[j - 1] = v[j];
				v[j] = intercambio;
			}
		}
	}
}

void bubblesortASM(int* v) {
	__asm {
		mov eax, v; //eax será el puntero al vector
		mov edx, tam; //edx, registro multiuso, cargamos el tamaño del array
		dec edx; //tam-1

				 //mov ebx, 0; //ebx será i
		xor ebx, ebx; //hacer un xor a un registro es mas eficiente que ponerle un 0
	bucle1:
		cmp ebx, edx; //si son iguales, salir (i < tam-1)
		je fin;

		mov ecx, edx; //ecx será j = tam-1
	bucle2:
		cmp ecx, ebx; //si son iguales, salir (j > i)
		je fin_bucle1;

		//FORMULA PARA ACCEDER A LA MEMORIA: Puntero inicial + posicion del array * tamaño (en bytes) del dato
		mov esi, [eax + ecx * 4]; //esi será v[j]
		dec ecx; //decrementamos j para tener el numero j-1
		mov edi, [eax + ecx * 4]; //edi será v[j-1]
								  //inc ecx; //deshacemos el incremento
		cmp esi, edi; //si uno es menor que otro, intercambiamos (v[j] < v[j-1])
		jl intercambio; //jl si ascendente; jg si descendente
		jmp bucle2; //terminamos bucle. j-- (ya hemos decrementado antes) y volvemos a empezar

	intercambio:
		mov[eax + ecx * 4], esi; //ponemos en v[j-1] esi, que era v[j]
		inc ecx; //deshacemos el incremento
		mov[eax + ecx * 4], edi; //ponemos en v[j] edi, que era v[j-1]
		dec ecx; //terminamos bucle. j-- y volvemos a empezar
		jmp bucle2;

	fin_bucle1:
		inc ebx; //terminamos bucle. i++ y volvemos a empezar
		jmp bucle1;

	fin:
	}
}

int main() {
	srand((unsigned int) time(NULL));
	int *v, *v2;
	v = new int[tam];
	v2 = new int[tam];
	long long initC, finC, initASM, finASM;
	for (int i = 0; i < tam; i++) v[i] = rand() % 50; //Construimos el array
	for (int i = 0; i < tam; i++) v2[i] = v[i]; //Lo copiamos para tener 2, uno para C y otro para ASM

	printf("Antes de ordenar:\n");
	/*for (int i = 0; i < tam; i++) {
		printf("%d", v[i]);
		if (i != tam - 1) printf(", ");
	}*/
	printf("\n\n");

	initC = milliseconds_now();
	bubblesortC(v2);
	finC = milliseconds_now();

	initASM = milliseconds_now();
	bubblesortASM(v);
	finASM = milliseconds_now();

	printf("\nDespues de ordenar (C):\n");
	/*for (int i = 0; i < tam; i++) {
		printf("%d", v2[i]);
		if (i != tam - 1) printf(", ");
	}*/
	printf("Tiempo: %lli ms\n", finC - initC);

	printf("\nDespues de ordenar (ASM):\n");
	/*for (int i = 0; i < tam; i++) {
		printf("%d", v[i]);
		if (i != tam - 1) printf(", ");
	}*/
	printf("Tiempo: %lli ms\n", finASM - initASM);

	printf("\n");
	system("pause");
}