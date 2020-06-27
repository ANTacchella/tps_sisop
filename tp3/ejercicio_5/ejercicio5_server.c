#include "lista.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h> 
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <netdb.h>
#include <fcntl.h>
#include <pthread.h>

typedef struct
{
    int id_socket;
    pthread_t id_thread;
}info_cliente;

typedef struct{
    char usuario[40];
    char clave[20];
    char rol;
    char com[6];
}t_usuario;

//Función que parsea los datos de un string a un t_usuario
void text_to_user(const char *cad, t_usuario *user){
    char usr[40],clv[20],com[6];
    int c;
    c = sscanf(cad,"%[^|]|%[^|]|%c|%[^\n]",usr,clv,&user->rol,com);

    strncpy(user->usuario,usr,sizeof(user->usuario));
    strncpy(user->clave,clv,sizeof(user->clave));
    strncpy(user->com,com,sizeof(user->com));
}


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

char archivo[100];
struct sockaddr_in direccion;
info_cliente clientes[15];
int socket_server;
int conectados=0;
int escuchar=1;
// t_lista lista;

void ayuda(){printf("\nAIUDAAAAAAAA\n");}
int abrir_socket();
void conectar_puerto(int ,int );
void* atender_cliente(void* args);
void signalHandler(int dummy);

int main(int argc, char *argv[])
{

    int PORT , len,res=0,habilitado = 1;
    // Validacion de parametros
    if( argc != 2 ){
        printf("La cantidad de parametros es invalida\n");
        return 0;
    }
    if(argc == 2)
    {
        if(strcmp(argv[1],"-h")==0 || strcmp(argv[1],"-?")==0 || strcmp(argv[1],"-help")==0)
        {
            ayuda();
            return 0;
        }
        else
        {
            PORT = atoi(argv[1]); //Asigno el puerto seleccionado
        }
        
    }
    
    
    socket_server= socket(AF_INET,SOCK_STREAM,0); //creamos el socket
    if(socket_server == -1)
    {
        printf("Error al abrir socket\n");
        exit(EXIT_FAILURE);
    }

    res = setsockopt(socket_server, SOL_SOCKET, SO_REUSEADDR, &habilitado, sizeof(int)); // ponemos al socket para que se pueda re-utilizar
   
    if(res < 0)
    {
        printf("No se pudo configurar las opciones del socket\n");
        exit(EXIT_FAILURE);
    }

    bzero( (char*) &direccion, sizeof(struct sockaddr_in) );
    direccion.sin_family = AF_INET;
    direccion.sin_port=htons(PORT);
    direccion.sin_addr.s_addr = INADDR_ANY;

    if(bind(socket_server,(struct sockaddr *) &direccion,sizeof(struct sockaddr_in))<0) //Enlazamos el socket con el puerto
    {
        perror("Fallo el bind\n");
        exit(EXIT_FAILURE);
    }
    printf("se conecto bien el puerto\n");
    if(listen(socket_server,15)<0) //Establecemos la cantidad maxima de personas a las que se recibira
    {
        perror("error al usar Listen");
        exit(EXIT_FAILURE);
    }
    signal(SIGINT,signalHandler); //
    signal(SIGTERM,signalHandler); // Definimos el comportamiento para las señales que se emitiran al demonio; 
    signal(SIGQUIT,signalHandler); //

    // crear_lista(&lista);
    // cargar_lista(&lista,archivo); // Creamos la lista donde se almacenara los registros de la base de datos.

    pid_t pid,sid;
    // pid = fork(); // Hacemos un fork y creamos el demonio.
    // if (pid < 0)
    // {
    //     exit(EXIT_FAILURE);
    // }
    // if (pid > 0) 
    // {
    //     exit(EXIT_SUCCESS);
    // }
    // sid = setsid(); // Cambiamos el sid para que el proceso demonio no muera.
    // if (sid < 0) 
    // {
    //   perror("Nuevo SID fallo");
    //   exit(EXIT_FAILURE);
    // }
    // umask(0); // Le damos permisos al demonio

    daemon(1,1);

    while(escuchar) 
    {
        struct sockaddr_in cliente_direccion;
        socklen_t client_size = sizeof(struct sockaddr_in);
        int connect = accept(socket_server,(struct sockaddr*)&cliente_direccion,&client_size); // Hacemos al cliente
        if(connect == -1)
            printf("No se conecto\n");
        else
        {
            printf("\nSe conectó correctamente\n\n");
            pthread_mutex_lock(&mutex);
			clientes[conectados].id_socket=connect;
            pthread_create(&clientes[conectados].id_thread,NULL,&atender_cliente,&connect); // Asignamos un hilo que escucharan las peticiones del mismo
            conectados++;
            len = send(connect,"Conectado al servidor\n",300,0);
            if(len < 0)
            {
                perror("Mensaje no enviado!\n");
                exit(EXIT_FAILURE);
            }
            pthread_mutex_unlock(&mutex);
        }
    }

    return 0;
}

