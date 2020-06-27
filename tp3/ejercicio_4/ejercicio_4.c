#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <stdlib.h> 
#include <signal.h>
#include <wait.h>
#include <time.h>
#include "hashmap.h"

/*
ejercicio_4.c | Trabajo Práctico 3) Ejercicio 4) | Primera Reentrega
Maximiliano José Bernardo | 41912800
Nicolás Agustín Fábregues | 41893896
Joela Belén Kachanosqui | 41917556
Alejandro Nicolás Tacchella | 41893930
Tomás Martín Vera | 41988332
*/

typedef struct ps_struct
{
    int pid;
    float cpu;
    float mem;
    char comm[20];
}ps_struct;

typedef struct reg_struct
{
    int pid;
    char desc[8];
}reg_struct;

// Variables globales
pid_t pid_control = -1;
pid_t pid_registro = -1;
int control_status;
int reg_status;
FILE *cmd;
char *control_fifo = "/tmp/control_fifo";
char *nombre_arch_registro = "./registro.txt";
char *nombre_arch_log = "./ejercicio_4.log";
FILE *reg_file;
FILE *log_file;
map_t hashmap;

void help(){
    printf("\n######    HELP Ejercicio_4    ######\n\n");
    printf("Este programa dado dos parámetros de entrada: X e Y \n");
    printf("Controla el uso de memoria y cpu por parte de los procesos cada 1seg.\n\n");
    printf("Verificando que no se exceda del valor limite X e Y ingresado.\n");
    printf("En caso de que un proceso supere esos limites\n");
    printf("se lo registrara en un archivo: %s \n", nombre_arch_registro);
    printf("\n");
    printf("Para ejecutar el programa: \"./Ejercicio_4 X Y\",\n");
    printf("donde X (limite de memoria) debe ser un float mayor a cero.\n");
    printf("donde y (limite de cpu) debe ser un float mayor a cero.\n\n");
    printf("El programa quedara ejecutando en segundo plano.\n");
    printf("Los errores encontrados durante la ejecución serán registrados en un archivo ejercicio_4.log.\n");
    printf("Para terminar el programa utilize la señal SIGUSR1.\n");
}

void signalHandler(int sig){

    mode_t theMode = S_IRWXU;
    if(unlink(control_fifo) != 0){
        fprintf(log_file, "¡ERROR! No se pudo borrar el archivo FIFO.\n");
        exit(EXIT_FAILURE);
    }
    hashmap_free(hashmap);
    if(fclose(reg_file) != 0){
        fprintf(log_file, "¡ERROR! No se pudo cerrar el archivo de registro.\n");
        exit(EXIT_FAILURE);
    }
    if (fclose(log_file) != 0) {
        fprintf(stderr, "¡Error! No se pudo cerrar el archivo de log.\n");
        exit(EXIT_FAILURE);
    }
    //Matamos a los hijos.
    kill(pid_control,SIGTERM);
    kill(pid_registro,SIGTERM);
    exit(EXIT_SUCCESS);
}

void char_to_ps_struct(const char *ps_line, ps_struct *ps_info){
    int res;
    res = sscanf(ps_line, "%d %f %f %[^;]", &ps_info->pid, &ps_info->cpu, &ps_info->mem, ps_info->comm);
    if(res == EOF){
        fprintf(log_file, "¡ERROR! Sscanf failure: No se pudo leer el string del proceso\n");
        exit(EXIT_FAILURE);
    }
}

void char_to_reg_struct(char *reg_line, reg_struct *reg_info){
    int res;
    char reg_line_cpy[40];
    strcpy(reg_line_cpy, reg_line);
    reg_info->pid = atoi(strtok(reg_line_cpy, " "));
    strtok(NULL, " ");
    strcpy(reg_info->desc, strtok(NULL, " "));
}

