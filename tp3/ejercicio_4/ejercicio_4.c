#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <stdlib.h> 
#include <signal.h>
#include <wait.h>

/*
ejercicio_4.c | Trabajo Práctico 3) Ejercicio 4) | Primera Entrega
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

pid_t pid_control = -1; //Global
pid_t pid_registro = -1; //Global
int control_status;
int reg_status;
FILE *cmd;
char *control_fifo = "./control_fifo";
char *nombre_arch_registro = "registro.txt";

void help(){
    printf("\n######    HELP Ejercicio_4    ######\n\n");
    printf("Este programa dado dos parámetros de entrada: X e Y \n");
    printf("Controla el uso de memoria y cpu por parte de los procesos cada 1seg.\n\n");
    printf("Verificando que no se exceda del valor limite X e Y ingresado.\n");
    printf("En caso de que un proceso supere esos limites\n");
    printf("se lo registrara en un archivo: %s \n", nombre_arch_registro);
    printf("\n");
    printf("Para ejecutar el programa: \"./Ejercicio_4 X Y\",\n");
    printf("donde X (limite de memoria) debe ser un float mayor o igual a cero.\n");
    printf("donde y (limite de cpu) debe ser un float mayor o igual a cero.\n\n");
    
}

void signalHandler(int sig){
    //Aca deberiamos matar a los hijos.
    kill(pid_control,SIGTERM);
    kill(pid_registro,SIGTERM);
    exit(EXIT_SUCCESS);
}

void char_to_ps_struct(const char *ps_line, ps_struct *ps_info){
    int res;
    res = sscanf(ps_line, "%d %f %f %[^;]", &ps_info->pid, &ps_info->cpu, &ps_info->mem, ps_info->comm);
    if(res == EOF){
        printf("¡ERROR! Sscanf failure: No se pudo leer el string del proceso\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[]) 
{
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
            printf ("\n¡ERROR!: Ingrese un float mayor o igual a cero.\n");
            printf ("%s\n", info);
            return EXIT_FAILURE;
        }
    }

    //signal handler SIGUSR1
    signal(SIGUSR1, &signalHandler);
    mkfifo(control_fifo, 0666);
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

        //Codigo Hijo Control
        while(1){
            file_desc = open(control_fifo, O_WRONLY);
            sprintf(ps, "ps --no-headers -Ao pid,%%cpu,%%mem,comm");
            cmd = popen(ps, "r");
            if (cmd == NULL) {
                printf("¡ERROR! Popen failure: No se pudo leer la salida de ps\n");
                exit(EXIT_FAILURE);
            }
            while (fgets(ps_line, sizeof(ps_line), cmd) != NULL) {
                //printf("%s\n", ps_line);
                char_to_ps_struct(ps_line, &ps_info);
                //printf("%d %f %f %s\n", (&ps_info)->pid, (&ps_info)->cpu, (&ps_info)->mem, (&ps_info)->comm);

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

                    sprintf(string_registro,"%d %s %s", (&ps_info)->pid, (&ps_info)->comm, tipo_de_exceso); //Falta agregar Hora del sistema
                    //printf("SUPERA LIMITE:  %d %s %s\n", (&ps_info)->pid, (&ps_info)->comm, tipo_de_exceso);
                    
                    
                    write(file_desc, string_registro, sizeof(string_registro));

                }
                //printf("NO SUPERA LIMITE:  %d %f %f %s\n", (&ps_info)->pid, (&ps_info)->cpu, (&ps_info)->mem, (&ps_info)->comm);
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
            char ps_line[40];
            int file_desc;
            //Codigo Hijo Registro
            while(1){
                file_desc = open(control_fifo, O_RDONLY);
                while(read(file_desc, ps_line, sizeof(ps_line))){
                    printf("Registro: %s\n", ps_line);
                    //parsear
                }
                close(file_desc);
                sleep(1);
            }
        }
    }   
    
    waitpid(pid_control, &control_status, 0);
    waitpid(pid_registro, &reg_status, 0);
    //podriamos crear FIFO aca tambien
    return 0; 
}
