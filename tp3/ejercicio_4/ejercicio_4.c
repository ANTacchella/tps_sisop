#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h>
#include <stdlib.h> 
#include <signal.h> 

/*
# ejercicio_4.c | Trabajo Práctico 3) Ejercicio 4) | Primera Entrega
# Maximiliano José Bernardo | 41912800
# Nicolás Agustín Fábregues | 41893896
# Joela Belén Kachanosqui | 41917556
# Alejandro Nicolás Tacchella | 41893930
# Tomás Martín Vera | 41988332
*/

pid_t pid_control = -1 ; //Global
pid_t pid_registro = -1 ; //Global

void signalHandler( int sig ){
    //Aca deberiamos matar a los hijos.

    return EXIT_SUCCESS;
}


int main() 
{ 
    //signal handler SIGUSR1
    signal( SIGUSR1, &signalHandler );

    //crear hijos y dejarlos en segundo plano
    
    pid_control = fork(); //Creo a Control
    if (pid_control == 0)
    {
        //Codigo Hijo Control
        while( 1 ){


            sleep(1);
        }
    }
    else{
        pid_registro = fork(); //Creo a Registro
        if (pid_registro == 0)
        {
            //Codigo Hijo Registro
            while( 1 ){

                
            sleep(1);
        }
        }
    }
    
    
    while( 1 ){


        sleep(1);
    }
    
    
    //podriamos crear FIFO aca tambien
    
    
    
    
    
    
    
    
    
    
    int fd; 
  
    // FIFO file path 
    char * myfifo = "/tmp/myfifo"; 
  
    // Creating the named file(FIFO) 
    // mkfifo(<pathname>, <permission>) 
    mkfifo(myfifo, 0666); 
  
    char arr1[80], arr2[80]; 
    while (1) 
    { 
        // Open FIFO for write only 
        fd = open(myfifo, O_WRONLY); 
  
        // Take an input arr2ing from user. 
        // 80 is maximum length 
        fgets(arr2, 80, stdin); 
  
        // Write the input arr2ing on FIFO 
        // and close it 
        write(fd, arr2, strlen(arr2)+1); 
        close(fd); 
  
        // Open FIFO for Read only 
        fd = open(myfifo, O_RDONLY); 
  
        // Read from FIFO 
        read(fd, arr1, sizeof(arr1)); 
  
        // Print the read message 
        printf("User2: %s\n", arr1); 
        close(fd); 
    }






    return 0; 
} 