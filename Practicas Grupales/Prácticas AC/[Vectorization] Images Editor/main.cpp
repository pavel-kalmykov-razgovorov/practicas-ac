#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <Windows.h>
using namespace std;

#pragma warning(push, 0) //Deshabilita warnings   
#include "CImg.h"
using namespace cimg_library;
#pragma warning(pop) //Los vuelve a habilitar

__int64 milliseconds_now() {
	static LARGE_INTEGER s_frequency;
	static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
	if (s_use_qpc) {
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return (1000LL * now.QuadPart) / s_frequency.QuadPart;
	}
	else return GetTickCount();
}

class Image {
private:
	CImg<unsigned char> src;
	int width;
	int height;
	unsigned char* pixels;
public:
	Image(string fileName) {
		src = CImg<unsigned char>(fileName.c_str());
		pixels = src.data();
		width = src.width();
		height = src.height();
	}

	void save(string fileName) {
		src.save(fileName.c_str());
	}

	void displayData() {
		int size = width * height * 3;
		for (int i = 0; i < size; i++) {
			cout << (int)pixels[i] << endl;
		}
	}

	void EditConstrastC(int percent) {
		int increment, length;
		float factor, newPixel;
		increment = (255 * percent) / 100; //Calculamos el incremento a partir de un porcentaje [-100,100], haciendo que el incremento sea [-255,255]
		length = width * height * 3;
		factor = (float)(259 * (increment + 255)) / (255 * (259 - increment)); //Formula del factor para el nuevo contraste
		for (int i = 0; i < length; ++i) {
			newPixel = factor * (pixels[i] - 128) + 128; //Formula del ajuste
			if (newPixel > 255) pixels[i] = 255;
			else if (newPixel < 0) pixels[i] = 0;
			else pixels[i] = (unsigned char)newPixel;
		}
	}

	void EditBrightnessC(int percent) {
		int increment, length, newPixel;
		//Los valores RGB van del 0 al 255. El incremento se realizará por cada píxel
		//P.E. Si tengo un porcentaje de brillo de -10%, haremos un incremento de -25'5 (25)
		increment = (255 * percent) / 100;
		length = width * height * 3; //ancho * alto = matriz de pixeles y 3 = cada color del RGB
		for (int i = 0; i < length; ++i) {
			newPixel = (int)pixels[i] + increment; //Calculamos el valor RGB del nuevo pixel
			if (newPixel > 255) pixels[i] = 255; //Si hay overflow de brillo, me quedo en el blanco
			else if (newPixel < 0) pixels[i] = 0; //Si hay underflow de brillo, me quedo en el negro
			else pixels[i] = newPixel; //Si no, entra en los valores normales
		}
	}

