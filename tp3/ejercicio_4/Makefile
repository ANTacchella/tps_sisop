all: Ejercicio_4

Ejercicio_4: ejercicio_4.o hashmap.o
	@gcc -o Ejercicio_4 ejercicio_4.o hashmap.o

ejercicio_4.o: ejercicio_4.c
	@gcc -c ejercicio_4.c

hashmap.o: hashmap.c
	@gcc -c hashmap.c

clean: 
	rm -f *.o