#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct{
    int item_id;
    char articulo[100];
    char producto[50];
    char marca[50];
}t_articulo;

typedef struct sNodo{

    t_articulo registro;
    struct sNodo* ant;
    struct sNodo* sig;
}t_nodo;

typedef t_nodo* t_lista;

int compare(const t_articulo*,const t_articulo*);
typedef int(*t_cmp)(const t_articulo*,const t_articulo*);

void crear_lista(t_lista *);
int alistar_orden(t_lista*,const t_articulo*,t_cmp);
int obtener_de_lista(t_lista*,t_articulo*,t_cmp);
void mostrar_lista(t_lista*);
void cargar_lista(t_lista* l,char* path);
void vaciar_lista(t_lista* );
int lista_vacia(t_lista* l);
int ver_registro(t_lista* l,t_articulo* d);
int desalistar(t_lista* l,t_articulo* d,t_cmp compare);


t_lista buscar_prod_id(t_lista *lista,char* peticion);
t_lista buscar_articulos(t_lista *lista,char* peticion);
t_lista buscar_productos(t_lista *lista,char* peticion);
t_lista buscar_marca(t_lista *lista,char* peticion);