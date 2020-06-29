#ifndef FUNCIONES_H_INCLUDED
#define FUNCIONES_H_INCLUDED

#define ES_BISIESTO(a) ((a)%400==0 || ((a)%100!=0 && (a)%4==0))?1:0

typedef struct{
    int dia,
        mes,
        anio;
}t_fecha;

int validar (t_fecha *);
int fmat(int, int);

#endif // FUNCIONES_H_INCLUDED
