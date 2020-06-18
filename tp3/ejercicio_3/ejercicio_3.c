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

#define SHM_PAGOS "/shmPagos"
#define SHM_ASIST "/shmAsist"

#define SEM_PAGOS "sem_pagos"
#define SEM_ASIST "sem_asist"

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

void text_to_soc(const char *cad,t_soc *soc){
    char nom[90],ape[90],dep[90],dia[90];
    int c;
    c = sscanf(cad,"%[^;];%[^;];%d;%[^;];%[^\n]",nom,ape,&soc->dni,dep,dia);

    strncpy(soc->nom,nom,sizeof(soc->nom));
    strncpy(soc->ape,ape,sizeof(soc->ape));
    strncpy(soc->dep,dep,sizeof(soc->dep));
    strncpy(soc->dia,dia,sizeof(soc->dia));
}

void text_to_pago(const char *cad, t_pago *pago){
    char fecha[90];
    int c;

    c = sscanf(cad,"%d;%[^\n]",&pago->dni,fecha);
    
    strncpy(pago->fecha,fecha,sizeof(pago->fecha));
}

void text_to_asist(const char *cad, t_asis *asist){
    char dia[90];
    int c;

    c = sscanf(cad,"%d;%[^\n]",&asist->dni,dia);

    strncpy(asist->dia,dia,sizeof(asist->dia));
}

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

    return 2.3;

}

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

