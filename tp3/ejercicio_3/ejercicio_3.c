#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SHM_PAGOS "/shmPagos" //Nombre del objeto de memoria compartida de pagos
#define SHM_ASIST "/shmAsist" //Nombre del objeto de memoria compartida de asistencias

#define SEM_PAGOS "sem_pagos" //Nombre del semáforo de de pagos
#define SEM_ASIST "sem_asist" //Nombre del semáforo de de asistencias

typedef struct
{
    char dep[20];
    char dia[20];
    float cuota;
}t_cuota;

typedef struct{
    char nom[20];
    char ape[20];
    int dni;
    char dep[20];
    char dia[20];
}t_soc;

typedef struct{
    int dni;
    char fecha[12];
}t_pago;

typedef struct{
    int dni;
    char dia[20];
}t_asis;

typedef struct{
    char nom[20];
    char ape[20];
    int dni;
    char dep[20];
    char dia[20];
    char dia_inc[20];
}t_inc_asis;

//Datos de los deportes
void cargar_cuotas(t_cuota vec[]){
    strcpy(vec[0].dep,"Fútbol");
    strcpy(vec[0].dia,"Lunes");
    vec[0].cuota = 1000;

    strcpy(vec[1].dep,"Fútbol");
    strcpy(vec[1].dia,"Miércoles");
    vec[1].cuota = 1000;

    strcpy(vec[2].dep,"Vóley");
    strcpy(vec[2].dia,"Martes");
    vec[2].cuota = 1200;

    strcpy(vec[3].dep,"Vóley");
    strcpy(vec[3].dia,"Jueves");
    vec[3].cuota = 1200;

    strcpy(vec[4].dep,"Básquet");
    strcpy(vec[4].dia,"Viernes");
    vec[4].cuota = 1300;

    strcpy(vec[5].dep,"Natación");
    strcpy(vec[5].dia,"Sábado");
    vec[5].cuota = 1800;
}

//Función que parsea los datos de un string a un t_soc
void text_to_soc(const char *cad,t_soc *soc){
    char nom[90],ape[90],dep[90],dia[90];
    int c;
    c = sscanf(cad,"%[^;];%[^;];%d;%[^;];%[^\n]",nom,ape,&soc->dni,dep,dia);

    strncpy(soc->nom,nom,sizeof(soc->nom));
    strncpy(soc->ape,ape,sizeof(soc->ape));
    strncpy(soc->dep,dep,sizeof(soc->dep));
    strncpy(soc->dia,dia,sizeof(soc->dia));
}

//Función que parsea los datos de un string a un t_pago
void text_to_pago(const char *cad, t_pago *pago){
    char fecha[90];
    int c;

    c = sscanf(cad,"%d;%[^\n]",&pago->dni,fecha);
    
    strncpy(pago->fecha,fecha,sizeof(pago->fecha));
}

//Función que parsea los datos de un string a un t_asis
void text_to_asist(const char *cad, t_asis *asist){
    char dia[90];
    int c;

    c = sscanf(cad,"%d;%[^\n]",&asist->dni,dia);

    strncpy(asist->dia,dia,sizeof(asist->dia));
}

//Función para calcular el importe de un pago evaluando el descuento
float calcular_importe(t_pago *pago, const t_soc *socios, int cant_soc){

    t_cuota cuotas[6];
    cargar_cuotas(cuotas);

    int descuento,dia;
    char *sdia,dep;

    sdia = strtok(pago->fecha,"-");
    sdia = strtok(NULL,"-");
    sdia = strtok(NULL,"-");

    dia = atoi(sdia);

    descuento = (dia >= 1 && dia <= 10) ? 1 : 0;

    for(int i = 0; i < cant_soc; i++){

        if(pago->dni == socios[i].dni){
            dep = socios[i].dep[0];
        }

    }
    
    switch (dep)
    {
    case 'F':
        return descuento == 0 ? cuotas[0].cuota : cuotas[0].cuota - (cuotas[0].cuota * 0.1);
        break;
    
    case 'V':
        return descuento == 0 ? cuotas[2].cuota : cuotas[2].cuota - (cuotas[2].cuota * 0.1);
        break;

    case 'B':
        return descuento == 0 ? cuotas[4].cuota : cuotas[4].cuota - (cuotas[4].cuota * 0.1);
        break;

    case 'N':
        return descuento == 0 ? cuotas[5].cuota : cuotas[5].cuota - (cuotas[5].cuota * 0.1);
        break;
    
    default:
        return 0;
        break;
    }

}

