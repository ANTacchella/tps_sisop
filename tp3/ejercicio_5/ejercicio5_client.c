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
#include <errno.h>
#include "lista.h"
#include "fechas.h"
#include <regex.h>

/*
# ejercicio5_client.c | Trabajo Práctico 3) Ejercicio 5) | Primera reentrega
# Maximiliano José Bernardo | 41912800
# Nicolás Agustín Fábregues | 41893896
# Joela Belén Kachanosqui | 41917556
# Alejandro Nicolás Tacchella | 41893930
# Tomás Martín Vera | 41988332
*/

int socket_cliente;

pthread_mutex_t mutex_main = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_hilo = PTHREAD_MUTEX_INITIALIZER;
t_usuario user;
int logged = 0, error = 0;
t_lista alumnos;


void signalHandler(int );
void * escuchar_servidor();
void ayuda();
void print_menu_alu();
void print_menu_doc();

extern int errno;

int main(int argc, char *argv[]){

    char ip[20];
    int PORT=7500,len,corte;
    struct sockaddr_in direccion_server;
	struct hostent *hstnm;
    pthread_t hilo_escucha;
    char peticion[300];
    if(argc == 2) // Validamos los parametros.
    {
        if(strcmp(argv[1],"-h")==0 || strcmp(argv[1],"-?")==0 || strcmp(argv[1],"-help")==0)
        {
            ayuda();
        return 0;
        }
    }
    if (argc==3)
    {

        strcpy(ip,argv[1]);
        strcat(ip,"\0");
        PORT=atoi(argv[2]);
    }
    
    if( argc < 2 ||argc > 3)
    {
        printf("La cantidad de argumentos es invalida, Se acepta el puerto o ninguno\n");
        exit(EXIT_FAILURE);
    }

    socket_cliente=socket(AF_INET,SOCK_STREAM,0); // creamos el socket
    if(socket_cliente<0)
    {
        printf("Fallo la creacion del socket\n");
        exit(EXIT_FAILURE);
    }

    bzero( (char *) &direccion_server, sizeof(struct sockaddr_in) ); // configuramos la estructura para conectarnos al servidor
    direccion_server.sin_family = AF_INET;
    direccion_server.sin_port = htons(PORT);
    direccion_server.sin_addr.s_addr = inet_addr( ip );
    int res = connect(socket_cliente, (struct sockaddr *)&direccion_server, sizeof(direccion_server)); // Hacemos la conexion al servidor.

    if ( res < 0) 
    { 
        int errnum = errno;
        printf("%s\n",strerror(errnum));
        printf("\nFallo la conexion.\n"); 
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGQUIT, signalHandler);
    pthread_mutex_lock(&mutex_hilo);
    pthread_create(&hilo_escucha,NULL,escuchar_servidor,NULL); // hilo que escucha los mensajes del servidor y imprime
    pthread_mutex_lock(&mutex_main);
    pthread_mutex_lock(&mutex_hilo);
    corte=1;

    int intento = 0;
    
    do {
        if(intento==0){
            printf("\n\t\tINICIO DE SESION\n");
            printf("-----------------------------------------------------\n");
        }
        else{
            printf("\nUsuario y/o contraseña incorrectos, intente nuevamente\n");
        }
        printf("\nUSUARIO: ");
        scanf("%s",user.usuario);
        fflush(stdin);
        printf("CLAVE: ");
        scanf("%s",user.clave);
        fflush(stdin);
        intento++;

        sprintf(peticion,"a;%s;%s",user.usuario,user.clave);

        len = send(socket_cliente,peticion,300,0); // Se envia la consulta al servidor
        if(len < 0) 
        {
            perror("Mensaje no enviado!!\n");
            exit(1);
        }
        bzero(peticion,300);
        printf("Mensaje enviado, esperando respuesta...\n");
            
        pthread_mutex_lock(&mutex_main); //Espera respuesta	del servidor	
        sleep(2);
        
    } while (intento < 3 && logged == 0);

    if (logged == 1) {
        system("clear");
        printf("\nBienvenido al Sistema %s %s\n",(user.rol == 'A' ? "Alumno" : "Docente"),user.usuario);
        
    } else {
        printf("\nHa sobrepasado el numero maximo de intentos permitidos\n");
        close(socket_cliente);
        exit(EXIT_FAILURE);
    }

    int op = 0;

    regex_t regex;
    char sregex[100];
    int reti,val_fecha;

    sprintf(sregex,"[0-9]{4}-[0-9]{2}-[0-9]{2}$");

    reti = regcomp(&regex, sregex, REG_EXTENDED);
    if (reti) {
        fprintf(stderr, "Could not compile regex\n");
        exit(EXIT_FAILURE);
    }

    do{
        if(user.rol == 'A'){
            print_menu_alu();
            printf("\n\nElija una de las opciones: ");
            fflush(stdin);
            scanf("%d",&op);
            system("clear");

            if(op == 1){
                char fecha[12];
                bzero(fecha,sizeof(fecha));

                printf("Ingrese la fecha a consultar (yyyy-mm-dd): ");
                fflush(stdin);
                scanf("%s",fecha);
            
                t_fecha date;
                sscanf(fecha,"%d-%d-%d",&date.anio,&date.mes,&date.dia);
                val_fecha = validar(&date);
                
                reti = regexec(&regex, fecha, 0, NULL, 0);
                while(reti == REG_NOMATCH || val_fecha == 0){
                    printf("\nFormato de fecha inválido!!\n\nVuelva a ingresar (yyyy-mm-dd): ");
                    fflush(stdin);
                    scanf("%s",fecha);

                    reti = regexec(&regex, fecha, 0, NULL, 0);
                    sscanf(fecha,"%d-%d-%d",&date.anio,&date.mes,&date.dia);
                    val_fecha = validar(&date);

                }

                sprintf(peticion,"b;%s",fecha);

                len = send(socket_cliente,peticion,300,0); // Se envia la consulta al servidor
                if(len < 0) 
                {
                    perror("Mensaje no enviado!!\n");
                    exit(1);
                }
                bzero(peticion,300);
                printf("Mensaje enviado, esperando respuesta...\n");
                    
                pthread_mutex_lock(&mutex_main); //Espera respuesta	del servidor	
                sleep(2);

                printf("\nPresione cualquier tecla para continuar...\n");
                char c;
                while((c= getchar()) != '\n' && c != EOF);
                getchar();
            }
            else if(op == 2){

                sprintf(peticion,"c;0");
                
                len = send(socket_cliente,peticion,300,0); // Se envia la consulta al servidor
                if(len < 0) 
                {
                    perror("Mensaje no enviado!!\n");
                    exit(1);
                }
                bzero(peticion,300);
                printf("Mensaje enviado, esperando respuesta...\n");
                    
                pthread_mutex_lock(&mutex_main); //Espera respuesta	del servidor	
                sleep(2);

                printf("\nPresione cualquier tecla para continuar...\n");
                char c;
                while((c= getchar()) != '\n' && c != EOF);
                getchar();

            }
            else if(op == 3){
                printf("Saliendo del sistema.");
                fflush(stdout);
                for(int i = 0; i < 5; i++){
                    sleep(1);
                    printf(".");
                    fflush(stdout);
                }
                close(socket_cliente);
                exit(EXIT_SUCCESS);
            }
            else{
                printf("Opción inválida!! Por favor elija una opción correcta: ");
                fflush(stdin);
                scanf("%d",&op);
            }
        }
        else{
            print_menu_doc();
            printf("\n\nElija una de las opciones: ");
            fflush(stdin);
            scanf("%d",&op);
            system("clear");

            if(op == 1){
                char fecha[12];
                bzero(fecha,sizeof(fecha));

                printf("Ingrese la fecha a consultar (yyyy-mm-dd): ");
                fflush(stdin);
                scanf("%s",fecha);

                t_fecha date;
                sscanf(fecha,"%d-%d-%d",&date.anio,&date.mes,&date.dia);
                val_fecha = validar(&date);
                
                reti = regexec(&regex, fecha, 0, NULL, 0);
                while(reti == REG_NOMATCH || val_fecha == 0){
                    printf("\nFormato de fecha inválido!!\n\nVuelva a ingresar (yyyy-mm-dd): ");
                    fflush(stdin);
                    scanf("%s",fecha);

                    reti = regexec(&regex, fecha, 0, NULL, 0);
                    sscanf(fecha,"%d-%d-%d",&date.anio,&date.mes,&date.dia);
                    val_fecha = validar(&date);

                }

                sprintf(peticion,"d;%s",fecha);

                len = send(socket_cliente,peticion,300,0); // Se envia la consulta al servidor
                if(len < 0) 
                {
                    perror("Mensaje no enviado!!\n");
                    exit(1);
                }
                bzero(peticion,300);
                printf("Mensaje enviado, esperando respuesta...\n");
                    
                pthread_mutex_lock(&mutex_main); //Espera respuesta	del servidor	
                sleep(2);

                printf("\nPresione cualquier tecla para continuar...\n");
                char c;
                while((c= getchar()) != '\n' && c != EOF);
                getchar();
            }
            else if(op == 2){
                char fecha[12];
                bzero(fecha,sizeof(fecha));

                printf("Ingrese la fecha de la clase (yyyy-mm-dd): ");
                fflush(stdin);
                scanf("%s",fecha);

                t_fecha date;
                sscanf(fecha,"%d-%d-%d",&date.anio,&date.mes,&date.dia);
                val_fecha = validar(&date);
                
                reti = regexec(&regex, fecha, 0, NULL, 0);
                while(reti == REG_NOMATCH || val_fecha == 0){
                    printf("\nFormato de fecha inválido!!\n\nVuelva a ingresar (yyyy-mm-dd): ");
                    fflush(stdin);
                    scanf("%s",fecha);

                    reti = regexec(&regex, fecha, 0, NULL, 0);
                    sscanf(fecha,"%d-%d-%d",&date.anio,&date.mes,&date.dia);
                    val_fecha = validar(&date);

                }

                sprintf(peticion,"e;0");

                len = send(socket_cliente,peticion,300,0); // Se envia la consulta al servidor
                if(len < 0) 
                {
                    perror("Mensaje no enviado!!\n");
                    exit(1);
                }
                bzero(peticion,300);
                printf("Mensaje enviado, esperando respuesta...\n");
                    
                pthread_mutex_lock(&mutex_main); //Espera respuesta	del servidor	
                sleep(2);

                while(alumnos->ant){
                    alumnos = alumnos->ant;
                }

                sprintf(peticion,"f;%s",fecha);
                len = send(socket_cliente,peticion,300,0);

                if(len < 0){
                    perror("Mensaje no enviado!!\n");
                    exit(1);
                }
                pthread_mutex_lock(&mutex_main); //Espera respuesta	del servidor	

                if(error == 1){
                    printf("Error! El archivo de asistencias para esa fecha ya existe!\n");
                    fflush(stdout);
                    sleep(2);
                    continue;
                }
                else if(error == 2){
                    printf("Error al crear el archivo de asistencias!\n");
                    fflush(stdout);
                    sleep(2);
                    continue;
                }
                sleep(2);

                t_asist asist;
                char op;
                printf("Ingrese los valores de asistencia para cada alumno:\n\n");

                while (lista_vacia(&alumnos) != -1)
                {
                    op = 0;
                    ver_registro(&alumnos,&asist);
                    desalistar(&alumnos,&asist,compare);

                    printf("%s: ",asist.usuario);

                    int c;
                    while((c= getchar()) != '\n' && c != EOF);
                    scanf("%c",&op);

                    while (op != 'A' && op != 'P'){
                        printf("Valor inválido!!\n\n%s: ",asist.usuario);
                        while((c= getchar()) != '\n' && c != EOF);
                        scanf("%c",&op);
                    }

                    asist.asist = op;

                    sprintf(peticion,"%s|%c",asist.usuario,asist.asist);
                    len = send(socket_cliente,peticion,300,0);

                    if(len < 0){
                        perror("Mensaje no enviado!!\n");
                        exit(1);
                    }
                    
                }
                
                sprintf(peticion,"Fin de lista");
                len = send(socket_cliente,peticion,300,0);

                if(len < 0){
                    perror("Mensaje no enviado!!\n");
                    exit(1);
                }

                printf("\nPresione cualquier tecla para continuar...\n");
                char c;
                while((c= getchar()) != '\n' && c != EOF);
                getchar();

            }
            else if(op == 3){
                printf("Saliendo del sistema.");
                fflush(stdout);
                for(int i = 0; i < 5; i++){
                    sleep(1);
                    printf(".");
                    fflush(stdout);
                }
                close(socket_cliente);
                exit(EXIT_SUCCESS);
            }
            else{
                printf("Opción inválida!! Por favor elija una opción correcta: ");
                fflush(stdin);
                scanf("%d",&op);
            }
        }
    }while(corte);


    close(socket_cliente);
    return 0;
}

