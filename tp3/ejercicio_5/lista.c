#include "lista.h"


void crear_lista(t_lista *l)
{
    *l=NULL;
}

int compare(const t_asist* r1,const t_asist* r2)
{
    return strcmp(r1->usuario,r2->usuario);
}


int alistar_orden(t_lista* l,const t_asist* d,t_cmp compare)
{
    int cmp;
    t_nodo* act=*l,
            *ant,
            *sig,
            *nue;
    if(!act)
    {
        ant=NULL;
        sig=NULL;
    }
    else
    {
        while (act->ant && compare(&act->registro,d)>0)
            act=act->ant;
        while (act->sig && compare(&act->registro,d)<0)
            act=act->sig;
        cmp = compare(&act->registro,d);
    
        if(cmp>0)
        {
            ant=act->ant;
            sig=act;
        }
        else
        {
            ant=act;
            sig=act->sig;
        }
    }

    nue = (t_nodo*) malloc(sizeof(t_nodo));
    if(!nue)
        return -1;
    nue->registro=*d;
    nue->ant=ant;
    nue->sig=sig;
    if(sig)
        sig->ant=nue;
    if(ant)
        ant->sig=nue;
    *l=nue;
    return 1;
}

void poner_al_inicio(t_lista*  l)
{
    if(!(*l))
    {
        perror("No se puede poner al inicio la lista,esta vacia \n");
        return;
    }
    while((*l)->ant)
        *l=(*l)->ant;
}

// void mostrar_lista(t_lista* l)
// {
//     t_nodo *act=*l;
//     while(act->ant){
//         act=act->ant;
//     }
//     while (act)
//     {
//         printf("%d;%s;%s;%s\n",act->registro.item_id,act->registro.articulo,act->registro.producto,act->registro.marca);
//         act=act->sig;
//     }
// }

void text_to_asist(const char *cad, t_asist *asist){
    char usr[40];

    sscanf(cad,"%[^|]|%c",usr,&asist->asist);

    strncpy(asist->usuario,usr,sizeof(asist->usuario));
}

void cargar_lista(t_lista* l,char* path)
{
    char linea[200];
    FILE* pf=fopen(path,"rt");
    if(!pf){
    // printf("No se pudo abrir el archivo a leer\n");
        *l = NULL;
        return;
    }
    else
    {
      //printf("Se abrio el archivo a leer correctamente\n");
    }

    t_asist reg;

    fgets(linea,200,pf);
    while(fgets(linea,200,pf))
    {
        text_to_asist(linea,&reg);
        
        alistar_orden(l,&reg,compare);
    }

    fclose(pf);
}

void vaciar_lista(t_lista* l)
{
    if(*l==NULL)
        return;
    poner_al_inicio(l);
    t_nodo * elim = *l;
    while ( (*l) != NULL)
    {   
        *l=elim->sig;
        free(elim);
        elim=*l;
    }
}

int ver_registro(t_lista* l,t_asist* d)
{
    if(*l!=NULL)
    {
        *d=(*l)->registro;
        return 1;
    }
    return -1;
}

int desalistar(t_lista* l,t_asist* d,t_cmp compare)
{
    t_nodo* act=*l,
            *sig,
            *ant;
    int cmp;
    if(act)
    {
        while(act->ant && compare(&act->registro,d)>0)
            act = act->ant;
        while(act->sig && compare(&act->registro,d)<0)
            act = act->sig;
        cmp=compare(&act->registro,d);
        if(cmp!=0)
            return -1;
        else
        {
                ant=act->ant;
                sig=act->sig;
            if(ant)
                ant->sig=sig;
            if(sig)
                sig->ant=ant;
            free(act);
            if(sig)
                *l=sig;
            else
            {
                if(ant)
                    *l=ant;
                else
                {
                    *l=NULL;
                    return -2;
                }
            }

        }
    }
    else
        return -2;
    return 1;
}

int lista_vacia(t_lista* l)
{
    if(!(*l))
        return -1;
    return 1;
}

void cargar_lista_usuarios(t_lista* l, const char *path, const char* com){

    char linea[200];
    FILE* pf=fopen(path,"rt");
    if(!pf){
        *l = NULL;
        return;
    }

    t_usuario reg;
    t_asist asist;

    fgets(linea,200,pf);
    while(fgets(linea,200,pf))
    {
        text_to_user(linea,&reg);

        if(reg.rol == 'A'){
            if(strcmp(com,reg.com) == 0){

                strcpy(asist.usuario,reg.usuario);
                
                alistar_orden(l,&asist,compare);
            }
        }

    }

    fclose(pf);
}


//Función que parsea los datos de un string a un t_usuario
void text_to_user(const char *cad, t_usuario *user){
    char usr[40],clv[20],com[6];
    int c;
    c = sscanf(cad,"%[^|]|%[^|]|%c|%[^\n]",usr,clv,&user->rol,com);

    strncpy(user->usuario,usr,sizeof(user->usuario));
    strncpy(user->clave,clv,sizeof(user->clave));
    strncpy(user->com,com,sizeof(user->com));
}