int main(int argc, char* argv[]) 
{
    /*Manda el proceso como demonio, el primer parámetro indica 
    que quede ejecutando desde el directorio que fue ejecutado (nochdir=1),
    el segundo que la salida por std se redireccione a /dev/null (noclose=0)*/
    daemon(1,1);

    //Parametros Entrada:
    float limite_mem = 0.0; 
    float limite_cpu = 0.0; 

    char info[]="Para más información ejecute Ejercicio_4 -h o Ejercicio_4 --help.\n";

    if(argc == 2){
        if( !strcmp("-h", argv[1]) || !strcmp("-?", argv[1]) || !strcmp("--help", argv[1]) ){
            help();
            return EXIT_SUCCESS;
        }
    }
    
    if(argc != 3){
        printf("\n¡ERROR!: El programa debe recibir dos parámetros.\n");
        printf ("%s\n", info);
        return EXIT_FAILURE;
    }

    limite_mem = atof(argv[1]);
    limite_cpu = atof(argv[2]);

    if( strcmp("0", argv[1]) || strcmp("0.0", argv[1]) || strcmp("0", argv[2]) || strcmp("0.0", argv[2]) ){

        if(limite_mem <= 0 || limite_cpu <= 0){
            printf ("\n¡ERROR!: Ingrese un float mayor a cero.\n");
            printf ("%s\n", info);
            return EXIT_FAILURE;
        }
    }

    log_file = fopen(nombre_arch_log, "w+");
    if (log_file == NULL) {
        fprintf(stderr, "¡Error! No se pudo abrir el archivo de log.\n");
        return EXIT_FAILURE;
    }
    reg_file = fopen(nombre_arch_registro, "w+");
    if (reg_file == NULL) {
        fprintf(log_file, "¡Error! No se pudo abrir el archivo de registro.\n");
        return EXIT_FAILURE;
    }
    hashmap = hashmap_new();
    if (hashmap == NULL){
        fprintf(log_file, "¡Error! Memoria insuficiente.\n");
        return EXIT_FAILURE;
    }

    //signal handler SIGUSR1
    signal(SIGUSR1, &signalHandler);
    if (mkfifo(control_fifo, 0666) != 0){
        fprintf(log_file, "¡Error! No se pudo crear el archivo FIFO.\n");
        return EXIT_FAILURE;
    }
    //crear hijos y dejarlos en segundo plano
    pid_control = fork(); //Creo a Control
    if (pid_control == 0)
    {   
        ps_struct ps_info;
        char ps[41];
        char ps_line[40];
        int pid;
        float cpu;
        float mem;
        char comm[20];
        int file_desc;

        char tipo_de_exceso[8];
        int cont = 0;
        char string_registro[40];
        time_t now;
        struct tm * timeinfo;

        //Codigo Hijo Control
        while(1){
            file_desc = open(control_fifo, O_WRONLY);
            sprintf(ps, "ps --no-headers -Ao pid,%%cpu,%%mem,comm");
            cmd = popen(ps, "r");
            if (cmd == NULL) {
                fprintf(log_file, "¡ERROR! Popen failure: No se pudo leer la salida de ps\n");
                exit(EXIT_FAILURE);
            }
            while (fgets(ps_line, sizeof(ps_line), cmd) != NULL) {
                char_to_ps_struct(ps_line, &ps_info);

                //Controlo si exceden
                if( (&ps_info)->cpu > limite_cpu ){
                    strcpy( tipo_de_exceso, "CPU");
                    cont++;
                }
                if( (&ps_info)->mem > limite_mem ){
                    strcpy( tipo_de_exceso, "MEMORIA");
                    cont++;
                }
                if( cont > 0 ){
                    if( cont == 2 ){
                        strcpy( tipo_de_exceso, "AMBOS");
                    }

                    time(&now);
                    timeinfo = localtime (&now);
                   
                    sprintf(string_registro,"%d %s %s %0d:%0d:%0d", (&ps_info)->pid, strtok((&ps_info)->comm,"\n"), tipo_de_exceso, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec); //Falta agregar Hora del sistema
                             
                    write(file_desc, string_registro, sizeof(string_registro));

                }
                cont = 0;
                *string_registro = '\0';
                *tipo_de_exceso = '\0';
            }
            pclose(cmd);
            close(file_desc);
            sleep(1);
        }
    }
    else{
        pid_registro = fork(); //Creo a Registro
        if (pid_registro == 0)
        {
            char reg_line[40];
            int file_desc;
            reg_struct reg_info;  
            //Codigo Hijo Registro
            while(1){
                file_desc = open(control_fifo, O_RDONLY);
                while(read(file_desc, reg_line, sizeof(reg_line))){
                    char_to_reg_struct(reg_line, &reg_info);
                    int value = hashmap_get(hashmap, (&reg_info)->pid);
                    if (value == MAP_MISSING){
                        if (strcmp("CPU", (&reg_info)->desc) == 0){
                            hashmap_put(hashmap, (&reg_info)->pid, 0);// 0 = cpu
                        }
                        else if (strcmp("MEMORIA", (&reg_info)->desc) == 0){
                            hashmap_put(hashmap, (&reg_info)->pid, 1);// 1 = mem
                        }
                        else{
                            hashmap_put(hashmap, (&reg_info)->pid, 2);// 2 = ambos
                        }
                        fprintf(reg_file, "%s\n", reg_line);
                    }
                    else{
                        if (value == 0 && strcmp("MEMORIA", (&reg_info)->desc) == 0){
                            hashmap_put(hashmap, (&reg_info)->pid, 2);
                            fprintf(reg_file, "%s\n", reg_line);
                        }
                        else if (value == 1 && strcmp("CPU", (&reg_info)->desc) == 0){
                            hashmap_put(hashmap, (&reg_info)->pid, 2);
                            fprintf(reg_file, "%s\n", reg_line);
                        }
                        else if (value == 0 && strcmp("AMBOS", (&reg_info)->desc) == 0){
                            hashmap_put(hashmap, (&reg_info)->pid, 2);
                            fprintf(reg_file, "%s\n", reg_line);
                        }
                        else if (value == 1 && strcmp("AMBOS", (&reg_info)->desc) == 0){
                            hashmap_put(hashmap, (&reg_info)->pid, 2);
                            fprintf(reg_file, "%s\n", reg_line);
                        }
                    }
                }
                fflush(reg_file);
                close(file_desc);
                sleep(1);
            }
        }
    }   
    
    waitpid(pid_control, &control_status, 0);
    waitpid(pid_registro, &reg_status, 0);
    return EXIT_SUCCESS; 
}