void * escuchar_servidor() // funcion que ejecuta el hilo que escucha al servidor
{
    int len,escuchar=1,rec_lista=0;
    char msg[300];
    bzero(msg,300);
    t_asist asist;
    crear_lista(&alumnos);
    while((len=recv(socket_cliente,msg,300,0)>0))
    {
        if(rec_lista == 0){
            if(strcmp(msg,"Error de login")==0){
                logged = 0;
                pthread_mutex_unlock(&mutex_main);
            }
            else if(strncmp(msg,"Login exitoso",13) == 0){
                logged = 1;
                sscanf(msg,"Login exitoso;%[^;];%[^;];%c;%[^\n]",user.usuario,user.clave,&user.rol,user.com);
                pthread_mutex_unlock(&mutex_main);
            }
            else if(strcmp(msg,"Presente") == 0){
                system("clear");
                printf("%s\n",msg);
                pthread_mutex_unlock(&mutex_main);
            }
            else if(strcmp(msg,"Ausente") == 0){
                system("clear");
                printf("%s\n",msg);
                pthread_mutex_unlock(&mutex_main);
            }
            else if(strcmp(msg,"No hay registro") == 0){
                system("clear");
                printf("%s\n",msg);
                pthread_mutex_unlock(&mutex_main);
            }
            else if(strncmp(msg,"Porcentajes",11) == 0){
                char* aux = strtok(msg,";");
                aux = strtok(NULL,";");

                system("clear");
                printf("Porcentaje de presentes: %s%c\n",aux,'%');

                aux = strtok(NULL,";");
                printf("Porcentaje de ausente: %s%c\n",aux,'%');

                pthread_mutex_unlock(&mutex_main);

            }
            else if(strcmp(msg,"Error de archivos de asistencia") == 0){
                system("clear");
                printf("%s\n",msg);
                pthread_mutex_unlock(&mutex_main);
            }
            else if(strcmp(msg,"Inicio de lista") == 0){
                system("clear");
                rec_lista = 1;
                printf("NOMBRE|PRESENTE\n");
            }
            else if(strcmp(msg,"Inicio de lista de alumnos") == 0){
                rec_lista = 2;
            }
            else if(strcmp(msg,"Archivo de asistencia ya existente") == 0){
                error = 1;
                pthread_mutex_unlock(&mutex_main);
            }
            else if(strcmp(msg,"Error al crear archivo de asistencia") == 0){
                error = 2;
                pthread_mutex_unlock(&mutex_main);
            }
            else if(strcmp(msg,"Archivo de asistencia creado exitosamente") == 0){
                error = 0;
                pthread_mutex_unlock(&mutex_main);
            }
            if(strcmp(msg,"-Servidor Desconectado")==0)
                break;
            if(strcmp(msg,"Conectado al servidor\n")==0)
            {
                system("clear");
                printf("%s",msg);
                pthread_mutex_unlock(&mutex_hilo);
            }
        }
        else if(rec_lista == 1){
            if(strcmp(msg,"Fin de lista") == 0){
                rec_lista = 0;
                pthread_mutex_unlock(&mutex_main);
            }
            else{
                printf("%s\n",msg);
            }
        }
        else if(rec_lista == 2){
            if(strcmp(msg,"Fin de lista") == 0){
                rec_lista = 0;
                pthread_mutex_unlock(&mutex_main);
            }
            else{
                strcpy(asist.usuario,msg);
                alistar_orden(&alumnos,&asist,compare);
            }
        }   
        
        memset(msg,'\0',sizeof(msg));
    }
    printf("Se cerro el servidor\n");
    close(socket_cliente);
    exit(EXIT_SUCCESS);
}