	void EditBrightnessSSE(int percent) {
		int increment, tam, signo;
		unsigned char* incr = new unsigned char[16];
		_asm {
			//increment = (255 * percent) / 100;
			imul eax, percent, 255; //Signed Multiplication percent*255 y lo guardamos en EAX
			cdq; //Extiende el signo negativo a los registros EDX:EAX para que podamos dividir con signo
			mov ecx, 100; //Movemos el inmediato 100 a un registro porque la division no permite dividir con inmediatos
			idiv ecx; //Dividimos EAX entre 100 y el resultado va a EAX
			mov increment, eax; //Copiamos el resultado de toda esta operacion aritmética en la variable increment

			//length = width * height * 3;
			mov esi, this; //Para acceder a los miembros de la clase se hace con la direccion this + miembro
			mov ebx, [esi + width];
			imul ebx, [esi + height];
			imul ebx, 3;

			sub ebx, 16; //restamos 16 a tam

			mov tam, ebx; //Se guarda el resultado final en la variable tam

			//Comprobacion del signo del incremento (eligira si el algoritmo es de suma o de resta)
			//Y asignacion del valor del incremento (en valor absoluto) a un array de 16 newPixel's
			mov edx, 0; // en el bucle cargar_incrementos, i=0
			mov edi, incr; // movemos el puntero de incr a edi
			mov signo, 0; //Será 0 si es positivo
			cmp eax, 0; //comparamos si nuestro incremento es positivo
			jg cargar_incrementos; //si es asi dejamos la variable signo a 0 
			mov signo, 1; //si no lo es, signo=1, que será negativo
			imul eax, eax, -1; // y hacemos el incremento positivo
		cargar_incrementos: //cargamos 16 bytes con el incremento en el vector auxiliar incr
			cmp edx, 16; // mientras i<16
			je fin_cargar_incrementos; //saltamos si i=16
			mov[edi + edx], eax; // incr[i]=incremento;
			inc edx; // i++
			jmp cargar_incrementos;

		fin_cargar_incrementos:
			movdqu xmm1, [edi]; //cargamos nuestro incrementox16 en xmm1

			//ALGORITMO TOCHO
			//Se ha repetido todo el código posible para evitar hacer la mínima cantidad de saltos
			mov esi, [esi + pixels]; // Copiamos el puntero a aux en esi
			mov edx, 0; //EDX = Indice del bucle
			cmp signo, 1; //compramos el signo del incremento
			je bucle_resta; //si es negativo restamos, en caso contrario sumamos

		bucle_suma:
			cmp edx, tam; //Comparamos si edx es < tam
			jge fin_operaciones; //Si son iguales salta al fin_bucle
			movdqu xmm0, [esi + edx]; //Movemos a xmm0 las i+15 posiciones pixels[i]
			paddusb xmm0, xmm1; //xmm0 = pixeles + incremento
			movdqu[esi + edx], xmm0; //Devolemos los nuevos valores al array
			add edx, 16; //Seguimos a por los 16 próximos píxeles
			jmp bucle_suma; 
			
		bucle_resta:
			cmp edx, tam; //Comparamos si edx es < tam
			jge fin_operaciones; //Si son iguales salta al fin_bucle
			movdqu xmm0, [esi + edx]; //Movemos a xmm0 las i+15 posiciones pixels[i]
			psubusb xmm0, xmm1; //xmm0 = pixeles + incremento
			movdqu[esi + edx], xmm0; //Devolemos los nuevos valores al array
			add edx, 16; //Seguimos a por los 16 próximos píxeles
			jmp bucle_resta;

		fin_operaciones:
			add tam, 16;
			jmp fin_bucle_operaciones;

		fin_bucle_operaciones:

			cmp edx, tam;
			je fin_bucle; //Si son iguales salta al fin_bucle

			movzx edi, [esi + edx]; //Movemos a edi pixels[i]
			mov ecx, increment; //ECX = increment
			
			add ecx, edi; //ECX += pixels[i]
			//sub ecx, edi; //ECX -= pixels[i]
			
			cmp ecx, 255; //Comprobamos overflow
			jg blanco; //Si es mayor, truncamos a blanco
			cmp ecx, 0; //Comprobamos undeflow
			jl negro; //Si es menor, truncamos a negro
			jmp nuevoColor; //En cualquier otro caso, vamos a nuevo color

		blanco:
			mov ecx, 255;
			jmp nuevoColor;

		negro:
			mov ecx, 0;
			jmp nuevoColor;

		nuevoColor:
			mov[esi + edx], cl; //Mover a pixels[i] la parte baja del registro ECX
			inc edx;
			jmp fin_bucle_operaciones;

		fin_bucle:
		}
	}
	
