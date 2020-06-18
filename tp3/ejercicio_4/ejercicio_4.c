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

void signalHandler(int sig){
    //Aca deberiamos matar a los hijos.
    kill(pid_control,SIGKILL);
    kill(pid_registro,SIGKILL);
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

int main() 
{ 
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
                write(file_desc, ps_line, sizeof(ps_line));
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
