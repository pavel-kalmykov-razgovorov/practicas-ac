unsigned long int getFibonacci(int j) {
	if (j == 0) return 0;
	unsigned long int a = 0, b = 1, t;
	for (int k = 1; k < j; k++) {
		t = a + b;
		a = b;
		b = t;
	}

	return b;
}

bool isFibonacci(unsigned long int n) {
	unsigned long int j, number;
	j = number = 0;
	while (true) {
		if (n > (number = getFibonacci(j))) j++;
		else if (n == number) return true;
		else return false;
	}
}

void guessIfFibonacci(unsigned long int* numbers, bool* fibonacci,int tam) {
	for (int i = 0; i < tam; i++) fibonacci[i] = isFibonacci(numbers[i]);
}

void guessIfEven(unsigned long int* numbers, bool* even, int tam) {
	for (int i = 0; i < tam; i++)
		if (numbers[i] % 2 == 0) even[i] = true;
		else even[i] = false;
}