	void EditBrightnessASM(int percent) {
		int increment, tam;
		_asm {
			//increment = (255 * percent) / 100;
			imul eax, percent, 255; //Signed Multiplication percent*255 y lo guardamos en EAX
			cdq; //Extiende el signo negativo a los registros EDX:EAX para que podamos dividir con signo
			mov ecx, 100; //Movemos el inmediato 100 a un registro porque la division no permite dividir con inmediatos
			idiv ecx; //Dividimos EAX entre 100 y el resultado va a EAX
			mov increment, eax; //Copiamos el resultado de toda esta operacion aritmética en la variable increment

			//length = width * height * 3;
			mov esi, this; //Para acceder a los miembros de la clase se hace con la direccion this + miembro
			mov ebx, [esi + width];
			imul ebx, [esi + height];
			imul ebx, 3;
			mov tam, ebx; //Se guarda el resultado final en la variable tam

			mov esi, [esi + pixels]; //Cambiamos el puntero de this a this->pixels. Hacer [esi + pixels + edx] no funciona
			mov edx, 0; //EDX = Indice del bucle
		bucle:
			cmp edx, tam;
			je fin_bucle; //Si son iguales salta al fin_bucle
			
			movzx edi, [esi + edx]; //Movemos a edi pixels[i]
			mov ecx, increment; //ECX = increment
			add ecx, edi; //ECX += pixels[i]
			cmp ecx, 255; //Comprobamos overflow
			jg blanco; //Si es mayor, truncamos a blanco
			cmp ecx, 0; //Comprobamos undeflow
			jl negro; //Si es menor, truncamos a negro
			jmp nuevoColor; //En cualquier otro caso, vamos a nuevo color

		blanco:
			mov ecx, 255;
			jmp nuevoColor;

		negro:
			mov ecx, 0;
			jmp nuevoColor;

		nuevoColor:
			mov [esi + edx], cl; //Mover a pixels[i] la parte baja del registro ECX
			inc edx;
			jmp bucle;

		fin_bucle:
		}
	}

	void EditConstrastASM(int percent) {
		int increment, tam, factorTemp, aux, number;
		float factor, newPixel;

		_asm {
			//increment = (255 * percent) / 100;
			imul eax, percent, 255; //Signed Multiplication percent*255 y lo guardamos en EAX
			cdq; //Extiende el signo negativo a los registros EDX:EAX para que podamos dividir con signo
			mov ecx, 100; //Movemos el inmediato 100 a un registro porque la division no permite dividir con inmediatos
			idiv ecx; //Dividimos EAX entre 100 y el resultado va a EAX
			mov increment, eax; //Copiamos el resultado de toda esta operacion aritmética en la variable increment

								//length = width * height * 3;
			mov esi, this; //Para acceder a los miembros de la clase se hace con la direccion this + miembro
			mov ebx, [esi + width];
			imul ebx, [esi + height];
			imul ebx, 3;
			mov tam, ebx; //Se guarda el resultado final en la variable tam

						  //factor = (float)(259 * (increment + 255)) / (255 * (259 - increment))
			add eax, 255; //Sumar incremento al 255 se guarda en eax
			mov ebx, 259;
			sub ebx, increment; //Restar 259 a incremento se guarda en ebx
			mov ecx, 259;
			imul ecx, eax; //(259 * (increment + 255)) se guarda en ecx
			mov edx, 255;
			imul edx, ebx; //(255 * (259 - increment)) se guarda en edx
			mov factorTemp, ecx;
			fild factorTemp;
			mov factorTemp, edx;
			fild factorTemp;
			fdivp st(1), st;
			fstp factor;

			mov eax, 0;
			mov ebx, tam;
			mov esi, [esi + pixels]; //Cambiamos el puntero de this a this->pixels. Hacer [esi + pixels + edx] no funciona
		bucle:
			fstp aux;
			fstp aux;
			fstp aux;
			cmp eax, ebx; //Compara tam y la i
			je fin_bucle; //Si son iguales salta al final

			//newPixel = factor * (pixels[i] - 128) + 128;
			movzx ecx, [esi + eax]; //Movemos a ecx pixels[i]
			sub ecx, 128; //Restamos 128 a pixels[i] y se gurada en ecx
			fld factor;
			mov aux, ecx;
			fild aux;
			fmul st(0), st(1);
			//fst newPixel;
			mov number, 128;
			fild number;
			fadd st(0), st(1);
			fisttp newPixel;
			mov ecx, newPixel;

			cmp ecx, 255; //Comprobamos overflow
			jg blanco; //Si es mayor, truncamos a blanco
			cmp ecx, 0; //Comprobamos undeflow
			jl negro; //Si es menor, truncamos a negro
			jmp nuevoColor; //En cualquier otro caso, vamos a nuevo color

		blanco:
			mov ecx, 255;
			jmp nuevoColor;

		negro:
			mov ecx, 0;
			jmp nuevoColor;

		nuevoColor:
			mov[esi + eax], cl; //Mover a pixels[i] la parte baja del registro ECX
			inc eax;
			jmp bucle;

		fin_bucle:
		}
	}

