#include <stdio.h>
#include <string.h> /* strcmp */
#include <stdlib.h> 

typedef struct{
    char usuario[80];
    char clave[80];
    char rol[2];
    char comicion[6];
}t_usuario;

//Función que parsea los datos de un string a un t_usuario
void text_to_user(const char *cad,t_usuario *user){
    char usr[80],clv[80],rol[2],com[6];
    int c;
    c = sscanf(cad,"%[^|]|%[^|]|%[^|]|%[^\n]",usr,clv,rol,com);

    strncpy(user->usuario,usr,sizeof(user->usuario));
    strncpy(user->clave,clv,sizeof(user->clave));
    strncpy(user->rol,rol,sizeof(user->rol));
    strncpy(user->comicion,com,sizeof(user->comicion));
}




int Login_al_sistema(t_usuario *user){

    //Abro el archivo de usuarios en modo lectura de texto
    FILE *fp = fopen("usuarios.txt","rt");
    if(fp == NULL){
        perror("\nError al abrir el archivo de pagos!!\n");
        exit(1);
    }
        
    char buf[200];
    t_usuario aux;
    int titulo = 1;

    while(fgets(buf,sizeof(buf),fp) != NULL)
    {   if(!titulo){
            text_to_user(buf,&aux);
            if(!strcmp(aux.usuario,user->usuario) && !strcmp(aux.clave,user->clave) ){
                *user=aux;
                return 1;                
            }
                     
        }
        titulo =0;
    }

    return 0;
}


int main() {

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
        
        ingresa=Login_al_sistema(&user);
        intento++;

    } while (intento < 3 && ingresa == 0);

    if (ingresa == 1) {
        printf("\nBienvenido al Sistema\n");
        //printf("%s--%s--%s--%s\n",user.usuario,user.clave,user.rol,user.comicion);
        
    } else {
        printf("\nHa sobrepasado el numero maximo de intentos permitidos\n");
    }

    return 0;
}
