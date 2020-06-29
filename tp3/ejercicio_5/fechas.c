#include <stdio.h>
#include <stdlib.h>
#include "fechas.h"

int validar (t_fecha *f)
{
    if(f->anio > 1599 && (f->mes > 0 && f->mes < 13) )
        {   if (f->dia > 0 && f->dia <= fmat(f->anio,f->mes))
                return 1;
            else return 0;
        }
    else
        return 0;
}

int fmat(int a, int m)
{
    int mat[2][12]={{31,28,31,30,31,30,31,31,30,31,30,31},{31,29,31,30,31,30,31,31,30,31,30,31}};
    return mat[ES_BISIESTO(a)][m-1];
}