	void EditConstrastSSE(int percent) {

		int increment, tam, factorTemp, tmp;
		float factor;

		float* factorArray = new float[4];
		float* unoDosOcho = new float[4];
		
		float* newPixelArray = new float[width * height * 3];
		for (int i = 0; i < width * height * 3; i++) newPixelArray[i] = pixels[i];

		for (int i = 0; i < 4; i++) unoDosOcho[i] = 128;

		_asm {
			

			//increment = (255 * percent) / 100;
			imul eax, percent, 255; //Signed Multiplication percent*255 y lo guardamos en EAX
			cdq; //Extiende el signo negativo a los registros EDX:EAX para que podamos dividir con signo
			mov ecx, 100; //Movemos el inmediato 100 a un registro porque la division no permite dividir con inmediatos
			idiv ecx; //Dividimos EAX entre 100 y el resultado va a EAX
			mov increment, eax; //Copiamos el resultado de toda esta operacion aritmética en la variable increment

								//length = width * height * 3;
			mov esi, this; //Para acceder a los miembros de la clase se hace con la direccion this + miembro
			mov ebx, [esi + width];
			imul ebx, [esi + height];
			imul ebx, 3;
			mov tam, ebx; //Se guarda el resultado final en la variable tam

						  //factor = (float)(259 * (increment + 255)) / (255 * (259 - increment))
			add eax, 255; //Sumar incremento al 255 se guarda en eax
			mov ebx, 259;
			sub ebx, increment; //Restar 259 a incremento se guarda en ebx
			mov ecx, 259;
			imul ecx, eax; //(259 * (increment + 255)) se guarda en ecx
			mov edx, 255;
			imul edx, ebx; //(255 * (259 - increment)) se guarda en edx
			mov factorTemp, ecx;
			fild factorTemp;
			mov factorTemp, edx;
			fild factorTemp;
			fdivp st(1), st;
			fstp factor;

			/*
			mov eax, 0; //i=0
			mov ebx, tam; //i<tam
			mov esi, newPixelArray; //puntero a newPixels
			mov edi, this;
			mov edi, [edi + pixels]; //Puntero a pixels;
		bucle_char_to_float: //Cargamos los chars en forma de float para poder trabajar con ellos
			fild[edi + eax];
			fstp[esi + eax * 4];
			inc eax;
			cmp eax, ebx;
			jl bucle_char_to_float;
			*/


			mov eax, 0;
			mov ebx, factorArray;
			mov ecx, factor;

		bucleFactor: //rellenamos un array de floats con 4 factores para trabajar con ellos
			mov [ebx + eax*4], ecx; //Array[i] = factor;
			inc eax; //i++
			cmp eax, 4; //i<4
			jl bucleFactor;

			mov eax, 0;
			mov ebx, tam;
			mov esi, newPixelArray; //Ponemos el puntero a newPixelArray
			mov ecx, unoDosOcho;
			movups xmm0, [ecx]; //xmm0 almacena 4 valores 128
			mov ecx, factorArray;
			movups xmm1, [ecx]; //xmm1 almacena 4 factores

			add ebx, -4; //Restamos 4 para evitar salirnos
		bucle:


			movups xmm2, [esi + eax * 4]; //muevo pixeles a xmm3
			subps xmm2, xmm0; //-128
			mulps xmm2, xmm1; //*factor
			addps xmm2, xmm0; //+128
			movups[esi + eax * 4], xmm2; //Guardamos el resultado
			
			add eax, 4; //Vamos de 4 en 4 asi que i+=4
			cmp eax, ebx; //Si i<tam, saltamos
			jl bucle;
			
			// continuamos
			mov ebx, tam; //i<tam
			add ebx, -4; //Evitamos salirnos
			mov eax, 0;  // i=0
			mov edi, this;
			mov edi, [edi + pixels]; //Copiamos el puntero a pixels
			finit; //Iniciamos la pila de la FPU

		comprobar_overflow:
			cmp eax, ebx;
			jge fin_bucle;  //i<tam

			fld [esi + eax * 4]; //Cargamos el float
			fistp tmp; //Y lo sacamos en forma de int
			mov ecx, tmp; //Lo movemos a un registro

			cmp ecx, 255; //Si es mayor de 255, pondremos 255
			jg overflow;

			cmp ecx, 0; //Si es menor de 0, pondremos 0
			jl underflow;

			mov [edi + eax], ecx; //Si esta entre 0 y 255 lo almacenamos directamente

			inc eax;
			jmp comprobar_overflow;

		overflow:
			mov[edi + eax], 255; //Almacenamos un 255 en su lugar
			inc eax; //i++
			jmp comprobar_overflow; //Continuamos con el bucle

		underflow:
			mov[edi + eax], 0; //Almacenamos un 0 en su lugar
			inc eax; //i++
			jmp comprobar_overflow; //Continuamos con el bucle
		fin_bucle:

		}

	}
};