//Función para listar a todos los socios que no pagaron la cuota mensual
void listar_deudores(int cant_soc,int cant_pagos,const t_soc *socios,const t_pago *pagos){
    int pagado, k = 0;
    t_soc deudores[cant_soc - cant_pagos];

    for(int i = 0; i < cant_soc; i++){

        pagado = 0;

        for(int j = 0; j < cant_pagos; j++){

            if(socios[i].dni == pagos[j].dni){
                pagado = 1;
                break;
            }


        }

        if(pagado == 0){
            deudores[k] = socios[i];
            k++;
        }

    }

    printf("*****************************************ADEUDAN LA CUOTA DEL MES*****************************************\n\n");
    printf("\t\t%-20s\t%-20s\tDNI\t\tDEPORTE\n","NOMBRE","APELLIDO");
    for(int i = 0; i < k; i++){
        printf("\t\t%-20s\t%-20s\t%d\t\t%-20s\n",deudores[i].nom,deudores[i].ape,deudores[i].dni,deudores[i].dep);
    }
    printf("**********************************************************************************************************\n\n");

}

//Función para contar la cantidad de registros en un archivo
int contar_registros_archivo(const char *path){

    int cont = 0;
    char buf[200];
    FILE *fp = fopen(path,"rt");

    if(fp == NULL){
        return -1;
    }

    while (fgets(buf,sizeof(buf),fp) != NULL)
    {
        cont++;
    }

    fclose(fp);
    
    return cont;
}

//Función para obtener los datos de asistencias que no corresponden
int get_incorrect_asists(const t_soc *socios,int cant_soc,const t_asis *asist,int cant_asist,t_inc_asis inc_asist[]){

    int cont = 0;

    for(int i = 0; i < cant_asist; i++){

        for(int j = 0; j < cant_soc; j++){

            if(asist[i].dni == socios[j].dni){
                
                if(strcmp(asist[i].dia,socios[j].dia)){

                    strcpy(inc_asist[cont].nom,socios[j].nom);
                    strcpy(inc_asist[cont].ape,socios[j].ape);
                    inc_asist[cont].dni = socios[j].dni;
                    strcpy(inc_asist[cont].dep,socios[j].dep);
                    strcpy(inc_asist[cont].dia,socios[j].dia);
                    strcpy(inc_asist[cont].dia_inc,asist[i].dia);

                    cont++;
                }

            }

        }

    }

    return cont;
 
}

void borrar_shm(){
    //Elimino los objetos de memoria compartida
        if(remove("/dev/shm/shmPagos") != 0){
            perror("\nError al borrar el objeto de memoria compartida!\n");
            exit(EXIT_FAILURE);
        }
            if(remove("/dev/shm/shmAsist") != 0){
            perror("\nError al borrar el objeto de memoria compartida!\n");
            exit(EXIT_FAILURE);
        }
}

void help(){
    printf("\n######    HELP Ejercicio_3    ######\n");
    printf("\n\nEste programa recibe por parámetro un directorio, en el cual se\n");
    printf("encuentran los archivos socios.txt, pagos.txt y asistencias.txt del club\n");
    printf("y se encarga de analizarlos para obtener el total recaudado en el mes,\n");
    printf("qué socios no han pagado su cuota y quiénes han asistido en día incorrecto.\n\n");
    printf("El programa recibe un único parámetro el cuál es el directorio donde se\n");
    printf("encuentran los tres archivos, y debe ejecutarse de una de las siguientes maneras\n\n");
    printf("--> ./Ejercicio_3 directorio\n\n--> ./Ejercicio_3 -p directorio\n\n--> ./Ejercicio_3 --path directorio\n");
}

