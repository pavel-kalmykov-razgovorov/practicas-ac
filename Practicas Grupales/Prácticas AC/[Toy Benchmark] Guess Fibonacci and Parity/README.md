# Toy Benchmark Para Práctica AC - FASE II
<br /><br />
Programa que pone a prueba el rendimiento del equipo mediante las siguientes pruebas:

- Generar de __100M de números aleatorios__ (acceso a memoria + funcion rand)
- Comprobar que cada uno de los números __pertenezca a la serie de Fibonacci__ (ASM)
- Comprobar que cada uno de los números __sea par o no__ (ASM)

Todo lo anteriormente dicho, se vuelve a realizar __simultáneamente__ (multihilo) dividiendo la talla del problema (100M) entre el número de núcleos que presente el procesador (no se vuelve a resolver de nuevo, si no que se generan para cada hilo talla/nucleos de numeros y se deben de adivinar otra vez).

El resultado de ésta adivinación se guarda en un fichero de texto (1'08 GiB de datos aproximadamente) para comprobar la velocidad de escritura del disco duro.

Por último, se muestra por pantalla el tiempo de ejecución de cada una de las pruebas y estos datos son exportados a un fichero que se enumera automáticamente con cada vez que ejecutas el benchmark (si lo ejecutas 5 veces, tendrás 5 resultados distintos)

Pavel Razgovorov (pr18@alu.ua.es) y Eddie Rodríguez (erp35@alu.ua.es)