int main(){
    
    sem_t *psem_pagos,*psem_asist;

    psem_pagos = sem_open(SEM_PAGOS, O_CREAT, 00700, 0);
    psem_asist = sem_open(SEM_ASIST, O_CREAT, 00700, 0);

    sem_close(psem_pagos);
    sem_close(psem_asist);

    pid_t pid;

    for(int i = 0; i < 3; i++){

        if((pid = fork()) == -1){
            perror("\nError al crear el nuevo proceso!!\n");
            exit(1);
        }
        else if(pid == 0){

            if(i == 0){ //Proceso productor para archivo de pagos

                int cant_reg = contar_registros_archivo("./pagos.txt");

                if(cant_reg == -1){
                    printf("\nError al abrir el archivo de pagos!!\n");
                    exit(1);
                }

                char *line = NULL;
                size_t len = 0;
                ssize_t read;
                char buf[200];
                
                FILE *fp = fopen("./pagos.txt","rt");
                if(fp == NULL){
                    perror("\nError al abrir el archivo de pagos!!\n");
                    exit(1);
                }

                t_pago pagos[cant_reg];
                int j = 0;
                while(fgets(buf,sizeof(buf),fp) != NULL)
                {
                    text_to_pago(buf,&pagos[j]);
                    j++;
                }

                int fd;
                void *ptr;

                fd = shm_open(SHM_PAGOS, O_CREAT | O_RDWR, 00700);
                if(fd == -1){
                    printf("\nError al crear el File Descriptor!!\n");
                    exit(1);
                }

                if(ftruncate(fd, sizeof(int) + cant_reg * sizeof(t_pago)) == -1){
                    printf("\nError al configurar el tamaño de la memoria compartida!!\n");
                    exit(1);
                }

                ptr = mmap(NULL,sizeof(int) + cant_reg * sizeof(t_pago), PROT_WRITE, MAP_SHARED, fd, 0);
                if(ptr == MAP_FAILED){
                    printf("\nMapeo falló en el proceso de escritura!!\n");
                    exit(1);
                }

                memcpy(ptr,&cant_reg,sizeof(int));
                memcpy(ptr + sizeof(int), pagos, sizeof(pagos));

                close(fd);
                fclose(fp);

                psem_pagos = sem_open(SEM_PAGOS, O_CREAT);
                sem_post(psem_pagos);
                sem_close(psem_pagos);

            }

            if(i == 1){  //Proceso productor para archivo de asistencias

                int cant_reg = contar_registros_archivo("./asistencia.txt");

                if(cant_reg == -1){
                    printf("\nError al abrir el archivo de asistencias!!\n");
                    exit(1);
                }

                FILE *fp = fopen("./asistencia.txt","rt");
                if(fp == NULL){
                    perror("\nError al abrir el archivo de asistencias!!\n");
                    exit(1);
                }

                char *line = NULL;
                size_t len = 0;
                ssize_t read;

                t_asis asists[cant_reg];
                int j = 0;
                while((read = getline(&line,&len,fp)) != -1)
                {
                    text_to_asist(line,&asists[j]);
                    j++;
                }

                int fd;
                void *ptr;

                fd = shm_open(SHM_ASIST, O_CREAT | O_RDWR, 00700);
                if(fd == -1){
                    printf("\nError al crear el File Descriptor!!\n");
                    exit(1);
                }

                if(ftruncate(fd, sizeof(int) + cant_reg * sizeof(t_asis)) == -1){
                    printf("\nError al configurar el tamaño de la memoria compartida!!\n");
                    exit(1);
                }

                ptr = mmap(NULL,sizeof(int) + cant_reg * sizeof(t_asis), PROT_WRITE, MAP_SHARED, fd, 0);
                if(ptr == MAP_FAILED){
                    printf("\nMapeo falló en el proceso de escritura!!\n");
                    exit(1);
                }

                memcpy(ptr,&cant_reg,sizeof(cant_reg));
                memcpy(ptr + sizeof(cant_reg), asists, sizeof(asists));


                close(fd);
                fclose(fp);

                psem_asist = sem_open(SEM_ASIST, O_CREAT);
                sem_post(psem_asist);
                sem_close(psem_asist);
            }

            if(i == 2){ //Proceso consumidor

                int cant_soc,cant_asist,cant_pagos,reg_size;
                char *line = NULL;
                size_t len = 0;
                ssize_t read;
                char buf[200];

                psem_pagos = sem_open(SEM_PAGOS, O_CREAT);
                psem_asist = sem_open(SEM_ASIST, O_CREAT);

                sem_wait(psem_pagos);

                int fd;
                void *ptr;
                struct stat shmobj_st;

                fd = shm_open(SHM_PAGOS, O_RDWR, 00400);
                if(fd == -1){
                    printf("\nError al crear el File Descriptor!!\n");
                    exit(1);
                }

                if(fstat(fd, &shmobj_st) == -1){
                    printf("\nError del fstat!!\n");
                    exit(1);
                }

                ptr = mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, fd, 0);
                if(ptr == MAP_FAILED){
                    printf("\nError del mapeo en el proceso de lectura!!\n");
                    exit(1);
                }

                memcpy(&cant_pagos,ptr,sizeof(int));
                t_pago pagos[cant_pagos];
                memcpy(pagos, ptr+sizeof(int), sizeof(pagos));     

                sem_unlink(SEM_PAGOS);

                close(fd);

                fd = shm_open(SHM_ASIST, O_RDWR, 00400);
                if(fd == -1){
                    printf("\nError al crear el File Descriptor!!\n");
                    exit(1);
                }

                if(fstat(fd, &shmobj_st) == -1){
                    printf("\nError del fstat!!\n");
                    exit(1);
                }

                ptr = mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, fd, 0);
                if(ptr == MAP_FAILED){
                    printf("\nError del mapeo en el proceso de lectura!!\n");
                    exit(1);
                }

                memcpy(&cant_asist,ptr,sizeof(int));
                t_asis asists[cant_asist];
                memcpy(asists, ptr+sizeof(int), sizeof(asists));     

                sem_unlink(SEM_ASIST);

                close(fd);

                cant_soc = contar_registros_archivo("./socios.txt");
                if(cant_soc == -1){
                    printf("\nError al abrir el archivo de socios!!\n");
                    exit(1);
                }
                else if(cant_soc == 0){
                    printf("\nEl archivo de socios está VACIO!!\n");
                    exit(1);
                }

                FILE *fp = fopen("./socios.txt","rt");
                if(fp == NULL){
                    printf("\nError al abrir el archivo de socios!!\n");
                    exit(1);
                }

                t_soc socios[cant_soc];
                int j = 0;
                while(fgets(buf,sizeof(buf),fp) != NULL)
                {
                    text_to_soc(buf,&socios[j]);
                    j++;
                }

                fclose(fp);

                float monto_total = 0;

                for(j = 0; j < cant_pagos; j++){
                    monto_total += calcular_importe(&pagos[j],socios,cant_soc);
                }

                printf("*****************************************MONTO RECAUDADO ESTE MES*****************************************\n\n");
                printf("\t\t\t\t\t\t%.2f $\n\n",monto_total);
                printf("**********************************************************************************************************\n\n");

                if(cant_pagos < cant_soc){
                    listar_deudores(cant_soc,cant_pagos,socios,pagos);
                }
                else{
                    printf("NO SE REGISTRAN DEUDAS ESTE MES!\n\n");
                }

                t_inc_asis inc_asist[cant_soc];
                int cant_inc_asist = get_incorrect_asists(socios,cant_soc,asists,cant_asist,inc_asist);

                if(cant_inc_asist > 0){

                    printf("*************************************ASISTENCIAS QUE NO CORRESPONDEN**************************************\n\n");
                    
                    for(j = 0; j < cant_inc_asist; j++){
                        printf("Socio de nombre %s %s y DNI %d que practica %s asistió un %s en lugar de %s\n",inc_asist[j].nom,inc_asist[j].ape,inc_asist[j].dni,inc_asist[j].dep,inc_asist[j].dia_inc,inc_asist[j].dia);
                    }

                    printf("**********************************************************************************************************\n\n");

                }
                else{
                    printf("NO SE REGISTRAN ASISTENCIAS QUE NO CORRESPONDEN ESTE MES!\n\n");

                }

            }

            break;

        }

    }

    if(pid != 0){
        pid_t wpid;
        int status = 0;
        while((wpid = wait(&status)) > 0);
        printf("\nTerminado\n");
    }

    return 0;
}