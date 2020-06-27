CC=gcc
CFLAGS=-I.

all: Ejercicio5_Server Ejercicio5_Client

Ejercicio5_Server: ejercicio5_server.o lista.o
	$(CC) -o Ejercicio5_Server ejercicio5_server.o lista.o -pthread

ejercicio5_server.o: ejercicio5_server.c lista.h
lista.o: lista.c

Ejercicio5_Client: ejercicio5_client.o
	$(CC) -o Ejercicio5_Client ejercicio5_client.o -pthread

ejercicio5_client.o: ejercicio5_client.c