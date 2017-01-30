#include <cstdio>

int suma(int, int);

int main(int a, int b) {
	int x, y;
	x = y = 0;

	printf("CALCULO DE LA SUMA DE DOS NUMEROS\n");
	printf_s("\n");
	printf_s("Primer sumando: ");
	scanf_s("%d", &x);
	printf_s("Segundo sumando: ");
	scanf_s("%d", &y);
	getchar();
	printf_s("Suma de %d y %d = %d\n", x, y, suma(x, y));
	printf_s("\n");
	printf_s("Pulse <Return> para finalizar...");
	getchar();
}

int suma(int a, int b) {
	__asm {
		mov eax, a;
		mov ecx, b;
		add eax, ecx;
	}
}