void signalHandler( int dummy) // funcion para controlar señales del cliente
{
    close(socket_cliente);
    printf("Socket cerrado.\n");
    exit(EXIT_SUCCESS);
}

void print_menu_alu(){
    printf("\n********************************MENU********************************\n");
    printf("*                                                                  *\n");
    printf("*\t1- Consultar asistencia en fecha                           *\n");
    printf("*\t2- Consultar porcentaje de asistencias/inasistencias       *\n");
    printf("*\t3- Salir                                                   *\n");
    printf("*                                                                  *\n");
    printf("********************************************************************\n");
}

void print_menu_doc(){
    printf("\n********************************MENU********************************\n");
    printf("*                                                                  *\n");
    printf("*\t1- Consultar asistencias de alumnos en fecha               *\n");
    printf("*\t2- Cargar asistencias                                      *\n");
    printf("*\t3- Salir                                                   *\n");
    printf("*                                                                  *\n");
    printf("********************************************************************\n");

}

void ayuda(){
    printf("\n######    HELP Ejercicio5_Client    ######\n\n");
    printf("Este programa dado dos parámetros de entrada: IP y PORT \n");
    printf("Se conecta con el servidor establecido en la IP y PORT dados\n");
    printf("y dependiendo del ROL del usuario (Alumno o Docente) se le\n");
    printf("presentarán las siguientes opciones: \n\n");
    printf("Docente:\n");
    printf("-Consultar asistencias de sus alumnos: \n");
    printf("\tEnviará como parámetro la fecha (formato\n");
    printf("\tde todas las fechas yyyy-mm-dd) a consultar,\n");
    printf("\ty se imprimirá por pantalla el listado de\n");
    printf("\talumnos del curso asignado mostrando el presentismo correspondiente.\n");
    printf("-Cargar asistencias:\n");
    printf("\tComo primer paso escribirá la fecha de asistencia\n");
    printf("\ty luego se imprimirá por pantalla (de uno a la vez)\n");
    printf("\tlos nombres de todos los alumnos asociados a esa comisión.\n");
    printf("\tEl docente deberá indicar si están presentes o no\n");
    printf("\tcompletando con la letra P o A en cada caso.\n\n");
    printf("Alumno:\n");
    printf("-Consultar si asistió determinado día\n");
    printf("\tEnviará como parámetro la fecha (formato\n");
    printf("\tde todas las fechas yyyy-mm-dd) a consultar,\n");
    printf("\ty se imprimirá por pantalla \"Presente\" o \"Ausente\".\n");
    printf("-Consultar el porcentaje de asistencias inasistencias\n");
    printf("\tImprimirá por pantalla los porcentajes de asistencias\n");
    printf("\te inasistencias del alumno.\n");
    printf("\nEjemplo: ./Ejercicio5_Client IP PORT\n");
}
