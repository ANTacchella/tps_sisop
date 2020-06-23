#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h> 
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <netdb.h>

typedef struct{
    char usuario[40];
    char clave[20];
    char rol[2];
    char comicion[6];
}t_usuario;

int socket_cliente;

pthread_mutex_t mutex_main = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_hilo = PTHREAD_MUTEX_INITIALIZER;


int main(int argc, char *argv[])
{
    t_usuario user;
    char login_resp[40];
    char user_login[80];
    char IP[20] = "192.168.100.20";
    int PORT=10500, len, intento = 0, ingresa = 0;
    struct sockaddr_in direccion_server;
	struct hostent *hstnm;
    pthread_t hilo_escucha;
    char peticion[300];
    if(argc == 2) // Validamos los parametros.
    {
        if(strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"-?") == 0 || strcmp(argv[1],"--help") == 0)
        {
            //Llamada a help
            return EXIT_SUCCESS;
        }
    }
    if (argc >= 2)
    {
        //Se envía como parámetro más que el help
        exit(EXIT_FAILURE);
    }

    socket_cliente=socket(AF_INET,SOCK_STREAM,0); // creamos el socket
    if(socket_cliente < 0)
    {
        printf("Falló el intento de conexión al servidor.\n");
        exit(EXIT_FAILURE);
    }
    
	bzero((char *)&direccion_server, sizeof(struct sockaddr_in) ); // configuramos la estructura para conectarnos al servidor
    direccion_server.sin_family = AF_INET;
    direccion_server.sin_port = htons(PORT);
    direccion_server.sin_addr.s_addr = inet_addr(IP);
    int res = connect(socket_cliente, (struct sockaddr *)&direccion_server, sizeof(direccion_server)); // Hacemos la conexion al servidor.
    if(res < 0) 
    { 
        printf("\nFalló la conexión al servidor.\n"); 
        exit(EXIT_FAILURE);
    } 

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGQUIT, signalHandler);
    pthread_mutex_lock(&mutex_hilo);
    pthread_create(&hilo_escucha, NULL, escuchar_servidor, NULL); // hilo que escucha los mensajes del servidor y imprime
    pthread_mutex_lock(&mutex_main);
    pthread_mutex_lock(&mutex_hilo);
    do // Hasta que no se ingrese "QUIT" Escucha las consultas del cliente por stdin
    {
        printf("Ingrese su usuario:\n");
        if(fgets(user.usuario, 40, stdin) == NULL){
            printf("Debe ingresar un nombre de usuario válido.\n");
            continue;
        }
        printf("Ingrese su contraseña:\n");
        if(fgets(user.clave, 20, stdin) == NULL){
            printf("Debe ingresar una contraseña.\n");
            continue;
        }
        sprintf(user_login, "LOGIN-%s-%s", user.usuario, user.clave);
        len = send(socket_cliente, user_login, sizeof(user_login), 0); // Se envia la consulta al servidor
        if(len < 0) 
        {
            printf("¡No pudo realizarse el login!\n");
            exit(1);
        }
        while(( len = recv(socket_cliente,login_resp,300,0) > 0 ))
        {
        }   
        if(strcmp(peticion, "QUIT\n") == 0)
        {
            printf("\n\nFin de la consulta\n");
            break;
        }
        else
        {
            len = send(socket_cliente, peticion, 300, 0); // Se envia la consulta al servidor
            if(len < 0) 
            {
        	    perror("¡Mensaje no enviado!\n");
			    exit(1);
            }
            bzero(peticion, 300);
		    printf("Mensaje enviado, esperando respuesta...\n");
				
		    pthread_mutex_lock(&mutex_main); //Espera respuesta	del servidor	
            sleep(2);
        }
        fflush(stdin);
    } while(intento < 3 && ingresa == 0);

    if (ingresa == 1) {
        printf("¡Bienvenido al %s!\n", user.usuario);  
    } else {
        printf("¡Ha sobrepasado el numero máximo de intentos permitidos!\n");
        return EXIT_FAILURE;
    }

    close(socket_cliente);
    return EXIT_SUCCESS;
}

void signalHandler(int dummy) // funcion para controlar señales del cliente
{
    close(socket_cliente);
    printf("¡Conexión terminada!\n");
    exit(EXIT_SUCCESS);
}

void * escuchar_servidor() // funcion que ejecuta el hilo que escucha al servidor
{
    int len,escuchar=1;
    char msg[300];
    bzero(msg,300);
    while((len=recv(socket_cliente,msg,300,0)>0))
    {
            if(strcmp(msg,"fin de consulta")==0){
                break;
            }
            if(strcmp(msg,"-Servidor Desconectado")==0)
                break;
            if(strcmp(msg,"Conectado al servidor\n")==0)
            {
                printf("%s",msg);
                pthread_mutex_unlock(&mutex_hilo);
            }
            else{
                if(strcmp(msg,"fin de lista")!=0)
                {
                    printf("%s\n",msg);
                }
                else{
                    printf("\n");
                    pthread_mutex_unlock(&mutex_main);
                }
            
            }
        
        memset(msg,'\0',sizeof(msg));
    }
    printf("¡Servidor cerrado!\n");
    close(socket_cliente);
    exit(EXIT_SUCCESS);
}
