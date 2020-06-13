#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void mostrar_procesos(pid_t vec[], int nodos[],int tam){
    for(int i = tam - 1; i >= 0; i--){
        if(vec[i] != 0){
            printf("(%d) %d - ",nodos[i],vec[i]);
        }
    }
    printf("\n");
    fflush(stdout);
}

void guardar_pid(pid_t vec[], pid_t pid, int nodos[], int nodo, int tam){
    for(int x = 0; x < tam; x++){
        if(vec[x] == 0){
            vec[x] = pid;
            nodos[x] = nodo;
            break;
        }
    }
}

void init_vector(pid_t vec[],int tam){
    for(int  i = 0; i < tam; i++){
        vec[i] = 0;
    }
}

int main(int argc, char* argv[]){

    int l;

    if(argc != 2){
        perror("El programa debe recibir un único parámetro!");
        exit(1);
    }
    
    l = atoi(argv[1]);
    if(l < 0){
        perror("La cantidad de procesos debe ser igual o mayor a cero!");
        exit(1);
    }

    const int tam = 4+l;
    pid_t array_pid[tam], wpid;

    int i,j,k,status=0;
    int nodos[tam];

    init_vector(array_pid,tam);
    init_vector(nodos,tam);

    guardar_pid(array_pid,getpid(),nodos,0,tam);
    mostrar_procesos(array_pid,nodos,tam);

    for(i = 0; i < 3; i++){
        if(fork() == 0){
            guardar_pid(array_pid, getpid(),nodos, i+1,tam);

            mostrar_procesos(array_pid,nodos,tam);
            if(i == 1){
                for(j = 0; j < 2; j++){
                    if(fork() == 0){
                        guardar_pid(array_pid, getpid(),nodos, 4+j,tam);
                        mostrar_procesos(array_pid,nodos,tam);

                        if(j == 0){

                            for(k = 0; k < 1+l; k++){
                                if(fork() == 0){
                                    guardar_pid(array_pid, getpid(),nodos,7+k,tam);
                                    mostrar_procesos(array_pid,nodos,tam);

                                    

                                }
                                else{

                                    break; 
                                }
                            }

                            while((wpid = wait(&status)) > 0);

                        }

                        break; 
                    }
                }

                while((wpid = wait(&status)) > 0);
            }
            else if(i == 2){

                if(fork() == 0){
                    guardar_pid(array_pid, getpid(),nodos,6,tam);
                    mostrar_procesos(array_pid,nodos,tam);
                }

            }
            while((wpid = wait(&status)) > 0);
            break;
        }
    }
    while((wpid = wait(&status)) > 0);
    return EXIT_SUCCESS;
}