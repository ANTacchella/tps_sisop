all: Ejercicio_3

Ejercicio_3: ejercicio_3.o
	gcc -o Ejercicio_3 ejercicio_3.o -lrt -pthread

ejercicio_3.o: ejercicio_3.c
	gcc -c ejercicio_3.c

clean:
	rm -f *.o