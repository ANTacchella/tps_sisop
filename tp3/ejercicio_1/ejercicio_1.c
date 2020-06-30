#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/*
# ejercicio_1.c | Trabajo Práctico 3) Ejercicio 1) | Primera Reentrega
# Maximiliano José Bernardo | 41912800
# Nicolás Agustín Fábregues | 41893896
# Joela Belén Kachanosqui | 41917556
# Alejandro Nicolás Tacchella | 41893930
# Tomás Martín Vera | 41988332
*/

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

void help(){
    printf("\n######    HELP Ejercicio_1    ######\n\n");
    printf("Este programa dado un parámetro N de entrada \n");
    printf("recrea el siguiente árbol de procesos.\n\n");
    printf("            1               \n");
    printf("        /       4   -   7   \n");
    printf("    /       /               \n");
    printf(" 0  -   2   -   5           \n");
    printf("    \\                      \n");
    printf("        3   -   6           \n\n");
    printf("Cada proceso imprimira su pid y el de toda su ascendencia.\n");
    printf("Y ademas creara N jerarquías de procesos hijos adicionales.\n");
    printf("\n");
    printf("Para ejecutar el programa: \"./Ejercicio_1 N\",\n");
    printf("donde N debe ser un entero mayor o igual a cero.\n\n");

    
}

int main(int argc, char* argv[]){

    int l;
    char info[]="Para más información ejecute Ejercicio_1 -h o Ejercicio_1 --help.\n";

    if(argc != 2){
        printf("\n¡ERROR!: El programa debe recibir un único parámetro\n");
        printf ("%s\n", info);
        return EXIT_FAILURE;
    }

    if( !strcmp("-h", argv[1]) || !strcmp("-?", argv[1]) || !strcmp("--help", argv[1]) ){
        help();
        return EXIT_SUCCESS;
    }

    l = atoi(argv[1]);
    if( strcmp("0", argv[1]) ){

        if(l <= 0){
            printf ("\n¡ERROR!: Ingrese un entero mayor o igual a cero\n");
            printf ("%s\n", info);
            return EXIT_FAILURE;
        }
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