int main() {
	string fileName = "lena.bmp";
	Image img(fileName);
	Image img2(fileName);
	Image img3(fileName);
	int percent = 100;

	img.EditConstrastSSE(percent);
	img.save("ResultadoSSE.bmp");

	
	long double startBrightnessC = (long double)milliseconds_now();
	img.EditBrightnessC(percent);
	long double endBrightnessC = (long double)milliseconds_now();

	long double startBrightnessASM = (long double)milliseconds_now();
	img2.EditBrightnessASM(percent);
	long double endBrightnessASM = (long double)milliseconds_now();

	long double startBrightnessSSE = (long double)milliseconds_now();
	img3.EditBrightnessSSE(percent);
	long double endBrightnessSSE = (long double)milliseconds_now();

	cout << "Brightness: " << endl;
	cout << "C = " << endBrightnessC - startBrightnessC << " ms\n"
		<< "ASM = " << endBrightnessASM - startBrightnessASM << " ms\n"
		<< "SSE = " << endBrightnessSSE - startBrightnessSSE << " ms\n"
		<< "INC 1 = " << (((endBrightnessC - startBrightnessC) - (endBrightnessASM - startBrightnessASM)) / (endBrightnessASM - startBrightnessASM)) * 100 << " %\n"
		<< "INC 2 = " << (((endBrightnessC - startBrightnessC) - (endBrightnessSSE - startBrightnessSSE)) / (endBrightnessSSE - startBrightnessSSE)) * 100 << " %\n"
		<< "INC 3 = " << (((endBrightnessASM - startBrightnessASM) - (endBrightnessSSE - startBrightnessSSE)) / (endBrightnessSSE - startBrightnessSSE)) * 100 << " %\n";
		
	long double startConstrastC = (long double)milliseconds_now();
	img.EditConstrastC(percent);
	long double endConstrastC = (long double)milliseconds_now();

	long double startConstrastASM = (long double)milliseconds_now();
	img2.EditConstrastASM(percent);
	long double endConstrastASM = (long double)milliseconds_now();

	long double startConstrastSSE = (long double)milliseconds_now();
	img3.EditConstrastSSE(percent);
	long double endConstrastSSE = (long double)milliseconds_now();

	cout << endl << "Constrast: " << endl;
	cout << "C = " << endConstrastC - startConstrastC << " ms\n"
		<< "ASM = " << endConstrastASM - startConstrastASM << " ms\n"
		<< "SSE = " << endConstrastSSE - startConstrastSSE << " ms\n"
		<< "INC 1 = " << (((endConstrastC - startConstrastC) - (endConstrastASM - startConstrastASM)) / (endConstrastASM - startConstrastASM)) * 100 << " %\n"
		<< "INC 2 = " << (((endConstrastC - startConstrastC) - (endConstrastSSE - startConstrastSSE)) / (endConstrastSSE - startConstrastSSE)) * 100 << " %\n"
		<< "INC 3 = " << (((endConstrastASM - startConstrastASM) - (endConstrastSSE - startConstrastSSE)) / (endConstrastSSE - startConstrastSSE)) * 100 << " %\n";

	system("pause");
	return 0;
}
