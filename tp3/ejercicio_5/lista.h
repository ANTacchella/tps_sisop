#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct{
    char usuario[40];
    char asist;
}t_asist;

typedef struct{
    char usuario[40];
    char clave[20];
    char rol;
    char com[6];
}t_usuario;

typedef struct sNodo{

    t_asist registro;
    struct sNodo* ant;
    struct sNodo* sig;
}t_nodo;

typedef t_nodo* t_lista;

int compare(const t_asist*,const t_asist*);
typedef int(*t_cmp)(const t_asist*,const t_asist*);

void crear_lista(t_lista *);
int alistar_orden(t_lista*,const t_asist*,t_cmp);
int obtener_de_lista(t_lista*,t_asist*,t_cmp);
// void mostrar_lista(t_lista*);
void cargar_lista(t_lista* l,char* path);
void vaciar_lista(t_lista* );
int lista_vacia(t_lista* l);
int ver_registro(t_lista* l,t_asist* d);
int desalistar(t_lista* l,t_asist* d,t_cmp compare);
void cargar_lista_usuarios(t_lista* l, const char *path, const char* com);
void text_to_asist(const char*,t_asist *);
void text_to_user(const char*, t_usuario*);