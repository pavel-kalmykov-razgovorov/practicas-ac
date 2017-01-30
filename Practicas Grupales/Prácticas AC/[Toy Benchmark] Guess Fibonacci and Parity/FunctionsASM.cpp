void guessIfFibonacciASM(unsigned long int* n, bool* g, int tam) {
	unsigned long int i, j, a, b;
	__asm {
		/**
		* VARIABLES:
		* eax -> vector de enteros. Intocable
		* ebx -> vector de booleanos. Intable
		* ecx -> auxiliar para hacer a = b; (imposible mover de memoria a memoria)
		* edx -> numero fibonacci calculado. Cambia segun la iteracion del algoritmo
		* edi -> auxiliar para hacer algunas comparaciones entre memoria y memoria
		* esi -> numero de la posicion i del vector
		*/
		mov eax, n;
		mov ebx, g;
		mov i, 0;
	bucle_guessIfFibonacci:
		mov edi, tam;
		cmp i, edi;
		je fin_bucle_guessIfFibonacci;

		mov a, 0; //a corresponde con el primer numero de fibonnacci
		mov b, 1; //b con el segundo elemento de fibonacci, la suma de ambos sera j nuestro numero actual
		mov edi, 1; //Indice de los k-ésimos números fibonacci que iremos calculando
		mov j, 0; //numero j-ésimo de la serie de Fibonacci
	bucle_isFibonacci:
		//getFibonacci
		cmp j, 0; //Si es el primer número, por norma es el 0. Si no, lo calculamos
		je primerFibonacci;

		mov edx, 1; //Si no es el primer número, como mínimo será 0
	bucle_getFibonacci:
		cmp j, edi; // j < k
		je fin_bucle_getFibonacci;

		xor edx, edx;
		add edx, a;
		add edx, b; //t = a+b
		mov ecx, b;
		mov a, ecx; // a = b
		mov b, edx; //b = t

		inc edi; //k++
		jmp bucle_getFibonacci;

	primerFibonacci:
		mov edx, 0;
		jmp fin_bucle_getFibonacci;

	fin_bucle_getFibonacci:
		//isFibonacci despues de la llamada de getFibonacci
		mov esi, i;
		mov esi, [eax + esi * 4]; //cada n que recibe isFibonacci
		cmp esi, edx; // n > number -> nextFibonacci
		jg nextFibonacci;
		cmp esi, edx; // n == number -> fibonacciTrue
		je fibonacciTrue;
		jmp fibonacciFalse; // en cualquier otro caso (n < number, nos hemos pasado) --> fibonacciFalse


	nextFibonacci:
		inc j; //Probamos a encontrar el siguiente numero de Fibonacci
		jmp bucle_isFibonacci;

	fibonacciTrue:
		mov esi, i;
		mov[ebx + esi], 1; //Metemos en la posicion i el valor true
		inc i; //i++ para avanzar en el proximo numero que adivinemos
		jmp bucle_guessIfFibonacci;

	fibonacciFalse:
		mov esi, i;
		mov[ebx + esi], 0; //Metemos en la posicion i el valor false
		inc i; //i++ para avanzar en el proximo numero que adivinemos
		jmp bucle_guessIfFibonacci;

	fin_bucle_guessIfFibonacci:
	}
}

void guessIfEvenASM(unsigned long int* n, bool* e, int tam) {
	//even true, odd false
	__asm {
		mov eax, 0; //i=0
		mov edi, n; //puntero al vector
	buclei:
		mov ecx, [edi + eax * 4]; //copiamos el numero a tratar
		and ecx, 00000001; //hacemos una and para actualizar el flag PF (parity flag)
		jp isEven; //si hay paridad par, saltamos a EVEN
		jnp isOdd; //en caso contrario a ODD

	continuarBucle:
		inc eax; //i++
		cmp eax, tam; //mientras i<VSIZE
		jl buclei; //continuamos
		jmp fin; //finalizamos

	isEven:
		mov edx, e; //vector de booleanos
					//mov ebx, i; //posicion del vector
		mov[edx + eax], 1; //Metemos en la posicion i el valor true
		jmp continuarBucle;

	isOdd:
		mov edx, e; //vector de booleanos
					//mov ebx, i; //posicion del vector
		mov[edx + eax], 0; //Metemos en la posicion i el valor false
		jmp continuarBucle;
	fin:
	}
}