#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct{
    char usuario[40];
    char clave[20];
    char rol[2];
    char com[6];
}t_usuario;

//Función que parsea los datos de un string a un t_usuario
void text_to_user(const char *cad, t_usuario *user){
    char usr[40],clv[20],rol[2],com[6];
    int c;
    c = sscanf(cad,"%[^|]|%[^|]|%[^|]|%[^\n]",usr,clv,rol,com);

    strncpy(user->usuario,usr,sizeof(user->usuario));
    strncpy(user->clave,clv,sizeof(user->clave));
    strncpy(user->rol,rol,sizeof(user->rol));
    strncpy(user->com,com,sizeof(user->com));
}

int login_al_sistema(t_usuario *user){

    //Abro el archivo de usuarios en modo lectura de texto
    FILE *fp = fopen("usuarios.txt","rt");
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


/* int main() {

    t_usuario user;
    int intento = 0;
    int ingresa = 0;
    
    do {
        if(intento==0){
            printf("\nINICIO DE SESION\n");
            printf("---------------\n");
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
        
        ingresa=login_al_sistema(&user);
        intento++;

    } while (intento < 3 && ingresa == 0);

    if (ingresa == 1) {
        printf("\nBienvenido al Sistema\n");
        //printf("%s--%s--%s--%s\n",user.usuario,user.clave,user.rol,user.comicion);
        
    } else {
        printf("\nHa sobrepasado el numero maximo de intentos permitidos\n");
    }

    return 0;
} */
