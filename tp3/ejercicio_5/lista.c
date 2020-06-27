#include "lista.h"


void crear_lista(t_lista *l)
{
    *l=NULL;
}

int compare(const t_articulo* r1,const t_articulo* r2)
{
    return r1->item_id-r2->item_id;
}


int alistar_orden(t_lista* l,const t_articulo* d,t_cmp compare)
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
int obtener_de_lista(t_lista* l,t_articulo* d,t_cmp compare)
{

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

void mostrar_lista(t_lista* l)
{
    t_nodo *act=*l;
    while(act->ant){
        act=act->ant;
    }
    while (act)
    {
        printf("%d;%s;%s;%s\n",act->registro.item_id,act->registro.articulo,act->registro.producto,act->registro.marca);
        act=act->sig;
    }
}


void cargar_lista(t_lista* l,char* path)
{
    char linea[200];
    FILE* pf=fopen(path,"rt");
    int ini,fin,i=0;
    if(!pf){
   // printf("No se pudo abrir el archivo a leer\n");
    exit(EXIT_FAILURE);
    }
    else
    {
      //printf("Se abrio el archivo a leer correctamente\n");
    }
    fgets(linea,200,pf);
    while(fgets(linea,200,pf))
    {
        ini=0;
        fin=0;
        char item[50];
        t_articulo reg;
        while(linea[fin] != ';')
            fin++;
        
        strncpy(item,&linea[ini],fin);
        item[fin]='\0';
        reg.item_id=atoi(item);
        fin++;
        ini=fin;
        while(linea[fin] != ';'){
            fin++;
        }
        strncpy(reg.articulo,&linea[ini],fin-ini);
        reg.articulo[fin-ini]='\0';
        fin++;
        ini=fin;
        while(linea[fin] != ';')
            fin++;
        strncpy(reg.producto,&linea[ini],fin-ini);
        reg.producto[fin-ini]='\0';
        fin++;
        ini=fin;
        int noEsLetra=0;
        while(linea[fin] != '\n' )
        {
            fin++;
        }
        strncpy(reg.marca,&linea[ini],fin-1-ini);
        reg.marca[fin-1-ini]='\0';
        
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

int ver_registro(t_lista* l,t_articulo* d)
{
    if(*l!=NULL)
    {
        *d=(*l)->registro;
        return 1;
    }
    return -1;
}

int desalistar(t_lista* l,t_articulo* d,t_cmp compare)
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


t_lista buscar_prod_id(t_lista *l,char* peticion)
{
    t_nodo* act=*l,
            *sig,
            *ant;
    t_articulo d;
    t_lista aux;
    crear_lista(&aux);
    d.item_id=atoi(peticion);
    int cmp;
    if(act)
    {
        while(act->ant && compare(&(act->registro),&d)>0)
            act = act->ant;
        while(act->sig && compare(&(act->registro),&d)<0)
            act = act->sig;
        cmp=compare(&(act->registro),&d);
        if(cmp!=0)
            return aux;
        else
        {
            strcpy(d.articulo,act->registro.articulo);
            strcpy(d.marca,act->registro.marca);
            strcpy(d.producto,act->registro.producto);
            
            strcat(d.articulo,"\0");
            strcat(d.marca,"\0");
            strcat(d.producto,"\0");
        
            alistar_orden(&aux,&d,compare);
            return aux;
        }
    }
}
t_lista buscar_articulos(t_lista *l,char* peticion)
{
    
    int largo=strlen(peticion);
    t_lista aux;
    t_articulo d;
    strcpy(d.articulo,peticion);
    d.articulo[largo-1]='\0';
    crear_lista(&aux);
    t_nodo* act=*l;
    while(act->ant){
        act=act->ant;
    }
    while(act->sig)
    {
        char *cmp =strstr(act->registro.articulo,d.articulo);
        if(cmp!=NULL)
            alistar_orden(&aux,&act->registro,compare);
        act=act->sig;
    }
    return aux;
}
t_lista buscar_productos(t_lista *l,char* peticion)
{
    t_lista aux;
    t_articulo d;
    int largo = strlen(peticion);
    strcpy(d.producto,peticion);
    d.producto[largo-1]='\0';
    crear_lista(&aux);
    t_nodo* act=*l;
    while(act->ant){
        act=act->ant;
    }
    while(act->sig)
    {
        int cmp =strcmp(act->registro.producto,d.producto);
        if(cmp==0)
            alistar_orden(&aux,&act->registro,compare);
        act=act->sig;
    }
    return aux;
}
t_lista buscar_marca(t_lista *l,char* peticion)
{
    t_lista aux;
    t_articulo d;
    int largo=strlen(peticion)-1;
    strcpy(d.marca,peticion);
    d.marca[largo]='\0';
    crear_lista(&aux);
    t_nodo* act=*l;
    while(act->ant){
        act=act->ant;
    }
    while(act->sig)
    {

        int cmp = strncmp(act->registro.marca,d.marca,largo);
        if(cmp==0){
            alistar_orden(&aux,&act->registro,compare);
        }
        act=act->sig;
    }
    return aux;
}