void signalHandler(int dummy) // funcion que atiende las señales
{
    escuchar = 0;
    pthread_mutex_lock(&mutex);
    for(int i = 0; i< conectados ; i++)
    {
        send(clientes[i].id_socket,"-Servidor Desconectado",100,0); // Desconecto a los clientes
        close(clientes[i].id_socket);
    }
    pthread_mutex_unlock(&mutex);
    // vaciar_lista(&lista);
    close(socket_server); // Cierro el server
    exit(EXIT_SUCCESS);
}

int login_al_sistema(t_usuario *user){

    //Abro el archivo de usuarios en modo lectura de texto
    FILE *fp = fopen("./archivos/Usuarios.txt","rt");
    if(fp == NULL){
        printf("\n¡Error al abrir el archivo usuarios!\n");
        exit(1);
    }
        
    char buf[200];
    t_usuario aux;
    int encabezado = 1;

    while(fgets(buf,sizeof(buf),fp) != NULL)
    {   
        if(!encabezado){
            text_to_user(buf,&aux);
            if(!strcmp(aux.usuario,user->usuario) && !strcmp(aux.clave,user->clave) ){
                *user=aux;
                fclose(fp);
                return 1;                
            }
                     
        }
        encabezado = 0;
    }
    fclose(fp);
    return 0;
}

int login(const char *param, char* respuesta){
    t_usuario user;

    sscanf(param,"%[^;];%[^\n]",user.usuario,user.clave);

    int result = login_al_sistema(&user);

    if(result == 1){
        sprintf(respuesta,"Login exitoso;%s;%s;%c;%s",user.usuario,user.clave,user.rol,user.com);
        return 1;
    }
    else{
        return 0;
    }
}

void* atender_cliente(void* args) // Funcion que se encarga de escuchar la consulta del cliente asignado al hilo
{
    info_cliente client = *((info_cliente*) args);
    char peticion[300], operacion, param[300];
    char respuestaErronea[100];
    // t_lista lista_respuesta;
    // crear_lista(&lista_respuesta);
    int i,j,len;
    while((len=recv(client.id_socket,peticion,300,0))>0)
    {
        if(len > 0)
        {

            peticion[len-1]='\0';
            pthread_mutex_lock(&mutex);

            sscanf(peticion,"%c;%s",&operacion,param);
            char respuesta[300];
            int res;

            switch (operacion)
            {
                //Operación de login
                case 'a':
                    
                    res = login(param,respuesta);
                    if(res == 1){
                        len = send(client.id_socket,respuesta,300,0);
                    }
                    else{
                        sprintf(respuesta,"Error de login");
                        len = send(client.id_socket,respuesta,300,0);
                    }

                    if(len < 0){
                        perror("Mensaje no enviado!!\n");
                    }
                    pthread_mutex_unlock(&mutex);

                    break;

                //Operación de consultar asistencias
                case 'b':

                    // res = consultar_asistencia()
                    break;
                
                default:
                    break;
            }

            pthread_mutex_unlock(&mutex);
            }
            //memset(peticion,'\0',sizeof(peticion));
    }
    pthread_mutex_lock(&mutex); // si salio del while significa que se desconecto el cliente
    for(i = 0; i < conectados; i++) {
        if(clientes[i].id_socket == client.id_socket) { // localizo el cliente y lo saco del vector de clientes
            j = i;
            while(j < conectados-1) {
                clientes[j].id_socket = clientes[j+1].id_socket;
                clientes[j].id_thread = clientes[j+1].id_thread;
                j++;
            }
        }
    }
	conectados--;
	pthread_mutex_unlock(&mutex);
    close(client.id_socket); //Cierro el socket del cliente
	pthread_exit((void*) 0);
}