all: Ejercicio_2 configurador

Ejercicio_2: ejercicio_2.o
	#gcc -o Ejercicio_2 ejercicio_2.o -l pthread
	g++ -o HiloFibonacci  hiloFibonacci.cpp -l pthread

ejercicio_2.o: ejercicio_2.cpp
	g++ -c hiloFibonacci.cpp
	#gcc -c ejercicio_2.cpp

clean: 
	rm *.o