int main(int argc, char* argv[]){

    if(argc != 3 && argc != 2){
        printf("\nError! Este programa debe recibir solo UN parámetro. Para más información ejecute ./Ejercicio_3 -h, ./Ejercicio_3 --help o ./Ejercicio_3 -?\n");
        exit(1);
    }

    if( !strcmp("-h", argv[1]) || !strcmp("-?", argv[1]) || !strcmp("--help", argv[1]) ){
        help();
        return EXIT_SUCCESS;
    }

    char path[100];

    if(argc == 3){
        if(strcmp(argv[1],"-p") && strcmp(argv[1],"--path")){
            printf("\nError de parámetros! Para más información ejecute ./Ejercicio_3 -h, ./Ejercicio_3 --help o ./Ejercicio_3 -?\n");
            exit(1);
        }

        strcpy(path,argv[2]);
    }
    else{
        strcpy(path,argv[1]);
    }

    char path_soc[100],path_pagos[100],path_asist[100];
    strcat(strcpy(path_soc,path), "/socios.txt"); 
    strcat(strcpy(path_pagos,path), "/pagos.txt"); 
    strcat(strcpy(path_asist,path), "/asistencia.txt"); 
    
    //Declaro punteros a semáforos
    sem_t *psem_pagos,*psem_asist;

    //Creo dos semáforos, uno para el proceso de pagos y otro para el proceso de asistencias
    //Ambos inicializados en 0
    psem_pagos = sem_open(SEM_PAGOS, O_CREAT, 00700, 0);
    psem_asist = sem_open(SEM_ASIST, O_CREAT, 00700, 0);

    //Cierro los semáforos ya que no se usan en el proceso padre
    sem_close(psem_pagos);
    sem_close(psem_asist);

    pid_t pid;

    //Utilizo un for de 0 a 2 para crear los 3 procesos necesarios con fork
    for(int i = 0; i < 3; i++){

        //Error de fork
        if((pid = fork()) == -1){
            perror("\nError al crear el nuevo proceso!!\n");
            exit(1);
        }
        //Es proceso hijo
        else if(pid == 0){

            //Proceso productor para archivo de pagos
            if(i == 0){ 

                //****LECTURA ARCHIVO DE PAGOS****

                //Calculo la cantidad de registros en el archivo de pagos
                int cant_reg = contar_registros_archivo(path_pagos);
                char buf[200];

                if(cant_reg == -1){
                    cant_reg = 0;

                    t_pago pagos[0];
                }

                //Creo un array de t_pago con espacio para todos los registros del archivo
                t_pago pagos[cant_reg];

                if(cant_reg != 0){
                    //Abro el archivo de pagos en modo lectura de texto
                    FILE *fp = fopen(path_pagos,"rt");
                    if(fp == NULL){
                        perror("\nError al abrir el archivo de pagos!!\n");
                        exit(1);
                    }

                    //Leo todo el archivo de pagos y cargo los datos en el array de t_pago
                    int j = 0;
                    while(fgets(buf,sizeof(buf),fp) != NULL)
                    {
                        text_to_pago(buf,&pagos[j]);
                        j++;
                    }

                    //Cierro el archivo de pagos
                    fclose(fp);
                }

                //****TRABAJO CON MEMORIA COMPARTIDA****

                int fd;
                void *ptr;

                //Creo un objeto de memoria compartida (flag O_CREAT) y lo habilito para lectura escritura (flag O_RDWR)
                fd = shm_open(SHM_PAGOS, O_CREAT | O_RDWR, 00700);

                //Error al crear el objeto de memoria compartida
                if(fd == -1){
                    printf("\nError al crear el File Descriptor!!\n");
                    exit(1);
                }

                //Le doy un tamaño a la memoria compartida
                //En este caso el de un entero para guardar la cantidad de registros de pagos
                // más el tamaño del array de pagos
                if(ftruncate(fd, sizeof(int) + sizeof(pagos)) == -1){
                    printf("\nError al configurar el tamaño de la memoria compartida!!\n");
                    exit(1);
                }

                //Mapeo el objeto de memoria compartida en nuestra memoria virtual del programa para poder trabajar con ella
                ptr = mmap(NULL,sizeof(int) + sizeof(pagos), PROT_WRITE, MAP_SHARED, fd, 0);
                if(ptr == MAP_FAILED){
                    printf("\nMapeo falló en el proceso de escritura!!\n");
                    exit(1);
                }

                //Copio al espacio de memoria compartida el entero con la cantidad de pagos
                memcpy(ptr,&cant_reg,sizeof(int));
                //Copio al espacio de memoria compartida el array de pagos
                memcpy(ptr + sizeof(int), pagos, sizeof(pagos));

                //Cierro el acceso al objeto de memoria
                close(fd);

                //Abro el semáforo de pagos
                psem_pagos = sem_open(SEM_PAGOS, O_CREAT);
                //Incremento el semáforo
                sem_post(psem_pagos);
                //Cierro el semáforo
                sem_close(psem_pagos);

            }

            //Proceso productor para archivo de asistencias
            if(i == 1){  

                //****LECTURA ARCHIVO DE ASISTENCIAS****

                //Calculo la cantidad de registros en el archivo de asistencias
                int cant_reg = contar_registros_archivo(path_asist);

                if(cant_reg == -1){
                    cant_reg = 0;
                }

                    //Creo un array de t_asis con espacio para todos los registros del archivo
                    t_asis asists[cant_reg];

                if(cant_reg != 0){
                    //Abro el archivo de asistencias en modo lectura de texto
                    FILE *fp = fopen(path_asist,"rt");
                    if(fp == NULL){
                        perror("\nError al abrir el archivo de asistencias!!\n");
                        exit(1);
                    }

                    char buf[200];

                    //Leo todo el archivo de asistencias y cargo los datos en el array de t_asis
                    int j = 0;
                    while(fgets(buf,sizeof(buf),fp) != NULL)
                    {
                        text_to_asist(buf,&asists[j]);
                        j++;
                    }

                    //Cierro el archivo de asistencias
                    fclose(fp);
                }

                //****TRABAJO CON MEMORIA COMPARTIDA****

                int fd;
                void *ptr;

                //Creo un objeto de memoria compartida (flag O_CREAT) y lo habilito para lectura escritura (flag O_RDWR)
                fd = shm_open(SHM_ASIST, O_CREAT | O_RDWR, 00700);

                //Error al crear el objeto de memoria compartida
                if(fd == -1){
                    printf("\nError al crear el File Descriptor!!\n");
                    exit(1);
                }

                //Le doy un tamaño a la memoria compartida
                //En este caso el de un entero para guardar la cantidad de registros de asistencia
                // más el tamaño del array de asistencias
                if(ftruncate(fd, sizeof(int) + cant_reg * sizeof(t_asis)) == -1){
                    printf("\nError al configurar el tamaño de la memoria compartida!!\n");
                    exit(1);
                }

                //Mapeo el objeto de memoria compartida en nuestra memoria virtual del programa para poder trabajar con ella
                ptr = mmap(NULL,sizeof(int) + cant_reg * sizeof(t_asis), PROT_WRITE, MAP_SHARED, fd, 0);
                if(ptr == MAP_FAILED){
                    printf("\nMapeo falló en el proceso de escritura!!\n");
                    exit(1);
                }

                //Copio al espacio de memoria compartida el entero con la cantidad de asistencias
                memcpy(ptr,&cant_reg,sizeof(cant_reg));
                //Copio al espacio de memoria compartida el array de asistencias
                memcpy(ptr + sizeof(cant_reg), asists, sizeof(asists));

                //Cierro el acceso al objeto de memoria
                close(fd);

                //Abro el semáforo de asistencias
                psem_asist = sem_open(SEM_ASIST, O_CREAT);
                //Incremento el semáforo
                sem_post(psem_asist);
                //Cierro el semáforo
                sem_close(psem_asist);
            }

            //Proceso consumidor
            if(i == 2){ 

                int cant_soc,cant_asist,cant_pagos,reg_size;

                char buf[200];


                //****LECTURA ARCHIVO DE SOCIOS****

                //Calculo la cantidad de registros en el archivo de socios
                cant_soc = contar_registros_archivo(path_soc);
                if(cant_soc == -1){
                    printf("\nError al abrir el archivo de socios!!\n");
                    exit(1);
                }
                else if(cant_soc == 0){
                    printf("\nEl archivo de socios está VACIO!!\n");
                    exit(1);
                }

                //Abro el archivo de socios en modo lectura de texto
                FILE *fp = fopen(path_soc,"rt");
                if(fp == NULL){
                    printf("\nError al abrir el archivo de socios!!\n");
                    exit(1);
                }

                //Creo un array de t_soc con espacio para todos los registros del archivo
                t_soc socios[cant_soc];

                //Leo todo el archivo de socios y cargo los datos en el array de t_soc
                int j = 0;
                while(fgets(buf,sizeof(buf),fp) != NULL)
                {
                    text_to_soc(buf,&socios[j]);
                    j++;
                }

                //Cierro el archivo de socios
                fclose(fp);

                //****TRABAJO CON MEMORIA COMPARTIDA DE PAGOS****

                //Abro los semáforos de pagos y asistencias
                psem_pagos = sem_open(SEM_PAGOS, O_CREAT);
                psem_asist = sem_open(SEM_ASIST, O_CREAT);

                //Intento decrementar el semáforo de pagos
                //Si el valor el semáforo es mayor que cero prosigue 
                // y si es cero el proceso se bloquea hasta que sea incrementado por otro proceso
                sem_wait(psem_pagos);

                int fd;
                void *ptr;
                struct stat shmobj_st;

                //Abro el objeto de memoria compartida de pagos en modalidad de lectura-escritura
                fd = shm_open(SHM_PAGOS, O_RDWR, 00400);

                //Error al abrir el objeto de memoria compartida
                if(fd == -1){
                    printf("\nError al crear el File Descriptor!!\n");
                    exit(1);
                }

                //Cargo en la estructura de tipo stat las especificaciones del objeto de memoria para obtener su tamaño
                if(fstat(fd, &shmobj_st) == -1){
                    printf("\nError del fstat!!\n");
                    exit(1);
                }

                //Mapeo el objeto de memoria compartida en nuestra memoria virtual del programa para poder trabajar con ella
                ptr = mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, fd, 0);
                if(ptr == MAP_FAILED){
                    printf("\nError del mapeo en el proceso de lectura!!\n");
                    exit(1);
                }

                //Copio la cantidad de pagos desde la memoria compartida a una variable del programa
                memcpy(&cant_pagos,ptr,sizeof(int));

                //Creo un array de pagos con espacio para todos los pagos 
                t_pago pagos[cant_pagos];
                //Copio las estructuras de pago desde memoria compartida al array creado
                memcpy(pagos, ptr+sizeof(int), sizeof(pagos));     

                //Cierro el objeto de memoria compartida de pagos
                close(fd);

                //Elimino el semáforo de pagos
                sem_unlink(SEM_PAGOS);

                //****TRABAJO CON MEMORIA COMPARTIDA DE ASISTENCIAS****

                //Intento decrementar el semáforo de asistencias
                //Si el valor el semáforo es mayor que cero prosigue 
                // y si es cero el proceso se bloquea hasta que sea incrementado por otro proceso
                sem_wait(psem_asist);

                //Abro el objeto de memoria compartida de asistencias en modalidad de lectura-escritura
                fd = shm_open(SHM_ASIST, O_RDWR, 00400);

                //Error al abrir el objeto de memoria compartida
                if(fd == -1){
                    printf("\nError al crear el File Descriptor!!\n");
                    exit(1);
                }

                //Cargo en la estructura de tipo stat las especificaciones del objeto de memoria para obtener su tamaño
                if(fstat(fd, &shmobj_st) == -1){
                    printf("\nError del fstat!!\n");
                    exit(1);
                }

                //Mapeo el objeto de memoria compartida en nuestra memoria virtual del programa para poder trabajar con ella
                ptr = mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, fd, 0);
                if(ptr == MAP_FAILED){
                    printf("\nError del mapeo en el proceso de lectura!!\n");
                    exit(1);
                }

                //Copio la cantidad de asistencias desde la memoria compartida a una variable del programa
                memcpy(&cant_asist,ptr,sizeof(int));

                //Creo un array de asistencias con espacio para todas las asistencias
                t_asis asists[cant_asist];
                //Copio las estructuras de asistencia desde memoria compartida al array creado
                memcpy(asists, ptr+sizeof(int), sizeof(asists));     

                //Cierro el objeto de memoria compartida de asistencias
                close(fd);

                //Elimino el semáforo de asistencias
                sem_unlink(SEM_ASIST);

                //Elimino los objetos de memoria compartida
                borrar_shm();

                //****CÁLCULO LOS REQUISITOS DEL PROGRAMA****

                //****MONTO TOTAL COBRADO EN EL MES****

                //Acumulador para el monto cobrado en el mes
                float monto_total = 0;

                //Calculo el importe de cada pago realizado y lo sumo al acumulador
                for(j = 0; j < cant_pagos; j++){
                    monto_total += calcular_importe(&pagos[j],socios,cant_soc);
                }

                //Imprimo en pantalla el monto recaudado
                printf("*****************************************MONTO RECAUDADO ESTE MES*****************************************\n\n");
                printf("\t\t\t\t\t\t%.2f $\n\n",monto_total);
                printf("**********************************************************************************************************\n\n");

                //****SOCIOS QUE NO PAGARON LA CUOTA MENSUAL****

                //Si hay menos pagos registrados que socios los listo
                if(cant_pagos < cant_soc){
                    listar_deudores(cant_soc,cant_pagos,socios,pagos);
                }
                //Sino imprimo que no hay deudores
                else{
                    printf("NO SE REGISTRAN DEUDAS ESTE MES!\n\n");
                }

                //****ASISTENCIAS QUE NO CORRESPONDEN****

                //Creo un array de asistencias incorrectas 
                t_inc_asis inc_asist[cant_soc];

                //Obtengo la cantidad de asistencias incorrectas y populo el array con los datos de las mismas
                int cant_inc_asist = get_incorrect_asists(socios,cant_soc,asists,cant_asist,inc_asist);

                //Si existen asistencias que no corresponden las listo
                if(cant_inc_asist > 0){

                    printf("*************************************ASISTENCIAS QUE NO CORRESPONDEN**************************************\n\n");
                    
                    for(j = 0; j < cant_inc_asist; j++){
                        printf("Socio de nombre %s %s y DNI %d que practica %s asistió un %s en lugar de %s\n",inc_asist[j].nom,inc_asist[j].ape,inc_asist[j].dni,inc_asist[j].dep,inc_asist[j].dia_inc,inc_asist[j].dia);
                    }

                    printf("**********************************************************************************************************\n\n");

                }
                //Sino imprimo que no hay asistencias que no corresponden
                else{
                    printf("NO SE REGISTRAN ASISTENCIAS QUE NO CORRESPONDEN ESTE MES!\n\n");

                }

            }

            //Break para que no se sigan realizando forks dentro de los procesos hijos
            break;

        }

    }

    //En el proceso padre
    if(pid != 0){
        pid_t wpid;
        int status = 0;

        //Espero a que los procesos hijos terminen
        while((wpid = wait(&status)) > 0);
    }

    return 0;
}